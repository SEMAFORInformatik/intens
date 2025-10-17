#!/usr/bin/env python3

#
# build and execute Ebnf2ps command
# to build eps diagrams from parserfile
#
import logging
import pathlib
import re
import subprocess

logging.basicConfig(level=logging.INFO, format='[%(asctime)s] - %(message)s')

def main(parserfile, outdir):
    empty = []  # empty syntax elements
    synonym = {}  # synonyms

    # create outdir if missing
    pathlib.Path(outdir).mkdir(parents=True, exist_ok=True)

    # =========================================================================
    # copy parseryacc.yy and modify some token
    # =========================================================================
    with open(parserfile) as parser:
        with open(f'{outdir}/parseryacc.yy', 'w') as modpar:
            for line in parser:
                replace = [
                    ('tGEQ', '">="'),
                    ('tLEQ', '"<="'),
                    ('tEQL', '"=="'),
                    ('tNEQ', '"!="'),
                    ('tNOT', '"!"'),
                    (r'tOR\b', '"||"'),
                    ('tAND', '"&&"'),
                    ('tINCR', '"++"'),
                    ('tDECR', '"--"'),
                    ('tACCUMULATE', '"+="'),

                    # Token name (remove t)
                    (' t([A-Z])', r' \1'),

                    # old syntax
                    ('_V2_', '_'),

                    # simplify
                    ('DISALLOW job_disallow', 'DISALLOW job_allow'),
                    ('DISABLE job_disable', 'DISABLE job_enable'),

                    # rename items
                    ('REAL_CONSTANT', 'real'),
                    (r'string_const\b', 'string'),
                    ('JUSTCENTER', 'CENTER'),

                    ('db_filter_query_function', 'filter_function'),

                    ('REASON_NEW_CONNECTION', 'REASON_CONNECTION')]

                for (old, new) in replace:
                    line = re.sub(old, new, line)

                # build array of empty syntax elements
                try:
                    empty.append(
                        re.compile(r'\s*([^\s]+).*DOCUMENTATION:EMPTY')
                        .match(line).group(1))
                except AttributeError:
                    pass

                # build hash of synonym elements
                m = re.compile(
                    r'\s*([^\s]+).*DOCUMENTATION:SYNONYM ([a-z_A-Z0-9]+)'
                ).match(line)
                try:
                    synonym[m.group(1)] = m.group(2)
                except AttributeError:
                    pass

                modpar.write(line)

    # ======================================================================
    # delete empty syntax elements, synonym syntax elements
    # ======================================================================
    pathlib.Path(f'{outdir}/parseryacc.yy').rename(
        pathlib.Path(f'{outdir}/parseryacc.yy.temp'))
    with open(f'{outdir}/parseryacc.yy.temp') as parser:
        with open(f'{outdir}/parseryacc.yy', 'w') as modpar:
            for line in parser:
                # delete empty syntax elements
                for token in empty:
                    line = re.sub(r' {}\b'.format(token), '', line)

                # synonym syntax elements
                for token, newToken in synonym.items():
                    line = re.sub(r' {}\b'.format(token),
                                  ' {}'.format(newToken), line)

                modpar.write(line)

    # ======================================================================
    # delete lines marked by DOCUMENTATION:HIDE
    # ======================================================================
    pathlib.Path(f'{outdir}/parseryacc.yy').rename(
        pathlib.Path(f'{outdir}/parseryacc.yy.temp'))
    with open(f'{outdir}/parseryacc.yy.temp') as parser:
        with open(f'{outdir}/parseryacc.yy', 'w') as modpar:
            hide = False
            mode = ''
            for line in parser:
                m = re.compile(r'DOCUMENTATION:HIDE (BEGIN|END|)').search(line)
                try:
                    mode = m.group(1)
                    hide = True
                except AttributeError:
                    pass
                if not hide:
                    modpar.write(line)
                elif mode != 'BEGIN':
                    hide = False
                    mode = ''

    # ======================================================================
    # add additional syntax needed for the documentation
    # ======================================================================
    with open('addendum.yy') as addendum:
        with open(f'{outdir}/parseryacc.yy', 'a') as modpar:
            for line in addendum:
                modpar.write(line)

    # ======================================================================
    # build list of terminals and nonterminals
    # ======================================================================
    diagram = []
    unfold = []
    with open(f'{outdir}/parseryacc.yy') as modpar:
        for line in modpar:
            m = re.compile(
                r'\s*([^\s]+).*DOCUMENTATION:(DIAGRAM|UNFOLD)').match(line)
            try:
                token = "^{}$".format(m.group(1))
                mode = m.group(2)
                if mode == 'DIAGRAM':
                    diagram.append(token)
                elif mode == 'UNFOLD':
                    unfold.append(token)
            except AttributeError:
                pass

    # ======================================================================
    # build list of undocumented items:
    # ======================================================================
    # system ('grep "^[a-z]" parseryacc.yy | grep -v DOCUMENTATION: | grep -v _V1_ > undocumented');
    # exit(1);

    # ======================================================================
    # build eps diagrams
    # ======================================================================
    subprocess.call(['ebnf2ps',
                     '-titleFont', 'Helvetica-Bold',
                     '-titleScale', '10',
                     '-titleColor', 'RoyalBlue',
                     '-ntFont', 'Helvetica',
                     '-ntScale', '8',
                     # '-ntBoxColor', 'RoyalBlue',
                     # '-tColor', 'White',
                     # '-tBg', 'RoyalBlue',
                     '-tFont', 'Helvetica-Bold',
                     '-tScale', '8',
                     '-arrowSize', '250',
                     '+unfold',
                     '+simplify',
                     '-yacc',
                     'parseryacc.yy']
                    + diagram + ['--'] + unfold, cwd=outdir)

    # ======================================================================
    # remove temporary files
    # ======================================================================
    pathlib.Path(f'{outdir}/parseryacc.yy').unlink()
    pathlib.Path(f'{outdir}/parseryacc.yy.temp').unlink()


if __name__ == "__main__":
    import sys
    import os
    if os.getenv('RGBPATH') == None:
        os.environ['RGBPATH'] = '/usr/share/X11'
    # invoke with args parseryacc.yy outdir
    main(sys.argv[1], sys.argv[2])
