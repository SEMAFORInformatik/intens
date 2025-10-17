UI_MANAGER
  FIELDGROUP results_fg {_("Results")}(
    VOID,
    "<b>" & _("I2T Pulse Energy") & "</b>"{ COLSPAN = 3 },
    _("Pos.")
    result.i2t_pos_max[0]*1e-3:8:1
    result.i2t_pos_max[1]*1e-3:8:1
    result.i2t_pos_max[2]*1e-3:8:1
    UNIT(result.i2t_pos_max),
    _("Neg.")
    result.i2t_neg_max[0]*1e-3:8:1
    result.i2t_neg_max[1]*1e-3:8:1
    result.i2t_neg_max[2]*1e-3:8:1
    UNIT(result.i2t_neg_max),
    VOID
    "<b>" & _("Phase Currents") & "</b>"{ COLSPAN = 3 },
    LABEL(result.ismax)
    result.ismax[0]:8:1
    result.ismax[1]:8:1
    result.ismax[2]:8:1
    UNIT(result.ismax),
    VOID
    result.ismin[0]:8:1
    result.ismin[1]:8:1
    result.ismin[2]:8:1
    UNIT(result.ismax),
    VOID,
    "<b>" & _("Torque") & "</b>"{ COLSPAN = 3 },
    LABEL(result.tpeak)
    result.tpeak*1e-3:8:1
    UNIT(result.tpeak),
    VOID,
    "<b>" & _("Stationary Point") & "</b>"{ COLSPAN = 3 },
    LABEL(result.u1) >
    result.u1:8:2
    UNIT(result.u1),
    LABEL(result.f1) >
    result.f1:8:2
    UNIT(result.f1),
    VOID,
    plot_button
  )
;

PLOT2D sc_plot {_("Short circuit plot")} (
  plot {
    _("Short Circuit Plot"),
    YAXIS1 {LABEL="A"},
    YAXIS2 {LABEL="kNm"}
  }(
    result.t[*]*1e3 {LABEL=_("time/ms")}
    (
      result.is[0,*] {LABEL=_("Is a"), YAXIS1},
      result.is[1,*] {LABEL=_("Is b"), YAXIS1},
      result.is[2,*] {LABEL=_("Is c"), YAXIS1},
      result.torque[*] {LABEL=_("Torque"), YAXIS2}
    )
  )
);

PLOT2D sc_torque_plot {_("Short circuit torque plot")} (
  plot {
    _("Short Circuit Torque Plot"),
    YAXIS1 {LABEL="kNm"}
  }(
    result.t[*]*1e3 {LABEL=_("time/ms")} (
      result.torque[*] {LABEL=_("Torque"), YAXIS1}
    )
  )
);

FOLDER plot_folder (
  [_("All"): plot_tab](
    sc_plot
  ),
  [_("Torque, Current"): plot2_tab](
    sc_torque_plot
  )
);

FORM plot_form {HIDDEN} (plot_folder);

END UI_MANAGER;

FUNCTIONS
  FUNC map_plot_form {
    MAP(plot_form);
  };
END FUNCTIONS;
