import zmq
import json
import os

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind(f"tcp://*:{os.getenv('INTENS_REPLY_B_PORT')}")
print(f"bind: tcp://*:{os.getenv('INTENS_REPLY_B_PORT')}")

while True:
    print(f"wait")
    header = socket.recv_string()
    print(f"recv: {header}")
    if (header == 'quit'):
        exit(0)
    data = socket.recv_string()
    if header == 'output':
        socket.send_string(json.dumps(dict(status='Ok')), zmq.SNDMORE)
        socket.send_string(data)
