DATAPOOL

STRUCT
  MQReply_Response {
    STRING  {SCALAR} status;
    STRING  {SCALAR} message;
    INTEGER {SCALAR} protoIndex;
    STRING  {SCALAR} protoReplyType;
  }
;

STRUCT
  MQReply_FunctionResponse {
    STRING {SCALAR} data;
  }
;

STRUCT
  MQReply_FunctionResult {
    STRING {SCALAR} method;
  }
;

STRUCT
  MQReply_UiManagerResponse {
    CDATA   {SCALAR} data;
  }
;

STRUCT
  MQReply_DatapoolResponse {
    STRING  {SCALAR} svalue;
    INTEGER {SCALAR} ivalue;
    REAL    {SCALAR} rvalue;
    CDATA   {SCALAR} cdata;
  }
;

STRUCT
  MQReply_InfoResponse {
    STRING    {SCALAR} hostname;
    STRING    {SCALAR} ipaddr;
  }
;

STRUCT
  MQReply_Function {
    STRING {SCALAR}
      name,
      this,
      reason,
      data
    ;
    INTEGER {SCALAR}
      index
    ;
  }
;

STRUCT
  MQReply_Control {
    STRING {SCALAR}
      command,
      guielement, method, form_name
    ;
    STRING argument;
    INTEGER {SCALAR}
      index, inum
    ;
  }
;

STRUCT
  MQReply_UiManager {
    STRING {SCALAR}
      name,
      type
    ;
  }
;

STRUCT
  MQReply_Datapool {
    STRING {SCALAR}
      method,
      varname,
      data,
      form_name
    ;
  }
;

STRUCT
  MQReply_Library {
    STRING {SCALAR}
      db_url,
      db_username,
      db_password,
      lc_all,
      funcname
    ;
  }
;

STRUCT
  MQReply_File {
    STRING  {SCALAR} filename;
    STRING  {SCALAR} mdsum;
    CDATA   {SCALAR} data {HIDDEN};
  }
;

// defined in DataPoolIntens
// FUNCTION_ARGS  mqReply_functionArgs
MQReply_Control   {GLOBAL, SCALAR} mqReply_control {HIDDEN};
MQReply_UiManager {GLOBAL, SCALAR} mqReply_uimanager {HIDDEN};
MQReply_Datapool  {GLOBAL, SCALAR} mqReply_datapool {HIDDEN};
MQReply_Library   {GLOBAL, SCALAR} mqReply_library {HIDDEN}; // muss global sein, entspricht einem Applikationsobjek

MQReply_Response          {GLOBAL, SCALAR} mqReply_response {HIDDEN};
MQReply_FunctionResponse  {GLOBAL, SCALAR, OMIT_TTRAIL} mqReply_function_response {HIDDEN};
MQReply_FunctionResult    {GLOBAL, SCALAR} mqReply_function_result {HIDDEN};
MQReply_UiManagerResponse {GLOBAL, SCALAR} mqReply_uimanager_response {HIDDEN};
MQReply_DatapoolResponse  {GLOBAL, SCALAR} mqReply_datapool_response {HIDDEN};
MQReply_InfoResponse      {GLOBAL, SCALAR} mqReply_info_response {HIDDEN};
MQReply_File              {GLOBAL, SCALAR} mqReply_upload {HIDDEN};
MQReply_File              {GLOBAL, SCALAR} mqReply_download {HIDDEN};
MQReply_File              {GLOBAL, SCALAR} mqReply_file {HIDDEN};

STRING {GLOBAL, SCALAR} mqReply_LC_ALL {HIDDEN};
STRING {GLOBAL, SCALAR} mqReply_funcname {HIDDEN};
STRING {GLOBAL, SCALAR} mqReply_reason {HIDDEN};
END DATAPOOL;

