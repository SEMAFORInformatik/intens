FIELDGROUP short_circuit_properties_fg(
  LABEL(variant.short_circuit.nsamples)  variant.short_circuit.nsamples,
  LABEL(variant.short_circuit.u1)        variant.short_circuit.u1
                                           UNIT(variant.short_circuit.u1),
  LABEL(variant.short_circuit.psi)       variant.short_circuit.psi
                                           UNIT(variant.short_circuit.psi),
  LABEL(variant.short_circuit.tload)     variant.short_circuit.tload
                                           UNIT(variant.short_circuit.tload),
  LABEL(variant.short_circuit.speed)     variant.short_circuit.speed*60
                                           UNIT(variant.short_circuit.speed),
  LABEL(variant.short_circuit.tshort)    variant.short_circuit.tshort*1e3
                                           UNIT(variant.short_circuit.tshort),
  LABEL(variant.short_circuit.tend)      variant.short_circuit.tend*1e3
                                           UNIT(variant.short_circuit.tend)
);

FORM short_circuit_form {_("ShortCircuit properties")}(
  short_circuit_properties_fg
);
