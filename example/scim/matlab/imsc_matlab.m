function [result] = imsc_matlab( m, sc )
% IMSC Calculate short circuit characteristics of induction motor
%    imsc(motor, sc) where 
  %    m is object of type motor (or structure)
%    sc is structure with fields
%    - nsamples : number of samples
%    - u1 : stator voltage
%    - u1 : stator voltage
%    - psi : stator flux
%    - tload : load torque 
%    - speed : shaft speed
%    - tshort : start time of short circuit
%    - tend : end time of calculation period
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
if( ~isa(m,'motor') )
    m=motor(m);
end

wm=2*pi*sc.speed;

% Steady state conditions
w1=m.w1(sc.u1, sc.psi, sc.tload, wm);
u1=m.u1(w1, m.psi, wm);

% auxiliary variables
rs=m.rstat(w1);
rr=m.rrot(0);
lm=m.Lh;
ls=m.Lls+lm ;
lr=m.Llr+lm ;
D=(ls*lr-lm^2);

lsd=ls/D;
lmd=lm/D;
lrd=lr/D;
        
a=rs*lrd;
b=rs*lmd;
c=rr*lmd;
d=rr*lsd;

wk=w1;
A=[[ a wk -b 0;
    -wk a  0 -b;
     -c 0  d wk-m.p*wm;
     0 -c  m.p*wm-wk d ]];
 
uq=sqrt(2)*abs(u1);
ud=0;
u=[uq;ud;0;0];

% solve equation system for initial values
Y0=[A\u; wm];

% Torque calculation with fluxes:
torque=@(y) 3*m.p*lmd/2*(y(1)*y(4)-y(2)*y(3));

options={};
t=linspace(0,sc.tend,sc.nsamples);
dydt=@(t,y) [ - a*y(1) + b*y(3) - wk*y(2);
           - a*y(2) + b*y(4) + wk*y(1);
          c*y(1) - (wk-m.p*y(5))*y(4) - d*y(3);
          c*y(2) + (wk-m.p*y(5))*y(3) - d*y(4);
                 ( torque(y) - sc.tload)/m.Iin ];

[t,y] = ode45(dydt,t,Y0,options );
%
% current phasor transformation
T=@(d) [[cos(d) sin(d);
         cos(d-2*pi/3) sin(d-2*pi/3);
         cos(d+2*pi/3) sin(d+2*pi/3)]];
%
% Stator current with fluxes:
istat=@(y) [[lrd  lrd  -lmd  -lmd; 
            -lmd -lmd   lsd   lsd]]*y;
%
j=0;
s=size(y);
tq=zeros(s(1),1);
ist=zeros(s(1),3);
for i=y'
  j = j+1;
  tq(j)=torque(i);
  ist(j,:)=T(w1*t(j))*istat(i(1:4));
end;

% calculate i^2 t for all phases
for k = 1:3
  i2t_pos(:,k)=cumtrapz(t,(ist(:,k).*(ist(:,k)>0)).^2);
  i2t_neg(:,k)=cumtrapz(t,(ist(:,k).*(ist(:,k)<0)).^2);
end;

result.u1=abs(u1);
result.f1=w1/(2*pi);
result.ismax=max(ist);
result.ismin=min(ist);
result.tpeak=max(abs(tq));
result.i2t_pos_max=max(i2t_pos);
result.i2t_neg_max=max(i2t_neg);
result.t=t';
result.torque=tq';
result.is=ist';
result.i2t_pos=i2t_pos';
result.i2t_neg=i2t_neg';


