Feature: VALID(array) with vaild array[1] and invalid array[0]
  This Feature tests VALID of an array

  Scenario: array is invalid
    When I run valid_task
    Then I get value valid = 0

  Scenario: array[1] is valid
    When I have value array[1] = 1
    And  I run valid_task
    Then I get value valid = 0

  Scenario: array[0] is valid
    When I have value array[0] = 1
    And  I run valid_task
    Then I get value valid = 1

  Scenario: array[0] and array[1] are valid
    When I have value array[0] = 1
    And  I have value array[1] = 1
    And  I run valid_task
    Then I get value valid = 1