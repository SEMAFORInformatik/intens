#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Generates liquibase migrations
# Ronald Tanner SEMAFOR Informatik & Energie AG (2023)
#
import os
import pathlib
import logging
import requests
from lxml import etree

LQB_NS = "http://www.liquibase.org/xml/ns/dbchangelog"
XSI_NS = "http://www.w3.org/2001/XMLSchema-instance"
EXT_NS = "http://www.liquibase.org/xml/ns/dbchangelog-ext"
NSMAP = {None: LQB_NS,
         "xsi": XSI_NS,
         "ext": EXT_NS}
xsi = [LQB_NS,
       "http://www.liquibase.org/xml/ns/dbchangelog/dbchangelog-3.8.xsd",
       "http://www.liquibase.org/xml/ns/dbchangelog-ext",
       "http://www.liquibase.org/xml/ns/dbchangelog/dbchangelog-ext.xsd"]

logger = logging.getLogger(__name__)


def _login(baseurl, username, password):
    if not (username and password):
        logger.error("username/password missing")
        return ''

    r = requests.post(baseurl+"/login",
                      headers={'content-type': 'application/json'},
                      timeout=5,  # time in seconds
                      json={'username': username,
                            'password': password})
    r.raise_for_status()
    try:
        return r.headers['Authorization']  # this should be the Bearer token
    except KeyError:
        pass
    return f'Bearer {r.text}'


def writeChangelog(changesdir, user, version, diff):
    """"Write new xml changelog and link all created csv data files"""
    parser = etree.XMLParser(remove_blank_text=True)
    dbchangelog = etree.Element(
        "databaseChangeLog",
        {'{%s}schemaLocation' % XSI_NS: ' '.join(xsi), 'logicalFilePath': version + '.xml'},
        nsmap=NSMAP)
    for t in diff:
        cs = etree.SubElement(dbchangelog, 'changeSet', author=user,
                              id='{}-{}'.format(version, t))
        etree.SubElement(cs, 'loadData', tableName=t, seperator=",",
                         relativeToChangelogFile="true",
                         file='{}/{}.csv'.format(version, t))
    et = etree.ElementTree(dbchangelog)
    xmlfile = changesdir / (version + '.xml')
    with xmlfile.open('wb') as fp:
        et.write(fp, encoding="utf-8", pretty_print=True,
                 xml_declaration=True)


def find_type_by_id(t, id):
    for k in t:
        if id == t[k]['id']:
            return k
    return ''

def element_types(paths):
    csvfile = 'element_types.csv'
    et = {}
    for p in paths:
        if not (p/csvfile).is_file():
            continue
        etr = (p/csvfile).read_text().split('\n')
        header = etr[0].split(',')
        eid = header.index("ID")
        name = header.index("NAME")
        for l in etr[1:]:
            if l:
                r = l.split(',')
                et[r[name]] = {'id': int(r[eid]), 'refs': {}}

    csvfile = 'element_type_references.csv'
    for p in paths:
        if not (p/csvfile).is_file():
            continue
        etr = (p/csvfile).read_text().split('\n')
        header = etr[0].split(',')
        pid = header.index('ELEMENT_TYPE_ID')
        cid = header.index('REFERENCES_ID')
        ref = header.index('REFERENCES_KEY')
        for l in etr[1:]:
            if l:
                r = l.split(',')
                parent = find_type_by_id(et, int(r[pid]))
                if parent:
                    child = find_type_by_id(et, int(r[cid]))
                    et[parent]['refs'][r[ref]] = child
    return et


def element_types_db(typedb):
    et = {}
    for t in typedb['element_types']:
        et[t['NAME']] = {'id': t['ID'],
                         'refs':{}}
    for r in typedb['element_type_references']:
        parent = find_type_by_id(et, r['ELEMENT_TYPE_ID'])
        et[parent]['refs'][r['REFERENCES_KEY']] = find_type_by_id(
            et, r['REFERENCES_ID'])
    return et


