#!/usr/bin/env python
# coding: utf-8
# \file ReportConv.py
# \brief A report format converter
#
import sys
import os
import re
import glob
import shutil
import argparse
import json
import tempfile

intens_home = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))


def isPlatformWindows():
    """return true if this is a window system"""
    return (sys.platform == 'win32' or sys.platform == 'win64')


class Report:
    """\brief Basic Report Format Converter class
          reads data from stdin and converts the output to several
           formats

     PS     copies input directly to <tmpfile>.ps without any conversion

     Text
            copies the stdin data into a temporary file <tmpfile>
            and invokes a2ps on it if fileformat is not ascii.


      If <printername> is defined the <tmpfile>.ps will be directed
      to this printer.

      In any case this script creates a pdf file if fileformat is pdf.

      Invocation: (usually invoked by intens)

           reportconv [-P<printername>] [-#<copies>] [-F<fileformat>] [-O<orientation>] [-S<PAPERSIZE] [-xsl <stylesheet>] [-dtd <dtd-file>] [<tmpfile>]

      known Bugs:
           clean up is not satisfactory

      """

    def __init__(self, tmpFilename):
        """Reportconstructor.
        """
        self.tmpfile = tmpFilename

    def get_ps(self, copies, orientation):
        self.copies = copies
        self.orientation = orientation


class LatexReport(Report):
    """\brief Latex Report Converter Class
            copies input into a temporary TeX-file <tmpfile>.tex
            and starts latex on this file if fileformat is not ascii.
            If successful dvips will be run to convert the dvi-output to
            PostScript.
    """
    def __init__(self, tmpdir, tmpFilename):
        """The constructor.
        \param tmpFilename filename of temporary file
        """
        Report.__init__(self, tmpFilename)
        self.tmpdir = tmpdir

    def unlinkLatexTempfiles(self, fn_tex):
        for ext in ['dvi', 'aux', 'log', 'tex']:
            fn = os.path.basename(fn_tex[:-4]) + "." + ext
            if os.path.isfile(fn):
                os.unlink(fn)

        if os.path.isfile(fn_tex):
            os.unlink(fn_tex)

    def callPdfLatex(self, fn_tex):
        """ invokes latex on the created tmpfile"""
        rerun = 1
        while rerun == 1:
            command = "pdflatex " + fn_tex + " 1>&2"
            # print(command)
            status = os.system(command)
            if status != 0:
                self.unlinkLatexTempfiles(fn_tex)
                for fn in glob.glob(fn_tex[:-4] + ".*"):
                    os.unlink(fn)
                # print("after glob")
                return 1

            rerun = 0
            try:
                logfile_name = os.path.join(self.tmpdir,
                                            os.path.basename(fn_tex[:-4]) + ".log")
                with open(logfile_name, 'r') as Latexlog:
                    for line in Latexlog.readlines():
                        matchobj = re.search('Rerun', line)
                        if matchobj:
                            rerun = 1
            except IOError:
                # i.E. log file not found
                pass
        self.unlinkLatexTempfiles(fn_tex)
        return 0


class TextReporter:
    """\brief Text Report coverter Class
     copies the stdin data into a temporary file <tmpfile>
            and invokes a2ps on it if fileformat is not ascii.
     """

    def __init__(self, tmpFilename):
        """The constructor.
        \param tmpFilename filename of temporary file
        """
        self.tmpfile = tmpFilename

    def get_ps(self, orientation):
        """ get_ps create Postscript output format
        """
        self.orientation = orientation
        options = "--landscape --font-size=7.5" if self.orientation == "landscape" else "--portrait"
        status = os.system(
            "a2ps --no-header --columns=1 --rows=1 {options} {tmpfile} > {tmpfile}.ps".format(
                options=options, tmpfile=self.tmpfile))


