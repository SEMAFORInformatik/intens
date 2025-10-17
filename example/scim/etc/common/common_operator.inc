// **********************************************************************
// Common
// **********************************************************************
FILESTREAM datapool_xml_fs = datapool_xml_stream {
  "Datapool (XML)"
, FILTER="XML (*.xml)"
, DIRNAME="./data/"
, HIDDEN
};

FILESTREAM datapool_json_fs = datapool_json_stream {
  "Datapool (JSON)"
, FILTER="JSON (*.json)"
, HIDDEN
};

FILESTREAM cycle_xml_fs = cycle_xml_stream {
  "Cycle (XML)"
, FILTER="XML (*.xml)"
, HIDDEN
};

FILESTREAM cycle_json_fs = cycle_json_stream {
  "Cycle (JSON)"
, FILTER="JSON (*.json)"
, HIDDEN
};

MESSAGE_QUEUE api_gateway_mq {
  REQUEST
, HOST=RESOURCE("API_GATEWAY_HOST")
, PORT_REQUEST=RESOURCE("API_GATEWAY_PORT")
, TIMEOUT=0
};
