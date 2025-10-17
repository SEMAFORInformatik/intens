Feature: INTENS operations
  This Feature tests the mathematical operations in Intens

  ########################################
  # Basic Calculation
  ########################################

  Scenario Outline: Addition
    When I have value x = <value 1>
    And I have value y = <value 2>
    And I run add
    Then I get value z = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       1 |      1 |
    |       1 |       0 |      1 |
    |       1 |       1 |      2 |
    |      -2 |       1 |     -1 |
    |      -2 |       0 |     -2 |
    |       1 |      "" |      1 |
    |      "" |       1 |      1 |
    |      "" |      "" |   None |

  Scenario Outline: Subtraction
    When I have value x = <value 1>
    And I have value y = <value 2>
    And I run subtract
    Then I get value z = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       1 |     -1 |
    |       1 |       0 |      1 |
    |       1 |       1 |      0 |
    |      -2 |       1 |     -3 |
    |      -2 |       0 |     -2 |
    |       1 |      "" |      1 |
    |      "" |       1 |   None |
    |      "" |      "" |   None |

  Scenario Outline: Multiplication
    When I have value x = <value 1>
    And I have value y = <value 2>
    And I run multiply
    Then I get value z = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       1 |      0 |
    |       1 |       0 |      0 |
    |       1 |       1 |      1 |
    |      -2 |       1 |     -2 |
    |      -2 |       0 |      0 |
    |       1 |      "" |   None |
    |      "" |       1 |   None |
    |      "" |      "" |   None |

  Scenario Outline: Division
    When I have value x = <value 1>
    And I have value y = <value 2>
    And I run divide
    Then I get value z = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       1 |      0 |
    |       1 |       0 |   None |
    |       1 |       1 |      1 |
    |      -2 |       1 |     -2 |
    |      -2 |       0 |   None |
    |       1 |      "" |   None |
    |      "" |       1 |   None |
    |      "" |      "" |   None |

  ########################################
  # Special division and multiplication
  ########################################

  Scenario Outline: Modulus
    When I have value x = <value 1>
    And I have value y = <value 2>
    And I run modulo
    Then I get value z = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       3 |      0 |
    |       1 |       3 |      1 |
    |       3 |       3 |      0 |
    |       3 |       0 |   None |
    |      -2 |       3 |     -2 |
    |      -1 |       3 |     -1 |
    |       1 |      "" |   None |
    |      "" |       3 |   None |
    |      "" |      "" |   None |

  Scenario Outline: Power
    When I have value x = <value 1>
    And I have value y = <value 2>
    And I run power
    Then I get value z = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       1 |      0 |
    |       1 |       0 |      1 |
    |       1 |       1 |      1 |
    |       0 |       2 |      0 |
    |       2 |       0 |      1 |
    |       2 |       2 |      4 |
    |      -2 |       1 |     -2 |
    |      -2 |       0 |      1 |
    |       2 |      -1 |    0.5 |
    |       4 |     0.5 |      2 |
    |       2 |      -2 |   0.25 |
    |       1 |      "" |   None |
    |      "" |       1 |   None |
    |      "" |      "" |   None |

  ########################################
  # Relational Operators
  ########################################

  Scenario Outline: Greater Than
    When I have value i = <value 1>
    And I have value j = <value 2>
    And I run gt
    Then I get value k = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       1 |      0 |
    |       1 |       0 |      1 |
    |       1 |       1 |      0 |
    |      -2 |       1 |      0 |
    |      -2 |       0 |      0 |
    |       1 |      "" |   None |
    |      "" |       1 |   None |
    |      "" |      "" |   None |

  Scenario Outline: Greater or Equal to
    When I have value i = <value 1>
    And I have value j = <value 2>
    And I run geq
    Then I get value k = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       1 |      0 |
    |       1 |       0 |      1 |
    |       1 |       1 |      1 |
    |      -2 |       1 |      0 |
    |      -2 |       0 |      0 |
    |       1 |      "" |   None |
    |      "" |       1 |   None |
    |      "" |      "" |   None |

  Scenario Outline: Less Than
    When I have value i = <value 1>
    And I have value j = <value 2>
    And I run lt
    Then I get value k = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       1 |      1 |
    |       1 |       0 |      0 |
    |       1 |       1 |      0 |
    |      -2 |       1 |      1 |
    |      -2 |       0 |      1 |
    |       1 |      "" |   None |
    |      "" |       1 |   None |
    |      "" |      "" |   None |

  Scenario Outline: Less or Equal to
    When I have value i = <value 1>
    And I have value j = <value 2>
    And I run leq
    Then I get value k = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       1 |      1 |
    |       1 |       0 |      0 |
    |       1 |       1 |      1 |
    |      -2 |       1 |      1 |
    |      -2 |       0 |      1 |
    |       1 |      "" |   None |
    |      "" |       1 |   None |
    |      "" |      "" |   None |

  Scenario Outline: Equal to
    When I have value i = <value 1>
    And I have value j = <value 2>
    And I run eq
    Then I get value k = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       1 |      0 |
    |       1 |       0 |      0 |
    |       1 |       1 |      1 |
    |      -2 |       1 |      0 |
    |      -2 |       0 |      0 |
    |       1 |      "" |      0 |
    |      "" |       1 |      0 |
    |      "" |      "" |      1 |

  Scenario Outline: Not Equal to
    When I have value i = <value 1>
    And I have value j = <value 2>
    And I run neq
    Then I get value k = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       1 |      1 |
    |       1 |       0 |      1 |
    |       1 |       1 |      0 |
    |      -2 |       1 |      1 |
    |      -2 |       0 |      1 |
    |       1 |      "" |      1 |
    |      "" |       1 |      1 |
    |      "" |      "" |      0 |

  ########################################
  # Logical Operators
  ########################################

  Scenario Outline: And
    When I have value i = <value 1>
    And I have value j = <value 2>
    And I run and
    Then I get value k = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       0 |      0 |
    |       1 |       0 |      0 |
    |       0 |       1 |      0 |
    |       1 |       1 |      1 |
    |      -2 |       1 |      1 |
    |      -2 |       0 |      0 |
    |       1 |      "" |      0 |
    |      "" |       1 |      0 |
    |      "" |      "" |      0 |

  Scenario Outline: Or
    When I have value i = <value 1>
    And I have value j = <value 2>
    And I run or
    Then I get value k = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       0 |      0 |
    |       1 |       0 |      1 |
    |       0 |       1 |      1 |
    |       1 |       1 |      1 |
    |      -2 |       1 |      1 |
    |      -2 |       0 |      1 |
    |       1 |      "" |      1 |
    |      "" |       1 |      1 |
    |      "" |      "" |      0 |

  Scenario Outline: Negate
    When I have value i = <value>
    And I run negate
    Then I get value j = <result>

  Examples:
    | value | result |
    |     0 |      0 |
    |     1 |     -1 |
    |    -2 |      2 |
    |    "" |   None |

  Scenario Outline: Not
    When I have value i = <value>
    And I run not
    Then I get value j = <result>

  Examples:
    | value | result |
    |     0 |      1 |
    |     1 |      0 |
    |    -2 |      0 |
    |    "" |      1 |

  ########################################
  # Change Index
  ########################################

  Scenario Outline: Increment
    When I have value i = <value>
    And I run incr
    Then I get value i = <result>

  Examples:
    | value | result |
    |     0 |      1 |
    |     1 |      2 |
    |    -2 |     -1 |
    |    "" |   None |

  Scenario Outline: Decrement
    When I have value i = <value>
    And I run decr
    Then I get value i = <result>

  Examples:
    | value | result |
    |     0 |     -1 |
    |     1 |      0 |
    |    -2 |     -3 |
    |    "" |   None |

  Scenario Outline: Accumulate
    When I have value i = <value 1>
    And I have value j = <value 2>
    And I run accumulate
    Then I get value i = <result>

  Examples:
    | value 1 | value 2 | result |
    |       0 |       1 |      1 |
    |       1 |       0 |      1 |
    |      -2 |       3 |      1 |
    |       1 |      "" |      1 |
    |      "" |       1 |      1 |
    |      "" |      "" |   None |

  ########################################
  # Absolute Value
  ########################################

  Scenario Outline: Absolute Value
    When I have value x = <value>
    And I run abs
    Then I get value y = <result>

  Examples:
    | value   | result |
    | -123.45 | 123.45 |
    |  123.45 | 123.45 |
    |       0 |      0 |
    |      "" |   None |

  ########################################
  # Lenght
  ########################################

  Scenario Outline: Length
    When I have value s = <value>
    And I run length
    Then I get value i = <result>

  Examples:
    | value    | result |
    |       "" |      0 |
    |      " " |      1 |
    | "string" |      6 |
    |    "öäü" |      6 |

  ########################################
  # Trigonometric Functions
  ########################################

  Scenario Outline: Sine
    When I have value x = <value>
    And I run sin
    Then I get value y = <result>

  Examples:
    | value    | result |
    |        0 |      0 |
    |     pi/2 |      1 |
    |       pi |      0 |
    | pi * 1.5 |     -1 |

  Scenario Outline: Cosine
    When I have value x = <value>
    And I run cos
    Then I get value y = <result>

  Examples:
    | value    | result |
    |        0 |      1 |
    |     pi/2 |      0 |
    |       pi |     -1 |
    | pi * 1.5 |      0 |

  Scenario Outline: Tangent
    When I have value x = <value>
    And I run tan
    Then I get value y = <result>

  Examples:
    | value | result |
    |     0 |      0 |
    |  pi/4 |      1 |
    | pi/-4 |     -1 |
    |    pi |      0 |

  Scenario Outline: Arc Sine
    When I have value x = <value>
    And I run asin
    Then I get value y = <result>

  Examples:
    | value | result  |
    |     0 |       0 |
    |     1 |    pi/2 |
    |    -1 | pi/(-2) |

  Scenario Outline: Arc Cosine
    When I have value x = <value>
    And I run acos
    Then I get value y = <result>

  Examples:
    | value | result |
    |     1 |      0 |
    |     0 |   pi/2 |
    |    -1 |     pi |

  Scenario Outline: Arc Tangent (value)
    When I have value x = <value>
    And I run atan
    Then I get value y = <result>

  Examples:
    | value | result  |
    |     0 |       0 |
    |     1 |    pi/4 |
    |    -1 | pi/(-4) |

  Scenario Outline: Arc Tangent (x/y)
    When I have value x = <coordinate 1>
    And I have value y = <coordinate 2>
    And I run atan2
    Then I get value z = <result>

  Examples:
    | coordinate 1 | coordinate 2 | result     |
    |            2 |            0 |          0 |
    |            2 |            2 |       pi/4 |
    |            0 |            2 |       pi/2 |
    |           -2 |            2 |  pi * 0.75 |
    |           -2 |            0 |         pi |
    |           -2 |           -2 | -pi * 0.75 |
    |            0 |           -2 |      -pi/2 |
    |            2 |           -2 |      -pi/4 |

  ########################################
  # Logarithm
  ########################################

  Scenario Outline: Logarithm
    When I have value x = <value>
    And I run log
    Then I get value y = <result>

  Examples:
    | value  | result |
    |      0 |   None |
    |      1 |      0 |
    | math.e |      1 |

  Scenario Outline: Logarithm Base-10
    When I have value x = <value>
    And I run log10
    Then I get value y = <result>

  Examples:
    | value | result |
    |     0 |   None |
    |  1e-5 |     -5 |
    |  1e-3 |     -3 |
    |   0.1 |     -1 |
    |     1 |      0 |
    |    10 |      1 |
    |   100 |      2 |
    |  1e10 |     10 |

  ########################################
  # Root
  ########################################

  Scenario Outline: Square Root
    When I have value x = <value>
    And I run sqrt
    Then I get value y = <result>

  Examples:
    | value | result |
    |     0 |      0 |
    |     1 |      1 |
    |     4 |      2 |
    |     9 |      3 |
    |   100 |     10 |
    |    -1 |   None |

  ########################################
  # Rounding
  ########################################

  Scenario Outline: Round
    When I have value x = <value 1>
    And I have value y = <value 2>
    And I run round
    Then I get value z = <result>

  Examples:
    | value 1 | value 2 | result |
    |      pi |       1 |    3.1 |
    |      pi |       3 |  3.142 |
    |     -pi |       3 | -3.142 |
    |   0.499 |       0 |      0 |
    |     0.5 |       0 |      1 |

  Scenario Outline: Round5
    When I have value x = <value>
    And I run round5
    Then I get value y = <result>

  Examples:
    | value | result |
    |     0 |      0 |
    |     1 |      1 |
    |    pi |   3.15 |
    |  1.22 |    1.2 |
    |  1.23 |   1.25 |
    | -1.22 |   -1.2 |
    | -1.23 |  -1.25 |

  Scenario Outline: Round10
    When I have value x = <value>
    And I run round10
    Then I get value y = <result>

  Examples:
    | value  | result |
    |      0 |      0 |
    |      1 |      1 |
    |     pi |    3.1 |
    |  1.249 |    1.2 |
    |  1.251 |    1.3 |
    | -1.249 |   -1.2 |
    | -1.251 |   -1.3 |

  ########################################
  # Complex values
  ########################################

  Scenario Outline: Real
    When I have a complex number. The real part is <real>. The imaginary part is <imag>
    And I run real
    Then I get value z = <result>

  Examples:
    | real | imag | result |
    |    1 |    2 |      1 |
    |    1 |   "" |   None |
    |   "" |    1 |   None |

  Scenario Outline: Imaginary
    When I have a complex number. The real part is <real>. The imaginary part is <imag>
    And I run imag
    Then I get value z = <result>

  Examples:
    | real | imag | result |
    |    1 |    2 |      2 |
    |    1 |   "" |   None |
    |   "" |    1 |   None |

  Scenario Outline: ARG
    When I have a complex number. The real part is <real>. The imaginary part is <imag>
    And I run arg
    Then I get value z = <result>

  Examples:
    | real | imag | result    |
    |    1 |    0 |         0 |
    |    1 |    1 |      pi/4 |
    |    0 |    1 |      pi/2 |
    |   -1 |    1 | pi * 0.75 |
    |   -1 |    0 |        pi |