STREAMER
  mqReply_function_request_stream {JSON} (
    mqReply_functionArgs
  );

  mqReply_control_request_stream {JSON} (
    mqReply_control
  );

  mqReply_function_response_stream (
    mqReply_function_response
  );

  mqReply_function_result_stream {JSON} (
    mqReply_function_result
  );

  mqReply_uimanager_request_stream {JSON} (
    mqReply_uimanager
  );

  mqReply_uimanager_response_stream (
    mqReply_uimanager_response
  );

  mqReply_datapool_request_stream {JSON} (
    mqReply_datapool
  );

  // tee: for debugging reasons
  mqReply_datapool_response_stream {JSON} (
    mqReply_datapool_response
  );

  mqReply_datavalue_response_stream {JSON} (
    mqReply_datapool_response
  );

  mqReply_info_response_stream {JSON} (
    mqReply_info_response
  );

  mqReply_response_stream {JSON} (
    mqReply_response
  );

  mqReply_datapool_stream {JSON, HIDDEN} (
    DATAPOOL
  );

  mqReply_progress_stream {JSON} (
    ProgressDialog, ProgressDialogPixmap, ProgressDialogDetailPixmap, ProgressDialogAbortCommand
  );

  mqReply_file_request_stream {JSON} (
    mqReply_file
  );

  mqReply_file_data_stream {NO_GZ} (
    mqReply_file.data
  );

END STREAMER;

OPERATOR
PROCESS
  mqReply_datapool_pr : BATCH{"cat"};

PROCESSGROUP
  mqReply_datapool_pg {FORM=NONE, HIDDEN}(
    [mqReply_datapool_response.cdata] = mqReply_datapool_pr(mqReply_datapool_stream);
  )
;

TIMER
  mqReply_exit_timer {
    FUNC=mqReply_exit_func
  }
;

FILESTREAM
  mqReply_file_load_fs
    = mqReply_file_data_stream {
    "File Up/Download"
  , FILTER="ALL (*.*)"
  , HIDDEN
  }
;
END OPERATOR;

