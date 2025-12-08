
%{
#include "utils/gettext.h"

#include "datapool/DataDictionary.h"
#include "app/DataSet.h"
#include "gui/GuiElement.h"
#include "gui/GuiDataField.h"
#include "gui/GuiForm.h"
#include "gui/GuiNavigator.h"
#include "parser/Configurator.h"
#include "parser/errorhandler.h"
#include "app/App.h"
#include "operator/ProcessGroup.h"
#include "parser/Flexer.h"

int yylex(void);
static DataDictionary::DataType datatype;
static std::string              parent_name;
static DataDictionary         * dict_item = 0;
static int                      ndata_items = 0;
static DataSet                * dataset;
static Configurator           * configurator = 0;
static int                      nset_items;
static int                      i;
static bool                     indexed;
std::string                     ErrorItemName;

static XferDataItem::ParameterType parametertype;

%}

%union{
  int                        val_integer;
  double                     val_real;
  std::string               *val_string;
  bool                       val_bool;
  GuiElement::Orientation    val_orient;
  GuiElement::Alignment      val_align;
  GuiElement::ScrollbarType  val_scrollbar;
  DataDictionary::DataType   val_datatype;
  GuiPlotDataItem           *val_plotdataitem;
  GuiListenerController     *val_listenerctrl;
  GuiElement                *val_guielement;
  DataReference             *val_dataref;
  JobCodeOpWhile            *val_codewhile;
  JobCodeOpIf               *val_codeif;
  JobFunction               *val_function;
  XferDataItem              *val_xferitem;
  char                       val_char;
};
%token  START_DESCRIPTION START_DATAPOOL START_POSTLOAD

%token  tVERSION
%token  tDESCRIPTION
%token  <val_string>   tIDENTIFIER
%token  <val_string>   tID_SOCKET
%token  <val_string>   tID_MESSAGE_QUEUE
%token  <val_string>   tID_TIMER
%token  <val_string>   tID_DATAVARIABLE
%token  <val_string>   tID_DATASTRUCTURE
%token  <val_string>   tID_DATASET
%token  <val_string>   tID_COLORSET
%token  <val_string>   tID_TAG
%token  <val_string>   tID_FORM
%token  <val_string>   tID_FIELDGROUP
%token  <val_string>   tID_THERMO
%token  <val_string>   tID_FOLDER
%token  <val_string>   tID_FOLDERGROUP
%token  <val_string>   tID_INDEX
%token  <val_string>   tID_UNIPLOT
%token  <val_string>   tID_PSPLOT
%token  <val_string>   tID_LIST
%token  <val_string>   tID_LISTPLOT
%token  <val_string>   tID_XRT3DPLOT
%token  <val_string>   tID_PLOT3D
%token  <val_string>   tID_IMAGE
%token  <val_string>   tID_LINEPLOT
%token  <val_string>   tID_PLOT2D
%token  <val_string>   tID_TEXTWINDOW
%token  <val_string>   tID_NAVIGATOR
%token  <val_string>   tID_PROCESSGROUP
%token  <val_string>   tID_FILESTREAM
%token  <val_string>   tID_REPORTSTREAM
%token  <val_string>   tID_TASK
%token  <val_string>   tID_FUNCTION
%token  <val_string>   tID_PYTHON
%token  <val_string>   tID_PLUGIN
%token  <val_string>   tID_MENU
%token  <val_string>   tID_STREAM
%token  <val_string>   tID_PROCESS
%token  <val_string>   tID_TABLE
%token  <val_string>   tID_TIMETABLE
%token  <val_string>   tID_PROGRESSBAR
%token  <val_integer>  tINT_CONSTANT
%token  <val_string>   tSQL_STRING
%token  <val_string>   tSTRING_CONSTANT
%token  <val_char>     tCHAR_CONSTANT
%token  <val_real>     tREAL_CONSTANT
%token  <val_string>   tPOSTGRES
%token  tRESOURCE_UNKNOWN

%token  tREAL_INVALID
%token  tDATAPOOL tCYCLE
%token  tINTEGER tREAL tCOMPLEX tSTRING tCDATA tSTRUCT
%token  tIMAG tABS tSIN tTAN tCOS tASIN tATAN tATAN2 tACOS tLOG tLOG10
%token  tSQRT tARG tLENGTH
%token  tSORT tSORTORDER
%token  tSET tINDEXED_SET tUNSET tOF
%token  tEDITABLE tOPTIONAL tLOCKABLE tGLOBAL tOMIT_TTRAIL tOMIT_ACTIVATE
%token  tCLOSE_BUTTON tBUTTON tTOGGLE tRADIO tCOMBOBOX tSLIDER tPROGRESS
%token  tTAG
%token  tEOLN
%token  tOPERATOR
%token  tSIZE tDATA_SIZE
%token  tCOMPARE tCOMPOSE tCOMPOSE_STRING
%token  tASSIGN_CONSISTENCY
%token  tSET_RESOURCE
%token  tTABLESIZE tSTEP tRANGE tOFFSET
%token  tALARM_LEVEL tALARM_COLOR tINVERTED
%token  tJUSTIFY tJUSTRIGHT tJUSTLEFT tJUSTCENTER tSTRETCH tEXPAND tSCROLL tALIGN
%token  tOVERLAY
%token  tTABLE tLIST tSORT_CRITERIA tORIENTATION tHORIZONTAL tVERTICAL tNAVIGATION tMARGIN tROTATE_180
%token  tPOSITION
%token  tARROWS
%token  tMAIN tAPP_MODAL tHIDDEN tPERSISTENT tSILENT tDEADLY_SILENT tNO_LOG
%token  tEXTENSION
%token  tHIDE_CYCLE
%token  tBUTTONS
%token  tSCROLLBARS tNO_SCROLLBARS tAUTO_SCROLL
%token  tPANEDWINDOW tNO_PANEDWINDOW
%token  tFRAME
%token  tSTD_WINDOW
%token  tLOG_WINDOW
%token  tBUTTON_CANCEL tBUTTON_YES tBUTTON_NO
%token  tWRAP
%token  tWEBAPI
%token  tSTREAMER tMATRIX tSCALAR tDBATTR tDBUNIT tTRANSIENT tCELL tDELIMITER tLOCALE
%token  tSKIP
%token  tFORM
%token  tSET_CURRENT_FORM
%token  tSTREAM tFILE tFILESTREAM tREPORTSTREAM tLATEXREPORT
%token  tBATCH tDAEMON tMATLAB
%token  tEND
%token  tUI_MANAGER
%token  tUI_UPDATE
%token  tFIELDGROUP tFOLDER
%token  tLOGON
%token  tLOGOFF
%token  tQUERY
%token  tVOID
%token  tMODIFY
%token  tMODIFIED tSET_DB_TIMESTAMP
%token  tMOVE
%token  tREASON
%token  tICON tICONVIEW tDIAGRAM
%token  tPLOTGROUP
%token  tXMLGROUP /* DOCUMENTATION:HIDE (not implemented in intens4) */
%token  tXML tATTRS
%token  tJSON
%token  tPROTO
%token  tINDENT
%token  tAPPEND
%token  tNO_GZ
%token  tSCHEMA tNAMESPACE
%token  tCAPTION
%token  tXAXIS tXAXIS2 tYAXIS
%token  tXANNOTATION tYANNOTATION tANNOTATION tMARKER tLEGEND
%token  tASPECT_RATIO tASPECT_RATIO_REF_AXIS
%token  tLOG_X tLOG_Y
%token  tFILTER
%token  tFILENAME tBASENAME tDIRNAME
%token  tRESET
%token  tREADONLY
%token  tWRITEONLY
%token  tIGNORE
%token  tDEPENDENCIES tNO_DEPENDENCIES tAUTOCLEAR_DEPENDENCIES
%token  tPROCESS tPROCESSGROUP
%token  tSOCKET tPORT tPORT_REQUEST tTIMEOUT tHEADER tRESPONSE tSETTINGS tMFM tTHUMBNAIL
%token  tMESSAGE_QUEUE tPUBLISH tSUBSCRIBE tREPLY tREQUEST tSET_MQ_HOST
%token  tTIMER tSTART tSTOP tPERIOD tDELAY tMAX_PENDING_FUNCTIONS
%token  tAXES_ORIGIN tAXES_ORIGIN_X tAXES_ORIGIN_Y
%token  tYAXIS1 tYAXIS2
%token  tSQL_ERROR
%token  tLISTPLOT tUNIPLOT tXRT3DPLOT tPLOT3D tXRTGRAPH tPLOT2D tPSPLOT
%token  tIMAGE tLINEPLOT
%token  tTHERMO tTIMETABLE
%token  tMINMAX tRMS tAVG
%token  tGET tDELETE tPOST tPUT tPATH tDATA
%token  tREST_LOGON tREST_JWT_LOGON tREST_LOGOFF

%token  tSTYLE  tY1_STYLE tY2_STYLE tSURFACE tCONTOUR

%token  tLINESTYLE tDISCRETE tLINEAR tBAR tSTACKING_BAR tPLOT tAREA tPOLAR tDOTS
%token  tHELPFILE tHELPKEY tHELPTEXT tCLASS tCLASSNAME tPATTERN tMAX_OCCURS tOPEN_FILE tOPEN_URL
%token  tPLACEHOLDER
%token  tALWAYS tSAME_YRANGE tLABEL tPIXMAP tDATASET_TEXT tUNIT tDISPLAY tNONE
%token  tSTRING_DATE tSTRING_TIME tSTRING_DATETIME tSTRING_VALUE tPASSWORD tWHEEL_EVENT
%token  tFROM_STRING_DATETIME tTO_STRING_DATETIME tTO_STRING_TIME
%token  tCOLSPAN tROWSPAN tCHANGED
%token  tMENU tSEPARATOR tTRANSACTION tTEXT_WINDOW tFORTRAN tEXPLICIT
%token  tLATEX tTEMPLATE tASCII tBINARY tFORMAT tURL
%token  tFUNCTIONS tFUNC tON_EOS tON_VIEW_ACTION tROW tCOL tTOOLTIP
%token  tPRIORITY tHIGH
%token  tFOCUS_FUNC
%token  tSET_ERROR tRESET_ERROR
%token  tSET_MSG
%token  tDESCRIPTION_FILE
%token  tIN tOUT tPLUGIN tSTYLESHEET
%token  tOPEN tSAVE tSEND tHOST
%token  tREPORT tPREVIEW
%token  tLOAD
%token  tBEGINTRANSACTION
%token  tCOMMITTRANSACTION
%token  tABORTTRANSACTION
%token  tNAVIGATOR tLASTLEVEL tFIRSTLEVEL tAUTOLEVEL tOPENLEVELS tHIDE_EMPTY_FOLDER
%token  tTSEP
%token  tSERIALIZE_FORM tSERIALIZE
%token  tWRITE_SETTINGS
%token  tCURRENT_TIME tCURRENT_DATE tCURRENT_DATETIME

%type <val_string>   application_name
%type <val_string>   index_identifier
%type <val_string>   identifier data_identifier
%type <val_string>   string_constant string_const string_element string_element_compose id_or_string_constant
%type <val_string>   filename_string document_string title_string label_string bg_color_string fg_color_string
%type <val_char>     char_const
%type <val_real>     real_value scale_factor opt_scale_factor
%type <val_real>     real_or_int_value
%type <val_integer>  sign width precision
%type <val_dataref>  data_reference
%type <val_string>   data_variable
%type <val_integer>  data_colorset_value_less_or_greater
%type <val_integer>  data_colorset_value_less
%type <val_integer>  data_colorset_value_greater
%type <val_integer>  menu_menu_declaration menu_submenu_declaration
%type <val_string>   menu_button_label
%type <val_integer>  ui_structure_menu_option
%type <val_string>   report_identifier report_save_identifier
%type <val_bool>     bool_constant bool_assignment

%type <val_string>   ui_file_identifier
%type <val_integer>  st_field_length st_opt_flag
%type <val_xferitem> st_data_reference temp_data_reference ui_xfer
%type <val_integer>  job_changed_option
%type <val_integer>  job_message_queue_dependencies
%type <val_char>     st_matrix_option
%type <val_string>   ui_form_element_identifier
%type <val_integer>  ui_form_void_size
%type <val_integer>  ui_fieldgroup_table_step ui_fieldgroup_spacing ui_table_range_max ui_index_range_max
%type <val_orient>   ui_orientation
%type <val_scrollbar> ui_scrollbar_options
%type <val_orient>   ui_scrollbar_orientation
%type <val_align>    ui_alignment ui_field_alignment ui_fieldgroup_index_justify
%type <val_integer>  ui_field_length ui_field_precision
%type <val_bool>     ui_form_menu_button_toggle ui_field_thousand_sep
%type <val_string>   ui_plugin ui_plugin_list
%type <val_integer>  ui_tbl_range_max
%type <val_string>   ui_plot3d_graph
%type <val_string>   ui_xrtgraph_graph
%type <val_string>   ui_plot2d_graph
%type <val_integer>  ui_form_menu_declaration ui_menu_declaration ui_menu_list
%type <val_function> ui_folder_group_function
%type <val_string>   ui_navigator_col_label
%type <val_string>   ui_navigator_hide_folder
%type <val_integer>   ui_navigator_col_style
%type <val_string>   ui_navigator_root_label
%type <val_bool>     ui_file_menu_option

%type <val_string>   op_exe_command op_exe_command_list op_exe_cmd
%type <val_string>   op_textwindow_name op_option_fifo opt_op_option_fifo
%type <val_integer>  op_stream_file_format
%type <val_integer>  op_menu_declaration op_menu_list

%type <val_string>   struct_data_reference

%token  tTOP tBOTTOM tLEFT tRIGHT
%token  tBEEP
%token  tTASK tIF tELSE tWHILE tPRINT tPRINT_LOG tPRINTSTYLE tRUN tUPDATE_FORMS
%token  tFIRSTCYCLE tLASTCYCLE tNEXTCYCLE tGOCYCLE
%token  tNEWCYCLE tDELETECYCLE tCLEARCYCLE
%token  tGETCYCLE tMAXCYCLE
%token  tALLCYCLES
%token  tCYCLENAME
%token  tGUI_ELEMENT_METHOD
%token  tVALID tINVALID tINPUT tOLDVALUE tINDEX
%token  tDEBUG tINFO tWARN tERROR tFATAL
%token  tTHIS tSET_THIS tSET_INDEX tSET_REASON tSET_FUNC_ARGS
%token  tPARENT tSOURCE tSOURCE2 tBASE tDIAGRAM_XPOS tDIAGRAM_YPOS tNODE
%token  tMAP tUNMAP
%token  tVISIBLE
%token  tCLEAR
%token  tERASE
%token  tPACK
%token  tPASTE tCOPY
%token  tSELECT_LIST tGET_SELECTION tGET_SORT_CRITERIA tMULTIPLE_SELECTION tCLEAR_SELECTION
%token  tALLOW tDISALLOW
%token  tENABLE tDISABLE
%token  tDISABLE_DRAG
%token  tROUND tROUND5 tROUND10
%token  tCONFIRM tCONFIRM_CANCEL tGETTEXT tMESSAGEBOX
%token  tASSIGN_CORR
%token  tRETURN tABORT tEXIT
%token  tREASON_INPUT
%token  tREASON_INSERT
%token  tREASON_DUPLICATE
%token  tREASON_CLEAR
%token  tREASON_REMOVE
%token  tREASON_PACK
%token  tREASON_SELECT
%token  tREASON_UNSELECT
%token  tREASON_SELECT_POINT
%token  tREASON_SELECT_RECTANGLE
%token  tREASON_SORT
%token  tREASON_ACTIVATE
%token  tREASON_OPEN
%token  tREASON_CLOSE
%token  tREASON_DROP tDROP
%token  tREASON_MOVE
%token  tREASON_NEW_CONNECTION
%token  tREASON_REMOVE_CONNECTION
%token  tREASON_REMOVE_ELEMENT
%token  tREASON_CYCLE_CLEAR
%token  tREASON_CYCLE_DELETE
%token  tREASON_CYCLE_NEW
%token  tREASON_CYCLE_RENAME
%token  tREASON_CYCLE_SWITCH
%token  tREASON_FOCUS
%token  tREASON_FOCUS_OUT
%token  tREASON_FUNCTION
%token  tREASON_TASK
%token  tREASON_GUI_UPDATE
%token  tREPLACE tBY
%token  tINCR tDECR
%token  tVAR
%token  tTRUE
%token  tFALSE
%token  tLOCK
%token  tCOLOR
%token  tCOLORBIT tNO_COLORBIT
%token  tCOLOR_SCALE
%token  tTIMESTAMP
%token  tTOUCH
%token  tTRANSPARENT
%token  tSCALE
%token  tAUTO_WIDTH

%type <val_function>   job_function_pointer
/// %type <val_string>     job_filename_options
%type <val_integer>    job_statement job_single_statement_list job_single_statement
%type <val_integer>    data_statement file_statement set_statement
%type <val_integer>    gui_statement gui_more_statement
%type <val_integer>    job_set_current_form_by_webapi job__set_current_form_element
%type <val_integer>    cycle_statement print_statement error_statement
%type <val_integer>    exit_statement system_statement messagebox_statement run_statement
%type <val_integer>    return_statement send_statement send_action database_statement
%type <val_integer>    if_statement while_statement copy_paste_statement
%type <val_integer>    timer_statement set_func_statement
%type <val_integer>    message_queue_statement message_queue_action
%type <val_integer>    job_run_statement job_run_function
%type <val_string>     function_reference
%type <val_integer>    job_run_action
%type <val_integer>    job_open_statement job_open_action
%type <val_integer>    job_xml_query_statement job_xml_query_action
%type <val_integer>    job_description_statement job_description_action
%type <val_integer>    job_save_statement job_save_action
%type <val_integer>    job_changed_action
%type <val_integer>    job_map job_map_element_list job_map_element
%type <val_integer>    job_map_omit
%type <val_integer>    job_unmap job_unmap_element_list job_unmap_element
%type <val_integer>    job_visible_action
%type <val_integer>    job_editable_action
%type <val_listenerctrl> job_listener_controller
%type <val_guielement> job_enable_disable_element
%type <val_string>     job_gui_element
%type <val_integer>    job_gui_element_method
%type <val_integer>    job_allow job_allow_element_list job_allow_element
%type <val_integer>    job_select_list job_select_element
%type <val_string>     job_select_identifier
%type <val_integer>    job_disallow job_disallow_element_list job_disallow_element
%type <val_integer>    job_enable job_enable_object_list job_enable_object
%type <val_integer>    job_disable job_disable_object_list job_disable_object
%type <val_integer>    job_disable_drag job_disable_drag_object_list job_disable_drag_object
%type <val_integer>    job_enable_disable_cycle
%type <val_integer>    job_set_attributes job_unset_attributes
%type <val_integer>    job_update job_update_element_list job_update_element
%type <val_integer>    job_ref_with_wildcards
%type <val_integer>    job_size job_data_size job_index
%type <val_integer>    job_class
%type <val_integer>    job_compare
%type <val_integer>    job_compose
%type <val_integer>    job_assign_consistency
%type <val_integer>    job_set_resource
%type <val_integer>    job_clear job_clear_element_list job_clear_element
%type <val_integer>    job_erase job_erase_element_list job_erase_element
%type <val_integer>    job_pack job_pack_element job_pack_row_or_col
%type <val_integer>    job_while_statement
%type <val_codewhile>  job_while_expression
%type <val_integer>    job_if_statement
%type <val_codeif>     job_if_expression
%type <val_integer>    job_if_stmt
%type <val_integer>    job_else_stmt
%type <val_integer>    job_next_address
%type <val_string>     job_form_identifier
%type <val_string>     job_ui_element_identifier
%type <val_string>     job_folder_group_identifier
%type <val_integer>    opt_job_map_element_folder_option job_map_element_folder_option
%type <val_integer>    job_expression
%type <val_integer>    job_confirm
%type <val_integer>    data_expression cycle_expression function_expression
%type <val_integer>    math_expression file_expression reason_expression
%type <val_integer>    job_boolean_expression
%type <val_integer>    job_data_reference job_data_reference_boolean
%type <val_string>     job_data_struct_identifier
%type <val_integer>    job_data_identifier job_dataset_identifier job_stringdate_identifier job_stringdatetime_identifier job_stringtime_identifier job_stringvalue_identifier
%type <val_integer>    job_data_this job_data_base
%type <val_integer>    job_data_source job_data_source_idx job_data_source2
%type <val_integer>    job_print_elements job_print_element_list job_print_element
%type <val_string>     job_log_level
%type <val_integer>    job_log_elements job_log_element_list job_log_element
%type <val_integer>    job_error_elements job_error_element_list job_error_element
%type <val_integer>    job_message_elements job_message_element_list job_message_element
%type <val_integer>    job_assign_corresponding
%type <val_integer>    job_string_expression job_string_list job_string
%type <val_integer>    job_cyclenumber
%type <val_integer>    job_get_selection job_get_selection_element
%type <val_integer>    job_get_sort_criteria job_get_sort_criteria_element
%type <val_integer>    job_set_stylesheet job_set_stylesheet_element
%type <val_integer>    job_clear_selection job_clear_selection_element
%type <val_integer>    job_paste job_paste_element job_paste_element_list
%type <val_integer>    job_copy job_copy_element job_copy_element_list
%type <val_integer>    restService_statement restService_action

%right  '='
%right  tACCUMULATE
%left   tOR
%left   tAND
%left   '>' tGEQ '<' tLEQ tEQL tNEQ
%left   '+' '-'
%left   '*' '/' '%'
%left   UNARYMINUS tNOT
%right  '^'

%start start
%%

start: START_DESCRIPTION description
   | START_DATAPOOL datapool_description
   | START_POSTLOAD descriptions

/* *************************************************************************** */
/* Intens Application Description Language                                     */
/* *************************************************************************** */

description /* DOCUMENTATION:DIAGRAM */
  : tDESCRIPTION
      {
        configurator=App::Instance().getConfigurator();
        configurator -> start();
      }
    application_name ';'
      {
        configurator -> setApplicationTitle( *$3 );
        delete $3;
      }
    descriptions
    tEND '.'
      {
        configurator -> end();
      }
  ;

application_name /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER      { $$ = $1; }
  | string_constant  { $$ = $1; }
  ;
descriptions /* DOCUMENTATION:DIAGRAM */
  : description_list
      { /* PCfixupItemFunctionList(); */ }
  ;
description_list /* DOCUMENTATION:UNFOLD */
  : description_part
  | description_list description_part
  ;
description_part /* DOCUMENTATION:UNFOLD */
  : helpfile_description
      {
        configurator -> dispatchPendingEvents();
      }
  | datapool_description
      {
        configurator -> dispatchPendingEvents();
      }
  | streamer_description
      {
        configurator -> dispatchPendingEvents();
      }
  | ui_manager_description
      {
        configurator -> dispatchPendingEvents();
      }
  | operator_description
      {
        configurator -> dispatchPendingEvents();
      }
  | functions_description
      {
        configurator -> dispatchPendingEvents();
      }
  ;

/* *************************************************************************** */
/* Global Definitions                                                          */
/* *************************************************************************** */

/* --------------------------------------------------------------------------- */
/* Bool Constant                                                               */
/* --------------------------------------------------------------------------- */

bool_constant /* DOCUMENTATION:UNFOLD */
  : tTRUE  { $$ = true; }
  | tFALSE { $$ = false; }
  ;

/* --------------------------------------------------------------------------- */
/* Bool Assignment                                                               */
/* --------------------------------------------------------------------------- */

bool_assignment /* DOCUMENTATION:UNFOLD */
  : /* none */ { $$ = true; }
  | '=' bool_constant { $$ = $2; }
  ;

/* --------------------------------------------------------------------------- */
/* String Constant                                                             */
/* --------------------------------------------------------------------------- */

string_constant /* DOCUMENTATION:UNFOLD */
  : string_const { $$ = configurator -> convertString( *($1 ) ); }
  ;

string_const /* DOCUMENTATION:DIAGRAM */
  : string_element_compose
  | string_const '&' string_element_compose
      { *($1) += *($3);
        delete $3;
        $$ = $1;
      }
  ;

string_element_compose /* DOCUMENTATION:UNFOLD */
  : string_element
  | tCOMPOSE_STRING '(' string_element ',' tINT_CONSTANT ')'
      { $$ = new std::string(compose(*($3), $5)); }
  | tCOMPOSE_STRING '(' string_element ',' tINT_CONSTANT ',' tINT_CONSTANT ')'
      { $$ = new std::string(compose(*($3), $5, $7)); }
  | tCOMPOSE_STRING '(' string_element ',' tINT_CONSTANT ',' tINT_CONSTANT',' tINT_CONSTANT ')' /* DOCUMENTATION:HIDE BEGIN */
      { $$ = new std::string(compose(*($3), $5, $7, $9)); } /* DOCUMENTATION:HIDE END */
  | tCOMPOSE_STRING '(' string_element ',' string_element ')'
      { $$ = new std::string(compose(*($3), *($5))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ')' /* DOCUMENTATION:HIDE BEGIN */
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9),
                                     *($11))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9),
                                     *($11), *($13))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9),
                                     *($11), *($13), *($15))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9),
                                     *($11), *($13), *($15), *($17))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9),
                                     *($11), *($13), *($15), *($17),
                                     *($19))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9),
                                     *($11), *($13), *($15), *($17),
                                     *($19), *($21))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9),
                                     *($11), *($13), *($15), *($17),
                                     *($19), *($21), *($23))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9),
                                     *($11), *($13), *($15), *($17),
                                     *($19), *($21), *($23), *($25))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9),
                                     *($11), *($13), *($15), *($17),
                                     *($19), *($21), *($23), *($25),
                                     *($27))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9),
                                     *($11), *($13), *($15), *($17),
                                     *($19), *($21), *($23), *($25),
                                     *($27), *($29))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9),
                                     *($11), *($13), *($15), *($17),
                                     *($19), *($21), *($23), *($25),
                                     *($27), *($29), *($31))); }
  | tCOMPOSE_STRING '(' string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ','
                        string_element ',' string_element ',' string_element ',' string_element ')'
      { $$ = new std::string(compose(*($3), *($5), *($7), *($9),
                                     *($11), *($13), *($15), *($17),
                                     *($19), *($21), *($23), *($25),
                                     *($27), *($29), *($31), *($33))); } /* DOCUMENTATION:HIDE END */
  ;

string_element /* DOCUMENTATION:DIAGRAM */
  : tSTRING_CONSTANT { $$ = $1; }
  | tLABEL '(' data_reference ')'
      { $$ = new std::string( static_cast<UserAttr*>(configurator->getDataRef()->getUserAttr())->Label( configurator->getDataRef() ) );
      }
  | tUNIT '(' data_reference ')'
      { $$ = new std::string( static_cast<UserAttr*>(configurator->getDataRef()->getUserAttr())->Unit(true, configurator->getDataRef()) ); }
  ;
  | tUNIT '(' data_reference ',' tLABEL')'
      { $$ = new std::string( static_cast<UserAttr*>(configurator->getDataRef()->getUserAttr())->Unit(false) ); }
  ;
id_or_string_constant /* DOCUMENTATION:UNFOLD */
  : string_constant
  | identifier
  ;

char_const /* DOCUMENTATION:DIAGRAM */
  : tCHAR_CONSTANT { $$ = $1; }
  ;

/* --------------------------------------------------------------------------- */
/* Identifiers                                                                 */
/* --------------------------------------------------------------------------- */

identifier /* DOCUMENTATION:DIAGRAM */
  : tIDENTIFIER
  | tID_DATAVARIABLE
  | tID_DATASTRUCTURE
  | tID_DATASET
  | tID_COLORSET
  | tID_FORM
  | tID_FIELDGROUP
  | tID_TIMER
  | tID_FOLDER
  | tID_INDEX
  | tID_UNIPLOT
  | tID_PSPLOT
  | tID_LISTPLOT
  | tID_XRT3DPLOT /* DOCUMENTATION:HIDE (deprecated, use PLOT3D) */
  | tID_PLOT3D
  | tID_IMAGE
  | tID_LIST
  | tID_LINEPLOT
  | tID_PLOT2D
  | tID_PROCESSGROUP
  | tID_TASK
  | tID_FUNCTION
  | tID_PLUGIN
  | tID_MENU
  | tID_STREAM
  | tID_PROCESS
  | tID_NAVIGATOR
  | tID_TIMETABLE
  | tID_TABLE
  ;
data_identifier /* DOCUMENTATION:DIAGRAM */
  : tIDENTIFIER
  | data_variable
  ;
data_variable /* DOCUMENTATION:UNFOLD */
  : tID_DATAVARIABLE
  | tID_DATASET
  ;
sign /* DOCUMENTATION:UNFOLD */
  : /* none */              { $$ = 1;  }
  | '-'                     { $$ = -1; }
  ;
field_conversion /* DOCUMENTATION:DIAGRAM */
  : /* none */
    {
      configurator -> resetConversionVariables();
    }
  | ':' sign width
    {
      configurator -> setConversionVarLengthvalue( $2, $3 );
    }
    field_prec
  ;
width /* DOCUMENTATION:DIAGRAM */
  : tINT_CONSTANT { $$ = $1; }
  ;
field_prec /* DOCUMENTATION:UNFOLD */
  : /* none */
    {
      configurator -> setConversionVarPrec( -1 );
    }
  | ':' precision
    {
      configurator -> setConversionVarPrec( $2 );
    }
    field_thousand_sep
  ;
precision /* DOCUMENTATION:DIAGRAM */
  : tINT_CONSTANT { $$ = $1; }
  ;
field_thousand_sep /* DOCUMENTATION:UNFOLD */
  : /* none */
    {
      configurator -> setConversionVarThousandsep( false );
    }
  | ':' tTSEP
    {
      configurator -> setConversionVarThousandsep( true );
    }
  ;

opt_scale_factor /* DOCUMENTATION:UNFOLD */
  : /* none */              { configurator -> setScaleFactor( 1.0 ); $$ = 1.0; }
  | scale_factor
  ;
scale_factor /* DOCUMENTATION:DIAGRAM */
  : '*' real_or_int_value   { configurator -> setScaleFactor( $2 ); $$ = $2 ; }
  | '*' temp_data_reference   { configurator -> setScaleFactor( $2 ); $$ = 1.0;
                               }
  | '/' real_or_int_value   { configurator -> setScaleDivisor( $2 ); $$ = 1./$2 ; }
  | '/' temp_data_reference   { configurator -> setScaleDivisor( $2 );
                                 $$ = 1.0;
                               }
  ;
real_or_int_value /* DOCUMENTATION:DIAGRAM */
  : real_value
  ;
temp_data_reference /* DOCUMENTATION:DIAGRAM */
  : data_variable
      {
        configurator->pushDataItem();
        bool clearIndexContainer = false;
        configurator -> uiNewXferDataItem( *($1), clearIndexContainer );
        delete $1;
      }
      ui_field_indizes opt_scale_factor_struct_data_reference_list
    {
        configurator -> uiSetDataReference();
        $$=configurator->getDataItem();
        configurator -> popDataItem();
    }
  ;
real_value /* DOCUMENTATION:DIAGRAM */
  : tREAL_CONSTANT          { $$ = $1; }
  | tREAL_INVALID /* DOCUMENTATION:HIDE (parser error) */
    { $$ = 0.0;
      ParserError( _("Invalid real value") );
    }
  | tINT_CONSTANT           { $$ = (double)($1); }
  | '-' real_value          { $$ = -$2; }
  | '(' real_value ')'           { $$ = $2; }
  | real_value '*' real_value    { $$ = ($1 * $3); }
  | real_value '/' real_value    { $$ = ($1 / $3); }
  | real_value '+' real_value    { $$ = ($1 + $3); }
  | real_value '-' real_value    { $$ = ($1 - $3); }
;
data_reference /* DOCUMENTATION:UNFOLD */
  : data_variable
      {
        configurator -> getDataReference( *($1) );
        delete $1;
      }
  | data_variable '.' struct_data_reference
      {
        configurator -> getDataReference( *($1)+"."+*($3) );
        delete $1;
        delete $3;
      }
  ;
struct_data_reference /* DOCUMENTATION:UNFOLD */
  : identifier
      {
        $$ = $1;
      }
  | struct_data_reference '.' identifier
      {
        $$ = new std::string( *($1) + "." + *($3) );
        delete $1;
        delete $3;
      }
  ;

/* --------------------------------------------------------------------------- */
/* Report Identifier                                                           */
/* --------------------------------------------------------------------------- */

report_identifier /* DOCUMENTATION:UNFOLD */
  : tID_STREAM
  | tID_REPORTSTREAM
  | tID_UNIPLOT
  | tID_LISTPLOT
  | tID_XRT3DPLOT /* DOCUMENTATION:HIDE (deprecated, use PLOT3D) */
  | tID_PLOT3D
  | tID_IMAGE
  | tID_LINEPLOT
  | tID_PLOT2D
  | tID_TEXTWINDOW
  | tSTD_WINDOW       { $$ = new std::string( "STD_WINDOW" ); }
  | tLOG_WINDOW       { $$ = new std::string( "LOG_WINDOW" ); }
  ;

report_save_identifier /* DOCUMENTATION:UNFOLD */
  : tID_STREAM
  | tID_REPORTSTREAM /* TODO-E */
  | tID_TEXTWINDOW /* TODO-E */
  | tSTD_WINDOW       { $$ = new std::string( "STD_WINDOW" ); }
  | tLOG_WINDOW       { $$ = new std::string( "LOG_WINDOW" ); }
  ;
/* --------------------------------------------------------------------------- */
/* Menu Global Functions                                                       */
/* --------------------------------------------------------------------------- */

menu_menu_declaration /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> createNewMenu( *$1 );
        delete $1;
        $$ = 0;
      }
  | tIDENTIFIER
      {
        configurator -> registerMenuId( (*$1) );
        delete $1;
      }
    menu_options
      {
        configurator -> attachMenu();
        $$ = 0;
      }
  | tID_MENU
      {
        configurator -> getExistingMenu( (*$1) );
        delete $1;
        $$ = 0;
      }
  ;
menu_submenu_declaration /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> createNewSubMenu( *$1 );
        delete $1;
        $$ = 0;
      }
  | tIDENTIFIER
      {
        configurator -> registerSubMenuId( (*$1) );
        delete $1;
//        $$ = 0;
      }
    menu_options { $$ = 0; }
  ;
menu_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' menu_option_list '}'
  ;
menu_option_list /* DOCUMENTATION:DIAGRAM */
  : menu_option
  | menu_option_list ',' menu_option
  ;
menu_option /* DOCUMENTATION:UNFOLD */
  : string_constant
      {
        configurator -> pulldownmenuSetLabel( (*$1) );
        delete $1;
      }
  | tHELPTEXT '=' string_constant /* DOCUMENTATION:HIDE (not implemented in intens4) */
      {
        configurator -> pulldownmenuSetHelptext( (*$3) );
        delete $3;
      }
  | tFORM '=' tID_FORM
      {
        configurator -> formAttachMenu( (*$3) );
        delete $3;
      }
  ;
menu_button_label /* DOCUMENTATION:DIAGRAM */
  : /* none */           { $$ = new std::string(); }
  | '=' string_constant  { $$ = $2; }
  ;

/* *************************************************************************** */
/* HELPFILE description                                                        */
/* *************************************************************************** */

helpfile_description /* DOCUMENTATION:DIAGRAM */
  : tHELPFILE
      {
        configurator->beginHelpfile();
      }
    help_file_list ';'
      {
        //cout<<"end Helpfile"<<endl;
      }
  ;
help_file_list /* DOCUMENTATION:UNFOLD */
  : help_file
  | help_file_list ',' help_file
  ;
help_file /* DOCUMENTATION:UNFOLD */
  : filename_string
      { configurator -> installHelpText( *($1) );
        delete $1;
      }
  | tOPEN_FILE filename_string
      { configurator -> installHelpHTML( *($2) );
        delete $2;
      }
    opt_helpfile_options
      { configurator -> installHelpCallback();
      }
  | tOPEN_URL document_string
      { configurator -> installHelpURL( *($2) );
        delete $2;
      }
    opt_helpfile_options
      { configurator -> installHelpCallback();
      }
  ;

filename_string /* DOCUMENTATION:DIAGRAM */
  : string_constant
  ;
document_string /* DOCUMENTATION:DIAGRAM */
  : string_constant
  ;

opt_helpfile_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | helpfile_options
  ;
helpfile_options /* DOCUMENTATION:DIAGRAM */
  : '(' help_option_list ')'
  ;
help_option_list /* DOCUMENTATION:UNFOLD */
  : help_option
  | help_option_list ',' help_option
  ;
help_option /* DOCUMENTATION:UNFOLD */
  : title_string
      {
        configurator -> setTitleHelpfile( *($1) );
        delete $1;
      }
  | tHELPKEY '(' helpfile_key_list ')'
  | tHELPKEY '=' helpfile_key
  | tHIDDEN
      { configurator -> hideHelpMenu();
      }
  ;
