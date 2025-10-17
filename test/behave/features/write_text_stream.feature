Feature: Issue 70 - Output Stream REAL Numbers

  Scenario Outline: Intens creates expected files
    When I run test_func
    Then the files <file> in folder reality and expected are the same

  Examples:
    | file   |
    | r.txt  |
    | r.xml  |
    | r.json |
