Feature: Test SCIM

  Scenario: Check Variant Name
     Given SCIM is running
      When project window is opened
       And project 0 is selected
       And variant 0 is selected
      Then string variant.name is "variant 1"

  Scenario: Calculate with Python
     Given SCIM is running
       And project window is opened
       And project 0 is selected
       And variant 0 is selected
      When call function imsc_python_pg
      Then real result.u1 is 378.67, rounded with precision 2
       And the results are as follows with a tolerance of 0.05
	| key	      |	value	       	       | factor |
      	| i2t_pos_max | 8.9 0.5 1.3	       | 1000	|
      	| i2t_neg_max | 0.0 6.6 4.0	       | 1000	|
      	| u1	      | 378.67	  	       | 1	|
      	| f1	      | 57.47		       | 1	|
      	| tpeak	      | 2.4		       | 1000	|
      	| ismin	      | -112.0 -1377.4 -1058.9 | 1	|
      	| ismax	      | 1502.9 430.0 739.2     | 1	|
