DATAPOOL
STRUCT
  {NS}MQReply_Data {
    STRING {SCALAR} user;
    STRING {SCALAR} token;
  }
;
{NS}MQReply_Data login_data;
END DATAPOOL;

STREAMER
  {NS}mqReply_login_data_stream {JSON} (
    login_data
  );
END STREAMER;

FUNCTIONS

  FUNC {SILENT} {NS}mqReply_login_data_func {
    RETURN;
  };
END FUNCTIONS;

OPERATOR
  MESSAGE_QUEUE
    DashboardURL_mqReply_mq {
      REPLY  // MessageQueue Pattern
    , PORT_REQUEST=__PORT_REPLY__
    , NO_DEPENDENCIES
    , ( HEADER="login_token"
      , REQUEST({NS}mqReply_login_data_stream)
      , RESPONSE({NS}mqReply_login_data_stream)
      )
    }
  ;
END OPERATOR;
