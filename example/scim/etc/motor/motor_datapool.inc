// **********************************************************************
// Motor
// **********************************************************************
STRUCT Motor : Component {
    INTEGER {EDITABLE, SCALAR}
      p {
        LABEL=_("Pole pairs"),
        HELPTEXT=_("Number of pole pairs"),
        DBUNIT="1"
      }
    ;
    REAL {EDITABLE, SCALAR}
      psityp {
        LABEL=_("Nom. Flux"),
        HELPTEXT=_("Stator Type flux"),
        UNIT="Vs"
      },
      rs_Tref {
        LABEL=_("Stator Resistance"),
        HELPTEXT=_("Stator Resistance at 20 deg Celsius"),
        UNIT="Ω"
      },
      rr_Tref {
        LABEL=_("Rotor Resistance"),
        HELPTEXT=_("Rotor Resistance at 20 deg Celsius"),
        UNIT="Ω"
      },
    Lls {
      LABEL=_("Stator Leakage"),
      HELPTEXT=_("Stator leakage inductance"),
      UNIT="mH",
      DBUNIT="H"
    },
    Lh {
      LABEL=_("Magnetizing Inductance"),
      HELPTEXT=_("Magnetizing inductance"),
      UNIT="mH",
      DBUNIT="H"
    },
    Llr {
      LABEL=_("Rotor Leakage"),
      HELPTEXT=_("Rotor leakage inductance"),
      UNIT="mH",
      DBUNIT="H"
    },
    rh {
      LABEL=_("Iron Resistance"),
      HELPTEXT=_("Resistance representing Iron Losses"),
      UNIT="Ω"
    },
    Iin {
      LABEL=_("Moment of Inertia"),
      HELPTEXT=_("Moment of Inertia of rotor"),
      UNIT="kg*m²"
    }
  ;
};

Motor
  motor {
    LABEL=_("Motor"),
    PERSISTENT, SCALAR
  }
;

STRUCT MotorLibrary : ComponentLibrary {
  STRING {EDITABLE}
    open {
      BUTTON,
      HELPTEXT=_("Open a Motor."),
      FUNC=motor_open_func
    },
    new {
      BUTTON,
      HELPTEXT=_("Create a new Motor."),
      FUNC=motor_new_func
    },
    store {
      BUTTON,
      HELPTEXT=_("Store the Motor."),
      FUNC=motor_store_func
    }
  ;

};

MotorLibrary {SCALAR}
  motor_library
;
