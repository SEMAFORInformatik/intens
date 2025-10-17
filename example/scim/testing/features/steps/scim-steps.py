import intens.intensZMQ as intens
from behave import given, when, then
import json
import math


def get_string(s):
    if len(s) > 1:
        if s[0] == '"' and s[-1] == '"':
            s = s[:-1][1:]
    return s.strip()


@given("SCIM is running")
def step_impl(context):
    intens.hello()


@when("database is open")
def step_open(context):
    intens.login_database("james", "bond")


@when("call function {name}")
def step_function(context, name):
    intens.call_process(get_string(name))


@given("project window is opened")
@when("project window is opened")
def step_open_project(context):
    intens.call_process("project_open_pressed_func")


@given("project {index} is selected")
@when("project {index} is selected")
def step_select_project(context, index):
    intens.call_process("project_select_list_func", get_string(index), "ACTIVATE")


@given("variant {index} is selected")
@when("variant {index} is selected")
def step_select_variant(context, index):
    intens.call_process("variant_select_list_func", get_string(index), "ACTIVATE")


@then("string {name} is {value}")
def step_test_string_value(context, name, value):
    rslt = intens.get_string_value(name)
    assert rslt == get_string(value)


@then("real {name} is {value}, rounded with precision {prec}")
def step_test_real_value(context, name, value, prec):
    r = intens.get_real_value(name)
    p = int(get_string(prec))
    v = float(get_string(value))
    assert round(r, p) == round(v, p)


@then("the results are as follows with a tolerance of {val:f}")
def step_test_results(context, val):
    s = intens.get_string_value("result")
    result = json.loads(s)[0]
    for row in context.table:
        lrow = []
        for item in row["value"].split():
            lrow.append(float(item) * int(row["factor"]))
        for i in range(len(lrow)):
            if lrow[i] == 0:
                assert val * float(row["factor"]) > result[row["key"]][i]
            else:
                assert math.isclose(
                    result[row["key"]][i], lrow[i], abs_tol=val * float(row["factor"])
                )
