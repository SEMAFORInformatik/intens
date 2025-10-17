#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Updates the database
#
import sys
import requests
import json
import os
from collections import defaultdict
import logging

logger = logging.getLogger(__name__)  # Get root logger


def _login(baseurl, username, password):
    if not (username and password):
        logger.error("username/password missing")
        return

    r = requests.post(baseurl+"/login",
                      headers={'content-type': 'application/json'},
                      json={'username': username,
                            'password': password})
    r.raise_for_status()

    try:
        return r.headers['Authorization'] # this should be the Bearer token
    except KeyError:
        pass
    return f'Bearer {r.text}'


def get_property_types(t, ptrefs):
    for p in t['props']:
        if 'props' in p:
            get_property_types(p, ptrefs)
        else:
            if p['name'] == "id":
                logger.warning("Property in %s can't be named %s", t['type'], p['name'])
            ptrefs[p['name']].append((t['type'], p.get('unit', '')))


def get_conflicting_property_types(element_types):
    ptrefs = defaultdict(list)
    for t in element_types:
        get_property_types(t, ptrefs)
    return {p:ptrefs[p] for p in ptrefs
            if len(ptrefs[p]) > 1 and len(set([u[-1] for u in ptrefs[p]]))>1}


def main(auth, updatefile, apiurl):
    # Try to get the json
    with open(updatefile) as fp:
        element_types = json.load(fp)

    conflicts = get_conflicting_property_types(element_types)
    if conflicts:
        logger.error("Conflicting Property Types (Total %d)", len(conflicts))
        for c in conflicts:
            print(f"{c}: {set([u for u in conflicts[c]])}")
        sys.exit(-1)

    results = dict(update_success=[], update_failed=[], code=[])

    for etype in element_types:
        headers = {'content-type': 'application/json',
                   'authorization': auth}

        try:
            # insert component type
            r = requests.put(apiurl + "/components/type",
                             data=json.dumps(etype),
                             headers=headers, timeout=500)
            r.raise_for_status()
            logger.info('%s: %s', etype['type'], r.status_code)
            results['update_success'].append(etype["type"])
        except requests.exceptions.HTTPError as e:
            results['update_failed'].append(etype["type"])
            logger.error('%s: %s  %s',
                         etype['type'], r.status_code,
                         json.loads(e.response.text)['error'])
        results['code'].append(r.status_code)
    return results


if __name__ == "__main__":

    FORMAT = "%(asctime)s %(levelname)s: %(message)s"
    logging.basicConfig(level=logging.INFO, format=FORMAT)
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
    argparser.add_argument('updatefile',
                           help='name of database update file',
                           nargs='?',
                           default='./etc/update-db.json')
    args = argparser.parse_args()

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

        r = main(auth, args.updatefile, baseurl+'/services/rest')
    except ValueError as ex:
        sys.stderr.write(ex.message + "\n")
        sys.exit(-1)
    except requests.exceptions.ConnectionError:
        sys.exit(f"Connection failure: {baseurl}\n")
    # except IOError as ex:
    #    sys.stderr.write(f'{ex.strerror}: {args.updatefile}\n')
    #    sys.exit(-1)
    logger.info("TOTAL success %d failed %d",
                len(r['update_success']),
                len(r['update_failed']))
##    json.dump(r, sys.stdout, indent=1)
