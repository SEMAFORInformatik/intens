SET python_logging_set (
  _("Debug") = "debug"
, _("Info") = "info"
, _("Warn") = "warn"
, _("Error") = "error"
, _("Critical") = "critical"
, _("All") = "all"
)
;

STRUCT PythonLogger {
  STRING { EDITABLE, SCALAR }
    log_level {
      LABEL = _("Log Level")
    , SET=python_logging_set
    , FUNC=python_loglevel_func
    }
  ;
  CDATA
    debug { LABEL="Python DEBUG Log", SCALAR }
  , info { LABEL="Python INFO Log" }
  , warn { LABEL="Python WARNING Log" }
  , error { LABEL="Python ERROR Log" }
  , critical { LABEL="Python CRITICAL Log" }
  , all { LABEL="Python ALL Log" }
  ;
};
PythonLogger {SCALAR, OMIT_TTRAIL}
  python_logger;
