FUNC g_project_get_status_allcomp_func {
  INTEGER size;

% for a in [p['object'] for p in projComps]:

  // ${a}
  SIZE(${a}[*],size);
  navIndex_${a} = 0;
  WHILE(navIndex_${a}<size) {
    RUN(g_${a}_get_status_func);
    allStatus[allStatusIndex] = elementStatus;
    allStatusIndex = allStatusIndex + 1;
    navIndex_${a} = navIndex_${a} + 1;
  }
  navIndex_${a} = 0;
% endfor
  RETURN;
};

FUNC g_project_check_modified_anycomp_func {
  INTEGER size;

% for a in [p['object'] for p in projComps]:

  // ${a}
  SIZE(${a}, size);
  navIndex_${a}=0;
  WHILE(navIndex_${a} < size) {
    RUN(g_${a}_get_status_func);
    IF(allStatus[allStatusIndex - 1].status != "notModified") {
      RETURN;
    }
    navIndex_${a} = navIndex_${a} + 1;
  }
  navIndex_${a} = 0;
% endfor
  RETURN;
};

FUNC g_project_clear_allcomp_func {
  RUN( variant_after_load_func );
% for a in [p['object'] for p in projComps]:
  CLEAR(${a});
  RUN( ${a}_after_load_func );
%endfor
  RETURN;
};

FUNC g_project_init_allcomp_func {
% for a in [p['object'] for p in projComps]:
  RUN(${a}_init_func);
%endfor
  RUN(g_project_type_set_fill_func);
};

FUNC g_project_type_set_fill_func{
  INTEGER i;

  CLEAR(type_set.Input, type_set.Output);
  i = 0;

  type_set.Output[i] = "Project";
  type_set.Input[i] = LABEL(project);
  i++;
  type_set.Output[i] = "Variant";
  type_set.Input[i] = LABEL(variant);
  i++;
% for p in projComps:
  type_set.Output[i] = "${p['type']}";
  type_set.Input[i] = LABEL(${p['object']});
  i++;
% endfor
};

FUNC g_project_save_allcomp_func {
  INTEGER size;

% for a in [p['object'] for p in projComps]:
  // ${a}
  SIZE(${a}[*],size);
  navIndex_${a} = 0;
  WHILE(navIndex_${a}<size) {
    IF(${a}[navIndex_${a}].modification == "modified" ||
       ${a}[navIndex_${a}].modification == "upToDateConflict" ||
      ${a}[navIndex_${a}].modification == "new"
      ) {
      RUN(g_${a}_save_func);
    }
    navIndex_${a} = navIndex_${a} + 1;
  }
  navIndex_${a} = 0;
% endfor
  RETURN;
};

FUNC g_project_get_status_func {
  IF(MODIFIED(project)) {
    project.type = "Project";
    [elementStatus] = PUT(
      PATH="projects/check"
    , DATA=[project]
    );
    project.id = elementStatus.id;
    IF(elementStatus.status == "notModified") {
      SET_DB_TIMESTAMP(project);
    }
  } ELSE {
    elementStatus.status = "notModified";
    elementStatus.id = project.id;
    elementStatus.name = project.name;
    elementStatus.rev = INVALID;
    elementStatus.type = "Project";
    elementStatus.text = INVALID;
  }
  allStatus[allStatusIndex] = elementStatus;
  allStatusIndex = allStatusIndex + 1;
};

FUNC g_variant_update_func {
  INTEGER size, index;
  IF( variant.detached == 1 ){
    RETURN;
  }
% for a in [p['object'] for p in projComps]:
  //
  // ${a}
  SIZE(${a}[*], size);
  index = navIndex_${a};
  navIndex_${a} = 0;
  WHILE( navIndex_${a} < size) {
    RUN(g_${a}_variant_update_func);
    navIndex_${a} = navIndex_${a} + 1;
  }
  navIndex_${a} = index;
% endfor
  RETURN;
};

