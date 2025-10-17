// Project
// --------------------------------------

// FUNCTIONS generated for persistent objects
//#include <g_project_functions.inc>

FUNC project_init_func {
  project_select.local = 1;
  project_select.division = 1;
  project_select.global = 1;
  project_select.delivered = 1;
  project_select.obsolete = 0;

  DISABLE(project_overview_top_section_fg);
};

FUNC project_name_func {
  INTEGER save;

  IF(VALID(THIS.id)) {
    save = CONFIRM_CANCEL(
      _("Workbook name has changed!")
    , BUTTON_YES=_("Create new Workbook")
    , BUTTON_NO=_("Rename Workbook")
    );

    IF(!VALID(save)) {  // Cancel
      ABORT;
    }
    IF(save == 1) {  // Yes => new project
      THIS.id = INVALID;
      RETURN;
    }
    IF(save == 0) {  // No => rename
      CLEAR(element_rename);
      element_rename.type = "Project";
      element_rename.pathType = "projects";
      element_rename.id = THIS.id;
      element_rename.newName = THIS.name;
      element_rename.oldName = OLDVALUE;

      RUN(element_rename_rename_func);
    }
  }
};

FUNC project_rename_func{
  IF (!VALID(THIS.id)){
    MESSAGEBOX( _("This workbook cannot be renamed because it has not been saved."));
    RETURN;
  }

  CLEAR(element_rename);
  element_rename.type    = "Project";
  element_rename.id      = THIS.id;
  element_rename.newName = THIS.name;

  RUN(element_rename_show_func);
};

FUNC project_show_modifications_func{
  CLEAR(modifications);
  ASSIGN_CORR(modifications.project, THIS);
  modifications.type = "project";
  RUN(modifications_show_func);
};

FUNC project_open_pressed_func {
  CLEAR(project_library);
  RUN(project_open_func);
};

FUNC project_open_func {
  // We want open a project, don't check project name and variant name
  project_library.whichButtonPressed = "open";
  // call project_store_func, only if not called by project_save_data_form
  IF (!VALID(project_library.whichButtonPressed[1])){
    RUN(project_store_func);
    IF (VALID(project_library.whichButtonPressed[1])){
      RETURN;
    }
  }

  RUN(project_saveIfModified_func);

  RUN(g_project_clear_allcomp_func);
  RUN(g_project_init_allcomp_func);

  MAP(project_select_form);
  RUN(project_select_refresh_func);
  RUN(variant_select_refresh_func);
};

FUNC project_import_open_pressed_func {
  CLEAR(project_library);
  RUN(project_import_open_func);
};

FUNC project_import_open_func {
  IF( !VALID(project.name) ) {
    MESSAGEBOX("No project available");
    RETURN;
  }

  CLEAR( import_project );
  project_library.whichButtonPressed = "import";
  // call project_store_func, only if not called by projectSaveComponent_form
  IF (! VALID(project_library.whichButtonPressed[1])){
    RUN(project_store_func);
    IF (VALID(project_library.whichButtonPressed[1])){
      RETURN;
    }
  }
  //

  MAP(project_select_import_form);
  RUN(project_select_import_refresh_func);
};

FUNC project_new_pressed_func{
  CLEAR(project_library);
  RUN(project_new_func);
};

FUNC project_new_func{
  project_library.whichButtonPressed = "new";
  // call project_store_func, only if not called by project_save_data_form
  IF (!VALID(project_library.whichButtonPressed[1])){
    RUN(project_store_func);
    IF (VALID(project_library.whichButtonPressed[1])){
      RETURN;
    }
  }

  RUN(project_saveIfModified_func);
  CLEAR(project, variant, logbook, logbookInfo);
  RUN(g_project_clear_allcomp_func);
  RUN(g_project_init_allcomp_func);
};

FUNC project_store_pressed_func{
  CLEAR(project_library);
  RUN(project_store_func);
};