title_string /* DOCUMENTATION:DIAGRAM */
  : string_constant
  ;
helpfile_key_list /* DOCUMENTATION:UNFOLD */
  : helpfile_key
  | helpfile_key_list ',' helpfile_key
  ;
helpfile_key /* DOCUMENTATION:DIAGRAM */
  : string_constant
      { configurator -> installHelpKey( *($1) );
        delete $1;
      }
  | tIDENTIFIER
      { configurator -> installHelpKey( *($1) );
        delete $1;
      }
  ;

/* *************************************************************************** */
/* DATAPOOL description                                                        */
/* *************************************************************************** */

datapool_description /* DOCUMENTATION:DIAGRAM */
  : tDATAPOOL
      {
      if( !configurator ){
        configurator=App::Instance().getConfigurator();
        configurator -> start();
      }
        configurator->beginDatapool();
      }
    data_declarations tEND tDATAPOOL ';'
      {
        configurator->endDatapool();
        configurator->setDictInitialise();
      }
  ;
data_declarations /* DOCUMENTATION:UNFOLD */
  : data_V2_declaration_list
  ;

data_V2_declaration_list /* DOCUMENTATION:UNFOLD */
  : data_V2_declaration
  | data_V2_declaration_list data_V2_declaration
  ;
data_V2_declaration /* DOCUMENTATION:UNFOLD */
  : set_declaration ';'
  | color_declaration ';'
  | structure_declaration ';'
  | data_V2_item_declaration ';'
  ;
opt_data_V2_item_declaration_list /* DOCUMENTATION:UNFOLD */
  : /* none */
  | data_V2_item_declaration_list
  ;

data_V2_item_declaration_list /* DOCUMENTATION:UNFOLD */
  : data_V2_item_declaration ';'
  | data_V2_item_declaration_list  data_V2_item_declaration ';'
  ;
data_V2_item_declaration /* DOCUMENTATION:DIAGRAM */
  : data_V2_datatype  data_V2_variable_declarations
  | data_V2_structure data_V2_structure_declarations
  ;
data_V2_simple_variables_list /* DOCUMENTATION:UNFOLD */
  : data_V2_simple_variables
  | data_V2_simple_variables_list  data_V2_simple_variables
  ;
data_V2_simple_variables /* DOCUMENTATION:UNFOLD */
  : data_V2_datatype  { configurator -> setUserAttr( 0 ); } data_V2_variable_declaration_list ';'
  | data_V2_structure { configurator -> setUserAttr( 0 ); } data_V2_structure_declaration_list ';'
  ;

/* --------------------------------------------------------------------------- */
/* Set Declaration                                                             */
/* --------------------------------------------------------------------------- */

set_declaration /* DOCUMENTATION:DIAGRAM */
  : tSET data_V2_set_declaration_list
  ;

data_V2_set_declaration_list /* DOCUMENTATION:UNFOLD */
  : data_V2_set_declaration
  | data_V2_set_declaration_list ',' data_V2_set_declaration
  ;
data_V2_set_declaration /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER data_V2_set_optional_attributes
      { configurator -> newDataSet( *($1) );
        delete $1;
      }
    '(' data_set_item_list ')'
  ;
data_V2_set_optional_attributes /* DOCUMENTATION:UNFOLD */
  :   { configurator -> setDictInitialise(); }
    data_V2_set_optional_attr
  ;
data_V2_set_optional_attr /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' data_V2_set_attr_list '}'
  ;
data_V2_set_attr_list /* DOCUMENTATION:UNFOLD */
  : data_V2_set_attr
  | data_V2_set_attr_list ',' data_V2_set_attr
  ;
data_V2_set_attr /* DOCUMENTATION:DIAGRAM */
  : tGLOBAL
      { configurator -> setDictIsGlobal( true ); }
  | tINVALID '=' tNONE
      { configurator -> setDataSetNoInvalidEntry();  }
  ;

/* --------------------------------------------------------------------------- */
/* ColorSet Declaration                                                        */
/* --------------------------------------------------------------------------- */

color_declaration /* DOCUMENTATION:DIAGRAM */
  : tCOLOR data_colorset_declaration_list
  ;

data_colorset_declaration_list /* DOCUMENTATION:UNFOLD */
  : data_colorset_declaration
  | data_colorset_declaration_list ',' data_colorset_declaration
  ;
data_colorset_declaration /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      { configurator -> newColorSet( *($1) );
        delete $1;
      }
    '(' data_colorset_item_list ')'
  ;

/* --------------------------------------------------------------------------- */
/* Global Variable Attributes                                                  */
/* --------------------------------------------------------------------------- */

opt_data_V2_variable_attributes /* DOCUMENTATION:UNFOLD */
  : /* none */ { configurator -> setDictInitialise(); }
  | data_V2_variable_attributes
  ;
data_V2_variable_attributes /* DOCUMENTATION:DIAGRAM */
  : '{'        { configurator -> setDictInitialise(); }
    data_V2_variable_attr_list '}'
  ;
data_V2_variable_attr_list /* DOCUMENTATION:UNFOLD */
  : data_V2_variable_attr
  | data_V2_variable_attr_list ',' data_V2_variable_attr
  ;
data_V2_variable_attr /* DOCUMENTATION:UNFOLD */
  : tEDITABLE     { configurator -> setUserAttrEditable(); }
  | tOPTIONAL     { configurator -> setUserAttrEditable();
                    configurator -> setUserAttrOptional();
                  }
  | tLOCKABLE     { configurator -> setUserAttrLockable(); }
  | tSCALAR       { configurator -> setUserAttrScalar(); }
  | tMATRIX       { configurator -> setUserAttrMatrix(); }
  | tCELL         { configurator -> setUserAttrCell(); }
  | tGLOBAL       { configurator -> setDictIsGlobal( true ); }
  | tOMIT_TTRAIL  { configurator -> setDictWithoutTTrail(); }
  | tNO_DEPENDENCIES
      {
        configurator -> setUserAttrNoDependencies();
      }

  | tCLASSNAME '=' string_constant
                  { configurator -> setUserAttrClassname( *($3) );
                    delete $3;
                  }
  | tMAX_OCCURS '=' tINT_CONSTANT /* DOCUMENTATION:HIDE BEGIN (plugin interface) */
                  { configurator -> setUserAttrMaxOccurs( $3 );
                  } /* DOCUMENTATION:HIDE END */
/*  | tTRANSIENT    { configurator -> setUserAttrDbTransient( ); } */
  ;

/* --------------------------------------------------------------------------- */
/* Variable Declarations                                                       */
/* --------------------------------------------------------------------------- */

data_V2_datatype /* DOCUMENTATION:UNFOLD */
  : tREAL        { configurator -> setDataType( DataDictionary::type_Real );    }
  | tINTEGER     { configurator -> setDataType( DataDictionary::type_Integer ); }
  | tSTRING      { configurator -> setDataType( DataDictionary::type_String );  }
  | tCOMPLEX     { configurator -> setDataType( DataDictionary::type_Complex ); }
  | tCDATA       { configurator -> setDataType( DataDictionary::type_CharData );  }
  ;
data_V2_variable_declarations /* DOCUMENTATION:UNFOLD */
  :   { configurator -> newUserAttr(); }
    opt_data_V2_variable_attributes data_V2_variable_declaration_list
      { configurator -> deleteUserAttr(); }
  ;
data_V2_variable_declaration_list /* DOCUMENTATION:UNFOLD */
  : data_V2_variable_declaration
  | data_V2_variable_declaration_list ',' data_V2_variable_declaration
  ;
data_V2_variable_declaration /* DOCUMENTATION:DIAGRAM */
  : identifier  /* TODO-H: data_identifier? */
      { configurator -> addDataVariable( (*$1) );
        delete $1;
      }
    opt_data_V2_dimension opt_data_V2_item_options
  ;
opt_data_V2_dimension /* DOCUMENTATION:UNFOLD */
  : /* none */
  | data_V2_dimension
  ;
data_V2_dimension /* DOCUMENTATION:DIAGRAM */
  : '[' { configurator -> setIndexNum( 0 ); }
     data_V2_dimension_size_list ']'
  ;
data_V2_dimension_size_list /* DOCUMENTATION:UNFOLD */
  : data_V2_dimension_size
  | data_V2_dimension_size_list ',' data_V2_dimension_size
  ;
data_V2_dimension_size /* DOCUMENTATION:UNFOLD */
  : tINT_CONSTANT
      { configurator -> setDictItemDefaultDimension( $1 ); }
  ;

/* --------------------------------------------------------------------------- */
/* Local Variable Attributes                                                   */
/* --------------------------------------------------------------------------- */

opt_data_V2_item_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | data_V2_item_options
  ;
data_V2_item_options /* DOCUMENTATION:DIAGRAM */
  : '{' data_V2_item_option_list '}'
  ;
data_V2_item_option_list /* DOCUMENTATION:UNFOLD */
  : data_V2_item_option
  | data_V2_item_option_list ',' data_V2_item_option
  ;
data_V2_item_option /* DOCUMENTATION:UNFOLD */
  : tSET '=' tID_DATASET
      { configurator -> setDataSetName( *($3) );
        delete $3;
      }
  | tINDEXED_SET '=' tID_DATASET
      { configurator -> setDataSetName( *($3), true );
        delete $3;
      }
  | tCOLOR '=' tID_COLORSET
      { configurator -> setColorSetName( *($3) );
        delete $3;
      }
  | tCOLOR
      { configurator -> setColorSetName("");  // used as color picker
      }
  | tNO_COLORBIT  { configurator -> setNoColorBit(); }
  | tFUNC '=' job_function_pointer
      { configurator -> setDictItemFunction(); }
  | tFOCUS_FUNC '=' job_function_pointer
      { configurator -> setDictItemFocusFunction(); }
  | tLABEL '=' string_constant
      { configurator -> setDictItemLabel( *($3) );
        delete $3;
      }
  | tUNIT '=' string_constant
      { configurator -> setDictItemUnits( *($3) );
        delete $3;
      }
  | tPATTERN '=' string_constant
      { configurator -> setDictItemPattern( *($3) );
        delete $3;
      }
  | tHELPTEXT '=' string_constant
      { configurator -> setDictItemHelpText( *($3) );
        delete $3;
      }
  | tBUTTON
      { configurator -> setDictItemButton(); }
  | tSLIDER
      { configurator -> setDictItemSlider(); }
  | tPROGRESS
      { configurator -> setDictItemProgress(); }
  | tRANGE '(' real_value ',' real_value ')'
      {
        configurator -> setDictItemRange( $3, $5 );
      }
  | tSTEP '(' real_value ')'
      {
        configurator -> setDictItemStep( $3 );
      }
  | tTOGGLE
      { configurator -> setDictItemToggle(); }
  | tRADIO
      { configurator -> setDictItemRadio(); }
  | tCOMBOBOX /* DOCUMENTATION:HIDE (an item with a SET is always shown as a combobox in intens4) */
      { configurator -> setDictItemCombobox(); }
  | tLABEL
      { configurator -> setDictItemLabelType(); }
  | tCLASSNAME '=' string_constant
      { configurator -> setDictItemClassName( *($3) ) ;
        delete $3;
      }
  | tSCALAR
      { configurator -> setDictItemScalar(); }
  | tMATRIX
      { configurator -> setDictItemMatrix(); }
  | tCELL
      { configurator -> setDictItemCell(); }
  | tHIDDEN
      { configurator -> setDictItemHidden(); }
  | tPLACEHOLDER '=' string_constant
                  { configurator -> setDictItemPlaceholder( *($3) );
                    delete $3;
                  }
  | data_V2_item_more_option /* split data_V2_item_options diagram */
  ;
data_V2_item_more_option /* DOCUMENTATION:DIAGRAM */
  : tPERSISTENT
      { configurator -> setDictItemPersistent(); }
  | tTRANSIENT    { configurator -> setDictItemDbTransient( ); }
  | tDBATTR      '=' string_constant
                  { configurator -> setDictItemDbAttr( *($3) );
                    delete $3; }
  | tDBUNIT      '=' string_constant
                  { configurator -> setDictItemDbUnit( *($3) );
                    delete $3; }
  | tTAG '=' data_tags
  | tFOLDER '=' tNONE
      { configurator -> unsetDictItemFolder(); }
  | tSTRING_DATE     { configurator -> setDictItemDate(UserAttr::string_kind_date); }
  | tSTRING_TIME     { configurator -> setDictItemDate(UserAttr::string_kind_time); }
  | tSTRING_DATETIME { configurator -> setDictItemDate(UserAttr::string_kind_datetime); }
  | tPASSWORD        { configurator -> setDictItemDate(UserAttr::string_kind_password); }
  | tWHEEL_EVENT     { configurator -> setDictItemWheelEvent(); }
  ;
data_tags /* DOCUMENTATION:DIAGRAM */
  : data_tag
  | '(' data_tag_list ')'
  ;
data_tag_list /* DOCUMENTATION:UNFOLD */
  : data_tag
  | data_tag_list ',' data_tag
  ;
data_tag /* DOCUMENTATION:DIAGRAM */
  : tIDENTIFIER
    { configurator -> setDictItemTag( *($1), true );
      delete $1;
    }
  | tID_TAG
    { configurator -> setDictItemTag( *($1), false );
      delete $1;
    }
  ;

/* --------------------------------------------------------------------------- */
/* Structure Variable Declarations                                             */
/* --------------------------------------------------------------------------- */

data_V2_structure /* DOCUMENTATION:UNFOLD */
  : tID_DATASTRUCTURE
    { configurator -> setDictStruct( *($1) );
      delete $1;
    }
  ;
data_V2_structure_declarations /* DOCUMENTATION:SYNONYM data_V2_variable_declarations */
  : { configurator -> newUserAttr(); }
      opt_data_V2_variable_attributes data_V2_structure_declaration_list
    { configurator -> deleteUserAttr(); }
  ;
data_V2_structure_declaration_list /* DOCUMENTATION:SYNONYM data_V2_variable_declaration_list */
  : data_V2_structure_declaration
  | data_V2_structure_declaration_list ',' data_V2_structure_declaration
  ;
data_V2_structure_declaration /* DOCUMENTATION:UNFOLD */
  : identifier  /* TODO-H: data_identifier? */
    { configurator -> addStructureVariable( *($1) );
      delete $1;
    }
    opt_data_V2_dimension opt_data_V2_item_options
  ;

/* --------------------------------------------------------------------------- */
/* Structure Definition                                                        */
/* --------------------------------------------------------------------------- */

structure_declaration /* DOCUMENTATION:DIAGRAM */
  : tSTRUCT data_V2_structure_definition
  ;

data_V2_structure_definition /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
       { configurator -> addStructureDefinition( *($1) );
        delete $1;
       }
    data_V2_structure_copies
       { configurator -> resetDictItem();
       }
    '{' opt_data_V2_item_declaration_list '}'
       { configurator -> eraseParentName();
       }
  ;
data_V2_structure_copies /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ':' data_V2_structure_copy_list
  ;
data_V2_structure_copy_list /* DOCUMENTATION:UNFOLD */
  : data_V2_structure_copy
  | data_V2_structure_copy_list ',' data_V2_structure_copy
  ;
data_V2_structure_copy /* DOCUMENTATION:UNFOLD */
  : tID_DATASTRUCTURE
      { configurator -> copyStructureDefinition( *($1) );
        delete $1;
      }
  ;
/* --------------------------------------------------------------------------- */
/* Datapool Set Items                                                          */
/* --------------------------------------------------------------------------- */

data_set_item_list /* DOCUMENTATION:UNFOLD */
  : data_set_item
  | data_set_item_list ',' data_set_item
  ;
data_set_item /* DOCUMENTATION:DIAGRAM */
  : label_string
      { configurator -> setDataSetItemValues( *($1) );
        delete $1;
      }
  | label_string '=' real_or_int_value
      { configurator -> setDataSetItemValues( *($1), $3 );
        delete $1;
      }
  | label_string '=' string_constant
      { configurator -> setDataSetItemValues( *($1), *($3) );
        delete $1;
        delete $3;
      }
  ;
label_string /* DOCUMENTATION:DIAGRAM */
  : string_constant
  ;

/* --------------------------------------------------------------------------- */
/* Datapool ColorSet Items                                                     */
/* --------------------------------------------------------------------------- */

data_colorset_item_list /* DOCUMENTATION:UNFOLD */
  : data_colorset_item
  | data_colorset_item_list ',' data_colorset_item
  ;
data_colorset_item /* DOCUMENTATION:DIAGRAM */
  : data_colorset_invalid    data_colorset_colors
  | data_colorset_value      data_colorset_colors
  | data_colorset_range      data_colorset_colors
  | data_colorset_item_else  data_colorset_colors
  ;
data_colorset_invalid /* DOCUMENTATION:UNFOLD */
  : tINVALID
      { configurator -> setColorSetItemInvalid();
      }
  ;
data_colorset_item_else /* DOCUMENTATION:UNFOLD */
  : tELSE
      { configurator -> setColorSetItemAlways();
      }
  ;
data_colorset_range /* DOCUMENTATION:DIAGRAM */
  : tRANGE '(' data_colorset_lowerbound ',' data_colorset_upperbound ')'
  ;
data_colorset_value_less_or_greater /* DOCUMENTATION:UNFOLD */
  : /* none */   { $$ = 0; }
  | '<'          { $$ = -1; }
  | '>'          { $$ = 1; }
  ;
data_colorset_value_less /* DOCUMENTATION:UNFOLD */
  : /* none */   { $$ = 0; }
  | '<'          { $$ = -1; }
  ;
data_colorset_value_greater /* DOCUMENTATION:UNFOLD */
  : /* none */   { $$ = 0; }
  | '>'          { $$ = 1; }
  ;
data_colorset_value /* DOCUMENTATION:DIAGRAM */
  : data_colorset_value_less_or_greater real_or_int_value
      { configurator -> setColorSetItemLowerValue( $1, $2 );
      }
  | data_colorset_value_less_or_greater string_constant
      { configurator -> setColorSetItemLowerValue( $1, (*$2) );
        delete $2;
      }
  | data_colorset_value_less_or_greater range_data_reference
      { configurator -> setColorSetItemLowerXfer( $1 );
      }
  ;
data_colorset_lowerbound /* DOCUMENTATION:UNFOLD */
  : data_colorset_value_greater real_value
      { configurator -> setColorSetItemLowerValue( $1, $2 );
      }
  | data_colorset_value_greater string_constant
      { configurator -> setColorSetItemLowerValue( $1, (*$2) );
        delete $2;
      }
  | data_colorset_value_greater range_data_reference
      { configurator -> setColorSetItemLowerXfer( $1 );
      }
  ;
data_colorset_upperbound /* DOCUMENTATION:UNFOLD */
  : data_colorset_value_less real_value
      { configurator -> setColorSetItemUpperValue( $1, $2 );
      }
  | data_colorset_value_less string_constant
      { configurator -> setColorSetItemUpperValue( $1, (*$2) );
        delete $2;
      }
  | data_colorset_value_less range_data_reference
      { configurator -> setColorSetItemUpperXfer( $1 );
      }
  ;
data_colorset_colors /* DOCUMENTATION:UNFOLD */
  : '=' '(' bg_color_string ',' fg_color_string ')'
      { configurator -> setColorSetItemColors( (*$3), (*$5) );
        delete $3;
        delete $5;
      }
  | '=' '(' st_data_reference ',' st_data_reference ')'
      { configurator -> setColorSetItemColorsXfer( $3, $5 );
      }
  ;
bg_color_string /* DOCUMENTATION:DIAGRAM */
  : tSTRING_CONSTANT
  ;
fg_color_string /* DOCUMENTATION:DIAGRAM */
  : tSTRING_CONSTANT
  ;

/* *************************************************************************** */
/* STREAMER description                                                        */
/* *************************************************************************** */

streamer_description /* DOCUMENTATION:DIAGRAM */
  : tSTREAMER
      {
        configurator->beginStreamer();
      }
    st_declaration_list
    tEND tSTREAMER ';'
  ;
st_declaration_list /* DOCUMENTATION:UNFOLD */
  :  st_declaration
  |  st_declaration_list st_declaration
  ;
st_declaration /* DOCUMENTATION:DIAGRAM */
  : tIDENTIFIER st_options
      {
        configurator -> beginStream( *($1) );
        configurator -> newStream( *($1) );
        delete $1;
      }
    '(' st_format_command_list ')' ';'
      {
        configurator -> resetStream();
        configurator -> endStream();
      }

  | tIDENTIFIER '{' tXML st_xml_options '}'
      {
        configurator -> beginStream( *($1) );
        configurator -> setStreamXMLFlag();
        configurator -> newStream( *($1) );
        delete $1;
      }
    '(' st_xml_format_command ')' ';'
      {
        configurator -> resetStream();
        configurator -> endStream();
      }

  | tIDENTIFIER '{' tJSON st_json_options '}'
      {
        configurator -> beginStream( *($1) );
        configurator -> setStreamJSONFlag();
        configurator -> newStream( *($1) );
        delete $1;
      }
    '(' st_json_format_command ')' ';'
      {
        configurator -> resetStream();
        configurator -> endStream();
      }
  ;

st_options /* DOCUMENTATION:UNFOLD */
  :/* none */
  | '{' st_option_list '}'
  ;
st_option_list /* DOCUMENTATION:DIAGRAM */
  : st_option
  | st_option_list ',' st_option
  ;
st_option /* DOCUMENTATION:UNFOLD */
  : tLATEX
    {
      configurator -> setStreamLatexFlag();
    }
  | tURL
    {
      configurator -> setStreamUrlFlag();
    }
  | tDELIMITER '=' char_const
    {
      configurator -> setStreamDelimiter( $3 );
    }
  | tLOCALE
    {
      configurator -> setStreamLocaleFlag();
    }
  | tPROCESS '=' string_constant
    {
      configurator -> setStreamerProcess( (*$3) );
      delete $3;
    }
  | tAPPEND
    {
      configurator -> setStreamAppendOption();
    }
  | tNO_GZ
    {
      configurator -> setStreamNoGzOption();
    }
  | tFILE
    {
      configurator -> setStreamFileFlag();
    }
  ;

st_json_options /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | ',' st_json_option_list
  ;
st_json_option_list /* DOCUMENTATION:UNFOLD */
  : st_json_option
  | st_json_option_list ',' st_json_option
  ;
st_json_option /* DOCUMENTATION:UNFOLD */
  : tPROCESS '=' string_constant
    {
      configurator -> setStreamerProcess( (*$3) );
      delete $3;
    }
  | tINDENT '=' tINT_CONSTANT
    {
      configurator -> setStreamIndent( $3 );
    }
  | st_opt_flag tHIDDEN
    {
      configurator -> setStreamHidden( !($1) );
    }
  | st_opt_flag tTRANSIENT
    {
      configurator -> setStreamTransient( !($1) );
    }
  ;

st_data_reference /* DOCUMENTATION:DIAGRAM */
  : data_variable
      {
        configurator -> stNewXferDataItem( *($1) );
        delete $1;
      }
    st_field_indizes
    opt_st_struct_data_reference_list
      {
        configurator -> stSetDataReference();
        $$=configurator->getDataItem();
      }
  ;
st_advanced_data_reference /* DOCUMENTATION:DIAGRAM */
  : data_variable
      {
        configurator -> stNewXferDataItem( *($1) );
        delete $1;
      }
    st_field_indizes
    opt_st_advanced_struct_data_reference_list
      {
        configurator -> stSetDataReference();
      }
  | tVAR '(' st_data_reference  ')'
    { configurator -> addVarParameter(); }
    st_field_indizes
    opt_st_struct_data_reference_list
  ;

range_data_reference /* DOCUMENTATION:DIAGRAM */
  : st_data_reference
       { configurator -> pushDataItem();
       }
    opt_scale_factor
  ;

opt_st_struct_data_reference_list /* DOCUMENTATION:UNFOLD */
  : /* none */
  | st_struct_data_reference_list
  ;
st_struct_data_reference_list /* DOCUMENTATION:UNFOLD */
  : st_struct_data_reference
  | st_struct_data_reference_list st_struct_data_reference
  ;
st_struct_data_reference /* DOCUMENTATION:UNFOLD */
  : '.' identifier
    {
      configurator -> getNextDataReference( *($2) );
      delete $2;
    }
    st_field_indizes
  ;
opt_st_advanced_struct_data_reference_list /* DOCUMENTATION:UNFOLD */
  : /* none */
  | st_advanced_struct_data_reference_list
  ;
st_advanced_struct_data_reference_list /* DOCUMENTATION:UNFOLD */
  : st_advanced_struct_data_reference
  | st_advanced_struct_data_reference_list st_advanced_struct_data_reference
  ;
st_advanced_struct_data_reference /* DOCUMENTATION:UNFOLD */
  : '.' identifier
    {
      configurator -> getNextDataReference( *($2) );
      delete $2;
    }
    st_field_indizes
  | '.' tVAR '('
    {
      configurator -> stSetVarDataReference();
    }
    st_data_reference ')'
    {
       configurator -> addVarParameter();
    }
    st_field_indizes
  ;
st_field_indizes /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | '[' st_field_index_list ']'
  ;
st_field_index_list /* DOCUMENTATION:UNFOLD */
  : st_field_index
  | st_field_index_list ',' st_field_index
  ;
st_field_index /* DOCUMENTATION:UNFOLD */
  : /* empty */
    { configurator -> newDataItemIndex( "#" );
    }
  | tINT_CONSTANT
    { configurator -> newDataItemIndex( $1 );
    }
  | index_identifier
    { configurator -> newDataItemIndex( *($1) );
      delete $1;
    }
  | tID_INDEX
    { configurator -> addIndexContainer( *($1) );
      delete $1;
    }
  ;
index_identifier /* DOCUMENTATION:UNFOLD */
  : '#' { $$ = new std::string("#"); }
  | '#' tIDENTIFIER { $$= new std::string( std::string("#")+*$2 ); delete $2; }
  | '#' tINT_CONSTANT { $$=new std::string( App::Instance().getFlexer()->YYText() ); }
;
st_format_command_list /* DOCUMENTATION:UNFOLD */
  : st_format_command
  | st_format_command_list ',' st_format_command
  ;
st_format_command /* DOCUMENTATION:DIAGRAM */
  : st_opt_flag st_advanced_data_reference opt_scale_factor field_conversion
    { configurator -> addParameter( $1 );
    }
  | tMATRIX st_matrix_option st_opt_flag st_data_reference opt_scale_factor field_conversion
    { configurator -> addMatrixParameter( $3, $2 );
    }
  | tDATASET_TEXT '(' st_data_reference ')'
    { configurator -> addDatasetParameter();
    } st_field_length
  | tSTRING_DATE '(' st_data_reference st_field_length ')'
    {   configurator -> addStringDateParameter(UserAttr::string_kind_date, $4);
    }
  | tSTRING_TIME '(' st_data_reference st_field_length ')'
    {   configurator -> addStringDateParameter(UserAttr::string_kind_time, $4);
    }
  | tSTRING_DATETIME '(' st_data_reference st_field_length ')'
    {   configurator -> addStringDateParameter(UserAttr::string_kind_datetime, $4);
    }
  | tSTRING_VALUE '(' st_data_reference st_field_length ')'
    { configurator -> addStringDateParameter(UserAttr::string_kind_value, $4);
    }
  | '#' st_field_length
    { configurator -> addIndexParameter( "#", $2 );
    }
  | '#' tIDENTIFIER st_field_length
    { configurator -> addIndexParameter( std::string("#")+*$2, $3 );
      delete $2;
    }
  | string_constant st_field_length
    { configurator -> addToken( *$1, $2 );
      delete $1;
    }
  | tPLOTGROUP '=' id_or_string_constant
    { configurator -> addPlotGroupParameter( *$3 );
      delete $3;
    }
    st_plotgroup_options

  | tXMLGROUP '=' tID_PLUGIN /* DOCUMENTATION:HIDE (not implemented in intens4) */
    { configurator -> addXMLPlotGroupStreamParameter( *($3) );
      delete $3;
    }

  | st_serialize_formId
    { configurator -> addSerializedFormParameter( "" );
    }

  | tSKIP ':' tINT_CONSTANT
    { configurator -> addToken( "", $3 );
    }
  | tEOLN
    { configurator -> addToken( "\n", 1 );
    }
  | tID_INDEX
    {
      configurator -> addGuiIndexParameter( *($1) );
      delete $1;
    }
  | '('
    { configurator -> stBeginGroup();
    }
    st_format_command_list ')'
      { configurator -> stEndGroup();
      }
  ;

st_serialize_formId /* DOCUMENTATION:UNFOLD */
  : tMAIN { configurator -> addSerializeFormId( "<MAINFORM>" );}
  | tID_FORM { configurator -> addSerializeFormId( *$1 ); }
;

st_matrix_option /* DOCUMENTATION:DIAGRAM */
  : /* none */ { $$ = '\0'; }
  | '{' tDELIMITER '=' char_const '}'  { $$ = $4;}
  ;
st_xml_options /* DOCUMENTATION:DIAGRAM */
  :/* none */
  | '{' st_xml_option_list '}'
  ;
st_xml_option_list /* DOCUMENTATION:UNFOLD */
  : st_xml_option
  | st_xml_option_list ',' st_xml_option
  ;
st_xml_option /* DOCUMENTATION:UNFOLD */
  : tATTRS '(' st_xml_attrs ')'
  | tSCHEMA '=' string_constant
    {
      configurator -> setXMLSchema( *$3 );
      delete $3;
    }
  | tNAMESPACE '=' string_constant
    {
      configurator -> setXMLNamespace( *$3 );
      delete $3;
    }
  | tVERSION '=' string_constant
    {
      configurator -> setXMLVersion( *$3 );
      delete $3;
    }
  | tSTYLESHEET '=' string_constant
    {
      configurator -> setXMLStylesheet( *$3 );
      delete $3;
    }
  ;
st_xml_attrs /* DOCUMENTATION:UNFOLD */
  : /* none */
  | st_xml_attr_list
  ;
st_xml_attr_list /* DOCUMENTATION:UNFOLD */
  : st_xml_attr
  | st_xml_attr_list ',' st_xml_attr
  ;
st_xml_attr /* DOCUMENTATION:UNFOLD */
  : tUNIT { configurator -> addXMLAttr( "unit" ); }
  | tLABEL { configurator -> addXMLAttr( "label" ); }
  | tHELPTEXT { configurator -> addXMLAttr( "helptext" ); }
  ;
st_xml_format_command /* DOCUMENTATION:DIAGRAM */
  : st_data_reference {
      configurator -> addXMLParameter();
    }
  | tDATAPOOL {
      configurator -> resetDataItem();
      configurator -> withAllCycles();
      configurator -> addXMLParameter();
    }
  | tCYCLE {
      configurator -> resetDataItem();
      configurator -> addXMLParameter();
    }
  ;

st_json_format_command /* DOCUMENTATION:DIAGRAM */
  : st_json_data_reference_list
  | tDATAPOOL {
      configurator -> resetDataItem();
      configurator -> withAllCycles();
      configurator -> addJSONParameter();
    }
  | tCYCLE {
      configurator -> resetDataItem();
      configurator -> addJSONParameter();
    }
  ;

st_json_data_reference_list /* DOCUMENTATION:UNFOLD */
  : st_json_data_reference
  | st_json_data_reference_list ',' st_json_data_reference
  ;

st_json_data_reference /* DOCUMENTATION:UNFOLD */
  : st_data_reference {
      configurator -> addJSONParameter();
    }
  ;

st_plotgroup_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' st_plotgroup_option_list '}'
  ;
st_plotgroup_option_list /* DOCUMENTATION:UNFOLD */
  : st_plotgroup_option
  | st_plotgroup_option_list ',' st_plotgroup_option
  ;
st_plotgroup_option /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | tRANGE '(' real_value ',' real_value ')'
      { configurator -> setXRangeReal( $3, $5 );
      }
  | tRANGE '(' range_data_reference ',' range_data_reference ')'
      { configurator -> setXRangeDataRef();
      }
  | tTRANSPARENT '=' bool_constant
      {
      configurator -> setPlotGroupParameterTransparentBackground( $3 );
///        configurator -> xrtgraphSetPrintStyle( $3 );
      }
  ;
st_opt_flag /* DOCUMENTATION:UNFOLD */
  : /* none */   { $$ = false; }
  | tNOT         { $$ = true; }
  ;
st_field_length /* DOCUMENTATION:DIAGRAM */
  : /* none */              { $$ = 0; }
  | ':' sign width  { $$ = $2 * $3; }
  ;

/* *************************************************************************** */
/* UI_MANAGER description                                                      */
/* *************************************************************************** */

ui_manager_description /* DOCUMENTATION:DIAGRAM */
  : tUI_MANAGER
      {
        configurator->beginUiManager();
      }
    ui_declaration_list
    tEND tUI_MANAGER ';'
      {
        //cout<<"end UI_Manager"<<endl;
      }
  ;
ui_declaration_list /* DOCUMENTATION:UNFOLD */
  : ui_declaration
  | ui_declaration_list ui_declaration
  ;
ui_declaration /* DOCUMENTATION:UNFOLD */
  : ui_guielement_declaration ';'
      {
        configurator -> dispatchPendingEvents();
      }
  ;
ui_guielement_declaration /* DOCUMENTATION:UNFOLD */
  : tFIELDGROUP ui_fieldgroup_list
  | tTABLE ui_table_list
  | tLIST ui_list_list
  | tINDEX ui_index_list
  | tLISTPLOT ui_listplot_list
  | tUNIPLOT ui_uniplot_list
  | tXRT3DPLOT ui_plot3d_list /* DOCUMENTATION:HIDE (deprecated, use PLOT3D) */
  | tPLOT3D ui_plot3d_list
  | tXRTGRAPH ui_xrtgraph_list /* DOCUMENTATION:HIDE (deprecated, use PLOT2D) */
  | tPLOT2D ui_plot2d_list
  | tNAVIGATOR ui_navigator_type_option ui_navigator_list
  | tTEXT_WINDOW ui_text_window_list
  | tSTD_WINDOW ui_std_window_options
  | tLOG_WINDOW ui_log_window_options
  | tFOLDER ui_folder_list
  | tFORM ui_form_list
  | tMENU ui_menu_list
  | tPSPLOT ui_psplot_list
  | tIMAGE ui_image_list
  | tLINEPLOT ui_lineplot_list
  | tTHERMO ui_thermo_list
  | tTIMETABLE ui_timetable_list
  | tPLUGIN ui_plugin_list
  ;

/* --------------------------------------------------------------------------- */
/* UI STD/LOG_WINDOW                                                           */
/* --------------------------------------------------------------------------- */

ui_std_window_options /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | '{' tMENU '=' bool_constant '}'
   {
     configurator -> configStandardWindow( $4 );
   }
  ;

ui_log_window_options /* DOCUMENTATION:SYNONYM ui_std_window_options */
  : /* none */
  | '{' tMENU '=' bool_constant '}'
   {
     configurator -> configLogWindow( $4 );
   }
  ;

/* --------------------------------------------------------------------------- */
/* UI Datafield Attribute Functions                                            */
/* --------------------------------------------------------------------------- */

ui_field_attributes /* DOCUMENTATION:DIAGRAM */
  : opt_scale_factor ui_field_length ui_field_alignment ui_field_thousand_sep
    {
      configurator -> setFieldAttributes( $2, $3, $4 );
    }
  ;
ui_field_additional_attributes /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | '{' ui_field_additional_attr_list '}'
  ;
ui_field_additional_attr_list /* DOCUMENTATION:UNFOLD */
  : ui_field_additional_attr
  | ui_field_additional_attr_list ',' ui_field_additional_attr
  ;
ui_field_additional_attr /* DOCUMENTATION:UNFOLD */
  : string_constant
    {
      configurator -> setDataFieldLabel( *($1) );
    }
  | tPIXMAP '=' id_or_string_constant
    {
      configurator -> setDataFieldPixmap( *($3) );
    }
  | tSIZE '(' tINT_CONSTANT ',' tINT_CONSTANT ')'
    {
      configurator -> setDataFieldSetGuiElementSize( $3, $5 );
    }
  | tSCROLLBARS
    {
      configurator -> setDataFieldScrollbar();
    }
  | tAUTO_SCROLL
    {
      configurator -> setDataFieldAutoScroll();
    }
  | tEXPAND
      {
        configurator -> setDataFieldExpandable();
      }
  | tCOLSPAN '=' tINT_CONSTANT
    {
      configurator -> setFieldColSpan( $3, false );
    }
  | tROWSPAN '=' tINT_CONSTANT
    {
      configurator -> setFieldRowSpan( $3, false );
    }
  | tVERTICAL
      {
       configurator -> setFieldOrientation( GuiElement::orient_Vertical, false );
      }
  |  tROTATE_180
      {
       configurator -> setFieldRotation(180, false);
      }
  | tHELPTEXT '=' tID_STREAM
      {
       configurator -> setFieldHelptextStream(*($3), false);
      }
  | tCLASS '=' string_constant
    {
      configurator -> setFieldClass( *($3), false) ;
      delete $3;
    }
