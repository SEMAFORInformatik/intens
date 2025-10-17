// **********************************************************************
// LogBook
// **********************************************************************

FIELDGROUP logbook_action_fg { "LogBook" }
(
  logbookInfo.new {PIXMAP="new"}
  logbookInfo.edit {PIXMAP="open"}
  // logbookInfo.save {PIXMAP="store"}
  // logbookInfo.load {PIXMAP="search"}
);

FIELDGROUP logbook_text_fg
(
  logbookInfo.text:80:20 {EXPAND}
);

FIELDGROUP logbook_entry_fg {
  _("Logbook Entry")
} (
  LABEL(logbookEntry.created)      logbookEntry.created:12   {COLSPAN=2} VOID(5,0)
  LABEL(logbookEntry.ownername)    logbookEntry.ownername:20 {COLSPAN=2} VOID(5,0)
  LABEL(logbookInfo.inx)           logbookInfo.inx:5
  //logbookInfo.new {PIXMAP="new"}
  //logbookInfo.edit {PIXMAP="open"}
  //logbookInfo.save {PIXMAP="store"}
, LABEL(logbookEntry.changed)      logbookEntry.changed:12     {COLSPAN=2} VOID(5,0)
  LABEL(logbookEntry.changername)  logbookEntry.changername:20 {COLSPAN=2}
);

FIELDGROUP logbook_entry_text_fg
(
  logbookEntry.text:80:20
);

FIELDGROUP logbook_pos_fg {
  TABLESIZE=5
}
(
  "start" logbookInfo.position[*].start
, "end"   logbookInfo.position[*].end
);

FORM logbook_entry_form {
  "Logbook Entry"
, HIDDEN
, HIDE_CYCLE
, CLOSE_BUTTON=NONE
}
(
  logbook_entry_fg
, logbook_entry_text_fg
//, logbook_pos_fg
);
