// **********************************************************************
// Component
// **********************************************************************
STRUCT Component {
  INTEGER {SCALAR}
    id
  ;
  STRING {EDITABLE, SCALAR, NO_DEPENDENCIES}
    name {
      LABEL=_("Name"),
      PATTERN=".{0,255}",
      HELPTEXT="Component name"}
  , desc {
      LABEL=_("Description"), TAG=(desc_tag),
      PATTERN=".{0,255}",
      HELPTEXT="Component description"}
  ;
  INTEGER {SCALAR}
    version {LABEL=_("Ver."), HELPTEXT="Version"},
    current_version {TRANSIENT},
    detached {TRANSIENT},
    rev {LABEL=_("Rev."), TAG=(rev_tag),
      HELPTEXT="Component revision"}
  ;

  STRING {EDITABLE, SCALAR, NO_DEPENDENCIES}
    applicab {
      LABEL=_("Applicab State"), TAG=(applicab_tag),
      SET=applicab_set, HIDDEN },
    approval {
      LABEL=_("Approval State"), TAG=(approval_tag),
      SET=approval_set, COLOR=approval_color,
      HELPTEXT=_("Approval state") }
  ;

  STRING {SCALAR}
    created  {LABEL=_("Created"), STRING_DATETIME, TAG=created_tag},
    owner {LABEL=_("OwnerId")},
    ownername {LABEL=_("Owner"), TRANSIENT},
    group {LABEL=_("Group")},
    type  {LABEL=_("Comp Type")},
    changed  {LABEL=_("Last Changed"), TRANSIENT, STRING_DATETIME},
    modcomment  {LABEL=_("Modification Comment"), TRANSIENT, HIDDEN},
    changer {LABEL=_("ChangedId"),TRANSIENT, HIDDEN},
    changername {LABEL=_("Changed by"),TRANSIENT},
    tooltip {TAG=(tooltip_tag), TRANSIENT, HIDDEN},
    approverTooltip {TRANSIENT, HIDDEN},
    reason
  ;

  STRING {SCALAR}
    modification {TRANSIENT, HIDDEN},
    modification_type {TRANSIENT, HIDDEN},  // trivial, nontrivial
    modification_approval {TRANSIENT, HIDDEN}  // new approval state
  ;
};

Component
  db_comp {SCALAR},
  db_comp2 {SCALAR},
  compList
;
