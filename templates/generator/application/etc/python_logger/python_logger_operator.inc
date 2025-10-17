MESSAGE_QUEUE
  api_gateway_logging_mq {
    SUBSCRIBE
  , HOST=RESOURCE("API_GATEWAY_HOST")
  , PORT=RESOURCE("API_GATEWAY_LOGGER_PORT")
  , (HEADER="python_log.DEBUG"
  , STREAM(python_debug_log_stream)
  )
, (
    HEADER="python_log.INFO"
  , STREAM(python_info_log_stream)
  )
, (
    HEADER="python_log.WARN"
  , STREAM(python_warn_log_stream)
  )
, (
    HEADER="python_log.ERROR"
  , STREAM(python_error_log_stream)
  )
, (
    HEADER="python_log.CRITICAL"
  , STREAM(python_critical_log_stream)
  )
, (
    HEADER="python_log"
  , STREAM(python_all_log_stream)
  )
};

