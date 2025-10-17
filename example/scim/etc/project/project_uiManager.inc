// **********************************************************************
// Project
// **********************************************************************
LIST project_select_list {
  TABLESIZE=10, FUNC=project_select_list_func} (
  LABEL(project_select.list.id)         project_select.list[*].id:7 {OPTIONAL},
  LABEL(project_select.list.name)       project_select.list[*].name:25,
  LABEL(project_select.list.desc)       project_select.list[*].desc:45,
  LABEL(project_select.list.ownername)  project_select.list[*].ownername:22,
  LABEL(project_select.list.created)    project_select.list[*].created:16,
  LABEL(project_select.list.changed)    project_select.list[*].changed:16,
  LABEL(project_select.list.status)     project_select.list[*].status:10
);

LIST project_select_import_list {
  TABLESIZE=10, FUNC=project_select_import_list_func} (
  LABEL(project_select.list.id)         project_select.list[*].id:7 {OPTIONAL},
  LABEL(project_select.list.name)       project_select.list[*].name:25,
  LABEL(project_select.list.desc)       project_select.list[*].desc:40,
  LABEL(project_select.list.ownername)  project_select.list[*].ownername:22,
  LABEL(project_select.list.created)    project_select.list[*].created:16,
  LABEL(project_select.list.changed)    project_select.list[*].changed:16,
  LABEL(project_select.list.status)     project_select.list[*].status:10
);

FIELDGROUP project_select_show_status_fg {
  _("Show status"), FRAME} (
  VOID(0, 10)
  project_select.local      LABEL(project_select.local)
  VOID(5, 0)
  project_select.division   LABEL(project_select.division)
  VOID(5, 0)
  project_select.global     LABEL(project_select.global)
  VOID(5, 0)
  project_select.delivered  LABEL(project_select.delivered)
  VOID(5, 0)
  project_select.obsolete   LABEL(project_select.obsolete),
  VOID
);

FIELDGROUP project_select_search_flags_fg {
  _("Search flags"), FRAME} (
  VOID(0, 10)
  LABEL(project_select.name) project_select.name:20
  VOID(15, 0)
  project_select.myProjects  LABEL(project_select.myProjects)
  VOID(15, 0)
  project_select.excludeOld  LABEL(project_select.excludeOld)
, VOID
);

FORM project_select_form {
  _("Workbook"), HIDE_CYCLE, HIDDEN, APP_MODAL}(
  (
    project_select_show_status_fg,
    VOID(40),
    project_select_search_flags_fg
  )[JUSTIFY],
  project_select_list,
  (
    variant_select_show_status_fg,
    VOID(40),
    variant_select_search_flags_fg
  )[JUSTIFY],
  variant_select_list
);

FORM project_select_import_form {
  _("Import Variant"), HIDE_CYCLE, HIDDEN, APP_MODAL}(
  (
    project_select_show_status_fg,
    VOID(40),
    project_select_search_flags_fg
  )[JUSTIFY],
  project_select_import_list,
  (
    variant_select_show_status_fg,
    VOID(40),
    variant_select_search_flags_fg
  )[JUSTIFY],
  variant_select_import_list
);

// Project Overview
// ---------------------------------------------------

MENU Project( // For Navigator in Overview
  FUNC project_rename_func = "Rename workbook ...",
  FUNC project_show_modifications_func = "Show workbook modifications ..."
);

FIELDGROUP project_overview_top_section_fg(
  VOID(0, 10),
  project.name:20 VOID(5, 0) project.desc:40 STRETCH(1, 0),
  VOID(0, 10),
  SEPARATOR);

NAVIGATOR project_navi{
  COL(
    "Description" project_name_tag:30,
    "Status" variant_approval_tag:2 PIXMAP,
    "Rev" rev_tag:3,
    "Creation date" variant_date_tag:16
  ), SIZE(450, 600), EXPAND, TOOLTIP(tooltip_tag)} (
  project[*] = _("Workbook") {AUTOLEVEL, HIDE_EMPTY_FOLDER}
);

FORM project_overview_form{
  _("Workbook Overview"), HIDE_CYCLE, HIDDEN, FUNC=project_overview_form_func}(
  project_overview_top_section_fg,
  (
    (project_navi) [_("Workbook"), FRAME],
    (variant_navi) ["Variant", FRAME]
  )
);

//
// Project Save Overview
// -----------------------------------------------------
FIELDGROUP
  project_save_data_fg {
  , TABLESIZE=15, RANGE(1, 15), TABLE ORIENTATION=VERTICAL
  , ARROWS=HIDDEN, STRETCH
  } (
    LABEL(project_save_data.id)       project_save_data[*].id:9<,
    LABEL(project_save_data.name)     project_save_data[*].name:40,
    LABEL(project_save_data.status)   project_save_data[*].status:10,
    LABEL(project_save_data.type)     project_save_data[*].type:10,
    LABEL(project_save_data.username) project_save_data[*].username:20,
    LABEL(project_save_data.group)    project_save_data[*].group:15,
    LABEL(project_save_data.resultDesc) project_save_data[*].resultDesc:12,
    LABEL(project_save_data.save)     project_save_data[*].save:5
  )
;

FIELDGROUP
  project_save_data_top_fg (
    project_save_data.title<
  )
;

FIELDGROUP
  project_save_data_modify_comment_fg (
    VOID(1,15)
  , LABEL(project_save_data.reason) project_save_data.reason:115 { EXPAND }
  )
;

FIELDGROUP
  project_save_data_bottom_fg (
    VOID(1,15)
  , project_save_data.footer< { COLSPAN=7 }
  , VOID(1,15)
  , project_save_data.save_btn:30  VOID(10,0)
    project_save_data.no_save_btn:30 STRETCH (1,0)
    project_save_data.cancel_btn:30
  )
;

// FIELDGROUP
//   project_save_data_used_in_projects_fg  {
//     "<b>Machine is used in Workbooks</b>"
//   , TABLESIZE=30
//   , RANGE(1, 1000)
//   , TABLE ORIENTATION=VERTICAL
//   , ARROWS=HIDDEN
//   , STRETCH
//   }(
//     LABEL(project_save_data.project.folder)   project_save_data.project[*].folder:6>
//   , LABEL(project_save_data.project.id)       project_save_data.project[*].id:9<
//   , LABEL(project_save_data.project.name)     project_save_data.project[*].name:40
//   , LABEL(project_save_data.project.rev)       project_save_data.project[*].rev:3<
//   , LABEL(project_save_data.project.type)     project_save_data.project[*].type
//   , LABEL(project_save_data.project.created)  project_save_data.project[*].created:16<
//   , LABEL(project_save_data.project.approval) project_save_data.project[*].approval:14
//   , LABEL(project_save_data.project.ownername) project_save_data.project[*].ownername:22
//   , LABEL(project_save_data.project.group)    project_save_data.project[*].group
//   , LABEL(project_save_data.project.resultDesc) project_save_data.project[*].resultDesc:15
//   )
// ;

FORM
  project_save_data_form{"Workbook save component overview",
    HIDE_CYCLE, CLOSE_BUTTON=NONE, HIDDEN, APP_MODAL, EXPAND}(
    project_save_data_top_fg,
    VOID(15),
    project_save_data_fg,
    // VOID(15),
    // project_save_data_used_in_projects_fg,
    VOID(15),
    project_save_data_modify_comment_fg,
    VOID(15),
    project_save_data_bottom_fg
  )
;
