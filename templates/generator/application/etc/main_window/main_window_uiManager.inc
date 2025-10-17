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

FOLDER main_window_folder {EXPAND}(
  [_("General"): General_tab] (
    VOID
  ),
  [_("Logbook"): logbook_tab] (
    (
      logbook_action_fg,
      logbook_text_fg
    )
  )
);

FORM main_window_form {
  "${applicationShortName}", MAIN
} (
#if defined(HAVE_DB)
  main_window_top_section_fg,
#endif
  main_window_folder
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
