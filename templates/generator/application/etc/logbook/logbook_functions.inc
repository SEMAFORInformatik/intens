// **********************************************************************
// LogBook
// **********************************************************************

FUNC logbook_update_func {
  REQUEST (
    MESSAGE_QUEUE = api_gateway_mq
  , HEADER="logbook"
  , REQUEST(logbook_in)
  , RESPONSE(response_stream, logbook_text_out, logbook_pos_out)
  );
};

FUNC logbook_init_func {
  DISABLE( logbookInfo.inx );
  DISABLE( logbookEntry.text );
  UNMAP( logbook_entry_form );
};

FUNC logbook_new_func {
  CLEAR( logbookEntry );
  logbookInfo.inx = INVALID;
  DISABLE(logbookInfo.inx);
  ENABLE(logbookEntry.text );
  MAP( logbook_entry_form );
};

FUNC logbook_edit_func {
  SIZE( logbook[*], logbookInfo.size );
  logbookInfo.inx = INVALID;
  CLEAR( logbookEntry );
  DISABLE( logbookEntry.text );
  ENABLE( logbookInfo.inx );
  MAP( logbook_entry_form );
};

FUNC logbook_edit_index_func {
  IF( !VALID(logbookInfo.inx) ){
    RETURN;
  }
  SIZE( logbook[*], logbookInfo.size );
  IF( logbookInfo.inx < logbookInfo.size ){
    logbookEntry = logbook[logbookInfo.inx];
    DISABLE( logbookInfo.inx );
    ENABLE( logbookEntry.text );
  }
};

FUNC logbook_get_func {
  IF( !VALID(project.id)) {
    RETURN;
  }
  CLEAR(logbook);
  logbookInfo.filter.projectId = project.id;

  [logbook[#]] = GET (
    PATH=[ "components/type/LogBook?_projection=(text,changed,changername)&_ignorecase" ]
  , FILTER=[logbookInfo.filter]
  );
  RUN( logbook_update_func );
};

FUNC logbook_save_func {
  IF( !VALID(logbookEntry.text) ){
    RETURN;
  }
  IF( !VALID(project.id)) {
    MESSAGEBOX("No Workbook available");
    RETURN;
  }

  SIZE( logbook[*], logbookInfo.size );

  IF( VALID(logbookEntry.id) ){
    [logbookDb] = GET (
      PATH=[ "components/", logbookEntry.id ]
    );
    logbookDb.reason = "modified entry";
  }
  ELSE{
    logbookDb.name = "log_entry_" + logbookInfo.size;
    logbookDb.reason = "new entry";
  }
  logbookDb.type = "LogBook";
  logbookDb.desc = "LogEntry";
  logbookDb.projectId = project.id;

  IF( !VALID(logbookInfo.inx) ){
    logbookInfo.inx = logbookInfo.size;
  }

  logbookDb.text = logbookEntry.text;

  [logbookEntry] = PUT(
    PATH="components"
  , DATA=[logbookDb]
  );
  [logbookEntry] = GET (
    PATH=["components/type/LogBook?id=", logbookEntry.id,
          "&_projection=(text,changed,changername)&_ignorecase"]
  );

  logbook[logbookInfo.inx] = logbookEntry;
  logbookInfo.id = logbookEntry.id;
  RUN( logbook_update_func );
  UNMAP( logbook_entry_form );
};

FUNC logbook_select_func {
  INTEGER i;
  IF( !REASON_SELECT ){
    RETURN;
  }
  i = 0;
  SIZE( logbookInfo.position[*], logbookInfo.size );
  WHILE(i < logbookInfo.size){
    IF(INDEX > logbookInfo.position[i].end){
      i++;
    }
    ELSE{
      logbookInfo.inx = logbookInfo.position[i].index;
      logbookEntry = logbook[logbookInfo.position[i].index];
      DISABLE( logbookInfo.inx );
      ENABLE( logbookEntry.text );
      MAP( logbook_entry_form );
      i = logbookInfo.size;
    }
  }
};
