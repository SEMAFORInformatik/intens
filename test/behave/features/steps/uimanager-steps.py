#!/usr/bin/env python
import logging
from behave import then

@then('{ui_element:S} {axis:S} title = {title:S}')
def step_impl(context, ui_element, axis, title):
    data = context.client.get_uimanager(ui_element)
    assert(data[axis]['title'].strip() == title)


@then('in fieldgroup {ui_element:S} I get value {name:S} = {value}')
def step_impl(context, ui_element, name, value):
    actual = None
    data = context.client.get_uimanager(ui_element)
    for l in data['element']:
        for e in l:
            if 'fullName' in e:
                if e['fullName'] == name:
                    logging.info("%s", e)
                    actual = e['formatted_value']
                    break
    assert(actual == value)
