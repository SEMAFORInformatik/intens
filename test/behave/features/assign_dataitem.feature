Feature: VALID(array) with vaild array[1] and invalid array[0]
  This Feature tests VALID of an array

  Scenario: Matrix Assign(dest, src)
    When I run assign_task
    Then I get integermatrix value dest.int = [[5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10],[5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10]]
    And I get integermatrix value dest[1].int = [[5, 6, 7, 8, 9, 10], [6, 7, 8, 9, 10, 11], [7, 8, 9, 10, 11, 12], [8, 9, 10, 11, 12, 13], [9, 10, 11, 12, 13, 14], [10, 11, 12, 13, 14, 15]]
    And I get integermatrix value dest[5].int = [[5, 6, 7, 8, 9, 10], [10, 11, 12, 13, 14, 15], [15, 16, 17, 18, 19, 20], [20, 21, 22, 23, 24, 25], [25, 26, 27, 28, 29, 30], [30, 31, 32, 33, 34, 35]]

    And I get integermatrix value dest[6] = None
    And I get realmatrix value dest.real = [4.3, 5.3, 6.3, 7.3, 8.3, 9.3]
    And I get realmatrix value dest[2].real = [8.3, 11.3, 14.3, 17.3, 20.3, 23.3]
    And I get stringmatrix value dest.string = ['S: 160', 'S: 200', 'S: 240', 'S: 280', 'S: 320', 'S: 360']

  Scenario: dest = src
    When I run assign_0_task
    Then I get integermatrix value dest.int = [[5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10],[5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10]]
    And I get integermatrix value dest[1].int = None

  Scenario: Assign(dest[0], src[0])
    When I run assign_1_task
    Then I get integermatrix value dest.int = [[5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10],[5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10]]
    And I get integermatrix value dest[1].int = None

  Scenario: Assign(dest.int, src.int)
    When I run assign_int_task
    Then I get integermatrix value dest.int = [[5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10],[5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10], [5, 6, 7, 8, 9, 10]]
    And I get integermatrix value dest[1].int = None

  Scenario: Assign(dest.real, src.real)
    When I run assign_real_task
    Then I get realmatrix value dest.real = [4.3, 5.3, 6.3, 7.3, 8.3, 9.3]
    Then I get real value dest.real = 4.3
    And I get realmatrix value dest[1].real = None

  Scenario: Assign(dest.string, src.string)
    When I run assign_string_task
    Then I get stringmatrix value dest.string = ['S: 160', 'S: 200', 'S: 240', 'S: 280', 'S: 320', 'S: 360']
    And I get stringmatrix value dest[1].string = None
    And I get integermatrix value dest[1].int = None
