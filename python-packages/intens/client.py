import zmq
import json
import base64
from pathlib import Path

REPLY_PORT = 15560
zmq_context = zmq.Context()

class Client:
    def __init__(self,
                 port=REPLY_PORT):
        self.socket = zmq_context.socket(zmq.REQ)
        self.socket.connect(f"tcp://localhost:{port}")

    def hello(self, timeout=2):
        self.socket.send_string("control", zmq.SNDMORE)
        request = {"command": "HELLO"}
        self.socket.send(json.dumps(request).encode("utf-8"))
        message = self.socket.recv_multipart()
        rslt = json.loads(message[0])
        assert rslt.get("status", None) == "OK"

    def quit(self):
        self.socket.send_string("control", zmq.SNDMORE)
        request = {"command": "EXIT", "argument": "force"}
        self.socket.send(json.dumps(request).encode("utf-8"))
        message = self.socket.recv_multipart()
        return message

    def login_database(self, user, passwd):
        self.socket.send_string("dblogin", zmq.SNDMORE)
        request = {"command": "LOGIN", "argument": [user, passwd]}
        self.socket.send(json.dumps(request).encode("utf-8"))
        message = self.socket.recv_multipart()
        rslt = json.loads(message[0])
        assert rslt.get("status", None) == "OK"

    def call_process(self, name, inx=None, reason=None, this=None):
        self.socket.send_string("function", zmq.SNDMORE)
        request = {"name": name, "index": inx, "reason": reason}
        if this is not None:
            request["this"] = this
        elif inx is not None or reason is not None:
            request["this"] = "mqReply_functionArgs"
        self.socket.send(json.dumps(request).encode("utf-8"))
        message = self.socket.recv_multipart()
        rslt = json.loads(message[0])

        if rslt.get("status", None) == "Query":
            # process responds with a query
            # MESSAGEBOX: {'status': 'Query', 'message': '{"command":"information","message":"open_mcv_task","title":"Inform"}'}
            # FILE OPEN: {'status': 'Query', 'message': '{"command":"file_open","directory":"","filter":"MCV.JSON (*.json)","message":"Please upload a file!","title":"Open MCV Data (JSON)"}'}
            return rslt

        assert rslt.get("status", None) == "OK"

    def respond_to_query_file_open(self, data, filename=None):
        self.socket.send_string("file_open", zmq.SNDMORE)

        # build dataURI
        # data must be sent as dataURI, base64 encoded
        # intens doesn't care about the content_type,
        # but it must start with 'application/', 'text/' or 'image/'
        # so, we could just always use i.E. 'application/json'
        try:
            content_type = {
                ".txt": "text/plain",
                ".xml": "application/xml",
                ".json": "application/json",
                ".pdf": "application/pdf",
                ".jpg": "image/jpeg",
                ".jpeg": "image/jpeg",
                ".png": "image/png",
                ".svg": "image/svg+xml",
            }[Path(filename).suffix.lower()]
        except (TypeError, KeyError):
            content_type = "application/json"
        encoded_data = base64.b64encode(data.encode('utf-8'))
        dataURI = f"data:{content_type};base64,{encoded_data.decode()}"

        # prepare request
        request = {"data": dataURI, "type": "stream"}
        if filename is not None:
            request["filename"] = filename

        # process request
        self.socket.send(json.dumps(request).encode("utf-8"))
        message =  self.socket.recv_multipart()
        rslt = json.loads(message[0])
        assert rslt.get("status", None) == "OK"

    def get_value(self, name):
        self.socket.send_string("getvalue", zmq.SNDMORE)
        request = {"varname": name}
        self.socket.send(json.dumps(request).encode("utf-8"))
        message =  self.socket.recv_multipart()
        rslt = json.loads(message[0])
        assert rslt.get("status", None) == "OK"
        return json.loads(message[1])

    def set_value(self, name, value):
        self.socket.send_string("setvalue", zmq.SNDMORE)
        request = {"varname": name, "data": str(value)}
        # print(f"request: {json.dumps(request).encode('utf-8')}")
        self.socket.send(json.dumps(request).encode("utf-8"))
        message = self.socket.recv_multipart()
        # print(f'message: {message}')
        rslt = json.loads(message[0])
        if rslt.get("status", None) != "OK":
            status = rslt.get("status", None)
            message = rslt.get("message", "")
            print(f"set_value({name}, {value}) returned status {status}: {message}")
        assert rslt.get("status", None) == "OK"

    def get_string_value(self, name):
        data = self.get_value(name)
        return data.get("svalue", None)

    def get_real_value(self, name):
        data = self.get_value(name)
        if not data:
            return None
        real = data.get("rvalue", None)
        if real is None:
            real = data.get("ivalue", None)
            if real is not None:
                real = float(real)
        return real

    def get_integer_value(self, name):
        data = self.get_value(name)
        integer = data.get("ivalue", None)
        if integer is not None:
            integer = int(integer)
        return integer

    def get_uimanager(self, name, type_="JSON"):
        self.socket.send_string("uimanager", zmq.SNDMORE)
        request = {"name": name, "type": type_}
        self.socket.send(json.dumps(request).encode("utf-8"))
        message = self.socket.recv_multipart()
        rslt = json.loads(message[0])
        print(f"rslt: {rslt}")

        response = json.loads(message[1])
        # print(f"data: {json.dumps(data, indent=4)}")
        # for element in data.get("element", []):
        #     print(element)
        #     print()
        return response
