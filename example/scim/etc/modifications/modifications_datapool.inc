// **********************************************************************
// Modifications
// **********************************************************************
STRUCT ModificationsList {
  STRING
    changed {
      LABEL=_("Date"), STRING_DATETIME
    },
    user {
      LABEL=_("User")
    },
    reason {
      LABEL=_("Reason")
    },
    type {
      LABEL
    }
  ;
  INTEGER
    mod_id {
      LABEL=_("Mod.Id")
    }
  ;
};

STRUCT ModProject {
  STRING {SCALAR}
    name {
      LABEL="Name"
    },
    desc {
      LABEL="Description"
    },
    status {
      LABEL="Status", SET=projectStatus_set
    }
  ;
  INTEGER {SCALAR}
    version {
      LABEL="Version"
    }
  , id
  ;
};

STRUCT ModVariant {
  STRING {SCALAR}
    name {
      LABEL="Name"
    },
    desc {
      LABEL="Description"
    },
    approval {
      LABEL="Approval State",
      SET=approval_set, COLOR=approval_color
    }
  ;
  INTEGER {SCALAR}
    rev {
      LABEL="Rev."
    },
    id
  ;
};

STRUCT ModComponent {
  STRING {SCALAR}
    name {
      LABEL="Name"
    },
    desc {
      LABEL="Description"
    },
    approval {
      LABEL="Approval State",
      SET=approval_set, COLOR=approval_color
    },
    object
  ;
  INTEGER {SCALAR}
    rev {
      LABEL="Rev."
    },
    comp_id,
    index
  ;
};

STRUCT Modifications {
  ModComponent
    element
  ;
  ModProject
    project
  ;
  ModVariant
    variant
  ;
  ModificationsList
    list
  ;
  INTEGER
    mod_id
  , id
  ;
  STRING
    type
  ;
};

Modifications
  modifications
;
