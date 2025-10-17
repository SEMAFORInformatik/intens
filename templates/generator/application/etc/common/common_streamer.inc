// **********************************************************************
// Common
// **********************************************************************
datapool_xml_stream {XML} (
  DATAPOOL
);

datapool_json_stream {JSON} (
  DATAPOOL
);

cycle_xml_stream {XML} (
  CYCLE
);

cycle_json_stream {JSON} (
  CYCLE
);

request_stream { JSON } (
  api_gateway_request
);

response_stream { JSON } (
  api_gateway_response
);
