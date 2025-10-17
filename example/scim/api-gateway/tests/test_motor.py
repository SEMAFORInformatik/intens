import pytest
import tractools
import math
import os

mpars = dict(
    p=2,
    rs_Tref=39.873e-3,
    rr_Tref=18.507e-3,
    Lls=0.251e-3,
    Lh=8.349e-3,
    rh=100,
    Llr=0.265e-3,
    Iin=2e6)


def test_loadjson():
    motor = tractools.InductionMotor(
        f'{os.path.dirname(__file__)}/motor.json')
    assert motor.p == mpars['p']


def test_w1calc():
    motor = tractools.InductionMotor(mpars)
    torque = 198
    speed = 1705 / 60.
    wm = 2 * math.pi * speed
    u1 = 460 / math.sqrt(3.)
    psi = u1 / (2 * math.pi * 60)

    w1 = motor.w1(u1, psi, torque, wm)
    assert w1 == pytest.approx(358.9529)
