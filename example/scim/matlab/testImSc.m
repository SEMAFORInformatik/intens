function testImSc

mpar.psityp=1.22;
mpar.p=2;
mpar.rs_Tref=0.057623376623;
mpar.rr_Tref=0.1510129;
mpar.Lls=0.8e-3;
mpar.Lh=34.7133758e-3;
mpar.rh=1e5;
mpar.Llr=0.8e-3 ;
mpar.Iin=1.662;

m=motor(mpar);

sc.u1=460;
sc.psi=1.22;
sc.tload=198;
sc.speed=28.433;
sc.tshort=4.1e-3;
sc.tend=0.1;
sc.nsamples=1000 ;
%
% calculate short circuit
r=imsc(m, sc);
%
% test result
assertEqual(size(r.t), [sc.nsamples,1]);
assertEqual(size(r.torque), [sc.nsamples,1]);
assertEqual(size(r.is), [sc.nsamples,3]);
assertEqual(size(r.i2t_pos), [sc.nsamples,3]);
assertEqual(size(r.i2t_neg), [sc.nsamples,3]);
assertElementsAlmostEqual(r.tpeak, 4083.6, 'absolute', 0.5);
assertElementsAlmostEqual(r.ismax, [1945.5 293.7 1092.4], 'absolute', 0.5);
assertElementsAlmostEqual(r.ismin, [-192.5 -1924.3 -1167.3], 'absolute', 0.5);
assertElementsAlmostEqual(r.i2t_pos_max, [20745 566.1 3177.7], 'absolute', 0.5 );
assertElementsAlmostEqual(r.i2t_neg_max, [45.6  16247 7642  ], 'absolute', 0.5 );
