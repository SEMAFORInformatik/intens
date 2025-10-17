// **********************************************************************
// Approval Report
// **********************************************************************
DATAPOOL

STRUCT ApprovalReport_list {
  STRING
    approver {
      LABEL=_("Approver")
    , HELPTEXT=_("Approver")
    }
  , organization {
      LABEL=_("Organization")
    , HELPTEXT=_("Organization")
    }
  , date {
      LABEL=_("Date")
    , HELPTEXT=_("Date")
    }
  , status {
      LABEL=_("Status")
    , HELPTEXT=_("Status")
    }
  ;
};


STRUCT ApprovalReport {
  STRING { EDITABLE }
    approvalIndicator
  ;
  STRING
    approvalObjectName
  ;
  INTEGER
    approvalRevision
  ;
  STRING {  EDITABLE }
    print {
      LABEL=_("Print")
    , HELPTEXT=_("Print this history")
    , BUTTON
    , FUNC=project_approval_report_print_func
    }
  , saveToFile {
      LABEL=_("Save to file")
    , HELPTEXT=_("Save an image of this history")
    , BUTTON
    , FUNC=project_approval_report_save_func
    }
  ;
  ApprovalReport_list
    approval_report_list;
  }
;
ApprovalReport
  approval_report
;

END DATAPOOL;

UI_MANAGER

LIST approval_report_list {
  TABLESIZE=20
} (
  LABEL(approval_report.approval_report_list.approver)
  approval_report.approval_report_list.approver[*]:20
, LABEL(approval_report.approval_report_list.organization)
  approval_report.approval_report_list.organization[*]:20
, LABEL(approval_report.approval_report_list.date)
  approval_report.approval_report_list.date[*]:10
, LABEL(approval_report.approval_report_list.status)
  approval_report.approval_report_list.status[*]:15
);

FIELDGROUP approval_reportButtons_fg (
  approval_report.saveToFile VOID(10, 0)  approval_report.print
);

FIELDGROUP approval_reportSection_fg {_("Approval history")} (
  approval_report.approvalIndicator:50
, approval_report_list
);

NAVIGATOR approval_navi {
  COL (
    "Active Project" nametag:30
  , "Rev" revisiontag:4
  , "Date of status" datetag:10
  , "Status" approval_state_tag:2 PIXMAP
  )
} (
  project[*] = "Project" {
    AUTOLEVEL
  , HIDE_EMPTY_FOLDER
  }
);

END UI_MANAGER;
