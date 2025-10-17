function comp( inputfilename )
%
% 2011-08-12, Markus Wildi
%
% For Demo Purposes Only
%
% Object oriented demonstrator for comparision with 
% Siemens' i2t_calc.m and Semafor's comp.py, by Ronald 
% Tanner
% comp.m contains basically the main method of comp.py
% while motor.m is a reincarnation of 
% comp.py:class InductionMotor()
% 
% Read the temporary data file
%
f=fopen(inputfilename);
s=fread(f);
data=parse_json(char(s'));
%
% Input objects
%
torque=data.shortCircuit.tload;
speed=data.shortCircuit.speed;

w1=2*pi*data.shortCircuit.f1;
wm=2*pi*speed;
u1=data.shortCircuit.u1;
psi=u1/w1;

inductionMotor= motor(data.motor);

w1=inductionMotor.w1(u1, psi, torque, wm);

fprintf( 'f1= %f Hz\n',w1/(2*pi));
u1=inductionMotor.u1(w1, inductionMotor.psi, wm);
i1=inductionMotor.i1(w1, inductionMotor.psi, wm);
fprintf( 'u1= %f V\n',abs(u1));
fprintf( 'i1= %f A\n',abs(i1));


p1= real(3*(u1*conj(i1)));
fprintf( 'p1= %f kW\n',p1/1000);
pm=wm*torque;
fprintf( 'pm= %f kW\n',pm/1000);
fprintf( 'eta= %f\n',pm/p1);

%%resultJson=mat2json(result);

