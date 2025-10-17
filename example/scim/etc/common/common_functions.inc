// **********************************************************************
// Common
// **********************************************************************
FUNC INIT {
  RUN(project_init_func);
  RUN(variant_init_func);

  // Db-URL for ebtens
  RESTBASE = RESOURCE("REST_SERVICE_BASE");

  // init components
  RUN(g_project_init_allcomp_func);

  // log level
  python_logger.log_level = "info";

  RUN(sample_data);
};

FUNC sample_data{
  motor.p = 2;
  motor.psityp = 1;
  motor.rs_Tref = 0.1;
  motor.rr_Tref = 0.1;
  motor.Lls = 1e-3;
  motor.Lh = 30e-3;
  motor.rh = 1000;
  motor.Llr = 1e-3;
  motor.Iin = 2;

  variant.short_circuit.nsamples = 1000;
  variant.short_circuit.u1 = 500;
  variant.short_circuit.psi = 1;
  variant.short_circuit.tload = 200;
  variant.short_circuit.speed = 1700/60.;
  variant.short_circuit.tshort = 0e-3;
  variant.short_circuit.tend = 50e-3;
};

// INIT-Function for Webtens
FUNC WEB_INIT {
  RETURN;
};

FUNC get_comp_usage {
  RETURN;
};

FUNC QUIT {
  CLEAR(project_library);
#if defined(HAVE_DB)
  project_library.whichButtonPressed = "close";
  RUN(project_saveIfModified_func);
#endif
  UNMAP(modifications_form);
  UNMAP(project_overview_form);
};

// FUNC common_create_approval_tool_tip_func {
//   // PREQUISITE: comp_id must be set
//   INTEGER last;
//   RUN(project_selectApprovalHist_db);
//   SIZE(approval_report.approval_report_list.approver[*], last);
//   IF(last < 1){
//     retVal = "(none)";
//   }
//   ELSE {
//     last = last - 1;
//     retVal =
//       approval_report.approval_report_list.status[last] + ": "
//     + approval_report.approval_report_list.organization[last] + "/"
//     + approval_report.approval_report_list.approver[last] + " "
//     + approval_report.approval_report_list.date[last];
//   }
// };

// FUNC common_approve_component_func {
//   STRING class;
//   INTEGER i, size;

//   IF(VALID(THIS.id)) {
//     comp_id = THIS.id;
//     THIS.approval_state = "approved";
//     RUN(approveObject_tr);

//     // update status in loaded component
//     class = CLASSNAME(THIS);
//     SIZE(VAR(class), size);
//     i = 0;
//     WHILE(i < size) {
//       IF(VAR(class)[i].id == THIS.id) {
//         VAR(class)[i].approval_state = "approved";
//       }
//       i++;
//     }

//   } ELSE {
//     MESSAGEBOX("No object to approve");
//   }
// };

FUNC component_coverage_radio_func {
  CLEAR(THIS.coverageAll);
  CLEAR(THIS.coverageProject);
  INPUT = 1;
};

FUNC compStatus_search_flags_func {
  IF(THIS.statusInPreparation == 1) {
    THIS.searchFlags[0] = "inPreparation";
  }
  ELSE {
    THIS.searchFlags[0] = " ";
  }
  IF(THIS.statusShared == 1) {
    THIS.searchFlags[1] = "shared";
  }
  ELSE {
    THIS.searchFlags[1] = " ";
  }
  IF(THIS.statusApproved == 1) {
    THIS.searchFlags[2] = "approved";
  }
  ELSE {
    THIS.searchFlags[2] = " ";
  }
  IF(THIS.statusTendered == 1) {
    THIS.searchFlags[3] = "tendered";
  }
  ELSE {
    THIS.searchFlags[3] = " ";
  }
  IF(THIS.statusTested == 1) {
    THIS.searchFlags[4] = "tested";
  }
  ELSE {
    THIS.searchFlags[4] = " ";
  }
  IF(THIS.statusObsolete == 1) {
    THIS.searchFlags[5] = "obsolete";
  }
  ELSE {
    THIS.searchFlags[5] = " ";
  }
};

FUNC common_get_all_users_func {
  RETURN;
  // RUN(get_users_db);
  // MAP(all_users_form);
};

FUNC AFTER_UPDATE_FORMS {
  // project
  IF(MODIFIED(project) || MODIFIED(variant)) {
    ENABLE(project_library.store);  // PRINT("project", EOLN);
  } ELSE {
    DISABLE(project_library.store);
  }

  // variant
  IF( VALID(project.name) ){
    ENABLE(project_library.import);
    ENABLE(project_library.new_variant);
  }
  ELSE {
    DISABLE(project_library.import);
    DISABLE(project_library.new_variant);
  }

  // components
// BEGIN BLOCK COMPONENT
//  IF(MODIFIED(<component>[navIndex_<component>])) {
//    ENABLE(<component>_library.store);
//  } ELSE {
//    DISABLE(<component>_library.store);
//  }
//  IF(MODIFIED(<component>)) {
//    ENABLE(project_library.store);  // PRINT("<component>", EOLN);
//  }
//
// END BLOCK COMPONENT
  IF(MODIFIED(motor[navIndex_motor])) {
    ENABLE(motor_library.store);
  } ELSE {
    DISABLE(motor_library.store);
  }
  IF(MODIFIED(motor)) {
    ENABLE(project_library.store);  // PRINT("motor", EOLN);
  }

};

FUNC datapool_xml_open_func {
  OPEN(datapool_xml_fs);
};
FUNC datapool_json_open_func {
  OPEN(datapool_json_fs);
};
FUNC cycle_xml_open_func {
  OPEN(cycle_xml_fs);
};
FUNC cycle_json_open_func {
  OPEN(cycle_json_fs);
};

FUNC datapool_xml_save_func {
  SAVE(datapool_xml_fs);
};
FUNC datapool_json_save_func {
  SAVE(datapool_json_fs);
};
FUNC cycleXml_save_func {
  SAVE(cycle_xml_fs);
};
FUNC cycleJson_save_func {
  SAVE(cycle_json_fs);
};