class XmlReport:
    """\brief Xml Report coverter Class
     copies the stdin data into a temporary file <tmpfile>
     and invokes Xalan on it if fileformat is not ascii.
     """
    def __init__(self, tmpFilename):
        """The constructor.
        \param tmpFilename filename of temporary file
        """
        self.tmpfile = tmpFilename

    def get_ps(self, stylesheet):
        """ get_ps create Postscript output format
        """

        if stylesheet and stylesheet != '':
            # print("/usr/opt/xalan-c-1_5/c/bin/Xalan " + self.tmpfile + " " + stylesheet + " > " + self.tmpfile + ".ps")
            status = os.system(
                "/usr/opt/xalan-c-1_5/c/bin/Xalan {tmpfile} {stylesheet} > {tmpfile}.ps".format(
                    tmpfile=self.tmpfile, stylesheet=stylesheet))
        else:
            print("NO XSL OR DTD FILE EXIST")


class ReportConverter:
    """\brief Report converter Class
        sets the outputfilename and
        checks input for xml latex or text
    """

    def __init__(self, argv=None):
        """The constructor,
        gets optional parameters and sets TEXINPUT
        environment variable for latex
        """

        self.parseArgs(argv)

        # set default values
        self.debug = 0
        self.lines = ''
        self.type = ''

        # create temporary directory
        if isPlatformWindows():
            username = os.environ['USERNAME']
        else:
            username = os.environ['USER']
        prefix = "report-{username}-".format(username=username)

        self.tmpdir = tempfile.mkdtemp(prefix=prefix)

        # check self.tmpfile
        if self.tmpfile == "":  # not provided
            handle, self.tmpfile = tempfile.mkstemp(prefix=prefix)
            os.close(handle)

        # set TEXINPUT environment variable for latex

        if isPlatformWindows():
            os.environ['TEXINPUTS'] = ".//;" + intens_home + "/texmf//"
            if os.environ.get('TEX_HOME'):
                os.environ['TEXINPUTS'] += "; " + os.environ['TEX_HOME'] + "/texmf/tex//"

        if self.debug:
            sys.stderr.write("INTENS REPORT CONVERTER\n")
            sys.stderr.write("Copies: {}  Printer: {}  FileFormat: {}  Orientation: {}  Stylesheet: {}  tempFile: {}  tempDir: {}\n".
                             format(self.copies, self.printer, self.fileformat, self.orientation, self.stylesheet, self.tmpfile, self.tmpdir))

        os.chdir(self.tmpdir)

        if self.debug:
            sys.stderr.write("pwd: {}\n".format(os.getcwd()))

        # set output file name

        # sys.stderr.write(self.fileformat + "\n")

        if self.fileformat == 'pdf' or self.fileformat == 'PDF':
            if re.search('\.pdf$', self.tmpfile):
                self.outfile = self.tmpfile
            else:
                self.outfile = self.tmpfile + ".pdf"

        elif self.fileformat == 'hpgl':
            if re.search('\.hpgl$', self.tmpfile):
                self.outfile = self.tmpfile
            else:
                self.outfile = self.tmpfile + ".hpgl"
        elif self.fileformat == 'ascii':
            self.outfile = self.tmpfile
        else:
            if re.search('\.ps$', self.tmpfile):
                self.outfile = self.tmpfile
            else:
                self.outfile = self.tmpfile + ".ps"

    def parseArgs(self, argv):
        argparser = argparse.ArgumentParser(description="A simple report format converter")
        argparser.add_argument("-P", "--printer", help="set printer name")
        argparser.add_argument("-#", "--copies", help="set the number of copies to print", type=int, default=1)
        argparser.add_argument("-O", "--orientation", help="set the orientation: portrait|landscape)", default='portrait')
        argparser.add_argument("-S", "--papersize", help="set the papersize", default='a4')
        argparser.add_argument("-F", "--fileformat", help="set the fileformat", default='postscript')
        argparser.add_argument("-xsl", "--stylesheet", help="set the XSL stylesheet")
        argparser.add_argument("-dtd", "--dtd", help="set the document type")
        argparser.add_argument("-t", "--template", help="set the template", type=str, default=None)
        argparser.add_argument("tmpfile", nargs='?', help="set tmpfile base name", default="")

        if argv:
            args = argparser.parse_args(argv)
        else:
            args = argparser.parse_args()
        self.printer = args.printer
        self.copies = args.copies
        self.orientation = args.orientation
        self.fileformat = args.fileformat
        self.papersize = args.papersize
        self.stylesheet = args.stylesheet
        self.dtd = args.dtd
        self.tmpfile = args.tmpfile
        self.template = args.template
        # change fileformat
        if self.fileformat == "postscript" and not re.search('\.ps$', self.tmpfile):
            self.fileformat = "pdf"

    def readall(self):
        """ reads stdin input lines """
        self.lines = sys.stdin.readlines()
        return len(self.lines)

    def write_tmpfile(self):
        """ creates a new output tmpfile object and
            writes inputlines into the file
        """

        # is template set? does file exist?
        try:
            if self.template is not None:
                from mako.template import Template
                templateStr = open(self.template, 'r').read()
                jsonForTemplate = json.loads(''.join(map(str, self.lines)))
                self.lines = Template(templateStr).render_unicode(data=jsonForTemplate).split('\n')
        except Exception as e:
            sys.stderr.write('Cannot render the template. Error: {0}'.format(e))
            sys.exit(1)

        self.file = open(self.tmpfile, "w")
        for l in self.lines:
            self.file.write(l)
        self.file.close()

    def get_input_type(self):
        """ get_input_type checks input for xml,latex or text
        """
        self.file = open(self.tmpfile, 'r')
        self.lines = self.file.readlines()
        self.file.close()
        # check input for xml, latex or text:

        firstline = 0
        index = 0
        self.type = "text"
        while(firstline == 0 and index < len(self.lines)):
            s = self.lines[index]

            matchobj = re.search('^\\\\documentclass', s)
            if matchobj:
                self.type = 'tex'

            matchobj = re.search('^<\?xml', s)
            if matchobj:
                self.type = 'xml'

            matchobj = re.search('^%!PS', s)
            if matchobj:
                self.type = 'ps'

            matchobj = re.search('^IN\;IP', s)
            if matchobj:
                self.type = 'hpgl'

            matchobj = re.search('^[^\#\%\s]|^\s*$', s)
            if matchobj:
                firstline = 1

            index = index + 1

        if self.type == "hpgl":
                self.fileformat = "hpgl"

        if self.debug:
            sys.stderr.write("Input format is: {}\n".format(self.type))

        # we are finished here if the requested format is ascii
        # (or should we check the printer variable also?)

        if self.fileformat == "ascii":
            shutil.move(self.tmpfile, self.outfile)

        return 0

    def process(self):
        """ copies input into a temporary TeX-file <tmpfile>.tex
        or into a temporary Text-file and creates LatexReport object or
        TextReport
        """
        if self.fileformat == "ascii":
            sys.exit(0)

        if self.type == 'tex' and self.fileformat == 'tex':
            shutil.move(self.tmpfile, self.tmpfile + ".tex")
            return 0

        elif self.type == 'tex' and self.fileformat != 'ascii':
            # print("RC: create LatexReport object")
            # get tex file napme
            if re.search('\.pdf$', self.tmpfile):
                fn = self.tmpfile[:-4] + ".tex"
            elif re.search('\.ps$', self.tmpfile):
                fn = self.tmpfile[:-3] + ".tex"
            else:
                fn = self.tmpfile + ".tex"
            shutil.move(self.tmpfile, fn)
            latexReport = LatexReport(self.tmpdir, self.tmpfile)

            # print("RC: call Latex::callPdfLatex")
            if latexReport.callPdfLatex(fn) == 0:
                # outfile ps
                if self.fileformat == "postscript":
                    os.system("pdf2ps " + fn[:-4] + ".pdf")
                    if os.path.isfile(os.path.join(self.tmpdir, os.path.basename(fn)[:-4] + ".ps")):
                        shutil.move(os.path.join(self.tmpdir, os.path.basename(fn)[:-4] + ".ps"), self.outfile)
                        self.type = "postscript"
                else:
                    self.type = "pdf"
                return 0

            return 1

        elif self.type == 'xml' and self.fileformat == 'xml':
            shutil.move(self.tmpfile, self.tmpfile + ".xml")
            return 0

        elif self.type == "xml":
            # print("RC: create XmlReport object")
            if self.dtd and self.dtd != '':
                # copy dtd-file to tmpdir
                file = re.sub('.+\/', '', self.dtd)
                dir = os.getcwd()
                print("demo : " + dir + "   " + self.dtd + "  " + file)
                shutil.copyfile(self.dtd, file)
            else:
                # ???
                return 1

            xmlReport = XmlReport(self.tmpfile)

            # print("RC:call Xml::get_ps")
            xmlReport.get_ps(self.stylesheet)

            if self.dtd and self.dtd != '':
                os.unlink(file)

            shutil.move(self.tmpfile + ".ps", self.tmpfile)
            self.get_input_type()

            return self.process()

        elif self.type == "text":
            # print("RC: create TextReporter object")
            textReporter = TextReporter(self.tmpfile)

            # print("RC: call Text::get_ps")
            textReporter.get_ps(self.orientation)

            self.type = "ps"
            return 0
        # type must be ps or hpgl

        else:
            shutil.move(self.tmpfile, self.tmpfile + "." + self.type)
        return 0

    def onPrint(self):
        """converts the printer names and executes lpr command
        """

        # if Printer is set => execute lpr command

        if self.printer:
            fileToPrint = os.path.join(self.tmpdir, os.path.basename(self.tmpfile + "." + self.type))

            self.server = None
            if re.search('\\\\\\\\', self.printer):
                # on MS windows systems we need to convert the printer names
                self.server = self.printer
                self.server = re.sub('\\\\\\\\.+', '', self.server)
                self.printer = re.sub('\\\\\\\\.+\\\\', '', self.printer)

            if self.server:
                command = "lpr -S {server} -P {printer} {filename}".format(
                    server=self.server, printer=self.printer,
                    filename=fileToPrint)
            else:
                command = "lpr -P {printer} {filename}".format(
                    printer=self.printer, filename=fileToPrint)
            if self.debug:
                sys.stderr.write("CMD: {}\n".format(command))
            status = os.system(command)

            for l in glob.glob(self.tmpfile + ".*"):
                os.unlink(l)
            if os.access(self.tmpfile, os.O_RDWR):
                os.unlink(self.tmpfile)
            shutil.rmtree(self.tmpdir)
            sys.exit(0)

    def get_pdf_file(self):
        """ creates pdf fileformat
        """

        # make pdf file if requested
        if re.search('pdf', self.fileformat):
            if self.type == "ps":
                self.size = "a4"
                if self.papersize != "unscaled":
                    self.size = self.papersize
                os.system(
                    "ps2pdf -sPAPERSIZE={size} {tmpfile}.ps {outfile}".format(
                        size=self.size, tmpfile=self.tmpfile, outfile=self.outfile))
                self.type = "pdf"

            # outputfile not created, move file from tmpdir
            if not os.path.isfile(self.outfile):
                shutil.move(os.path.join(self.tmpdir, os.path.splitext(os.path.basename(self.tmpfile))[0] + ".pdf"), self.outfile)

        # Tmpfile must be PS or hpgl.in any other case there is an exit above
        elif re.search('.hpgl', self.outfile):
            shutil.move(self.tmpfile + ".hpgl", self.outfile)

        else:
            if os.path.isfile(self.tmpfile):
                shutil.move(self.tmpfile, self.outfile)

        for fn in glob.glob(self.tmpfile + ".*"):
            if fn != self.outfile:
                os.unlink(fn)

        if os.access(self.tmpfile, os.O_RDWR) and self.tmpfile != self.outfile:
            os.unlink(self.tmpfile)

        shutil.rmtree(self.tmpdir)


def main():
    r = ReportConverter()
    # sys.argv muss nicht übergeben werden
    #  (argparser.parse_args() verwendet sys.argv)

    if r.readall() > 0:
        r.write_tmpfile()
        r.get_input_type()
        if r.process() == 0:
            r.onPrint()
            r.get_pdf_file()
    sys.exit(0)


if __name__ == '__main__':
    main()
