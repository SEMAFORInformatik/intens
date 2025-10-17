//
// Element Rename
// ---------------------------------------
FUNC element_rename_show_func {
  // set pathType
  // We have to do the GET here, because intens can not have '/' in a variable in the PATH / -> %2f
  IF(element_rename.type == "Variant") {
    element_rename.pathType = "variants";
    [compList[#]] = GET(PATH=[element_rename.pathType, "?id=", element_rename.id]);
  } ELSE IF(element_rename.type == "Project") {
    element_rename.pathType = "projects";
    [compList[#]] = GET(PATH=[element_rename.pathType, "?id=", element_rename.id]);
  } ELSE {
    element_rename.pathType = "components";
    [compList[#]] = GET(PATH=["components/type/", element_rename.type, "?id=", element_rename.id]);
  }

  // set title
  element_rename.title = "<b>" + element_rename.type + "</b>";

  IF(!VALID(compList)) {
    RETURN;
  }
  element_rename.oldName = compList[0].name;

  MAP(element_rename_form);
};

FUNC element_rename_close_func {
  UNMAP(element_rename_form);
};

FUNC element_rename_rename_func {
  INTEGER c, currentCycle;

  // check id
  IF(!VALID(element_rename.id)) {
    MESSAGEBOX(_("Unexpected error::") + EOLN
    + "element_rename_rename_func" + EOLN
    + _("called with invalid element_rename.id")
    );
    RUN(element_rename_close_func);
    RETURN;
  }

  // check newName
  IF(!VALID(element_rename.newName) ||
     element_rename.newName == "" ||
     element_rename.newName == element_rename.oldName
    ) {
    PRINT(_("Rename action ignored (same or invalid name)."));
    RUN(element_rename_close_func);
    RETURN;
  }

  [renamedElement] = PUT(
    PATH=[element_rename.pathType, "/", element_rename.id, "/rename"]
  , DATA=[element_rename.newName]
  );

  // update name in user session (all cycles)
  currentCycle = GETCYCLE;

  c = 1; // cycles start with 1
  WHILE(MAXCYCLE >= c) {
    GOCYCLE(c);

    IF(element_rename.type == "Variant") {
      IF(variant.id == element_rename.id) {
        variant.name = element_rename.newName;
        variant.version = renamedElement.version;
        SET_DB_TIMESTAMP(variant.name);
        SET_DB_TIMESTAMP(variant.version);
      }
      RUN(variant_select_refresh_func); // project overview

    } ELSE IF(element_rename.type == "Project") {
      IF(project.id == element_rename.id) {
        project.name = element_rename.newName;
        project.version = renamedElement.version;
        SET_DB_TIMESTAMP(project.name);
        SET_DB_TIMESTAMP(project.version);
      }

    } ELSE {
      // find the correct component and set the new name
      // Then load the new component so there is no up to date conflict
      RUN(component_refresh_func);
    }
    c++;
  }

  GOCYCLE(currentCycle);

  RUN(element_rename_close_func);
};

// For every component the same
// avoid duplicate code
FUNC component_rename_func {
  element_rename.id = THIS.comp_id;
  element_rename.newName = THIS.name;
  RUN(element_rename_show_func);
};
