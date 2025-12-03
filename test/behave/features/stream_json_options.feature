Feature: JSON Stream with different stream json options
  This feature tests writing json stream options in Intens

  Scenario: Default Write Json Stream
    Given I run test_json_default_stream_func
    Then I get string value dataInput.transient = "transient"
    AND I get string value dataInput.hidden = None
    AND I get string value dataInput.ok = "ok"

  Scenario: With Hidden Write Json Stream
    Given I run test_json_hidden_stream_func
    Then I get string value dataInput.transient = "transient"
    AND I get string value dataInput.hidden = "hidden"

  Scenario: With No Transient Write Json Stream
    Given I run test_json_no_transient_stream_func
    Then I get string value dataInput.transient = None
    AND I get string value dataInput.hidden = None

