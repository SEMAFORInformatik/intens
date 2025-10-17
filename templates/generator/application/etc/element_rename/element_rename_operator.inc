// **********************************************************************
// Element Rename
// **********************************************************************
TASK element_rename_rename_task {
  _("Rename")
, FORM=element_rename_form
, HIDDEN
, SILENT
} {
  RUN(element_rename_rename_func);
};
