import zrpc
import logging
import signal
from config import Config

app = zrpc.create_app("apigw")


@zrpc.route("hello_world")
def hello_world(request):
    app.logger.info("Say Hello to %s", request)
    return [{"status": "ok", "message": f"Hello {request}"}]


# new functions here


def exit_gracefully(signum, frame):
    app.logger.info("Exiting..")
    app.stop()


if __name__ == "__main__":

    FORMAT = "%(asctime)s %(levelname)s: %(message)s"
    logging.basicConfig(level=logging.INFO, format=FORMAT)

    signal.signal(signal.SIGINT, exit_gracefully)
    signal.signal(signal.SIGTERM, exit_gracefully)

    app.create_logger(Config.logger_port)
    app.run(Config.gw_port)
