#!/usr/bin/env python
#
import pytest
import tractools

mpars = dict(
    p=2,
    rs_Tref=0.057623376623,
    rr_Tref=0.1510129,
    Lls=0.8e-3,
    Lh=34.7133758e-3,
    rh=1e5,
    Llr=0.8e-3,
    Iin=1.662)
#    p=2,
#    rs_Tref=39.873e-3,
#    rr_Tref=18.507e-3,
#    Lls=0.251e-3,
#    Lh=8.349e-3,
#    rh=100,
#    Llr=0.265e-3,
#    Iin=2e6)


def test_imsc():
    motor = tractools.InductionMotor(mpars)

    sc = tractools.Component(dict(
        u1=460,
        psi=1.22,
        tload=198.0,
        speed=28.43333,
        tshort=4.1e-3,
        tend=0.1,
        nsamples=200))
    r = tractools.imsc(motor, sc)

    assert len(r['t']) == sc.nsamples
    assert len(r['torque']) == sc.nsamples
    assert len(r['is']) == 3
    assert len(r['i2t_pos']) == 3
    assert len(r['i2t_neg']) == 3
    assert r['u1'] == pytest.approx(454.7854, abs=1e-4)
    assert r['f1'] == pytest.approx(57.671, abs=1e-3)
    assert r['tpeak'] == pytest.approx(4079, abs=1)
    assert r['ismax'] == pytest.approx([1942.9, 292.1, 1082.2], abs=1e-1)
    assert r['ismin'] == pytest.approx([-184.2, -1911.6, -1167.2], abs=1e-1)
    assert r['i2t_pos_max'] == pytest.approx([20755.1, 567.95, 3176.5], abs=1e-1)
    assert r['i2t_neg_max'] == pytest.approx([45.4, 16248.2, 7652.6], abs=1e-1)
