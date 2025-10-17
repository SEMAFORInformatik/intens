// **********************************************************************
// LogBook
// **********************************************************************
STRUCT LogBook : Component {
  INTEGER {SCALAR}
    projectId
  ;
  STRING {EDITABLE, SCALAR}
    text
  ;
};

STRUCT LogBookPosition {
  INTEGER
    start
  , end
  , index
  ;
};

STRUCT LogBookFilter {
  INTEGER
    projectId {SCALAR}
  ;
  STRING
    approval
  ;
};

STRUCT LogBookInfo {
  INTEGER {EDITABLE, SCALAR}
    new  {BUTTON, FUNC=logbook_new_func}
  , edit {BUTTON, FUNC=logbook_edit_func}
  , save {BUTTON, FUNC=logbook_save_func}
  , load {BUTTON, FUNC=logbook_get_func}
  , inx  {LABEL=_("Entry #"), FUNC=logbook_edit_index_func}
  ;
  INTEGER
    size
  , id
  ;
  STRING {SCALAR}
    text {FUNC=logbook_select_func}
  ;
  LogBookPosition
    position
  ;
  LogBookFilter {SCALAR}
    filter
  ;
};
LogBookInfo {SCALAR}
  logbookInfo
;
LogBook
  logbook
;
LogBook {SCALAR}
  logbookEntry
, logbookDb {PERSISTENT}
;
