#!/usr/bin/env python
#
# Intens Persistent Interface Generator
#
# Read an Intens generated XML file containing type definition
# and generate Intens description and SQL files on the basis of
# a couple of template files (see directory templates)
#
# Ronald Tanner, 2010-01-10, SEMAFOR Informatik & Energie AG
#
import os.path
import sys
import xml.dom.minidom
import mako.template
import mako.lookup
import json

#
# naming convention for detecting project variant component types:
#
#   VariantTypeXY
#
# expects that TypeXY is defined and is PERSISTENT
#
PROJVAR = 'Variant'
PRIMITIVE_TYPE_SET = set(['INTEGER', 'REAL', 'STRING', 'DATE', 'CDATA'])


class PersistItem:
    '''represents an item that must be persisted'''
    def __init__(self, typepath, path, item):
        if len(path) > 0:
            self.objectName = f"{'.'.join(path)}.{item.getAttribute('name')}"
        else:
            self.objectName = item.getAttribute('name')
        self.typeName = item.getAttribute('type')
        self.label = item.getAttribute('label')
        if not self.label:
            self.label = self.typeName
        self.typepath = typepath
        print(self.objectName + ': ' + self.typeName)
        self.properties = []
        self.assemblies = []
        self.projComps = []
        self.baseProperties = []
        self.item = item
        self.indent = 0
        self.useVariant = False
        self.useProjComps = False
        self.singleton = item.getAttribute('scalar') == 'true'

    def printStructItem(self, item):
        '''print indented item names of structure item'''
        if item.getAttribute('name') != '':
            print(item.getAttribute('name'))
            self.indent += 1
        for subitem in self.item.childNodes:
            if subitem.nodeType != subitem.TEXT_NODE:  # not '\n'
                print(' ' * (3 * self.indent))
                if subitem.getAttribute('type') not in PRIMITIVE_TYPE_SET:
                    self.printStructItem(subitem)
                else:
                    print(subitem.getAttribute('name'))
        if item.getAttribute('name') != '':
            self.indent -= 1

    def printItem(self):
        self.printStructItem(self.item)


