#!/usr/bin/env python
from pygls.server import LanguageServer
import os
import urllib
import urllib.request
import shutil
import tempfile
import configparser
import subprocess
import re
import xml.etree.ElementTree as ET
import asyncio
from typing import Dict
from autoCompleter import AutoCompleter, is_function_call, lowest_function
from lsprotocol import types


BUILD_CONFIG_FILE = '.intensproj'
LOCALS_PREFIX = '@LocalVariablesOf'
XML_ITEM_QUERY = "./ITEM"
URL_ROOT = 'file:///' if os.name == 'nt' else 'file://'
NO_SYMBOLS_ERROR = "Can't load symbols for file. (Parser error, build failure, or outside of workspace)"

server = LanguageServer('intens-lsp', '0.2.0')

build_dirs = dict()
workspace_symbols = dict()
unimplemented_funcs = []
build_queue: Dict[str, asyncio.Lock] = dict()
auto_completer = AutoCompleter(server)


def get_folder_for_file(folder_uris: list[str], file_uri: str):
    found = [f for f in folder_uris if file_uri.startswith(f)]
    if len(found) == 0:
        return None
    return found[0]


@server.feature(
    types.INITIALIZE
)
def initialize(params: types.InitializeParams):
    return types.InitializeResult(
        capabilities={
            'text_document_sync': types.TextDocumentSyncKind.Incremental,
            'completion_provider': {
                'resolve_provider': True
            }
        }
    )


def parse_unimplemented_functions(stderr: str):
    """
        intens outputs the information about inimplemented functions in the style of:
        Warning: {name} not implemented!

        This will scan all of stderr and extract the function names from it
    """
    return [line.split(' ')[1]
            for line in stderr.split('\n')
            if 'not implemented!' in line]


@server.feature(
    types.INITIALIZED
)
async def initialized(params):
    for k, folder in server.workspace.folders.items():
        build_queue[folder.uri] = asyncio.Lock()
        async with build_queue[folder.uri]:
            await build_and_validate(folder)


