// **********************************************************************
// Main Window
// **********************************************************************
FIELDGROUP main_window_top_section_fg (

  // Project
  project_library.overview

  project.name:20
  project.desc:40
  project.status:12
  VOID(5, 0)
  project_library.open {PIXMAP="open"}
  project_library.new {PIXMAP="new"}
  project_library.store {PIXMAP="store"}

  // programVersion:6
  STRETCH(1, 0),

  // Variant
  VOID

  variant.name:20
  variant.desc:40
  variant.approval:12
  VOID(5, 0)
  project_library.import { PIXMAP="open" }
  project_library.new_variant { PIXMAP="new" }
  VOID,
  VOID(0, 2),
  SEPARATOR,
  VOID(0, 2)
);

FOLDER main_window_folder(
  [_("Motor"): motor_tab] (
    (
#if defined(HAVE_DB)
    motor_top_section_fg,
#endif
     motor_properties_fg)
  ),
  [_("Short Circuit"): short_circuit_tab] (
     short_circuit_properties_fg
  ),
  [_("Logbook"): logbook_tab] (
    (
      logbook_action_fg,
      logbook_text_fg
    )
  )
);

FORM main_window_form {
  "SCIM", MAIN
} (
  #if defined(HAVE_DB)
  main_window_top_section_fg,
  #endif
  (main_window_folder, VOID(20), results_fg)
);

FOLDER
  std_log_folder { EXPAND } (
    [_("Standard output")] ( STD_WINDOW { SIZE=24*80 } ),
    [_("Session log")] ( LOG_WINDOW { SIZE=24*80 } ),
    [_("Python log")] ( python_log_fg )
  )
;

FORM
  std_log_form {
    _("Standard output, Session log"),
    HIDE_CYCLE
  } ( std_log_folder )
;
