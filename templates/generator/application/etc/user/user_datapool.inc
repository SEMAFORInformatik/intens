// **********************************************************************
// User
// **********************************************************************
STRUCT UserName {
  STRING {SCALAR} name;
};

STRUCT User {
  STRING {SCALAR} username {LABEL=_("Username"), LABEL};
  STRING {SCALAR} firstName {LABEL=_("First Name")};
  STRING {SCALAR} lastName {LABEL=_("Last Name")};

  // user only can change password they never see the password
  STRING {SCALAR, EDITABLE}
    password {
      LABEL=_("New password")
    , PASSWORD
    , FUNC=user_check_password_func
    }
  , rep_password {
      LABEL=_("repeat new password")
    , PASSWORD
    , FUNC=user_check_password_func
    }
  ;
  STRING {SCALAR}
    update_password {
      LABEL=_("Update password")
    , BUTTON
    , FUNC=user_updateUserPassword_func
    }
  ;
  INTEGER {SCALAR} enabled {LABEL=_("Enabled"), TOGGLE};

  UserName {SCALAR} activeGroup {LABEL=_("Active Group")};
  UserName groups {LABEL=_("Groups")};
  UserName roles {LABEL=_("Roles")};
};

User
  user {SCALAR}
;