FUNC g_variant_load_components_func {
  INTEGER size;
% for a in [p['object'] for p in projComps]:

  // ${a}
  IF(${a}_library.saveCanceled != 1) {
    CLEAR(${a});
    SIZE(variant.${a}[*], size);
    navIndex_${a} = 0;
    WHILE(navIndex_${a} < size) {
      [${a}[navIndex_${a}]] = GET(
        PATH=[ "components/", variant.${a}[navIndex_${a}].comp_id ]
      );
      RUN( ${a}_after_load_func );
      navIndex_${a} = navIndex_${a} + 1;
    }
    navIndex_${a} = 0;
    RUN(${a}_post_load_func);
  }
% endfor
  RETURN;
};

FUNC g_variant_check_different_components_func {
  RETURN;
};

FUNC g_variant_load_nav_components_func {
  INTEGER i, size;
% for p in projComps:
<% a = p['object'] %><% t = p['type'] %>
  // ${a}
  SIZE(project_variant.${a}[*],size);
  i = 0;
  WHILE(i < size) {
    dbId = project_variant.${a}[i].comp_id;
    [db_comp[#]] = GET(
      PATH=[ "components/type/${t}?id=", dbId ]
    );
    project_variant.${a}[i].name = db_comp[0].name;
    project_variant.${a}[i].rev = db_comp[0].rev;
    project_variant.${a}[i].approval = db_comp[0].approval;
    project_variant.${a}[i].created = db_comp[0].created;
    i++;
  }
% endfor
  RETURN;
};

FUNC g_variant_get_status_func {
  IF(MODIFIED(variant) && variant.detached != 1) {
    variant.type = "Variant";
    [elementStatus] = PUT(
      PATH="variants/check"
    , DATA=[variant]
    );
    IF(!VALID(elementStatus.substatus) && elementStatus.status == "modified") {
      elementStatus.substatus = "nontrivial";
    }
    variant.id = elementStatus.id;
    IF(elementStatus.status == "notModified") {
      SET_DB_TIMESTAMP(variant);
    }
  } ELSE {
    elementStatus.status = "notModified";
    elementStatus.id = variant.id;
    elementStatus.name = variant.name;
    elementStatus.rev = variant.rev;
    elementStatus.type = "Variant";
    elementStatus.text = INVALID;
  }
  allStatus[allStatusIndex] = elementStatus;
  allStatusIndex = allStatusIndex + 1;
};

FUNC g_load_modified_component_func {
  INTEGER load;
  IF (modifications.type == "project") {
    RETURN;
  }
  IF( modifications.type == "variant") {
    allStatusIndex = 0;
    RUN(g_variant_get_status_func);
    IF(allStatus[allStatusIndex - 1].status != "notModified") {
      load = CONFIRM(
        "Variant has modifications!\n" +
        "Do you want to load data and undo modifications?"
      , BUTTON_YES=_("Load data")
      , BUTTON_NO=_("Cancel")
      );
      IF( !load ){
        RETURN;
      }
    }
    [variant[0]] = GET(
      PATH=["components/", variant.id, "/modifications/", modifications.mod_id]
    );
    variant[0].current_version = modifications.mod_id;
    RUN( variant_after_load_func );
  }

  IF(!VALID(modifications.element.comp_id) ||
     !VALID(modifications.element.object) ||
     !VALID(modifications.element.index) ||
     !VALID(modifications.mod_id)
    ) {
    RETURN;
  }

<% cond='IF' %>
% for a in [p['object'] for p in projComps]:
  ${cond}(modifications.element.object == "${a}") { <% cond = 'ELSE IF' %>
    navIndex_${a} = modifications.element.index;
    RUN(g_${a}_saveIfModified_func);
    [${a}[navIndex_${a}]] = GET(
      PATH=["components/", modifications.element.comp_id, "/modifications/", modifications.mod_id]
    );
    ${a}[navIndex_${a}].current_version = modifications.mod_id;
    RUN(${a}_after_load_func);
    RUN(${a}_post_load_func);
  }
% endfor
  RETURN;
};
