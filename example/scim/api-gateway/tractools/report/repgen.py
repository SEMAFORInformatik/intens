# -*- coding: utf-8 -*-
"""
  Copyright (c) 2025 Semafor Informatik & Energie AG
"""
import logging
import os
import subprocess
import re
import codecs
import tractools.report.template
import sys

logger = logging.getLogger(__name__)


class Error(Exception):
    pass


class ReportGen(object):
    def __init__(self, workdir, template):
        self.workdir = workdir
        self.template = template


class HtmlReportGenerator(ReportGen):
    def __init__(self, workdir=None):
        if getattr(sys, 'frozen', False):
            tmpldir = os.path.join(
                sys._MEIPASS, 'reporthtml')
            package_name = ''
        else:
            package_name = 'tractools.report'
            tmpldir = 'html'
        super(self.__class__,
              self).__init__(
                  workdir,
                  tractools.report.template.JinjaTemplate(tmpldir,
                                                          package_name))

    def generate(self, templatename, content):
        doc = self.template.render(content, templatename)
        if self.workdir:
            repfile = os.path.join(self.workdir, templatename+'.html')
            with codecs.open(repfile, 'w',
                             encoding='utf-8') as output:
                output.write(doc)

            logger.info('DONE')
            return repfile
        return doc

if __name__ == '__main__':
    import json
    import sys
    h = HtmlReportGenerator()
    with open(sys.argv[1]) as f:
        content = json.load(f)
    print(h.generate('report', content))
