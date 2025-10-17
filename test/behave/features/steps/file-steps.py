#!/usr/bin/env python

from behave import given, when, then
import os
import filecmp
import json


# Compare files
@then("the value of {name} is the content of the file {basename}")
def step_impl(context, name, basename):  # noqa: F811
    value = context.client.get_string_value(name)
    print(f"value of {name} is:\n{value}")

    filename = f"{context.apphome}/{basename}"
    assert os.path.exists(filename)
    print(f"filename: {filename}")
    with open(filename, "r") as f:
        data = f.read()
        print(f"content of {filename} is:\n{data}")
    assert value == data


# compare files in two folders
@then("the files {file} in folder {folder1} and {folder2} are the same")
def step_impl(context, file, folder1, folder2):  # noqa: F811
    assert hasattr(context, "apphome")
    filename1 = f"{context.apphome}/{folder1}/{file}"
    filename2 = f"{context.apphome}/{folder2}/{file}"
    assert os.path.exists(filename1)
    assert os.path.exists(filename2)
    # os.system(f"diff {filename1} {filename2}")
    assert filecmp.cmp(filename1, filename2, False)


# load content of file {file} to motor[navIndex_motor]
@when("I load the file {basename} into Intens")
def step_impl(context, basename):  # noqa: F811
    # todo: replace to variable
    # open prompt to select file
    aswr = context.client.call_process("open_motor_task")

    # TASK open_motor_task calls OPEN(motor_json_fs)
    # instead of showing a file open dialog, a query is returned
    assert aswr["status"] == "Query"

    # (optional) uncomment the following lines for further certainty
    # message = json.loads(aswr["message"])
    # assert message["command"] == "file_open"
    # assert message["directory"] == ""
    # assert message["message"] == "Please upload a file!"
    # assert message["title"] == "Open MOTOR Data (JSON)"
    # assert message["filter"] == "JSON (*.json)"

    # intens reads json-file
    filename = f"{context.apphome}/{basename}"
    with open(filename) as file:
        data = file.read()
    # select file to load in prompt
    context.client.respond_to_query_file_open(data, filename)

# compare original file to all files saved in intens
@then('Intens can return all values from file {basename} with index {index}')
def step_impl(context, basename, index):  # noqa: F811
    filename = f"{context.apphome}/{basename}"
    with open(filename) as file:
        # create python dict from file
        data = json.load(file)
        # get {key}: {value} from every key (non-numeric index)
        for key, value in data.items():
            # motor with index and key is needed to get single return
            query = f"motor[{index}].{key}"

            # print is only shown if a something went wrong
            print("Key:")
            print(key)
            print(type(key))
            print("")
            print("Value:")
            print(value)
            print(type(value))
            print("")
            print("intens.get_value(query):")
            print(context.client.get_value(query))
            print(type(context.client.get_value(query)))
            print("")

            # compare file content with what intens returns
            if type(value) is str:
                assert(value == context.client.get_string_value(query))
            elif type(value) is float:
                assert(value == context.client.get_real_value(query))
            elif type(value) is int:
                assert(value == context.client.get_integer_value(query))
            else:
                # generaly not recommended but in this test case we expect nothing else
                assert(False)
