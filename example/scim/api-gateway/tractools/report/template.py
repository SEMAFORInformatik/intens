# -*- coding: utf-8 -*-
"""
  Copyright (c) 2015 Semafor Informatik & Energie AG
"""
import logging
import mako
import mako.lookup

import jinja2
 
logger = logging.getLogger(__name__)


class MakoTemplate:
    def __init__(self, templatedir):
        self.lookup = mako.lookup.TemplateLookup(
            directories=[templatedir],
            disable_unicode=False,
            input_encoding='utf-8',
            output_encoding='utf-8',
            default_filters=['decode.utf8'])

    def render(self, content):
        template = self.lookup.get_template('report.mako')
        logger.debug('mako rendering with {}'.format('report'))
        return template.render_unicode(content=content)


class JinjaTemplate:
    def __init__(self, templatedir, package_name='ifemag.report'):
        extensions = ['jinja2.ext.i18n']
        
        if package_name:
            loader = jinja2.PackageLoader(package_name, templatedir)
        else:
            loader = jinja2.FileSystemLoader(templatedir)
        self.env = jinja2.Environment(
            autoescape=False,
            extensions=extensions,
            loader=loader)

    def render(self, content, templ='report'):
        template = self.env.get_template(templ+'.jinja2')
        logger.debug('jinja rendering with {}'.format(templ))
        return template.render(content=content)

class JinjaStringTemplate:
    def __init__(self, templatestring):
        self.templatestring = templatestring
        self.template = jinja2.Template(templatestring)

    def render(self, content):
        return self.template.render(content=content)

