// **********************************************************************
// Element Rename
// **********************************************************************
FIELDGROUP element_rename_fg (
  element_rename.title:60 {COLSPAN=2}

, LABEL(element_rename.oldName)
  LABEL(element_rename.newName)

, element_rename.oldName:30
  element_rename.newName:30
);

FORM element_rename_form {
  _("Rename")
, APP_MODAL
, HIDDEN
, HIDE_CYCLE
} (
  element_rename_fg
);
