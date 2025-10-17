"""
  Copyright (c) 2011 Semafor Informatik & Energie AG

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
import json
import math
import scipy.optimize

TREF = 20.0


class Component:
    def __init__(self, arg):
        """initialize this object either from a json file
        or from a set of parameters"""
        parameters = arg
        if type(arg) == str:
            file = open(arg)
            parameters = json.load(file)
            file.close()
        for k in parameters.keys():
            self.__setattr__(k, parameters[k])

    def __setattr__(self, name, val):
        self.__dict__[name] = val  # this will create the attribute name

    def to_json(self):
        """return json format of this object as string"""
        return json.dumps(self.__dict__)

    def __str__(self):
        "return string format of this object"
        return repr(self.__dict__)

    def __repr__(self):
        "representation of this object"
        return self.__str__()


class InductionMotor(Component):
    def __init__(self, parameters):
        #        rs_Tref; rr_Tref; Lls; Lh; rh; Llr; Iin; p;
        Component.__init__(self, parameters)
        self.Tact = 150
        self.Tref = TREF

    def imag(self, psi):
        """magnetizing current"""
        return psi / self.Lh

    def rrot(self, w):
        """stator resistance"""
        return self.rr_Tref * (self.Tact + 235) / (self.Tref + 235)

    def rstat(self, w):
        """stator resistance"""
        return self.rs_Tref * (self.Tact + 235) / (self.Tref + 235)

    def lrot(self, w):
        """rotor leakage inductance"""
        return self.Llr

    def lstat(self, w):
        """stator leakage inductance"""
        return self.Lls

    def sigma(self, w):
        """leakage factor"""
        return (1 - self.Lh**2.0 /
                ((self.Lh + self.Lls) * (self.Lh + self.Llr)))

    def lh(self, psi):
        return psi / self.imag(psi)

    def lr(self, w2, psi):
        return self.lh(psi) + self.lrot(w2)

    def ls(self, w1, psi):
        return self.lh(psi) + self.lstat(w1)

    def u1(self, w1, psi, wm):
        """stator voltage"""
        istat = self.i1(w1, psi, wm)
        z1 = (self.rstat(w1) + w1 * self.Lls * 1j)
        return w1 * psi * 1j + istat * z1

    def i1(self, w1, psi, wm):
        """stator current"""
        imag = self.imag(psi)
        if abs(w1) > 0:
            imag += w1 * psi / self.rh * 1j
        return self.i2(w1, psi, wm) + imag

    def i2(self, w1, psi, wm):
        """rotor current"""
        w2 = w1 - self.p * wm
        if abs(w2) > 0:
            z2 = (self.rrot(w2) + w2 * self.lrot(w2) * 1j)
            return w2 * psi * 1j / z2
        return 0

    def inertia(self):
        """rotor inertia"""
        return self.Iin

    def torqueu(self, w1, u1max, psi, wm):
        """calculate motor torque"""
        # check stator voltage
        u1 = self.u1(w1, psi, wm)
        self.psi = psi
        if abs(u1) > u1max:  # must adjust flux
            self.psi = scipy.optimize.bisect(
                lambda psi: u1max - abs(self.u1(w1, psi, wm)),
                psi, 0.01 * psi)
        return self.torque(w1, self.psi, wm)

    def pullouttorque(self, w1, u1):
        """pull out torque"""
        sk = self.sk(w1)
        w2 = sk * w1
        r2 = self.rrot(w2)
        x1 = w1 * (self.Lh + self.Lls)
        x2 = w1 * (self.Lh + self.Llr)
        r1 = self.rstat(w1)
        sigma = self.sigma(w1)
        return 3 * self.p * u1**2 / w1 * (1 - sigma) /\
            ((r1**2 + x1**2) * r2 / (sk * x1 * x2) + sk * x2 *
             (r2**2 + sigma**2 * x1**2) / (r2 * x1) + 2 * r1 * (1 - sigma))

    def sk(self, w1):
        """pullout slip"""
        r2 = self.rrot(0.)
        x1 = w1 * (self.Lh + self.Lls)
        x2 = w1 * (self.Lh + self.lrot(0.))
        r1 = self.rstat(w1)
        return r2 / x2 * math.sqrt((r1**2 + x1**2) / (self.sigma(w1)**2 * x1**2 + r1**2))

    def torque(self, w1, psi, wm):
        """motor torque (in airgap)"""
        w2 = w1 - self.p * wm
        if w2 == 0:
            return 0.
        s = w2 / w1
        r2 = self.rrot(w2)
        i2 = self.i2(w1, psi, wm)
        return 3 * self.p / w1 / s * r2 * (i2 * i2.conjugate()).real

    def w1(self, u1max, psi, tload, wm):
        """calculate stator frequency with given torque and speed"""
        sign = 1       # driving mode
        if tload < 0:  # braking mode
            sign = -1
        wsync = wm * self.p

        return scipy.optimize.bisect(
            lambda w1: self.torqueu(w1, u1max, psi, wm) - tload,
            wsync, wsync * (1 + sign * self.sk(wsync)))
