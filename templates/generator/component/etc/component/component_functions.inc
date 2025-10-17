// **********************************************************************
// ${componentTypeName}
// **********************************************************************
#include <g_${component}_functions.inc>

FUNC ${component}_init_func {
  navIndex_${component} = 0;
  RUN(${component}_navIndex_func);

  ${component}_select.experimental = 1;
  ${component}_select.inPreparation = 1;
  ${component}_select.shared = 1;
  ${component}_select.approved = 1;
  ${component}_select.tested = 1;
  ${component}_select.obsolete = 0;
};

FUNC ${component}_post_load_func {
  RUN(${component}_navIndex_func);
};

FUNC ${component}_post_save_func {
  RUN(${component}_post_load_func);
};

FUNC ${component}_pre_save_func {
  RETURN;
};

FUNC ${component}_pre_get_status_func {
  RETURN;
};

FUNC ${component}_open_func {
  RUN(g_${component}_select_func);
};

FUNC ${component}_new_func {
  RUN(g_${component}_new_func);
};

FUNC ${component}_remove_func {
  RUN(g_${component}_remove_func);
};

FUNC ${component}_store_func {
  RUN(g_${component}_save_func);
};

FUNC ${component}_map_func {
  MAP(${component}_form);
};

FUNC ${component}_variant_func {
  IF(REASON_ACTIVATE) {
    MAP(${component}_form);
  }
};

FUNC ${component}_navIndex_func {
  RETURN;
};

FUNC ${component}_post_delete_func {
  RETURN;
};