;
ui_set_field_attributes /* DOCUMENTATION:EMPTY */
  : /* none */
    {
      configurator -> setDataFieldAttributes();
    }
  ;
ui_field_length /* DOCUMENTATION:DIAGRAM */
  :  /* none */
    {
      configurator -> setFieldPrecision( -1 );
      $$ = GuiDataField::defaultFieldLength();
    }
  | ':' sign tINT_CONSTANT ui_field_precision
    {
      configurator -> setFieldPrecision( $4 );
      $$ = $2*$3;
    }
  ;
ui_field_thousand_sep /* DOCUMENTATION:UNFOLD */
  : /* none */        { $$ = false; }
  | ':' tTSEP         { $$ = true; }
  ;
ui_field_precision /* DOCUMENTATION:DIAGRAM */
  : /* none */        { $$ = -1; }
  | ':' tINT_CONSTANT { $$ = $2; }
  ;
ui_field_alignment /* DOCUMENTATION:DIAGRAM */
  : /* none */        { $$ = GuiElement::align_Default; }
  | '>'               { $$ = GuiElement::align_Right; }
  | '<'               { $$ = GuiElement::align_Left; }
  | '|'               { $$ = GuiElement::align_Center; }
  | '^'               { $$ = GuiElement::align_Top; }
  | '_'               { $$ = GuiElement::align_Bottom; }
  ;

/* --------------------------------------------------------------------------- */
/* UI Datafield Data Definition                                                */
/* --------------------------------------------------------------------------- */

ui_field_data_reference /* DOCUMENTATION:DIAGRAM */
  : ui_xfer { XferDataItem *x=$1; }
  | tREAL '(' tID_DATAVARIABLE
      {
        configurator -> uiNewXferDataItemComplexReal( *($3) );
        delete $3;
      }
    ui_field_indizes opt_ui_field_struct_data_reference_list ')'
      {
        configurator -> uiSetDataReference();
      }
  | tIMAG '(' tID_DATAVARIABLE
      {
        configurator -> uiNewXferDataItemComplexImag( *($3) );
        delete $3;
      }
    ui_field_indizes opt_ui_field_struct_data_reference_list ')'
      {
        configurator -> uiSetDataReference();
      }
  | tABS '(' tID_DATAVARIABLE
      {
        configurator -> uiNewXferDataItemComplexAbs( *($3) );
        delete $3;
      }
    ui_field_indizes opt_ui_field_struct_data_reference_list ')'
      {
        configurator -> uiSetDataReference();
      }
  | tARG '(' tID_DATAVARIABLE
      {
        configurator -> uiNewXferDataItemComplexArg( *($3) );
        delete $3;
      }
    ui_field_indizes opt_ui_field_struct_data_reference_list ')'
      {
        configurator -> uiSetDataReference();
      }
  | tFILENAME '(' tID_FILESTREAM ')'
      {
        configurator -> uiNewXferDataItemFilestream( *($3) );
        delete $3;
      }
  | tID_FILESTREAM
      {
        configurator -> uiNewXferDataItemFilestream( *($1) );
        delete $1;
      }
  ;

ui_xfer /* DOCUMENTATION:DIAGRAM */
  :   data_variable
      {
        configurator -> uiNewXferDataItem( *($1) );
        delete $1;
      }
    ui_field_indizes opt_ui_field_struct_data_reference_list
      {
        configurator -> uiSetDataReference();
        $$ = configurator->getDataItem();
      }
;
opt_scale_factor_struct_data_reference_list /* DOCUMENTATION:UNFOLD */
  : /* none */
  | scale_factor_struct_data_reference_list
  ;
scale_factor_struct_data_reference_list /* DOCUMENTATION:UNFOLD */
  : scale_factor_struct_data_reference
  | scale_factor_struct_data_reference_list scale_factor_struct_data_reference
  ;
scale_factor_struct_data_reference /* DOCUMENTATION:UNFOLD */
  : '.' identifier
      {
        configurator -> getDataReferenceStruct( *($2) );
      }
      ui_field_indizes
  ;

opt_ui_field_struct_data_reference_list /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ui_field_struct_data_reference_list
  ;
ui_field_struct_data_reference_list /* DOCUMENTATION:UNFOLD */
  : ui_field_struct_data_reference
  | ui_field_struct_data_reference_list ui_field_struct_data_reference
  ;
ui_field_struct_data_reference /* DOCUMENTATION:UNFOLD */
  : '.' identifier
      {
        configurator -> getDataReferenceStruct( *($2) );
      }
    ui_field_indizes
  ;
ui_field_indizes /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | '[' ui_field_index_list ']'
  ;
ui_field_index_list /* DOCUMENTATION:UNFOLD */
  : ui_field_index
  | ui_field_index_list ',' ui_field_index
  ;
ui_field_index /* DOCUMENTATION:DIAGRAM */
  :   {
        configurator -> newDataItemIndex();
      }
    ui_field_index_element
  ;
ui_field_index_element /* DOCUMENTATION:UNFOLD */
  : /* none */
      {
        configurator -> setWildcard();
      }
  | '*'
      {
        configurator -> setWildcard();
      }
  | '#'  /* Backward compatibility ! */
      {
        configurator -> setWildcard();
      }
  | tINT_CONSTANT ui_field_index_maximum
      {
        configurator -> setLowerbound( $1 );
      }
  | tINT_CONSTANT '+' ui_field_index_variable ui_field_index_variable_maximum
      {
        configurator -> setLowerbound( $1 );
      }
  | ui_field_index_variable ui_field_index_variable_maximum
  ;
ui_field_index_variable /* DOCUMENTATION:UNFOLD */
  : tID_INDEX
      {
        configurator -> uiAddIndexContainer( *($1) );
        delete $1;
      }
  ;
ui_field_index_maximum /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ':' tINT_CONSTANT
      {
        configurator -> setUpperbound( $2 );
      }
  | ':' '*'
      {
        configurator -> setWildcard();
      }
  ;
ui_field_index_variable_maximum /* DOCUMENTATION:SYNONYM ui_field_index_maximum */
  : /* none */
  | ':' tINT_CONSTANT
      {
        configurator -> setUpperbound( $2 );
      }
  | ':' '*'
  ;

/* --------------------------------------------------------------------------- */
/* UI Fieldgroup Definition                                                    */
/* --------------------------------------------------------------------------- */

ui_fieldgroup_list /* DOCUMENTATION:DIAGRAM */
  : ui_fieldgroup
  | ui_fieldgroup_list ',' ui_fieldgroup
  ;
ui_fieldgroup /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newFieldgroup( *($1) );
        delete $1;
      }
    ui_fieldgroup_options
    ui_fieldgroup_fields
  ;
ui_fieldgroup_options /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | '{' ui_fieldgroup_option_list '}'
  ;
ui_fieldgroup_option_list /* DOCUMENTATION:UNFOLD */
  : ui_fieldgroup_option
  | ui_fieldgroup_option_list ',' ui_fieldgroup_option
  ;
ui_fieldgroup_option /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | string_constant ui_field_alignment
      {
        configurator -> setTitleFieldgroup( *($1), $2 );
        delete $1;
      }
  | tJUSTLEFT
      {
        configurator -> setAlignment( GuiElement::align_Left );
      }
  | tJUSTRIGHT
      {
        configurator -> setAlignment( GuiElement::align_Right );
      }
  | tJUSTCENTER
      {
        configurator -> setAlignment( GuiElement::align_Center );
      }
  | tTABLESIZE '=' tINT_CONSTANT ui_fieldgroup_table_step ui_fieldgroup_index_justify
      {
        configurator -> setFieldgroupTableSize( $3, $4 );
      }
  | tTABLE tORIENTATION '=' ui_orientation
      {
        configurator -> setFieldgroupOrientation( $4 );
      }
  | tTABLE tNAVIGATION '=' ui_orientation
      {
        configurator -> setFieldgroupNavigation( $4 );
      }
  | tTABLE tPOSITION '=' tINT_CONSTANT
      {
        configurator -> setTablePosition( $4 );
      }
  | tRANGE '(' tINT_CONSTANT ui_table_range_max ')'
      {
        configurator -> setTableIndexRange( $3, $4 );
      }
  | tMENU '=' tHIDDEN
      {
        configurator -> hideFieldgroupIndexMenu();
      }
  | tARROWS '=' tHIDDEN
      {
        configurator -> hideFieldgroupIndexLabel();
      }
  | tSTRETCH /* DOCUMENTATION:HIDE (no longer needed) */
      {
        configurator -> setAlignment( GuiElement::align_Stretch );
      }
  | tALIGN /* DOCUMENTATION:HIDE (not implemented in qt) */
      {
        configurator -> setAlignFields();
      }
  | tFRAME
      {
        configurator -> fieldgroupSetFrameOn();
      }
  | tMARGIN '(' tINT_CONSTANT ui_fieldgroup_spacing ')'
      {
        configurator -> fieldgroupSetMargin( $3, $4 );
      }
  | tJUSTIFY /* DOCUMENTATION:HIDE (not implemented in qt) */
      {
        configurator -> fieldgroupSetUseRuler();
      }
  | tINDEX '=' tID_INDEX
    {
      configurator -> fieldgroupSetGuiIndex( *($3) );
    }
  | tCLASS '=' string_constant
    {
      configurator -> fieldgroupSetClass( *($3) ) ;
      delete $3;
    }
  | tOVERLAY '(' sign tINT_CONSTANT ',' sign tINT_CONSTANT ')'
    {
     configurator-> fieldgroupSetOverlay( static_cast<int>($3*$4), static_cast<int>($6*$7) );
    }
  | tOVERLAY '(' sign tINT_CONSTANT ',' sign tINT_CONSTANT ',' sign tINT_CONSTANT ',' sign tINT_CONSTANT ')'
    {
      configurator-> fieldgroupSetOverlay( $3*$4, $6*$7, $9*$10, $12*$13 );
    }
  | tSCROLLBARS
    {
      configurator -> fieldgroupWithScrollBar();
    }
  | tFUNC '=' job_function_pointer
    {
      configurator -> setFieldgroupFunction();
    }
  ;

ui_fieldgroup_spacing /* DOCUMENTATION:UNFOLD */
  : /* none */                 { $$ = -1;  }
  | ',' tINT_CONSTANT          { $$ = $2; }
  ;
ui_table_range_max /* DOCUMENTATION:UNFOLD */
  : /* none */                 { $$ = 0;  }
  | ',' tINT_CONSTANT          { $$ = $2; }
  ;
ui_fieldgroup_table_step /* DOCUMENTATION:UNFOLD */
  : /* none */                 { $$ = -1; }
  | tSTEP tINT_CONSTANT        { $$ = $2; }
  ;
ui_fieldgroup_index_justify /* DOCUMENTATION:UNFOLD */
  : /* none */      { $$ = GuiElement::align_Default; }
  | ui_alignment
  ;
ui_orientation /* DOCUMENTATION:UNFOLD */
  : tVERTICAL       { $$ = GuiElement::orient_Vertical; }
  | tHORIZONTAL     { $$ = GuiElement::orient_Horizontal; }
  ;
ui_alignment /* DOCUMENTATION:UNFOLD */
  : tJUSTRIGHT      { $$ = GuiElement::align_Right; }
  | tJUSTLEFT       { $$ = GuiElement::align_Left; }
  | tJUSTCENTER     { $$ = GuiElement::align_Center; }
  ;
ui_fieldgroup_fields /* DOCUMENTATION:UNFOLD */
  : '(' ui_fieldgroup_line_list ')'
  ;
ui_fieldgroup_line_list /* DOCUMENTATION:UNFOLD */
  : ui_fieldgroup_line
  | ui_fieldgroup_line_list ',' ui_fieldgroup_line
  ;
ui_fieldgroup_line /* DOCUMENTATION:DIAGRAM */
  :   {
        configurator -> newFieldgroupLine();
      }
    ui_field_item_list
  | tARROWS
      {
        configurator -> addArrowBar();
      }
  ;
ui_field_item_list /* DOCUMENTATION:UNFOLD */
  : ui_field_item
  | ui_field_item_list  ui_field_item
  ;
ui_field_item /* DOCUMENTATION:UNFOLD */
  : ui_field_data_reference ui_field_attributes
      {
        configurator -> newDataField();
      }
    ui_field_additional_attributes ui_set_field_attributes
      {
        configurator -> attachDataField();
      }
  | string_constant ui_field_alignment
      {
        configurator -> attachStringConstant( (*$1), $2 );
        delete $1;
      }
    ui_field_options

  | tVOID
      {
        configurator -> attachVoidField();
      }
    ui_field_void_size ui_field_options
  | tSTRETCH '(' tINT_CONSTANT ',' tINT_CONSTANT ')'
      {
        configurator -> attachColumnStretchField( $3, $5 );
      }
     ui_field_options
  | tPIXMAP '(' string_constant ')' ui_field_alignment
      {
        configurator -> attachPixmap( (*$3), $5 );
        delete $3;
      }
    ui_field_options
  | tPIXMAP '(' ui_field_data_reference
      {
        configurator -> attachNewPixmap();
      }
    ui_field_pixmap_size ')'
    ui_field_options
  | tID_INDEX ui_field_alignment
      {
        configurator -> attachIndexField( *($1), $2 );
        delete $1;
      }
    ui_field_options
  | tID_FIELDGROUP ui_field_alignment
      {
        configurator -> attachFieldgroup( *($1), $2 );
        delete $1;
      }
    ui_field_options
  | tID_THERMO ui_field_alignment
     {
        configurator -> attachThermo( *($1), $2 );
        delete $1;
      }
    ui_field_options
  | tID_LIST ui_field_alignment
     {
        configurator -> attachList( *($1), $2 );
        delete $1;
      }
    ui_field_options
  | tID_TABLE ui_field_alignment
     {
        configurator -> attachTable( *($1), $2 );
        delete $1;
      }
    ui_field_options
  | tID_PLOT2D ui_field_alignment
     {
        configurator -> attachPlot2d( *($1), $2 );
        delete $1;
      }
    ui_field_options
  | tID_FOLDER ui_field_alignment
      {
        configurator -> attachFolder((*$1), $2 );
      }
    ui_field_options
  | tSEPARATOR ui_field_alignment
      {
        configurator -> newSeparator(false, $2);
      }
    ui_field_options
  ;
ui_field_pixmap_size /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | ',' tINT_CONSTANT ',' tINT_CONSTANT
      { configurator -> setPixmapSize( $2, $4 ); }
  ;
ui_field_void_size /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | '(' tINT_CONSTANT ',' tINT_CONSTANT ')'
      { configurator -> setVoidfieldSize( $2, $4 ); }
  | '(' tINT_CONSTANT '%' ',' tINT_CONSTANT '%' ')'
      { configurator -> setVoidfieldDisplayPercentSize( $2, $5 ); }
  ;

ui_field_options /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | '{' ui_field_option_list '}'
  ;
ui_field_option_list /* DOCUMENTATION:UNFOLD */
  : ui_field_option
  | ui_field_option_list ',' ui_field_option
  ;
ui_field_option /* DOCUMENTATION:UNFOLD */
  : /* none */
  | tCOLSPAN '=' tINT_CONSTANT
      {
        configurator -> setFieldColSpan( $3 );
      }
  | tROWSPAN '=' tINT_CONSTANT
      {
        configurator -> setFieldRowSpan( $3 );
      }
  | tVERTICAL
      {
       configurator -> setFieldOrientation( GuiElement::orient_Vertical );
      }
  |  tROTATE_180
      {
       configurator -> setFieldRotation(180);
      }
  | tHELPTEXT '=' tID_STREAM /* DOCUMENTATION:HIDE (not implemented in intens4) */
      {
       configurator -> setFieldHelptextStream(*($3));
      }
  | tCLASS '=' string_constant
    {
      configurator -> setFieldClass( *($3) ) ;
      delete $3;
    }
  ;
/* --------------------------------------------------------------------------- */
/* UI List Definition                                                          */
/* --------------------------------------------------------------------------- */

ui_list_list /* DOCUMENTATION:DIAGRAM */
  : ui_list
  | ui_list_list ',' ui_list
  ;
ui_list /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newList( *($1) );
        delete $1;
      }
    ui_list_options
    ui_list_items
  ;
ui_list_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_list_option_list '}'
  ;
ui_list_option_list /* DOCUMENTATION:DIAGRAM */
  : ui_list_option
  | ui_list_option_list ',' ui_list_option
  ;
ui_list_option /* DOCUMENTATION:UNFOLD */
  : string_constant ui_field_alignment
      {
        configurator -> setTitlelist( *($1), $2 );
        delete $1;
      }
  | tTABLESIZE '=' tINT_CONSTANT
      {
        configurator -> setListTableSize( $3 );
      }
  | tSTRETCH /* DOCUMENTATION:HIDE BEGIN */
      {
        configurator -> setListResizeable();
      } /* DOCUMENTATION:HIDE END */
  | tMULTIPLE_SELECTION
      {
        configurator -> setListMultipleSelection();
      }
  | tFUNC '=' job_function_pointer
      {
        configurator -> setListFunction();
      }
  | tINDEX ui_field_length
      {
        configurator -> withIndex( $2 );
      }
  | tSORT tDISABLE
      {
        configurator -> setListEnableSort( false );
      }
  ;
ui_list_items /* DOCUMENTATION:UNFOLD */
  : '(' ui_list_item_list ')'
  ;
ui_list_item_list /* DOCUMENTATION:DIAGRAM */
  : ui_list_item
  | ui_list_item_list ',' ui_list_item
  ;
ui_list_item /* DOCUMENTATION:UNFOLD */
  : string_constant ui_field_data_reference ui_field_attributes ui_list_item_options
      {
        configurator -> addListColumn( *($1) );
        delete $1;
      }  /* data_reference */
  | st_data_reference ui_field_data_reference ui_field_attributes
      {
        configurator -> addListColumn( $1 );
      }
  | tTOOLTIP ui_field_data_reference
      {
        configurator -> addListColumnToolTip();
      }
  | tCOLOR ui_field_data_reference
      {
        configurator -> addListColumnColor();
      }
  ;

ui_list_item_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_list_item_option_list '}'
  ;
ui_list_item_option_list /* DOCUMENTATION:UNFOLD */
  : ui_list_item_option
  | ui_list_item_option_list ',' ui_list_item_option
  ;
ui_list_item_option /* DOCUMENTATION:UNFOLD */
  : tOPTIONAL
      {
        configurator -> addListColumnOptional();
      }
  ;
/* --------------------------------------------------------------------------- */
/* UI Index Definition                                                         */
/* --------------------------------------------------------------------------- */

ui_index_list /* DOCUMENTATION:DIAGRAM */
  : ui_index
  | ui_index_list ',' ui_index
  ;
ui_index /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newIndexField( *($1) );
        delete $1;
     }
     ui_index_options
  ;
ui_index_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_index_option_list '}'
  ;
ui_index_option_list /* DOCUMENTATION:DIAGRAM */
  : ui_index_option
  | ui_index_option_list ',' ui_index_option
  ;
ui_index_option /* DOCUMENTATION:UNFOLD */
  : tORIENTATION '=' ui_orientation
      {
        configurator -> setIndexfieldOrientation( $3 );
      }
  | tSTEP tINT_CONSTANT
      {
        configurator -> setIndexfieldStep( $2 );
      }
  | tFUNC '=' job_function_pointer
      {
        configurator -> setIndexfieldFunction();
      }
  | tRANGE '(' tINT_CONSTANT ui_index_range_max ')'
      {
        configurator -> setIndexfieldRange( $3, $4 );
      }
  ;
ui_index_range_max /* DOCUMENTATION:UNFOLD */
  : /* none */                 { $$ = -1; }
  | ',' tINT_CONSTANT          { $$ = $2; }
  ;

/* --------------------------------------------------------------------------- */
/* UI Form Definition                                                          */
/* --------------------------------------------------------------------------- */

ui_form_list /* DOCUMENTATION:DIAGRAM */
  : ui_form
  | ui_form_list ',' ui_form
  ;
ui_form /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newGuiForm( *($1) );
        delete $1;
      }
    ui_form_options '(' ui_form_vertical_list ')'
      {
        configurator -> createButtonbar();
      }
  ;
ui_form_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_form_option_list '}'
  ;
ui_form_option_list /* DOCUMENTATION:DIAGRAM */
  : ui_form_option
  | ui_form_option_list ',' ui_form_option
  ;
ui_form_option /* DOCUMENTATION:UNFOLD */
  : string_constant
      {
        configurator -> setTitleForm( (*$1) );
        delete $1;
      }
  | tMAIN
      {
        configurator -> setMain();
      }
  | tAPP_MODAL
      {
        configurator -> setApplicationModal();
      }
  | tHIDDEN
      {
        configurator -> formSetMenuInstalled();
      }
  | tSCROLLBARS
      {
        configurator -> formSetScrollbarOn();
      }
  | tNO_SCROLLBARS
      {
        configurator -> formSetScrollbarOff();
      }
  | tPANEDWINDOW
      {
        configurator -> formSetPanedWindowOn();
      }
  | tNO_PANEDWINDOW
      {
        configurator -> formSetPanedWindowOff();
      }
  | tJUSTIFY /* DOCUMENTATION:HIDE (not implemented in qt) */
      {
        configurator -> formSetUseRuler();
      }
  | tHIDE_CYCLE
      {
        configurator -> resetCycleButton();
      }
  | tHELPKEY id_or_string_constant
      {
        configurator -> setHelpkey( (*$2) );
        delete $2;
      }
  | tBUTTONS '=' tINT_CONSTANT
      {
        configurator -> setButtonsPerLine( $3 );
      }
  | tHELPTEXT '=' string_constant /* DOCUMENTATION:HIDE (not implemented in intens4) */
      {
        configurator -> formSetHelptext( (*$3) );
        delete $3;
      }
  | tFUNC '=' job_function_pointer
      { configurator->formSetFunction( $3 );
      }
  | tCLOSE_BUTTON '=' tNONE
      {
        configurator->form_hasCloseButton(false);
      }
  | tEXPAND
      {
        configurator -> formSetExpandPolicy(GuiForm::expand_AtMapTime);
      }
  | tSETTINGS '=' tNONE
      {
        configurator -> formUseSettings(false);
      }
  ;

/* --------------------------------------------------------------------------- */
/* UI Form Container Options                                                   */
/* --------------------------------------------------------------------------- */

opt_ui_form_container_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ui_form_container_options
  ;
ui_form_container_options /* DOCUMENTATION:DIAGRAM */
  : '[' ui_form_container_option_list ']'
  ;
ui_form_container_option_list /* DOCUMENTATION:UNFOLD */
  : ui_form_container_option
  | ui_form_container_option_list ',' ui_form_container_option
  ;
ui_form_container_option /* DOCUMENTATION:UNFOLD */
  : tSCROLLBARS ui_scrollbar_orientation ui_scrollbar_options
      {
        configurator -> containerSetScrollbarOn( $2, $3 );
      }
  | tNO_SCROLLBARS
      {
        configurator -> containerSetScrollbarOff();
      }
  | tPANEDWINDOW
      {
        configurator -> containerSetPanedWindowOn();
      }
  | tNO_PANEDWINDOW
      {
        configurator -> containerSetPanedWindowOff();
      }
  | tJUSTIFY
      {
        configurator -> containerSetUseRuler();
      }
  | string_constant
      {
        configurator -> containerSetTitle( (*$1) );
      }
  | tFRAME
      {
        configurator -> containerSetFrameOn();
      }
  | tHELPTEXT '=' string_constant
      {
        configurator -> containerSetHelptext( (*$3) );
        delete $3;
      }
  ;

ui_scrollbar_orientation /* DOCUMENTATION:UNFOLD */
  : /* none */      { $$ = GuiElement::orient_Both; }
  | tVERTICAL       { $$ = GuiElement::orient_Vertical; }
  | tHORIZONTAL     { $$ = GuiElement::orient_Horizontal; }
  ;
ui_scrollbar_options /* DOCUMENTATION:UNFOLD */
  : /* none */ { $$ = GuiElement::scrollbar_AS_NEEDED; }
  | tALWAYS    { $$ = GuiElement::scrollbar_ON; }
  ;

/* --------------------------------------------------------------------------- */
/* UI Form Vertical Container Definition                                       */
/* --------------------------------------------------------------------------- */

ui_form_vertical_list /* DOCUMENTATION:DIAGRAM */
  : ui_form_vertical
  | ui_form_vertical_list ',' ui_form_vertical
  ;
ui_form_vertical /* DOCUMENTATION:UNFOLD */
  : ui_form_vertical_element
  ;
ui_form_vertical_element /* DOCUMENTATION:UNFOLD */
  :   {
        configurator -> newContainer( GuiElement::orient_Horizontal );   // 1: Horizontal, 2: Vertical
      }
    '(' ui_form_horizontal_list ')' opt_ui_form_container_options
      {
        configurator -> popContainerStack();
      }
  | ui_form_container_element
  ;

/* --------------------------------------------------------------------------- */
/* UI Form Horizontal Container Definition                                     */
/* --------------------------------------------------------------------------- */

ui_form_horizontal_list /* DOCUMENTATION:DIAGRAM */
  : ui_form_horizontal
  | ui_form_horizontal_list ',' ui_form_horizontal
  ;
ui_form_horizontal /* DOCUMENTATION:UNFOLD */
  : ui_form_horizontal_element
  ;
ui_form_horizontal_element /* DOCUMENTATION:UNFOLD */
  :   {
        configurator -> newContainer( GuiElement::orient_Vertical );   // 1: Horizontal, 2: Vertical
      }
    '(' ui_form_vertical_list ')' opt_ui_form_container_options
      {
        configurator -> popContainerStack();
      }
  | ui_form_container_element
  ;

/* --------------------------------------------------------------------------- */
/* UI Form Container Element Definition                                        */
/* --------------------------------------------------------------------------- */

ui_form_element_identifier /* DOCUMENTATION:DIAGRAM */
  : tID_FOLDER
  | tID_FIELDGROUP
  | tID_PLUGIN
  | tID_UNIPLOT
  | tID_LISTPLOT
  | tID_XRT3DPLOT /* DOCUMENTATION:HIDE (deprecated, use PLOT3D) */
  | tID_PLOT3D
  | tID_IMAGE
  | tID_LIST
  | tID_LINEPLOT
  | tID_PLOT2D
  | tID_TEXTWINDOW
  | tID_NAVIGATOR
  | tID_THERMO
  | tID_TIMETABLE
  | tID_TABLE
  ;
ui_form_container_element /* DOCUMENTATION:DIAGRAM */
  : ui_form_element_identifier
      {
        configurator -> cloneElement( (*$1) );
      }
  | tSTD_WINDOW
      {
        configurator -> attachStandardWindow();
      }
    ui_text_window_options
  | tLOG_WINDOW
      {
        configurator -> attachLogWindow();
      }
    ui_text_window_options
  | tSEPARATOR
      {
        configurator -> newSeparator();
      }
  | tVOID ui_form_void_size
      {
        configurator -> newVoid( $2 );
      }
  | tSTRETCH ui_form_void_size
      {
        configurator -> newStretch( $2 );
      }
  ;
ui_form_void_size /* DOCUMENTATION:UNFOLD */
  : /* none */            { $$ = 0; }
  | '(' tINT_CONSTANT ')' { $$ = $2; }
  ;

/* --------------------------------------------------------------------------- */
/* UI LISTPLOT Definition                                                      */
/* --------------------------------------------------------------------------- */

ui_listplot_list /* DOCUMENTATION:DIAGRAM */
  : ui_listplot
  | ui_listplot_list ',' ui_listplot
  ;
ui_listplot /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newListPlot( *($1) );
        delete $1;
      }
    ui_listplot_options '(' ui_listplot_vertical_graph_list ')'
  ;
ui_listplot_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_listplot_option_list '}'
  ;
ui_listplot_option_list /* DOCUMENTATION:DIAGRAM */
  : ui_listplot_option
  | ui_listplot_option_list ',' ui_listplot_option
  ;
ui_listplot_option /* DOCUMENTATION:UNFOLD */
  : string_constant
      {
        configurator -> setTitleListplot( *($1) );
      }
  | tCAPTION '=' tID_STREAM
      {
        configurator -> setCaptionStream( *($3) );
      }
  | tSIZE '(' tINT_CONSTANT ',' tINT_CONSTANT ')'
      {
        configurator -> listplot_setSize( $3, $5 );
      }
  ;
ui_listplot_vertical_graph_list /* DOCUMENTATION:DIAGRAM */
  : ui_listplot_vertical_graph
  | ui_listplot_vertical_graph_list ',' ui_listplot_vertical_graph
  ;
ui_listplot_vertical_graph /* DOCUMENTATION:UNFOLD */
  :  /* empty */
  | '('
      {
        configurator -> addHGraph();
      }
    ui_listplot_horizontal_graph_list ')'
  ;
ui_listplot_horizontal_graph_list /* DOCUMENTATION:DIAGRAM */
  : ui_listplot_horizontal_graph_item
  | ui_listplot_horizontal_graph_list ',' ui_listplot_horizontal_graph_item
  ;
ui_listplot_horizontal_graph_item /* DOCUMENTATION:DIAGRAM */
  : tIDENTIFIER
      {
        configurator -> addGraph( *($1) );
      }
    ui_listplot_graph_options '(' ui_listplot_item_list ')'
      {
        configurator -> checkGraph();
      }
  ;
ui_listplot_graph_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_listplot_graph_option_list '}'
  ;
ui_listplot_graph_option_list /* DOCUMENTATION:UNFOLD */
  : ui_listplot_graph_option
  | ui_listplot_graph_option_list ',' ui_listplot_graph_option
  ;
ui_listplot_graph_option /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> setTitleListgraph( *($1) );
      }
  | tID_STREAM
      {
        configurator -> setTitleListgraphStream( *($1) );
      }
  | tXAXIS '=' ui_field_data_reference opt_scale_factor
      {
        configurator -> setXaxis();
      }
      ui_listplot_item_options
  | tLABEL '=' tID_STREAM
      {
        configurator -> plotaxisSetLabelStream( *($3) );
      }
  | tAXES_ORIGIN '=' sign tREAL_CONSTANT
      {
        configurator -> listgraphSetAxisOriginY( $3 * $4 );
      }
  | tAXES_ORIGIN '=' sign tINT_CONSTANT
      {
        configurator -> listgraphSetAxisOriginY( static_cast<double>($3*$4) );
      }
  | tLOG_X
      {
        configurator -> listgraphSetLogX();
      }
  | tLOG_Y
      {
        configurator -> listgraphSetLogY();
      }
  | tSAME_YRANGE
      {
        configurator -> setSameYRange();
      }
  ;
ui_listplot_item_list /* DOCUMENTATION:UNFOLD */
  : ui_listplot_item
  | ui_listplot_item_list ',' ui_listplot_item
  ;
ui_listplot_item /* DOCUMENTATION:DIAGRAM */
  : ui_field_data_reference opt_scale_factor
      {
        configurator -> addGraphItem();
      }
    ui_listplot_item_options
  ;
ui_listplot_item_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_listplot_item_option_list '}'
  ;
ui_listplot_item_option_list /* DOCUMENTATION:UNFOLD */
  : ui_listplot_item_option
  | ui_listplot_item_option_list ',' ui_listplot_item_option
  ;
ui_listplot_item_option /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> setUnit( *($1) );
      }
  | tLINESTYLE '=' ui_listplot_lstyle
  | tLABEL '=' string_const
      {
        configurator -> plotaxisSetLabel( *($3) );
      }
  | tLABEL '=' tID_STREAM
      {
        configurator -> plotaxisSetLabelStream( *($3) );
      }
  | tUNIT '=' string_constant
      {
        configurator -> setUnit( *($3) );
      }
  | tUNIT '=' tID_STREAM
      {
        configurator -> plotaxisSetUnitStream( *($3) );
      }
  ;
ui_listplot_lstyle /* DOCUMENTATION:UNFOLD */
  : tLINEAR
      {
        configurator -> plotaxisSetLineStyle( GuiPlotDataItem::LINEAR );
      }
  | tSTEP
      {
        configurator -> plotaxisSetLineStyle( GuiPlotDataItem::STEP );
      }
  | tDISCRETE
      {
        configurator -> plotaxisSetLineStyle( GuiPlotDataItem::DISCRETE );
      }
  ;
/* --------------------------------------------------------------------------- */
/* UI UNIPLOT Definition                                                       */
/* --------------------------------------------------------------------------- */

ui_uniplot_list /* DOCUMENTATION:DIAGRAM */
  : ui_uniplot
  | ui_uniplot_list ',' ui_uniplot
  ;
ui_uniplot /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newSimpelPlot( *($1) );
        delete $1;
      }
    ui_uniplot_options
  ;
ui_uniplot_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' string_constant '}'
      {
        configurator -> setTitleSimpel( *($2) );
        delete $2;
      }
  ;
/* --------------------------------------------------------------------------- */
/* UI PSPLOT Definition                                                        */
/* --------------------------------------------------------------------------- */

ui_psplot_list /* DOCUMENTATION:DIAGRAM */
  : ui_psplot
  | ui_psplot_list ',' ui_psplot
  ;
ui_psplot /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newPSStream( *($1) );
        delete $1;
      }
    ui_psplot_options
  ;
ui_psplot_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' string_constant '}'
      {
        configurator -> setTitlePsplot( *($2) );
        delete $2;
      }
  ;


/* --------------------------------------------------------------------------- */
/* UI PLOT3D Definition                                                     */
/* --------------------------------------------------------------------------- */

ui_plot3d_list /* DOCUMENTATION:DIAGRAM */
  : ui_plot3d
  | ui_plot3d_list ',' ui_plot3d
  ;
ui_plot3d /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newGui3dPlot( *($1) );
        delete $1;
      }
    ui_plot3d_options '(' ui_plot3d_graph ')'
  ;
ui_plot3d_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_plot3d_option_list '}'
  ;
ui_plot3d_option_list /* DOCUMENTATION:UNFOLD */
  : ui_plot3d_option
  | ui_plot3d_option_list ',' ui_plot3d_option
  ;
ui_plot3d_option /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> plot3dSetMenuText( *($1) );
        delete $1;
      }
  | tHIDDEN
      {
        configurator -> plot3dSetMenuInstalled();
      }
  | tCAPTION '=' tID_STREAM
      {
        configurator -> plot3dSetFooterStream( *($3) );
        delete $3;
      }
  | tCAPTION '=' string_constant
      {
        configurator -> plot3dSetFooterText( *($3) );
        delete $3;
      }
  | tSTYLE '=' ui_plot3d_style
  | tSIZE '(' tINT_CONSTANT ',' tINT_CONSTANT ')'
      {
        configurator -> plot3dSetSize( $3, $5 );
      }
  ;
ui_plot3d_style /* DOCUMENTATION:DIAGRAM */
  : tBAR
      {
        configurator -> setPlotStyleBar();
      }
  | tSURFACE
      {
        configurator -> setPlotStyleSurface();
      }
  | tCONTOUR
      {
        configurator -> setPlotStyleContour();
      }
  ;
ui_plot3d_graph /* DOCUMENTATION:DIAGRAM */
  : tIDENTIFIER
    ui_plot3d_graph_options '(' ui_plot3d_item_list ')'
  ;
ui_plot3d_graph_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_plot3d_graph_option_list '}'
  ;
ui_plot3d_graph_option_list /* DOCUMENTATION:DIAGRAM */
  : ui_plot3d_graph_option
  | ui_plot3d_graph_option_list ',' ui_plot3d_graph_option
  ;
ui_plot3d_graph_option /* DOCUMENTATION:UNFOLD */
  : string_constant
      {
        configurator -> plot3dSetHeaderText( *($1) );
        delete $1;
      }
  | tID_STREAM
      {
        configurator -> plot3dSetHeaderStream( *($1) );
        delete $1;
      }
  | tXAXIS ui_plot3d_graph_axis_item ui_plot3d_axis_options
      {
        configurator -> putDataItemXaxis();
      }
  | tYAXIS ui_plot3d_graph_axis_item ui_plot3d_axis_options
      {
        configurator -> putDataItemYaxis();
      }
  | tXANNOTATION
      {
        configurator -> showPlot3dAnnotationLabels(true);
      }
  | tYANNOTATION
      {
        configurator -> showPlot3dAnnotationLabels(false);
      }
  ;
ui_plot3d_graph_axis_item /* DOCUMENTATION:DIAGRAM */
  : /* none */
      {
        configurator -> newPlotDataItem();
      }
  | '=' ui_field_data_reference opt_scale_factor
      {
        configurator -> plot3dRegisterIndexContainer();
      }
  ;
ui_plot3d_axis_option /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | tLABEL '=' string_const
      {
        configurator -> plotaxisSetLabel( *($3) );
        delete $3;
      }
  | tLABEL '=' tID_STREAM
      {
        configurator -> plotaxisSetLabelStream( *($3) );
        delete $3;
      }
  | tSCALE '(' real_value ',' real_value ')'
    {
       configurator->plot3dAxis_setScale( $3, $5 );
    }
  | tSCALE '(' temp_data_reference ',' temp_data_reference ')'
    {
       configurator->plot3dAxis_setScaleDataRef($3, $5);
    }
  | ui_plot3d_axis_marker_option
  | tANNOTATION '(' ui_xrtgraph_item_anno ')'
      {
        configurator -> withPlot3dAnnotationOption( true );
      }
  ;
