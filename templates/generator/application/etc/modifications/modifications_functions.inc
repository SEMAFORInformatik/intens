// **********************************************************************
// Modifications
// **********************************************************************
FUNC modifications_show_func {
  IF(modifications.type == "project") {
    IF( !VALID(modifications.project.id) ) {
      RETURN;
    }
    modifications.id = modifications.project.id;
    MAP(modifications_project_fg);
    UNMAP(modifications_variant_fg);
    UNMAP(modifications_component_fg);
  }
  ELSE IF (modifications.type == "variant") {
    IF( !VALID(modifications.variant.id) ) {
      RETURN;
    }
    modifications.id = modifications.variant.id;
    UNMAP(modifications_project_fg);
    MAP(modifications_variant_fg);
    UNMAP(modifications_component_fg);

  }
  ELSE IF (modifications.type == "component") {
    IF( !VALID(modifications.element.comp_id) ) {
      RETURN;
    }
    modifications.id = modifications.element.comp_id;
    UNMAP(modifications_project_fg);
    UNMAP(modifications_variant_fg);
    MAP(modifications_component_fg);

  }
  ELSE{
    RETURN;
  }

  IF(modifications_pageSize_index < 1) {
    modifications_page_index = 0;
    modifications_pageSize_index = 50;
  }

  [modifications.list[#]] = GET(
    PATH=[
    "components/", modifications.id, "/modifications"
  , "?_page=", modifications_page_index
  , "&_pageSize=", modifications_pageSize_index]
  );

  MAP(modifications_form);
};

FUNC modifications_list_func {
  IF(REASON_ACTIVATE) {
    modifications.mod_id = modifications.list[INDEX].mod_id;
    RUN(g_load_modified_component_func);
  }
};
