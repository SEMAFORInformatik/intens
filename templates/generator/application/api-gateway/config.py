import os
import pathlib

name = 'api-gateway '

def vcs_info():
     g = pathlib.Path(__file__).parent / 'vcs.info'
     if g.exists():
         return name + g.read_text().strip()
     import subprocess
     try:
         p = subprocess.run(['git', 'describe'], capture_output=True)
         if p.returncode==0:
             return name + p.stdout.decode().strip()
     except:
         pass
     return name.strip()

class Config(object):
    VCS_INFO = vcs_info()
    gw_port = int(os.environ.get(
        'API_GATEWAY_PORT', 15558))
    logger_port = int(os.environ.get(
        'API_GATEWAY_LOGGER_PORT', 15574))
    otel_endpoint = os.environ.get("OTEL_EXPORTER_OTLP_TRACES_ENDPOINT", "")