ui_plot3d_axis_marker_option /* DOCUMENTATION:DIAGRAM */
  : tXAXIS ui_plot3d_marker_xaxis_option ui_plot3d_marker_option
      {
        configurator -> setPlot2dItemIsMarker();
      }
  ;

ui_plot3d_axis_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_plot3d_axis_option_list '}'
  ;

ui_plot3d_axis_option_list /* DOCUMENTATION:DIAGRAM */
  : ui_plot3d_axis_option
  | ui_plot3d_axis_option_list ',' ui_plot3d_axis_option
  ;

ui_plot3d_item_list /* DOCUMENTATION:UNFOLD */
  : ui_plot3d_item
  | ui_plot3d_item_list ',' ui_plot3d_item
  ;
ui_plot3d_item /* DOCUMENTATION:DIAGRAM */
  : ui_field_data_reference opt_scale_factor
      {
        configurator -> putDataItemZaxis();
      }
    ui_plot3d_axis_options
  ;
ui_plot3d_marker_xaxis_option /* DOCUMENTATION:UNFOLD */
  : '(' ui_field_data_reference opt_scale_factor ')'
     {
        configurator -> setPlot3dMarkerXAxis();
     }
ui_plot3d_marker_option /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ',' tMARKER '(' ui_field_data_reference ')'
     {
        configurator -> setPlot3dMarker();
     }
  ;


/* --------------------------------------------------------------------------- */
/* UI IMAGE Definition                                                         */
/* --------------------------------------------------------------------------- */

ui_image_list /* DOCUMENTATION:DIAGRAM */
  : ui_image
  | ui_image_list ',' ui_image
  ;
ui_image /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newImage( *($1) );
        delete $1;
      }
    ui_image_options
  ;
ui_image_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_image_option_list '}'
  ;
ui_image_option_list /* DOCUMENTATION:UNFOLD */
  : ui_image_option
  | ui_image_option_list ',' ui_image_option
  ;
ui_image_option /* DOCUMENTATION:DIAGRAM */
  : tSIZE '(' tINT_CONSTANT ',' tINT_CONSTANT ')'
      {
        configurator -> imageSetSize( $3, $5 );
      }
  | tSOCKET '=' tID_SOCKET
      {
        configurator -> imageSetSocket( *$3 );
        delete $3;
      }
  | tSETTINGS '=' ui_xfer
      {
        configurator -> imageSetImageSettings( $3 );
      }
  ;

/* --------------------------------------------------------------------------- */
/* UI LINE PLOT Definition                                                     */
/* --------------------------------------------------------------------------- */

ui_lineplot_list /* DOCUMENTATION:DIAGRAM */
  : ui_lineplot
  | ui_lineplot_list ',' ui_lineplot
  ;
ui_lineplot /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newLinePlot( *($1) );
        delete $1;
      }
    ui_lineplot_options
  ;
ui_lineplot_options /* DOCUMENTATION:SYNONYM ui_image_options */
  : /* none */
  | '{' ui_lineplot_option_list '}'
  ;
ui_lineplot_option_list /* DOCUMENTATION:HIDE BEGIN */
  : ui_lineplot_option
  | ui_lineplot_option_list ',' ui_lineplot_option
  ;
ui_lineplot_option
  : tSOCKET '=' tID_SOCKET
      {
        configurator -> imageSetSocket( *$3 );
        delete $3;
      }
  | tSETTINGS '=' ui_xfer
      {
        configurator -> imageSetImageSettings( $3 );
      }
  | tSIZE '(' tINT_CONSTANT ',' tINT_CONSTANT ')'
      {
        configurator -> imageSetSize( $3, $5 );
      }
  ; /* DOCUMENTATION:HIDE END */

/* --------------------------------------------------------------------------- */
/* UI THERMO Definition                                                        */
/* --------------------------------------------------------------------------- */

ui_thermo_list /* DOCUMENTATION:DIAGRAM */
  : ui_thermo
  | ui_thermo_list ',' ui_thermo
  ;
ui_thermo /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newThermo( *($1) );
        delete $1;
      }
    ui_thermo_options
    '(' ui_xfer opt_scale_factor ')'
      {
       configurator -> thermo_setXfer();
      }

  ;
ui_thermo_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_thermo_option_list '}'
  ;

ui_thermo_option_list /* DOCUMENTATION:UNFOLD */
  : ui_thermo_option
  | ui_thermo_option_list ',' ui_thermo_option
  ;
ui_thermo_option /* DOCUMENTATION:DIAGRAM */
  : tRANGE '(' real_value ',' real_value ')'
      {
        configurator -> thermo_setRange( $3, $5 );
      }
  | tRANGE '(' range_data_reference ',' range_data_reference ')'
      { configurator -> thermo_setRange();
      }

  | tOFFSET '(' real_value ')'
      {
        configurator -> thermo_setOffset( $3 );
      }
  | tOFFSET '(' range_data_reference ')'
      { configurator -> thermo_setOffset();
      }

  | tCOLOR '=' tID_COLORSET
      { configurator -> thermo_setColorSetName( *($3) );
        delete $3;
      }

  | tCOLOR_SCALE ui_thermo_color_scale_option
      {
        configurator -> thermo_setColorScaleOption();
      }
  | tORIENTATION '=' ui_orientation
      {
        configurator -> thermo_setOrientation( $3 );
      }
  | tSIZE '(' tINT_CONSTANT ',' tINT_CONSTANT ')'
      {
        configurator -> thermo_setSize( $3, $5 );
      }
  | tFORMAT '=' string_constant
    {
       configurator->thermo_setScaleFormat( *($3) );
       delete $3;
    }
  | tLABEL '=' tNONE
    {
       configurator->thermo_hideLabel();
    }
  | tLABEL '=' string_const
    {
          configurator->thermo_setLabel( *($3) );
          delete $3;
    }
  | tLABEL '=' temp_data_reference
    {
          configurator->thermo_setLabel( $3 );
    }
  | tUNIT '=' tNONE
    {
       configurator->thermo_hideUnits();
    }
  | tUNIT '=' string_const
    {
       configurator->thermo_setUnits( *($3) );
       delete $3;
    }
  | tUNIT '=' temp_data_reference
    {
       configurator->thermo_setUnits( $3 );
    }
  | tALARM_LEVEL '=' real_value
    {
       configurator -> thermo_setAlarmLevel( $3 );
    }
  | tALARM_LEVEL  '=' range_data_reference
    {
       configurator -> thermo_setAlarmLevel();
    }
  | tALARM_COLOR '=' string_constant
    {
       configurator -> thermo_setAlarmColorName( *($3) );
       delete $3;
    }
  | tINVERTED '=' bool_constant
    {
       configurator -> thermo_setInverted($3);
    }
  ;

ui_thermo_color_scale_option /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '(' tSCALE ')'
     {
        configurator -> thermo_setColorScaleOptionScaled();
     }
  ;

/* --------------------------------------------------------------------------- */
/* UI TIME TABLE Definition                                                    */
/* --------------------------------------------------------------------------- */

ui_timetable_list /* DOCUMENTATION:DIAGRAM */
  : ui_timetable
  | ui_timetable_list ',' ui_timetable
  ;
ui_timetable /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newTimeTable( *($1) );
        delete $1;
      }
    ui_timetable_options
  ;
ui_timetable_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_timetable_option_list '}'
  ;
ui_timetable_option_list /* DOCUMENTATION:UNFOLD */
  : ui_timetable_option
  | ui_timetable_option_list ',' ui_timetable_option
  ;
ui_timetable_option /* DOCUMENTATION:DIAGRAM */
  : tFROM_STRING_DATETIME '=' ui_field_data_reference
      {
        configurator -> timeTableSetFromStringDateTime();
      }
  | tTO_STRING_TIME '=' ui_field_data_reference
      {
        configurator -> timeTableSetToStringTime();
      }
  | tTO_STRING_DATETIME '=' ui_field_data_reference
      {
        configurator -> timeTableSetToStringDateTime();
      }
  | tSTRING '=' ui_field_data_reference
      {
        configurator -> timeTableSetString();
      }
  | tSIZE '(' tINT_CONSTANT ',' tINT_CONSTANT ')'
      {
        configurator -> timeTableSetSize( $3, $5 );
      }
  ;


/* --------------------------------------------------------------------------- */
/* UI XRTGRAPH Definition                                                      */
/* --------------------------------------------------------------------------- */

ui_xrtgraph_list
  : ui_xrtgraph
  | ui_xrtgraph_list ',' ui_xrtgraph
  ;
ui_xrtgraph
  : tIDENTIFIER
      {
        configurator -> newXrtGraph( *($1) );
        delete $1;
      }
    ui_xrtgraph_options '(' ui_xrtgraph_graph ')'
  ;
ui_xrtgraph_options
  : /* none */
  | '{' ui_xrtgraph_option_list '}'
  ;
ui_xrtgraph_option_list
  : ui_xrtgraph_option
  | ui_xrtgraph_option_list ',' ui_xrtgraph_option
  ;
ui_xrtgraph_option
  : string_constant
      {
        configurator -> xrtgraphSetMenuText( *($1) );
        delete $1;
      }
  | tCAPTION '=' tID_STREAM
      {
        configurator -> xrtgraphSetFooterStream( *($3) );
        delete $3;
      }
  | tCAPTION '=' string_constant
      {
        configurator -> xrtgraphSetFooterText( *($3) );
        delete $3;
      }
  | tPRINTSTYLE '=' bool_constant
      {
        configurator -> xrtgraphSetPrintStyle( $3 );
      }
  | tMINMAX ui_field_precision bool_assignment
    { configurator -> xrtgraphSetMINMAX( $3, $2 ); }
  | tRMS ui_field_precision bool_assignment
    { configurator -> xrtgraphSetRMS( $3, $2 ); }
  | tAVG ui_field_precision bool_assignment
    { configurator -> xrtgraphSetAVG( $3, $2 ); }
  ;

ui_xrtgraph_graph
  : tIDENTIFIER
    ui_xrtgraph_graph_options '(' ui_xrtgraph_item_list ')'
  ;
ui_xrtgraph_graph_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_xrtgraph_graph_option_list '}'
  ;
ui_xrtgraph_graph_option_list /* DOCUMENTATION:UNFOLD */
  : ui_xrtgraph_graph_option
  | ui_xrtgraph_graph_option_list ',' ui_xrtgraph_graph_option
  ;
ui_xrtgraph_graph_option /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> xrtgraphSetHeaderText( *($1) );
        delete $1;
      }
  | tID_STREAM
      {
        configurator -> xrtgraphSetHeaderStream( *($1) );
        delete $1;
      }
  | tXAXIS ui_xrtgraph_graph_axis_item ui_xrtgraph_xaxis_label  /* undocumented ! */
      {
        configurator -> addDataItem();
      }
  | tXAXIS ui_xrtgraph_xaxis_label
  | tYAXIS1 ui_xrtgraph_yaxis1_label
  | tYAXIS2 ui_xrtgraph_yaxis2_label
  | tALLCYCLES
      {
        configurator -> setAllCycles();
      }
  | tAXES_ORIGIN_X '=' real_value
      {
        configurator -> setOriginXAxis( $3 );
      }
  | tAXES_ORIGIN_Y '=' real_value
      {
        configurator -> setOriginYAxis( $3 );
      }
  | tLOG_X
      {
        configurator -> setLogX();
      }
  | tLOG_Y
      {
        configurator -> setLogY();
      }
  | tCOMBOBOX ui_field_length
      {
        configurator -> setConfigDialogStyle( $2 );
      }
  | tSAME_YRANGE /* DOCUMENTATION:HIDE (does nothing) */
      {
//        configurator -> setSameYRange();
      }
  | tSTYLE '=' ui_xrtgraph_style
  | tY1_STYLE '=' ui_xrtgraph_y1_style
  | tY2_STYLE '=' ui_xrtgraph_y2_style
  | tSCROLLBARS
      {
        configurator -> setWithScrollBar();
      }
  | tXANNOTATION
      {
        configurator -> showAnnotationLabels();
      }
  ;

ui_xrtgraph_linestyle /* DOCUMENTATION:UNFOLD */
  : tLINEAR
  | tSTEP
      {
        configurator -> setYPlotStyle( Gui2dPlot::STEP );
      }
  | tDISCRETE /* DOCUMENTATION:HIDE (does nothing) */
      {
//        configurator -> plotaxisSetLineStyle( GuiPlotDataItem::DISCRETE );
      }
  ;

ui_xrtgraph_style /* DOCUMENTATION:DIAGRAM */
  : tPLOT
      {
        configurator -> setYPlotStyle( Gui2dPlot::PLOT );
      }
  | tBAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::BAR );
      }
  | tSTACKING_BAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::STACKING_BAR );
      }
  | tAREA
      {
        configurator -> setYPlotStyle( Gui2dPlot::AREA );
      }
  | tPOLAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::POLAR );
      }
  | tSTEP
      {
        configurator -> setYPlotStyle( Gui2dPlot::STEP );
      }
  ;
ui_xrtgraph_y1_style /* DOCUMENTATION:SYNONYM ui_xrtgraph_style */
  : tPLOT
      {
        configurator -> setYPlotStyle( Gui2dPlot::PLOT, Gui2dPlot::Y1AXIS );
      }
  | tBAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::BAR, Gui2dPlot::Y1AXIS );
      }
  | tSTACKING_BAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::STACKING_BAR, Gui2dPlot::Y1AXIS );
      }
  | tAREA
      {
        configurator -> setYPlotStyle( Gui2dPlot::AREA, Gui2dPlot::Y1AXIS );
      }
  | tPOLAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::POLAR, Gui2dPlot::Y1AXIS );
      }
  | tSTEP
      {
        configurator -> setYPlotStyle( Gui2dPlot::STEP, Gui2dPlot::Y1AXIS );
      }
  ;
ui_xrtgraph_y2_style /* DOCUMENTATION:SYNONYM ui_xrtgraph_style */
  : tPLOT
      {
        configurator -> setYPlotStyle( Gui2dPlot::PLOT, Gui2dPlot::Y2AXIS );
      }
  | tBAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::BAR, Gui2dPlot::Y2AXIS );
      }
  | tSTACKING_BAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::STACKING_BAR, Gui2dPlot::Y2AXIS );
      }
  | tAREA
      {
        configurator -> setYPlotStyle( Gui2dPlot::AREA, Gui2dPlot::Y2AXIS );
      }
  | tPOLAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::POLAR, Gui2dPlot::Y2AXIS );
      }
  | tSTEP
      {
        configurator -> setYPlotStyle( Gui2dPlot::STEP, Gui2dPlot::Y2AXIS );
      }
  ;
ui_xrtgraph_graph_axis_item /* DOCUMENTATION:DIAGRAM */
  : '=' ui_field_data_reference opt_scale_factor  /* undocumented ! */
      {
        configurator -> xrtgraphRegisterIndexContainer();
      }
  ;

ui_xrtgraph_xaxis_label /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | '{' tLABEL '=' string_const '}'
      {
        configurator -> xrtgraphSetLabel( *($4), Gui2dPlot::XAXIS );
        delete $4;
      }
  | '{' tLABEL '=' tID_STREAM '}'
      {
        configurator -> xrtgraphSetLabelStream( *($4), Gui2dPlot::XAXIS );
      }
  ;
ui_xrtgraph_yaxis1_label /* DOCUMENTATION:DIAGRAM */
  : '{' tLABEL '=' string_const '}'
      {
        configurator -> xrtgraphSetLabel( *($4), Gui2dPlot::Y1AXIS );
        delete $4;
      }
  | '{' tLABEL '=' tID_STREAM '}'
      {
        configurator -> xrtgraphSetLabelStream( *($4), Gui2dPlot::Y1AXIS );
      }
  ;
ui_xrtgraph_yaxis2_label /* DOCUMENTATION:SYNONYM ui_xrtgraph_yaxis1_label */
  : '{' tLABEL '=' string_const '}'
      {
        configurator -> xrtgraphSetLabel( *($4), Gui2dPlot::Y2AXIS );
        delete $4;
      }
  | '{' tLABEL '=' tID_STREAM '}'
      {
        configurator -> xrtgraphSetLabelStream( *($4), Gui2dPlot::Y2AXIS );
      }
  ;

ui_xrtgraph_item_list /* DOCUMENTATION:UNFOLD */
  : ui_xrtgraph_item
  | ui_xrtgraph_item_list ',' ui_xrtgraph_item
  ;
ui_xrtgraph_item /* DOCUMENTATION:DIAGRAM */
  : ui_field_data_reference opt_scale_factor
      {
        configurator -> xrtgraphNewPlotDataItem();
      }
    ui_xrtgraph_item_options
  ;
ui_xrtgraph_item_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_xrtgraph_item_option_list '}'
  ;
ui_xrtgraph_item_option_list /* DOCUMENTATION:UNFOLD */
  : ui_xrtgraph_item_option
  | ui_xrtgraph_item_option_list ',' ui_xrtgraph_item_option
  ;
ui_xrtgraph_item_option /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> setUnit( *($1) );
        delete $1;
      }
  | tLABEL '=' string_constant
      {
        configurator -> plotaxisSetLabel( *($3) );
        delete $3;
      }
  | tLABEL '=' tID_STREAM
      {
        configurator -> plotaxisSetLabelStream( *($3) );
        delete $3;
      }
  | tUNIT '=' string_constant
      {
        configurator -> setUnit( *($3) );
        delete $3;
      }
  | tUNIT '=' tID_STREAM
      {
        configurator -> plotaxisSetUnitStream( *($3) );
        delete $3;
      }
  | tYAXIS1
      {
        configurator -> setAxisType( Gui2dPlot::Y1AXIS );
      }
  | tYAXIS2
      {
        configurator -> setAxisType( Gui2dPlot::Y2AXIS );
      }
  | tXAXIS
      {
        configurator -> setAxisType( Gui2dPlot::XAXIS );
      }
  | tHIDDEN
      {
        configurator -> setAxisType( Gui2dPlot::HIDDEN );
      }
  | tXANNOTATION '(' ui_xrtgraph_item_anno ')'
      {
        configurator -> withAnnotationOption( true );
      }
  | tLINESTYLE '=' ui_xrtgraph_linestyle
  ;
ui_xrtgraph_item_anno /* DOCUMENTATION:DIAGRAM */
  : ui_xrtgraph_item_anno_title ui_xrtgraph_item_anno_values ',' ui_xrtgraph_item_anno_labels ui_xrtgraph_item_anno_angle
  ;
ui_xrtgraph_item_anno_values /* DOCUMENTATION:DIAGRAM */
  : ui_field_data_reference opt_scale_factor
      {
        configurator -> setAnnotationValues();
      }
  ;
ui_xrtgraph_item_anno_labels /* DOCUMENTATION:DIAGRAM */
  : ui_field_data_reference
      {
        configurator -> setAnnotationLabels();
      }
  ;

ui_xrtgraph_item_anno_title /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | tLABEL '=' string_constant ','
      {
        configurator -> plotAxisSetAnnoTitle( *($3) );
        delete $3;
      }
ui_xrtgraph_item_anno_angle /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | ',' real_value
      {
        configurator -> plotAxisSetAnnoAngle( $2 );
      }
  ;

/* --------------------------------------------------------------------------- */
/* UI PLOT2D Definition                                                        */
/* --------------------------------------------------------------------------- */

ui_plot2d_list /* DOCUMENTATION:DIAGRAM */
  : ui_plot2d
  | ui_plot2d_list ',' ui_plot2d
  ;
ui_plot2d /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> new2dPlot( *($1) );
        delete $1;
      }
    ui_plot2d_options '(' ui_plot2d_graph ')'
  ;

ui_plot2d_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_plot2d_option_list '}'
  ;
ui_plot2d_option_list /* DOCUMENTATION:UNFOLD */
  : ui_plot2d_option
  | ui_plot2d_option_list ',' ui_plot2d_option
  ;
ui_plot2d_option /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> xrtgraphSetMenuText( *($1) );
        delete $1;
      }
  | tHIDDEN
      {
        configurator -> xrtgraphSetMenuInstalled();
      }
  | tCAPTION '=' tID_STREAM
      {
        configurator -> xrtgraphSetFooterStream( *($3) );
        delete $3;
      }
  | tCAPTION '=' string_constant
      {
        configurator -> xrtgraphSetFooterText( *($3) );
        delete $3;
      }
  | tPRINTSTYLE '=' bool_constant
      {
        configurator -> xrtgraphSetPrintStyle( $3 );
      }
  | tMINMAX ui_field_precision  bool_assignment
    { configurator -> xrtgraphSetMINMAX( $3, $2 ); }
  | tRMS ui_field_precision  bool_assignment
    { configurator -> xrtgraphSetRMS( $3, $2 ); }
  | tAVG ui_field_precision  bool_assignment
    { configurator -> xrtgraphSetAVG( $3, $2 ); }
  ;
ui_plot2d_graph /* DOCUMENTATION:DIAGRAM */
  : tIDENTIFIER
    ui_plot2d_graph_options '(' ui_plot2d_item_list ')'
  ;

ui_plot2d_graph_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_plot2d_graph_option_list '}'
  ;
ui_plot2d_graph_option_list /* DOCUMENTATION:UNFOLD */
  : ui_plot2d_graph_option
  | ui_plot2d_graph_option_list ',' ui_plot2d_graph_option
  ;
ui_plot2d_graph_option /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> xrtgraphSetHeaderText( *($1) );
        delete $1;
      }
  | tID_STREAM
      {
        configurator -> xrtgraphSetHeaderStream( *($1) );
        delete $1;
      }
  | tXAXIS  '{' ui_plot2d_xaxis_options  '}'
  | tXAXIS2 '{' ui_plot2d_xaxis2_options '}'
  | tYAXIS1 '{' ui_plot2d_yaxis1_options '}'
  | tYAXIS2 '{' ui_plot2d_yaxis2_options '}'
  | tSIZE '(' tINT_CONSTANT ',' tINT_CONSTANT ')'
      {
        configurator -> plot2d_setSize( $3, $5 );
      }
  | tAXES_ORIGIN_X '=' real_value
      {
        configurator -> setOriginXAxis( $3 );
      }
  | tAXES_ORIGIN_Y '=' real_value
      {
        configurator -> setOriginYAxis( $3 );
      }
  | tLOG_X
      {
        configurator -> setLogX();
      }
  | tLOG_Y
      {
        configurator -> setLogY();
      }
  | tCOMBOBOX ui_field_length
      {
        configurator -> setConfigDialogStyle( $2 );
      }
  | tSTYLE '=' ui_plot2d_style
  | tY1_STYLE '=' ui_plot2d_y1_style
  | tY2_STYLE '=' ui_plot2d_y2_style
  | tALLCYCLES
      {
        configurator -> setAllCycles();
      }
  | tSCROLLBARS
      {
        configurator -> setWithScrollBar();
      }
  | tXANNOTATION
      {
        configurator -> showAnnotationLabels();
      }
  | tFUNC '=' job_function_pointer
      {
        configurator -> setPlot2dFunction();
      }
  | tCOLOR '=' tID_COLORSET
    {
        configurator->setPlot2dColorSetName( *($3) );
        delete $3;
    }
  ;

ui_plot2d_xaxis_options /* DOCUMENTATION:DIAGRAM */
  : ui_plot2d_xaxis_option
  | ui_plot2d_xaxis_options ',' ui_plot2d_xaxis_option
  ;
ui_plot2d_xaxis_option /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ui_plot2d_xaxis_label
  | tSCALE '(' real_value ',' real_value ')'
    {
       configurator->plot2d_setScale( $3, $5, Gui2dPlot::XAXIS );
    }
  | tSCALE '(' range_data_reference ',' range_data_reference ')'
    {
       configurator->plot2d_setScaleDataRef( Gui2dPlot::XAXIS );
    }
  | tFORMAT '=' string_constant
    {
       configurator->plot2d_setScaleFormat( Gui2dPlot::XAXIS, *($3) );
       delete $3;
    }
  | tHIDDEN
    {
       configurator->setPlot2dAxisHide( Gui2dPlot::XAXIS );
    }
  | tASPECT_RATIO_REF_AXIS
      {
        configurator -> plot2d_setAspectRatioReferenceAxis(Gui2dPlot::XAXIS);
      }
  | tASPECT_RATIO '=' real_value
      {
        configurator -> plot2d_setAspectRatio(Gui2dPlot::XAXIS, $3);
      }
  | tASPECT_RATIO '=' st_data_reference
      {
        configurator -> plot2d_setAspectRatioXfer(Gui2dPlot::XAXIS);
      }
  ;

ui_plot2d_xaxis2_options /* DOCUMENTATION:SYNONYM ui_plot2d_xaxis_options */
  : ui_plot2d_xaxis2_option
  | ui_plot2d_xaxis2_options ',' ui_plot2d_xaxis2_option
  ;
ui_plot2d_xaxis2_option /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ui_plot2d_xaxis2_label
  | tSCALE '(' real_value ',' real_value ')'
    {
       configurator->plot2d_setScale( $3, $5, Gui2dPlot::XAXIS2 );
    }
  | tSCALE '(' range_data_reference ',' range_data_reference ')'
    {
       configurator->plot2d_setScaleDataRef( Gui2dPlot::XAXIS2 );
    }
  | tFORMAT '=' string_constant
    {
       configurator->plot2d_setScaleFormat( Gui2dPlot::XAXIS2, *($3) );
       delete $3;
    }
  | tHIDDEN
    {
       configurator->setPlot2dAxisHide( Gui2dPlot::XAXIS2 );
    }
  | tASPECT_RATIO_REF_AXIS
      {
        configurator -> plot2d_setAspectRatioReferenceAxis( Gui2dPlot::XAXIS2 );
      }
  | tASPECT_RATIO '=' real_value
      {
        configurator -> plot2d_setAspectRatio( Gui2dPlot::XAXIS2, $3 );
      }
  | tASPECT_RATIO '=' st_data_reference
      {
        configurator -> plot2d_setAspectRatioXfer(Gui2dPlot::XAXIS2);
      }
  ;

ui_plot2d_yaxis1_options /* DOCUMENTATION:SYNONYM ui_plot2d_xaxis_options */
  : ui_plot2d_yaxis1_option
  | ui_plot2d_yaxis1_options ',' ui_plot2d_yaxis1_option
  ;
ui_plot2d_yaxis1_option /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ui_plot2d_yaxis1_label
  | tSCALE '(' real_value ',' real_value ')'
    {
       configurator->plot2d_setScale( $3, $5, Gui2dPlot::Y1AXIS );
    }
  | tSCALE '(' range_data_reference ',' range_data_reference ')'
    {
       configurator->plot2d_setScaleDataRef( Gui2dPlot::Y1AXIS );
    }
  | tFORMAT '=' string_constant
    {
       configurator->plot2d_setScaleFormat( Gui2dPlot::Y1AXIS, *($3) );
       delete $3;
    }
  | tHIDDEN
    {
       configurator->setPlot2dAxisHide( Gui2dPlot::Y1AXIS );
    }
  | tASPECT_RATIO_REF_AXIS
      {
        configurator -> plot2d_setAspectRatioReferenceAxis( Gui2dPlot::Y1AXIS );
      }
  | tASPECT_RATIO '=' real_value
      {
        configurator -> plot2d_setAspectRatio( Gui2dPlot::Y1AXIS, $3 );
      }
  | tASPECT_RATIO '=' st_data_reference
      {
        configurator -> plot2d_setAspectRatioXfer(Gui2dPlot::Y1AXIS);
      }
  ;

ui_plot2d_yaxis2_options /* DOCUMENTATION:SYNONYM ui_plot2d_xaxis_options */
  : ui_plot2d_yaxis2_option
  | ui_plot2d_yaxis2_options ',' ui_plot2d_yaxis2_option
  ;
ui_plot2d_yaxis2_option /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ui_plot2d_yaxis2_label
  | tSCALE '(' real_value ',' real_value ')'
    {
       configurator->plot2d_setScale( $3, $5, Gui2dPlot::Y2AXIS );
    }
  | tSCALE '(' range_data_reference ',' range_data_reference ')'
    {
       configurator->plot2d_setScaleDataRef( Gui2dPlot::Y2AXIS );
    }
  | tFORMAT '=' string_constant
    {
       configurator->plot2d_setScaleFormat( Gui2dPlot::Y2AXIS, *($3) );
       delete $3;
    }
  | tHIDDEN
    {
       configurator->setPlot2dAxisHide( Gui2dPlot::Y2AXIS );
    }
  | tASPECT_RATIO_REF_AXIS
      {
        configurator -> plot2d_setAspectRatioReferenceAxis( Gui2dPlot::Y2AXIS );
      }
  | tASPECT_RATIO '=' real_value
      {
        configurator -> plot2d_setAspectRatio( Gui2dPlot::Y2AXIS, $3 );
      }
  | tASPECT_RATIO '=' st_data_reference
      {
        configurator -> plot2d_setAspectRatioXfer(Gui2dPlot::Y2AXIS);
      }
  ;

ui_plot2d_style /* DOCUMENTATION:DIAGRAM */
  : tPLOT
      {
        configurator -> setYPlotStyle( Gui2dPlot::PLOT );
      }
  | tBAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::BAR );
      }
  | tSTACKING_BAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::STACKING_BAR );
      }
  | tAREA
      {
        configurator -> setYPlotStyle( Gui2dPlot::AREA );
      }
  | tPOLAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::POLAR );
      }
  | tSTEP
      {
        configurator -> setYPlotStyle( Gui2dPlot::STEP );
      }
  | tDOTS
      {
        configurator -> setYPlotStyle( Gui2dPlot::DOTS );
      }
  ;
ui_plot2d_y1_style /* DOCUMENTATION:SYNONYM ui_plot2d_style */
  : tPLOT
      {
        configurator -> setYPlotStyle( Gui2dPlot::PLOT, Gui2dPlot::Y1AXIS );
      }
  | tBAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::BAR, Gui2dPlot::Y1AXIS );
      }
  | tSTACKING_BAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::STACKING_BAR, Gui2dPlot::Y1AXIS );
      }
  | tAREA
      {
        configurator -> setYPlotStyle( Gui2dPlot::AREA, Gui2dPlot::Y1AXIS );
      }
  | tPOLAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::POLAR, Gui2dPlot::Y1AXIS );
      }
  | tSTEP
      {
        configurator -> setYPlotStyle( Gui2dPlot::STEP, Gui2dPlot::Y1AXIS );
      }
  | tDOTS
      {
        configurator -> setYPlotStyle( Gui2dPlot::DOTS, Gui2dPlot::Y1AXIS );
      }
  ;
ui_plot2d_y2_style /* DOCUMENTATION:SYNONYM ui_plot2d_style */
  : tPLOT
      {
        configurator -> setYPlotStyle( Gui2dPlot::PLOT, Gui2dPlot::Y2AXIS );
      }
  | tBAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::BAR, Gui2dPlot::Y2AXIS );
      }
  | tSTACKING_BAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::STACKING_BAR, Gui2dPlot::Y2AXIS );
      }
  | tAREA
      {
        configurator -> setYPlotStyle( Gui2dPlot::AREA, Gui2dPlot::Y2AXIS );
      }
  | tPOLAR
      {
        configurator -> setYPlotStyle( Gui2dPlot::POLAR, Gui2dPlot::Y2AXIS );
      }
  | tSTEP
      {
        configurator -> setYPlotStyle( Gui2dPlot::STEP, Gui2dPlot::Y2AXIS );
      }
  | tDOTS
      {
        configurator -> setYPlotStyle( Gui2dPlot::DOTS, Gui2dPlot::Y2AXIS );
      }
  ;

ui_plot2d_xaxis_label /* DOCUMENTATION:UNFOLD */
  : tLABEL '=' ui_plot2d_xaxis_label_text
  ;

ui_plot2d_xaxis_label_text /* DOCUMENTATION:UNFOLD */
  : string_const
      {
        configurator -> xrtgraphSetLabel( *($1), Gui2dPlot::XAXIS );
        delete $1;
      }
  | tID_STREAM
     {
        configurator -> xrtgraphSetLabelStream( *($1), Gui2dPlot::XAXIS );
      }
  ;

ui_plot2d_xaxis2_label /* DOCUMENTATION:UNFOLD */
  : tLABEL '=' ui_plot2d_xaxis2_label_text
  ;

ui_plot2d_xaxis2_label_text /* DOCUMENTATION:UNFOLD */
  : string_const
      {
        configurator -> xrtgraphSetLabel( *($1), Gui2dPlot::XAXIS2 );
        delete $1;
      }
  | tID_STREAM
     {
        configurator -> xrtgraphSetLabelStream( *($1), Gui2dPlot::XAXIS2 );
      }
  ;

ui_plot2d_yaxis1_label /* DOCUMENTATION:HIDE BEGIN ui_plot2d_yaxis1_options renamed to ui_plot2d_xaxis_options */
  : tLABEL '=' ui_plot2d_yaxis1_label_text
  ;
ui_plot2d_yaxis1_label_text
  : string_const
      {
        configurator -> xrtgraphSetLabel( *($1), Gui2dPlot::Y1AXIS );
        delete $1;
      }
  | tID_STREAM
      {
        configurator -> xrtgraphSetLabelStream( *($1), Gui2dPlot::Y1AXIS );
      }
  ;
ui_plot2d_yaxis2_label
  : tLABEL '=' ui_plot2d_yaxis2_label_text
  ;
ui_plot2d_yaxis2_label_text
  : string_const
      {
        configurator -> xrtgraphSetLabel( *($1), Gui2dPlot::Y2AXIS );
        delete $1;
      }
  | tID_STREAM
      {
        configurator -> xrtgraphSetLabelStream( *($1), Gui2dPlot::Y2AXIS );
      }
  ; /* DOCUMENTATION:HIDE END */

ui_plot2d_item_list /* DOCUMENTATION:DIAGRAM */
  : ui_plot2d_item
  | ui_plot2d_item_list ',' ui_plot2d_item
  | ui_plot2d_item_list ',' '(' ui_plot2d_x_item ',' ui_plot2d_y_item_list ')'
  ;
ui_plot2d_item /* DOCUMENTATION:DIAGRAM */
  : ui_plot2d_x_item '(' ui_plot2d_y_item_list ')'
  ;
ui_plot2d_y_item_list /* DOCUMENTATION:UNFOLD */
  : ui_plot2d_y_item
  | ui_plot2d_y_item_list ',' ui_plot2d_y_item
  ;

ui_plot2d_x_item /* DOCUMENTATION:DIAGRAM */
  : /* none */
    {
        configurator -> addDummyItem();
    }
  | ui_field_data_reference opt_scale_factor
      {
        configurator -> plot2dAddDataItemX();
      }
    ui_plot2d_x_item_options
  ;
ui_plot2d_x_item_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_plot2d_x_item_option_list '}'
  ;
ui_plot2d_x_item_option_list /* DOCUMENTATION:UNFOLD */
  : ui_plot2d_x_item_option
  | ui_plot2d_x_item_option_list ',' ui_plot2d_x_item_option
  ;
ui_plot2d_x_item_option /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> setUnit( *($1) );
        delete $1;
      }
  | tLABEL '=' string_const
      {
        configurator -> plotaxisSetLabel( *($3) );
        delete $3;
      }
  | tLABEL '=' tID_STREAM
      {
        configurator -> plotaxisSetLabelStream( *($3) );
        delete $3;
      }
  | tUNIT '=' string_const
      {
        configurator -> setUnit( *($3) );
        delete $3;
      }
  | tUNIT '=' tID_STREAM
      {
        configurator -> plotaxisSetUnitStream( *($3) );
        delete $3;
      }
  | tXANNOTATION '(' ui_xrtgraph_item_anno ')'
      {
        configurator -> withAnnotationOption( true );
      }
  | tINDEX '=' tINT_CONSTANT
    {
       configurator->setPlot2dItemWildcardIndex( ($3) );
    }
  | tXAXIS
      {
        configurator -> setAxisType( Gui2dPlot::XAXIS );
      }
  ;


ui_plot2d_y_item /* DOCUMENTATION:DIAGRAM */
  : ui_field_data_reference opt_scale_factor
      {
        configurator -> plot2dAddDataItemY();
      }
    ui_plot2d_y_item_options
  ;
ui_plot2d_y_item_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_plot2d_y_item_option_list '}'
  ;
ui_plot2d_y_item_option_list /* DOCUMENTATION:UNFOLD */
  : ui_plot2d_y_item_option
  | ui_plot2d_y_item_option_list ',' ui_plot2d_y_item_option
  ;
