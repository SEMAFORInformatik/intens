// **********************************************************************
// Project
// **********************************************************************
SET projectStatus_set (
  _("Local") = "local"
, _("Division") = "division"
, _("Global") = "global"
, _("Delivered") = "delivered"
, _("Obsolete") = "obsolete"
);

STRING project_component_classname;
STRING project_component_varname;
STRING project_component_library_varname;

STRUCT Project {
  INTEGER {SCALAR}
    id, version
  ;
  STRING {SCALAR}
    created {
      LABEL="Creation Date",
      STRING_DATETIME,
      HIDDEN
    },
    changed {
      LABEL="Last Modification",
      STRING_DATETIME,
      HIDDEN
    },
    owner {
      LABEL="OwnerId",
      HIDDEN
    },
    ownername {
      LABEL="Owner",
      HIDDEN, TRANSIENT
    },
    group {
      LABEL="Group",
      HIDDEN
    },
    type,  // always "Project", used for 'New Project ... dialog'
    reason
  ;
  STRING {EDITABLE, SCALAR}
    name {
      LABEL="Name", TAG=(project_name_tag),
      HELPTEXT="Definition Name",
      PATTERN=".{0,255}",
      FUNC=project_name_func
    },
    desc {
      LABEL="Description", TAG=(desc_tag),
      HELPTEXT="Definition Description",
      PATTERN=".{0,255}"
    },
    status {
      LABEL="Status", TAG=(status_tag),
      HELPTEXT="Workbook status",
      SET=projectStatus_set
    }
  ;

  // list of variants in project
  // used when loading a project / variant
  //      in project overview
  Variant
    variant {
      LABEL=LABEL(variant),
      TRANSIENT, HIDDEN,
      CLASSNAME="variant",
      FUNC=variant_project_variant_nav_func
    }
  ;
};

Project
  project {
    SCALAR, LABEL=_("Workbook")
  },
  import_project {
    SCALAR, LABEL=_("Workbook")
  }
;

STRUCT ProjectLibrary {
  STRING {EDITABLE, SCALAR}
    open {
      BUTTON,
      HELPTEXT="Open a workbook.",
      FUNC=project_open_pressed_func
    },
    new {
      BUTTON,
      HELPTEXT="Create a new workbook.",
      FUNC=project_new_pressed_func
    },
    store {
      BUTTON,
      HELPTEXT="Store the workbook.",
      FUNC=project_store_pressed_func
    },
    import {
      BUTTON,
      HELPTEXT=_("Import Variant from workbook."),
      FUNC=project_import_open_pressed_func
    },
    new_variant {
      BUTTON,
      HELPTEXT=_("Create a new variant."),
      FUNC=variant_new_pressed_func
    },

    overview {
      LABEL=_("Workbook"),BUTTON,
      HELPTEXT=_("Open window for managing workbook overview and entries."),
      FUNC=project_overview_map_func
    },
    // This is used when open project button is pressed
    // open project runs store project, this checks if project name and
    // project variant is set. But when we want to open a project we don't want
    // this check
    whichButtonPressed
  ;
  INTEGER
    dont_save
  ;
};

ProjectLibrary {SCALAR}
  project_library
;

STRUCT ProjectSelectFilter {
  INTEGER {SCALAR}
    id
  ;
  STRING
    name {SCALAR}, desc {SCALAR}, status, owner {SCALAR}
  ;
  INTEGER
    maxAge {SCALAR}
  ;
};

STRUCT ProjectSelect {
  INTEGER {EDITABLE, SCALAR}
    id {
      LABEL=_("Id"),
      FUNC=project_select_refresh_func
    }
  ;
  STRING {EDITABLE, SCALAR}
    name {
      LABEL=_("Name"),
      FUNC=project_select_refresh_func
    },
    desc {
      LABEL=_("Description"),
      FUNC=project_select_refresh_func
    }
  ;
  INTEGER {EDITABLE, SCALAR}
    local {
      LABEL=_("Local"), TOGGLE,
      FUNC=project_select_refresh_func
    },
    division {
      LABEL=_("Division"), TOGGLE,
      FUNC=project_select_refresh_func
    },
    global {
      LABEL=_("Global"), TOGGLE,
      FUNC=project_select_refresh_func
    },
    delivered {
      LABEL=_("Delivered"), TOGGLE,
      FUNC=project_select_refresh_func
    },
    obsolete {
      LABEL=_("Obsolete"), TOGGLE,
      FUNC=project_select_refresh_func
    },
    myProjects {
      LABEL=_("My workbooks"), TOGGLE,
      HELPTEXT=_("Only my workbooks"),
      FUNC=project_select_refresh_func
    },
    excludeOld {
      LABEL="< 3 years", TOGGLE,
      HELPTEXT="Exclude projects older than 3 years",
      FUNC=project_select_refresh_func
    }
  ;

  ProjectSelectFilter {SCALAR}
    filter
  ;

  Project
    list
  ;
};
ProjectSelect {SCALAR}
  project_select
;


// **********************************************************************
// Project Save Dialog
// **********************************************************************
STRUCT
  ProjectSaveDialogProjectList {
    STRING { SCALAR }
      name   { LABEL=_("Name") }
    , created { LABEL=_("Created"), STRING_DATE }
    , status { LABEL=_("Status") }
    , approval { LABEL=_("Approval") }
    , ownername { LABEL=_("Owner") }
    , group  { LABEL=_("Group") }
    , type  { LABEL=_("FeType") }
    , resultDesc  { LABEL=_("Result") }
    , folder  { LABEL=" ", LABEL }
    ;
    INTEGER { SCALAR }
      id { LABEL=_("Id") }
    , rev { LABEL=_("Rev") }
    ;
  }
;

STRUCT
  ProjectSaveData {
    INTEGER { SCALAR }
      id   { LABEL=_("Id") }
    ;
    INTEGER { SCALAR, EDITABLE }
      save {
        LABEL=_("Save")
      , TOGGLE
      }
    , create_new_machine
    ;

    STRING { SCALAR, EDITABLE }
      save_btn {
        BUTTON
      , LABEL=_("Yes")
      , HELPTEXT="Save workbook."
      , FUNC=project_save_data_dialog_save_func
      }
    , no_save_btn {
        BUTTON
      , HELPTEXT="Do not save workbook."
      , FUNC=project_save_data_dialog_nosave_func
      }
    , cancel_btn {
        BUTTON
      , HELPTEXT="Cancel"
      , FUNC=project_save_data_dialog_cancel_func
      }
    ;
    STRING   { SCALAR, EDITABLE }
      reason { LABEL=_("Modify comment:") }
    ;
    STRING { SCALAR }
      title {LABEL}
    , footer {LABEL}
    , name   { LABEL=_("Name") }
    , status { LABEL=_("State") }
    , substatus { LABEL=_("State") }
    , type   { LABEL=_("Type"), SET=type_set }
    , desc   { LABEL=_("Description") }
    , owner  { LABEL=_("Owner") }
    , username { LABEL=_("Username") }
    , group  { LABEL=_("Group") }
    , resultDesc  { LABEL=_("Result") }
    ;

    ProjectSaveDialogProjectList project;
  }
;

ProjectSaveData project_save_data;

STRUCT
  ProjectSaveDataLibrary {
    STRING { SCALAR } save_mode_machine;
    INTEGER { SCALAR } deleted_result_comps;
  }
;
ProjectSaveDataLibrary project_save_data_library;
