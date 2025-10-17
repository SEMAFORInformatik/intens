import zrpc
import logging
import signal
from config import Config

app = zrpc.create_app("apigw")


@zrpc.route("hello_world")
def hello_world(request):
    app.logger.info("Say Hello to %s", request)
    return [{"status": "ok", "message": f"Hello {request}"}]


@zrpc.route("scimcalc")
def scimcalc(request):
    try:
        import scim_calc

        return [
            {"status": "ok", "message": "successfully completed"},
            scim_calc.calculate_sc(request),
        ]
    except Exception as e:
        app.logger.error(e, exc_info=True)
        return [{"status": "error", "message": str(e)}]

@zrpc.route("report")
def report(request):
    import tractools.repgen
    pmMotor = request.get("motor", {})
    shortCircuit = request.get("short_circuit", {})
    result = request.get("result", [{}])[0]
    try:
        doc = tractools.repgen.main(
            pmMotor,
            shortCircuit,
            result)
        return [{"status": "ok"}, doc]
    except Exception:
        import traceback
        return [{
            "status": "error",
            "message": "<h3>Incomplete Report data</h3><p>{}</p>".format(
                traceback.format_exc().replace("\\n", "<br/>"))},
            {}]
    return [{"status": "info", "message": "empty document"}, {}]


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
