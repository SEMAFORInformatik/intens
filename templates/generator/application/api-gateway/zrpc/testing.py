import zmq
import json
from .app import Service
import threading


class TestClient(object):
    def __init__(self):
        self.service = Service('zrpc-test')
        self.socket = zmq.Context().instance().socket(zmq.REQ)
        for f in self.service.get_routes():
            self._add_method(f)
        # start service using ipc socket
        self.thread = threading.Thread(target=self.service.run, args=(0,))
        self.thread.start()
        self.socket.connect(f"ipc:///tmp/{self.service.name}")

    def _add_method(self, f):
        def wrapper(arg):
            return self.call_method(f, arg)
        setattr(self, f, wrapper)

    def call_method(self, name, arg):
        self.socket.send_multipart([name.encode(),
                                    json.dumps(arg).encode()])

        return self.socket.recv_multipart()

    def stop_server(self):
        self.service.stop()


def test_client():
    return TestClient()