ui_plot2d_y_item_option /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> setUnit( *($1) );
        delete $1;
      }
  | tLABEL '='  ui_xfer
      {
        configurator -> plotaxisSetLabel( $3 );
      }
  | tLABEL '=' string_const
      {
        configurator -> plotaxisSetLabel( *($3) );
        delete $3;
      }
  | tLABEL '=' tID_STREAM
      {
        configurator -> plotaxisSetLabelStream( *($3) );
        delete $3;
      }
  | tUNIT '=' string_const
      {
        configurator -> setUnit( *($3) );
        delete $3;
      }
  | tUNIT '=' tID_STREAM
      {
        configurator -> plotaxisSetUnitStream( *($3) );
        delete $3;
      }
  | tYAXIS1
      {
        configurator -> setAxisType( Gui2dPlot::Y1AXIS );
      }
  | tYAXIS2
      {
        configurator -> setAxisType( Gui2dPlot::Y2AXIS );
      }
  | tHIDDEN
      {
        configurator -> setAxisType( Gui2dPlot::HIDDEN );
      }
  | tMARKER ui_plot2d_marker_option
      {
        configurator -> setPlot2dItemIsMarker();
      }
  | tCOLOR '=' tID_COLORSET
    {
        configurator->setPlot2dItemColorSetName( *($3) );
        delete $3;
    }
  | tINDEX '=' tINT_CONSTANT
    {
       configurator->setPlot2dItemWildcardIndex( ($3) );
    }
  | tLEGEND '=' tFALSE
    {
       configurator->setPlot2dItemLegendHide(true);
    }
  | tLEGEND '=' tTRUE
    {
       configurator->setPlot2dItemLegendHide(false);
    }
  | tXAXIS
      {
        configurator -> setAxisType( Gui2dPlot::XAXIS );
      }
  | tXANNOTATION '(' ui_xrtgraph_item_anno ')'
      {
        configurator -> withAnnotationOption( true );
      }
  ;

ui_plot2d_marker_option /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '(' ui_field_data_reference ')'
     {
        configurator -> setPlot2dMarker();
     }
  ;

/* --------------------------------------------------------------------------- */
/* UI Navigator                                                                */
/* --------------------------------------------------------------------------- */
ui_navigator_type_option /* DOCUMENTATION:DIAGRAM */
  : /* none */
    { configurator -> setNavigatorType(GuiNavigator::type_Default); }
  | '{' tICONVIEW '}'
    { configurator -> setNavigatorType(GuiNavigator::type_IconView); }
  | '{' tDIAGRAM '}'
    { configurator -> setNavigatorType(GuiNavigator::type_Diagram); }
  ;

ui_navigator_list /* DOCUMENTATION:DIAGRAM */
  : ui_navigator
  | ui_navigator_list ',' ui_navigator
  ;
ui_navigator /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newNavigator( *($1) );
        delete $1;
      }
    ui_navigator_options '(' ui_navigator_root_list ')'
  ;
ui_navigator_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_navigator_option_list '}'
  ;
ui_navigator_option_list /* DOCUMENTATION:DIAGRAM */
  : ui_navigator_option
  | ui_navigator_option_list ',' ui_navigator_option
  ;
ui_navigator_option /* DOCUMENTATION:UNFOLD */
  : tSIZE '(' tINT_CONSTANT ',' tINT_CONSTANT ')'
      {
        configurator -> navigatorSetSize( $3, $5 );
      }
  | tEXPAND
      {
        configurator -> navigatorSetExpandable();
      }
  | tSCROLL
      {
        configurator -> navigatorSetScrollable();
      }
  | tCOL '(' ui_navigator_col_list ')'
  | tTOOLTIP '(' tID_TAG ')'
       {
       configurator -> navigatorSetToolTip( *($3) );
       delete $3;
       }
  | tMULTIPLE_SELECTION
      {
        configurator -> navigatorMultipleSelection();
      }
  | tCOMPARE
      {
        configurator -> navigatorSetCompareMode();
      }
  | tTRANSPARENT '(' ui_navigator_hide_folder_list ')' /* DOCUMENTATION:HIDE (obsolete, only used with tDIFF) */
  ;
ui_navigator_hide_folder_list /* DOCUMENTATION:HIDE BEGIN obsolete, used in tDIFF */
  : ui_navigator_hide_folder
  | ui_navigator_hide_folder_list ',' ui_navigator_hide_folder
  ;

   ui_navigator_hide_folder
   : string_constant { configurator -> addTransparentFolder( *$1 ); }
  ; /* DOCUMENTATION:HIDE END */

ui_navigator_col_list /* DOCUMENTATION:DIAGRAM */
  : ui_navigator_col
  | ui_navigator_col_list ',' ui_navigator_col
  ;
ui_navigator_col /* DOCUMENTATION:UNFOLD */
  : ui_navigator_col_label tID_TAG ui_field_attributes ui_navigator_col_style
      {
        configurator -> navigatorAddColumn( *($2), *($1), (GuiNavigator::ColumnStyle)$4 );
        delete $1;
        delete $2;
      }
  ;
ui_navigator_col_label /* DOCUMENTATION:UNFOLD */
  : /* none */
         { $$ = new std::string(); }
  | string_constant
         { $$ = $1; }
  ;
ui_navigator_col_style /* DOCUMENTATION:UNFOLD */
  : /*  none */
      { $$ = GuiNavigator::columnStyle_Default; }
  | tPIXMAP
      { $$ = GuiNavigator::columnStyle_Pixmap; }
  | tSORT
      { $$ = GuiNavigator::columnStyle_Sort; }
  ;

ui_navigator_root_list /* DOCUMENTATION:DIAGRAM */
  : ui_navigator_root
  | ui_navigator_root_list ',' ui_navigator_root
  ;
ui_navigator_root /* DOCUMENTATION:UNFOLD */
  : ui_field_data_reference ui_navigator_root_label
      {
        configurator -> setRoot( *($2) );
        delete $2;
      }
    ui_navigator_root_options
  ;

ui_navigator_root_label /* DOCUMENTATION:UNFOLD */
  : /* none */
      { $$ = new std::string(); }
  | '=' string_constant
      { $$ = $2; }
  ;
ui_navigator_root_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_navigator_root_option_list '}'
  ;
ui_navigator_root_option_list /* DOCUMENTATION:DIAGRAM */
  : ui_navigator_root_option
  | ui_navigator_root_option_list ',' ui_navigator_root_option
  ;
ui_navigator_root_option /* DOCUMENTATION:UNFOLD */
  : tAUTOLEVEL
      {
        configurator -> setAutoLevel();
      }
  | tHIDE_EMPTY_FOLDER
      {
        configurator -> setHideEmptyFolder();
      }
  | tOPENLEVELS '=' tINT_CONSTANT
      {
        configurator -> setOpenLevels( $3 );
      }
  | tFIRSTLEVEL '=' tINT_CONSTANT
      {
        configurator -> setFirstStructFolder( $3 );
      }
  | tLASTLEVEL '=' tINT_CONSTANT
      {
        configurator -> setLastlevel( $3 );
      }
  ;

/* --------------------------------------------------------------------------- */
/* UI Textwindow Definitions                                                   */
/* --------------------------------------------------------------------------- */

ui_text_window_list /* DOCUMENTATION:DIAGRAM */
  : ui_text_window
  | ui_text_window_list ',' ui_text_window
  ;
ui_text_window /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newGuiScrolledText( *($1) );
        delete $1;
      }
    ui_text_window_options
  ;

/* --------------------------------------------------------------------------- */
/* UI Form Textwindow Options                                                  */
/* --------------------------------------------------------------------------- */

ui_text_window_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_text_window_option_list '}'
  ;
ui_text_window_option_list /* DOCUMENTATION:DIAGRAM */
  : ui_text_window_option
  | ui_text_window_option_list ',' ui_text_window_option
  ;
ui_text_window_option /* DOCUMENTATION:UNFOLD */
  : tSIZE '=' tINT_CONSTANT '*' tINT_CONSTANT
      {
        configurator -> scrolledtextSetSize( $3, $5 );
      }
  | string_constant
      {
        configurator -> scrolledtextSetLabel( (*$1) );
        delete $1;
      }
  | tWRAP
      {
        configurator -> setWordwrap( true );
      }
  | tFORTRAN
      {
        configurator -> setFormatFortran();
      }
  | tFILTER '=' string_constant
      {
        configurator -> scrolledtextSetPrintFilter( (*$3) );
        delete $3;
      }
  ;

/* --------------------------------------------------------------------------- */
/* UI Folder Definitions                                                       */
/* --------------------------------------------------------------------------- */

ui_folder_list /* DOCUMENTATION:DIAGRAM */
  : ui_folder
  | ui_folder_list ',' ui_folder
  ;
ui_folder /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newGuiFolder( (*$1) );
        delete $1;
        i = 0;
      }
    ui_folder_options
    '(' ui_folder_definition_list ')'
      {
        configurator -> testNumberOfPages();
      }
  ;
ui_folder_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_folder_option_list '}'
  ;
ui_folder_option_list /* DOCUMENTATION:DIAGRAM */
  : ui_folder_option
  | ui_folder_option_list ',' ui_folder_option
  ;
ui_folder_option /* DOCUMENTATION:UNFOLD */
  : tTOP
      {
        configurator -> setButtonPlacement( GuiElement::align_Top );
      }
  | tBOTTOM
      {
        configurator -> setButtonPlacement( GuiElement::align_Bottom );
      }
  | tRIGHT
      {
        configurator -> setButtonPlacement( GuiElement::align_Right );
      }
  | tLEFT
      {
        configurator -> setButtonPlacement( GuiElement::align_Left );
      }
  | tHORIZONTAL
      {
        configurator -> setLabelOrientation( GuiElement::orient_Horizontal );
      }
  | tVERTICAL
      {
        configurator -> setLabelOrientation( GuiElement::orient_Vertical );
      }
  | tSTRETCH
      {
        configurator -> setTabStretch();
      }
  | tEXPAND
      {
        configurator -> setTabExpandable();
      }
  | tMOVE
      {
        configurator -> setTabMovable();
      }
  | tBUTTON '=' tNONE
      {
        configurator -> hideFolderButtons();
      }
  | tBUTTON '=' tTRUE
      {
        configurator -> showAlwaysFolderButtons();
      }
  ;
ui_folder_definition_list /* DOCUMENTATION:DIAGRAM */
  : ui_folder_definition
  | ui_folder_definition_list ',' ui_folder_definition
  ;
ui_folder_definition /* DOCUMENTATION:UNFOLD */
  : '[' ui_folder_button ']' '(' ui_form_horizontal_list ')' opt_ui_form_container_options
      {
//        configurator -> uiFolderDefinitionFolderGroup();  (m_rep->i++)
        i++;
      }
  ;
ui_folder_button /* DOCUMENTATION:DIAGRAM */
  : string_constant opt_ui_folder_button_options
      {
        configurator -> addFolderPage( (*$1) );
      }
    opt_ui_folder_group
  ;

opt_ui_folder_button_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ',' ui_folder_button_option_list
  ;
ui_folder_button_option_list /* DOCUMENTATION:DIAGRAM */
  : ui_folder_button_option
  | ui_folder_button_option_list ',' ui_folder_button_option
  ;
ui_folder_button_option /* DOCUMENTATION:UNFOLD */
  : tPIXMAP '=' id_or_string_constant {
      configurator -> setFolderTabIcon( (*$3) );
      }
  | tHIDDEN '=' string_constant {
      configurator -> setFolderTabHidden( (*$3) );
  }
  | tHIDDEN '=' tINT_CONSTANT {
      configurator -> setFolderTabHidden( std::to_string($3) );
  }
  ;

opt_ui_folder_group /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ui_folder_group
  ;
ui_folder_group /* DOCUMENTATION:DIAGRAM */
  : ':' tIDENTIFIER ui_folder_group_function
      {
        configurator -> registerFolderGroupId( *($2) );
        configurator -> addFolderGroupEntry( *($2) );
        delete $2;
      }
  | ':' tID_FOLDERGROUP ui_folder_group_function
      {
        configurator -> addFolderGroupEntry( *($2) );
        delete $2;
      }
  ;
ui_folder_group_function /* DOCUMENTATION:UNFOLD */
  : /* none */
      {
        configurator -> resetFunction();
        $$ = 0;
      }
  | ',' tFUNC '=' job_function_pointer { $$ = $4; }
  ;

/* --------------------------------------------------------------------------- */
/* UI Plugin Definitions                                                       */
/* --------------------------------------------------------------------------- */

ui_plugin_list /* DOCUMENTATION:DIAGRAM */
  : ui_plugin
  | ui_plugin_list ',' ui_plugin
  ;
ui_plugin /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> addNamedElement( *($1) );
      }
    ui_plugin_options
    ui_plugin_fields
  ;

/* --------------------------------------------------------------------------- */
/* UI PLUGIN OPTIONS                                                           */
/* --------------------------------------------------------------------------- */

ui_plugin_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_plugin_option_list '}'
  ;
ui_plugin_option_list /* DOCUMENTATION:UNFOLD */
  : ui_plugin_option
  | ui_plugin_option_list ',' ui_plugin_option
  ;
ui_plugin_option /* DOCUMENTATION:DIAGRAM */
  : tLABEL '=' string_constant
      {
        configurator -> pluginSetLabel( *($3) );
        delete $3;
      }
  | tSTYLESHEET '=' string_constant
      { configurator -> setPluginXslFileName( *($3) );
        delete $3;
      }
  ;

/* --------------------------------------------------------------------------- */
/* UI PLUGIN FIELDS                                                            */
/* --------------------------------------------------------------------------- */

ui_plugin_fields /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '(' ui_plugin_item_list ')'
  ;
ui_plugin_item_list /* DOCUMENTATION:UNFOLD */
  : ui_plugin_line
  | ui_plugin_item_list ',' ui_plugin_line
  ;
ui_plugin_line /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | ui_form_element_identifier
      {
        configurator -> pluginAttachGuiElement( (*$1) );
        delete $1;
      }
      ui_plugin_item_options
  ;
ui_plugin_item_options /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | '{' ui_plugin_item_option_list '}'
  ;
ui_plugin_item_option_list /* DOCUMENTATION:UNFOLD */
  : ui_plugin_item_option
  | ui_plugin_item_option_list ',' ui_plugin_item_option
  ;
ui_plugin_item_option /* DOCUMENTATION:UNFOLD */
  : /* none */
  | tROW '=' tINT_CONSTANT
      { int x,y;
        configurator -> getGuiElement() -> getLayoutPosition( x, y );
        configurator -> getGuiElement() -> setLayoutPosition( x, $3 );
      }
  | tCOL '=' tINT_CONSTANT
      { int x,y;
        configurator -> getGuiElement() -> getLayoutPosition( x, y );
        configurator -> getGuiElement() -> setLayoutPosition( $3, y );
      }
  | tROWSPAN '=' tINT_CONSTANT
      {
        configurator -> getGuiElement() -> setRowSpan( $3 );
      }
  | tCOLSPAN '=' tINT_CONSTANT
      {
        configurator -> getGuiElement() -> setColSpan( $3 );
      }
  ;

/* --------------------------------------------------------------------------- */
/* UI XRT Table                                                                */
/* --------------------------------------------------------------------------- */

ui_table_list /* DOCUMENTATION:DIAGRAM */
  : ui_table
  | ui_table_list ',' ui_table
  ;
ui_table /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newGuiTable( *($1) );
        delete $1;
      }
    ui_tbl_options
    '(' ui_tbl_part_list ')'
  ;

/* --------------------------------------------------------------------------- */
/* UI XRT Table Options                                                        */
/* --------------------------------------------------------------------------- */

ui_tbl_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' ui_tbl_option_list '}'
  ;
ui_tbl_option_list /* DOCUMENTATION:UNFOLD */
  : ui_tbl_option
  | ui_tbl_option_list ',' ui_tbl_option
  ;
ui_tbl_option /* DOCUMENTATION:DIAGRAM */
  : string_constant ui_field_alignment
      {
        configurator -> setTitleTable( *($1), $2 );
        delete $1;
      }
  | tJUSTLEFT
      {
        configurator -> setLabelAlignment( GuiElement::align_Left );
      }
  | tJUSTRIGHT
      {
        configurator -> setLabelAlignment( GuiElement::align_Right );
      }
  | tJUSTCENTER
      {
        configurator -> setLabelAlignment( GuiElement::align_Center );
      }
  | tORIENTATION '=' ui_orientation
      {
        configurator -> setTableOrientation( $3 );
      }
  | tNAVIGATION '=' ui_orientation
      {
        configurator -> setTableNavigation( $3 );
      }
  | tHORIZONTAL
      {
        configurator -> newTableSize();
      }
    '(' ui_tbl_size_options ')'
      {
        configurator -> setTableColSize();
      }
  | tVERTICAL
      {
        configurator -> newTableSize();
      }
    '(' ui_tbl_size_options ')'
      {
        configurator -> setTableRowSize();
      }
  | tMENU '=' tHIDDEN
      {
        configurator -> hideTableIndexMenu();
      }
  | tFUNC '=' job_function_pointer
      {
        configurator -> setTableFunction();
      }
  | tAUTO_WIDTH
      {
        configurator -> setTableAutoWidth();
      }
  ;
ui_tbl_size_options /* DOCUMENTATION:DIAGRAM */
  : ui_tbl_size_option
  | ui_tbl_size_options ',' ui_tbl_size_option
  ;
ui_tbl_size_option /* DOCUMENTATION:UNFOLD */
  : tTABLESIZE '=' tINT_CONSTANT
      {
        configurator -> tablesizeSetSize( $3 );
      }
  | tRANGE '(' tINT_CONSTANT ui_tbl_range_max ')'
      {
        configurator -> tablesizeSetRange( $3, $4 );
      }
  | tHIDDEN
      { configurator -> tablesizeHideHeader();
      }
  ;
ui_tbl_range_max /* DOCUMENTATION:UNFOLD */
  : /* none */           { $$ = 0;  }
  | ',' tINT_CONSTANT    { $$ = $2; }
  ;
opt_ui_tbl_horizontal_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ui_tbl_horizontal_options
  ;
ui_tbl_horizontal_options /* DOCUMENTATION:UNFOLD */
  : '{' string_constant '}'
      {
        configurator -> setTitleTableHorizontal( *($2) );
        delete $2;
      }
  ;
opt_ui_tbl_vertical_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ui_tbl_vertical_options
  ;
ui_tbl_vertical_options /* DOCUMENTATION:UNFOLD */
  : '{' string_constant '}'
      {
        configurator -> setTitleTableVertical( *($2) );
        delete $2;
      }
  ;
ui_tbl_part_list /* DOCUMENTATION:UNFOLD */
  : ui_tbl_part
  | ui_tbl_part_list ui_tbl_part
  ;
ui_tbl_part /* DOCUMENTATION:DIAGRAM */
  : tTOP opt_ui_tbl_horizontal_options
      {
        configurator -> setHorizontalPlacement( GuiElement::align_Top );
      }
    '(' ui_tbl_horizontal_definition ')' ';'
  | tBOTTOM opt_ui_tbl_horizontal_options
      {
        configurator -> setHorizontalPlacement( GuiElement::align_Bottom );
      }
    '(' ui_tbl_horizontal_definition ')' ';'
  | tLEFT opt_ui_tbl_vertical_options
      {
        configurator -> setVerticalPlacement( GuiElement::align_Left );
      }
    '(' ui_tbl_vertical_definition ')' ';'
  | tRIGHT opt_ui_tbl_vertical_options
      {
        configurator -> setVerticalPlacement( GuiElement::align_Right );
      }
    '(' ui_tbl_vertical_definition ')' ';'
  | tTABLE
      {
        configurator -> setTable();
      }
    '(' ui_tbl_table_definition ')' ';'
  ;

/* --------------------------------------------------------------------------- */
/* UI XRT Table Horizontal Lines                                               */
/* --------------------------------------------------------------------------- */

ui_tbl_horizontal_definition /* DOCUMENTATION:UNFOLD */
  : /* empty */
  | ui_tbl_horizontal_line_list
  ;
ui_tbl_horizontal_line_list /* DOCUMENTATION:DIAGRAM */
  : ui_tbl_horizontal_line
  | ui_tbl_horizontal_line_list ',' ui_tbl_horizontal_line
  ;
ui_tbl_horizontal_line /* DOCUMENTATION:UNFOLD */
  : string_constant ui_field_alignment
      {
        configurator -> addHorizontalLine( *($1), $2 );
        delete $1;
      }
    ui_tbl_line_definition
  ;
ui_tbl_line_definition /* DOCUMENTATION:UNFOLD */
  : ui_tbl_line_item_list
      {
        configurator -> tablelineCheck();
      }
  ;
ui_tbl_line_item_list /* DOCUMENTATION:UNFOLD */
  : ui_tbl_line_item
  | ui_tbl_line_item_list ui_tbl_line_item
  ;
ui_tbl_line_item /* DOCUMENTATION:DIAGRAM */
  : tVOID
      {
        configurator -> addTableLabelItem( "", GuiElement::align_Default );
      }
  | string_constant ui_field_alignment
      {
        configurator -> addTableLabelItem( (*$1), $2 );
        delete $1;
      }
  | ui_field_data_reference ui_field_attributes
      {
        configurator -> addTableDataItem();
      }
    ui_set_field_attributes
  ;

/* --------------------------------------------------------------------------- */
/* UI XRT Table Vertical Lines                                                 */
/* --------------------------------------------------------------------------- */

ui_tbl_vertical_definition /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ui_tbl_vertical_line_list
  ;
ui_tbl_vertical_line_list /* DOCUMENTATION:SYNONYM ui_tbl_horizontal_line_list */
  : ui_tbl_vertical_line
  | ui_tbl_vertical_line_list ',' ui_tbl_vertical_line
  ;
ui_tbl_vertical_line /* DOCUMENTATION:UNFOLD */
  : string_constant ui_field_alignment
      {
        configurator -> addVerticalLine( (*$1), $2 );
        delete $1;
      }
    ui_tbl_line_definition
  ;

/* --------------------------------------------------------------------------- */
/* UI XRT Table Matrix Lines                                                   */
/* --------------------------------------------------------------------------- */

ui_tbl_table_definition /* DOCUMENTATION:UNFOLD */
  : ui_tbl_table_line_list
  | ui_tbl_table_matrix
  ;
ui_tbl_table_line_list /* DOCUMENTATION:DIAGRAM */
  : ui_tbl_table_line
  | ui_tbl_table_line_list ',' ui_tbl_table_line
  ;
ui_tbl_table_line /* DOCUMENTATION:UNFOLD */
  : string_constant ui_field_alignment
      {
        configurator -> addTableLine( (*$1), $2 );
        delete $1;
      }
     ui_tbl_line_definition
  | tTOOLTIP
      {
        configurator -> addTableLineToolTip();
      }
     ui_tbl_line_definition
  | tCOLOR      {
        configurator -> addTableLineColor();
      }
     ui_tbl_line_definition
  ;
ui_tbl_table_matrix /* DOCUMENTATION:DIAGRAM */
  : ui_field_data_reference ui_field_attributes
      {
        configurator -> addTableDataItemMatrix();
      }
    ui_set_field_attributes
  ;

/* --------------------------------------------------------------------------- */
/* UI Menu Definitions                                                         */
/* --------------------------------------------------------------------------- */

ui_menu_list /* DOCUMENTATION:DIAGRAM */
  : ui_menu_declaration
  | ui_menu_list ',' ui_menu_declaration
  ;
ui_menu_declaration /* DOCUMENTATION:UNFOLD */
  : ui_form_menu_declaration
  | tOPEN ui_open_menu_declaration   { $$ = 0; }
  | tSAVE ui_save_menu_declaration   { $$ = 0; }
  | tPRINT ui_print_menu_declaration { $$ = 0; }
  | ui_form_element_menu_declaration { $$ = 0; }
  | ui_structure_menu_declaration    { $$ = 0; }
  | ui_plot2d_menu_declaration       { $$ = 0; }
  ;

/* --------------------------------------------------------------------------- */
/* UI List Menu Definitions                                               */
/* --------------------------------------------------------------------------- */

ui_form_element_menu_declaration /* DOCUMENTATION:UNFOLD */
  : ui_form_element_identifier
      {
        configurator -> newFormElementMenu( *($1) );
        delete $1;
      }
    '(' ui_form_element_menu_button_list ')'
  ;
ui_form_element_menu_button_list /* DOCUMENTATION:DIAGRAM */
  : ui_form_element_menu_button
  | ui_form_element_menu_button_list ',' ui_form_element_menu_button
;
ui_form_element_menu_button /* DOCUMENTATION:UNFOLD */
  : tSEPARATOR
      {
        configurator -> newSeparator();
      }
  | tFUNC function_reference menu_button_label
      {
        configurator -> addFunctionMenuButton( (*$2), (*$3) );
        delete $2;
        delete $3;
      }
  ;

/* --------------------------------------------------------------------------- */
/* UI Structure Menu Definitions                                               */
/* --------------------------------------------------------------------------- */

ui_structure_menu_declaration /* DOCUMENTATION:UNFOLD */
  : tID_DATASTRUCTURE ui_structure_menu_option
      {
        configurator -> newStructureMenu( *($1), $2 );
        delete $1;
      }
    '(' ui_structure_menu_button_list ')'
  | tID_DATASTRUCTURE ui_structure_drop_menu_option
      {
        configurator -> newStructureDropMenu( *($1) );
        delete $1;
      }
    '(' ui_structure_menu_button_list ')'
  ;
ui_structure_menu_button_list /* DOCUMENTATION:DIAGRAM */
  : ui_structure_menu_button
  | ui_structure_menu_button_list ',' ui_structure_menu_button
;
ui_structure_menu_button /* DOCUMENTATION:UNFOLD */
  : tSEPARATOR
      {
        configurator -> newSeparator();
      }
  | tFUNC function_reference menu_button_label
      {
        configurator -> addFunctionNavigatorMenuButton( (*$2), (*$3) );
        delete $2;
        delete $3;
      }
  ;

ui_structure_menu_option /* DOCUMENTATION:DIAGRAM */
  : /* none */              { $$ = 0;  }
  |  '{' tINDEX '(' tINT_CONSTANT ')' '}'         { $$ = $4; }
  ;

ui_structure_drop_menu_option /* DOCUMENTATION:DIAGRAM */
  : '{' tDROP '}'
  ;

/* --------------------------------------------------------------------------- */
/* UI Plot2d Menu Definitions                                                  */
/* --------------------------------------------------------------------------- */

ui_plot2d_menu_declaration /* DOCUMENTATION:UNFOLD */
  : tPLOT2D { configurator -> newPlot2dMenuDescription();
            }
    '(' ui_plot2d_menu_entry_list ')'
  ;
ui_plot2d_menu_entry_list /* DOCUMENTATION:DIAGRAM */
  : ui_plot2d_menu_entry
  | ui_plot2d_menu_entry_list ',' ui_plot2d_menu_entry
;
ui_plot2d_menu_entry /* DOCUMENTATION:UNFOLD */
  : tSTRING_CONSTANT
    { configurator -> addPlot2dMenuEntry( *$1 );
      delete $1;
    }
  ;

/* --------------------------------------------------------------------------- */
/* UI FORM Menu Definitions                                                    */
/* --------------------------------------------------------------------------- */

ui_form_menu_declaration /* DOCUMENTATION:UNFOLD */
  : menu_menu_declaration '(' ui_form_menu_button_list ')'
  ;
ui_form_submenu_declaration /* DOCUMENTATION:UNFOLD */
  : menu_submenu_declaration '(' ui_form_menu_button_list ')'
      {
        configurator -> getParentMenu();
      }
  ;
ui_form_menu_button_list /* DOCUMENTATION:DIAGRAM */
  : ui_form_menu_button
  | ui_form_menu_button_list ',' ui_form_menu_button
  ;
ui_form_menu_button /* DOCUMENTATION:UNFOLD */
  : tSEPARATOR
      {
        configurator -> newSeparator();
      }
  | tFORM tID_FORM menu_button_label ui_form_menu_button_toggle
      {
        configurator -> addFormMenuButton( (*$2), (*$3), $4 );
        delete $2;
        delete $3;
      }
  | tFUNC function_reference menu_button_label
      {
        configurator -> addFunctionMenuButton( (*$2), (*$3) );
        delete $2;
        delete $3;
      }
  | tMENU ui_form_submenu_declaration
  ;
ui_form_menu_button_toggle /* DOCUMENTATION:UNFOLD */
  : /* none */ { $$ = false; }
  | tTOGGLE    { $$ = true; }
  ;

/* --------------------------------------------------------------------------- */
/* UI FILE Menu Definitions                                                    */
/* --------------------------------------------------------------------------- */

ui_open_menu_declaration /* DOCUMENTATION:UNFOLD */
  : /* OPEN */ ui_file_menu_option
      {
        configurator -> getOpenMenu( $1 );
      }
    '(' ui_file_menu_button_list ')'
      {
        configurator -> resetPulldownmenu();
      }
  ;
ui_save_menu_declaration /* DOCUMENTATION:SYNONYM ui_open_menu_declaration */
  : /* SAVE */ ui_file_menu_option
      {
        configurator -> getSaveMenu( $1 );
      }
    '(' ui_file_menu_button_list ')'
      {
        configurator -> resetPulldownmenu();
      }
  ;
ui_file_menu_option /* DOCUMENTATION:UNFOLD */
  : /* none */         { $$ = true; }
  | '{' tEXPLICIT '}'  { $$ = false; }
  ;
ui_file_menu_declaration /* DOCUMENTATION:UNFOLD */
  : menu_submenu_declaration '(' ui_file_menu_button_list ')'
      {
        configurator -> getParentMenu();
      }
  ;
ui_file_menu_button_list /* DOCUMENTATION:DIAGRAM */
  : ui_file_menu_button
  | ui_file_menu_button_list ',' ui_file_menu_button
  ;
ui_file_identifier /* DOCUMENTATION:UNFOLD */
  : tID_FILESTREAM
  | tID_STREAM
  | tID_TEXTWINDOW
  ;
ui_file_menu_button /* DOCUMENTATION:UNFOLD */
  : tSEPARATOR
      {
        configurator -> newSeparator();
      }
  | ui_file_identifier menu_button_label
      {
        configurator -> createFileButton( *$1, *$2 );
        delete $1;
        delete $2;
      }
  | tFUNC function_reference menu_button_label
      {
        configurator -> addFunctionMenuButton( (*$2), (*$3) );
        delete $2;
        delete $3;
      }
  | tSTD_WINDOW menu_button_label
      {
        configurator -> createFileButton( "STD_WINDOW", *$2 );
        delete $2;
      }
  | tLOG_WINDOW menu_button_label
      {
        configurator -> createFileButton( "LOG_WINDOW", *$2 );
        delete $2;
      }
  | tMENU ui_file_menu_declaration
  ;

/* --------------------------------------------------------------------------- */
/* UI PRINT Menu Definitions                                                   */
/* --------------------------------------------------------------------------- */

ui_print_menu_declaration /* DOCUMENTATION:UNFOLD */
  : /* PRINT */  ui_file_menu_option
      {
        configurator -> getHardcopyFolder( $1 );
      }
    '(' ui_print_menu_button_list ')'
      {
        configurator -> resetPrintfolder();
      }
  ;
ui_print_submenu_declaration /* DOCUMENTATION:UNFOLD */
  : id_or_string_constant
      {
        configurator -> addHardcopyFolder( *($1) );
        delete $1;
      }
    '(' ui_print_menu_button_list ')'
      {
        configurator -> getParentFolder();
      }
  ;
ui_print_menu_button_list /* DOCUMENTATION:DIAGRAM */
  : ui_print_menu_button
  | ui_print_menu_button_list ',' ui_print_menu_button
  ;
ui_print_menu_button /* DOCUMENTATION:UNFOLD */
  : tSEPARATOR
      {
        configurator -> printfolderAddSeparator();
      }
  | report_identifier menu_button_label
      {
        configurator -> addObject( *($1), *($2) );
        delete $1;
        delete $2;
      }
  | tMENU ui_print_submenu_declaration
  ;

/* *************************************************************************** */
/* OPERATOR description                                                        */
/* *************************************************************************** */

operator_description /* DOCUMENTATION:DIAGRAM */
  : tOPERATOR
      {
         configurator->beginOperator();
      }
    op_declarations
    tEND tOPERATOR ';'
  ;
op_declarations /* DOCUMENTATION:UNFOLD */
  : op_declaration_list
  ;

op_declaration_list /* DOCUMENTATION:UNFOLD */
  : op_declaration
  | op_declaration_list op_declaration
  ;
op_declaration /* DOCUMENTATION:UNFOLD */
  : op_element_declaration ';'
      {
        configurator -> dispatchPendingEvents();
      }
  ;
op_element_declaration /* DOCUMENTATION:DIAGRAM */
  : tPROCESS      op_process_declaration_list
  | tPROCESSGROUP op_processgroup_declaration_list
  | tSTREAM       op_stream_declaration_list          /* DOCUMENTATION:HIDE (old syntax) */
  | tFILESTREAM   op_filestream_declaration_list
  | tREPORTSTREAM op_reportstream_declaration_list
  | tLATEXREPORT  op_latexreport_declaration_list
  | tTASK         job_task_list
  | tMESSAGE_QUEUE op_message_queue_declaration_list
  | tMENU         op_menu_list
  | tSOCKET       op_socket_declaration_list
  | tTIMER        op_timer_declaration_list
  ;

/* --------------------------------------------------------------------------- */
/* MESSAGE_QUEUE                                                               */
/* --------------------------------------------------------------------------- */

op_message_queue_declaration_list /* DOCUMENTATION:DIAGRAM */
  : op_message_queue_declaration
  | op_message_queue_declaration_list ',' op_message_queue_declaration
  ;

op_message_queue_declaration /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER '{' op_message_queue_option_list '}'
    {
      configurator->newMsgQueue( *$1 );
      delete $1;
    }
  ;
op_message_queue_option_list /* DOCUMENTATION:UNFOLD */
  : op_message_queue_option
  | op_message_queue_option_list ',' op_message_queue_option
  ;

op_message_queue_option /* DOCUMENTATION:DIAGRAM */
  : tPUBLISH
    { configurator -> transferData_setType(MessageQueue::type_Publisher); }
  | tSUBSCRIBE
    { configurator -> transferData_setType(MessageQueue::type_Subscriber); }
  | tREQUEST
    { configurator -> transferData_setType(MessageQueue::type_Request); }
  | tREPLY
    { configurator -> transferData_setType(MessageQueue::type_Reply); }
  | tPORT '=' tINT_CONSTANT
      {
        configurator -> transferData_setPort( $3 );
      }
  | tPORT_REQUEST '=' tINT_CONSTANT
      {
        configurator -> transferData_setPortRequest( $3 );
      }
  | tHOST '=' tSTRING_CONSTANT
      {
        configurator -> transferData_setHost( *($3) );
        delete $3;
      }
  | tHOST '=' tINT_CONSTANT /* DOCUMENTATION:HIDE */
      {
        configurator -> transferData_setHost( std::to_string($3) );
      }
  | tTIMEOUT '=' tINT_CONSTANT
      {
        configurator -> transferData_setTimeout( $3 );
      }
 | tFUNC '=' function_reference  /* used only by mq reply as default arg */
      {
        configurator -> transferData_setFunc( *($3) );
        delete $3;
      }
  | tREQUEST '(' op_message_queue_stream_list ')'  /* used only by mq reply as default arg */
      {
        configurator -> transferData_setInStreams();
      }
  | tRESPONSE '(' op_message_queue_stream_list ')'  /* used only by mq reply as default arg */
      {
        configurator -> transferData_setOutStreams();
      }
  | '(' op_message_queue_header_option_list ')'
      {
        configurator -> msgQueueHeader_append();
      }
  | tNO_DEPENDENCIES
      {
        configurator -> transferData_setNoDependencies();
      }
  ;

op_message_queue_header_option_list /* DOCUMENTATION:UNFOLD */
  : op_message_queue_header_option
  | op_message_queue_header_option_list ',' op_message_queue_header_option
  ;

op_message_queue_header_option /* DOCUMENTATION:DIAGRAM */
  : tHEADER '=' tSTRING_CONSTANT
      {
        configurator -> msgQueueHeader_setHeader( *($3) );
        delete $3;
      }
  | tHEADER '=' tINT_CONSTANT /* DOCUMENTATION:HIDE */
      {
        configurator -> msgQueueHeader_setHeader( std::to_string($3) );
      }
  | tREQUEST '=' tID_STREAM
      {
        configurator -> msgQueueHeader_setInStream( *($3), false );
        delete $3;
      }
  | tREQUEST '(' op_message_queue_stream_list ')'
      {
        configurator -> msgQueueHeader_setInStreams();
      }
  | tRESPONSE '=' tID_STREAM
      {
        configurator -> msgQueueHeader_setOutStream( *($3), false );
        delete $3;
      }
  | tRESPONSE '(' op_message_queue_stream_list ')'
      {
        configurator -> msgQueueHeader_setOutStreams();
      }
  | tSTREAM '=' tID_STREAM
      {
        configurator -> msgQueueHeader_setOutStream( *($3), false );
        delete $3;
      }
  | tSTREAM '(' op_message_queue_stream_list ')'
      {
        configurator -> msgQueueHeader_setOutStreams();
      }
  | tFUNC '=' function_reference
      {
        configurator -> msgQueueHeader_setFunc( *($3) );
        delete $3;
      }
  | tPLUGIN '=' tID_PLUGIN
    { configurator -> msgQueueHeader_setPluginId( *($3) );
      delete $3;
    }
  | tPLUGIN '(' op_message_queue_plugin_list ')'
      {
        configurator -> msgQueueHeader_setOutPlugins();
      }
  ;

