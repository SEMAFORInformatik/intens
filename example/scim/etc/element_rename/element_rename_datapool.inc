// **********************************************************************
// Element Rename
// **********************************************************************
STRUCT ElementRename {
  INTEGER {SCALAR}
    id
  ;
  INTEGER {SCALAR}
    version
  ;
  STRING {SCALAR}
    oldName {
      LABEL=_("Old name")
    }
  , name
  , type
  , pathType
  , title {LABEL}
  ;
  STRING {SCALAR, EDITABLE}
    newName {
      LABEL=_("New name")
    }
  ;
};
ElementRename {GLOBAL, SCALAR}
  element_rename, renamedElement
;
