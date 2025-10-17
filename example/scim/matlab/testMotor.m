function testMotor
% test motor class
%
in.psityp=1.22;
in.p=2;
in.rs_Tref=0.057623376623;
in.rr_Tref=0.1510129;
in.Lls=0.8e-3;
in.Lh=34.7133758e-3;
in.rh=1e5;
in.Llr=0.8e-3 ;
in.Iin=1.662;

m=motor(in);

u1=460.0;
psi=m.psityp;
tload=198;
wm=2*pi*28.433;
%
% calculate stator frequency and voltage
w1=m.w1(u1, psi, tload, wm);
u1=m.u1(w1, m.psi, wm);
%
% test result
assertElementsAlmostEqual(u1, -4.7431e+00 + 4.5476e+02i, 'relative', 1e-5);



