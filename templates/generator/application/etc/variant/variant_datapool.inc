// **********************************************************************
// Variant
// **********************************************************************
STRUCT VariantComponent {
  INTEGER {SCALAR}
    comp_id
  ;
  STRING {SCALAR}
    name {
      LABEL="Name", TAG=(variant_name_tag),
      PATTERN=".{0,255}",
      HELPTEXT="Component name",
      HIDDEN, TRANSIENT},
    desc {
      LABEL="Description", TAG=(desc_tag),
      HELPTEXT="Component description",
      PATTERN=".{0,255}",
      HIDDEN, TRANSIENT},
    approval {
      LABEL="Approval State", TAG=(approval_tag),
      SET=approval_set, COLOR=approval_color,
      HELPTEXT="Approval state",
      HIDDEN, TRANSIENT},
    object {HIDDEN}  // for loading modified component
  ;
  INTEGER {SCALAR}
    rev {
      LABEL="Rev.", TAG=(rev_tag),
      HELPTEXT="Component revision",
      HIDDEN, TRANSIENT},
    index {HIDDEN}  // for loading modified component
  ;
  STRING {SCALAR}
    created {
      LABEL="Creation Date",
      STRING_DATETIME,
      HIDDEN, TRANSIENT}
  ;
};

// BEGIN BLOCK COMPONENT
//
//STRUCT Variant<componentType> <baseType> {
//  // add variant attributes
//};
// END BLOCK COMPONENT

STRUCT Variant {
  INTEGER {SCALAR}
    id
  , projectId
  ;
  STRING {EDITABLE, SCALAR}
    name {
      LABEL="Name", TAG=(variant_name_tag, project_name_tag),
      PATTERN=".{0,255}",
      HELPTEXT="Variant Name",
      FUNC=variant_name_func
    },
    desc {
      LABEL="Description", TAG=(desc_tag),
      HELPTEXT="Variant description",
      PATTERN=".{0,255}"
    }
  ;
  INTEGER {SCALAR}
    version,
    current_version {TRANSIENT}
  , detached {TRANSIENT}
  ;
  INTEGER {SCALAR}
    rev {
      LABEL="Rev.", TAG=(rev_tag),
      HELPTEXT="Variant revision",
      SCALAR
    }
  ;

  STRING {EDITABLE, SCALAR}
    approval {
      LABEL="Status", TAG=(variant_approval_tag),
      SET=approval_set, COLOR=approval_color,
      HELPTEXT="Definition Status"
    }
  ;
  STRING {SCALAR}
    created {
      LABEL="Creation Date", TAG=(variant_date_tag),
      STRING_DATETIME, HIDDEN
    },
    changed {
      LABEL="Last Modification",
      STRING_DATETIME,
      HIDDEN,
      TRANSIENT
    },
    owner {LABEL="OwnerId", HIDDEN},
    ownername {LABEL="Owner", HIDDEN},
    group {LABEL="Group", HIDDEN},
    type,  // always "Variant", used for 'New Variant ... dialog'
    tooltip {TAG=(tooltip_tag), TRANSIENT, HIDDEN},
    reason
  ;

// BEGIN BLOCK COMPONENT
//
//  Variant<componentType>
//    <component> {
//      FUNC=<component>_variant_func,
//      CLASSNAME="<componentType>",
//      SCALAR
//    }
//  ;
// END BLOCK COMPONENT
};

Variant {SCALAR}
  variant {
    SCALAR,
    LABEL=_("Variant"),
    PERSISTENT
  },
  activeVariant {
    LABEL=_("Variant"),
    SCALAR,
    CLASSNAME="variant",
    FUNC=variant_nav_func
  },
  project_variant {SCALAR}
;

STRUCT VariantSelectFilter {
  INTEGER
    projectId {SCALAR},
    id {SCALAR}
  ;

  STRING
    name {SCALAR}, approval, owner {SCALAR}, desc {SCALAR}
  ;
  INTEGER
    maxAge {SCALAR}
  ;
};

STRUCT VariantSelect {
  INTEGER {SCALAR} projectId;

  INTEGER {EDITABLE, SCALAR}
    id {
      LABEL=_("Id"),
      FUNC=variant_select_refresh_func
    }
  ;
  STRING {EDITABLE, SCALAR}
    name {
      LABEL=_("Name"),
      FUNC=variant_select_refresh_func
    },
    desc {
      LABEL=_("Description"),
      FUNC=variant_select_refresh_func
    }
  ;
  INTEGER {EDITABLE, SCALAR}
    experimental {
      LABEL=_("Experimental"), TOGGLE,
      FUNC=variant_select_refresh_func
    },
    inPreparation {
      LABEL=_("In Preparation"), TOGGLE,
      FUNC=variant_select_refresh_func
    },
    shared {
      LABEL=_("Shared"), TOGGLE,
      FUNC=variant_select_refresh_func
    },
    approved {
      LABEL=_("Approved"), TOGGLE,
      FUNC=variant_select_refresh_func
    },
    tested {
      LABEL=_("Tested"), TOGGLE,
      FUNC=variant_select_refresh_func
    },
    obsolete {
      LABEL=_("Obsolete"), TOGGLE,
      FUNC=variant_select_refresh_func
    },
    myVariants {
      LABEL=_("My variants"), TOGGLE,
      HELPTEXT=_("Only my variants"),
      FUNC=variant_select_refresh_func
    },
    excludeOld {
      LABEL="< 3 years", TOGGLE,
      HELPTEXT="Exclude variants older than 3 years",
      FUNC=variant_select_refresh_func
    }
  ;

  VariantSelectFilter {SCALAR}
    filter
  ;
};

VariantSelect {SCALAR}
  variant_select
;