op_message_queue_stream_list /* DOCUMENTATION:UNFOLD */
  : op_message_queue_stream
  | op_message_queue_stream_list ',' op_message_queue_stream
  ;
op_message_queue_stream /* DOCUMENTATION:UNFOLD */
  : tID_STREAM
    {
      configurator -> transferData_addStream( *($1), false );
      delete $1;
    }
  ;

op_message_queue_plugin_list /* DOCUMENTATION:UNFOLD */
  : op_message_queue_plugin
  | op_message_queue_plugin_list ',' op_message_queue_plugin
  ;
op_message_queue_plugin /* DOCUMENTATION:UNFOLD */
  : tID_PLUGIN
    {
      configurator -> transferData_addPlugin( *($1) );
      delete $1;
    }
  ;

/* --------------------------------------------------------------------------- */
/* SOCKET                                                                      */
/* --------------------------------------------------------------------------- */

op_socket_declaration_list /* DOCUMENTATION:DIAGRAM */
  : op_socket_declaration
  | op_socket_declaration_list ',' op_socket_declaration
  ;

op_socket_declaration /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER '{' op_socket_option_list '}'
    { configurator->newSocket( *$1 );
      delete $1;
    }
  ;
op_socket_option_list /* DOCUMENTATION:UNFOLD */
  : op_socket_option
  | op_socket_option_list ',' op_socket_option
  ;

op_socket_option /* DOCUMENTATION:DIAGRAM */
  : tPORT '=' tINT_CONSTANT
      {
        configurator -> socket_setPort( $3 );
      }
  | tSTREAM '=' tID_STREAM
      {
        configurator -> socket_setInStream( *($3) );
      }
  | tFUNC '=' function_reference
      {
        configurator -> socket_setFunc( *($3) );
      }
  | tMFM
      {
        configurator -> socket_setMFM();
      }
  | tON_EOS '=' function_reference
      {
        configurator -> socket_setOnEos( *($3) );
      }
  | tON_VIEW_ACTION '=' function_reference
      {
        configurator -> socket_setOnViewAction( *($3) );
      }
  | ui_xfer
      {
        configurator -> socket_setDataref( $1 );
      }
  | tTHUMBNAIL '(' ui_xfer ',' tINT_CONSTANT ',' tINT_CONSTANT ')'
      {
        configurator -> socket_setThumbnail( $3, $5, $7 );
      }
  | '(' op_socket_header_option_list ')'
      {
        configurator -> socketRequest_append();
      }
  ;

op_socket_header_option_list /* DOCUMENTATION:UNFOLD */
  : op_socket_header_option
  | op_socket_header_option_list ',' op_socket_header_option
  ;

op_socket_header_option /* DOCUMENTATION:DIAGRAM */
  : tHEADER '=' tSTRING_CONSTANT
      {
        configurator -> socketRequest_setHeader( *($3) );
        delete $3;
      }
  | tHEADER '=' tINT_CONSTANT /* DOCUMENTATION:HIDE */
      {
        configurator -> socketRequest_setHeader( std::to_string($3) );
      }
  | tREQUEST '=' tID_STREAM
      {
        configurator -> socketRequest_setInStream( *($3) );
      }
  | tRESPONSE '=' tID_STREAM
      {
        configurator -> socketRequest_setOutStream( *($3) );
      }
  | tFUNC '=' function_reference
      {
        configurator -> socketRequest_setFunc( *($3) );
      }
  ;

/* --------------------------------------------------------------------------- */
/* TIMER                                                                       */
/* --------------------------------------------------------------------------- */

op_timer_declaration_list /* DOCUMENTATION:DIAGRAM */
  : op_timer_declaration
  | op_timer_declaration_list ',' op_timer_declaration
  ;

op_timer_declaration /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER '{' op_timer_option_list '}'
    {
        configurator->newTimer( *$1 );
        delete $1;
    }
  ;

op_timer_option_list /* DOCUMENTATION:UNFOLD */
  : op_timer_option
  | op_timer_option_list ',' op_timer_option
  ;

op_timer_option /* DOCUMENTATION:DIAGRAM */
  : tFUNC '=' job_function_pointer
  | tMAX_PENDING_FUNCTIONS '=' tINT_CONSTANT
    {
        configurator -> timer_setMaxPendingFunctions( $3 );
    }
  ;

/* --------------------------------------------------------------------------- */
/* PROCESS                                                                     */
/* --------------------------------------------------------------------------- */

op_process_declaration_list /* DOCUMENTATION:DIAGRAM */
  : op_process_declaration
  | op_process_declaration_list ',' op_process_declaration
  ;
op_process_declaration /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER ':' tBATCH '{' op_exe_command '}'
      {
        configurator -> newBatchProcess( (*$1), (*$5) );
        delete $1;
        delete $5;
      }
  | tIDENTIFIER ':' tDAEMON '{' op_exe_command '}'
      {
        configurator -> newDaemonProcess( (*$1), (*$5) );
        delete $1;
        delete $5;
      }
  | tIDENTIFIER ':' tMATLAB
      {
        configurator -> newMatlabProcess( *($1) );
        delete $1;
      }
  ;
op_exe_command /* DOCUMENTATION:UNFOLD */
  : op_exe_command_list
      { $$ = $1; }
  ;
op_exe_command_list /* DOCUMENTATION:UNFOLD */
  : op_exe_cmd
      { $$ = $1; }
  | op_exe_command_list ',' op_exe_cmd
      { *($1) += *($3);
        delete $3;
        $$ = $1;
      }
  ;
op_exe_cmd /* DOCUMENTATION:UNFOLD */
  : string_constant
      {
        $$ = $1;
      }
  | tIDENTIFIER
      {
        $$ = configurator -> fifoCreateName( *($1) );
        delete $1;
      }
  ;

/* --------------------------------------------------------------------------- */
/* PROCESSGROUP                                                                */
/* --------------------------------------------------------------------------- */

op_processgroup_declaration_list /* DOCUMENTATION:DIAGRAM */
  : op_processgroup_declaration
  | op_processgroup_declaration_list ',' op_processgroup_declaration
  ;
op_processgroup_declaration /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newProcessGroup( *($1) );
        delete $1;
      }
    op_processgroup_options '(' op_process_statement_list ')'
      {
        configurator -> addProcessActionButton();
      }
  ;
op_processgroup_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' op_processgroup_option_list '}'
  ;
op_processgroup_option_list /* DOCUMENTATION:DIAGRAM */
  : op_processgroup_option
  | op_processgroup_option_list ',' op_processgroup_option
  ;
op_processgroup_option /* DOCUMENTATION:UNFOLD */
  : string_constant
      {
        configurator -> processgroupSetLabel( (*$1) );
        delete $1;
      }
  | tFORM '=' tNONE
      {
        configurator -> processgroupUnsetForm();
      }
  | tFORM '=' tID_FORM
      {
        configurator -> processgroupSetForm( (*$3) );
        delete $3;
      }
  | tHIDDEN
      {
        configurator -> processgroupSetMenuInstalled();
      }
  | tHELPTEXT '=' string_constant /* DOCUMENTATION:HIDE (not implemented in intens4) */
      {
        configurator -> processgroupSetHelptext( *($3) );
        delete $3;
      }
  | tSILENT
      {
        configurator -> processgroupSetSilent();
      }
  | tNO_LOG
      {
        configurator -> processgroupSetLogOff();
      }
  | tUI_UPDATE '(' tINT_CONSTANT ')'
      {
        configurator -> setUiUpdate( $3 );
      }
  | tNO_DEPENDENCIES
      {
        configurator -> processgroupNoDependencies();
      }
  | tAUTOCLEAR_DEPENDENCIES
      {
        configurator -> processgroupAutoClearDependencies();
      }
  ;
op_process_statement_list /* DOCUMENTATION:UNFOLD */
  : op_process_statement
  | op_process_statement_list op_process_statement
  ;
op_process_statement /* DOCUMENTATION:DIAGRAM */
  : op_process_lhs tID_PROCESS
      {
        configurator -> setOutputBasicStreams( (*$2) );
        delete $2;
      }
    '(' opt_op_input_stream_list ')' ';'
      {
        configurator -> setTargets();
      }
  ;
op_process_lhs /* DOCUMENTATION:UNFOLD */
  : /* empty */
     {
       configurator -> resetStream();
     }
  | op_output_stream_list '='
  ;

op_output_stream_list /* DOCUMENTATION:DIAGRAM */
  : op_output_stream
  | op_output_stream_list ',' op_output_stream
  | '['
      {
        configurator -> newStream();
      }
    op_stream_parameters ']'
      {
        configurator -> registerBasicStream();
      }
  ;

op_stream_parameters /* DOCUMENTATION:UNFOLD */
  : op_stream_parameter
  | op_stream_parameters ',' op_stream_parameter
  ;
op_stream_parameter /* DOCUMENTATION:DIAGRAM */
  : st_opt_flag st_data_reference opt_scale_factor field_conversion
      {
        configurator -> addParameter( $1 );
      }
  | tMATRIX st_opt_flag st_data_reference opt_scale_factor field_conversion
      {
        configurator -> addMatrixParameter( $2, '\0' );
      }
  | string_constant st_field_length
      {
        configurator -> addToken( *$1, $2 );
      }
  | '#' st_field_length
      {
        configurator -> addIndexParameter( "#", $2 );
      }
  | '#' tIDENTIFIER st_field_length
      {
        configurator -> addIndexParameter( std::string("#")+*$2, $3 );
        delete $2;
      }
  | tDATASET_TEXT '(' st_data_reference ')'
    { configurator -> addDatasetParameter();
    } st_field_length
  | tSTRING_DATE '(' st_data_reference st_field_length ')'
    {   configurator -> addStringDateParameter(UserAttr::string_kind_date, $4);
    }
  | tSTRING_TIME '(' st_data_reference st_field_length ')'
    {   configurator -> addStringDateParameter(UserAttr::string_kind_time, $4);
    }
  | tSTRING_DATETIME '(' st_data_reference st_field_length ')'
    {   configurator -> addStringDateParameter(UserAttr::string_kind_datetime, $4);
    }
  | tSTRING_VALUE '(' st_data_reference st_field_length ')'
    { configurator -> addStringDateParameter(UserAttr::string_kind_value, $4);
    }
  | tEOLN
      {
        configurator -> addToken( "\n", 1);
      }
  | tID_INDEX
    {
      configurator -> addGuiIndexParameter( *($1) );
      delete $1;
    }
  | '('
      {
        configurator -> stBeginGroup();
      }
    op_stream_parameters ')'
      {
        configurator -> stEndGroup();
      }
  ;
op_output_stream /* DOCUMENTATION:DIAGRAM */
  : tID_STREAM opt_op_option_fifo
      {
        configurator -> getStream( (*$1), (*$2) );
        delete $1;
        delete $2;
      }
    op_output_stream_options
  | tID_FIELDGROUP
      {
        configurator -> getStreamableGuiElement( (*$1) );
        delete $1;
      }
  | tID_UNIPLOT
      {
        configurator -> getSimpelPlot( (*$1) );
        delete $1;
      }
  | tID_PSPLOT
      {
        configurator -> getPSStream( (*$1) );
        delete $1;
      }
  ;

op_output_stream_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' op_output_stream_option_list '}'
  ;
op_output_stream_option_list /* DOCUMENTATION:UNFOLD */
  : op_output_stream_option
  | op_output_stream_option_list ',' op_output_stream_option
  ;
op_output_stream_option /* DOCUMENTATION:DIAGRAM */
  : tDISPLAY '=' op_textwindow_name
      {
        configurator -> setTextWindow( (*$3) );
        delete $3;
      }
  | tFORMAT '=' op_stream_file_format
      {
        std::cerr << "Parser warning: FORMAT spec ignored" << std::endl;
      }
  ;
op_stream_file_format /* DOCUMENTATION:UNFOLD */
  : tASCII           { $$ = tASCII; }
  | tBINARY          { $$ = tBINARY; }
  ;
op_textwindow_name /* DOCUMENTATION:UNFOLD */
  : tNONE            { $$ = new std::string();  }
  | tID_TEXTWINDOW   { $$ = $1; }
  ;
opt_op_input_stream_list /* DOCUMENTATION:UNFOLD */
  : /* none */
  | op_input_stream_list
  |   {
        configurator -> addNewInputStream();
      }
    op_stream_parameters
  ;
op_input_stream_list /* DOCUMENTATION:UNFOLD */
  : op_input_stream
  | op_input_stream_list ',' op_input_stream
  ;
op_input_stream /* DOCUMENTATION:DIAGRAM */
  : tID_STREAM opt_op_option_fifo
      {
        configurator -> addInputStream( (*$1), (*$2) );
        delete $1;
        delete $2;
      }
  ;
opt_op_option_fifo /* DOCUMENTATION:UNFOLD */
  : /* none */
      {
        configurator -> getStandardWindow();
        $$ = new std::string;
      }
  | '[' op_option_fifo ']'
      {
        $$ =  $2;
      }
  ;
op_option_fifo /* DOCUMENTATION:UNFOLD */
  : string_constant
      {
        configurator -> resetScrolledtext();
        $$ =  $1;
      }
  | identifier
      {
        $$ = configurator -> fifoCreateName( *($1) );
        configurator -> resetScrolledtext();
      }
  ;
/* STREAM (old Syntax)
   ------
*/
op_stream_declaration_list /* DOCUMENTATION:HIDE BEGIN (old syntax) */
  : op_stream_declaration
  | op_stream_declaration_list ',' op_stream_declaration
  ;
op_stream_declaration
  : tID_STREAM ':' tFILESTREAM
      {
        configurator -> opOLDfilestream( *$1 );
        delete $1;
      }
    op_filestream_options
      {
        configurator -> opOLDfilestreamOptions();
      }
  | tID_STREAM ':' tREPORTSTREAM
      {
        configurator -> opOLDreportstream( *$1 );
        delete $1;
      }
    op_reportstream_options
  ; /* DOCUMENTATION:HIDE END */

/* FILESTREAM
   ----------
*/
op_filestream_declaration_list /* DOCUMENTATION:DIAGRAM */
  : op_filestream_declaration
  | op_filestream_declaration_list ',' op_filestream_declaration
  ;
op_filestream_declaration /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER '=' tID_STREAM
      {
        configurator -> newFileStream( (*$1), *$3 );
        delete $1;
        delete $3;
      }
    op_filestream_options
      {
        configurator -> filestreamInstall();
      }
  ;
op_filestream_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' op_filestream_option_list '}'
  ;
op_filestream_option_list /* DOCUMENTATION:UNFOLD */
  : op_filestream_option
  | op_filestream_option_list ',' op_filestream_option
  ;
op_filestream_option /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> filestreamSetLabel( (*$1) );
        delete $1;
      }
  | tFILTER '=' string_constant
      {
        configurator -> setFilter( (*$3) );
        delete $3;
      }
  | tPROCESS '=' string_constant
      {
        configurator -> setProcess( (*$3) );
        delete $3;
      }
  | tRESET
      {
        configurator -> setReset();
      }
  | tREADONLY
      {
        configurator -> setReadOnly();
      }
  | tWRITEONLY
      {
        configurator -> setWriteOnly();
      }
  | tNO_LOG
      {
        configurator -> filestreamSetLogOff();
      }
  | tHIDDEN
      {
        configurator -> setHidden();
      }
  | tBINARY
      {
        configurator -> setBinaryMode();
      }
  | tEXTENSION '=' string_constant
      {
        configurator -> setExtension( (*$3) );
        delete $3;
      }
  | tDIRNAME '=' string_constant
      {
        configurator -> filestream_SetDirname( (*$3) );
        delete $3;
      }
  ;
/* REPORTSTREAM
   ------------
*/
op_reportstream_declaration_list /* DOCUMENTATION:DIAGRAM */
  : op_reportstream_declaration
  | op_reportstream_declaration_list ',' op_reportstream_declaration
  ;
op_reportstream_declaration /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER '=' tID_STREAM
      {
        configurator -> newReportStream( *($1), (*$3) );
        delete $1;
        delete $3;
      }
    op_reportstream_options
      {
        configurator -> installReportStream();
      }
  ;
op_reportstream_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' op_reportstream_option_list '}'
  ;
op_reportstream_option_list /* DOCUMENTATION:UNFOLD */
  : op_reportstream_option
  | op_reportstream_option_list ',' op_reportstream_option
  ;
op_reportstream_option /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> setTitleReportstream( *($1) );
        delete $1;
      }
  | tLATEX
      {
        configurator -> setLatexFlag();
      }
  | tFILTER '=' string_constant
      {
        configurator -> setPrintFilter( (*$3) );
        delete $3;
      }
  | tXML
      {
        configurator -> setXmlFileFormat();
      }
  | tSTYLESHEET '=' string_constant
      {
        configurator -> setXslFileName( (*$3) );
      }
  | tTEMPLATE '=' string_constant
      {
        configurator -> setTemplateFileName( (*$3) );
      }
  | tDIRNAME '=' string_constant
      {
        configurator -> reportstream_SetDirname( (*$3) );
        delete $3;
      }
  ;

/* LATEXREPORT
   ------------
*/
op_latexreport_declaration_list /* DOCUMENTATION:DIAGRAM */
  : op_latexreport_declaration
  | op_latexreport_declaration_list ',' op_latexreport_declaration
  ;
op_latexreport_declaration /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER '=' tID_STREAM
      {
        configurator -> newReportStream( *($1), (*$3) );
        delete $1;
        delete $3;
        configurator -> setLatexFlag();
      }
    op_latexreport_options
      {
        configurator -> installReportStream();
      }
  ;
op_latexreport_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' op_latexreport_option_list '}'
  ;
op_latexreport_option_list /* DOCUMENTATION:UNFOLD */
  : op_latexreport_option
  | op_latexreport_option_list ',' op_latexreport_option
  ;
op_latexreport_option /* DOCUMENTATION:DIAGRAM */
  : string_constant
      {
        configurator -> setTitleReportstream( *($1) );
        delete $1;
      }
  | tFILTER '=' string_constant
      {
        configurator -> setPrintFilter( (*$3) );
        delete $3;
      }
  | tHIDDEN
      {
        configurator -> setHiddenReportstream();
      }
  ;


/* --------------------------------------------------------------------------- */
/* OP Process Menu Definitions                                                 */
/* --------------------------------------------------------------------------- */

op_menu_list /* DOCUMENTATION:DIAGRAM */
  : op_menu_declaration
  | op_menu_list ',' op_menu_declaration
  ;
op_menu_declaration /* DOCUMENTATION:UNFOLD */
  : menu_menu_declaration '(' op_menu_button_list ')'
  ;
op_submenu_declaration /* DOCUMENTATION:UNFOLD */
  : menu_submenu_declaration '(' op_menu_button_list ')'
      {
        configurator -> getParentMenu();
      }
  ;
op_menu_button_list /* DOCUMENTATION:DIAGRAM */
  : op_menu_button
  | op_menu_button_list ',' op_menu_button
  ;
op_menu_button /* DOCUMENTATION:UNFOLD */
  : tSEPARATOR
      {
        configurator -> newSeparator();
      }
  | tPROCESS tID_PROCESSGROUP menu_button_label
      {
        configurator -> addProcessMenuButton( (*$2), *($3) );
        delete $2;
        delete $3;
      }
  | tPROCESS tID_TASK menu_button_label
      {
        configurator -> addTaskMenuButton( (*$2), *($3) );
        delete $2;
        delete $3;
      }
  | tMENU op_submenu_declaration
  ;


/* *************************************************************************** */
/* JOB description                                                             */
/* *************************************************************************** */

functions_description /* DOCUMENTATION:DIAGRAM */
  : tFUNCTIONS
      {
        configurator->beginJob();
      }
    job_function_declaration_list
    tEND tFUNCTIONS ';'
      {
        //cout<<"end Functions"<<endl;
      }
  ;
job_function_declaration_list /* DOCUMENTATION:UNFOLD */
  : job_function_declaration
  | job_function_declaration_list job_function_declaration
  ;
job_function_declaration /* DOCUMENTATION:DIAGRAM */
  : tFUNC
      {
        configurator -> setFuncSilent( false );
        configurator -> setFuncUpdateForms( true );
        configurator -> func_setHighPriority( false );
      }
       job_options job_function_list ';'
  ;

/* --------------------------------------------------------------------------- */
/* Function Definitions                                                        */
/* --------------------------------------------------------------------------- */

job_function_list /* DOCUMENTATION:UNFOLD */
  : job_function
  | job_function_list ',' job_function
  ;

job_function /* DOCUMENTATION:DIAGRAM */
  : tIDENTIFIER '{'
      {
        configurator -> beginFunc(*($1));
        configurator -> newFunction( *($1), true, true );   // bool registerId, bool setNamespace
        delete $1;
      }
    job_local_variables job_single_statement_list '}'
      {
        configurator -> getAndResetNamespace();
        configurator -> endFunc();
      }

  | tID_FUNCTION '{'
      {
        configurator -> beginFunc(*($1));
        configurator -> newFunction( *($1), false, true );   // bool registerId, bool setNamespace
        delete $1;
      }
     job_local_variables job_single_statement_list '}'
      {
        configurator -> getAndResetNamespace();
        configurator -> endFunc();
      }
  | tIDENTIFIER
      {
        configurator -> beginFunc(*($1));
        configurator -> newFunction( *($1), true, false );   // bool registerId, bool setNamespace
        delete $1;
        configurator -> endFunc();
      }

  ;

job_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' job_option_list '}'
  ;

job_option_list /* DOCUMENTATION:UNFOLD */
  : job_option
  | job_option_list ',' job_option
  ;

job_option /* DOCUMENTATION:DIAGRAM */
  : tSILENT
    {
      configurator -> setFuncSilent( true );
    }
  | tDEADLY_SILENT /* DOCUMENTATION:HIDE BEGIN (deprecated, use SILENT instead) */
    {
      configurator -> setFuncSilent( true );
      std::cerr << "DEADLY_SILENT is deprecated. Please use SILENT instead." << std::endl;
    } /* DOCUMENTATION:HIDE END */
  | tUPDATE_FORMS '=' tNONE
    {
      configurator -> setFuncUpdateForms( false );
    }
  | tPRIORITY '=' tHIGH
    {
      configurator->func_setHighPriority( true );
    }
  | tWEBAPI '=' temp_data_reference /* DOCUMENTATION:HIDE (WebApi) */
    {
    configurator->setFuncWebResponse($3);
    }
  | tPROTO
    {
      configurator->setFuncWebResponseProto();
    }
  ;

job_function_pointer /* DOCUMENTATION:DIAGRAM */
  : tIDENTIFIER
      {
        configurator -> cppComment("//4 FUNC="+*($1));
        configurator -> newFunction( *($1), true, false );   // bool registerId, bool setNamespace
        delete $1;
        $$=configurator->getFunction();
      }
  | tID_FUNCTION
      {
        configurator -> cppComment("//5 FUNC="+*($1));
        configurator -> getFunction( *($1) );
        delete $1;
        $$=configurator->getFunction();
      }
  ;

/* --------------------------------------------------------------------------- */
/* Task Definitions                                                            */
/* --------------------------------------------------------------------------- */

job_task_list /* DOCUMENTATION:DIAGRAM */
  : job_task
  | job_task_list ',' job_task
  ;

job_task /* DOCUMENTATION:UNFOLD */
  : tIDENTIFIER
      {
        configurator -> newTask( *($1), true );   // bool registerId
        delete $1;
      }
    job_task_declaration

  | tID_TASK
      {
        configurator -> newTask( *($1), false );   // bool registerId
        delete $1;
      }
    job_task_declaration
      {
        if( configurator -> isNOTTaskImplemented() )
          ParserError( compose(_("Task '%1' already declared."), $1) );
      }
  ;

job_task_declaration /* DOCUMENTATION:UNFOLD */
  : /* none */
  | job_task_options '{'
      {
        configurator -> setNamespace();
      }
    job_local_variables job_single_statement_list '}'
      {
        configurator -> addTaskActionButton();
      }
  ;

job_task_options /* DOCUMENTATION:DIAGRAM */
  : '{' opt_job_task_option_list '}'
  ;

opt_job_task_option_list /* DOCUMENTATION:UNFOLD */
  : /* none */
  | job_task_option_list
  ;
job_task_option_list /* DOCUMENTATION:UNFOLD */
  : job_task_option
  | job_task_option_list ',' job_task_option
  ;

job_task_option /* DOCUMENTATION:UNFOLD */
  : string_constant
      {
        configurator -> taskSetLabel( *($1) );
        delete $1;
      }
  | tFORM '=' tNONE
      {
        configurator -> taskUnsetForm();
      }
  | tFORM '=' tID_FORM
      {
        configurator -> taskSetForm( *($3) );
        delete $3;
      }
  | tHIDDEN
      {
        configurator -> taskSetMenuInstalled();
      }
  | tHELPTEXT '=' string_constant /* DOCUMENTATION:HIDE (not implemented in intens4) */
      {
        configurator -> taskSetHelptext( *($3) );
        delete $3;
      }
  | tNO_LOG
      {
        configurator -> taskSetLogOff();
      }
  | tSILENT
      {
        configurator -> taskSetSilent();
      }
  ;

/* --------------------------------------------------------------------------- */
/* Job local Variables                                                         */
/* --------------------------------------------------------------------------- */

job_local_variables /* DOCUMENTATION:DIAGRAM */
  : /* none */
  |
    data_V2_simple_variables_list
  ;

/* --------------------------------------------------------------------------- */
/* Job Data Definition                                                         */
/* --------------------------------------------------------------------------- */


job_data_reference /* DOCUMENTATION:DIAGRAM */
  : job_data_identifier job_data_identifier_options
      {
        $$ = $1;
      }
  | tDATASET_TEXT '(' job_dataset_identifier job_data_identifier_options ')'
      {
        $$ = $3;
      }
  | tSTRING_DATE '(' job_stringdate_identifier job_data_identifier_options ')'
    {   $$ = $3;
    }
  | tSTRING_TIME '(' job_stringtime_identifier job_data_identifier_options ')'
    {   $$ = $3;
    }
  | tSTRING_DATETIME '(' job_stringdatetime_identifier job_data_identifier_options ')'
    {   $$ = $3;
    }
  | tSTRING_VALUE '(' job_stringvalue_identifier job_data_identifier_options ')'
    {   $$ = $3;
    }
  | tCURRENT_TIME
    {
        configurator -> opCurrentIsoTime();
        $$=configurator->getFunctionNextAddress();
    }
  | tCURRENT_DATE
    {
        configurator -> opCurrentIsoDate();
        $$=configurator->getFunctionNextAddress();
    }
  | tCURRENT_DATETIME
    {
        configurator -> opCurrentIsoDatetime();
        $$=configurator->getFunctionNextAddress();
    }
  | tINPUT
      {
        $$ = configurator -> opPushNewValue();
      }
  | job_data_source job_data_source_idx opt_job_data_struct_reference_list
      { configurator -> opSetSource();
        $$ = $1;
      }
  | job_data_source2 opt_job_data_struct_reference_list
      { configurator -> opSetSource2();
        $$ = $1;
      }
  | job_data_this opt_job_data_struct_reference_list
      {
        configurator -> opSetThis();
        $$ = $1;
      }
  | job_data_base opt_job_data_struct_reference_list
      {
        configurator -> opSetBase();
        $$ = $1;
      }
  ;
job_data_identifier_options /* DOCUMENTATION:DIAGRAM */
  : job_data_indizes opt_job_data_struct_reference_list
    job_data_push_variable job_data_cycle_number
  ;
job_data_push_variable /* DOCUMENTATION:EMPTY */
  :   {
        configurator -> opPushVariable();
      }
  ;
job_data_cycle_number /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '@' '[' job_expression ']'
      {
        configurator -> opSetCycleNumber();
      }
  ;
job_data_identifier /* DOCUMENTATION:UNFOLD */
  : data_identifier
      {
        configurator -> newJobDataReference( *($1) );
        $$=configurator->getFunctionNextAddress();
        delete $1;
      }
  | tVAR '(' job_next_address job_new_var_identifier ')'
      {
        $$ = $3;
      }
  | tPARENT '(' job_next_address job_new_parent_var_identifier ')'
      {
        $$ = $3;
      }
  ;
job_dataset_identifier /* DOCUMENTATION:UNFOLD */
  : data_identifier
      {
        bool dataset = true;
        configurator -> newJobDataReference( *($1), dataset );
        $$=configurator->getFunctionNextAddress();
        delete $1;
      }
  ;
job_stringdate_identifier /* DOCUMENTATION:SYNONYM job_dataset_identifier */
  : data_identifier
      {
        configurator -> newJobDataReference( *($1), false, UserAttr::string_kind_date );
        $$=configurator->getFunctionNextAddress();
        delete $1;
      }
  ;
job_stringdatetime_identifier /* DOCUMENTATION:SYNONYM job_dataset_identifier */
  : data_identifier
      {
        configurator -> newJobDataReference( *($1), false, UserAttr::string_kind_datetime );
        $$=configurator->getFunctionNextAddress();
        delete $1;
      }
  ;
job_stringtime_identifier /* DOCUMENTATION:SYNONYM job_dataset_identifier */
  : data_identifier
      {
        configurator -> newJobDataReference( *($1), false, UserAttr::string_kind_time );
        $$=configurator->getFunctionNextAddress();
        delete $1;
      }
  ;
job_stringvalue_identifier /* DOCUMENTATION:SYNONYM job_dataset_identifier */
  : data_identifier
      {
        configurator -> newJobDataReference( *($1), false, UserAttr::string_kind_value );
        $$=configurator->getFunctionNextAddress();
        delete $1;
      }
  ;

job_data_this /* DOCUMENTATION:UNFOLD */
  : tTHIS
      {
        configurator -> newJobVarDataReference();
        $$=configurator->getFunctionNextAddress();
      }
  ;
job_data_source /* DOCUMENTATION:UNFOLD */
  : tSOURCE
      {
        configurator -> newJobVarDataReference();
        $$=configurator->getFunctionNextAddress();
      }
  ;
job_data_source_idx /* DOCUMENTATION:UNFOLD */
  :  { $$=0; } /* none */
  | '[' data_identifier ']' /* tINT_CONSTANT */
      {
        configurator -> newJobDataReference2( *($2) );
      $$=0;
      }
  ;
job_data_source2 /* DOCUMENTATION:UNFOLD */
  : tSOURCE2
      {
        configurator -> newJobVarDataReference();
        $$=configurator->getFunctionNextAddress();
      }
  ;
job_data_base /* DOCUMENTATION:UNFOLD */
  : tBASE
      {
        configurator -> newJobVarDataReference();
        $$=configurator->getFunctionNextAddress();
      }
  ;
job_data_expression /* DOCUMENTATION:UNFOLD */
  :
      {
        configurator -> pushDataRef();
      }
    job_expression
      {
        configurator -> popDataRef();
      }
  ;
job_new_var_identifier /* DOCUMENTATION:UNFOLD */
  :
      {
        configurator -> jobdatarefAddVariableName();
      }
    job_data_expression
  ;
job_new_parent_var_identifier /* DOCUMENTATION:UNFOLD */
  :
      {
        configurator -> jobdatarefAddVariableName(true);
      }
    job_data_expression
  ;
opt_job_data_struct_reference_list /* DOCUMENTATION:UNFOLD */
  : /* none */
  | job_data_struct_reference_list
  ;
job_data_struct_reference_list /* DOCUMENTATION:DIAGRAM */
  : job_data_struct_reference
  | job_data_struct_reference_list job_data_struct_reference
  ;
job_data_struct_reference /* DOCUMENTATION:UNFOLD */
  : '.' job_data_struct_identifier
      {
        configurator -> incrementDatalevel();
      }
    job_data_indizes
  ;
job_data_struct_identifier /* DOCUMENTATION:UNFOLD */
  : identifier
      {
        configurator -> extendDataReference( *($1) );
        delete $1;
      }
  | tVAR '(' job_data_expression ')'
      {
        configurator -> convertToVar();
      }
  ;
job_data_indizes /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | '[' job_data_index_list ']'
  ;
job_data_index_list /* DOCUMENTATION:UNFOLD */
  : job_data_index
  | job_data_index_list ',' job_data_index
  ;
job_data_index /* DOCUMENTATION:UNFOLD */
  :
      {
        configurator -> addIndex();
      }
    job_data_expression
  | '*'
      {
        configurator -> setDataWildcard();
      }
  ;

/* --------------------------------------------------------------------------- */
/* Job Strings                                                                 */
/* --------------------------------------------------------------------------- */

job_string_expression /* DOCUMENTATION:DIAGRAM */
  : job_string_list
  ;

job_string_list /* DOCUMENTATION:UNFOLD */
  : job_string
  | job_string_list '&' job_string
      {
        configurator -> opAdd();
      }
  ;
job_string /* DOCUMENTATION:UNFOLD */
  : tSTRING_CONSTANT
      {
        configurator -> convertString( *($1) );
        $$ = configurator -> opPushString( *($1) );
        delete $1;
      }
  | tUNIT '(' job_data_reference ')'
      {
       configurator -> opPushUnits();
       $$ = $3;
      }
  | tLABEL '(' job_data_reference ')'
      {
       configurator -> opPushLabel();
       $$ = $3;
      }

  ;

/* --------------------------------------------------------------------------- */
/* Statements                                                                  */
/* --------------------------------------------------------------------------- */

job_statement /* DOCUMENTATION:DIAGRAM */
  : '{' job_single_statement_list '}'
      {
        $$ = $2;
      }
  | job_single_statement
  ;
job_single_statement_list /* DOCUMENTATION:UNFOLD */
  : job_single_statement
  | job_single_statement_list job_single_statement
  ;
job_single_statement /* DOCUMENTATION:DIAGRAM */
  : data_statement
  | file_statement
  | set_statement
  | gui_statement
  | cycle_statement
  | print_statement
  | error_statement
  | exit_statement
  | system_statement
  | messagebox_statement
  | run_statement
  | return_statement
  | send_statement
  | message_queue_statement
  | timer_statement
  | database_statement
  | if_statement
  | while_statement
  | copy_paste_statement
  | set_func_statement
  | restService_statement
  ;

data_statement /* DOCUMENTATION:DIAGRAM */
  : job_data_reference job_assignments ';'
      {
        $$ = $1;
      }
  | tCLEAR job_clear ';'
      {
        $$ = $2;
      }
  | tERASE job_erase ';'
      {
        $$ = $2;
      }
  | tPACK job_pack ';'
      {
        $$ = $2;
      }
  | tSIZE job_size ';'
      {
        $$ = $2;
      }
  | tDATA_SIZE job_data_size ';'
      {
        $$ = $2;
      }
  | tINDEX job_index ';'
      {
        $$ = $2;
      }
  | tCOMPARE '(' job_compare ')' ';'
      {
        $$ = $3;
      }
  | tASSIGN_CORR job_assign_corresponding ';'
      {
        $$ = $2;
      }
  | tSET_RESOURCE '(' job_set_resource  ')' ';'
      {
        $$ = $3;
      }
  | tCOMPOSE '(' job_compose ')' ';'
      {
        $$ = $3;
      }
  | tASSIGN_CONSISTENCY job_assign_consistency ';'
      {
        configurator -> opActionResult();
        $$ = $2;
      }
  | tCLASS '(' job_class ')' ';'
      {
        $$ = $3;
      }
  ;

file_statement /* DOCUMENTATION:DIAGRAM */
  : tOPEN '(' job_open_statement ')' ';'
      {
        $$ = $3;
      }
  | tLOAD '(' job_xml_query_statement ')' ';' /* DOCUMENTATION:HIDE (not implemented in intens4) */
      {
        $$ = $3;
      }
  | tLOAD '(' job_description_statement ')' ';'
      {
        $$ = $3;
      }
  | tSAVE '(' job_save_statement ')' ';'
      {
        $$ = $3;
      }
  | tSAVE '(' report_save_identifier ')' ';'
      {
        $$ = configurator -> opExecuteJobActionReport( ReportGen::SAVE, *($3) );
        delete $3;
      }
  | tSERIALIZE '(' ui_form_element_identifier ',' tXML ',' job_file_option ')' ';'
     {
        configurator -> setStreamXMLFlag();
        $$ = configurator -> opSerializeGuiElement( *($3) );
     }
  | tSERIALIZE '(' ui_form_element_identifier ',' tJSON ',' job_file_option ')' ';'
     {
        configurator -> setStreamJSONFlag();
        $$ = configurator -> opSerializeGuiElement( *($3) );
     }
  | tSERIALIZE_FORM '('  job_serialize_statement ')'  ';'
     {
        $$ = configurator -> opSerializeGui();
     }
  | tSERIALIZE_FORM job_form_identifier ';'
     {
        $$ = configurator -> opSerializeForm( *($2) );
     }
  | tWRITE_SETTINGS ';'
      {
        $$ = configurator -> opWriteSettings();
      }
  ;
