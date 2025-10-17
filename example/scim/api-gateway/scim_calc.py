#!/usr/bin/env python
#
# calculate the short circuit characteristics
# of an induction motor
#
import sys
import json
import tractools
import logging

def calculate_sc(parameters):
    m = tractools.InductionMotor(parameters['motor'])
    sc = tractools.Component(parameters['short_circuit'])
    return tractools.imsc(m, sc)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(message)s')
    json.dump(calculate_sc(json.load(sys.stdin)), sys.stdout)
