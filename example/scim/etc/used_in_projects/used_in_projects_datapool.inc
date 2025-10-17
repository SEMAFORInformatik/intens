// **********************************************************************
// Used in Projects
// **********************************************************************
STRUCT UsedInProjects {
  VariantComponent
    element
  ;

  Project
    list
  ;
};

UsedInProjects
  used_in_projects
;

STRUCT CompInProjectsVariants{
  STRING type {LABEL=_("Type")};
  STRING vcomp {LABEL=_("Var Comp.")};
  INTEGER comp_id, rev {LABEL=_("Rev.")};
  STRING name {LABEL=_("Name")};
  STRING approval {LABEL=_("Approval"), SET=approval_set};
  STRING ownername {LABEL=_("Owner")};
  STRING created {LABEL=_("Created"), STRING_DATETIME};
  STRING group {LABEL=_("Group")};

  Project projects;
  Variant variants;
  INTEGER nfound;
  STRING message;
};

CompInProjectsVariants compInProjects;
