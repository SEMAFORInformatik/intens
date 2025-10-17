#!/usr/bin/env python
#
# ZMQ Server
#
import json
import sys
import zmq
import traceback
import threading
import os
import logging
from .apilog import create_logger


BACKEND_URL = 'inproc://backend'


def write_json(pars, funcname, logdir):
    """write pars to json if logdir"""
    if logdir:
        with open(os.path.join(logdir,
                               '{}.json'.format(funcname)), 'w') as f:
            json.dump(pars, f)


class BackendWorkerTask(threading.Thread):
    """invoke workerfunc with arguments from broker
    and send result back to broker
    """

    def __init__(self, workerfunc):
        self.workerfunc = workerfunc
        threading.Thread.__init__(self)
        self.logger = logging.getLogger('zrpc')

    def run(self):
        socket = zmq.Context().instance().socket(zmq.REP)
        socket.connect(BACKEND_URL)
        request = socket.recv_multipart()
        if isinstance(request, list):
            args = list()
            for r in request:
                try:
                    args.append(json.loads(r.decode()))
                except:
                    args.append(r)
            # args = [json.loads(r.decode()) for r in request]
        else:
            args = [json.loads(request.decode())]
        try:
            response = self.workerfunc(*args)
        except Exception:
            e_type, e_obj, e_tb = sys.exc_info()
            response = [{"status": "error",
                        "message": traceback.format_exc()}]
        socket.send_multipart([json.dumps(r).encode() if type(r) != bytes else r
                               for r in response])
        socket.close()


class Service(object):
    functions = dict()

    def __init__(self, name):
        self.name = name
        self.logger = logging.getLogger('zrpc')
        # add functions of module defroutes
        from . import defroutes
        for f in dir(defroutes):
            if callable(defroutes.__dict__[f]) and not f.startswith('__'):
                self.functions[f] = defroutes.__dict__[f]

    def create_logger(self, logport):
        self.logger = create_logger(self.name, logport)

    def _create_sockets(self, port, ctl_port):
        context = zmq.Context.instance()
        self.frontend = context.socket(zmq.ROUTER)
        self.backend = context.socket(zmq.REQ)
        if port:
            self.frontend.bind(f'tcp://*:{port}')
        else:  # use unix domain socket
            self.frontend.bind(f'ipc:///tmp/{self.name}')

        self.backend.bind(BACKEND_URL)
        self.poller = zmq.Poller()
        self.poller.register(self.frontend, zmq.POLLIN)
        self.poller.register(self.backend, zmq.POLLIN)
        if ctl_port:
            self.controller_url = f'tcp://*:{ctl_port}'
        else:
            self.controller_url = 'inproc://zrpc-ctl'

        self.controller = context.socket(zmq.PULL)
        self.controller.bind(self.controller_url)
        self.poller.register(self.controller, zmq.POLLIN)

    def stop(self):
        self.logger.info("stop server %s", self.controller_url)
        if self.controller_url:
            socket = zmq.Context.instance().socket(zmq.PUSH)
            socket.connect(self.controller_url.replace('*', 'localhost'))
            socket.send(b"quit")
            socket.close()
        #super(ZmqServer, self).stop()

    def run(self, port, ctl_port=0, logdir=''):
        self._create_sockets(port, ctl_port)
        self.logger.info("ZMQ RPC service '{}' ready on Port: {}"
                         .format(self.name, port))
        worker = None
        funcname = ''
        while True:
            socks = dict(self.poller.poll())
            if socks.get(self.frontend) == zmq.POLLIN:
                # read message from client
                try:
                    message = self.frontend.recv_multipart()
                    client = message[0]
                    self.logger.debug(message)
                    if len(message) > 2:  # we need a funcname at least
                        funcname = message[2].decode()
                        if funcname not in ['autolog']:
                            self.logger.debug(
                                "INFO Receive funcname %s", funcname)
                        request = list()
                        for r in message[3:]:
                            try:
                                request.append(json.loads(r.decode()))
                            except:
                                request.append(r)
                        # request = [json.loads(r.decode())
                        #            for r in message[3:]]
                        write_json(request, funcname, logdir)
                        if worker:
                            self.logger.warning("worker not yet finished, maybe terminated")
                            continue
                        # delegate function call to worker
                        worker = BackendWorkerTask(self.functions[funcname])
                        worker.start()
                        self.backend.send_multipart(message[3:])
                    else:
                        self.logger.warning('empty request')
                        self.frontend.send_multipart(message) # TODO echo response?
                        funcname = ''
                except KeyError as e:
                    self.logger.error(e, exc_info=True)
                    response = {"status": "error",
                                "message": "no such function {} in {}"
                                .format(funcname, self.get_routes())}
                    self.frontend.send_multipart([message[0], b'',
                                                  json.dumps(response).encode()])

            if socks.get(self.backend) == zmq.POLLIN:
                # receive function results and send them to frontend
                response = self.backend.recv_multipart()
                worker.join()
                worker = None
                self.frontend.send_multipart([client, b''] + response)
                if funcname not in ['autolog']:
                    self.logger.debug("%s: %s",
                                     funcname,
                                     [r.decode() if len(str(r)) < 700 else f'msglen={len(str(r))}'
                                      for r in response])
                funcname = ''

            if socks.get(self.controller) == zmq.POLLIN:
                message = self.controller.recv()
                if message == b'quit':
                    self.logger.debug("stop zmq rpc service")
                    break  # finish message processing
                if message == b'interrupt' and funcname:
                    # attempt to interrupt funcname
                    try:
                        r = self.functions['terminate_calc'](funcname)
                        self.logger.info('interrupt :%s', r)
                    except KeyError:
                        self.logger.warning('interrupt ignored')

        self.controller.close()
        self.frontend.close()
        self.backend.close()
        #zmq.Context.instance().term()
        self.logger.info(f"ZMQ RPC service {self.name} terminated.")

    def get_routes(self):
        return [k for k in self.functions.keys()]
