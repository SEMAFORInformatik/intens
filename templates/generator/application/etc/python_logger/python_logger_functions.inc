FUNC
  python_loglevel_func {

    IF (VALID(INPUT)) {
      python_logger.log_level = INPUT;
    }

    IF(python_logger.log_level == "debug") {
      MAP(python_debug_log_tab);
    } ELSE IF(python_logger.log_level == "info") {
      MAP(python_info_log_tab);
    } ELSE IF(python_logger.log_level == "warn") {
      MAP(python_warn_log_tab);
    } ELSE IF(python_logger.log_level == "error") {
      MAP(python_error_log_tab);
    } ELSE IF(python_logger.log_level == "critical") {
      MAP(python_critical_log_tab);
    } ELSE {
      MAP(python_all_log_tab);
    }
  }
;
