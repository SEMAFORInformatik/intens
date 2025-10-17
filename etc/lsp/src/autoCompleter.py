from lsprotocol import types
import re
import pygls
import yaml
import os
from typing import List
from tree_sitter import Language, Parser, Point
import xml.etree.ElementTree as ET
from ctypes import cdll, c_void_p


try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader


def lang_from_so(path: str, name: str) -> Language:
    lib = cdll.LoadLibrary(os.fspath(path))
    language_function = getattr(lib, f'tree_sitter_{name}')
    language_function.restype = c_void_p
    language_ptr = language_function()
    return Language(language_ptr)


def is_function_call(line: str, char: int):
    """
        check if our cursor is after a RUN instruction or FUNC variable
    """
    if 'RUN' in line and line.find('(') < char:
        return True

    if 'FUNC' in line and line.find('=') < char:
        return True

    return False


def is_call_constructor(func: str, wanted_commas=0):
    def F(line: str, char: int):
        """
            check if we're in a call to a specified builtin function {func}
            additionally, check if we're in a certain argument of the call with {wanted_commas}
        """
        start_index = line.find('(')
        commas = 0
        # Check if we're inside a call like that in the first place
        if start_index < char and re.search(f'{func}\\s*\\(', line) is not None:
            # count commas before the cursor inside the call
            for i in range(start_index, char + 1):
                if line[i-1] == ',':
                    commas += 1

            return commas == wanted_commas

        return False
    return F


def lowest_function(lines):
    """
        search through lines to get the last function definition
    """
    in_function = ''
    regex = re.compile('FUNC\\s+([a-zA-Z_]+)')
    # Find the most recent FUNC definition above the cursor to get local variables
    for nr, line in enumerate(lines):
        # search for the next identifier after FUNC
        # searching across multiple lines because the
        # name of the function doesn't have to be the same line as FUNC
        if line.startswith('FUNC'):
            rest = '\n'.join(lines[nr:-1])
            result = regex.search(rest)
            if result is not None:
                in_function = result.group(1)

    return in_function


# check different types of builtins we could be calling and which arg we are at
call_types = {
    'SAVE_ARG0': is_call_constructor('SAVE'),
    'UNSET_ARG0': is_call_constructor('UNSET'),
    'SET_ARG0': is_call_constructor('SET'),
    'LOG_ARG0': is_call_constructor('LOG'),
    'MAP_ARG0': is_call_constructor('MAP'),
    'UNMAP_ARG0': is_call_constructor('UNMAP'),
}


def extract_triggers(completions: dict, type: str):
    """
        turn :
        "a": {
            [type]: [
                "trig_a",
                "trig_b"
            ]
        },
        "b": {
            [type]: [
                "trig_e",
                "trig_f"
            ]
        }

        into:
        "trig_a": "a",
        "trig_b": "a",
        "trig_e": "b",
        "trig_f": "b"
    """
    return {trigger: name for name, val in completions.items() if type in val for trigger in val[type]}


def get_word_to_left_of_cursor(line: str, cursor_position: int) -> str or None:
    # strip brackets from the line and add a $ to track cursor position
    word_only = re.sub('\\[.*\\]', '', line[:cursor_position] +
                       '$' + line[cursor_position:])
    # get cursor position minus the brackets
    index = word_only.find('$')
    # remove string after cursor
    leftover_line = word_only[:index]
    # find word before the last remaining dot
    searchpos = leftover_line.rfind('.') - 1
    if searchpos > -1:
        endpos = searchpos
        while searchpos > 0:
            # stop at first non alphanumeric and non underscore character
            if not leftover_line[searchpos].isalpha() and leftover_line[searchpos] != '_':
                break

            searchpos -= 1

        # extract the word based on our findings
        return leftover_line[searchpos + 1:endpos + 1]

    return None


def create_completion_item(comp):
    return types.CompletionItem(
        label=comp['token'],
        kind=types.CompletionItemKind.Field,
        data=comp['token'],
        #
        # if we got a snippet format, delcare that here
        insert_text_format=types.InsertTextFormat.Snippet
        if 'snippet' in comp
        else types.InsertTextFormat.PlainText,
        insert_text=comp['snippet'] if 'snippet' in comp else None,

        documentation=types.MarkupContent(
            kind=types.MarkupKind.Markdown,
            value=comp['doc'] if comp['doc'] is not None else ''
        ) if 'doc' in comp else None
    )


