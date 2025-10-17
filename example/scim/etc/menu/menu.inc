// **********************************************************************
// Menu
// **********************************************************************
UI_MANAGER

// MENU "&Edit" (
//   FUNC undo_func = "undo"
// , FUNC redo_func = "redo"
// );

MENU "&Log" (
  FORM std_log_form
);

#ifdef DEBUG
MENU "De&bug" (
  MENU _("Open") (
    FUNC datapool_xml_open_func = "Datapool (XML)..."
  // , FUNC datapool_json_open_func = "Datapool (JSON)..."
  , FUNC cycle_xml_open_func = "Cycle (XML)..."
  // , FUNC cycle_json_open_func = "Cycle (JSON)..."
  )
, MENU _("Save") (
    FUNC datapool_xml_save_func = "Datapool (XML)..."
  , FUNC datapool_json_save_func = "Datapool (JSON)..."
  , FUNC cycle_xml_save_func = "Cycle (XML)..."
  , FUNC cycle_json_save_func = "Cycle (JSON)..."
  )
, FUNC common_get_all_users_func = _("DB Users...")
);
#endif

END UI_MANAGER;
