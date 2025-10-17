#!/usr/bin/env python3
# convert ebnf eps diagrams to latex pictures
#
import logging
import sys
import re
import pathlib

def picture(line):
    matchobj = re.compile(
        r'%%BoundingBox: 0 0 (?P<width>\d+) (?P<height>\d+)'
    ).search(line)
    width = matchobj.group('width')
    height = matchobj.group('height')
    tline = '\\begin{picture}' + '({0},{1})'.format(width, height)
    # picture offset for wide pictures
    endMinipage = ''
    if int(width) > 402:
        # 72(2.54cm) : left border; 595(21cm) : paper width
        offset = 72 - int((595 - int(width)) / 2)
        offset = max(offset, 0)
        pictureOffset = '({0},0)'.format(offset)
        tline = '\\begin{minipage}{' + width + 'pt}\n' + \
            tline + pictureOffset
        endMinipage = '\\end{minipage}\n'
    return endMinipage, tline

def makebox(line, cx, cy):
    matchobj = re.compile(r'\((?P<text>.*)\)').search(line)
    text = re.sub(r'\\261', r'-', matchobj.group('text'))
    text = re.sub(r'\\', r'', text)
    text = re.sub(r'_', r'\\_', text)
    text = re.sub(r'#', r'\\#', text)
    text = re.sub(r'&', r'\\&', text)
    text = re.sub(r'{', r'\\{', text)
    text = re.sub(r'}', r'\\}', text)
    text = re.sub(r'<', r'$<$', text)
    text = re.sub(r'>', r'$>$', text)
    text = re.sub(r'\^', r'\^', text)
    text = re.sub(r'%', r'\\%', text)
    text = re.sub(r'\|', r'$|$', text)
    text = re.sub(r'--', r'-\/-', text)
    return f'  \\put({cx}, {cy})' + \
        '{\\makebox(0,0){\\sffamily\\bfseries\\footnotesize ' + \
        text + \
        '}}\n'

def oval(line):
    matchobj = re.compile(
        r'(?P<llx>[0-9.]+) (?P<lly>[0-9.]+) (?P<width>[0-9.]+) '
        r'(?P<height>[0-9.]+) 1 255 255 255 RBox$'
    ).search(line)
    cx = float(matchobj.group('llx')) + float(matchobj.group('width')) / 2
    cy = float(matchobj.group('lly')) + float(matchobj.group('height')) / 2
    new_line = f'  \\put({cx},{cy})' + \
        '{\\thicklines \\oval[5pt](' + \
        matchobj.group('width') + \
        ',' + \
        matchobj.group('height') + \
        ')}\n'
    return cx, cy, new_line

def framebox(line, llx, lly, width, height):
    matchobj = re.compile(r'\((?P<text>.*)\)').search(line)
    text = re.sub(r'_', r'\\_', matchobj.group('text'))
    ref = re.sub(r'_', r'', matchobj.group('text'))
    return f'  \\put({llx},{lly})' + \
        '{\\thicklines \\framebox(' + \
        str(width) + \
        ',' + \
        str(height) + \
        '){\\sffamily\\footnotesize \\hyperref[dia:' + \
        ref + \
        ']{' + \
        text + \
        '}}}\n'

def draw_line(line):
    matchobj = re.compile(
        r'n (?P<x0>[0-9.]+) (?P<y0>[0-9.]+) m (?P<rx1>[-0-9.]+) '
        r'(?P<ry1>[-0-9.]+) rl (?P<rx2>[-0-9.]+) (?P<ry2>[-0-9.]+) rl s$'
    ).search(line)
    x0 = float(matchobj.group('x0'))
    y0 = float(matchobj.group('y0'))
    rx1 = float(matchobj.group('rx1'))
    ry1 = float(matchobj.group('ry1'))
    rx2 = float(matchobj.group('rx2'))
    ry2 = float(matchobj.group('ry2'))
    # adjust arrows
    if rx1 == 2.5 and rx2 == -3.5 and ry1 == -3.5 and ry2 == -3.5:
        # right arrow
        x0 -= 1
        y0 += 1
        rx1 += 1
    elif rx1 == -3.5 and rx2 == 2.5 and ry1 == -3.5 and ry2 == -3.5:
        # left arrow
        x0 += 1
        y0 += 1
        rx2 += 1
    else:
        logging.warning("UNEXPECTED ARROW in file: %s, line = %s",
                        eps_file, line)

    x1 = x0 + rx1
    y1 = y0 + ry1
    x2 = x1 + rx2
    y2 = y1 + ry2

    # \put(){\line would be nicer but cannot be used
    # because the minimal length would have to be 10pt}
    return '  \\drawline(' + \
        str(x0) + \
        ',' + \
        str(y0) + \
        ')(' + \
        str(x1) + \
        ',' + \
        str(y1) + \
        ')(' + \
        str(x2) + \
        ',' + \
        str(y2) + \
        ')\n'

def put_line(line):
    matchobj = re.compile(
        r'n (?P<x0>[0-9.]+) (?P<y0>[0-9.]+) m (?P<rx1>[-0-9.]+) '
        r'(?P<ry1>[-0-9.]+) rl s$'
    ).search(line)
    x0 = float(matchobj.group('x0'))
    y0 = float(matchobj.group('y0'))
    rx1 = float(matchobj.group('rx1'))
    ry1 = float(matchobj.group('ry1'))

    if rx1 != 0:
        y1 = 0
        l = rx1
        x1 = 1
    elif ry1 != 0:
        x1 = 0
        l = ry1
        y1 = 1
    else:
        return ''
    return '  \\put(' + \
        str(x0) + \
        ',' + \
        str(y0) + \
        '){\\line(' + \
        str(x1) + \
        ',' + \
        str(y1) + \
        '){' + \
        str(l) + \
        '}}\n'

