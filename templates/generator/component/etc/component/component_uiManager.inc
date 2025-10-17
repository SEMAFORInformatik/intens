// **********************************************************************
// ${componentTypeName}
// **********************************************************************
INDEX
  navIndex_${component} {
    FUNC=${component}_navIndex_func
  }
;
#include <g_${component}_uiManager.inc>

FIELDGROUP ${component}_top_section_fg(
  ${component}[navIndex_${component}].name:25
  ${component}[navIndex_${component}].rev:4
  VOID(10, 0)
  ${component}[navIndex_${component}].approval:12
  VOID(20, 0)
  ${component}_library.open { PIXMAP="open" }
  ${component}_library.new { PIXMAP="new" }
  ${component}_library.store { PIXMAP="store" },

 ${component}[navIndex_${component}].desc:50 {COLSPAN=4}
  STRETCH(1, 0),
  VOID(0, 2),
  SEPARATOR,
  VOID(0, 2)
);

FIELDGROUP ${component}_properties_fg(
  VOID
);

FORM ${component}_form{_("${componentTypeName} properties")}(
  (
    (
      ${component}_top_section_fg,
      ${component}_properties_fg
    )
  )
);