class AutoCompleter:
    def __init__(self, server: pygls.server.LanguageServer):
        self.server = server
        # first load the .so/.dll file for the grammar
        grammar_name = '/libtree-sitter-intens' + \
            ('.dll' if os.name == 'nt' else '.so')
        folder = os.path.dirname(os.path.realpath(
            __file__))
        lang = lang_from_so(
            folder + grammar_name, 'intens')
        self.parser = Parser(lang)
        # load the completions file and transform the structure for faster lookup
        with open(folder + '/completions.yaml', 'r') as f:
            self.completions = yaml.load(f, Loader=Loader)
            self.token_triggers = extract_triggers(
                self.completions, 'triggers')
            self.regex_triggers = extract_triggers(
                self.completions, 'regex_triggers')

    def get_full_completions(self, item: str):
        # as a fallback, get job_statement completions
        if item not in self.completions:
            item = 'job_statements'
        comps = self.completions[item].copy()
        # get the direct completions of that item
        completions = comps['completions'] if 'completions' in comps else []
        # if that item inherits completions from another item, include those too
        if 'inherits' in comps:
            completions.extend([
                comp
                for parent in comps['inherits']
                for comp in self.completions[parent]['completions']
                if 'completions' in self.completions[parent] and comp is not None
            ])
        comps['completions'] = completions
        return comps

    def get_function_names_completions(self, funcs: list[str]):
        """
            turn a list of function names into a list of completions with them
        """
        return [types.CompletionItem(
            label=f,
            kind=types.CompletionItemKind.Function,
            data=f
        ) for f in funcs]

    def get_variable_completions(self, workspace_symbols, parent=None, locals=None, ui_eles_only=False):
        variables: List[ET.Element] = workspace_symbols['variables']
        completions = variables
        # check if we got any function-local variables to add to the completion
        if not ui_eles_only and locals in workspace_symbols['locals']:
            self.server.show_message_log(str(len(completions)))
            completions = workspace_symbols['locals'][locals].findall(
                './ITEM') + completions
            self.server.show_message_log(str(len(completions)))

        # if we got a parent, we just want to return the completions of said parent
        if parent is not None:
            try:
                struct_type = next(v.attrib['struct']
                                   for v in variables if v.attrib['name'] == parent)
                completions = next(v
                                   for v in variables if v.attrib['name'] == struct_type)
            # If there are no completions for a parent, we return no completions
            except StopIteration:
                return []

        if ui_eles_only:
            completions = [c for c in completions if c.attrib.get('uiele')]

        return [types.CompletionItem(
            label=item.attrib['name'],
            kind=types.CompletionItemKind.Variable,
            data=item.attrib['name']
        ) for item in completions]

    def get_completion(self, pos: types.Position, file: pygls.workspace.TextDocument, workspace_symbols):

        lines = file.lines
        line = lines[pos.line]

        # check if we call a function and then just return with the function list
        if is_function_call(line, pos.character):
            return self.get_function_names_completions([a.attrib['name'] for a in workspace_symbols['functions']])

        regex_trigger = ''
        # check our list of regexes if we are in specific calls
        for name, func in call_types.items():
            if func(line, pos.character):
                regex_trigger = name
                break

        item_type = ''
        # if we are in  regex trigger, get the completions from there
        # otherwise, parse file with tree-sitter and check what type of token we're at
        if len(regex_trigger) > 0:
            item_type = self.regex_triggers[regex_trigger]
        else:
            tree = self.parser.parse(file.source.encode('utf-8'))
            point = Point(pos.line, pos.character)
            node = tree.root_node.named_descendant_for_point_range(
                point, point)
            if node is not None:
                item_type = self.token_triggers.get(node.type, '')

        self.server.show_message_log(item_type)
        # get completions of that item type
        completions = self.get_full_completions(item_type)

        # construct completion items
        items = [
            create_completion_item(comp)
            for comp in completions['completions']
        ]

        # if the completion item tells to give
        # the variables as possible completions too, include them
        #
        if 'give_variables' in completions:
            # check if we possibly got a parent struct we are accessing
            parent = get_word_to_left_of_cursor(line, pos.character)
            # get current function
            in_func = lowest_function(file.lines[0:int(pos.line)])

            ui_eles_only = completions['give_variables'] == 'ui'
            # get completions based on presence of parent and local function
            variables_completions = self.get_variable_completions(
                workspace_symbols, parent, '@LocalVariablesOf' + in_func, ui_eles_only)
            # if we got a parent, return only the variable completions
            if parent is not None:
                return variables_completions

            # add the variable completions before or after the rest, depending on setting
            if completions['give_variables'] == 'before':
                items[:0] = variables_completions
            else:
                items.extend(variables_completions)

        return items
