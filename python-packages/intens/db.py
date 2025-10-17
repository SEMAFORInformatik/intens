import requests
import json
import base64


class Client(object):
    def __init__(self,
                 baseurl,
                 username='',
                 password=''):
        self.baseurl = baseurl
        self.apiurl = self.baseurl + '/services/rest'
        self.verify = False
        self.username = username
        self.password = password
        self.headers = None

        if not (username and password):
            return

        try:
            self._login(username, password)
        except Exception:
            # try with Basic authorization
            next
        else:
            # successful login
            return

        auth = base64.b64encode(f'{username}:{password}'.encode()).decode()
        self.headers = {'content-type': 'application/json',
                        'authorization': 'Basic ' + auth}

    def _login(self, username, password):
        self.username = username
        self.password = password

        if not (username and password):
            print("username/password missing")
            return

        r = requests.post(self.baseurl+"/login",
                          headers={'content-type': 'application/json'},
                          json={'username': username,
                                'password': password})
        r.raise_for_status()

        auth = r.headers['Authorization']  # this should be the Bearer token
        self.headers = {'content-type': 'application/json',
                        'authorization': auth}

    # === Users ===

    def getUsers(self):
        r = requests.get(self.apiurl+"/users",
                         headers=self.headers,
                         verify=self.verify)
        r.raise_for_status()
        return r.json()

    def saveUser(self, user):
        return requests.put(self.apiurl+"/users",
                            data=json.dumps(user),
                            headers=self.headers,
                            verify=self.verify)

    # === Types ===

    def createType(self, typedef):
        return requests.put(self.apiurl+"/components/type",
                            data=json.dumps(typedef),
                            headers=self.headers,
                            verify=self.verify)

    def getTypes(self):
        r = requests.get(self.apiurl+"/components/types",
                         headers=self.headers,
                         verify=self.verify)
        r.raise_for_status()
        return r.json()

    # === Components ===

    def getComponentsByType(self, typename, filterargs={}):
        r = requests.get(self.apiurl+"/components/type/{}".format(typename),
                         params=filterargs,
                         headers=self.headers,
                         verify=self.verify)
        r.raise_for_status()
        return r.json()

    def getComponent(self, id, filterargs={}):
        r = requests.get(self.apiurl+"/components/{}".format(id),
                         params=filterargs,
                         headers=self.headers,
                         verify=self.verify)
        r.raise_for_status()
        return r.json()

    def saveComponent(self, comp):
        r = requests.put(self.apiurl+"/components",
                         json=comp,
                         headers=self.headers,
                         verify=self.verify)
        r.raise_for_status()
        return r.json()

    def checkComponent(self, comp):
        return requests.put(self.apiurl+"/components/check",
                            json=comp,
                            headers=self.headers,
                            verify=self.verify)

    def importComponent(self, typename, content):
        content['type'] = typename
        r = requests.put(self.apiurl+"/components/import",
                         data=json.dumps(content),
                         headers=self.headers,
                         verify=self.verify)
        r.raise_for_status()
        return r.json()

    def getComponentModification(self, id, mod_id=None):
        url = self.apiurl + f"/components/{id}/modifications"
        if mod_id:
           url += f"/{mod_id}"
        r = requests.get(url,
                         headers=self.headers,
                         verify=self.verify)
        r.raise_for_status()
        return r.json()


    # === Projects ===

    def getProjects(self, filterargs={}):
        """valid filter keys: _page, _pagesize, _sort, _projection,
        name, status, owner, maxAge, """
        r = requests.get(self.apiurl+"/projects",
                         params=filterargs,
                         headers=self.headers,
                         verify=self.verify)
        r.raise_for_status()
        return r.json()

    def getProject(self, id):
        r = requests.get(self.apiurl+"/projects/{}".format(id),
                         headers=self.headers,
                         verify=self.verify)
        r.raise_for_status()
        return r.json()

    def putProject(self, content):
        content['type'] = 'Project'
        return requests.put(self.apiurl+"/projects",
                            data=json.dumps(content),
                            headers=self.headers,
                            verify=self.verify)

    def importProject(self, content):
        content['type'] = 'Project'
        r = requests.put(self.apiurl+"/components/import",
                            data=json.dumps(content),
                            headers=self.headers,
                            verify=self.verify)
        r.raise_for_status()
        return r.json()

    def saveProject(self, content):
        return self.putProject(content)

    def deleteProject(self, id):
        return requests.delete(self.apiurl+"/projects/{}".format(id),
                               headers=self.headers,
                               verify=self.verify)

    def checkProject(self, content):
        content['type'] = 'Project'
        return requests.put(self.apiurl+"/projects/check",
                            data=json.dumps(content),
                            headers=self.headers,
                            verify=self.verify)

    # === Variants ===

    def getVariants(self, projectid=0, filterargs={}):
        url = self.apiurl+"/variants"
        if projectid > 0:
            url += "?projectId={}".format(projectid)

        r = requests.get(url,
                         params=filterargs,
                         headers=self.headers,
                         verify=self.verify)
        r.raise_for_status()
        return r.json()

    def getVariant(self, id, filterargs={}):
        r = requests.get(self.apiurl+"/variants/{}".format(id),
                         params=filterargs,
                         headers=self.headers,
                         verify=self.verify)
        r.raise_for_status()
        return r.json()

    def putVariant(self, content):
        content['type'] = 'Variant'
        return requests.put(self.apiurl+"/variants",
                            data=json.dumps(content),
                            headers=self.headers,
                            verify=self.verify)

    def importVariant(self, content, projectid):
        content['type'] = 'Variant'
        content['projectId'] = projectid

        r = requests.put(self.apiurl+"/components/import",
                            data=json.dumps(content),
                            headers=self.headers,
                            verify=self.verify)
        r.raise_for_status()
        return r.json()

    def saveVariant(self, content):
        return self.putVariant(content)

    def deleteVariant(self, id):
        return requests.delete(self.apiurl+"/variants/{}".format(id),
                               headers=self.headers,
                               verify=self.verify)