async def build_and_validate(folder: types.WorkspaceFolder):
    """
        build a intens description file in a tmp folder and get information from it
    """
    global unimplemented_funcs, build_dirs, workspace_symbols
    unimplemented_funcs = []
    server_configuration = await server.get_configuration_async(
        types.WorkspaceConfigurationParams(
            items=[
                types.ConfigurationItem(
                    scope_uri=folder.uri, section='intens'),
            ]
        )
    )
    configs = {}
    if len(server_configuration) > 0 and server_configuration[0] is not None:
        configs = {key: val for key,
                   val in server_configuration[0].items() if val is not None}

    # get the root directory of the project
    # on windows the path gets returned weirdly, so we strip the first character away on there
    folder_path = urllib.parse.unquote(
        urllib.request.url2pathname(urllib.parse.urlparse(folder.uri).path)
    )[(1 if os.name == 'nt' else 0):]

    venv_dir = configs.get('venv', '${workspaceFolder}/.venv').replace(
        '${workspaceFolder}', folder_path)
    intens_dir = configs.get('installation', '/usr/local/intens/current')
    msys_dir = configs.get('msys', 'C:\\msys64')
    # add msys and intens installation folders to the PATH on windows
    sys_path = os.pathsep.join([
        os.path.join(intens_dir, 'bin'),
        os.path.join(venv_dir, 'Scripts'),
        os.getenv('PATH'),
        os.path.join(msys_dir, 'usr', 'bin'),
        os.path.join(msys_dir, 'mingw64', 'bin'),
    ]) if os.name == 'nt' else os.pathsep.join([
        os.path.join(venv_dir, 'bin'),
        os.getenv('PATH'),
    ])

    env = {}
    env.update(os.environ)
    env['PATH'] = sys_path
    lp_key = 'LD_LIBRARY_PATH'  # for GNU/Linux and *BSD.
    lp_orig = env.get(lp_key + '_ORIG')
    if lp_orig is not None:
        env[lp_key] = lp_orig  # restore the original, unmodified value
    else:
        # This happens when LD_LIBRARY_PATH was not set.
        # Remove the env var as a last resort:
        env.pop(lp_key, None)

    config_file = server.workspace.get_text_document(
        os.path.join(folder_path, BUILD_CONFIG_FILE))

    # get the configuration from the project's config file
    # abort if it's not found
    config = configparser.ConfigParser(inline_comment_prefixes='#')
    try:
        # configparser does not automatically put unscoped variables into the root
        # so we prefix the root scope
        f = '[root]\n' + config_file.source
    except:

        # currently also check .lspconfig file too until all is migrated
        config_file = server.workspace.get_text_document(
            os.path.join(folder_path, '.lspconfig'))

        # get the configuration from the project's config file
        # abort if it's not found
        config = configparser.ConfigParser(inline_comment_prefixes='#')
        try:
            # configparser does not automatically put unscoped variables into the root
            # so we prefix the root scope
            f = '[root]\n' + config_file.source
        except:
            server.show_message('File ' + BUILD_CONFIG_FILE +
                                ' does not exist in project', types.MessageType.Error)
            return
    config.read_string(f)

    # create a temporary directory and configure cmake in it
    # if already built before, reuse directory
    build_dir = build_dirs[folder_path] if folder_path in build_dirs else tempfile.mkdtemp(
    )
    build_dirs[folder_path] = build_dir

    cmake = shutil.which("cmake", path=sys_path)
    if not cmake:
        server.show_message(
            "lsp error: cmake command not found.", types.MessageType.Error)
        return

    cmake_proc = subprocess.Popen(
        [cmake, folder_path] + (['-G', 'MSYS Makefiles']
                                if os.name == 'nt' else []),
        cwd=build_dir,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env=env
    )

    out, err = cmake_proc.communicate()

    if cmake_proc.returncode != 0:
        server.show_message_log(out.decode('utf-8'), types.MessageType.Error)
        server.show_message_log(err.decode('utf-8'), types.MessageType.Error)
        server.show_message(
            "Cmake error. Please check server logs", types.MessageType.Error)
        return

    # build the description file based on the target specified in .lspconfig
    target = config.get('cmake', 'target')

    build_exec = shutil.which("make", path=sys_path)
    make_process = subprocess.Popen(
        [build_exec, target],
        cwd=build_dir,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env=env
    )

    out, err = make_process.communicate()

    # analyze the generated description file with intens
    intens_cmd = shutil.which("intens", path=sys_path)
    intens_process = subprocess.Popen(
        [intens_cmd, '--lspWorker', config.get('root', 'output')],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        cwd=build_dir,
        env=env
    )

    # if the make already failed we want to analyze that output instead
    if make_process.returncode == 0:
        out, err = intens_process.communicate()

    out = out.decode('utf-8')
    err = err.decode('utf-8')
    # if the build and analysis was successful
    if make_process.returncode == 0 and intens_process.returncode == 0:
        # parse the intens xml output and store the variable/function info
        root = ET.fromstring(out)
        variables = [a for a in root.findall(
            XML_ITEM_QUERY) if 'internal' not in a.attrib and not a.attrib['name'].startswith('__')]
        func_locals = {a.attrib['name']: a for a in root.findall(
            XML_ITEM_QUERY) if a.attrib['name'].startswith(LOCALS_PREFIX)}

        # analysis can still be successful with unimplemented functions
        unimplemented_funcs = parse_unimplemented_functions(
            err)
        workspace_symbols[folder.uri] = {
            'variables': variables,
            'functions': (root.findall('./function') or []) +
            (root.findall('./task') or []),
            'locals': func_locals}
    else:
        # either make failed or intens failed to analyse the file due to a parser error
        server.show_message_log(err, types.MessageType.Warning)

        # if the string "intens aborted" is not in the error output, the build failed elsewhere
        # and analysis of intens symbols or errors cannot be provided
        if "intens aborted" not in err:
            server.show_message("Build error: " + err, types.MessageType.Error)
            return

        # analyze synatx errors from output
        if ': at' in err:
            parts = err.split('\n')[0].split(':')
            # example output:
            # /home/scr/projects/app/etc/pmm/pmm_uiManager.inc:15: at ';': Parser error.
            #
            # after split:
            # [
            #   '/home/scr/projects/app/etc/pmm/pmm_uiManager.inc',
            #   '15',
            #   ' at ";"',
            #   ' Parser error.'
            # ]

            # if the file path has colons, reuinte them
            # the file name is always from the first to the fourth-last part in the list
            # also strip trailing quote from file if present
            error_file_path: str = ':'.join(parts[0:-3]).rstrip('"')

            # Line is one-indexed in the third-last part of the list
            line = int(parts[-3]) - 1
            server.show_message_log(str(parts))

            # second last part contains the erroring symbol in quotes, extract it
            character = re.search('"(.*)"', parts[-2]).group(1)
            file = server.workspace.get_text_document(
                f'{URL_ROOT}{error_file_path}')

            # check where on the line the error happens, as we don't get that info in the message
            index = file.lines[line].find(character)
            server.publish_diagnostics(f'{URL_ROOT}{error_file_path}', [types.Diagnostic(
                range=types.Range(
                    start=types.Position(
                        line=line,
                        character=index
                    ),
                    end=types.Position(
                        line=line,
                        character=index + len(character)
                    )
                ),
                severity=types.DiagnosticSeverity.Error,
                # last part is the error message
                message=parts[-1].strip()
            )])


