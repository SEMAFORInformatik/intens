#!/usr/bin/env python

from behave import when, then
import pytest
import math  # noqa: F401 (math needed for pi)
import logging

########################################
# Set values
########################################

# set one value // calculation
@given('I have value {name} = {value}')
@when('I have value {name} = {value}')
def step_impl(context, name, value):
    logging.info("name %s value %s", name, value)
    # replace string pi with number pi
    value = value.replace("pi", "math.pi")

    # set value and calculate string
    context.client.set_value(name, eval(value))


# set a complex number
@when('I have a complex number. The real part is {real}. The imaginary part is {imag}')
def step_impl(context, real, imag):  # noqa: F811
    # replace pseudo empty string with empty string
    real = real.replace('""', '')
    imag = imag.replace('""', '')

    # set value
    context.client.set_value("x", real)
    context.client.set_value("y", imag)


########################################
# Call function
########################################

# call a given function
@given('I run {function}')
@when('I run {function}')
def step_impl(context, function):  # noqa: F811
    # run function
    context.client.call_process(function)


########################################
# Get values
########################################

# compare return with expected value
@then('I get {data_type} value {name} = {result}')
@then('I get value {name} = {result}')
def step_impl(context, name, result, data_type="real"):  # noqa: F811
    # print(result)
    # print(type(result))

    # replace string pi with number pi
    result = result.replace("pi", "math.pi")

    # get calculated answer from Intens
    action = getattr(context.client, f"get_{data_type}_value")
    answ = action(name)

    # "None" > None
    # math.pi > number
    # "string" > string
    result = eval(result)

    # compare values
    try:
        assert result == pytest.approx(answ)
    except AssertionError:
        import json
        print(f"Expected {json.dumps(result)}")
        print(f"Received {json.dumps(answ)}")
        raise