def property_types(et, paths):
    csvfile = 'property_types.csv'
    pt = {}
    for p in paths:
        if not (p/csvfile).is_file():
            continue
        etr = (p/csvfile).read_text().split('\n')
        header = etr[0].split(',')
        tid = header.index('ID')
        tname = header.index('NAME')
        ttype = header.index('TYPE')
        tunit = header.index('UNIT')
        for l in etr[1:]:
            if l:
                r = l.split(',')
                pt[r[tname]] ={'id': int(r[tid]), 'unit': f"{r[tunit]}",
                           'type': f"{r[ttype]}",
                           'element_types': []}

    csvfile = 'element_types_property_types.csv'
    for p in paths:
        if not (p/csvfile).is_file():
            continue
        etr = (p/csvfile).read_text().split('\n')
        header = etr[0].split(',')
        etid = header.index('ELEMENT_TYPE_ID')
        ptid = header.index('PROPERTY_TYPES_ID')
        for l in etr[1:]:
            if l:
                r = l.split(',')
                element_type = find_type_by_id(et, int(r[etid]))
                property_type = find_type_by_id(pt, int(r[ptid]))
                pt[property_type]['element_types'].append(element_type)
    return pt


def property_types_db(et, typedb):
    pt = {}
    for t in typedb['property_types']:
        name = t['NAME']
        pt[name] ={
            'id': t['ID'],
            'unit': t['UNIT'],
            'type': t['TYPE'],
            'element_types': []}
    for t in typedb['element_types_property_types']:
        property_type = find_type_by_id(pt, t['PROPERTY_TYPES_ID'])
        try:
            pt[property_type]['element_types'].append(find_type_by_id(et, t['ELEMENT_TYPE_ID']))
        except KeyError:
            logger.info("No property_type for element_type_id: %s ", t)
    return pt


def getTypesDB(baseurl, auth):
    """Get newest type structure by requesting the db-service"""
    headers = {'content-type': 'application/json',
               'authorization': auth}
    #r = requests.get(baseurl+"/services/rest/components/types",
    r = requests.get(baseurl+"/services/liquibase/types",
                     timeout=5, headers=headers)
    if r.status_code == 200:  # OK
        return r.json()
    r.raise_for_status()


def getNextVersion(changesdir):
    """Return next changelog version depending on already
    created changelog files"""
    try:
        dbchg = sorted([child.name[1:].split('.')[0]
                        for child in changesdir.glob('v*.xml')])
        version = dbchg[-1] if len(dbchg) > 0 else '0000'
    except FileNotFoundError:
        changesdir.mkdir()
        version = '0000'
    return 'v' + str(int(version) + 1).zfill(len(version))


