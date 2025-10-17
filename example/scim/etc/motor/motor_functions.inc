// **********************************************************************
// Motor
// **********************************************************************
#include <g_motor_functions.inc>

FUNC motor_init_func {
  navIndex_motor = 0;
  RUN(motor_navIndex_func);

  motor_select.experimental = 1;
  motor_select.inPreparation = 1;
  motor_select.shared = 1;
  motor_select.approved = 1;
  motor_select.tested = 1;
  motor_select.obsolete = 0;
};

FUNC motor_post_load_func {
  RUN(motor_navIndex_func);
};

FUNC motor_post_save_func {
  RUN(motor_post_load_func);
};

FUNC motor_pre_save_func {
  RETURN;
};

FUNC motor_pre_get_status_func {
  RETURN;
};

FUNC motor_open_func {
  RUN(g_motor_select_func);
};

FUNC motor_new_func {
  RUN(g_motor_new_func);
};

FUNC motor_remove_func {
  RUN(g_motor_remove_func);
};

FUNC motor_store_func {
  RUN(g_motor_save_func);
};

FUNC motor_map_func {
  MAP(motor_form);
};

FUNC motor_variant_func {
  IF(REASON_ACTIVATE) {
    MAP(motor_form);
  }
};

FUNC motor_navIndex_func {
  RETURN;
};

FUNC motor_post_delete_func {
  RETURN;
};
