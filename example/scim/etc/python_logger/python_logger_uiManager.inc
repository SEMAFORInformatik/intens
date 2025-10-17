FIELDGROUP
  python_debug_log_fg (
    python_logger.debug:80:4 {EXPAND}
  )
;

FIELDGROUP
  python_info_log_fg (
    python_logger.info:80:4 { EXPAND }
  );

  FIELDGROUP python_warn_log_fg (
    python_logger.warn:80:4 { EXPAND }
  );

  FIELDGROUP python_error_log_fg (
    python_logger.error:80:4 { EXPAND }
  );

  FIELDGROUP python_critical_log_fg (
    python_logger.critical:80:4 { EXPAND }
  );


  FIELDGROUP
    python_all_log_fg (
      python_logger.all:80:4 { EXPAND }
    );

    FOLDER
      python_log_folder { EXPAND, BUTTON=NONE } (
        [_("Python log"): python_debug_log_tab] ( python_debug_log_fg )
      , [_("Python log"): python_info_log_tab] ( python_info_log_fg )
      , [_("Python log"): python_warn_log_tab] ( python_warn_log_fg )
      , [_("Python log"): python_error_log_tab] ( python_error_log_fg )
      , [_("Python log"): python_critical_log_tab] ( python_critical_log_fg )
      , [_("Python log"): python_all_log_tab] ( python_all_log_fg )
      );

      FIELDGROUP
        python_log_fg (
          python_logger.log_level
        , python_log_folder
        );
