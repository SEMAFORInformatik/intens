%To read a json file in matlab proceed as follows
f=fopen('siemens.json-ori')
s=fread(f)

data=parse_json(char(s'));

fprintf( 1, '-----------------%f\n', data.motor{1}.rs_Tref);
fprintf( 1, '-----------------%f\n', data.operatingPoint{1}.n0_rpm);
fprintf( 1, '-----------------%f\n', data.shortCircuit{1}.V_dio);
