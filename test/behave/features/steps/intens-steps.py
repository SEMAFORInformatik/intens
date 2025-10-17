#!/usr/bin/env python

from behave import given, when

# recieve response if intens is working
@given("Intens responds")
@when("Intens responds")
def step_impl(context):  # noqa: F811
    context.client.hello(20)

@given("I log in with {username} and {password}")
def step_impl(context, username, password):
    context.client.login_database(username, password)