@server.feature(types.TEXT_DOCUMENT_DID_SAVE)
async def on_save(params: types.DidSaveTextDocumentParams):
    # rebuild on save
    await initialized(None)
    get_diagnostics(params)


@server.feature(types.TEXT_DOCUMENT_DID_OPEN)
async def on_open(params: types.DidOpenTextDocumentParams):
    folder = get_folder_for_file(build_queue.keys(), params.text_document.uri)
    # rebuild the project on opening a file in case the syntax error is in it
    lock = build_queue[folder]
    # Wait for a build to have happened
    async with lock:
        get_diagnostics(params)


def get_diagnostics(params):
    if len(unimplemented_funcs) == 0:
        return
    # we want to see if there are any calls to unimplemented function in this file
    text = server.workspace.get_text_document(params.text_document.uri).source
    matches = [{'name': search[1], 'match': search[0].group(0)} for search in [(re.search(
        f'RUN.?\\(.*{func}.*\\)', text), func) for func in unimplemented_funcs] if search[0] is not None]

    lines = text.splitlines()
    matches_lines = [
        (match['name'], i)
        for match in matches
        for i, line in enumerate(lines)
        if match['match'] in line
    ]

    diagnostics = [types.Diagnostic(
        range=types.Range(
            start=types.Position(
                line=m[1],
                character=lines[m[1]].find(m[0])
            ),
            end=types.Position(
                line=m[1],
                character=lines[m[1]].find(m[0]) + len(m[0])
            )
        ),
        severity=types.DiagnosticSeverity.Warning,
        message=f'Unimplemented function {m[0]}.'
    ) for m in matches_lines]
    server.publish_diagnostics(params.text_document.uri, diagnostics)


@server.feature(
    types.TEXT_DOCUMENT_COMPLETION
)
def completion(params: types.CompletionParams):
    file = server.workspace.get_text_document(params.text_document.uri)

    folder = get_folder_for_file(workspace_symbols.keys(), file.uri)
    if folder is None:
        server.show_message(
            NO_SYMBOLS_ERROR,
            types.MessageType.Error)
        return

    return auto_completer.get_completion(
        params.position, file, workspace_symbols[folder])


def index_to_coordinates(s, index):
    '''Returns (line_number, col) of `index` in `s`.'''
    left = index
    for num, line in enumerate(s.splitlines(True)):
        if len(line) > left:
            break
        left -= len(line)
    return num, left


@server.feature(
    types.TEXT_DOCUMENT_DEFINITION
)
def go_to_definition(params: types.DefinitionParams):
    file = server.workspace.get_text_document(params.text_document.uri)

    # find out if there is a function above us and if yes which one
    in_function = lowest_function(file.lines[0:params.position.line])

    word = file.word_at_position(params.position)

    # Go to definiton across workspaces is not supported
    folder = get_folder_for_file(
        workspace_symbols.keys(), params.text_document.uri)

    if folder is None:
        server.show_message(
            NO_SYMBOLS_ERROR,
            types.MessageType.Error)
        return

    workspace = workspace_symbols[folder]

    variables = workspace['functions'] + workspace['variables']
    # if we have a function above us, we also take its locals into our search
    if in_function != '':
        key = LOCALS_PREFIX + in_function
        if key in workspace['locals']:
            # add function-local variables of the most recent function above you
            variables.extend(workspace['locals'][key].findall(XML_ITEM_QUERY))

    # find the variable we are hovering
    for v in variables:
        if v.attrib['name'] == word and (file := v.attrib['file']) != '':
            # turn a relative file path to an absolute one
            file = URL_ROOT + urllib.parse.quote(file)
            line = v.attrib['line']

            try:
                # get the line where the variable is defined
                # the -1 is because the output we get from intens is 1 indexed for lines
                line_text: str = server.workspace.get_text_document(
                    file).lines[int(line)-1]
            except:
                return
            # find location in line
            character = max(line_text.find(word), 1)
            return types.Location(
                uri=file,
                range=types.Range(
                    start=types.Position(
                        line=int(line) - 1,
                        character=character,
                    ),
                    end=types.Position(
                        line=int(line) - 1,
                        character=character+len(word),
                    )
                )
            )


server.start_io()
