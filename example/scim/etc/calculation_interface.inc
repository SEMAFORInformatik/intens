DATAPOOL
  STRUCT
    MqResponse {
      STRING {SCALAR} status;
      STRING message;
    }
  ;
  MqResponse mq_response;
  STRUCT ResultReport {
    CDATA short_circuit;
  };
  ResultReport report;
END DATAPOOL;

UI_MANAGER
FIELDGROUP short_circuit_report_fg (
  report.short_circuit:80:10 {EXPAND}
);

FORM short_circuit_report_form {
  _("Short Circuit Report"), HIDDEN } (
  short_circuit_report_fg
);

MENU "Reports" (
  FORM short_circuit_report_form
);
END UI_MANAGER;

STREAMER
  sc_input_stream{JSON}(motor, variant.short_circuit);
  sc_output_stream{JSON}(result);
  report_input_stream{JSON}(motor, variant.short_circuit, result);
  report_output_stream{JSON}(report.short_circuit);
  mq_response_stream{JSON}(mq_response);
END STREAMER;

OPERATOR
  PROCESS imsc_python: BATCH{"scim_calc.py"};
  PROCESS imsc_matlab: MATLAB;

  PROCESSGROUP
    imsc_python_pg { _("Calculate (Python)"), HIDDEN, SILENT, NO_LOG }(
      sc_output_stream{DISPLAY=NONE} = imsc_python(sc_input_stream);
    ),
    imsc_matlab_pg { _("Calculate (Matlab)"), HIDDEN, SILENT, NO_LOG }(
      [result] = imsc_matlab(motor, variant.short_circuit);
    )
  ;

  MESSAGE_QUEUE python_mq {
    REQUEST,
    HOST=RESOURCE("API_GATEWAY_HOST"),
    PORT_REQUEST=RESOURCE("API_GATEWAY_PORT"),
    TIMEOUT=0
  };

  TASK calculate {_("Calculate (api-gateway)")}{
    REQUEST(MESSAGE_QUEUE=python_mq,
              HEADER="scimcalc",
              REQUEST(sc_input_stream),
              RESPONSE(mq_response_stream, sc_output_stream)
            );
   IF (mq_response.status != "ok" && VALID(mq_response.message)) {
       MESSAGEBOX("<h3>" & _("Message") & "</h3><p>" + mq_response.message + "</p>");
   }
  };
  TASK report_task {_("Report")}{
    REQUEST(MESSAGE_QUEUE=python_mq,
              HEADER="report",
              REQUEST(report_input_stream),
              RESPONSE(mq_response_stream, report_output_stream)
            );
   IF (mq_response.status != "ok" && VALID(mq_response.message)) {
       MESSAGEBOX("<h3>" & _("Message") & "</h3><p>" + mq_response.message + "</p>");
     }
  };
END OPERATOR;
