import steps.intens.intensZMQ as intens
import os
from pathlib import Path
import threading


def before_scenario(context, scenario):
    # check apphome and keep it in context
    apphome = "../build"
    assert Path(apphome).is_dir()
    context.apphome = apphome

    # build start command
    command = f"env INTENS_REPLY_PORT={intens.reply_port} {apphome}/scripts/${application}"

    # start Intens application in a thread
    # to allow quit to join the thread
    # to wait for the Intens application to finish
    thread = threading.Thread(target=os.system, args=(command,))
    thread.start()
    context.thread = thread

    # check that intens is running
    intens.hello()

    # login to the database
    intens.login_database("james", "bond")


def after_scenario(context, scenario):
    intens.quit(context.thread)
