Feature: Issue 72 - Plot2D

  Scenario Outline: Running Intens
    Given I have value unit = <unit>
    And I run unit_func
    Then xy_plot XAxis title = <title>


  Examples:
    | unit  | title   |
    |     0 | degrees |
    |     1 | radians |
    |     2 |       n |
