// **********************************************************************
// Variant
// **********************************************************************
LIST variant_select_list {
  TABLESIZE=10
, FUNC=variant_select_list_func
} (
  LABEL(project.variant.id)         project.variant[*].id:7 {OPTIONAL}
, LABEL(project.variant.name)       project.variant[*].name:25
, LABEL(project.variant.desc)       project.variant[*].desc:45
, LABEL(project.variant.ownername)  project.variant[*].ownername:22
, LABEL(project.variant.created)    project.variant[*].created:16
, LABEL(project.variant.changed)    project.variant[*].changed:16
, LABEL(project.variant.approval)   project.variant[*].approval:10
);

LIST variant_select_import_list {
  TABLESIZE=10
, FUNC=variant_select_import_list_func
} (
  LABEL(project.variant.id)         import_project.variant[*].id:7 {OPTIONAL}
, LABEL(project.variant.name)       import_project.variant[*].name:25
, LABEL(project.variant.desc)       import_project.variant[*].desc:40
, LABEL(project.variant.ownername)  import_project.variant[*].ownername:22
, LABEL(project.variant.created)    import_project.variant[*].created:16
, LABEL(project.variant.changed)    import_project.variant[*].changed:16
, LABEL(project.variant.approval)   import_project.variant[*].approval:14
);

FIELDGROUP variant_select_show_status_fg {
  _("Show status")
, FRAME
} (
  variant_select.experimental  LABEL(variant_select.experimental)
  VOID(5, 0)
  variant_select.inPreparation LABEL(variant_select.inPreparation)
  VOID(5, 0)
  variant_select.shared        LABEL(variant_select.shared)
  VOID(5, 0)
  variant_select.approved      LABEL(variant_select.approved)
  VOID(5, 0)
  variant_select.tested        LABEL(variant_select.tested)
  VOID(5, 0)
  variant_select.obsolete      LABEL(variant_select.obsolete)
, VOID
);

FIELDGROUP variant_select_search_flags_fg {
  _("Search flags"), FRAME} (
  LABEL(variant_select.name)   variant_select.name
  VOID(5, 0)
  variant_select.myVariants    LABEL(variant_select.myVariants)
  VOID(5, 0)
  variant_select.excludeOld    LABEL(variant_select.excludeOld)
, VOID
);

// BEGIN BLOCK COMPONENT
//    MENU Variant<componentType> (
//      FUNC component_show_modifications_func = "Show <component> modifications ..."
//    , FUNC component_show_used_in_projects_func = "Show <component> usage in projects ..."
//    );
// END BLOCK COMPONENT
    MENU VariantShortCircuit (
      FUNC component_show_modifications_func = "Show short_circuit modifications ..."
    , FUNC component_show_used_in_projects_func = "Show short_circuit usage in projects ..."
    );
    MENU VariantMotor (
      FUNC component_show_modifications_func = "Show motor modifications ..."
    , FUNC component_show_used_in_projects_func = "Show motor usage in projects ..."
    );


MENU Variant (
  FUNC variant_rename_func = "Rename variant ..."
, FUNC variant_show_modifications_func = "Show variant modifications ..."
);

NAVIGATOR variant_navi {
  COL(
    "Description" variant_name_tag:30
  , "Status" approval_tag:2 PIXMAP
  , "Rev" rev_tag:3
  )
, SIZE(400, 600)
, EXPAND
, TOOLTIP(tooltip_tag)
} (
  activeVariant[*] = "Variant" {
    AUTOLEVEL
  , HIDE_EMPTY_FOLDER
  }
);
