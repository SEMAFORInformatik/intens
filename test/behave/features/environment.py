from behave import fixture, use_fixture
from pathlib import Path
import intens.client
import subprocess
import threading
import logging

@fixture
def start_intens(context, apphome):
    # -- SETUP-FIXTURE PART: use generator function
    replyport = '15560'
    def run():
        args = [apphome / 'start.sh', replyport]
        ret = subprocess.run(args,
                             check=True, capture_output=False)
    thread = threading.Thread(target=run)
    thread.start()
    context.thread = thread
    context.apphome = apphome
    context.client = intens.client.Client(port=replyport)
    yield thread
    thread.join()


def before_feature(context, feature):
    f = Path(feature.filename)
    fdir = Path(f'./features/{f.stem}')
    #test
    if fdir.is_dir():
        use_fixture(start_intens, context, fdir.absolute())


def after_feature(context, feature):
    context.client.quit()
