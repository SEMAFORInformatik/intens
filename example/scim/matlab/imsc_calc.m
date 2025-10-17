function result=imsc_calc( inputfilename )
%
% 2011-07-08, Markus Wildi, Ronald Tanner
%
% For Demo Purposes Only
%
% Read the input file and collect parameters:

f=fopen(inputfilename);
s=fread(f);
fclose(f);
data=parse_json(char(s'));

m=motor(data.motor);

% execute calculation
r=imsc(m, data.shortCircuit );

% output result in JSON format, full results including values for plots
result=mat2json(r);
end