set_statement /* DOCUMENTATION:DIAGRAM */
  : tSET '(' job_set_attributes ')' ';'
      {
        $$ = $3;
      }
  | tUNSET '(' job_unset_attributes ')' ';'
      {
        $$ = $3;
      }
  | tTOUCH '(' job_ref_with_wildcards job_data_reference ')' ';'
      {
        $$ = $3;
        configurator -> opPushInteger( 1 ); /* true */
        configurator -> opSetDataTimestamp();
      }
  ;

gui_statement /* DOCUMENTATION:DIAGRAM */
  : tUPDATE_FORMS ';'
      {
        $$ = configurator -> opUpdateForms();
      }
  | tUI_UPDATE job_update ';'
      {
        $$ = $2;
      }
  | tMAP job_map ';'
      {
        $$ = $2;
      }
  | tUNMAP job_unmap ';'
      {
        $$ = $2;
      }
  | tALLOW job_allow ';'
      {
        $$ = $2;
      }
  | tDISALLOW job_disallow ';'
      {
        $$ = $2;
      }
  | tENABLE job_enable ';'
      {
        $$ = $2;
      }
  | tDISABLE job_disable ';'
      {
        $$ = $2;
      }
  | tDISABLE_DRAG job_disable_drag ';'
      {
        $$ = $2;
      }
  | tID_INDEX '=' job_expression ';'
      {
        configurator -> opAssignIndex( *($1) );
        $$ = $3;
      }
  | gui_more_statement /* split gui_statement diagram */
      {
        $$ = $1;
      }
  | tSET_CURRENT_FORM job_set_current_form_by_webapi ';'  /* DOCUMENTATION:HIDE BEGIN (only wepapi) */
      {
        $$ = $2;
      } /* DOCUMENTATION:HIDE END */
  ;

job_set_current_form_by_webapi  /* DOCUMENTATION:HIDE BEGIN (only wepapi) */
  : '(' job__set_current_form_element ')' { $$ = $2; }
  ;
job__set_current_form_element
  : temp_data_reference
      {
        $$ = configurator -> opSetCurrentFormByWebApi( $1 );
      }
  ; /* DOCUMENTATION:HIDE END */

gui_more_statement /* DOCUMENTATION:DIAGRAM */
  : tRANGE '(' tID_FIELDGROUP ',' job_expression ',' job_expression ')' ';'
      {
        configurator -> opSetGuiFieldgroupRange( *($3) );
        $$ = $5;
      }
  | tSELECT_LIST job_select_list ';'
      {
        $$ = $2;
      }
  | tGET_SELECTION job_get_selection ';'
      {
        $$ = $2;
      }
  | tCLEAR_SELECTION job_clear_selection ';'
      {
        $$ = $2;
      }
  | tGET_SORT_CRITERIA job_get_sort_criteria ';'
      {
        $$ = $2;
      }
  | tSTYLESHEET job_set_stylesheet ';'
      {
        $$ = $2;
      }
  | tREPLACE ui_form_element_identifier tBY ui_form_element_identifier ';'
      {
        $$ = configurator -> opReplaceGuiElement( *($2), *($4) );
      }
  ;

cycle_statement /* DOCUMENTATION:DIAGRAM */
  : tNEWCYCLE ';'
      {
        $$ = configurator -> opPushInvalid();
        configurator -> opNewCycle();
      }
  | tNEWCYCLE '(' job_expression ')' ';'
      {
        $$ = $3;
        configurator -> opNewCycle();
      }
  | tDELETECYCLE ';'
      {
        $$ = configurator -> opDeleteCycle();
      }
  | tCLEARCYCLE ';'
      {
        $$ = configurator -> opClearCycle();
      }
  | tFIRSTCYCLE ';'
      {
        $$ = configurator -> opFirstCycle();
      }
  | tLASTCYCLE ';'
      {
        $$ = configurator -> opLastCycle();
      }
  | tNEXTCYCLE ';'
      {
        $$ = configurator -> opNextCycle();
      }
  | tGOCYCLE '(' job_expression ')' ';'
      {
        configurator -> opGoCycle();
        $$ = $3;
      }
  | tCYCLENAME job_cyclenumber '=' job_expression ';'
      {
        configurator -> opSetCycleName();
        $$ = $2;
      }
  ;
print_statement /* DOCUMENTATION:DIAGRAM */
  : tPRINT       job_print_elements ';'
      {
        $$ = $2;
      }
  | tSET_MSG job_message_elements ';'
      {
        $$ = $2;
      }
  | tLOG '(' job_log_level ',' job_log_elements ')' ';'
      {
      configurator -> opLog( *($3) );
      $$ = $5;
      delete $3;
      }
  | tREPORT '(' report_identifier ')' ';'
      {
        $$ = configurator -> opExecuteJobActionReport( ReportGen::PRINT, *($3) );
        delete $3;
      }
  | tPREVIEW '(' report_identifier ')' ';'
      {
        $$ = configurator -> opExecuteJobActionReport( ReportGen::PREVIEW, *($3) );
        delete $3;
      }
  | tPRINT_LOG ';'
      {
        $$ = configurator -> opPrintLog();
      }
  ;
error_statement /* DOCUMENTATION:DIAGRAM */
  : tSET_ERROR   job_error_elements ';'
      {
        $$ = $2;
      }
  | tABORT       job_error_elements ';'
      {
        $$ = $2;
        configurator -> opAbort();
      }
  | tRESET_ERROR ';'
      {
        $$ = configurator -> opResetError();
      }
  ;
exit_statement /* DOCUMENTATION:DIAGRAM */
  : tEXIT ';'
      {
        $$ = configurator -> opExitIntens();
      }
  | tEXIT '(' bool_constant ')'  ';'
      {
        $$ = configurator -> opExitIntens($3);
      }
  ;
system_statement /* DOCUMENTATION:DIAGRAM */
  : tBEEP ';'
      {
        configurator -> opBeep();
      }
  ;
messagebox_statement /* DOCUMENTATION:DIAGRAM */
  : tMESSAGEBOX '(' job_expression ')' ';'
      {
        $$ = $3;
        configurator -> opMessageBox();
      }
  ;
  | tMESSAGEBOX '(' job_expression ',' job_expression ')' ';'
      {
        $$ = $3;
        configurator -> opMessageBox(true);
      }
  ;
run_statement /* DOCUMENTATION:DIAGRAM */
  : tRUN '(' job_run_statement ')' ';'
      {
        $$ = $3;
      }
  ;
return_statement /* DOCUMENTATION:DIAGRAM */
  : tRETURN ';'
      {
        $$ = configurator -> opEndOfFunction();
      }
  ;

send_statement /* DOCUMENTATION:DIAGRAM */
  : send_action ';'
  ;

send_action /* DOCUMENTATION:DIAGRAM */
  : tSEND '(' job_socket_option_list ')'
      {
        $$ = configurator -> opSend();
      }
  | tID_STREAM '=' tSEND '(' job_socket_option_list ')'
      {
        configurator -> socket_setInStream( *($1) );
        $$ = configurator -> opSend();
        delete $1;
      }
  ;
job_socket_option_list /* DOCUMENTATION:UNFOLD */
  : job_socket_option
  | job_socket_option_list ',' job_socket_option
  ;
job_socket_option /* DOCUMENTATION:UNFOLD */
  : tHOST '=' job_socket_host
  | tPORT '=' job_socket_port
  | tHEADER '=' tSTRING_CONSTANT
      {
        configurator -> socket_setHeader( *($3) );
        delete $3;
      }
  | tHEADER '=' tINT_CONSTANT /* DOCUMENTATION:HIDE */
      {
        configurator -> socket_setHeader( std::to_string($3) );
      }
  | tSTREAM '=' tID_STREAM
      {
        configurator -> socket_setOutStream( *($3) );
        delete $3;
      }
  ;
job_socket_host /* DOCUMENTATION:UNFOLD */
  : tSTRING_CONSTANT
    {
        configurator -> socket_setHost( *($1) );
        delete $1;
    }
  | tINT_CONSTANT /* DOCUMENTATION:HIDE */
    {
        configurator -> socket_setHost( std::to_string($1) );
    }
  | ui_xfer
    {
        configurator -> socket_setHostXfer();
    }
  ;
job_socket_port /* DOCUMENTATION:UNFOLD */
  : tINT_CONSTANT
    {
        configurator -> socket_setPort( $1 );
    }
  | ui_xfer
    {
        configurator -> socket_setPortXfer();
    }
  ;

message_queue_statement /* DOCUMENTATION:DIAGRAM */
  : message_queue_action ';'
  ;

message_queue_action /* DOCUMENTATION:DIAGRAM */
  : tREQUEST '(' job_message_queue_options ')'
      {
        $$ = configurator -> opRequest();
      }
  | tPUBLISH '(' job_message_queue_options ')'
      {
        $$ = configurator -> opPublish();
      }
  | tREQUEST '(' job_plugin_options ')'
      {
        $$ = configurator -> opRequest();
      }
  | tSUBSCRIBE '(' job_plugin_options ')'
      {
        $$ = configurator -> opSubscribe();
      }
  | tSET_MQ_HOST '(' job_expression ',' job_expression ')'
      {
        $$ = configurator -> opSetMessageQueueHost();
      }
  ;
job_message_queue_options /* DOCUMENTATION:UNFOLD */
  : tMESSAGE_QUEUE '=' tID_MESSAGE_QUEUE
      {
        configurator -> targets_clearTargetStreams();
        configurator -> transferData_setMessageQueue( *($3) );
        delete $3;
      }
    job_message_queue_option_list
  ;
job_message_queue_option_list /* DOCUMENTATION:UNFOLD */
  : ',' job_message_queue_option
  | job_message_queue_option_list ',' job_message_queue_option
  ;
job_message_queue_option /* DOCUMENTATION:DIAGRAM */
  : tREQUEST '=' tID_STREAM job_message_queue_dependencies
      {
        configurator -> transferData_setInStream( *($3), $4 );
        delete $3;
      }
  | tREQUEST '(' job_message_queue_stream_list ')'
      {
        configurator -> transferData_setInStreams();
      }
  | tRESPONSE '=' tID_STREAM job_message_queue_dependencies
      {
        configurator -> transferData_setOutStream( *($3), $4 );
        delete $3;
      }
  | tRESPONSE '(' job_message_queue_stream_list ')'
      {
        configurator -> transferData_setOutStreams();
      }
  | tHEADER '=' tSTRING_CONSTANT
      {
        configurator -> transferData_setHeader( *($3) );
        delete $3;
      }
  | tHEADER '=' tINT_CONSTANT /* DOCUMENTATION:HIDE */
      {
        configurator -> transferData_setHeader( std::to_string($3) );
      }
  | tTIMEOUT '=' tINT_CONSTANT
      {
        configurator -> transferData_setTimeout( $3 );
      }
  | tAUTOCLEAR_DEPENDENCIES
      {
        configurator -> targets_setAutoClear();
      }
  | tNO_DEPENDENCIES
      {
        configurator -> targets_noDependencies();
      }
  ;

job_message_queue_stream_list /* DOCUMENTATION:UNFOLD */
  : job_message_queue_stream
  | job_message_queue_stream_list ',' job_message_queue_stream
  ;
job_message_queue_stream /* DOCUMENTATION:UNFOLD */
  : tID_STREAM job_message_queue_dependencies
    {
      configurator -> transferData_addStream( *($1), $2 );
      delete $1;
    }
  ;

job_message_queue_dependencies  /* DOCUMENTATION:UNFOLD */
  : /* none */
    { $$ = -1; }
  | '(' tDEPENDENCIES ')'
    { $$ = 1; /* true */ }
  | '(' tNO_DEPENDENCIES ')'
    { $$ = 0; /* false */ }
  ;

job_plugin_options /* DOCUMENTATION:UNFOLD */
  : tPLUGIN '=' tID_PLUGIN
      {
        configurator -> transferData_setPlugin( *($3) );
        delete $3;
      }
    job_plugin_option_list
  ;
job_plugin_option_list /* DOCUMENTATION:UNFOLD */
  : ',' job_plugin_option
  | job_plugin_option_list ',' job_plugin_option
  ;
job_plugin_option /* DOCUMENTATION:DIAGRAM */
  : tREQUEST '=' tID_STREAM job_message_queue_dependencies
      {
        configurator -> transferData_setInStream( *($3), $4 );
        delete $3;
      }
  | tREQUEST '(' job_message_queue_stream_list ')'
      {
        configurator -> transferData_setInStreams();
      }
  | tRESPONSE '=' tID_STREAM job_message_queue_dependencies
      {
        configurator -> transferData_setOutStream( *($3), $4 );
        delete $3;
      }
  | tRESPONSE '(' job_message_queue_stream_list ')'
      {
        configurator -> transferData_setOutStreams();
      }
  | tHEADER '=' tSTRING_CONSTANT
      {
        configurator -> transferData_setHeader( *($3) );
        delete $3;
      }
  | tHEADER '=' tINT_CONSTANT /* DOCUMENTATION:HIDE */
      {
        configurator -> transferData_setHeader( std::to_string($3) );
      }
  | tFUNC '=' function_reference
      {
        configurator -> transferData_setFunc( *($3) );
        delete $3;
      }
  ;


timer_statement /* DOCUMENTATION:DIAGRAM */
  : tSTART '(' job_timer_timer job_timer_options ')' ';'
      {
        $$ = configurator -> getFunctionNextAddress();
        configurator -> opTimerStart();
      }
  | tSTOP '(' job_timer_timer ')' ';'
      {
        $$ = configurator -> getFunctionNextAddress();
        configurator -> opTimerStop();
      }
  ;
job_timer_timer /* DOCUMENTATION:UNFOLD */
  : tID_TIMER
      {
        configurator -> timer_setTimer( *($1) );
      }
  ;
job_timer_options /* DOCUMENTATION:DIAGRAM */
  : /* none */
  | ',' job_timer_option_list
  ;
job_timer_option_list /* DOCUMENTATION:UNFOLD */
  : job_timer_option
  | job_timer_option_list ',' job_timer_option
  ;
job_timer_option /* DOCUMENTATION:UNFOLD */
  : tPERIOD '=' job_timer_period
  | tDELAY '=' job_timer_delay
  ;
job_timer_period /* DOCUMENTATION:UNFOLD */
  : tINT_CONSTANT
    {
        configurator -> timer_setPeriod( $1 );
    }
  | ui_xfer
    {
        configurator -> timer_setPeriodXfer();
    }
  ;
job_timer_delay /* DOCUMENTATION:SYNONYM job_timer_period */
  : tINT_CONSTANT
    {
        configurator -> timer_setDelay( $1 );
    }
  | ui_xfer
    {
        configurator -> timer_setDelayXfer();
    }
  ;

database_statement /* DOCUMENTATION:DIAGRAM */
  : tBEGINTRANSACTION ';'
      {
        $$ = configurator -> opBeginTransaction();
      }
  | tCOMMITTRANSACTION ';'
      {
        $$ = configurator -> opCommitTransaction();
      }
  | tABORTTRANSACTION ';'
      {
        $$ = configurator -> opAbortTransaction();
      }
  | tSET_DB_TIMESTAMP '(' job_data_reference ')'  ';'
      {
        $$ = configurator -> opSetDbTimestamp();
      }
  ;
if_statement /* DOCUMENTATION:DIAGRAM */
  : tIF job_if_statement
      {
        $$ = $2;
      }
  ;
while_statement /* DOCUMENTATION:DIAGRAM */
  : tWHILE job_while_statement
      {
        $$ = $2;
      }
  ;
copy_paste_statement /* DOCUMENTATION:DIAGRAM */
  : tPASTE job_paste ';'
      {
        $$ = $2;
      }
  | tCOPY job_copy ';'
      {
        $$ = $2;
      }
  ;

set_func_statement /* DOCUMENTATION:DIAGRAM */
  : tSET_THIS '(' job_data_reference ')'  ';'
      {
        $$=configurator -> assignThis();
      }
  | tSET_INDEX '(' job_data_reference ')'  ';'
      {
        $$=configurator -> assignFuncIndex();
      }
  | tSET_REASON '(' job_data_reference ')'  ';'
      {
        $$=configurator -> assignFuncReason();
      }
  | tSET_FUNC_ARGS '(' job_data_reference ')'  ';'  /* DOCUMENTATION:HIDE BEGIN (only wepapi) */
      {
        $$=configurator -> assignFuncArgs();
      } /* DOCUMENTATION:HIDE END */
  ;

/* --------------------------------------------------------------------------- */
/* REST SERVICE                                                                */
/* --------------------------------------------------------------------------- */
restService_statement /* DOCUMENTATION:DIAGRAM */
  : restService_action ';'
      {
        configurator -> opActionResult();
      }
  ;

restService_action_logon_additional_options  /* DOCUMENTATION:HIDE BEGIN (only wepapi) */
  : /* none */
  |     ',' ui_xfer
    {
      configurator -> restService_setMessageXfer();
    }
  ; /* DOCUMENTATION:HIDE END */

restService_action /* DOCUMENTATION:DIAGRAM */
  : opt_restService_responseStream tGET '(' restService_get_option_list ')'
    {
      $$ = configurator -> opRestServiceGet();
    }
  | tDELETE '(' restService_get_option_list ')'
    {
      $$ = configurator -> opRestServiceDelete();
    }
  | opt_restService_responseStream tPUT '(' restService_put_option_list ')'
    {
      $$ = configurator -> opRestServicePut();
    }
  | opt_restService_responseStream tPOST '(' restService_post_option_list ')'
    {
      $$ = configurator -> opRestServicePost();
    }
  | tREST_LOGON '(' ui_xfer
    {
      configurator -> restService_setBaseUrlXfer();
    }
    ',' ui_xfer
    {
      configurator -> restService_setUsernameXfer();
    }
    ',' ui_xfer
    {
      configurator -> restService_setPasswordXfer();
    }
    restService_action_logon_additional_options  /* DOCUMENTATION:HIDE (only wepapi) */
    ')'
    {
      $$ = configurator -> opRestServiceLogon();
    }
  | tREST_JWT_LOGON '(' ui_xfer
    {
      configurator -> restService_setBaseUrlXfer();
    }
    ',' ui_xfer
    {
      configurator -> restService_setJwtXfer();
    }
    restService_action_logon_additional_options  /* DOCUMENTATION:HIDE (only wepapi) */
    ')'
    {
      $$ = configurator -> opRestServiceJwtLogon();
    }
  | tREST_LOGOFF
    {
      $$ = configurator -> opRestServiceLogoff();
    }
  ;

opt_restService_responseStream /* DOCUMENTATION:UNFOLD */
  : /* none */
  | restService_responseStream
  ;

restService_responseStream /* DOCUMENTATION:DIAGRAM */
  : tID_STREAM '='
   {
      configurator -> restService_setResponseStream( *($1) );
      delete $1;
    }
  | '['
    {
      configurator -> setStreamJSONFlag();
      configurator -> newStream();
    }
    st_json_format_command ']' '='
    {
      configurator -> restService_setResponseStream();
      configurator -> resetStream();
    }
  ;

restService_get_option_list /* DOCUMENTATION:UNFOLD */
  : restService_get_option
  | restService_get_option_list ',' restService_get_option
  ;
restService_get_option /* DOCUMENTATION:DIAGRAM */
  : tPATH '=' restService_path
  | tFILTER '=' restService_filter
  ;

restService_put_option_list /* DOCUMENTATION:UNFOLD */
  : restService_put_option
  | restService_put_option_list ',' restService_put_option
  ;
restService_put_option /* DOCUMENTATION:DIAGRAM */
  : tPATH '=' restService_path
  | tDATA '=' restService_data
  | tSET_DB_TIMESTAMP
    {
      configurator -> restService_setSetDbTimestamp();
    }
  ;

restService_post_option_list /* DOCUMENTATION:UNFOLD */
  : restService_post_option
  | restService_post_option_list ',' restService_post_option
  ;
restService_post_option /* DOCUMENTATION:DIAGRAM */
  : tPATH '=' restService_path
  | tFILTER '=' restService_filter
  | tDATA '=' restService_data
  | tSET_DB_TIMESTAMP
    {
      configurator -> restService_setSetDbTimestamp();
    }
  ;

restService_path /* DOCUMENTATION:DIAGRAM */
  : tSTRING_CONSTANT
    {
      configurator -> restService_setPath( *($1) );
      delete $1;
    }
  | tINT_CONSTANT /* DOCUMENTATION:HIDE */
    {
      configurator -> restService_setPath( std::to_string($1) );
    }
  | ui_xfer
    {
      configurator -> restService_setPathXfer();
    }
  | tID_STREAM
    {
      configurator -> restService_setPathStream( *($1) );
      delete $1;
    }
  | '['
    {
      configurator -> setStreamUrlFlag();
      configurator -> newStream();
    }
    op_stream_parameters ']'
    {
      configurator -> restService_setPathStream();
      configurator -> resetStream();
    }
  ;

restService_filter /* DOCUMENTATION:UNFOLD */
  : tID_STREAM
    {
      configurator -> restService_setFilterStream( *($1) );
      delete $1;
    }
  | '['
    {
      configurator -> setStreamJSONFlag();
      configurator -> newStream();
    }
    st_json_format_command ']'
    {
      configurator -> restService_setFilterStream();
      configurator -> resetStream();
    }
  ;

restService_data /* DOCUMENTATION:UNFOLD */
  : tID_STREAM
    {
      configurator -> restService_setDataStream( *($1) );
      delete $1;
    }
  | '['
    {
      configurator -> setStreamJSONFlag();
      configurator -> newStream();
    }
    st_json_format_command ']'
    {
      configurator -> restService_setDataStream();
      configurator -> resetStream();
    }
  ;

job_assignments /* DOCUMENTATION:UNFOLD */
  : '=' job_expression
      {
        configurator -> opAssignValue();
      }
  | tACCUMULATE job_expression
      {
        configurator -> opAccumulateValue();
      }
  | tINCR
      {
        configurator -> opIncrementValue();
      }
  | tDECR
      {
        configurator -> opDecrementValue();
      }
  ;

/* --------------------------------------------------------------------------- */
/* Cycle                                                                       */
/* --------------------------------------------------------------------------- */

job_cyclenumber /* DOCUMENTATION:UNFOLD */
  : /* none */
      {
        $$ = configurator -> opPushInvalid();
      }
  | '[' job_expression ']'
      {
        $$ = $2;
      }
  ;

/* --------------------------------------------------------------------------- */
/* RUN                                                                         */
/* --------------------------------------------------------------------------- */

job_run_statement /* DOCUMENTATION:UNFOLD */
  : job_run_action
      {
        configurator -> opActionResult();
      }
  | job_run_function
  ;
job_run_function /* DOCUMENTATION:UNFOLD */
  : tID_TASK
      {
        $$ = configurator -> opExecuteTask( *($1) );
        delete $1;
      }
  | function_reference
      {
        $$ = configurator -> opExecuteFunction( *($1) );
        delete $1;
      }
  | temp_data_reference
      {
        $$ = configurator -> opExecuteFunction( $1 );
      }
  ;
function_reference /* DOCUMENTATION:UNFOLD */
  : tID_FUNCTION
      {
        $$ = $1;
      }
  | tIDENTIFIER /* DOCUMENTATION:HIDE BEGIN simplify diagrams */
      {
        $$ = $1;
      } /* DOCUMENTATION:HIDE END */
  ;
job_run_action /* DOCUMENTATION:DIAGRAM */
  : tID_PROCESSGROUP
      {
        $$ = configurator -> opExecuteProcess( *($1) );
        delete $1;
      }
  | tID_PLUGIN
      {
        /*$$ = */configurator -> opPluginInitialise( *($1) );
        delete $1;
      }
    '(' job_plugin_parameter ')'
      {
        configurator -> opExecutePlugin();
        $$=0;
      }
  ;
job_plugin_parameter /* DOCUMENTATION:UNFOLD */
  : /* none */
  | job_plugin_param_list
  ;
job_plugin_param_list /* DOCUMENTATION:UNFOLD */
  : job_plugin_param
  | job_plugin_param_list ',' job_plugin_param
  ;
job_plugin_param /* DOCUMENTATION:UNFOLD */
  : job_expression
      {
        configurator -> opPluginParameter();
      }
  ;

/* --------------------------------------------------------------------------- */
/* OPEN und SAVE                                                               */
/* --------------------------------------------------------------------------- */

job_open_statement /* DOCUMENTATION:UNFOLD */
  : job_open_action
       {
        configurator -> opActionResult();
       }
  ;
job_open_action /* DOCUMENTATION:UNFOLD */
  : tID_FILESTREAM opt_job_file_option
      {
        $$ = configurator -> opExecuteJobActionOpen( *($1) );
        delete $1;
      }
  ;
job_save_statement /* DOCUMENTATION:UNFOLD */
  : job_save_action
      {
        configurator -> opActionResult();
      }
  ;
job_save_action /* DOCUMENTATION:UNFOLD */
  : tID_FILESTREAM opt_job_file_option
      {
        $$ = configurator -> opExecuteJobActionSave( *($1) );
        delete $1;
      }
  | tID_FORM opt_job_file_option
      {
        $$ = configurator -> opExecuteJobActionSave( *($1) );
        delete $1;
      }
  | ui_form_element_identifier opt_job_file_option
      {
        $$ = configurator -> opExecuteJobActionSave( *($1) );
         delete $1;
      }
  | tID_REPORTSTREAM job_file_option
      {
        $$ = configurator -> opExecuteJobActionSave( *($1) );
        delete $1;
      }
  ;

opt_job_file_option /* DOCUMENTATION:UNFOLD */
  : /* none */
  | ',' job_file_option
  ;
job_file_option /* DOCUMENTATION:UNFOLD */
  : job_data_reference
    {
      configurator -> fileOption_setXferFilename();
    }
  | string_constant
      {
        configurator -> fileOption_setFilename(*($1));
        delete $1;
      }
  | tWEBAPI '=' job_data_reference /* DOCUMENTATION:HIDE (WebApi) */
      {
        configurator -> fileOption_setXferBaseFilename();
      }
  | tWEBAPI '=' string_constant /* DOCUMENTATION:HIDE (WebApi) */
      {
        configurator -> fileOption_setBaseFilename(*($3));
        delete $3;
      }
  ;

/* ------------------------------------------------------------------------- */
/* CHANGED (change indicator)                                                */
/* ------------------------------------------------------------------------- */

job_changed_action /* DOCUMENTATION:UNFOLD */
  : tID_FORM ',' job_data_reference job_changed_option
      {
        $$ = configurator -> opEvalChanged( *($1), $4 );
        delete $1;
      }
  ;
job_changed_option /* DOCUMENTATION:UNFOLD */
  : /* none */
    {
      $$ = false;
    }
  | ',' job_data_reference
    {
      $$ = true;
    }
  ;

/* --------------------------------------------------------------------------- */
/* LOAD (xml query)                                                            */
/* --------------------------------------------------------------------------- */

job_xml_query_statement /* DOCUMENTATION:HIDE BEGIN not implemented in intens4 */
  : job_xml_query_action
      {
        configurator -> opActionResult();
      }
  ;
job_xml_query_action
  : tID_FILESTREAM ',' st_data_reference ',' st_data_reference
      {
        $$ = configurator -> opExecuteJobActionXMLXPath( *($1), $3, $5 );
        delete $1;
      }
  ; /* DOCUMENTATION:HIDE END */

/* --------------------------------------------------------------------------- */
/* LOAD (description)                                                          */
/* --------------------------------------------------------------------------- */

job_description_statement /* DOCUMENTATION:UNFOLD */
  : job_description_action
      {
        configurator -> opActionResult();
      }
  ;
job_description_action /* DOCUMENTATION:UNFOLD */
  : st_data_reference
      {
        $$ = configurator -> opParse( $1 );
      }
  | string_constant
      {
        $$ = configurator -> opParse( *($1) );
        delete $1;
      }
  ;

/* --------------------------------------------------------------------------- */
/* SERIALIZE (serialize)                                                       */
/* --------------------------------------------------------------------------- */

job_serialize_statement /* DOCUMENTATION:UNFOLD */
  : serialize_element_option serialize_type_option serialize_result_option
  ;
serialize_element_option /* DOCUMENTATION:UNFOLD */
  : tID_FORM
      {
        configurator -> setSerializeElement( *($1) );
        delete $1;
      }
  | ui_form_element_identifier
      {
        configurator -> setSerializeElement( *($1) );
        delete $1;
      }
  | job_data_reference
      {
//        configurator -> setSerializeGuiElement("");
      }
  ;

serialize_type_option /* DOCUMENTATION:UNFOLD */
  : ',' tXML
    {
      configurator -> setSerializeType(AppData::serialize_XML);
    }
  | ',' tJSON
    {
      configurator -> setSerializeType(AppData::serialize_JSON);
    }
  | ',' tPROTO
    {
      configurator -> setSerializeType(AppData::serialize_PROTO);
    }
  ;

serialize_result_option /* DOCUMENTATION:UNFOLD */
  : ',' job_data_reference
  | ',' string_constant
      {
        configurator -> setSerializeOutputFilename( *($2) );
        delete $2;
      }
  ;

/* --------------------------------------------------------------------------- */
/* MAP, UNMAP und VISIBLE                                                      */
/* --------------------------------------------------------------------------- */

job_map /* DOCUMENTATION:UNFOLD */
  : '(' job_map_element_list ')' { $$ = $2; }
  ;
job_map_element_list /* DOCUMENTATION:UNFOLD */
  : job_map_element
  | job_map_element_list ',' job_map_element
  ;
job_map_element /* DOCUMENTATION:DIAGRAM */
  : job_form_identifier
      {
        $$ = configurator -> opMapForm( *($1) );
        delete $1;
      }
  | job_folder_group_identifier job_map_omit
      {
        $$ = configurator -> opMapFolderGroup( *($1), (GuiFolderGroup::OmitMap) $2 );
        delete $1;
      }
  | tID_TABLE  ',' job_ref_with_wildcards job_data_reference
      {
        $$ = configurator ->  opMapTableLine( *($1) );
      }
  | ui_form_element_identifier
      {
        $$ = configurator ->  opMapGuiElement( *($1) );
        delete $1;
      }
  | temp_data_reference opt_job_map_element_folder_option
      {
        $$ = configurator -> opMap( $1, $2 );
      }
  | tID_FOLDER job_map_element_folder_option
      {
        $$ = configurator ->  opMapFolder(*($1), $2);
        delete $1;
      }
  ;
opt_job_map_element_folder_option /* DOCUMENTATION:UNFOLD */
  : /* none */ { $$ = -1; }
  | job_map_element_folder_option
  ;
job_map_element_folder_option /* DOCUMENTATION:UNFOLD */
  : ':' job_data_reference
  {
    $$ = -2;
  }
  | ':' tINT_CONSTANT
  {
    $$ = $2;
  }
;
job_map_omit /* DOCUMENTATION:UNFOLD */
  : /* none */       { $$ = 0; }
  | ':' tOMIT_TTRAIL { $$ = 1; }
  | ':' tOMIT_ACTIVATE { $$ = 2; }
  ;
job_unmap /* DOCUMENTATION:UNFOLD */
  : '(' job_unmap_element_list ')' { $$ = $2; }
  ;
job_unmap_element_list /* DOCUMENTATION:UNFOLD */
  : job_unmap_element
  | job_unmap_element_list ',' job_unmap_element
  ;
job_unmap_element /* DOCUMENTATION:DIAGRAM */
  : job_form_identifier
      {
        $$ = configurator -> opUnmapForm( *($1) );
        delete $1;
      }
  | job_folder_group_identifier
      {
        $$ = configurator ->  opUnmapFolderGroup( *($1) );
        delete $1;
      }
  | tID_TABLE  ',' job_ref_with_wildcards job_data_reference
      {
        $$ = configurator ->  opUnmapTableLine( *($1) );
        delete $1;
      }
  | ui_form_element_identifier
      {
        $$ = configurator ->  opUnmapGuiElement( *($1) );
        delete $1;
      }
  | job_data_reference
      {
        $$ = configurator -> opUnmap();
      }
  ;

job_visible_action /* DOCUMENTATION:DIAGRAM */
  : job_form_identifier
      {
        $$ = configurator -> opVisibleForm( *($1) );
        delete $1;
      }
  | job_folder_group_identifier
      {
        $$ = configurator ->  opVisibleFolderGroup( *($1) );
        delete $1;
      }
  | temp_data_reference
      {
        $$ = configurator -> opVisible( $1 );
      }
  ;

job_editable_action /* DOCUMENTATION:DIAGRAM */
  : job_data_reference
      {
        $$ = configurator -> opEditable();
      }
  | tID_FIELDGROUP
      {
      $$ = configurator -> opEditable(*($1));
      }
  ;

job_form_identifier /* DOCUMENTATION:UNFOLD */
  : tID_FORM       { $$ = $1; }
  | tID_PROGRESSBAR{ $$ = $1; }
  ;
job_ui_element_identifier /* DOCUMENTATION:UNFOLD */
  : ui_form_element_identifier {$$ = $1;}
  | tID_FORM {$$ = $1;}
  ;
job_folder_group_identifier /* DOCUMENTATION:UNFOLD */
  : tID_FOLDERGROUP         { $$ = $1; }
  ;

/* --------------------------------------------------------------------------- */
/* COPY / PASTE                                                                */
/* --------------------------------------------------------------------------- */

job_copy /* DOCUMENTATION:UNFOLD */
  : job_copy_element              { $$ = $1; }
  | '(' job_copy_element_list ')' { $$ = $2; }
  ;
job_copy_element_list /* DOCUMENTATION:UNFOLD */
  : job_copy_element
  | job_copy_element_list ',' job_copy_element
  ;
job_copy_element /* DOCUMENTATION:UNFOLD */
  : identifier
      {
        $$ = configurator -> opCopy( *($1) );
        delete $1;
      }
  ;

job_paste /* DOCUMENTATION:UNFOLD */
  : job_paste_element              { $$ = $1; }
  | '(' job_paste_element_list ')' { $$ = $2; }
  ;
job_paste_element_list /* DOCUMENTATION:UNFOLD */
  : job_paste_element
  | job_paste_element_list ',' job_paste_element
  ;
job_paste_element /* DOCUMENTATION:UNFOLD */
  : identifier
      {
        $$ = configurator -> opPaste( *($1) );
        delete $1;
      }
  ;

/* --------------------------------------------------------------------------- */
/* SELECT LIST ROW                                                             */
/* --------------------------------------------------------------------------- */

job_select_list /* DOCUMENTATION:UNFOLD */
  : '(' job_select_element ')' { $$ = $2; }
  ;
job_select_element /* DOCUMENTATION:UNFOLD */
  : job_select_identifier ',' job_ref_with_wildcards job_data_reference
      {
        configurator -> opSelectRows( *($1) );
        $$ = $4;
        delete $1;
      }
  | job_select_identifier ',' tINT_CONSTANT
      {
        configurator -> opPushInteger( $3 );
        configurator -> opSelectRows( *($1), true );
        $$ = $3;
        delete $1;
      }
  ;
job_select_identifier /* DOCUMENTATION:UNFOLD */
  : tID_LIST   { $$ = $1; }
  | tID_NAVIGATOR   { $$ = $1; }
  | tID_TABLE   { $$ = $1; }
  ;

/* --------------------------------------------------------------------------- */
/* GET SELECTED LIST ROW                                                             */
/* --------------------------------------------------------------------------- */

job_get_selection /* DOCUMENTATION:UNFOLD */
  : '(' job_get_selection_element ')' { $$ = $2; }
  ;
job_get_selection_element /* DOCUMENTATION:DIAGRAM */
  : tID_LIST ',' job_data_reference
      {
        configurator -> opGetSelection( *($1) );
        $$ = $3;
        delete $1;
      }
  | tID_NAVIGATOR ',' job_data_reference
      {
        configurator -> opGetSelection( *($1) );
        $$ = $3;
        delete $1;
      }
  | tID_TABLE ',' job_data_reference ',' job_data_reference
      {
        configurator -> opGetSelection( *($1) );
        $$ = $3;
        delete $1;
      }
  | tID_PLOT2D ',' job_data_reference ',' job_data_reference ',' job_data_reference',' job_data_reference
      {
        configurator -> opGetSelection( *($1) );
        $$ = $3;
        delete $1;
      }
  ;

/* --------------------------------------------------------------------------- */
/* GET SORT LIST COLUMN                                                        */
/* --------------------------------------------------------------------------- */

job_get_sort_criteria /* DOCUMENTATION:UNFOLD */
  : '(' job_get_sort_criteria_element ')' { $$ = $2; }
  ;
job_get_sort_criteria_element /* DOCUMENTATION:UNFOLD */
  : tID_LIST ',' job_data_reference
      {
        configurator -> opGetSortCriteria( *($1) );
        $$ = $3;
        delete $1;
      }
  ;