FUNCTIONS
  FUNC {SILENT} mqReply_exit_func {
    STOP(mqReply_exit_timer);
    IF(mqReply_control.argument == "force" ||
       mqReply_control.argument == "FORCE"){
      EXIT(TRUE);
    }ELSE{
      EXIT;
    }
  };

  FUNC {SILENT, PROTO, WEBAPI=mqReply_function_response.data} mqReply_function_func {
    LOG(INFO, "mqReply_function_func called: ", mqReply_functionArgs);
    CLEAR(mqReply_response);
    mqReply_response.protoIndex = 1;
    mqReply_response.protoReplyType = "WEBAPI";
    CLEAR(mqReply_function_response);
    SET_CURRENT_FORM(mqReply_functionArgs.form_name);
    // assign
    SET_FUNC_ARGS(mqReply_functionArgs);
    IF (VALID(mqReply_functionArgs.this) && REASON_INPUT) {
      IF(mqReply_functionArgs.value == "") {
        mqReply_functionArgs.value = INVALID;
      }
      IF (mqReply_functionArgs.value != "￾ignore-me￿") {
        IF(!ASSIGN_CONSISTENCY(VAR(mqReply_functionArgs.this), mqReply_functionArgs.value)) {
          mqReply_response.status = "ERROR";
          mqReply_response.message = "Invalid varname: " + mqReply_functionArgs.this;
          MESSAGEBOX("Conversion error");
          ABORT;
        }
      }
    }
    // run function
    LOG(INFO, COMPOSE("call func: <b>%1</b>, This: <b>%2</b>, Index: <b>%3</b>, Value: <b>%4</b>, Source: <b>%5</b>, x: <b>%6</b>, y: <b>%7</b><br/>",
              mqReply_functionArgs.name, mqReply_functionArgs.this, mqReply_functionArgs.index,
              mqReply_functionArgs.value, mqReply_functionArgs.source,
              mqReply_functionArgs.x_pos, mqReply_functionArgs.y_pos));
    // copy pos to Global_Point.X/Y
    IF (VALID(mqReply_functionArgs.x_pos)) {
      Global_Point.X = mqReply_functionArgs.x_pos;
    }
    IF (VALID(mqReply_functionArgs.y_pos)) {
      Global_Point.Y = mqReply_functionArgs.y_pos;
    }

    IF (mqReply_functionArgs.name == "GUIELEMENT_METHOD") {
      MESSAGEBOX("Not Implemented yet");
      mqReply_response.status = "OK";
      RETURN;
    }
    IF (mqReply_functionArgs.name == "QUIT") {
      IF (!CONFIRM(_("Do you really want to leave this application?"))) {
        ABORT;
      }
      LOG(INFO, "DO EXIT");
      IF (RESOURCE("IS_WEBTENS") == 1) {
        EXIT;
      } ELSE {
        START(mqReply_exit_timer, PERIOD=1);
      }
    }
    IF (!VALID(mqReply_functionArgs.name)) {
      mqReply_response.status = "ERROR";
      mqReply_response.message = "Empty Function name";
    } ELSE {
      RUN(mqReply_functionArgs.name);
      mqReply_response.status = "OK";
    }

    // QUIT (only set an exit status)
    IF (mqReply_functionArgs.name == "QUIT") {
      REST_LOGOFF;
    }
    LOG(INFO, "mqReply_function_func DONE, response: ", mqReply_response);
  };

  FUNC {SILENT, PROTO, WEBAPI=mqReply_function_response.data} mqReply_control_func {
    CLEAR(mqReply_response);
    mqReply_response.protoIndex = 2;
    mqReply_response.protoReplyType = "WEBAPI";
    CLEAR(mqReply_function_response);
    SET_CURRENT_FORM(mqReply_control.form_name);

    // Login
    IF (mqReply_control.command == "LOGIN") {
      RUN(mqReply_login_func);
      IF (mqReply_response.status == "ERROR") {
	      RETURN;
      }
    } ELSE {
      LOG(INFO, "mqReply_control_func called: ", mqReply_control);
    }

    // Map Folder or Form
    IF (mqReply_control.command == "MAP") {
      MAP(mqReply_control.argument);
      LOG(INFO, "DO MAP");
    }

    // Unmap Folder or Form
    IF (mqReply_control.command == "UNMAP") {
      UNMAP(mqReply_control.argument);
      LOG(INFO, "DO UNMAP");
    }

    // element method call
    IF (mqReply_control.command == "GUIELEMENT_METHOD") {
      LOG(INFO, "GUIELEMENT_METHOD guielement: ", mqReply_control.guielement);
      mqReply_function_result.method = GUI_ELEMENT_METHOD(mqReply_control.guielement,
                                                          mqReply_control.method,
                                                          mqReply_control.argument);
    }

    // element method call
    IF (mqReply_control.command == "MENU_BUTTON_ACTION") {
      LOG(INFO, "MENU_BUTTON_ACTION guielement: ", mqReply_control.guielement);
      mqReply_function_result.method = GUI_ELEMENT_METHOD(mqReply_control.guielement,
                                                          mqReply_control.command,
                                                          mqReply_control.argument);
      CLEAR(mqReply_response);  // to be sure, to clear a file_save operation
    }

    // // get Icon/bitmap
    // IF (mqReply_control.command == "BITMAP") {
    //   mqReply_function_result.icon = ICON(mqReply_control.argument);
    //   LOG(INFO, "BITMAP, Size:", LENGTH(mqReply_function_result.icon), " NAME: ", mqReply_control.argument);
    //   PRINT("BITMAP, Size:", LENGTH(mqReply_function_result.icon), " NAME: ", mqReply_control.argument, EOLN);
    // }

    // Map FolderTab (with index)
    IF (mqReply_control.command == "FOLDERTAB") {
      MAP(mqReply_control.argument:mqReply_control.index );
    }

    // GoCycle
    IF (mqReply_control.command == "GOCYCLE") {
      mqReply_control.inum = mqReply_control.argument;
      IF(mqReply_control.inum != GETCYCLE &&
         mqReply_control.inum <= MAXCYCLE
      ) {
        GOCYCLE(mqReply_control.inum);

        mqReply_funcname = "ON_CYCLE_SWITCH";
        RUN(mqReply_funcname);
        CLEAR(mqReply_funcname);

        mqReply_reason = "cycle_switch";
        SET_REASON(mqReply_reason);
        CLEAR(mqReply_reason);
        mqReply_funcname = "ON_CYCLE_EVENT";
        RUN(mqReply_funcname);
        CLEAR(mqReply_funcname);
      }
      mqReply_response.status = "OK";
    }

    // NewCycle
    IF (mqReply_control.command == "NEWCYCLE") {
      NEWCYCLE(mqReply_control.argument);

      mqReply_reason = "cycle_new";
      SET_REASON(mqReply_reason);
      CLEAR(mqReply_reason);
      mqReply_funcname = "ON_CYCLE_EVENT";
      RUN(mqReply_funcname);
      CLEAR(mqReply_funcname);
    }

    // ClearCycle
    IF (mqReply_control.command == "CLEARCYCLE") {
      CLEARCYCLE;

      mqReply_funcname = "AFTER_DB_LOGON";
      RUN(mqReply_funcname);
      CLEAR(mqReply_funcname);

      mqReply_funcname = "INIT";
      RUN(mqReply_funcname);
      CLEAR(mqReply_funcname);

      mqReply_reason = "cycle_clear";
      SET_REASON(mqReply_reason);
      CLEAR(mqReply_reason);
      mqReply_funcname = "ON_CYCLE_EVENT";
      RUN(mqReply_funcname);
      CLEAR(mqReply_funcname);
    }

    // DeleteCycle
    IF (mqReply_control.command == "DELETECYCLE") {
      DELETECYCLE;

      mqReply_funcname = "ON_CYCLE_SWITCH";
      RUN(mqReply_funcname);
      CLEAR(mqReply_funcname);

      mqReply_reason = "cycle_delete";
      SET_REASON(mqReply_reason);
      CLEAR(mqReply_reason);
      mqReply_funcname = "ON_CYCLE_EVENT";
      RUN(mqReply_funcname);
      CLEAR(mqReply_funcname);
    }

    // RenameCycle
    IF (mqReply_control.command == "RENAMECYCLE") {
      IF (VALID(mqReply_control.argument[1])) {
        mqReply_control.index = mqReply_control.argument[0];
        // TODO ?: OLDVALUE = CYCLENAME[mqReply_control.index];
        CYCLENAME[mqReply_control.index] = mqReply_control.argument[1];
      } ELSE {
        // TODO ?: OLDVALUE = CYCLENAME[mqReply_control.index];
        CYCLENAME[mqReply_control.index] = mqReply_control.argument;
      }
      mqReply_reason = "cycle_rename";
      SET_REASON(mqReply_reason);
      CLEAR(mqReply_reason);
      mqReply_funcname = "ON_CYCLE_EVENT";
      RUN(mqReply_funcname);
      CLEAR(mqReply_funcname);
    }

    // Exit
    IF (mqReply_control.command == "EXIT") {
      START(mqReply_exit_timer, PERIOD=1);
    }

    // Hello Intens
    IF (mqReply_control.command == "HELLO") {
      LOG(INFO, "mqReply_control_func: Hello Intens");
    }

    IF (!VALID(mqReply_response.status)) {
      mqReply_response.status = "OK";
    }
    LOG(INFO, "mqReply_control_func DONE, response: ", mqReply_response);
  };

  FUNC {SILENT} mqReply_uimanager_func {
    CLEAR(mqReply_response);
    mqReply_response.protoIndex = 1;
    mqReply_response.protoReplyType = "FORM";
    CLEAR(mqReply_uimanager_response);
    LOG(INFO, "mqReply_uimanager_func called: ", mqReply_uimanager);
    IF (mqReply_uimanager.type == "PROTO") {
      SERIALIZE_FORM(mqReply_uimanager.name, PROTO, mqReply_uimanager_response.data );
      mqReply_response.status = "OK";
    } ELSE {
      IF (mqReply_uimanager.type == "JSON") {
        SERIALIZE_FORM(mqReply_uimanager.name, JSON, mqReply_uimanager_response.data );
        mqReply_response.status = "OK";
      } ELSE {
        mqReply_response.status = "ERROR";
        mqReply_response.message = "unknown type: " + mqReply_uimanager.type;
      }
    }
    LOG(DEBUG, "mqReply_uimanager_func DONE, response: ", mqReply_response);
  };

  FUNC {SILENT, PROTO, WEBAPI=mqReply_function_response.data} mqReply_datapool_func {
    CLEAR(mqReply_response);
    mqReply_response.protoIndex = 2;
    mqReply_response.protoReplyType = "WEBAPI";
    CLEAR(mqReply_function_response);
    SET_CURRENT_FORM(mqReply_datapool.form_name);
    LOG(INFO, "mqReply_datapool_func called: ", mqReply_datapool);
    IF (mqReply_datapool.method == "PUT") {
      IF(mqReply_datapool.data == "") {
        mqReply_datapool.data = INVALID;
      }
      IF (ASSIGN_CONSISTENCY(VAR(mqReply_datapool.varname), mqReply_datapool.data)) {
///        PRINT(">>>", mqReply_datapool.varname, " = ", VAR(mqReply_datapool.varname), " (", VALID(VAR(mqReply_datapool.varname)), ")", EOLN);
        mqReply_response.status = "OK";
      } ELSE {
        mqReply_response.status = "ERROR";
        mqReply_response.message = "Invalid varname: " + mqReply_datapool.varname;
        MESSAGEBOX("Conversion error");
        ABORT;
      }
    } ELSE {
      IF (mqReply_datapool.method == "GET") {
        CLEAR(mqReply_datapool_response);
        IF (VALID(mqReply_datapool.varname)) {
          mqReply_datapool_response.svalue = VAR(mqReply_datapool.varname);
          mqReply_datapool_response.ivalue = VAR(mqReply_datapool.varname);
          mqReply_datapool_response.rvalue = VAR(mqReply_datapool.varname);
        } ELSE {
          // export complete datapool
          RUN(mqReply_datapool_pg);
        }
        mqReply_response.status = "OK";
      } ELSE {
        mqReply_response.status = "ERROR";
        mqReply_response.message = "unknown method: " + mqReply_datapool.method;
      }
    }
    LOG(DEBUG, "mqReply_datapool_func DONE, responseLength: ", LENGTH(mqReply_datapool_response));
  };

  FUNC {SILENT} mqReply_info_func {
    LOG(INFO, "mqReply_info_func called: ", mqReply_datapool);
    CLEAR(mqReply_response);
    mqReply_response.protoIndex = 1;
    CLEAR(mqReply_info_response);

    mqReply_info_response.hostname = VAR("HOST");
    mqReply_info_response.ipaddr = VAR("IPADDR");

    mqReply_response.status = "OK";
    LOG(INFO, "mqReply_info_func DONE, response: ", mqReply_response);
    LOG(INFO, "mqReply_info_func DONE, hostname: ", mqReply_info_response.hostname);
  };

  FUNC mqReply_login_func {
    // db login
    IF (!VALID(mqReply_library.db_url)) {
      mqReply_library.db_url = RESTBASE;
    }
    IF (!VALID(mqReply_library.db_url) || mqReply_library.db_url == "") {
      mqReply_library.db_url = RESOURCE("REST_SERVICE_BASE");
      LOG(INFO, "SET URL["+mqReply_library.db_url+"]");
    }
    mqReply_library.db_username = "test";
    mqReply_library.db_password = "test";
    IF (VALID(mqReply_control.argument[0])) {
      mqReply_library.db_username = mqReply_control.argument[0];
    }
    IF (VALID(mqReply_control.argument[1])) {
      mqReply_library.db_password = mqReply_control.argument[1];
    }
    IF (VALID(mqReply_control.argument[2])) {
      IF(LENGTH(mqReply_control.argument[2]) == 5) {
        mqReply_control.argument[2] += ".utf8";
      }
      SET_RESOURCE("LC_ALL", mqReply_control.argument[2]);
      mqReply_library.lc_all = mqReply_control.argument[2];
      LOG(INFO, "LC_ALL: " + mqReply_library.lc_all);
      mqReply_LC_ALL = mqReply_control.argument[2];
    }
    CLEAR(mqReply_control);

    // logoff
    REST_LOGOFF;

    LOG(INFO, "DB URL["+mqReply_library.db_url+"] user["+mqReply_library.db_username+
              "] passw[...]");
    PRINT("DB URL["+mqReply_library.db_url+"] user["+mqReply_library.db_username+
              "] passw[...]",EOLN);
    IF(!REST_LOGON(
      mqReply_library.db_url
    , mqReply_library.db_username
    , mqReply_library.db_password
    , mqReply_response.message
    )) {
      LOG(INFO, "Login Failed, message: " + mqReply_response.message);
      mqReply_response.status = "ERROR";
      CLEAR(mqReply_library);
      RETURN;
    }
    IF (mqReply_library.db_username == "__accesstoken__") {
      RETURN;
    }
    CLEAR(mqReply_library);
    LOG(INFO, "Login Done.");

    // Delete Cycles
    WHILE ( MAXCYCLE >= 2 ) {
      GOCYCLE(2);
      DELETECYCLE;
    }

    // clear base Cycle
    GOCYCLE(1);
    CLEARCYCLE;

    mqReply_funcname = "AFTER_DB_LOGON";
    RUN(mqReply_funcname);
    CLEAR(mqReply_funcname);

    mqReply_funcname = "INIT";
    RUN(mqReply_funcname);
    CLEAR(mqReply_funcname);

    mqReply_reason = "cycle_clear";
    SET_REASON(mqReply_reason);
    CLEAR(mqReply_reason);
    mqReply_funcname = "ON_CYCLE_EVENT";
    RUN(mqReply_funcname);
    CLEAR(mqReply_funcname);

    mqReply_response.status = "OK";
  };

  FUNC { SILENT} mqReply_dblogin_func {
    IF (VALID(mqReply_control.argument[3])) {  // got restservice url
      mqReply_library.db_url = mqReply_control.argument[3];
    }
    RUN(mqReply_login_func);
  };

  FUNC { SILENT} mqReply_getvalue_func {
    CLEAR(mqReply_response);
    CLEAR(mqReply_datapool_response);

    mqReply_response.status = "OK";
    IF (VALID(mqReply_datapool.varname)) {
      LOG(DEBUG, "var ", mqReply_datapool.varname, " is valid");
      mqReply_datapool_response.svalue = VAR(mqReply_datapool.varname);
      mqReply_datapool_response.ivalue = VAR(mqReply_datapool.varname);
      mqReply_datapool_response.rvalue = VAR(mqReply_datapool.varname);
    } ELSE {
      LOG(DEBUG, "var ", mqReply_datapool.varname, " is NOT valid");
    }
    LOG(INFO, "mqReply_getvalue_func DONE, response: ", mqReply_datapool_response);
  };

  FUNC { SILENT} mqReply_setvalue_func {
    CLEAR(mqReply_response);

    IF (ASSIGN_CONSISTENCY(VAR(mqReply_datapool.varname), mqReply_datapool.data)) {
      mqReply_response.status = "OK";
    } ELSE {
      mqReply_response.status = "ERROR";
      mqReply_response.message = "Invalid varname: " + mqReply_datapool.varname;
    }
    LOG(INFO, "mqReply_setvalue_func DONE, response: ", mqReply_response);
  };

  FUNC { SILENT} mqReply_download_func {
    CLEAR(mqReply_response);
    OPEN(mqReply_file_load_fs, mqReply_file.filename);
  };

  FUNC { SILENT} mqReply_upload_func {
    CLEAR(mqReply_response);
    SAVE(mqReply_file_load_fs, mqReply_file.filename);
  };
