import zmq
import json


reply_port = 15560
zmq_context = zmq.Context()
socket = zmq_context.socket(zmq.REQ)
socket.connect("tcp://localhost:{}".format(reply_port))

poller = zmq.Poller()
poller.register(socket, zmq.POLLIN)


def hello():
    socket.send_string("control", zmq.SNDMORE)
    request = {"command": "HELLO"}
    socket.send(json.dumps(request).encode("utf-8"))
    message = receive_msg()
    rslt = json.loads(message[0])
    assert rslt.get("status", None) == "OK"


def quit(thread=None):
    socket.send_string("control", zmq.SNDMORE)
    request = {"command": "EXIT"}
    socket.send(json.dumps(request).encode("utf-8"))
    receive_msg()

    if thread is not None:
        # wait for Intens application to finish
        thread.join()


def login_database(user, passwd):
    socket.send_string("dblogin", zmq.SNDMORE)
    request = {"command": "LOGIN", "argument": [user, passwd]}
    socket.send(json.dumps(request).encode("utf-8"))
    message = receive_msg()
    rslt = json.loads(message[0])
    assert rslt.get("status", None) == "OK"


def call_process(name, inx=None, reason=None, this=None):
    socket.send_string("function", zmq.SNDMORE)
    request = {"name": name, "index": inx, "reason": reason}

    # this
    if this is not None:
        request["this"] = this
    elif inx is not None or reason is not None:
        request["this"] = "mqReply_functionArgs"

    socket.send(json.dumps(request).encode("utf-8"))
    message = receive_msg()
    rslt = json.loads(message[0])

    if rslt.get("status", None) == "Query":
        # process responds with a query
        # MESSAGEBOX: {'status': 'Query', 'message': '{"command":"information","message":"open_mcv_task","title":"Inform"}'}
        # FILE OPEN: {'status': 'Query', 'message': '{"command":"file_open","directory":"","filter":"MCV.JSON (*.json)","message":"Please upload a file!","title":"Open MCV Data (JSON)"}'}
        return rslt

    assert rslt.get("status", None) == "OK"


def respond_to_query_file_open(data, filename=None):
    socket.send_string("file_open", zmq.SNDMORE)
    request = {"data": data, "type": "stream"}
    if filename is not None:
        request["filename"] = filename
    socket.send(json.dumps(request).encode("utf-8"))
    message = receive_msg()
    rslt = json.loads(message[0])
    assert rslt.get("status", None) == "OK"


def get_value(name):
    socket.send_string("getvalue", zmq.SNDMORE)
    request = {"varname": name}
    socket.send(json.dumps(request).encode("utf-8"))
    message = receive_msg()
    rslt = json.loads(message[0])
    assert rslt.get("status", None) == "OK"
    return json.loads(message[1])


def set_value(name, value):
    socket.send_string("setvalue", zmq.SNDMORE)
    request = {"varname": name, "data": str(value)}
    socket.send(json.dumps(request).encode("utf-8"))
    message = receive_msg()
    rslt = json.loads(message[0])
    if rslt.get("status", None) != "OK":
        status = rslt.get("status", None)
        message = rslt.get("message", "")
        print(f"set_value({name}, {value}) returned status {status}: {message}")
    assert rslt.get("status", None) == "OK"


def get_string_value(name):
    data = get_value(name)
    return data.get("svalue", None)


def get_real_value(name):
    data = get_value(name)
    if not data:
        return None
    real = data.get("rvalue", None)
    if real is None:
        real = data.get("ivalue", None)
    if real is not None:
        real = float(real)
    return real


def get_integer_value(name):
    data = get_value(name)
    integer = data.get("ivalue", None)
    if integer is not None:
        integer = int(integer)
    return integer


def get_uimanager(name, type_="JSON"):
    socket.send_string("uimanager", zmq.SNDMORE)
    request = {"name": name, "type": type_}
    socket.send(json.dumps(request).encode("utf-8"))
    message = receive_msg()
    rslt = json.loads(message[0])
    print(f"rslt: {rslt}")

    response = json.loads(message[1])
    data = json.loads(response.get("data", "{}"))
    # print(f"data: {json.dumps(data, indent=4)}")
    # for element in data.get("element", []):
    #     print(element)
    #     print()
    return data


def receive_msg():
    socks = dict(poller.poll(2000))
    assert socks

    if socks.get(socket) != zmq.POLLIN:
        assert False

    message = socket.recv_multipart()
    assert message
    return message
