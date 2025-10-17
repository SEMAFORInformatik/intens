// **********************************************************************
// LogBook
// **********************************************************************

TASK logbook_save_task {
  _("Save")
, HIDDEN
, SILENT
, FORM=logbook_entry_form
} {
  RUN ( logbook_save_func );
};

TASK logbook_cancel_task {
  _("Cancel")
, HIDDEN
, SILENT
, FORM=logbook_entry_form
} {
  UNMAP( logbook_entry_form );
};
