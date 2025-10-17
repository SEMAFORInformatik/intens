%
%  Copyright (c) 2011 Semafor Informatik & Energie AG
%
%  This file is part of tractools.
%
%  tractools is free software; you can redistribute it and/or modify
%  it under the terms of the GNU Lesser General Public License as published by
%  the Free Software Foundation; either version 2.1 of the License, or
%  (at your option) any later version.
%
%  This software is distributed in the hope that it will be useful,
%  but WITHOUT ANY WARRANTY; without even the implied warranty of
%  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%  GNU Lesser General Public License for more details.
%
%  You should have received a copy of the GNU Lesser General Public License
%  along with this software; if not, write to the Free Software
%  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
%
% 
%
% Author: Markus Wildi
% 
%
% inheritence handle: 
% Implementing the BankAccount Class
% ...You would not want independent copies of the object that could 
% have, for example, different values for the account balance. 
% Therefore, the BankAccount class should be implemented as a handle class.
% see: http://www.mathworks.com/help/techdoc/matlab_oop/brhzttf.html

classdef motor < handle

    properties
        rs_Tref;
        rr_Tref;
        Lls;
        Lh;
        rh;
        Llr;
        Iin;
        p;
        psityp;
        psi;% No dynamic instanciation of class attributes
	Tact ;
        Tref ;
        EPS ; % ToDo:  might not be th final place
    end
    methods
        function self= motor( p, psityp, rs_Tref, rr_Tref, Lls, Lh, rh, Llr, Iin )
         self.Tact= 150; 
         self.Tref= 20;  
         self.EPS= 1e-13;

       if  nargin == 1
         self.rs_Tref= p.rs_Tref;
         self.rr_Tref= p.rr_Tref;
         self.Lls    = p.Lls;
         self.Lh     = p.Lh;
         self.rh     = p.rh;
         self.Llr    = p.Llr;
         self.Iin    = p.Iin;
         self.p      = p.p;
         self.psityp = p.psityp;
       elseif nargin == 9
         self.rs_Tref= rs_Tref;
         self.rr_Tref= rr_Tref;
         self.Lls    = Lls;
         self.Lh     = Lh;
         self.rh     = rh;
         self.Llr    = Llr;
         self.Iin    = Iin;
         self.p      = p;
         self.psityp = psityp;
      end
      end

      function val= imag(self, psi)
      % magnetizing current
        val= psi/self.Lh ;
      end

      function val= rrot(self, w)
      % stator resistance
        val= self.rr_Tref*(self.Tact+235)/(self.Tref+235);
      end

      function val= rstat(self, w)
        % stator resistance
        val= self.rs_Tref*(self.Tact+235)/(self.Tref+235);
      end

      function val= lrot(self, w)
      % rotor leakage inductance
        val= self.Llr;
      end

      function val= sigma(self, w)
      % leakage factor
        val=  (1 - self.Lh.^2.0/( ( self.Lh+self.Lls )*( self.Lh+self.Llr ) ) );
      end

      function val= u1(self, w1, psi, wm )
        % stator voltage
        istat=self.i1(w1,psi,wm) ;
        z1=(self.rstat(w1) + w1*self.Lls*j) ;
        val=  w1*psi*j + istat*z1 ;
      end

      function val= i1( self, w1, psi, wm )
        % stator current
        imag=self.imag(psi);
        if abs(w1)>0
	  imag = imag + w1*psi/self.rh*j;
	end
        val=  self.i2(w1,psi,wm) + imag;
      end
      function val= i2( self, w1, psi, wm )
      % rotor current
        w2 = w1 - self.p * wm ;
        if abs(w2)>self.EPS
            z2=(self.rrot(w2) + w2*self.lrot(w2)*j);
            val=  w2*psi*j/z2;
        else
            val=  0;
        end
      end

      function val= upsi( self, psi, u1max, w1, wm )
      % auxiliary function for psi adjustement
        val=  u1max - abs(self.u1(w1, psi, wm ));
      end

      function val= w1torque( self,w1,u1max,psix,wm,tload )
        % check stator voltage
        u1=self.u1(w1, psix,wm);
        self.psi=psix;
        if abs(u1)>u1max % # must adjust flux
           self.psi=fzero(@(psi) self.upsi(psi, u1max,w1,wm), [ psix 0.1*psix]);
        end
        val=  self.torque( w1, self.psi, wm )-tload;
      end

      function val= pullouttorque( self, w1, u1 )
        % pull out torque
        sk=self.sk(w1);
        w2=sk*w1;
        r2=self.rrot(w2);
        l1=self.Lh+self.Lls;
        l2=self.Lh+self.Llr;
        x1=w1*l1;
        x2=w1*l2;
        r1=self.rstat(w1);
        sigma=1 - self.Lh.^2 / ( l1*l2 ); %% POWER
        val=  3*self.p * u1.^2/w1 * (1 - sigma)/ ((r1.^2 + x1.^2)*r2/(sk * x1 * x2) + sk * x2*(r2.^2 + sigma.^2 * x1.^2)/(r2* x1) + 2*r1*(1-sigma));
      end

      function val= sk( self, w1 )
        % pullout slip
        r2=self.rrot(0.);
        x1=w1*(self.Lhh+self.Lls);
        x2=w1*(self.Lh+self.lrot(0.));
        r1=self.rstat(w1);
        sigma=(1 - self.Lh.^2.0/( ( self.Lh+self.Lls )*( self.Lh+self.lrot( 0. ) ) ) );
        val=  r2/x2*math.sqrt( (r1.^2+x1.^2)/(sigma.^2*x1.^2 + r1.^2) );
      end

      function val= torque( self, w1, psi, wm )
        % electric torque (in airgap)
        w2=w1-self.p*wm;
        if w2==0

	  val=  0.;
	else
	    s=w2/w1;
            r2=self.rrot(w2);
            i2=self.i2(w1, psi, wm);
            val=  real(3*self.p/w1/s*r2*(i2*conj(i2)));
	end
      end

      function val= w1( self, u1max, psi, torque, wm )
      % calculate stator frequency with given torque and speed
        if torque>=0 % driving mode
	  sign=1;
        else         % braking mode
	  sign=-1;
	end
        psix=psi;
        wsync=wm*self.p;
        w2=sign*self.rr_Tref/(self.Lls+self.Llr);
	val =fzero(@(w1) self.w1torque(w1, u1max,psix,wm,torque), [wsync wsync+w2]);
      end
    end 
end
