// **********************************************************************
// User
// **********************************************************************
FUNC user_load_func {
  CLEAR(user);

  IF(!VALID(RESTUSERNAME) || RESTUSERNAME == "") {
    RETURN;
  }

  // IF : AVOID ABORT INIT due to CURL error (DB not found/running) and login canceled
  IF([user] = GET(
       PATH=["users/", RESTUSERNAME]
     )
    ) {
    // User may have changed username
    // -> if so, reload user information
    IF(RESTUSERNAME != user.username) {
      [user] = GET(
        PATH=["users/", RESTUSERNAME]
      );
    }
  }
};

FUNC user_check_password_func {
  IF(user.password == user.rep_password) {
    ENABLE(user.update_password);
  } ELSE {
    DISABLE(user.update_password);
  }
};

FUNC user_updateUserPassword_func {
  IF(user.password != user.rep_password) {
    MESSAGEBOX("Password is not correct");
    ABORT;
  }

  [user] = PUT(PATH="users/password", DATA=[user.password]);
  IF(!VALID(user)) {
    MESSAGEBOX("Could not change password");
    ABORT;
  }
  DISABLE(user.update_password);
  MESSAGEBOX("Password successfully updated");
};
