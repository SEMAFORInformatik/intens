Feature: constistency check
  This Feature tests the constistency check

  Scenario Outline: automatic constistency check A
    Given I run setValues
    And I run <func>
    And I have value <var> = 12
    Then I get value output = <result>

  Examples:
    | func           | var      | result |
    | mq_input1_func | input2mq |      0 |
    | mq_input2_func | input1mq |      5 |
    | pg_input1_func | input1mq |     10 |
    | pg_input2_func | input2mq |     15 |
    | mq_input1_func | input1mq |   None |
    | mq_input2_func | input2mq |   None |
    | pg_input1_func | input1pg |   None |
    | pg_input2_func | input2pg |   None |

  Scenario Outline: automatic constistency check B
    Given I run setValues
    And I run <func>
    And I run <func2>
    And I have value <var> = 12
    Then I get value output = <result>

  Examples:
    | func           | func2          | var      | result |
    | mq_input1_func | mq_input2_func | input1mq |   5    |
    | mq_input2_func | mq_input1_func | input2mq |   0    |
    | mq_input1_func | mq_input2_func | input2mq |   None |
    | mq_input2_func | mq_input1_func | input1mq |   None |

    | pg_input1_func | pg_input2_func | input1pg |   15   |
    | pg_input2_func | pg_input1_func | input2pg |   10   |
    | pg_input1_func | pg_input2_func | input2pg |   None |
    | pg_input2_func | pg_input1_func | input1pg |   None |
