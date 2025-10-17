// ***************************
// ${id} select
// ***************************
LIST g_${id}_select_list {
  TABLESIZE=10
, FUNC=g_${id}_select_list_func
} (
  LABEL(${id}_select.list.id)          ${id}_select.list[*].id {OPTIONAL}
, LABEL(${id}_select.list.name)        ${id}_select.list[*].name:30
, LABEL(${id}_select.list.rev)         ${id}_select.list[*].rev:3
, LABEL(${id}_select.list.approval)    ${id}_select.list[*].approval:14
, LABEL(${id}_select.list.desc)        ${id}_select.list[*].desc:30 {OPTIONAL}
, LABEL(${id}_select.list.ownername)   ${id}_select.list[*].ownername:22
, LABEL(${id}_select.list.group)       ${id}_select.list[*].group:12 {OPTIONAL}
, LABEL(${id}_select.list.created)     ${id}_select.list[*].created:17
, LABEL(${id}_select.list.changed)     ${id}_select.list[*].changed:17
, LABEL(${id}_select.list.changername) ${id}_select.list[*].changername:22 {OPTIONAL}
, LABEL(${id}_select.list.modcomment)  ${id}_select.list[*].modcomment:30 {OPTIONAL}
);

MENU g_${id}_select_list (
  FUNC g_${id}_delete_func = _("Delete...")
);

FIELDGROUP g_${id}_select_show_status_fg {
  _("Show status")
} (
  VOID(0, 10)
  ${id}_select.experimental  LABEL(${id}_select.experimental)
  VOID(5, 0)
  ${id}_select.inPreparation LABEL(${id}_select.inPreparation)
  VOID(5, 0)
  ${id}_select.shared        LABEL(${id}_select.shared)
  VOID(5, 0)
  ${id}_select.approved      LABEL(${id}_select.approved)
  VOID(5, 0)
  ${id}_select.tested        LABEL(${id}_select.tested)
  VOID(5, 0)
  ${id}_select.obsolete      LABEL(${id}_select.obsolete)
, VOID(0, 10)
);

FIELDGROUP g_${id}_select_search_flags_fg {
  _("Search flags")
} (
  VOID(0, 10)
  LABEL(${id}_select.name) ${id}_select.name:30
  VOID(5, 0)
  ${id}_select.my${type}s  LABEL(${id}_select.my${type}s)
  VOID(5, 0)
  ${id}_select.excludeOld  LABEL(${id}_select.excludeOld)
  VOID(5, 0)
);

FORM g_${id}_select_form {
  _("${type}s")
, HIDE_CYCLE
, HIDDEN
, APP_MODAL
} (
  (
    g_${id}_select_show_status_fg
  , VOID(80)
  , g_${id}_select_search_flags_fg
  )
, g_${id}_select_list
);