def getUser():
    """Get username of logged in user"""
    return os.environ["USERNAME"] if "C:" in os.getcwd() else os.environ["USER"]

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(message)s')
    import argparse
    argparser = argparse.ArgumentParser(
        description='Update Database Types of Intens Application.')
    argparser.add_argument('--host',
                           help='host of database',
                           dest='dbhost',
                           default='localhost')
    argparser.add_argument('-p', '--port',
                           help='port number of database',
                           dest='dbport',
                           type=int,
                           default=8080)
    argparser.add_argument('--db',
                           help='name of database',
                           dest='dbname',
                           default='workbook')
    argparser.add_argument('changelogdir',
                           help='name of changelog directory',
                           nargs='?',
                           default='../db/src/main/resources/db/changelog')
    args = argparser.parse_args()

    if (pathlib.Path(args.changelogdir) / 'changes').is_dir():
        changesdir = pathlib.Path(args.changelogdir) / 'changes'
    else:
        changesdir = pathlib.Path(args.changelogdir)

    baseurl = os.getenv('REST_SERVICE_BASE',
                        default=f'http://{args.dbhost}:{args.dbport}/{args.dbname}')
    try:
        # username/password
        try:
            auth = _login(baseurl, 'admin', 'admin')
        except requests.exceptions.HTTPError:
            # build (and use) basic authorization
            auth = 'Basic ' + os.getenv('REST_SERVICE_AUTHHEADER',
                                        default='YWRtaW46YWRtaW4=')

    except requests.exceptions.HTTPError:
        auth = 'Basic ' + os.getenv('REST_SERVICE_AUTHHEADER',
                                    default='YWRtaW46YWRtaW4=')
    #
    paths = [child for child in changesdir.iterdir()
             if child.is_dir() and child.name[0] == 'v']

    version = getNextVersion(changesdir)
    vchglog = pathlib.Path(changesdir / version)
    vchglog.mkdir(exist_ok=True)
    total_changes = 0

    typesDB = getTypesDB(baseurl, auth)
    logger.debug("DB property_types %d", len(typesDB['property_types']))
    logger.debug("DB element_types %d", len(typesDB['element_types']))
    logger.debug("DB element_types_property_types %d", len(typesDB['element_types_property_types']))
    logger.debug("DB element_type_references %d", len(typesDB['element_type_references']))
    # collect types from database and files
    et = [element_types(paths), element_types_db(typesDB)]
    try:
        id = max([et[0][k]['id'] for k in et[0]])
    except ValueError:  # empty sequence
        id = 0
    etmap = {}
    diff = []
    # add new element types ------------------------------
    # if no element types, don't do diff with 0 list, otherwise the order of the list will be changed
    if len(et[0].keys())==0:
        etdiff = et[1].keys()
    else:
        etdiff = et[1].keys() - et[0].keys()

        
    content = ['ID,BEAN_ID,BEAN_VERSION_ID,NAME']
    for name in etdiff:
        id += 1
        content.append(f"{id},id,version,{name}")
        etmap[name] = id
    logger.info("%s: num types %d -> %d",
                baseurl, len(et[0]), len(et[1]))
    if len(content)-1 > 0:
        (vchglog / 'element_types.csv').write_text('\n'.join(content))
        diff.append('element_types')
    total_changes += len(content)-1
    logger.info('element_types.csv: %d', len(content)-1)

    # add new element type references ---------------------
    content = ['ELEMENT_TYPE_ID,REFERENCES_ID,REFERENCES_KEY']
    for name in etdiff:
        for ref in et[1][name]['refs']:
            rtype = et[1][name]['refs'][ref]
            try:
                idparent = et[0][name]['id']
            except KeyError:
                idparent = etmap[name]
            try:
                idchild = et[0][rtype]['id']
            except KeyError:
                idchild = etmap[rtype]
            content.append(f"{idparent},{idchild},{ref}")
    for name in et[1].keys() - etdiff:
        for ref in et[1][name]['refs']:
            if ref not in et[0][name]['refs']:
                rtype = et[1][name]['refs'][ref]
                try:
                    idparent = et[0][name]['id']
                except KeyError:
                    idparent = etmap[name]
                try:
                    idchild = et[0][rtype]['id']
                except KeyError:
                    idchild = etmap[rtype]
                content.append(f"{idparent},{idchild},{ref}")

    if len(content)-1 > 0:
        (vchglog / 'element_type_references.csv').write_text('\n'.join(content))
        diff.append('element_type_references')
    total_changes += len(content)-1
    logger.info('element_type_references.csv: %d', len(content)-1)

    # property_types ----------------------------------------------------
    content = ['ID,NAME,TYPE,UNIT']
    pt = property_types(et[0], paths), property_types_db(et[1], typesDB)
    ptmap = {}
    try:
        id = max([pt[0][k]['id'] for k in pt[0]])
    except ValueError:  # empty sequence
        id = 0
        
    # if no property types, don't do diff with 0 list, otherwise the order of the list will be changed
    if len(et[0].keys())==0:
        ptdiff = sorted(pt[1].keys(), key=lambda x: (pt[1][x]['id']))
    else:
        ptdiff = pt[1].keys() - pt[0].keys()

    for name in ptdiff:
        id += 1
        ido = pt[1][name]['id']
        content.append(
            f"{id},{name},{pt[1][name]['type']},{pt[1][name]['unit']}")
        ptmap[name] = id
    if len(content)-1 > 0:
        (vchglog / 'property_types.csv').write_text('\n'.join(content))
        diff.append('property_types')
    total_changes += len(content)-1
    logger.info('property_types.csv: %d', len(content)-1)

    # element types property_types --------------------------------------
    ptet0 = []
    for pname in pt[0]:
        for ename in [e for e in pt[0][pname]['element_types'] if e]:
            ptet0.append((pname, ename))

    ptet = []
    for pname in pt[1]:
        for ename in pt[1][pname]['element_types']:
            ptet.append((pname, ename))

    new_combos = list(set(ptet)-set(ptet0))

    content = ['ELEMENT_TYPE_ID,PROPERTY_TYPES_ID']

    for pname, ename in new_combos:
        try:
            ptid = pt[0][pname]['id']
        except KeyError:
            ptid = ptmap[pname]
        try:
            etid = et[0][ename]['id']
        except KeyError:
            etid = etmap[ename]
        content.append(f'{etid},{ptid}')

    if len(content)-1 > 0:
        (vchglog / 'element_types_property_types.csv').write_text('\n'.join(content))
        diff.append('element_types_property_types')
    total_changes += len(content)-1
    logger.info('element_types_property_types.csv: %d', len(content)-1)

    if total_changes > 0:
        logger.info("Create csv changelog files: %s", version)
        writeChangelog(changesdir, getUser(), version, diff)
    else:
        logger.info("No diffs found")
        vchglog.rmdir()