FUNC project_store_func{
  INTEGER size, failed, existing_projects, found;
  INTEGER showbox, i, save, show_reason_comment, variantSave;
  STRING s;

  // Here we must have an project.name and variant.name
  //-------------------------------------------------------
  project_library.whichButtonPressed[1] = project_library.whichButtonPressed; // [1] called from
  project_library.whichButtonPressed = "store";

  // check
  // -----
  //  To avoid an interrupted save, all checks have to pass before saving

  // get status
  allStatusIndex = 0;
  RUN(g_project_get_status_allcomp_func);
  RUN(g_project_get_status_func);
  RUN(g_variant_get_status_func);

  // check status
  size = allStatusIndex;
  allStatusIndex = 0;
  showbox = 0;
  failed = 0;
  show_reason_comment = 0;
  variantSave = 0;
  CLEAR(project_save_data[*]);

  WHILE(allStatusIndex < size){
    IF (VALID(allStatus[allStatusIndex].type) &&
        (allStatus[allStatusIndex].status == "modified" ||
         allStatus[allStatusIndex].status == "upToDateConflict" ||
         allStatus[allStatusIndex].status == "new" ||
         allStatus[allStatusIndex].status == "error")){
      ASSIGN_CORR(project_save_data[showbox], allStatus[allStatusIndex]);
      project_save_data[showbox].desc = allStatus[allStatusIndex].substatus;

      project_save_data[showbox].save = 1; // save
      ENABLE(project_save_data[showbox].save);
      IF (project_save_data[showbox].type == "Variant") variantSave = 1;

      // show reason comment field?
      IF (allStatus[allStatusIndex].status == "modified" ||
          allStatus[allStatusIndex].status == "upToDateConflict") {
        show_reason_comment = 1;
      }

      IF(allStatus[allStatusIndex].status == "error"){
        failed++;
        project_save_data[showbox].save = 0;
        DISABLE(project_save_data[showbox].save);
      }
      showbox += 1;
    }
    allStatusIndex = allStatusIndex + 1;
  }

  // if new component exists,
  // variant is modified AND needs a modidy reason
  IF (show_reason_comment == 0 && showbox > 0 && variantSave == 0) {
    show_reason_comment = 1; // we need comment for variant modify
  }

  //
  // get projects of this name and check id
  // --------------------------------------
  IF( VALID(project.name) && VALID( variant.name ) ){
    [db_comp2[#]] = GET(PATH=["projects?name=", project.name] );
    SIZE(db_comp2[*], existing_projects);
    IF( VALID(project.id) ){
      i=0;
      found=0;
      WHILE( i<existing_projects ){
        IF( db_comp2[i].id == project.id ){
          existing_projects=0; // break
          found=1;
        }
        i+=1;
      }
      IF( found==0 ){
        CLEAR(project.id);
      }
    }
    CLEAR( db_comp2 );
  }

  //
  // store
  // -----------------------------
  IF(showbox == 0){
    // reset called from, stored in [1]
    project_library.whichButtonPressed[1] = INVALID;
    RETURN;
  }

  // store components
  IF (VALID(project.reason)){
    save = 1;
    RUN(project_save_data_dialog_save_func); // save button pressed
  }
  ELSE { //  fill data for project_save_data Dialog

    //  project_save_data Dialog labels
    //  --------------------------------
    project_save_data.cancel_btn = _("Cancel"); // set label of cancel button
    IF (!VALID(project_library.whichButtonPressed[1]) ||
        project_library.whichButtonPressed[1] == "store") {
      project_save_data.cancel_btn = ""; // invisible
    }
    // projects with project.name found, but none of them have project.id
    IF( existing_projects > 0 ) {
      s = "<h3>" + project.name + "</h3>";
      project_save_data.title = COMPOSE(_("There are <b>%1</b> other workbook(s) with the name %2"), existing_projects, s);
      project_save_data.title += "<b>" + _("The workbook has unsaved modifications:") + "</b>";
      project_save_data.footer = _("Do you want to save a NEW workbook?");
      project_save_data.no_save_btn = "No Saving";
    } ELSE {
      project_save_data.title = "<b>" + _("The workbook has unsaved modifications:") + "</b>";
      project_save_data.footer = _("Do you want to save the workbook?");
      project_save_data.no_save_btn = _("No");
    }

    // from by quit function
    IF (project_library.whichButtonPressed[1] == "close") {
      s = "<table cellpadding='5', cellspacing='0', border='1'>\n"+
          "<tr><th>Name</th><th>Type</th><th>Status</th></tr>\n";
      // get max string len
      i = 0;
      WHILE(VALID(project_save_data[i].type)) {
        s = s + COMPOSE("<tr><td>%1</td><td>%2</td><td>%3</td></tr>\n",
        project_save_data[i].name, project_save_data[i].type, project_save_data[i].status);
        i += 1;
      }
      IF (i == 0) {
        s = "";
      } ELSE {
        s = s + "</table><br/>\n";
      }
      // MAP(project_save_data_form) not possible because we are in QUIT
      save = CONFIRM( "<h3>Workbook has unsaved modifications.</h3>"+s
                    , BUTTON_YES=_("Do not exit")
                    , BUTTON_NO=_("Exit without saving"));
      IF (save != 1) { // means not saving later, abort quit
        RETURN;
      }
      ABORT;
    }

    // project_save_data set list length, map/unmap comment_fg
    // -------------------------------------------------------
    RANGE(project_save_data_fg, 1, showbox);
    IF (show_reason_comment == 1) { // with modify comment?
      MAP(project_save_data_modify_comment_fg);
    } ELSE {
      UNMAP(project_save_data_modify_comment_fg);
    }

    // project_save_data, fill project list and map dialog
    // ---------------------------------------------------
    // compInProjects.vcomp = "<varcomp>"
    // compInProjects.nfound=0;
    // RUN( used_in_projects_find_func );
    // IF( compInProjects.nfound > 0 ){
    //   SIZE( project_save_data.project[*], size);
    //   RANGE(project_save_data_used_in_projects_fg, 1, size);

    //   MAP(project_save_data_used_in_projects_fg);
    // } ELSE {
    //   UNMAP(project_save_data_used_in_projects_fg);
    // }

    // map dialog
    MAP(project_save_data_form);
  }
};

FUNC project_overview_map_func {
  MAP(project_overview_form);
};

FUNC project_saveIfModified_func {  // ask to save modifications if any
  INTEGER size, save;

  IF( project_library.dont_save == 1 ){
    RETURN;
  }

  allStatusIndex = 0;
  RUN(g_project_get_status_func);
  IF(allStatus[allStatusIndex - 1].status == "notModified") {
    RUN(g_variant_get_status_func);
    IF(allStatus[allStatusIndex - 1].status == "notModified") {
      RUN(g_project_check_modified_anycomp_func);
    }
  }

  // check status (modified or new -> confirm to save
  IF(allStatus[allStatusIndex - 1].status != "notModified") {
    RUN(project_store_func);
  }
};

FUNC project_select_filter_refresh_func {
  INTEGER i;

  CLEAR( project_select.filter );
  ASSIGN_CORR(project_select.filter, project_select); // name

  // status
  i = 0;
  IF(project_select.local == 1) {
    project_select.filter.status[i] = "local";
    i++;
  }
  IF(project_select.division == 1) {
    project_select.filter.status[i] = "division";
    i++;
  }
  IF(project_select.global == 1) {
    project_select.filter.status[i] = "global";
    i++;
  }
  IF(project_select.delivered == 1) {
    project_select.filter.status[i] = "delivered";
    i++;
  }
  IF(project_select.obsolete == 1) {
    project_select.filter.status[i] = "obsolete";
    i++;
  }

  // owner
  IF(project_select.myProjects == 1) {
    project_select.filter.owner = RESTUSERNAME;
  }

  // maxAge
  IF(project_select.excludeOld == 1) {
    project_select.filter.maxAge = 3;
  }

  [project_select.list[#]] = GET (
    PATH="projects?_projection=(changed)&_ignorecase"
  , FILTER=[project_select.filter]
  );
};

FUNC project_select_refresh_func {
  INTEGER i, size;

  RUN( project_select_filter_refresh_func );
  CLEAR(project.variant);

  // select current project in list, if found
  IF(VALID(project.id)) {
    SIZE(project_select.list, size);
    i = 0;
    WHILE(i < size) {
      IF(project_select.list[i].id == project.id) {
        SELECT_LIST(project_select_list, i);
        size = -1;  // break
      }
      i++;
    }
  }
};

FUNC project_select_import_refresh_func {
  RUN( project_select_filter_refresh_func );
};

FUNC project_select_list_func {
  IF(REASON_ACTIVATE) {
    dbId = project_select.list[INDEX].id;

    [project] = GET(
      PATH=["projects/", dbId]
    );

    RUN(variant_select_refresh_func);
    RUN(logbook_get_func);
  }
};

FUNC project_select_import_list_func {
  IF ( REASON_ACTIVATE ) {
    dbId = project_select.list[INDEX].id;

    [ import_project ] = GET (
      PATH=["projects/",dbId]
    );

    CLEAR(import_project.variant);
    RUN ( variant_select_import_refresh_func );
  }
};

// Project Overview
// -----------------
FUNC project_overview_form_func {
  IF(REASON_ACTIVATE) {
    RUN(variant_update_func);
  }
};

FUNC projectFillApprovalReport_func {
  RETURN;
};

// Project Save Component Dialog
// --------------------------------
FUNC project_component_varname_func {
  INTEGER j;
  STRING projComps;
  INTEGER navIndexes;

  j = 0;
  projComps[j] = "Project"; projComps[1,j] = "project"; navIndexes[j] = 0; j++;
  projComps[j] = "Variant"; projComps[1,j] = "variant"; navIndexes[j] = 0; j++;
// BEGIN BLOCK COMPONENT
//  projComps[j] = "<componentType>"; projComps[1,j] = "<component>"; navIndexes[j] = 0; j++;
// END BLOCK COMPONENT

  // variable components
  j = 0;
  WHILE (VALID(projComps[j])) {
    IF (project_component_classname == projComps[j]) {
      project_component_varname = projComps[1, j]+"[" + navIndexes[j] + "]";
      project_component_library_varname = projComps[1, j]+"Library";
      RETURN;
    }
    j += 1;
  }
};

FUNC project_save_data_dialog_recall_func {
  // recall function which do MAP(project_save_data_form)
  IF (project_library.whichButtonPressed[1] == "new") {
    RUN(project_new_func);
  }
  IF (project_library.whichButtonPressed[1] == "open") {
    RUN(project_open_func);
  }
  IF (project_library.whichButtonPressed[1] == "import") {
    RUN(project_import_open_func);
  }
  IF (project_library.whichButtonPressed[1] == "newvar") {
    RUN(variant_new_func);
  }
  IF (project_library.whichButtonPressed[1] == "drop_variant") {
    RUN(variant_load_from_dbId_func);
  }
  // reset all
  CLEAR(project_library);
};

FUNC project_save_data_dialog_save_func {
  // Save Button pressed
  INTEGER size, i,j;
  STRING projComps, s;
  INTEGER compSave;

  i = 0;
  projComps[i] = "Project"; i++;
  projComps[i] = "Variant"; i++;
// BEGIN BLOCK COMPONENT
//  projComps[i] = "<componentType>"; i++;
// END BLOCK COMPONENT

  i = 0;
  WHILE(VALID(project_save_data[i].status)){
    // variable components
    j = 0;
    compSave = 0;
    WHILE (VALID(projComps[j])) {
      IF(project_save_data[i].type == projComps[j]) {
        project_component_classname = projComps[j];
        RUN(project_component_varname_func);
        IF (project_save_data[i].save == 1) {
          VAR(project_component_library_varname).saveCanceled = 0;
          compSave = 1;

          IF (project_save_data[i].status == "new") {
            VAR(project_component_varname).reason = _("Component created.");
          } ELSE IF (project_save_data[i].status == "modified") {
            IF ( !VALID(project_save_data.reason)) {
              project_save_data.reason = "";
            }
            VAR(project_component_varname).reason = project_save_data.reason;
          }
        } ELSE {
          VAR(project_component_varname).modification = "notModified";
          VAR(project_component_library_varname).saveCanceled = 1;
        }
      }
      j += 1;
    }
    i += 1;
  }

  UNMAP(project_save_data_form);
  IF (compSave == 0){
    // MESSAGEBOX("Nothing to save");
    RETURN;
  }

  IF( !VALID(project.name) && !VALID(variant.name)){
    RETURN;
  }

  RUN(g_project_save_allcomp_func);

  // store project
  project.type = "Project";
  [ project ] = PUT (
    PATH="projects"
  , DATA=[ project ]
  );

  // store variant
  IF(variant.detached == 1){
    RETURN;
  }
  IF (!VALID(project.id) || !VALID(variant.name)){
    RETURN;
  }
  variant.projectId = project.id;

  RUN (variant_update_func);

  variant.type = "Variant";

  variant[1] = variant[0];
  IF ( [ variant[0] ] = PUT(
    PATH="variants" // TODO "variants"
  , DATA=[ variant[0] ])){
    // load components:
    //   needed if variant (PUT) returned an upToDateConflict and Load was clicked
    //   because the loaded variant version may use different components
    RUN(g_variant_check_different_components_func);
    IF (VALID(variant[1])) {
      PRINT("reload variant components");
      RUN(g_variant_load_components_func);
    }
  }
  CLEAR(variant[1]);

  RUN(variant_update_func); // set all labels again
  SET_DB_TIMESTAMP(variant);

  RUN(variant_select_refresh_func); // update project overview (left part)
  RUN(project_save_data_dialog_recall_func);
};

FUNC project_save_data_dialog_nosave_func{
  // No Button pressed
  UNMAP(project_save_data_form);
  project_library.dont_save = 1;
  RUN(project_save_data_dialog_recall_func);
};

FUNC project_save_data_dialog_cancel_func{
  // Cancel Button pressed
  UNMAP(project_save_data_form);
  // reset all
  CLEAR(project_library);
};
