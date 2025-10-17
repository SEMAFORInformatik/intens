<% o = object + '[navIndex_' + id + ']' %>
FUNC g_${id}_select_refresh_func {
  INTEGER i;

  CLEAR(${id}_select.filter);

  // name
  ${id}_select.filter.name = ${id}_select.name;

  // approval
  i=0;
  IF(${id}_select.experimental == 1) {
    ${id}_select.filter.approval[i] = "experimental";
    i++;
  }
  IF(${id}_select.inPreparation == 1) {
    ${id}_select.filter.approval[i] = "inPreparation";
    i++;
  }
  IF(${id}_select.shared == 1) {
    ${id}_select.filter.approval[i] = "shared";
    i++;
  }
  IF(${id}_select.approved == 1) {
    ${id}_select.filter.approval[i] = "approved";
    i++;
  }
  IF(${id}_select.tested == 1) {
    ${id}_select.filter.approval[i] = "tested";
    i++;
  }
  IF(${id}_select.obsolete == 1) {
    ${id}_select.filter.approval[i] = "obsolete";
    i++;
  }

  // owner
  IF(${id}_select.my${type}s == 1) {
    ${id}_select.filter.owner = RESTUSERNAME;
  }

  // maxAge
  IF(${id}_select.excludeOld == 1) {
    ${id}_select.filter.maxAge = 3;
  }

  [${id}_select.list[#]] = GET(
    PATH="components/type/${type}?_projection=(desc,changed,changername,modcomment)&_ignorecase"
  , FILTER=[${id}_select.filter]
  );
};

FUNC g_${id}_select_func {
  RUN(g_${id}_select_refresh_func);
  loadType = "reload";
  MAP(g_${id}_select_form);
};

FUNC g_${id}_new_func {
  % if singleton:
  VariantComponent vc;
  navIndex_${id}=0;
  RUN(g_${id}_saveIfModified_func);
  CLEAR(${object});
  ASSIGN_CORR(variant.${object}, vc);  // better than CLEAR(variant.${object});
  % else:
  INTEGER size;
  SIZE(${object}[*], size);
  navIndex_${id}=size;
  % endif
  RUN(${id}_navIndex_func);
  RUN( ${id}_after_load_func );
  SET_DB_TIMESTAMP(${o});
};

FUNC g_${id}_remove_func {
  % if singleton:
  VariantComponent vc;
  navIndex_${id}=0;
  RUN(g_${id}_saveIfModified_func);
  CLEAR(${object});
  ASSIGN_CORR(variant.${object}, vc); // better than CLEAR(variant.${object});
  RUN(${id}_navIndex_func);
  SET_DB_TIMESTAMP(${o});
  % else:
  INTEGER size;
  VariantComponent vc;
  SIZE(${object}[*], size);
  IF(navIndex_${id}<size){
    RUN(g_${id}_saveIfModified_func);
    CLEAR(${id}[navIndex_${id}]);
    PACK(${id}[*]);
    ASSIGN_CORR(variant.${id}[navIndex_${id}], vc);  // better than CLEAR(variant.${id}[navIndex_${id}]);
    PACK(variant.${id}[*]);
  }
  % endif
};

FUNC ${id}_after_load_func {
  IF( !VALID(${id}[navIndex_${id}].current_version ) ){
    ${id}[navIndex_${id}].current_version = ${id}[navIndex_${id}].version;
  }
  IF(${id}[navIndex_${id}].current_version == ${id}[navIndex_${id}].version) {
    ${id}[navIndex_${id}].detached = 0;
    SET( COLOR, ${id}[navIndex_${id}], 0 );
    ENABLE( ${id}[navIndex_${id}] );
  }
  ELSE{
    ${id}[navIndex_${id}].detached = 1;
    SET( COLOR, ${id}[navIndex_${id}], 5 );
    DISABLE( ${id}[navIndex_${id}] );
  }
};

FUNC g_${id}_load_from_dbcomp_func{
  INTEGER new, load, i, size, save;
  INTEGER notModifiedHere;
  Component comp;
  comp = db_comp;

  LOG(DEBUG, "g_${id}_load_from_dbcomp_func  id:", db_comp.id);
  IF(!VALID(comp.id)){
    LOG(INFO, "g_${id}_load_from_dbcomp_func  no comp.id");
    RETURN;
  }

  new = 1; load = 1;
  // check if component is already loaded
  SIZE(${object}[*], size);
  i = 0;
  WHILE(i < size){
    LOG(DEBUG, "g_${id}_load_from_dbcomp_func  size:", size, ", ids: ", ${object}[i].id, " == ", comp.id);
    IF(${object}[i].id == comp.id){
      new = 0; load = 0;
      navIndex_${id} = i;

      IF(loadType == "reload"){
        // check if the component is modified
        IF(!MODIFIED(${o})){
          notModifiedHere = 1;
          SET(TIMESTAMP, ${o}.name);  // force MODIFY Check (${o} may have been modified externally)
        }
        RUN(g_${id}_get_status_func);
        IF(${o}.modification == "new"){
          new = 1;
          load = 1;
        }ELSE IF(${o}.modification != "notModified"){
          IF(notModifiedHere){  // modified externally
            load = 1;  // load external modifications without asking
          }ELSE IF(CONFIRM("You want to load the ${label}\n"
                           + ${o}.name
                           + ", Rev. " + ${o}.rev
                           + ".\n"
                           + "It is loaded already but has modifications.\n\n"
                           + "Do you want to reload it (undo modifications)?"
                          )
                  ){
            load = 1;
          }
        }ELSE{
          load = 2;  // reload children
        }
      }
      i = size;  // break
    }
    i++;
  }

  // load component, if needed
  LOG(DEBUG, "g_${id}_load_from_dbcomp_func  load: ", load, ", new: ", new);
  IF(load == 1){
    IF(new == 1){
      % if singleton:
      navIndex_${id} = 0;
      // check if the component is modified -> ask to save
      RUN(g_${id}_get_status_func);
      IF(${o}.modification != "notModified"){
        save = CONFIRM_CANCEL(
          "You want to load the ${label}\n"
          + comp.name
          + ", Rev. " + comp.rev
          + ".\n"
          + "This will replace the modified ${label}\n"
          + ${o}.name
          + ", Rev. " + ${o}.rev
          + ".\n"
          + "Do you want to save the modifications first?"
        );
        IF(!VALID(save)){  // Cancel
          ABORT;
        }ELSE IF(save == 1){
          RUN(g_${id}_save_func);
        }
      }
      % else:
      navIndex_${id} = size;
      % endif
    }
    db_comp.id = comp.id;
    [${o}] = GET(
      PATH=["components/", db_comp.id]
    );
    RUN(${id}_after_load_func);
    LOG(DEBUG, "g_${id}_load_from_dbcomp_func loaded: ${o}, id: ", ${o}.id);
  }
  IF(load > 0){
    RUN(${id}_post_load_func);
    RUN(variant_update_func);
  }
};

FUNC g_${id}_select_list_func {
  IF(REASON_ACTIVATE) {
    ASSIGN_CORR(db_comp, ${id}_select.list[INDEX]);
    RUN(g_${id}_load_from_dbcomp_func);
    CLEAR(loadType);
    UNMAP(g_${id}_select_form);
  }
};

FUNC g_${id}_variant_update_func {
  INTEGER modified, comp_id;
  IF( variant.detached == 1 ){
    RETURN;
  }
  modified = MODIFIED ( variant.${id}[navIndex_${id}] );
  comp_id = variant.${id}[navIndex_${id}].comp_id;
  CLEAR ( variant.${id}[navIndex_${id}] );
  IF ( VALID(${id}[navIndex_${id}].id) ) {
    ASSIGN_CORR( variant.${id}[navIndex_${id}], ${id}[navIndex_${id}] );
    variant.${id}[navIndex_${id}].comp_id = ${id}[navIndex_${id}].id;
    variant.${id}[navIndex_${id}].object = "${id}";
    variant.${id}[navIndex_${id}].index = 0;
  }
  IF ( modified == 0 && comp_id == variant.${id}[navIndex_${id}].comp_id ) {
    SET_DB_TIMESTAMP ( variant.${id}[navIndex_${id}] );
  }
};

FUNC g_${id}_save_func {
  ${id}_library.saveCanceled = 0;
  RUN(${id}_pre_save_func);
  IF(${id}_library.saveCanceled == 1) {
    RETURN;
  }

  ${o}.type = "${type}";
  IF([${o}]
     = PUT(
         PATH="components"
       , DATA=[${o}]
       )
    ) {
    RUN(${id}_post_save_func);
    RUN(g_${id}_variant_update_func);
  } ELSE {
    ${id}_library.saveCanceled = 1;
  }
};

FUNC g_${id}_get_status_func{
  IF(MODIFIED(${o}) && ${o}.detached != 1){
    ${o}.type = "${type}";
    [elementStatus] = PUT(
      PATH="components/check",
      DATA=[${o}]);
    ${o}.id = elementStatus.id;
    IF(elementStatus.status=="notModified"){
      ${o}.version = elementStatus.version;
      SET_DB_TIMESTAMP(${o});
    }
    IF(!VALID(elementStatus.type)){
      elementStatus.type = "${type}";
    }
  }ELSE{
    CLEAR(elementStatus);
    elementStatus.status = "notModified";
    elementStatus.id = ${o}.id;
    elementStatus.name = ${o}.name;
    elementStatus.rev = ${o}.rev;
    elementStatus.type = "${type}";
    elementStatus.text = INVALID;
  }
  ${o}.modification = elementStatus.status;
  ${o}.modification_type = elementStatus.substatus;
  ${o}.modification_approval[0] = elementStatus.approval[0];
  ${o}.modification_approval[1] = elementStatus.approval[1];
};

FUNC g_${id}_saveIfModified_func {
  INTEGER save;

  RUN(g_${id}_get_status_func);

  // check status (modified or new -> confirm to save
  IF(${o}.modification != "notModified") {
    save = CONFIRM_CANCEL("Modified ${label} data exist.\n\nDo you want to save the ${label}?");
    IF(!VALID(save)) { // Cancel
      ABORT;
    } ELSE IF(save == 1) { // Yes
      RUN(g_${id}_save_func);
    }
  }
};

FUNC g_${id}_delete_func {
  INTEGER index, size, i, found;
  GET_SELECTION(g_${id}_select_list, index);
  IF(!VALID(index)) {
    RETURN;
  }
  ASSIGN_CORR(db_comp, ${id}_select.list[index]);
  DELETE(PATH=["components/", db_comp.id]);
  RUN(g_${id}_select_refresh_func);

  SIZE(${id}[*], size);
  i=0;
  found = 0;
  WHILE(i<size && found == 0) {
    IF(${id}[i].id == db_comp.id) {
      IF(CONFIRM("${id} " + ${id}[i].name + "/" + ${id}[i].rev
                 + " removed.\n OK to remove it in current user session also?")) {
        CLEAR(${id}[i]);
        found = 1;
      }
    }
    i++;
  }

  RUN(${id}_post_delete_func);
};
