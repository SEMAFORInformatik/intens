#!/usr/bin/env python
#
import argparse
import os
import json
try:
    from mako.template import Template
except:
    print("MAKO NOT INSTALLED. NOT GENERATING HIGHLIGHTS")
    exit(0)

parser = argparse.ArgumentParser(
    prog='GenerateHighlights',
    description='Generate intens highlighting definitions for various')

parser.add_argument('outfile')
parser.add_argument(
    '-f', '--format', choices=['emacs', 'vscode', 'vscode-textmate', 'helix'], required=True)

args = parser.parse_args()

etc_dir = os.path.dirname(os.path.realpath(__file__))

with open(os.path.join(etc_dir, 'highlights.json'), 'r') as f:
    highlights = json.load(f)

template = Template(filename=os.path.join(
    etc_dir, f'ts-template-{args.format}.mako'))

with open(args.outfile, 'w') as f:
    f.write(template.render(**highlights, json=json.dumps(highlights)))