END FUNCTIONS;

OPERATOR
  MESSAGE_QUEUE
    mqReply_reply_mq {
      REPLY  // MessageQueue Pattern
    , PORT_REQUEST=__PORT_REPLY__   //* Request Port of Reply MesssageQueue
    , NO_DEPENDENCIES
    , ( HEADER="function"
      , FUNC = mqReply_function_func
      , RESPONSE(
          mqReply_response_stream,
          mqReply_function_response_stream
        )
      , REQUEST(mqReply_function_request_stream)
      )
    , ( HEADER="control"
      , FUNC = mqReply_control_func
      , RESPONSE(
          mqReply_response_stream,
          mqReply_function_result_stream,
          mqReply_function_response_stream
        )
      , REQUEST(mqReply_control_request_stream)
      )
    , ( HEADER="uimanager"
      , FUNC = mqReply_uimanager_func
      , RESPONSE(
          mqReply_response_stream,
          mqReply_uimanager_response_stream
        )
      , REQUEST(mqReply_uimanager_request_stream)
      )
    , ( HEADER="datapool"
      , FUNC = mqReply_datapool_func
      , RESPONSE(
          mqReply_response_stream,
          mqReply_datapool_response_stream,
          mqReply_function_response_stream
        )
      , REQUEST(mqReply_datapool_request_stream)
      )
    , ( HEADER="info"
      , FUNC = mqReply_info_func
      , RESPONSE(
          mqReply_response_stream,
          mqReply_info_response_stream
        )
      , REQUEST(mqReply_datapool_request_stream)
      )

      // Functions for Tests
    , ( HEADER="dblogin"
      , FUNC = mqReply_dblogin_func
      , RESPONSE(mqReply_response_stream)
      , REQUEST(mqReply_control_request_stream)
      )
    , ( HEADER="getvalue"
      , FUNC = mqReply_getvalue_func
      , RESPONSE(mqReply_response_stream, mqReply_datavalue_response_stream)
      , REQUEST(mqReply_datapool_request_stream)
      )
    , ( HEADER="setvalue"
      , FUNC = mqReply_setvalue_func
      , RESPONSE(mqReply_response_stream)
      , REQUEST(mqReply_datapool_request_stream)
      )
    , ( HEADER="download"
      , FUNC = mqReply_download_func
      , RESPONSE(mqReply_response_stream, mqReply_file_data_stream)
      , REQUEST(mqReply_file_request_stream)
      )
    , ( HEADER="upload"
      , FUNC = mqReply_upload_func
      , RESPONSE(mqReply_response_stream)
      , REQUEST(mqReply_file_request_stream, mqReply_file_data_stream)
      )
    }
  ;

END OPERATOR;