def put_oval(line):
    matchobj = re.compile(
        r'n (?P<x0>[0-9.]+) (?P<y0>[0-9.]+) m (?P<x1>[-0-9.]+) '
        r'(?P<y1>[-0-9.]+) (?P<x2>[-0-9.]+) (?P<y2>[-0-9.]+) '
        r'(?P<r>[0-9.]+) apr$'
    ).search(line)
    x0 = float(matchobj.group('x0'))
    y0 = float(matchobj.group('y0'))
    x1 = float(matchobj.group('x1'))
    y1 = float(matchobj.group('y1'))
    x2 = float(matchobj.group('x2'))
    y2 = float(matchobj.group('y2'))
    d = float(matchobj.group('r')) * 2
    if x0 == x1:  # top
        cx = x2
        cy = y0
        if x2 > x1:  # top left
            part = 'tl'
        else:  # top right
            part = 'tr'
    else:  # bottom
        cx = x0
        cy = y2
        if x1 < x0:  # bottom left
            part = 'bl'
        else:  # bottom right
            part = 'br'
    return cx, cy, '  \\put(' + \
        str(cx) + \
        ',' + \
        str(cy) + \
        '){\\oval(' + \
        str(d) + \
        ',' + \
        str(d) + \
        ')[' + \
        part + \
        ']}\n'

def put_text(line):
    matchobj = re.compile(
        r'(?P<llx>[0-9.]+) (?P<lly>[0-9.]+) m\((?P<text>.*)\) show$'
    ).search(line)
    text = re.sub(r'_', r'\\_', matchobj.group('text'))
    label = re.sub(r'_', r'', matchobj.group('text'))
    return label, '  \\put(' + \
        matchobj.group('llx') + \
        ',' + \
        matchobj.group('lly') + \
        '){\\textcolor{blue}{\\sffamily\\bfseries\\large ' + \
        text + \
        '}}\n'

def convert(epsfilename):
    # write tex header
    eps = pathlib.Path(epsfilename)
    tex = eps.with_suffix('.tex')
    name = re.sub(r'_', r'\\_', eps.stem)
    label = ''
    with eps.open(mode='r') as eps_file:
        with tex.open(mode='w') as tex_file:
            tex_file.write("\\paragraph[" + name + "]{} \\hfill \\\\\n")
            tex_file.write("\\cornersize{1}\n")

            prolog = 1
            rbox = 0
            box = 0
            endMinipage = ''

            for line in eps_file:
                # prolog
                if prolog == 1:
                    if re.compile(r'BoundingBox').search(line):
                        endMinipage, tline = picture(line)
                        logging.debug("picture %s", tline)
                        tex_file.write(tline+'\n')
                    # Begin
                    elif re.compile(r'\$Ebnf2psBegin').match(line):
                        prolog = 0

                # RBOX
                elif rbox == 1 and re.compile(r'show').search(line):
                    rbox = 0
                    tline = makebox(line, cx, cy)
                    logging.debug("makebox %s", tline)
                    tex_file.write(tline)
                elif re.compile(r' RBox').search(line):
                    rbox = 1
                    cx, cy, tline = oval(line)
                    logging.debug("oval %s", tline)
                    tex_file.write(tline)
                # BOX
                elif box == 1 and re.compile(r'show').search(line):
                    box = 0
                    tline = framebox(line, llx, lly, width, height)
                    logging.debug("framebox %s", tline)
                    tex_file.write(tline)

                elif re.compile(r' Box').search(line):
                    box = 1
                    matchobj = re.compile(
                        r'(?P<llx>[0-9.]+) (?P<lly>[0-9.]+) (?P<width>[0-9.]+) '
                        r'(?P<height>[0-9.]+) 1 255 255 255 Box$'
                    ).search(line)
                    llx = float(matchobj.group('llx'))
                    width = float(matchobj.group('width'))
                    lly = float(matchobj.group('lly'))
                    height = float(matchobj.group('height'))

                # arrow
                elif re.compile(r'rl.*rl s').search(line):
                    tline = draw_line(line)
                    logging.debug("draw_line %s", tline)
                    tex_file.write(tline)

                # line
                elif re.compile(r'rl s').search(line):
                    tline = put_line(line)
                    logging.debug("putline %s", tline)
                    tex_file.write(tline)

                # arc
                elif re.compile(r'apr').search(line):
                    cx, cy, tline = put_oval(line)
                    logging.debug("put_oval %s", tline)
                    tex_file.write(tline)

                # title
                elif re.compile(r'show').search(line):
                    label, tline = put_text(line)
                    logging.debug("put_text %s", tline)
                    tex_file.write(tline)

            # write tex footer
            tex_file.write('  \\label{dia:' + label + '}\n')
            tex_file.write("\\end{picture}\n")
            tex_file.write(endMinipage)
            tex_file.write("\n")

if __name__ == '__main__':
    import sys
    import pathlib
    logging.basicConfig(level=logging.INFO,
                        format='[%(asctime)s] - %(message)s')
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <dir> | <eps file> ..")
        sys.exit(-1)
    if len(sys.argv) == 2:
        p = pathlib.Path(sys.argv[1])
        if p.is_dir():
            files = list(p.glob('*.eps'))
        else:
            files = sys.argv[1:]
    for f in files:
        print(f)
        convert(f)
