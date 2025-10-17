#!/usr/bin/env python
#
# Standard API Gateway for Intens Applications
#
import logging
import logging.config
import zmq
import zmq.log.handlers

logger = None


def set_zmq_log_format(handler):
    """The format for the logger in PUBHandler can not be set over formatters
       It is hard coded: https://github.com/zeromq/pyzmq/blob/master/zmq/log/handlers.py
       Here we use html to show color in the intens application.
    """
    handler.formatters[logging.DEBUG] = logging.Formatter(
        '<font>[%(levelname)s] %(name)s - %(filename)s:%(lineno)d: %(message)s</font><br />')
    handler.formatters[logging.INFO] = logging.Formatter(
        '<font style="color:green;">[%(levelname)s] %(name)s - %(filename)s:%(lineno)d: %(message)s</font><br />')
    handler.formatters[logging.WARN] = logging.Formatter(
        '<font style="color:orange;">[%(levelname)s] %(name)s - %(filename)s:%(lineno)d: %(message)s</font><br />')
    handler.formatters[logging.ERROR] = logging.Formatter(
        '<font style="color:red;">[%(levelname)s] %(name)s - %(filename)s:%(lineno)d: %(message)s - %(exc_info)s</font><br />')
    handler.formatters[logging.CRITICAL] = logging.Formatter(
        '<font style="color:red;font-weight:bold;">[%(levelname)s] %(name)s - %(filename)s:%(lineno)d: %(message)s</font><br />')


def create_logger(name, port):
    #FORMAT = "%(asctime)s %(levelname)s: %(message)s"
    #    logging.basicConfig(level=logging.INFO, format=FORMAT)
    logger = logging.getLogger(name)  # Get root logger
    if port:
        zmqH = zmq.log.handlers.PUBHandler(f'tcp://*:{port}')
        # Important set HEADER for ZMQ
        # The full header will be
        #   python_log.INFO, python_log.ERROR or python_log.DEBUG
        zmqH.root_topic = "python_log"
        set_zmq_log_format(zmqH)
        # add this handler to root logger
        logging.getLogger('').addHandler(zmqH)

    return logger