class PersistGen:
    '''create intens description files from xml'''
    def __init__(self, homedir, filename):
        # XML einlesen und parsen
        self.xmldoc = xml.dom.minidom.parse(filename)
        self.basePropertySet = set(['id', 'name',
                                    'version', 'rev', 'desc',
                                    'devel', 'lifecycle',
                                    'applicab', 'approval', 'owner',
                                    'created', 'group', 'projectId',
                                    'changer', 'changedate',
                                    'type', 'reason'])
        self.lookup = mako.lookup.TemplateLookup(
            directories=[homedir + '/templates'])
        self.outDirectory = '.'
        self.persistItems = []
        self.path = []
        self.typepath = []

    def printChildren(self):
        for item in self.xmldoc.firstChild.childNodes:
            if item.nodeType != item.TEXT_NODE:  # not '\n'
                print(item.getAttribute('name'))

    def printItems(self):
        for item in self.xmldoc.getElementsByTagName('item'):
            print(item.getAttribute('name'))

    def getItem(self, path, node, lastIndex):
        '''return dict item of node '''
        v = {}
        if len(path) > 0:
            v['name'] = f"{'.'.join(path)}.{node.getAttribute('name')}"
            if lastIndex:
                v['indexedName'] = (
                    f".{'.'.join(path)}.{node.getAttribute('name')}[#]")
            else:
                v['indexedName'] = (
                    f".{'.'.join(path)}[#].{node.getAttribute('name')}")
        else:
            v['name'] = node.getAttribute('name')
            if lastIndex:
                v['indexedName'] = f".{node.getAttribute('name')}[#]"
            else:
                v['indexedName'] = f"[#].{node.getAttribute('name')}"
        # print(v['indexedName'])
        # print(v['name'])
        v['dbattr'] = node.getAttribute('dbattr')
        v['type'] = node.getAttribute('type')
        v['parenttype'] = list(self.typepath)
        v['label'] = node.getAttribute('label')
        if(node.getAttribute('matrix') == ''
           or node.getAttribute('matrix') == 'false'):
            v['matrix'] = ''
        else:
            v['matrix'] = 'MATRIX'

        v['unit'] = node.getAttribute('unit')
        if v['unit'] == '':
            v['unit'] = '1'
        v['dbunit'] = node.getAttribute('dbunit')
        if v['dbunit'] == '':
            v['dbunit'] = v['unit'].replace('[', '').replace(']', '')
            if v['dbunit'] == '':  # v['unit'] is '[]'
                v['dbunit'] = '1'
        v['helptext'] = node.getAttribute('helptext')
        if v['dbattr'] == '':
            v['dbattr'] = node.getAttribute('name')
        if v['label'] == '':
            v['label'] = node.getAttribute('name')
        if v['helptext'] == '':
            v['helptext'] = node.getAttribute('name')
        return v

    def createFile(self, persistItem, filename, templatename):
        '''creates the output file from template'''
        mytemplate = self.lookup.get_template(templatename)
        with open(filename, 'w', encoding='utf-8', newline='\n') as fs:
            fs.write(mytemplate.render_unicode(
                properties=persistItem.properties,
                assemblies=persistItem.assemblies,
                projComps=persistItem.projComps,
                baseProperties=persistItem.baseProperties,
                object=persistItem.objectName,
                id=persistItem.objectName.replace('.', '_'),
                type=persistItem.typeName,
                label=persistItem.label,
                useVariant=persistItem.useVariant,
                useProjComps=persistItem.useProjComps,
                singleton=persistItem.singleton))

    def buildAssembly(self, item):
        '''build assembly from item'''
        name = item.getAttribute('name')
        ass = {}
        if len(self.path) > 1:
            ass['name'] = f"{'.'.join(self.path[1:])}.{name}"
        else:
            ass['name'] = name
        ass['asstype'] = item.getAttribute('type').replace(PROJVAR, '')
        ass['type'] = item.getAttribute('type')
        ass['helptext'] = item.getAttribute('helptext')
        if item.getAttribute('helptext') == '':
            ass['helptext'] = item.getAttribute('asstype')
        ass['properties'] = []

        # print('Assembly Name %s Type %s Component Type %s' %
        #       (ass['name'], ass['asstype'], ass['type']))
        return ass

    def getProjComponentItems(self, projComps, item):
        '''extract all proj component items of this item'''
        if item.getAttribute('name') != '':
            self.path.append(item.getAttribute('name'))
        for subitem in item.childNodes:
            if subitem.nodeType != subitem.TEXT_NODE:  # not '\n'
                if subitem.getAttribute('type') not in PRIMITIVE_TYPE_SET:
                    if subitem.getAttribute('type').startswith(PROJVAR):
                        self.getProjComponentItems(projComps, subitem)
                else:
                    if(item.getAttribute('type').startswith(PROJVAR)
                       and subitem.getAttribute('name') == 'comp_id'):
                        # print('.'.join(self.path), item.getAttribute('name'))
                        projComps.append(item.getAttribute('name'))

        if item.getAttribute('name') != '':
            self.path.pop()

    def getItems(self, parent, persistItem, item):
        '''extract all child items of this item'''
        if item.getAttribute('name') != '':
            if item.getAttribute('dbattr') != '':
                self.path.append(item.getAttribute('dbattr'))
            else:
                self.path.append(item.getAttribute('name'))
            self.typepath.append(item.getAttribute('type'))
        for subitem in item.childNodes:
            if subitem.nodeType != subitem.TEXT_NODE:  # not '\n'
                if subitem.getAttribute('type') not in PRIMITIVE_TYPE_SET:
                    if(subitem.getAttribute('type').startswith(PROJVAR)
                       # prevent recursive assembly structures
                       and(item.getAttribute('type') == PROJVAR
                           or not item.getAttribute('type').startswith(PROJVAR))):
                        parent.assemblies.append(
                            self.buildAssembly(subitem))
                    self.getItems(parent, persistItem, subitem)
                else:
                    v = self.getItem(self.path, subitem, False)
                    if len(self.path) > 1:
                        v['name'] = (f"{'.'.join(self.path[1:])}."
                                     f"{subitem.getAttribute('name')}")
                    else:
                        v['name'] = subitem.getAttribute('name')
                    if v['dbattr'] in self.basePropertySet:
                        v['name'] = v['dbattr']
                        v.pop('dbattr')
                        persistItem.baseProperties.append(v)
                    else:
                        persistItem.properties.append(v)
        if item.getAttribute('name') != '':
            self.path.pop()
            self.typepath.pop()

    def addProperty(self, props, name, ptype, parenttype, dbattr='', unit=''):
        keys = list(zip(name.split('.'), parenttype[1:] + [ptype]))
        for k in keys[:-1]:
            if k not in props:
                props[k] = {}
            props = props[k]
        props[keys[-1]] = (dbattr, unit)

    def dictToProps(self, props_dict):
        props = []
        for k, v in props_dict.items():
            if isinstance(v, dict):
                props.append({'name': k[0], 'type': k[1],
                              'props': self.dictToProps(v)})
            else:
                # k: (datapool variable name, datapool variable type)
                # v: (dbattr, dbunit)
                prop = {'name': v[0] if (v and v[0]) else k[0],
                        'type': k[1]}
                if v and v[1]:
                    prop['unit'] = v[1]
                props.append(prop)
        return props

    def createType(self, ptype, properties):
        # base properties
        props_dict = {('name', 'STRING'): None,
                      ('rev', 'INTEGER'): None,
                      ('desc', 'STRING'): None,
                      ('approval', 'STRING'): None,
                      ('created', 'DATE'): None,
                      ('group', 'STRING'): None}

        if ptype == 'Variant' or ptype == 'LogBook':
            props_dict.update({('projectId', 'LONG'): None})
        else:
            props_dict.update({('applicab', 'STRING'): None})
        for v in properties:
            self.addProperty(props_dict, v['name'], v['type'], v['parenttype'],
                             v.get('dbattr', ''), v.get('dbunit', ''))
        props = self.dictToProps(props_dict)

        return {'type': ptype, 'props': props}

    def createFiles(self):
        '''create intens include files for all persistent items'''
        self.persistItems = []
        varianttypes = []
        projComps = []
        variant = 0
        for item in self.xmldoc.firstChild.childNodes:
            if item.nodeType != item.TEXT_NODE:  # not '\n'

                persistItem = PersistItem([], [], item)
                persistGen.getItems(persistItem, persistItem, item)
                if item.getAttribute('name') != 'variant':
                    self.persistItems.append(persistItem)
                else:  # create project variant file
                    variant = persistItem
                    persistGen.getProjComponentItems(projComps, item)
                    for t in persistItem.assemblies:
                        varianttypes.append(t['asstype'])

        # create other
        typedef = []
        for persistItem in self.persistItems:
            persistItem.useVariant = persistItem.typeName in varianttypes
            persistItem.useProjComps = persistItem.objectName in projComps
            if persistItem.useProjComps:
                variant.projComps.append({'object': persistItem.objectName,
                                          'type': persistItem.typeName})
            # else:
            #     print(persistItem.objectName, ' not in ', str(projComps))

            self.createFile(persistItem, '%s/g_%s_uiManager.inc' %
                            (self.outDirectory, persistItem.objectName),
                            'ui.mako')
            self.createFile(persistItem, '%s/g_%s_functions.inc' %
                            (self.outDirectory, persistItem.objectName),
                            'functions.mako')
            self.createFile(persistItem, '%s/g_%s_datapool.inc' %
                            (self.outDirectory, persistItem.objectName),
                            'datapool.mako')
            typedef.append(self.createType(persistItem.typeName,
                                           persistItem.properties))

        if variant != 0:
            self.createFile(variant, '%s/g_project_functions.inc' %
                            (self.outDirectory,),
                            'projfuncs.mako')
            typedef.append(self.createType('Variant', variant.properties))

        props = [{'name': 'name', 'type': 'STRING'},
                 {'name': 'desc', 'type': 'STRING'},
                 {'name': 'status', 'type': 'STRING'},
                 {'name': 'group', 'type': 'STRING'},
                 {'name': 'created', 'type': 'DATE'}]
        typedef.append({'type': 'Project', 'props': props})

        with open('{}/update-db.json'.format(self.outDirectory),
                  'w', encoding='utf-8', newline='\n') as outfile:
            json.dump(typedef, outfile, indent=3)
            outfile.write('\n')  # Add newline as json.dump does not


if __name__ == '__main__':
    if len(sys.argv) > 1:
        homedir = os.path.abspath(os.path.dirname(sys.argv[0]) + '/..')
        for f in sys.argv[1:]:
            persistGen = PersistGen(homedir, f)
            persistGen.createFiles()
    else:
        print('Usage: %s <filename>' % sys.argv[0])
