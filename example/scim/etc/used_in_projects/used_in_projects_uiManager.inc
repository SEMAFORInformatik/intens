// **********************************************************************
// Used in Projects
// **********************************************************************
FIELDGROUP used_in_projects_element_fg (
  _("Component:")
, used_in_projects.element.name:25
  used_in_projects.element.desc:40
  used_in_projects.element.rev:4
  used_in_projects.element.approval:12
, VOID
, _("Workbooks:")
);

LIST used_in_projects_list {
  TABLESIZE=20
} (
  LABEL(used_in_projects.list.name)    used_in_projects.list[*].name:25
, LABEL(used_in_projects.list.created) used_in_projects.list[*].created:16
, LABEL(used_in_projects.list.status)  used_in_projects.list[*].status:15
, LABEL(used_in_projects.list.ownername)   used_in_projects.list[*].ownername:22
, LABEL(used_in_projects.list.group)   used_in_projects.list[*].group:20
);

FORM used_in_projects_form {
  _("Used In Workbooks")
, HIDE_CYCLE
, HIDDEN
} (
  used_in_projects_element_fg
, used_in_projects_list
);