/* --------------------------------------------------------------------------- */
/* SET STYLESHEET                                                              */
/* --------------------------------------------------------------------------- */

job_set_stylesheet /* DOCUMENTATION:UNFOLD */
  : '(' job_set_stylesheet_element ')' { $$ = $2; }
  ;
job_set_stylesheet_element /* DOCUMENTATION:DIAGRAM */
  : job_ui_element_identifier ',' job_data_reference
      {
        configurator -> opSetStylesheet( *($1) );
        $$ = $3;
        delete $1;
      }
  | job_ref_with_wildcards job_data_reference ',' job_data_reference
      {
      $$=configurator -> opSetStylesheet();
      }
  ;

/* --------------------------------------------------------------------------- */
/* CLEAR_SELECTION                                                             */
/* --------------------------------------------------------------------------- */

job_clear_selection /* DOCUMENTATION:UNFOLD */
  : '(' job_clear_selection_element ')' { $$ = $2; }
  ;
job_clear_selection_element /* DOCUMENTATION:UNFOLD */
  : job_select_identifier
      {
        $$ = configurator -> opClearSelection( *($1) );
        delete $1;
      }
  ;

/* --------------------------------------------------------------------------- */
/* ALLOW und DISALLOW                                                          */
/* --------------------------------------------------------------------------- */

job_allow /* DOCUMENTATION:UNFOLD */
  : '(' job_allow_element_list ')' { $$ = $2; }
  ;
job_allow_element_list /* DOCUMENTATION:UNFOLD */
  : job_allow_element
  | job_allow_element_list ',' job_allow_element
  ;
job_allow_element /* DOCUMENTATION:UNFOLD */
  : job_listener_controller
      {
        $$ = configurator -> opAllow();
      }
  ;

job_disallow /* DOCUMENTATION:HIDE BEGIN renamed to job_allow */
  : '(' job_disallow_element_list ')' { $$ = $2; }
  ;
job_disallow_element_list
  : job_disallow_element
  | job_disallow_element_list ',' job_disallow_element
  ;
job_disallow_element
  : job_listener_controller
      {
        $$ = configurator -> opDisallow();
      }
  ; /* DOCUMENTATION:HIDE END */

job_listener_controller /* DOCUMENTATION:UNFOLD */
  : tID_FORM
      {
        $$ = configurator -> getForm( *($1) )->getListenerController();
        delete $1;
      }
  | tID_TASK
      {
        $$ = configurator -> getTask( *($1) );
        delete $1;
      }
  | function_reference
      {
        $$ = configurator -> jobGetFunction( *($1) );
        delete $1;
      }
  | tID_PROCESSGROUP
      {
        $$ = configurator -> findProcessGroup( *($1) );
        delete $1;
      }
  ;

/* --------------------------------------------------------------------------- */
/* GUI_ELEMENT                                                                 */
/* --------------------------------------------------------------------------- */

job_gui_element /* DOCUMENTATION:UNFOLD */
  : tID_FORM
  | tID_FIELDGROUP
  | tID_PLOT2D
  ;

/* --------------------------------------------------------------------------- */
/* ENABLE und DISABLE                                                          */
/* --------------------------------------------------------------------------- */

job_enable /* DOCUMENTATION:UNFOLD */
  : '(' job_enable_object_list ')' { $$ = $2; }
  ;
job_enable_object_list /* DOCUMENTATION:UNFOLD */
  : job_enable_object
  | job_enable_object_list ',' job_enable_object
  ;
job_enable_object /* DOCUMENTATION:UNFOLD */
  : job_enable_disable_element
      {
        $$ = configurator -> opEnableGuiElement();
      }
  | job_data_reference
      {
        $$ = $1;
        configurator -> opPushInteger( 1 );
        configurator -> opSetEditable();
      }
  | tCYCLE job_enable_disable_cycle
      {
        $$ = $2; /* cycle index */
        configurator -> opPushInteger( 1 ); /* 1 == enable */
        configurator -> opSetEditableCycle();
      }
  ;

job_disable /* DOCUMENTATION:UNFOLD */
  : '(' job_disable_object_list ')' { $$ = $2; }
  ;
job_disable_object_list /* DOCUMENTATION:UNFOLD */
  : job_disable_object
  | job_disable_object_list ',' job_disable_object
  ;
job_disable_object /* DOCUMENTATION:UNFOLD */
  : job_enable_disable_element
      {
        $$ = configurator -> opDisableGuiElement();
      }
  | job_data_reference
      {
        $$ = $1;
        configurator -> opPushInteger( 0 );
        configurator -> opSetEditable();
      }
  | tCYCLE job_enable_disable_cycle
      {
        $$ = $2; /* cycle index */
        configurator -> opPushInteger( 0 ); /* 0 == disable */
        configurator -> opSetEditableCycle();
      }
  ;

job_disable_drag /* DOCUMENTATION:UNFOLD */
  : '(' job_disable_drag_object_list ')' { $$ = $2; }
  ;
job_disable_drag_object_list /* DOCUMENTATION:UNFOLD */
  : job_disable_drag_object
  | job_disable_drag_object_list ',' job_disable_drag_object
  ;
job_disable_drag_object /* DOCUMENTATION:UNFOLD */
  : job_enable_disable_element
      {
        $$ = configurator -> opDisableDragGuiElement();
      }
  ;

job_enable_disable_element /* DOCUMENTATION:UNFOLD */
  : tID_FORM
      {
        $$ = configurator -> getNamedElement( *($1) );
        delete $1;
      }
  | tID_FIELDGROUP
      {
        $$ = configurator -> getNamedElement( *($1) );
        delete $1;
      }
  | tID_FOLDER
      {
        $$ = configurator -> getNamedElement( *($1) );
        delete $1;
      }
  | tID_TABLE
      {
        $$ = configurator -> getNamedElement( *($1) );
        delete $1;
      }
  | tID_NAVIGATOR
      {
        $$ = configurator -> getNamedElement( *($1) );
        delete $1;
      }
  | tID_INDEX
      {
        $$ = configurator -> getNamedElement( *($1) );
        delete $1;
      }
  | tID_MENU
      {
        $$ = configurator -> getNamedElement( *($1) );
        delete $1;
      }
  ;

job_enable_disable_cycle /* DOCUMENTATION:DIAGRAM */
  : /* empty */
      { /* current cycle */
        $$ = configurator -> opPushInteger( -1 );
      }
  | '(' job_expression ')'
      { /* cycle number */
        $$ = $2;
      }
  ;

/* --------------------------------------------------------------------------- */
/* ATTRIBUTES                                                                  */
/* --------------------------------------------------------------------------- */

job_set_attributes /* DOCUMENTATION:UNFOLD */
  : tLOCK ',' job_data_reference_boolean
      {
        $$ = $3;
        configurator -> opSetLocked();
      }
  | tEDITABLE ',' job_data_reference_boolean
      {
        $$ = $3;
        configurator -> opSetEditable();
      }
  | tTIMESTAMP ',' job_ref_with_wildcards job_data_reference_boolean
      {
        $$ = $4;
        configurator -> opSetValueTimestamp();
      }
  | tCOLOR ',' job_ref_with_wildcards job_data_reference ',' job_expression
      {
        $$ = $3;
        configurator -> opSetColor();
      }
  | tCOLORBIT ',' job_ref_with_wildcards job_data_reference ',' job_expression
      {
        $$ = $3;
        configurator -> opSetColorBit();
      }
  ;
job_data_reference_boolean /* DOCUMENTATION:DIAGRAM */
  : job_data_reference job_set_attr_boolean
      {
        $$ = $1;
      }
  ;
job_set_attr_boolean /* DOCUMENTATION:UNFOLD */
  : /* none */
      { configurator -> opPushInteger( 1 ); /* true */
      }
  | ',' job_boolean_expression
  ;

job_unset_attributes /* DOCUMENTATION:UNFOLD */
  : tLOCK ',' job_data_reference
      {
        $$ = $3;
        configurator -> opPushInteger( 0 ); /* false */
        configurator -> opSetLocked();
      }
  | tEDITABLE ',' job_data_reference
      {
        $$ = $3;
        configurator -> opPushInteger( 0 ); /* false */
        configurator -> opSetEditable();
      }
  | tTIMESTAMP ',' job_ref_with_wildcards job_data_reference
      {
        $$ = $4;
        configurator -> opPushInteger( 0 ); /* false */
        configurator -> opSetValueTimestamp();
      }
  | tCOLORBIT ',' job_ref_with_wildcards job_data_reference ',' job_expression
      {
        $$ = $3;
        configurator -> opUnsetColorBit();
      }
  ;

/* --------------------------------------------------------------------------- */
/* UPDATE                                                                      */
/* --------------------------------------------------------------------------- */

job_update /* DOCUMENTATION:UNFOLD */
  : '(' job_update_element_list ')' { $$ = $2; }
  ;
job_update_element_list /* DOCUMENTATION:UNFOLD */
  : job_update_element
  | job_update_element_list ',' job_update_element
  ;
job_update_element /* DOCUMENTATION:UNFOLD */
  : job_gui_element
      {
        $$ = configurator -> opUpdateGuiElement( *($1) );
        delete $1;
      }
  ;

job_ref_with_wildcards /* DOCUMENTATION:EMPTY */
  :   {
        configurator -> allowDataWildcards();
        $$ = configurator->getFunctionNextAddress();
      }
  ;
/* --------------------------------------------------------------------------- */
/* SIZE                                                                        */
/* --------------------------------------------------------------------------- */

job_size /* DOCUMENTATION:UNFOLD */
  : '(' job_ref_with_wildcards job_data_reference ',' job_data_reference ')'
      {
        configurator -> opSize();
        $$ = $2;
      }
  ;

/* --------------------------------------------------------------------------- */
/* DATA_SIZE                                                                   */
/* --------------------------------------------------------------------------- */

job_data_size /* DOCUMENTATION:UNFOLD */
  : '(' job_data_reference ',' job_data_reference ')'
      {
        configurator -> opDataSize();
        $$ = $2;
      }
  ;

/* --------------------------------------------------------------------------- */
/* INDEX                                                                       */
/* --------------------------------------------------------------------------- */

job_index /* DOCUMENTATION:UNFOLD */
  : '(' job_data_reference ',' job_data_reference ')'
      {
        configurator -> opIndex();
        $$ = $2;
      }
  ;

/* --------------------------------------------------------------------------- */
/* COMPARE                                                                     */
/* --------------------------------------------------------------------------- */

job_compare /* DOCUMENTATION:UNFOLD */
  : job_data_reference ',' job_ref_with_wildcards job_data_reference
    ',' job_ref_with_wildcards job_data_reference
      {
        configurator -> opCompare(false);
        $$ = $1;
      }
  | job_data_reference ',' job_ref_with_wildcards job_data_reference
      {
        configurator -> opCompare(true);
        $$ = $1;
      }
  ;

/* --------------------------------------------------------------------------- */
/* ASSIGN_CONSISTENCY                                                          */
/* --------------------------------------------------------------------------- */

job_assign_consistency /* DOCUMENTATION:UNFOLD */
  : '(' job_data_reference ',' job_data_reference ')'
      {
        configurator -> opAssignConsistency();
        $$ = $2;
      }
  | '(' job_data_reference ',' tINVALID ')'
      {
        configurator -> opPushInvalid();
        configurator -> opAssignConsistency();
        $$ = $2;
      }
  ;

/* --------------------------------------------------------------------------- */
/* CLASS                                                                       */
/* --------------------------------------------------------------------------- */

job_class /* DOCUMENTATION:UNFOLD */
  : ui_form_element_identifier ',' string_constant
      {
        $$ = configurator -> opClass( *($1),  *($3));
      }
  | job_data_reference ',' string_constant
      {
        $$ = configurator -> opClass("", *($3));
      }
  | string_constant ',' string_constant
      {
        $$ = configurator -> opClass( *($1),  *($3));
      }
  ;
/* --------------------------------------------------------------------------- */
/* GUI_ELEMENT_METHOD                                                          */
/* --------------------------------------------------------------------------- */

job_gui_element_method /* DOCUMENTATION:UNFOLD */
  : '(' ui_form_element_identifier ',' job_data_reference ',' job_data_reference ')'
      {
        $$ = configurator -> opGuiElementMethod( (*$2) );
      }
  | '(' job_data_reference ',' job_data_reference ',' job_data_reference ')'
      {
        $$ = configurator -> opGuiElementMethod();
      }
  ;

/* --------------------------------------------------------------------------- */
/* COMPOSE                                                                     */
/* --------------------------------------------------------------------------- */
job_compose /* DOCUMENTATION:UNFOLD */
  : { configurator -> opComposeNew(); }
    job_compose_element_list
      {
        $$ = configurator -> opCompose();
      }
  ;
job_compose_element_list /* DOCUMENTATION:UNFOLD */
  : job_compose_element
  | job_compose_element_list ',' job_compose_element
  ;
job_compose_element /* DOCUMENTATION:UNFOLD */
  : { configurator -> opComposeIncreaseArg(); }
    job_expression
   ;

/* --------------------------------------------------------------------------- */
/* SET_RESOURCE                                                                */
/* --------------------------------------------------------------------------- */

job_set_resource /* DOCUMENTATION:UNFOLD */
  : string_constant ',' job_data_reference
      {
        $$ = configurator -> opSetResource( *($1), "", false );
        delete $1;
      }
  | string_constant ',' string_constant
      {
        $$ = configurator -> opSetResource( *($1), *($3), true );
        delete $1;
        delete $3;
      }
  ;

/* --------------------------------------------------------------------------- */
/* CLEAR                                                                       */
/* --------------------------------------------------------------------------- */

job_clear /* DOCUMENTATION:UNFOLD */
  : '(' job_clear_element_list ')' { $$ = $2; }
  ;
job_clear_element_list /* DOCUMENTATION:UNFOLD */
  : job_clear_element
  | job_clear_element_list ',' job_clear_element
  ;
job_clear_element /* DOCUMENTATION:UNFOLD */
  : job_ref_with_wildcards job_data_reference
      {
        configurator -> opClearValues();
      }
  | job_next_address tID_TEXTWINDOW
      {
        configurator -> opClearTextWindow( *$2 );
        delete $2;
      }
  | job_next_address tSTD_WINDOW
      {
        configurator -> opClearTextWindow( "STD_WINDOW" );
      }
  | job_next_address tLOG_WINDOW
      {
        configurator -> opClearTextWindow( "LOG_WINDOW" );
      }
  ;

/* --------------------------------------------------------------------------- */
/* ERASE                                                                       */
/* --------------------------------------------------------------------------- */

job_erase /* DOCUMENTATION:UNFOLD */
  : '(' job_erase_element_list ')' { $$ = $2; }
  ;
job_erase_element_list /* DOCUMENTATION:UNFOLD */
  : job_erase_element
  | job_erase_element_list ',' job_erase_element
  ;
job_erase_element /* DOCUMENTATION:UNFOLD */
  : job_ref_with_wildcards job_data_reference
      {
        configurator -> opEraseValues();
      }
  ;

/* --------------------------------------------------------------------------- */
/* PACK                                                                        */
/* --------------------------------------------------------------------------- */

job_pack /* DOCUMENTATION:UNFOLD */
  : '(' job_pack_element ')' { $$ = $2; }
  ;
job_pack_element /* DOCUMENTATION:UNFOLD */
  : job_ref_with_wildcards job_data_reference job_pack_row_or_col
      {
        configurator -> opPackValues( $3 );
      }
  ;
job_pack_row_or_col /* DOCUMENTATION:UNFOLD */
  : /* none */  { $$ = 1; }
  | ',' tROW    { $$ = 1; }
  | ',' tCOL    { $$ = 0; }
  ;

/* --------------------------------------------------------------------------- */
/* ASSIGN CORRESPONDING                                                        */
/* --------------------------------------------------------------------------- */

job_assign_corresponding /* DOCUMENTATION:UNFOLD */
  : '(' job_ref_with_wildcards job_data_reference ',' job_ref_with_wildcards job_data_reference ')'
     {
       configurator -> opAssignCorresponding();
       $$ = $2;
     }
  ;

/* --------------------------------------------------------------------------- */
/* IF Statement                                                                */
/* --------------------------------------------------------------------------- */

job_if_statement /* DOCUMENTATION:UNFOLD */
  : job_next_address job_if_expression job_if_stmt job_else_stmt
      {
       configurator -> setElseAddress( $4 );
      }
  ;
job_next_address /* DOCUMENTATION:EMPTY */
  : /* none */
      {
       $$ = configurator -> getFunctionNextAddress();
      }
  ;
job_if_expression /* DOCUMENTATION:UNFOLD */
  : '(' job_expression ')'
      {
       $$ = configurator -> opIfOperator();
      }
  ;
job_if_stmt /* DOCUMENTATION:UNFOLD */
  : job_statement
      {
       configurator -> opBranch();
      }
  ;
job_else_stmt /* DOCUMENTATION:UNFOLD */
  : /* none */
      {
       $$ = configurator -> getFunctionNextAddress();
      }
  | tELSE job_next_address job_statement
      {
        /* job_next_address ist eigentlich nicht nötig,
         * da job_statement denselben Wert haben sollte.
         * Es wurde eingefügt, weil neue Statements immer
         * wieder einfach den Wert 0 oder 1 hatten statt
         * ihrer Adresse
         */
        $$ = $2;
      }
  ;

/* --------------------------------------------------------------------------- */
/* WHILE Statement                                                             */
/* --------------------------------------------------------------------------- */

job_while_statement /* DOCUMENTATION:UNFOLD */
  : job_next_address job_while_expression job_statement
      {
       configurator -> setNextAddress( $1 );
      }
  ;
job_while_expression /* DOCUMENTATION:UNFOLD */
  : '(' job_expression ')'
      {
       $$ = configurator -> opWhileOperator();
      }
  ;

/* --------------------------------------------------------------------------- */
/* Expression                                                                  */
/* --------------------------------------------------------------------------- */

job_expression /* DOCUMENTATION:DIAGRAM */
  : data_expression
  | cycle_expression
  | function_expression
  | math_expression
  | file_expression
  | reason_expression
  ;

data_expression /* DOCUMENTATION:DIAGRAM */
  : tREAL_CONSTANT
      {
       $$ = configurator -> opPushReal( $1 );
      }
  | tINT_CONSTANT
      {
       $$ = configurator -> opPushInteger( $1 );
      }
  | job_string_expression
  | tEOLN
      {
       $$ = configurator -> opPushEndOfLine();
      }
  | tINVALID
      {
       $$ = configurator -> opPushInvalid();
      }
  | tOLDVALUE
      {
       $$ = configurator -> opPushOldValue();
      }
  | tINDEX
      {
       $$ = configurator -> opPushIndexNumber();
      }
  | tCOL
      {
       $$ = configurator -> opPushColumn();
      }
  | tROW
      {
       $$ = configurator -> opPushRow();
      }
  | tDIAGRAM_XPOS
      {
       $$ = configurator -> opPushDiagramXPos();
      }
  | tDIAGRAM_YPOS
      {
       $$ = configurator -> opPushDiagramYPos();
      }
  | tSORT_CRITERIA
      {
       $$ = configurator -> opPushSortCriteria();
      }
  | tERROR
      {
       $$ = configurator -> opPushError();
      }
  | tID_INDEX
      {
       $$ = configurator -> opPushIndex( *($1) );
        delete $1;
      }
  | job_data_reference
  | '(' job_expression ')'
      {
        $$=$2;
      }
  | job_expression '+'  job_expression
      {
       configurator -> opAdd();
      }
  | job_expression '-'  job_expression
      {
       configurator -> opSub();
      }
  | job_expression '*'  job_expression
      {
       configurator -> opMul();
      }
  | job_expression '/'  job_expression
      {
       configurator -> opDiv();
      }
  | job_expression '%'  job_expression
      {
       configurator -> opModulo();
      }
  | job_expression '^'  job_expression
      {
       configurator -> opPower();
      }
  | job_expression '>'  job_expression
      {
       configurator -> opGtr();
      }
  | job_expression tGEQ job_expression
      {
       configurator -> opGeq();
      }
  | job_expression '<'  job_expression
      {
       configurator -> opLss();
      }
  | job_expression tLEQ job_expression
      {
       configurator -> opLeq();
      }
  | job_expression tEQL job_expression
      {
       configurator -> opEql();
      }
  | job_expression tNEQ job_expression
      {
       configurator -> opNeq();
      }
  | job_expression tAND job_expression
      {
       configurator -> opAnd();
      }
  | job_expression tOR  job_expression
      {
       configurator -> opOr();
      }
  | '-' job_expression %prec UNARYMINUS
      {
       configurator -> opNegate();
        $$=$2;
      }
  | tNOT job_expression
      {
       configurator -> opNot();
        $$=$2;
      }
  ;

cycle_expression /* DOCUMENTATION:DIAGRAM */
  : tGETCYCLE
      {
       $$ = configurator -> opPushCycle();
      }
  | tMAXCYCLE
      {
       $$ = configurator -> opPushMaxCycle();
      }
  | tCYCLENAME job_cyclenumber
      {
        configurator -> opGetCycleName();
        $$ = $2;
      }
  ;

function_expression /* DOCUMENTATION:DIAGRAM */
  : tVALID '(' job_expression ')'
      {
       configurator -> opEvalValid();
        $$=$3;
      }
  | tINDEX '(' job_expression ')'
      {
       configurator -> opPushIndex();
        $$=$3;
      }
  | tINDEX '(' job_expression ',' job_expression ')'
      {
       configurator -> opPushIndexOfLevel();
        $$=$3;
      }
  | tCONFIRM job_confirm
      {
       $$=configurator -> opConfirm(false); // without cancel button
      }
  | tCONFIRM_CANCEL job_confirm
      {
       $$=configurator -> opConfirm(true); // with cancel button
      }
  | tGETTEXT '(' job_expression ')'
      {
       $$=configurator -> opGetText();
      }
  | tTIMESTAMP '(' job_data_reference ')'
      {
        configurator -> opGetTimestamp();
        $$=$3;
      }
  | tMODIFIED '(' job_data_reference ')'
      {
        configurator -> opGetDbModified();
        $$=$3;
      }
  | tCLASSNAME '(' job_data_reference ')'
      {
        configurator -> opGetClassname();
        $$=$3;
      }
  | tNODE '(' job_data_reference ')'
      {
       $$ = configurator -> opGetNodename();
      }
  | tRUN '(' job_run_action ')'
      {
        $$ = $3;
      }
  | tLOAD '(' job_description_action ')'
      {
        $$ = $3;
      }
  | send_action
      {
        $$ = $1;
      }
  | message_queue_action
      {
        $$ = $1;
      }
  | restService_action
      {
        $$ = $1;
      }
  | tSORTORDER '(' tID_LIST ')'
      {
        $$ = configurator -> opPushSortOrder( *($3) );
        delete $3;
      }
  | tCHANGED '(' job_changed_action ')'
      {
        $$ = $3;
      }
  | tVISIBLE '(' job_visible_action ')'
      {
        $$ = $3;
      }
  | tEDITABLE '(' job_editable_action ')'
      {
        $$ = $3;
      }
  | tCOMPARE '(' job_compare  ')'
      {
        $$ = $3;
      }
  | tSET_RESOURCE '(' job_set_resource  ')'
      {
        $$ = $3;
      }
  | tCOMPOSE '(' job_compose  ')'
      {
        $$ = $3;
      }
  | tICON '(' job_data_reference ')' /* DOCUMENTATION:HIDE (WebApi) */
      {
        $$ = configurator -> opIcon();
      }
  | tASSIGN_CONSISTENCY job_assign_consistency
      {
        $$ = $2;
      }
  | tGUI_ELEMENT_METHOD job_gui_element_method /* DOCUMENTATION:HIDE (WebApi) */
      {
        $$ = $2;
      }
  ;

/* --------------------------------------------------------------------------- */
/* CONFIRM                                                                     */
/* --------------------------------------------------------------------------- */

job_confirm /* DOCUMENTATION:UNFOLD */
  : '(' job_expression ')'
      {
        $$ = $2;
      }
  | '(' job_expression ',' job_confirm_option_list ')'
      {
        $$ = $2;
      }
  ;
job_confirm_option_list /* DOCUMENTATION:UNFOLD */
  : job_confirm_option
  | job_confirm_option_list ',' job_confirm_option
  ;
job_confirm_option /* DOCUMENTATION:DIAGRAM */
  :  /* none */
  | tBUTTON_YES '=' string_constant
      {
        configurator -> setButtonText( GuiElement::button_Yes, (*$3) );
        delete $3;
      }
  ;
  | tBUTTON_NO '=' string_constant
      {
        configurator -> setButtonText( GuiElement::button_No, (*$3) );
        delete $3;
      }
  | tBUTTON_CANCEL '=' string_constant
      {
        configurator -> setButtonText( GuiElement::button_Cancel, (*$3) );
        delete $3;
      }
  ;
  ;

/* --------------------------------------------------------------------------- */
/* MATH                                                                        */
/* --------------------------------------------------------------------------- */

 math_expression /* DOCUMENTATION:DIAGRAM */
  : tABS '(' job_expression ')'
      {
       configurator -> opEvalAbsValue();
        $$=$3;
      }
  | tLENGTH '(' job_expression ')'
      {
       configurator -> opEvalLengthValue();
        $$=$3;
      }
  | tSIN '(' job_expression ')'
      {
       configurator -> opEvalSinValue();
        $$=$3;
      }
  | tCOS '(' job_expression ')'
      {
       configurator -> opEvalCosValue();
        $$=$3;
      }
  | tTAN '(' job_expression ')'
      {
       configurator -> opEvalTanValue();
        $$=$3;
      }
  | tASIN '(' job_expression ')'
      {
       configurator -> opEvalASinValue();
        $$=$3;
      }
  | tACOS '(' job_expression ')'
      {
       configurator -> opEvalACosValue();
        $$=$3;
      }
  | tATAN '(' job_expression ')'
      {
       configurator -> opEvalATanValue();
        $$=$3;
      }
  | tATAN2 '(' job_expression ',' job_expression ')'
      {
       configurator -> opEvalATan2Value();
        $$=$3;
      }
  | tLOG '(' job_expression ')'
      {
       configurator -> opEvalLogValue();
        $$=$3;
      }
  | tLOG10 '(' job_expression ')'
      {
       configurator -> opEvalLog10Value();
        $$=$3;
      }
  | tSQRT '(' job_expression ')'
      {
       configurator -> opEvalSqrtValue();
       $$=$3;
      }
  | tROUND '(' job_expression ',' job_expression ')'
      {
       configurator -> opRound();
        $$=$3;
      }
  | tROUND5 '(' job_expression ')'
      {
       configurator -> opRound5();
        $$=$3;
      }
  | tROUND10 '(' job_expression ')'
      {
       configurator -> opRound10();
        $$=$3;
      }
  | tREAL '(' job_expression ')'
      {
       configurator -> opEvalComplexRealValue();
        $$=$3;
      }
  | tIMAG '(' job_expression ')'
      {
       configurator -> opEvalComplexImagValue();
        $$=$3;
      }
  | tARG '(' job_expression ')'
      {
       configurator -> opEvalArgValue();
        $$=$3;
      }
  | tCOMPLEX '(' job_expression ',' job_expression ')'
      {
       configurator -> opEvalComplexValue();
        $$=$3;
      }
  ;

file_expression /* DOCUMENTATION:DIAGRAM */
  : tOPEN '(' job_open_action ')'
      {
        $$ = $3;
      }
  | tLOAD '(' job_xml_query_action ')' ';' /* DOCUMENTATION:HIDE (not implemented in intens4) */
      {
        $$ = $3;
      }
  | tSAVE '(' job_save_action ')'
      {
        $$ = $3;
      }
  | tFILENAME '(' tID_FILESTREAM ')'
      {
        configurator -> opGetFileName( *($3) );
        delete $3;
      }
  | tFILENAME job_filename_options
      {
        configurator -> opFileDialog( false );
      }

  | tBASENAME '(' tID_FILESTREAM ')'
      {
        configurator -> opGetBaseName( *($3) );
        delete $3;
      }
  | tBASENAME '(' st_data_reference ')'
      {
        configurator -> opGetBaseName( "" );
      }
  | tDIRNAME '(' tID_FILESTREAM ')'
      {
        configurator -> opGetDirName( *($3) );
        delete $3;
      }
  | tDIRNAME job_dirname_options
      {
        configurator -> opFileDialog();
      }
  | tDIRNAME '(' st_data_reference ')'
      {
        configurator -> opGetDirName( "" );
      }
  ;

reason_expression /* DOCUMENTATION:DIAGRAM */
  : tREASON_INPUT
      {
       $$ = configurator -> opPushReason( JobManager::cll_Input );
      }
  | tREASON_INSERT
      {
       $$ = configurator -> opPushReason( JobManager::cll_Insert );
      }
  | tREASON_DUPLICATE
      {
       $$ = configurator -> opPushReason( JobManager::cll_Dupl );
      }
  | tREASON_CLEAR
      {
       $$ = configurator -> opPushReason( JobManager::cll_Clear );
      }
  | tREASON_REMOVE
      {
       $$ = configurator -> opPushReason( JobManager::cll_Delete );
      }
  | tREASON_PACK
      {
       $$ = configurator -> opPushReason( JobManager::cll_Pack );
      }
  | tREASON_SELECT
      {
       $$ = configurator -> opPushReason( JobManager::cll_Select );
      }
  | tREASON_UNSELECT
      {
       $$ = configurator -> opPushReason( JobManager::cll_Unselect );
      }
  | tREASON_SELECT_POINT
      {
       $$ = configurator -> opPushReason( JobManager::cll_SelectPoint );
      }
  | tREASON_SELECT_RECTANGLE
      {
       $$ = configurator -> opPushReason( JobManager::cll_SelectRectangle );
      }
  | tREASON_SORT
      {
       $$ = configurator -> opPushReason( JobManager::cll_Sort );
      }
  | tREASON_ACTIVATE
      {
       $$ = configurator -> opPushReason( JobManager::cll_Activate );
      }
  | tREASON_OPEN
      {
       $$ = configurator -> opPushReason( JobManager::cll_Open );
      }
  | tREASON_CLOSE
      {
       $$ = configurator -> opPushReason( JobManager::cll_Close );
      }
  | tREASON_DROP
      {
       $$ = configurator -> opPushReason( JobManager::cll_Drop );
      }
  | tREASON_MOVE
      {
       $$ = configurator -> opPushReason( JobManager::cll_Move );
      }
  | tREASON_NEW_CONNECTION
      {
       $$ = configurator -> opPushReason( JobManager::cll_NewConnection );
      }
  | tREASON_REMOVE_CONNECTION
      {
       $$ = configurator -> opPushReason( JobManager::cll_RemoveConnection );
      }
  | tREASON_REMOVE_ELEMENT
      {
       $$ = configurator -> opPushReason( JobManager::cll_RemoveElement );
      }
  | tREASON_CYCLE_CLEAR
      {
       $$ = configurator -> opPushReason( JobManager::cll_CycleClear );
      }
  | tREASON_CYCLE_DELETE
      {
       $$ = configurator -> opPushReason( JobManager::cll_CycleDelete );
      }
  | tREASON_CYCLE_NEW
      {
       $$ = configurator -> opPushReason( JobManager::cll_CycleNew );
      }
  | tREASON_CYCLE_RENAME
      {
       $$ = configurator -> opPushReason( JobManager::cll_CycleRename );
      }
  | tREASON_CYCLE_SWITCH
      {
       $$ = configurator -> opPushReason( JobManager::cll_CycleSwitch );
      }
  | tREASON_FOCUS
      {
       $$ = configurator -> opPushReason( JobManager::cll_FocusIn );
      }
  | tREASON_FOCUS_OUT
      {
       $$ = configurator -> opPushReason( JobManager::cll_FocusOut );
      }
  | tREASON_FUNCTION
      {
       $$ = configurator -> opPushReason( JobManager::cll_Function );
      }
  | tREASON_TASK
      {
       $$ = configurator -> opPushReason( JobManager::cll_Task );
      }
  | tREASON_GUI_UPDATE
      {
       $$ = configurator -> opPushReason( JobManager::cll_GuiUpdate );
      }
  ;

job_filename_options /* DOCUMENTATION:UNFOLD */
  :  /* none */
  | '{' job_filename_option_list '}'
  ;
job_filename_option_list /* DOCUMENTATION:UNFOLD */
  : job_filename_option
  | job_filename_option_list ',' job_filename_option
  ;
job_filename_option /* DOCUMENTATION:UNFOLD */
  : tFILTER '=' tSTRING_CONSTANT
    {
      configurator -> opFileDialog_setFilter( *($3) );
      delete $3;
    }
  | tFILTER '=' tINT_CONSTANT /* DOCUMENTATION:HIDE */
    {
      configurator -> opFileDialog_setFilter( std::to_string($3) );
    }
  | tDIRNAME '=' job_dirname_dirname
  | tOPEN
    {
      configurator->opFileDialog_setOpenMode(true);
    }
  | tSAVE
    {
      configurator->opFileDialog_setOpenMode(false);
    }
  ;

job_dirname_options /* DOCUMENTATION:UNFOLD */
  : /* none */
  | '{' job_dirname_option_list '}'
  ;
job_dirname_option_list /* DOCUMENTATION:UNFOLD */
  : job_dirname_option
  | job_dirname_option_list ',' job_dirname_option
  ;
job_dirname_option /* DOCUMENTATION:UNFOLD */
  : tDIRNAME '=' job_dirname_dirname
  ;
job_dirname_dirname /* DOCUMENTATION:UNFOLD */
  : tSTRING_CONSTANT
    {
      configurator -> opFileDialog_setDirname( *($1) );
      delete $1;
    }
  | tINT_CONSTANT /* DOCUMENTATION:HIDE */
    {
      configurator -> opFileDialog_setDirname( std::to_string($1) );
    }
  | ui_xfer
    {
      configurator -> opFileDialog_setDirnameXfer();
    }
  ;

/* --------------------------------------------------------------------------- */
/* Boolean Expression                                                          */
/* --------------------------------------------------------------------------- */

job_boolean_expression /* DOCUMENTATION:UNFOLD */
  : bool_constant
      {
       $$ = configurator -> opPushInteger( $1 );
      }
  | job_expression
  ;

/* --------------------------------------------------------------------------- */
/* PRINT                                                                       */
/* --------------------------------------------------------------------------- */

job_print_elements /* DOCUMENTATION:UNFOLD */
  : '(' job_print_element_list ')'
      {
        $$ = $2;
      }
  ;
job_print_element_list /* DOCUMENTATION:UNFOLD */
  : job_print_element
  | job_print_element_list ',' job_print_element
  ;
job_print_element /* DOCUMENTATION:UNFOLD */
  : job_expression
      {
       configurator -> opPrint();
      }
  ;

/* --------------------------------------------------------------------------- */
/* LOG                                                                         */
/* --------------------------------------------------------------------------- */

job_log_elements /* DOCUMENTATION:UNFOLD */
  : job_log_element_list
      {
        $$ = $1;
      }
  ;
job_log_element_list /* DOCUMENTATION:UNFOLD */
  : job_log_element
  | job_log_element_list ',' job_log_element
  ;
job_log_element /* DOCUMENTATION:UNFOLD */
  : job_expression
      {
        configurator -> opLogMsg();
      }
  ;
job_log_level /* DOCUMENTATION:UNFOLD */
  : tDEBUG { $$ = new std::string("DEBUG"); }
  | tINFO  { $$ = new std::string("INFO"); }
  | tWARN  { $$ = new std::string("WARN"); }
  | tERROR { $$ = new std::string("ERROR"); }
  | tFATAL { $$ = new std::string("FATAL"); }
  ;

/* --------------------------------------------------------------------------- */
/* SET_ERROR                                                                   */
/* --------------------------------------------------------------------------- */

job_error_elements /* DOCUMENTATION:UNFOLD */
  : /* none */
      {
       $$ = configurator -> opSetError();
      }
  | '(' job_error_element_list ')'
      {
        $$ = $2;
      }
  ;
job_error_element_list /* DOCUMENTATION:UNFOLD */
  : job_error_element
  | job_error_element_list ',' job_error_element
  ;
job_error_element /* DOCUMENTATION:UNFOLD */
  : job_expression
      {
       configurator -> opSetErrorMsg();
      }
  ;

/* --------------------------------------------------------------------------- */
/* SET_MSG                                                                 */
/* --------------------------------------------------------------------------- */

job_message_elements /* DOCUMENTATION:SYNONYM job_print_elements */
  : '(' job_message_element_list ')'
      {
       configurator -> opSetMessage();
        $$ = $2;
      }
  ;
job_message_element_list
  : job_message_element
  | job_message_element_list ',' job_message_element
  ;
job_message_element
  : job_expression
      {
       configurator -> opMessage();
      }
  ; /* DOCUMENTATION:HIDE END */

/* --------------------------------------------------------------------------- */
/* END OF PARSERYACC.YY                                                        */
/* --------------------------------------------------------------------------- */
