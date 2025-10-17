"""
  Copyright (c) 2011-2025 Semafor Informatik & Energie AG

  This file is part of tractools.

  tractools is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
"""
import scipy.integrate as it
import numpy as np
import numpy.linalg as la


def K(d):
    """space phasor transformation matrix
    arguments:
      d: rotation angle

    returns transformation matrix"""
    return np.array(((np.cos(d), np.sin(d)),
                     (np.cos(d - 2 * np.pi / 3), np.sin(d - 2 * np.pi / 3)),
                     (np.cos(d + 2 * np.pi / 3), np.sin(d + 2 * np.pi / 3))))


def torque(p, lmd, Y):
    """calculate airgap torque from flux

    arguments:
      p number of pole pairs
      lmd lm /(ls*lr-lm**2)
      Y vector of fluxes
    """
    return 3 * p * lmd / 2 * (Y[0] * Y[3] - Y[1] * Y[2])


def imsc(motor, sc):
    """calculate short circuit characteristics

    arguments:
      motor  motor object
        psityp type flux
        p number of pole pairs
        rs_Tref stator resistance at 20 deg Celsius
        rr_Tref rotor resistance at 20 deg Celsius
        Lls stator leakage inductance
        Lh magentizing inductance
        rh resistance representing iron losses
        Llr rotor leakage inductance
        Iin inertia torque

      sc     short circuit specification object with
        u1  stator maximum voltage
        psi stator maximum flux
        tload load torque
        speed shaft speed at load torque
        tshort start time of short circuit
        tend   end time of calculation period
        nsamples number of samples

    returns result dict:
     t      time vector
     torque torque vector
     is     3 phase currents matrix
     i2t_pos matrix of integrated i**2(t) for positive values
     i2t_neg matrix of integrated i**2(t) for negative values
     ismax  vector of maximum values of phase currents
     ismin  vector of minimum values of phase currents
     tpeak  peak value of torque
     i2t_pos_max vector of maximum values of positive i2t
     i2t_neg_max vector of maximum values of negative i2t
     u1    stator voltage at begin of short circuit
     f1    stator frequency at begin of short circuit
    """
    wm = 2 * np.pi * sc.speed
    # find stator frequency and voltage
    w1 = motor.w1(sc.u1, sc.psi, sc.tload, wm)
    u1 = motor.u1(w1, motor.psi, wm)

    w2 = w1 - motor.p * wm
    rs = motor.rstat(w1)
    rr = motor.rrot(w2)
    lm = motor.lh(motor.psi)
    ls = motor.ls(w1, motor.psi)
    lr = motor.lr(0, motor.psi)
    D = (ls * lr - lm**2)

    lsd = ls / D
    lmd = lm / D
    lrd = lr / D

    a = rs * lrd
    b = rs * lmd
    c = rr * lmd
    d = rr * lsd

    wk = w1
    uq = np.sqrt(2) * abs(u1)
    ud = 0

    # solve voltage equations for steady state
    A = np.array((
        (a, wk, -b, 0),
        (-wk, a, 0, -b),
        (-c, 0, d, wk - motor.p * wm),
        (0, -c, motor.p * wm - wk, d)))
    Y0 = la.solve(A, np.array(((uq), (ud), (0), (0))))

    # set initial condition for flux and speed
    Y0 = np.append(np.array(Y0), [wm])

    # solve differential equations for flux and speed
    t = np.linspace(0, sc.tend, sc.nsamples)
    uq = 0
    ud = 0
    y = it.odeint(lambda y, t: (
        # build time-derivatives of flux and speed
        # y[0]     y[1]   y[2]    y[3]    y[4]
        # psi_sq  psi_sd  psi_rq  psi_rd   wm
        uq - a * y[0] + b * y[2] - wk * y[1],
        ud - a * y[1] + b * y[3] + wk * y[0],
        c * y[0] - (wk - motor.p * y[4]) * y[3] - d * y[2],
        c * y[1] + (wk - motor.p * y[4]) * y[2] - d * y[3],
        (torque(motor.p, lmd, y) - sc.tload) / motor.inertia()),
                   Y0, t)

    result = {}
    result['torque'] = [torque(motor.p, lmd, yy) for yy in y]

    Is = np.array([np.dot(np.array(((lrd, lrd, -lmd, -lmd),
                                    (-lmd, -lmd, lsd, lsd))), yy)
                  for yy in y[:, 0:4]])
    istat = np.array([np.dot(K(w1 * x[0]), x[1]) for x in zip(t, Is)]).T

    result['t'] = t.tolist()
    # Cumulatively integrate i**2(t) for positive values is
    result['i2t_pos'] = [
        it.cumulative_trapezoid([i**2 if i > 0 else 0
                                 for i in istat[k]], x=t).tolist()
        for k in range(3)
    ]
    # Cumulatively integrate i**2(t) for negative values is
    result['i2t_neg'] = [
        it.cumulative_trapezoid([i**2 if i < 0 else 0
                                 for i in istat[k]], x=t).tolist()
        for k in range(3)
    ]
    result['is'] = istat.tolist()
    result['ismax'] = [max(x) for x in result['is']]
    result['ismin'] = [min(x) for x in result['is']]
    result['tpeak'] = max([abs(x) for x in result['torque']])
    result['i2t_pos_max'] = [max(x) for x in result['i2t_pos']]
    result['i2t_neg_max'] = [max(x) for x in result['i2t_neg']]

    result['u1'] = abs(u1)
    result['f1'] = w1 / (2 * np.pi)
    return result
