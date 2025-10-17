// **********************************************************************
// Component
// **********************************************************************
FUNC component_refresh_func {
  // Handle rename of components
  // see: element_rename
  RETURN;
};

FUNC component_show_modifications_func {
  CLEAR(modifications);
  ASSIGN_CORR(modifications.element, THIS);
  modifications.type = "component";
  RUN(modifications_show_func);
};

FUNC component_show_used_in_projects_func {
  CLEAR(used_in_projects.element);
  ASSIGN_CORR(used_in_projects.element, THIS);
  used_in_projects.element.object = NODE(THIS);
  RUN(used_in_projects_show_func);
};

