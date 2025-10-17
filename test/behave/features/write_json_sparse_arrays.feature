Feature: JSON stream writing sparse arrays
  This Feature tests the stream writing in Intens.
  Several JSON-files are created and then compared to the expected outcome.

  Scenario Outline: Compare folders with expected list
    When I run test_func
    Then the files <file> in folder reality and expected are the same

  Examples:
    | file                                    |
    | i/new.json                              |
    | i-indented/new.json                     |
    | data/new.json                           |
    | data-indented/new.json                  |
    | i/one.json                              |
    | i-indented/one.json                     |
    | data/one.json                           |
    | data-indented/one.json                  |
    | i/invalid.json                          |
    | i-indented/invalid.json                 |
    | data/invalid.json                       |
    | data-indented/invalid.json              |
    | i/one_new_one.json                      |
    | i-indented/one_new_one.json             |
    | data/one_new_one.json                   |
    | data-indented/one_new_one.json          |
    | i/one_one_one.json                      |
    | i-indented/one_one_one.json             |
    | data/one_one_one.json                   |
    | data-indented/one_one_one.json          |
    | i/one_invalid_one.json                  |
    | i-indented/one_invalid_one.json         |
    | data/one_invalid_one.json               |
    | data-indented/one_invalid_one.json      |
    | i/new_new_new_new_one.json              |
    | i-indented/new_new_new_new_one.json     |
    | data/new_new_new_new_one.json           |
    | data-indented/new_new_new_new_one.json  |
