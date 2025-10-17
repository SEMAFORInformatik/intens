DATAPOOL
  STRUCT Result {
    REAL
      i2t_pos_max {
        LABEL=_("I2t Max (Pos)"),
        UNIT="kA²s"
      },
      i2t_neg_max {
        LABEL=_("I2t Max (Pos)"),
        UNIT="kA²s"
      },
      u1 {
        LABEL=_("Stator Voltage"),
        UNIT="V"
      },
      f1 {
        LABEL=_("Stator Frequency"),
        UNIT="Hz"
      },
      tpeak {
        LABEL=_("Maximum Torque"),
        UNIT="kNm"
      },
      ismin {
        LABEL=_("Is min"),
        UNIT="A"
      },
      ismax {
        LABEL=_("Is max"),
        UNIT="A"
      },
      t {
        LABEL=_("time"),
        UNIT="s"
      },
      torque {
        LABEL=_("torque"),
        UNIT="Nm"
      },
      is {
        LABEL=_("current"),
        UNIT="A",
        HELPTEXT=_("Phase Currents")
      },
      i2t_pos {
        LABEL=_("I2T pos"),
        UNIT="kA²s",
        HELPTEXT=_("I2T Pulse Energy for positive currents")
      },
      i2t_neg {
        LABEL=_("I2T neg"),
        UNIT="kA²s",
        HELPTEXT=_("I2T Pulse Energy for negative currents")
      };
    }
  ;

  Result result;

  INTEGER {EDITABLE}
    plot_button {
      LABEL=_("Plot..."),
      BUTTON,
      FUNC=map_plot_form
    }
  ;
END DATAPOOL;
