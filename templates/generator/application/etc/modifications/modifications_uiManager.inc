// **********************************************************************
// Modifications
// **********************************************************************
INDEX modifications_page_index {
  RANGE(1)
, FUNC=modifications_show_func
};
INDEX modifications_pageSize_index {
  FUNC=modifications_show_func
};

FIELDGROUP modifications_paging_fg {FRAME, "Paging"} (
  _("Page") modifications_page_index
  _("Size") modifications_pageSize_index
);

FIELDGROUP modifications_project_fg {FRAME, "Workbook"} (
  modifications.project.name:25
  modifications.project.desc:40
  modifications.project.version:4
  modifications.project.status:12
);

FIELDGROUP modifications_variant_fg {FRAME, "Variant"} (
  modifications.variant.name:25
  modifications.variant.desc:40
  modifications.variant.rev:4
  modifications.variant.approval:12
);

FIELDGROUP modifications_component_fg {FRAME, "Component"} (
  modifications.element.name:25
  modifications.element.desc:40
  modifications.element.rev:4
  modifications.element.approval:12
);

LIST modifications_list {
  TABLESIZE=15
, FUNC=modifications_list_func
} (
  LABEL(modifications.list.changed) modifications.list[*].changed:16
, LABEL(modifications.list.user)    modifications.list[*].user:25
, LABEL(modifications.list.reason)  modifications.list[*].reason:70
);

FORM modifications_form {
  "Modifications"
, HIDE_CYCLE
, HIDDEN
} (
  ( modifications_project_fg
  , modifications_variant_fg
  , modifications_component_fg
  , VOID(20)
  , modifications_paging_fg
  )
, modifications_list
);
