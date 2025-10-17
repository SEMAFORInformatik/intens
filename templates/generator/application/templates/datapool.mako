DATAPOOL
// *****************************************************
// ${object}
// *****************************************************
STRUCT ${type}SelectFilter {
  STRING
    name {SCALAR}
  , approval
  , owner {SCALAR}
  ;
  INTEGER
    maxAge {SCALAR}
  ;
};

STRUCT ${type}Select {
  STRING {EDITABLE, SCALAR}
    name {
      LABEL=_("Name")
    , FUNC=g_${id}_select_refresh_func
    }
  ;
  INTEGER {EDITABLE, SCALAR}
    experimental {
      LABEL=_("Experimental")
    , FUNC=g_${id}_select_refresh_func
    , TOGGLE
    }
  , inPreparation {
      LABEL=_("In Preparation")
    , FUNC=g_${id}_select_refresh_func
    , TOGGLE
    }
  , shared {
      LABEL=_("Shared")
    , FUNC=g_${id}_select_refresh_func
    , TOGGLE
    }
  , approved {
      LABEL=_("Approved")
    , FUNC=g_${id}_select_refresh_func
    , TOGGLE
    }
  , tested {
      LABEL=_("Tested")
    , FUNC=g_${id}_select_refresh_func
    , TOGGLE
    }
  , obsolete {
      LABEL=_("Obsolete")
    , FUNC=g_${id}_select_refresh_func
    , TOGGLE
    }
  , my${type}s {
      LABEL=_("My ${id}s")
    , HELPTEXT=_("Only my ${id}s")
    , FUNC=g_${id}_select_refresh_func
    , TOGGLE
    }
  , excludeOld {
      LABEL="< 3 years"
    , HELPTEXT="Exclude ${id}s not modificated in the last 3 years"
    , FUNC=g_${id}_select_refresh_func
    , TOGGLE
    }
  ;

  ${type}SelectFilter {SCALAR}
    filter
  ;

  Component
    list
  ;
};
${type}Select {SCALAR}
  ${id}_select
;

END DATAPOOL;
