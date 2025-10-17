// **********************************************************************
// Motor
// **********************************************************************
INDEX
  navIndex_motor {
    FUNC=motor_navIndex_func
  }
;
#include <g_motor_uiManager.inc>

FIELDGROUP motor_top_section_fg(
  motor[navIndex_motor].name:25
  motor[navIndex_motor].rev:4
  VOID(10, 0)
  motor[navIndex_motor].approval:12
  VOID(20, 0)
  motor_library.open { PIXMAP="open" }
  motor_library.new { PIXMAP="new" }
  motor_library.store { PIXMAP="store" },

 motor[navIndex_motor].desc:50 {COLSPAN=4}
  STRETCH(1, 0),
  VOID(0, 2),
  SEPARATOR,
  VOID(0, 2)
);

FIELDGROUP motor_properties_fg(
  LABEL(motor.p)        motor.p         UNIT(motor.p),
  LABEL(motor.psityp)   motor.psityp    UNIT(motor.psityp),
  LABEL(motor.rs_Tref)  motor.rs_Tref   UNIT(motor.rs_Tref),
  LABEL(motor.rr_Tref)  motor.rr_Tref   UNIT(motor.rr_Tref),
  LABEL(motor.Lls)      motor.Lls*1e3   UNIT(motor.Lls),
  LABEL(motor.Lh)       motor.Lh*1e3    UNIT(motor.Lh),
  LABEL(motor.rh)       motor.rh        UNIT(motor.rh),
  LABEL(motor.Llr)      motor.Llr*1e3   UNIT(motor.Llr),
  LABEL(motor.Iin)      motor.Iin       UNIT(motor.Iin)
);

FORM motor_form{_("Motor properties")}(
  (
    (
      motor_top_section_fg,
      motor_properties_fg
    )
  )
);
