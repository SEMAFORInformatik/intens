Feature: unit management
  This Feature tests the scaling of values

  Scenario: automatic scaling
    Given I have value motor.p = 2
    And I have value motor.psityp = 1
    And I have value motor.rs_Tref = 0.1
    And I have value motor.Lls = 0.001
    Then I get value p = 2
    And in fieldgroup motor_properties_fg I get value motor[0].Lls[0] = 1
