// **********************************************************************
// ${componentTypeName}
// **********************************************************************
STRUCT ${componentType} : Component {

  // Define variables for ${componentTypeName} here

};

${componentType}
  ${component} {
    LABEL=_("${componentTypeName}"),
    PERSISTENT
  }
;

STRUCT ${componentType}Library : ComponentLibrary {
  STRING {EDITABLE}
    open {
      BUTTON,
      HELPTEXT=_("Open a ${componentTypeName}."),
      FUNC=${component}_open_func
    },
    new {
      BUTTON,
      HELPTEXT=_("Create a new ${componentTypeName}."),
      FUNC=${component}_new_func
    },
    store {
      BUTTON,
      HELPTEXT=_("Store the ${componentTypeName}."),
      FUNC=${component}_store_func
    }
  ;

};

${componentType}Library {SCALAR}
  ${component}_library
;
