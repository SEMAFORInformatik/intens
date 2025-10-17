// **********************************************************************
// Variant
// **********************************************************************
FUNC variant_init_func {
  variant_select.experimental  = 1;
  variant_select.inPreparation = 1;
  variant_select.shared        = 1;
  variant_select.approved      = 1;
  variant_select.tested        = 1;
  variant_select.obsolete      = 0;
};

FUNC variant_select_filter_refresh_func {
  INTEGER i;

  CLEAR(variant_select.filter);
  ASSIGN_CORR(variant_select.filter, variant_select );

  // approval
  i = 0;
  IF(variant_select.experimental == 1) {
    variant_select.filter.approval[i] = "experimental";
    i++;
  }
  IF(variant_select.inPreparation == 1) {
    variant_select.filter.approval[i] = "inPreparation";
    i++;
  }
  IF(variant_select.shared == 1) {
    variant_select.filter.approval[i] = "shared";
    i++;
  }
  IF(variant_select.approved == 1) {
    variant_select.filter.approval[i] = "approved";
    i++;
  }
  IF(variant_select.tested == 1) {
    variant_select.filter.approval[i] = "tested";
    i++;
  }
  IF(variant_select.obsolete == 1) {
    variant_select.filter.approval[i] = "obsolete";
    i++;
  }

  // owner
  IF(variant_select.myVariants == 1) {
    variant_select.filter.owner = RESTUSERNAME;
  }

  // maxAge
  IF(variant_select.excludeOld == 1) {
    variant_select.filter.maxAge = 3;
  }
};

FUNC variant_select_refresh_func {
  INTEGER i, size;

  CLEAR(variant_select.filter);

  // projectId
  IF(!VALID(project.id)) {
    RETURN;
  }

  variant_select.projectId = project.id;
  RUN( variant_select_filter_refresh_func );

  [project.variant[#]] = GET(
    PATH="variants?_projection=(desc,rev,changed)&_ignorecase&_sort=index:1"
  , FILTER=[variant_select.filter]
  );

  IF( VISIBLE(project_select_form) ){
    // select current variant in list, if found
    IF(VALID(variant.id)) {
      SIZE(project.variant, size);
      i = 0;
      WHILE(i < size) {
        IF(project.variant[i].id == variant.id) {
          SELECT_LIST(variant_select_list, i);
          size = -1; // break
        }
        i++;
      }
    }
  }
};

FUNC variant_select_import_refresh_func {
  variant_select.projectId = import_project.id;
  RUN( variant_select_filter_refresh_func );

  [import_project.variant[#]] = GET (
    PATH="variants?_projection=(desc,rev,changed)&_ignorecase&_sort=index:1"
  , FILTER=[variant_select.filter]
  );
};

FUNC variant_load_from_dbId_func {
  [variant] = GET(
    PATH=["variants/", dbId]
  );
  CLEAR(variant.current_version);
  RUN( variant_after_load_func );

  // load components
  RUN(g_variant_load_components_func);

  // update project overview
  RUN(variant_update_func);
};

FUNC variant_select_list_func {
  IF(REASON_ACTIVATE) {
    UNMAP(project_select_form);
    dbId = project.variant[INDEX].id;
    RUN(variant_load_from_dbId_func);
  }
};

FUNC variant_select_import_list_func {
  IF( REASON_ACTIVATE ) {
    UNMAP( project_select_import_form );
    dbId = import_project.variant[INDEX].id;
    RUN ( variant_load_from_dbId_func );
    variant.id = INVALID;
    variant.name = INVALID;
    SET(COLOR, variant.name, 4);
  }
};

FUNC variant_rename_func {
  IF ( !VALID(THIS.id) ) {
    MESSAGEBOX ( _("This variant cannot be renamed because it has not been saved.") );
    RETURN;
  }

  CLEAR ( element_rename );
  element_rename.type = "Variant";
  element_rename.id = THIS.id;
  element_rename.newName = THIS.name;

  RUN ( element_rename_show_func );
};

FUNC variant_show_modifications_func {
  CLEAR(modifications);
  ASSIGN_CORR(modifications.variant, THIS);
  modifications.type = "variant";
  RUN(modifications_show_func);
};

FUNC variant_nav_func {
  IF ( REASON_DROP ) {
    IF ( CLASSNAME(SOURCE) == "variant" ) {
      IF ( SOURCE.id == activeVariant.id ){
        RETURN;
      }
      dbId = SOURCE.id;
      project_library.whichButtonPressed = "drop_variant";
      RUN ( project_store_func );
      IF (VALID(project_library.whichButtonPressed[1])) {
        RETURN;
      }
      RUN ( variant_load_from_dbId_func );
    }
  }
};

FUNC variant_project_variant_nav_func {
  RETURN;
};

FUNC variant_update_func {
  RUN( g_variant_update_func );
  activeVariant = variant;
};

FUNC variant_name_func {
  INTEGER save;

  IF( VALID(THIS.name) ){
    SET(COLOR, THIS.name, 0);
  }

  IF ( VALID(THIS.id) ) {
    save = CONFIRM_CANCEL(
      "Variant name has changed!"
    , BUTTON_YES=_("Create new Variant")
    , BUTTON_NO=_("Rename Variant")
    );

    IF ( !VALID(save) ) { // Cancel
      ABORT;
    }
    IF ( save == 1) { // Yes => new variant
      THIS.id = INVALID;
      RETURN;
    }
    IF ( save == 0 ) { // No => rename
      CLEAR ( element_rename );
      element_rename.type = "Variant";
      element_rename.pathType = "variants";
      element_rename.id = THIS.id;
      element_rename.newName = THIS.name;
      element_rename.oldName = OLDVALUE;

      RUN ( element_rename_rename_func );
    }
  }
};

FUNC variant_new_pressed_func {
  CLEAR(project_library);
  RUN(variant_new_func);
};

FUNC variant_new_func {
  IF( !VALID(project.name) ) {
    MESSAGEBOX("No project available");
    RETURN;
  }

  project_library.whichButtonPressed = "newvar";
  // call project_store_func, only if not called by projectSaveComponent_form
  IF (! VALID(project_library.whichButtonPressed[1])) {
    RUN ( project_store_func );
    IF (VALID(project_library.whichButtonPressed[1])) {
      RETURN;
    }
  }

  CLEAR ( variant );
  SET(COLOR, variant.name, 0);
  RUN ( g_project_clear_allcomp_func );
  RUN ( g_project_init_allcomp_func );

  RUN ( variant_update_func );
  SET_DB_TIMESTAMP ( variant );
};

FUNC variant_after_load_func {
  IF( !VALID(variant.current_version ) ){
    variant.current_version = variant.version;
  }
  IF( variant.current_version == variant.version) {
    variant.detached = 0;
    SET( COLOR, variant, 0 );
    ENABLE( variant );
  }
  ELSE{
    variant.detached = 1;
    SET( COLOR, variant, 5 );
    DISABLE( variant );
  }
};