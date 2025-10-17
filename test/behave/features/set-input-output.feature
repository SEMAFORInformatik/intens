Feature: Issue 162 - SET Input and Output values
  This feature tests the SET Output Values set by Intens.

  Scenario: Test Input and Output values of SET gb_int_set
    Then I get string value gb_int_set.Input[0] = "Good"
    Then I get string value gb_int_set.Input[1] = "Bad"
    Then I get string value gb_int_set.Input[2] = " "
    Then I get string value gb_int_set.Input[3] = None
    Then I get string value gb_int_set.Output[0] = "1"
    Then I get string value gb_int_set.Output[1] = "0"
    Then I get string value gb_int_set.Output[2] = None

  Scenario: Test Input and Output values of SET gb_string_set
    Then I get string value gb_string_set.Input[0] = "Good"
    Then I get string value gb_string_set.Input[1] = "Bad"
    Then I get string value gb_string_set.Input[2] = " "
    Then I get string value gb_string_set.Input[3] = None
    Then I get string value gb_string_set.Output[0] = "1"
    Then I get string value gb_string_set.Output[1] = "0"
    Then I get string value gb_string_set.Output[2] = None

  Scenario: Test Input and Output values of SET gb_int_withoutInvalid_set
    Then I get string value gb_int_withoutInvalid_set.Input[0] = "Good"
    Then I get string value gb_int_withoutInvalid_set.Input[1] = "Bad"
    Then I get string value gb_int_withoutInvalid_set.Input[2] = None
    Then I get string value gb_int_withoutInvalid_set.Output[0] = "1"
    Then I get string value gb_int_withoutInvalid_set.Output[1] = "0"
    Then I get string value gb_int_withoutInvalid_set.Output[2] = None

  Scenario: Test Input and Output values of SET gb_string_withoutInvalid_set
    Then I get string value gb_string_withoutInvalid_set.Input[0] = "Good"
    Then I get string value gb_string_withoutInvalid_set.Input[1] = "Bad"
    Then I get string value gb_string_withoutInvalid_set.Input[2] = None
    Then I get string value gb_string_withoutInvalid_set.Output[0] = "1"
    Then I get string value gb_string_withoutInvalid_set.Output[1] = "0"
    Then I get string value gb_string_withoutInvalid_set.Output[2] = None

  Scenario: Test Input and Output values of SET gb_int_default_set
    Then I get string value gb_int_default_set.Input[0] = "Bad"
    Then I get string value gb_int_default_set.Input[1] = "Good"
    Then I get string value gb_int_default_set.Input[2] = " "
    Then I get string value gb_int_default_set.Input[3] = None
    Then I get string value gb_int_default_set.Output[0] = "0"
    Then I get string value gb_int_default_set.Output[1] = "1"
    Then I get string value gb_int_default_set.Output[2] = None

  Scenario: Test Input and Output values of SET gb_string_default_set
    Then I get string value gb_string_default_set.Input[0] = "Bad"
    Then I get string value gb_string_default_set.Input[1] = "Good"
    Then I get string value gb_string_default_set.Input[2] = " "
    Then I get string value gb_string_default_set.Input[3] = None
    Then I get string value gb_string_default_set.Output[0] = "0"
    Then I get string value gb_string_default_set.Output[1] = "1"
    Then I get string value gb_string_default_set.Output[2] = None
