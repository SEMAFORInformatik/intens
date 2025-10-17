FUNC short_circuit_map_func{
  MAP(short_circuit_form);
};

FUNC short_circuit_variant_func{
  IF(REASON_ACTIVATE){
    RUN(short_circuit_map_func);
  }
};
