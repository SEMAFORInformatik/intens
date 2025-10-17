// **********************************************************************
// User
// **********************************************************************
FIELDGROUP user_fg (
  _("Connection") RESTBASE:35 {COLSPAN=2}
, LABEL(user.username)    user.username:10
, LABEL(user.firstName)   user.firstName:20
, LABEL(user.lastName)    user.lastName:20
, LABEL(user.enabled)     user.enabled
, LABEL(user.activeGroup) user.activeGroup.name:20
, VOID {COLSPAN=2}
, _("Change Password")     {COLSPAN=2}
, LABEL(user.password)     user.password:25
, LABEL(user.rep_password) user.rep_password:25  user.update_password
);

FIELDGROUP user_groupsAndRoles_fg {
  TABLESIZE=5
, RANGE(1, 9)
, TABLE ORIENTATION=VERTICAL
, MENU=HIDDEN
} (
  LABEL(user.groups) user.groups[*].name:20
, LABEL(user.roles)  user.roles[*].name:20
);

FORM user_form {
  _("Database Connection")
, HIDE_CYCLE
, APP_MODAL
, HIDDEN
} (
  user_fg
, user_groupsAndRoles_fg
);
