Feature: JSON Stream mit GuiIndex
  This feature tests reading the input in Intens

  Scenario Outline: GuiIndex
    Given I have value index = <index>
    And I run set_navIndex_motor_func
    When I load the file <file> into Intens
    Then Intens can return all values from file <file> with index <index>

  Examples:
    | file        | index |
    | motor1.json |     0 |
    | motor2.json |     1 |
