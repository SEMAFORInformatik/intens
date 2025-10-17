#!/usr/bin/env python
#
# ZMQ RPC Server
#
from .app import Service
from .testing import test_client

from inspect import signature
from config import Config
from opentelemetry import trace
from opentelemetry.trace.propagation.tracecontext import TraceContextTextMapPropagator

tracer = trace.get_tracer("calculate")


def create_app(name='apigw'):
    global app
    app = Service(name)
    return app


def route(name):
    def decorator(f):
        def otel_call(*args):
            sig = signature(f)
            # Check if the function takes a span
            takes_span = len(sig.parameters) == len(args)

            metadata = args[-1]
            # Get the context from the request's metadata
            ctx = TraceContextTextMapPropagator().extract(carrier=metadata)
            with tracer.start_as_current_span(name, context=ctx, kind=trace.SpanKind.SERVER) as span:
                app.logger.info(
                    'Calling method "%s" with arguments: %s', name, args)

                if takes_span:
                    return f(*args[:-1], span)
                else:
                    return f(*args[:-1])

        if not Config.otel_endpoint or name == 'terminate_calc':
            app.functions[name] = f
        else:
            app.functions[name] = otel_call
        return f
    return decorator


def run(port=15558):
    app.run(port)


def stop():
    app.stop()
