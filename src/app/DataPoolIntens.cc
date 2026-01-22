
#ifndef _WIN32
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <algorithm>
#include "app/AppData.h"
#include "app/DataPoolIntens.h"
#include "parser/IdManager.h"
#include "app/DataSet.h"
#include "app/ColorSet.h"
#include "app/UserAttr.h"
#include "app/ItemAttr.h"
#include "app/DataEventsIntens.h"
#include "streamer/TargetContainer.h"
#include "gui/DialogCompare.h"
#include "gui/DialogProgressBar.h"
#include "utils/Date.h"
#include "utils/gettext.h"

INIT_LOGGER();

typedef std::map< DataDictionary::DataType, std::string > TypeMap;
static TypeMap typemap;

/*=============================================================================*/
/* static variables                                                            */
/*=============================================================================*/

DataPoolIntens   *DataPoolIntens::s_instance    = 0;
DataPool         *DataPoolIntens::s_datapool    = 0;
DataDictionary   *DataPoolIntens::s_root_dict   = 0;
TransactionNumber DataPoolIntens::s_transaction = 0;

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

DataPoolIntens &DataPoolIntens::Instance(){
  if( s_instance == 0 ){
    s_instance = new DataPoolIntens;
    s_instance->initializeDataPool();
    typemap.insert( TypeMap::value_type(DataDictionary::type_Integer, "INTEGER" ) );
    typemap.insert( TypeMap::value_type(DataDictionary::type_Real, "REAL" ) );
    typemap.insert( TypeMap::value_type(DataDictionary::type_String, "STRING" ) );
    typemap.insert( TypeMap::value_type(DataDictionary::type_Complex, "COMPLEX" ) );
    typemap.insert( TypeMap::value_type(DataDictionary::type_CharData, "CDATA" ) );
    typemap.insert( TypeMap::value_type(DataDictionary::type_StructVariable, "STRUCT" ) );
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* Destroy --                                                                  */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::Destroy(){
  DataPool::Destroy();
  s_datapool = 0;
}

/* --------------------------------------------------------------------------- */
/* getDataPool --                                                              */
/* --------------------------------------------------------------------------- */

DataPool &DataPoolIntens::getDataPool(){
  if( s_datapool == 0 ){
    UserAttr Ua;
    ItemAttr Ia;
    s_datapool = DataPool::Instance( &Ua, &Ia );
    s_datapool->setDataEvents( new DataEventsIntens() );
  }
  return *s_datapool;
}

std::string DataPoolIntens::getAddrInfo(){
#ifndef _WIN32
  struct addrinfo* result;
  int error = getaddrinfo(AppData::Instance().HostName().c_str(), NULL, NULL, &result);
  if( error == 0){
    struct sockaddr_in *addr = (struct sockaddr_in *) result->ai_addr;
    std::string ipAddr = inet_ntoa(addr->sin_addr);
    freeaddrinfo(result);
    return ipAddr;
  }
  return "ERROR!";
#endif
  return "METHOD NOT SUPPORTED YET ON PLATFORM WIN32";
}

/* --------------------------------------------------------------------------- */
/* initializeDataPool --                                                       */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::initializeDataPool(){
  DataPool &datapool = getDataPool();
  DataDictionary *dict;

  // --------------
  // Standard Items
  // --------------
  datapool.AddToDictionary( "", "DATE", DataDictionary::type_String, 0, true );
  IdManager::Instance().registerId( "DATE", IdManager::id_DataVariable );
  datapool.AddToDictionary( "", "USER", DataDictionary::type_String, 0, true );
  IdManager::Instance().registerId( "USER", IdManager::id_DataVariable );
  datapool.AddToDictionary( "", "HOST", DataDictionary::type_String, 0, true );
  IdManager::Instance().registerId( "HOST", IdManager::id_DataVariable );
  datapool.AddToDictionary( "", "IPADDR", DataDictionary::type_String, 0, true );
  IdManager::Instance().registerId( "IPADDR", IdManager::id_DataVariable );
  datapool.AddToDictionary( "", "INTENS_VERSION", DataDictionary::type_String, 0, true );
  IdManager::Instance().registerId( "INTENS_VERSION", IdManager::id_DataVariable );
  datapool.AddToDictionary( "", "INTENS_VERSION_MAJOR", DataDictionary::type_Integer, 0, true );
  IdManager::Instance().registerId( "INTENS_VERSION_MAJOR", IdManager::id_DataVariable );
  datapool.AddToDictionary( "", "INTENS_VERSION_MINOR", DataDictionary::type_Integer, 0, true );
  IdManager::Instance().registerId( "INTENS_VERSION_MINOR", IdManager::id_DataVariable );
  datapool.AddToDictionary( "", "INTENS_VERSION_PATCH", DataDictionary::type_String, 0, true );
  IdManager::Instance().registerId( "INTENS_VERSION_PATCH", IdManager::id_DataVariable );
  datapool.AddToDictionary( "", "INTENS_REVISION", DataDictionary::type_String, 0, true );
  IdManager::Instance().registerId( "INTENS_REVISION", IdManager::id_DataVariable );

  // Meta Data
  if (AppData::Instance().OpenTelemetryMetadata()) {
    datapool.AddToDictionary( "", "OPENTELEMETRY_METADATA_IN", DataDictionary::type_CharData, 0, true );
    IdManager::Instance().registerId( "OPENTELEMETRY_METADATA_IN", IdManager::id_DataVariable );
    datapool.AddToDictionary( "", "OPENTELEMETRY_METADATA_OUT", DataDictionary::type_CharData, 0, true );
    IdManager::Instance().registerId( "OPENTELEMETRY_METADATA_OUT", IdManager::id_DataVariable );
  }

  // Rest Service
  datapool.AddToDictionary( "", "RESTUSERNAME", DataDictionary::type_String, 0, true );
  IdManager::Instance().registerId( "RESTUSERNAME", IdManager::id_DataVariable );
  datapool.AddToDictionary( "", "RESTUSERNAMELIST", DataDictionary::type_String, 0, true );
  IdManager::Instance().registerId( "RESTUSERNAMELIST", IdManager::id_DataVariable );
  datapool.AddToDictionary( "", "RESTBASE", DataDictionary::type_String, 0, true );
  IdManager::Instance().registerId( "RESTBASE", IdManager::id_DataVariable );
  // REST_SERVICE
  datapool.AddToDictionary( "", "REST_SERVICE@intens", DataDictionary::type_StructDefinition);
  IdManager::Instance().registerId( "REST_SERVICE@intens", IdManager::id_DataStructure );
  dict = datapool.AddToDictionary( "", "REST_SERVICE", DataDictionary::type_StructVariable, "REST_SERVICE@intens", true);
  static_cast<UserAttr*>(dict->GetAttr())->SetScalar();
  dict->setItemWithoutTTrail();
  // REST_SERVICE.*
  // Logon Status (OK, ABORT, CANCELED, OLD_DB, OLD_APP)
  datapool.AddToDictionary( "REST_SERVICE@intens", "LOGON_STATUS", DataDictionary::type_String );
  // Für automatische Versionkontrolle zwischen Datenbank und Applikation
  datapool.AddToDictionary( "REST_SERVICE@intens", "APP_VERSION_MAJOR", DataDictionary::type_Integer );
  datapool.AddToDictionary( "REST_SERVICE@intens", "APP_VERSION_MINOR", DataDictionary::type_Integer );
  datapool.AddToDictionary( "REST_SERVICE@intens", "APP_VERSION_PATCH", DataDictionary::type_Integer );
  // Verlangte minimale DB Version (AppVersionCtrl app_major, app_minor, app_patch)
  datapool.AddToDictionary( "REST_SERVICE@intens", "DB_VERSION_MAJOR", DataDictionary::type_Integer );
  datapool.AddToDictionary( "REST_SERVICE@intens", "DB_VERSION_MINOR", DataDictionary::type_Integer );
  datapool.AddToDictionary( "REST_SERVICE@intens", "DB_VERSION_PATCH", DataDictionary::type_Integer );
  // Keine Versionskontrolle (wenn > 0)
  datapool.AddToDictionary( "REST_SERVICE@intens", "DB_VERSION_IGNORE", DataDictionary::type_Integer );
  // Meldungen im Login Dialog
  datapool.AddToDictionary( "REST_SERVICE@intens", "MESSAGE_UNAUTHORIZED", DataDictionary::type_String );
  datapool.AddToDictionary( "REST_SERVICE@intens", "MESSAGE_CURLERROR", DataDictionary::type_String );
  // Meldungen im Login Dialog
  datapool.AddToDictionary( "REST_SERVICE@intens", "MESSAGE_DIALOG_TITLE", DataDictionary::type_String );

  // Plot
  datapool.AddToDictionary( "", "PLOT2D_UIMODE", DataDictionary::type_String, 0, true );
  IdManager::Instance().registerId( "PLOT2D_UIMODE", IdManager::id_DataVariable );
  datapool.AddToDictionary( "", "PLOT2D_SYMBOLSIZE", DataDictionary::type_Integer);
  IdManager::Instance().registerId( "PLOT2D_SYMBOLSIZE", IdManager::id_DataVariable );

  // --------------
  // Result Rectangle Struct
  // --------------
  datapool.AddToDictionary( "", "RECTANGLE_STRUCT@intens", DataDictionary::type_StructDefinition);
  IdManager::Instance().registerId( "RECTANGLE_STRUCT@intens", IdManager::id_DataStructure );
  datapool.AddToDictionary( "", "Global_Rect", DataDictionary::type_StructVariable, "RECTANGLE_STRUCT@intens", true);
  IdManager::Instance().registerId( "Global_Rect", IdManager::id_DataVariable );
  datapool.AddToDictionary( "RECTANGLE_STRUCT@intens", "X1", DataDictionary::type_Real );
  datapool.AddToDictionary( "RECTANGLE_STRUCT@intens", "Y1", DataDictionary::type_Real );
  datapool.AddToDictionary( "RECTANGLE_STRUCT@intens", "X2", DataDictionary::type_Real );
  datapool.AddToDictionary( "RECTANGLE_STRUCT@intens", "Y2", DataDictionary::type_Real );

  // --------------
  // Result Point Struct
  // --------------
  datapool.AddToDictionary( "", "POINT_STRUCT@intens", DataDictionary::type_StructDefinition);
  IdManager::Instance().registerId( "POINT_STRUCT@intens", IdManager::id_DataStructure );
  datapool.AddToDictionary( "", "Global_Point", DataDictionary::type_StructVariable, "POINT_STRUCT@intens", true);
  IdManager::Instance().registerId( "Global_Point", IdManager::id_DataVariable );
  datapool.AddToDictionary( "POINT_STRUCT@intens", "X", DataDictionary::type_Real );
  datapool.AddToDictionary( "POINT_STRUCT@intens", "Y", DataDictionary::type_Real );
  datapool.AddToDictionary( "POINT_STRUCT@intens", "Y2", DataDictionary::type_Real );

  // --------------
  // Result Diff Struct
  // --------------
  // tags
  IdManager::Instance().registerId( "TDiffIcon", IdManager::id_Tag );
  IdManager::Instance().registerId( "TDiffName", IdManager::id_Tag );
  for (int i = 0; i < 10; i++) {
    std::ostringstream os;
    os << "TDiffValue" << "_" << i;
    IdManager::Instance().registerId( os.str(), IdManager::id_Tag );
  }

  // --------------
  // ProgressBar Dialog Items
  // --------------
  datapool.AddToDictionary( "", "PROGRESSDIALOG", DataDictionary::type_StructDefinition);
  IdManager::Instance().registerId( "PROGRESSDIALOG", IdManager::id_DataStructure );
  IdManager::Instance().registerId( DialogProgressBar::DIALOG_NAME, IdManager::id_Form );
  IdManager::Instance().registerId( DialogProgressBar::FIELDGROUP_NAME, IdManager::id_Fieldgroup );
  dict = datapool.AddToDictionary( "", "ProgressDialog", DataDictionary::type_StructVariable, "PROGRESSDIALOG", true);
  static_cast<UserAttr*>(dict->GetAttr())->SetScalar();
  IdManager::Instance().registerId( "ProgressDialog", IdManager::id_DataVariable );
  IdManager::Instance().registerId( "ProgressDialogAbortCommand", IdManager::id_DataVariable );
  IdManager::Instance().registerId( "ProgressDialogLoopTitle", IdManager::id_DataVariable );
  IdManager::Instance().registerId( "ProgressDialogPixmap", IdManager::id_DataVariable );
  IdManager::Instance().registerId( "ProgressDialogDetailPixmap", IdManager::id_DataVariable );
  datapool.AddToDictionary( "PROGRESSDIALOG", "WindowTitle", DataDictionary::type_String );
  datapool.AddToDictionary( "PROGRESSDIALOG", "MainTitle", DataDictionary::type_String );
  datapool.AddToDictionary( "PROGRESSDIALOG", "MainPercent", DataDictionary::type_Integer );
  dict = datapool.AddToDictionary( "PROGRESSDIALOG", "MainFooter", DataDictionary::type_String );
  static_cast<UserAttr*>(dict->GetAttr())->SetLabelType();
  datapool.AddToDictionary( "PROGRESSDIALOG", "SubTitle", DataDictionary::type_String );
  datapool.AddToDictionary( "PROGRESSDIALOG", "SubPercent", DataDictionary::type_Integer );
  datapool.AddToDictionary( "PROGRESSDIALOG", "ErrorString", DataDictionary::type_String );
  datapool.AddToDictionary( "PROGRESSDIALOG", "DataTitle", DataDictionary::type_String);
  datapool.AddToDictionary( "PROGRESSDIALOG", "DataFooter", DataDictionary::type_String);
  datapool.AddToDictionary( "PROGRESSDIALOG", "DataLabel", DataDictionary::type_String);
  datapool.AddToDictionary( "PROGRESSDIALOG", "Data", DataDictionary::type_String );
  dict = datapool.AddToDictionary( "", "ProgressDialogAbortCommand", DataDictionary::type_String );
  static_cast<UserAttr*>(dict->GetAttr())->SetScalar();
  datapool.AddToDictionary( "", "ProgressDialogLoopTitle", DataDictionary::type_String );
  dict = datapool.AddToDictionary( "", "ProgressDialogPixmap", DataDictionary::type_CharData );
  static_cast<UserAttr*>(dict->GetAttr())->SetScalar();
  dict = datapool.AddToDictionary( "", "ProgressDialogDetailPixmap", DataDictionary::type_CharData );
  static_cast<UserAttr*>(dict->GetAttr())->SetScalar();

  // --------------
  // Function Argument Items used by WebApi
  // --------------
  datapool.AddToDictionary( "", "FUNCTION_ARGS", DataDictionary::type_StructDefinition);
  IdManager::Instance().registerId( "FUNCTION_ARGS", IdManager::id_DataStructure );
  dict = datapool.AddToDictionary( "", "mqReply_functionArgs", DataDictionary::type_StructVariable, "FUNCTION_ARGS", true);
  static_cast<UserAttr*>(dict->GetAttr())->SetScalar();
  IdManager::Instance().registerId( "mqReply_functionArgs", IdManager::id_DataVariable );
  datapool.AddToDictionary( "FUNCTION_ARGS", "name", DataDictionary::type_String );
  datapool.AddToDictionary( "FUNCTION_ARGS", "this", DataDictionary::type_String );
  datapool.AddToDictionary( "FUNCTION_ARGS", "source", DataDictionary::type_String );
  datapool.AddToDictionary( "FUNCTION_ARGS", "source2", DataDictionary::type_String );
  datapool.AddToDictionary( "FUNCTION_ARGS", "reason", DataDictionary::type_String );
  datapool.AddToDictionary( "FUNCTION_ARGS", "value", DataDictionary::type_String );
  datapool.AddToDictionary( "FUNCTION_ARGS", "old_value", DataDictionary::type_String );
  datapool.AddToDictionary( "FUNCTION_ARGS", "data", DataDictionary::type_String );
  datapool.AddToDictionary( "FUNCTION_ARGS", "index", DataDictionary::type_Integer );
  datapool.AddToDictionary( "FUNCTION_ARGS", "select_index", DataDictionary::type_Integer );
  datapool.AddToDictionary( "FUNCTION_ARGS", "guielement", DataDictionary::type_String );
  datapool.AddToDictionary( "FUNCTION_ARGS", "x_pos", DataDictionary::type_Real );
  datapool.AddToDictionary( "FUNCTION_ARGS", "y_pos", DataDictionary::type_Real );
  datapool.AddToDictionary( "FUNCTION_ARGS", "form_name", DataDictionary::type_String );

  // --------------
  // Compare Dialog Items
  // --------------
  datapool.AddToDictionary( "", "COMPAREDIALOG", DataDictionary::type_StructDefinition);
  IdManager::Instance().registerId( "COMPAREDIALOG", IdManager::id_DataStructure );
  IdManager::Instance().registerId( "COMPARE_DIALOG", IdManager::id_Form );
  datapool.AddToDictionary( "", DialogCompare::StructVarname,
			    DataDictionary::type_StructVariable, "COMPAREDIALOG", true);
  IdManager::Instance().registerId( "CompareDialog", IdManager::id_DataVariable );
  datapool.AddToDictionary( "COMPAREDIALOG", DialogCompare::QuerySourceVarname, DataDictionary::type_String );
  datapool.AddToDictionary( "COMPAREDIALOG", DialogCompare::QueryDestinationVarname, DataDictionary::type_String );
  datapool.AddToDictionary( "COMPAREDIALOG", DialogCompare::ItemVarname, DataDictionary::type_String );
  datapool.AddToDictionary( "COMPAREDIALOG", DialogCompare::ItemLabelVarname, DataDictionary::type_String );
  datapool.AddToDictionary( "COMPAREDIALOG", DialogCompare::ItemIgnoreVarname, DataDictionary::type_String );
  datapool.AddToDictionary( "COMPAREDIALOG", DialogCompare::HeaderVarname, DataDictionary::type_String );
  datapool.AddToDictionary( "COMPAREDIALOG", DialogCompare::XAnnotationAxisVarname, DataDictionary::type_Real );
  datapool.AddToDictionary( "COMPAREDIALOG", DialogCompare::XAnnotationValueVarname, DataDictionary::type_Real );
  datapool.AddToDictionary( "COMPAREDIALOG", DialogCompare::XAnnotationLabelVarname, DataDictionary::type_String );

  // ---------------
  // DataPool Cycles
  // ---------------
  datapool.AddToDictionary( ""
			    , INTERNAL_CYCLE_LIST
			    , DataDictionary::type_String, 0, true );
  m_cycle_ref = getDataReference( INTERNAL_CYCLE_LIST );
  dict = datapool.AddToDictionary( ""
				   , INTERNAL_CYCLE_NAME
				   , DataDictionary::type_String, 0, true );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();

  // ------------------------------
  // Structure Definition fuer Sets
  // ------------------------------
  datapool.AddToDictionary( "", "SetPair", DataDictionary::type_StructDefinition );
  dict = datapool.AddToDictionary( "SetPair", "Input", DataDictionary::type_String );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( "SetPair", "Output", DataDictionary::type_String );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( "SetPair", "Strings", DataDictionary::type_Integer );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();

  // ---------------------------------
  // Structure Definition fuer Streams
  // ---------------------------------
  dict = datapool.AddToDictionary( ""
				   , INTERNAL_STREAM_STRUCT
				   , DataDictionary::type_StructDefinition );
  assert( dict != 0 );
  dict = datapool.AddToDictionary( INTERNAL_STREAM_STRUCT
				   , "Buffer"
				   , DataDictionary::type_String );
  assert( dict != 0 );
  dict = datapool.AddToDictionary( INTERNAL_STREAM_STRUCT
				   , "ItemsAreValid"
				   , DataDictionary::type_Integer );
  assert( dict != 0 );
  dict = datapool.AddToDictionary( INTERNAL_STREAM_STRUCT
				   , "SourceItemsAreValid"
				   , DataDictionary::type_Integer );
  assert( dict != 0 );
  dict = datapool.AddToDictionary( INTERNAL_STREAM_STRUCT
				   , "Filename"
				   , DataDictionary::type_String );
  assert( dict != 0 );

  // -----------------------------------
  // Structure Definition fuer ListGraph
  // -----------------------------------
  DataSet *dataset = newDataSet( "@ListGraphOptionsSet", false, true );
  dataset->setItemValues( _("Automatic"), 1 );     // Dynamic
  dataset->setItemValues( _("User Defined"), 2 );  // Static

  dict = datapool.AddToDictionary( ""
				   , INTERNAL_LISTGRAPH_STRUCT
				   , DataDictionary::type_StructDefinition );
  dict = datapool.AddToDictionary( INTERNAL_LISTGRAPH_STRUCT
				   , "name"
				   , DataDictionary::type_String );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( INTERNAL_LISTGRAPH_STRUCT
				   , "min"
				   , DataDictionary::type_Real );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( INTERNAL_LISTGRAPH_STRUCT
				   , "max"
				   , DataDictionary::type_Real );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( INTERNAL_LISTGRAPH_STRUCT
				   , "state"
				   , DataDictionary::type_Integer );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  static_cast<UserAttr*>(dict->GetAttr())->SetDataSetName( "@ListGraphOptionsSet" );
  static_cast<UserAttr*>(dict->GetAttr())->SetCombobox();

  // -------------------------------------
  // Structure Definition fuer GuiXrtGraph
  // -------------------------------------
  dict = datapool.AddToDictionary( ""
				   , INTERNAL_XRTGRAPH_STRUCT
				   , DataDictionary::type_StructDefinition );
  dict = datapool.AddToDictionary( INTERNAL_XRTGRAPH_STRUCT
				   , "y1axis"
				   , DataDictionary::type_Integer );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( INTERNAL_XRTGRAPH_STRUCT
				   , "y2axis"
				   , DataDictionary::type_Integer );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( INTERNAL_XRTGRAPH_STRUCT
				   , "xaxis"
				   , DataDictionary::type_Integer );
  // color picker
  dict = datapool.AddToDictionary( INTERNAL_XRTGRAPH_STRUCT
				   , "color"
				   , DataDictionary::type_String );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  static_cast<UserAttr*>(dict->GetAttr())->setColorPicker();
  static_cast<UserAttr*>(dict->GetAttr())->SetLabel(" ");
  static_cast<UserAttr*>(dict->GetAttr())->SetButton();
  // color picker
  dict = datapool.AddToDictionary( INTERNAL_XRTGRAPH_STRUCT
				   , "symbolColor"
				   , DataDictionary::type_String );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  static_cast<UserAttr*>(dict->GetAttr())->setColorPicker();
  static_cast<UserAttr*>(dict->GetAttr())->SetLabel(" ");
  static_cast<UserAttr*>(dict->GetAttr())->SetButton();
  dict = datapool.AddToDictionary( INTERNAL_XRTGRAPH_STRUCT
				   , "min"
				   , DataDictionary::type_Real );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( INTERNAL_XRTGRAPH_STRUCT
				   , "max"
				   , DataDictionary::type_Real );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( INTERNAL_XRTGRAPH_STRUCT
				   , "scaleEnable"
				   , DataDictionary::type_Integer );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( INTERNAL_XRTGRAPH_STRUCT
				   , "aspectRatio"
				   , DataDictionary::type_Real );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( INTERNAL_XRTGRAPH_STRUCT
				   , "aspectRatioType"
				   , DataDictionary::type_Integer );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  static_cast<UserAttr*>(dict->GetAttr())->SetCombobox();
  dict = datapool.AddToDictionary( INTERNAL_XRTGRAPH_STRUCT
				   , "showCycle"
				   , DataDictionary::type_Integer );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();

  // -----------------------------------
  // Structure Definition fuer Gui3dPlot
  // -----------------------------------
  dict = datapool.AddToDictionary( ""
				   , INTERNAL_XRT3DPLOT_STRUCT
				   , DataDictionary::type_StructDefinition );
  dict = datapool.AddToDictionary( INTERNAL_XRT3DPLOT_STRUCT
				   , "NumDistnLevels"
				   , DataDictionary::type_Integer );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( INTERNAL_XRT3DPLOT_STRUCT
				   , "DistnMethod"
				   , DataDictionary::type_Integer );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dict = datapool.AddToDictionary( INTERNAL_XRT3DPLOT_STRUCT
				   , "DistnTable"
				   , DataDictionary::type_Real );
  static_cast<UserAttr*>(dict->GetAttr())->SetEditable();

  dict = datapool.AddToDictionary( INTERNAL_XRT3DPLOT_STRUCT
				   , "rotationX"
				   , DataDictionary::type_Integer );
  dynamic_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dynamic_cast<UserAttr*>(dict->GetAttr())->SetSlider();
  dict = datapool.AddToDictionary( INTERNAL_XRT3DPLOT_STRUCT
				   , "rotationY"
				   , DataDictionary::type_Integer );
  dynamic_cast<UserAttr*>(dict->GetAttr())->SetEditable();
  dynamic_cast<UserAttr*>(dict->GetAttr())->SetSlider();

}

/* --------------------------------------------------------------------------- */
/* createDataPool --                                                           */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::createDataPool(){
  DataPool &datapool = getDataPool();

  datapool.NewCycle();
  setCycleName( 0, _("Base") );

  createDataSetItems();
  // Die Set-Variablen werden als erstes generiert. Damit die GuiCombobox
  // beim ersten update() nicht die Picklist neu generiert, wird sie schon
  // jetzt ganz alt gemacht. Der GuiManager beginnt mit 1.
  DataPoolIntens::NewTransaction();

  DataReference *data;
  data = getDataReference( "USER" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  data->SetValue( AppData::Instance().RealName() );
  delete data;

  data = getDataReference( "HOST" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  data->SetValue( AppData::Instance().HostName() );
  delete data;

  data = getDataReference( "IPADDR" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  data->SetValue( getAddrInfo() );
  delete data;

  data = getDataReference( "INTENS_VERSION" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  data->SetValue( AppData::Instance().Version() );
  delete data;

  std::istringstream is(AppData::Instance().Version());
  int major, minor;
  std::string patch;
  char dot;
  is >> major >> dot >> minor >> dot >> patch;

  data = getDataReference( "INTENS_VERSION_MAJOR" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  data->SetValue( major );
  delete data;

  data = getDataReference( "INTENS_VERSION_MINOR" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  data->SetValue( minor );
  delete data;

  data = getDataReference( "INTENS_VERSION_PATCH" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  data->SetValue( patch );
  delete data;

  data = getDataReference( "INTENS_REVISION" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  data->SetValue( AppData::Instance().Revision() );
  delete data;

  data = getDataReference( "DATE" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  data->SetValue( GetDate() );
  delete data;

  data = getDataReference( "RESTUSERNAME" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  static_cast<UserAttr*>(data->getUserAttr())->SetLabelType();
  delete data;

  data = getDataReference( "RESTUSERNAMELIST" );
  static_cast<UserAttr*>(data->getUserAttr())->SetLabelType();
  delete data;

  data = getDataReference( "RESTBASE" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  static_cast<UserAttr*>(data->getUserAttr())->SetLabelType();
  delete data;

  data = getDataReference( "PLOT2D_UIMODE" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  static_cast<UserAttr*>(data->getUserAttr())->SetLabelType();
  delete data;

  data = getDataReference( "PLOT2D_SYMBOLSIZE" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  delete data;

  data = getDataReference( "Global_Rect.X1" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  delete data;

  data = getDataReference( "Global_Rect.Y1" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  delete data;

  data = getDataReference( "Global_Rect.X2" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  delete data;

  data = getDataReference( "Global_Rect.Y2" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  delete data;

  data = getDataReference( "Global_Point.X" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  delete data;

  data = getDataReference( "Global_Point.Y" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  delete data;

  data = getDataReference( "Global_Point.Y2" );
  static_cast<UserAttr*>(data->getUserAttr())->SetScalar();
  delete data;
}

/* --------------------------------------------------------------------------- */
/* createDataSetItems --                                                       */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::createDataSetItems(){
  DataSetList::iterator di;
  for( di = m_datasetlist.begin(); di != m_datasetlist.end(); ++di ){
    createDataSet( (*di).first, (*di).second );
  }
  NewTransaction();
}

/* --------------------------------------------------------------------------- */
/* createDataSet --                                                            */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::createDataSet( const std::string &name, DataSet *dataset ){
  dataset->create( name );
}

/* --------------------------------------------------------------------------- */
/* NewTransaction --                                                           */
/* --------------------------------------------------------------------------- */

TransactionNumber DataPoolIntens::NewTransaction(){
  s_transaction = getDataPool().NewTransaction();
  return s_transaction;
}

/* --------------------------------------------------------------------------- */
/* CurrentTransaction --                                                       */
/* --------------------------------------------------------------------------- */

TransactionNumber DataPoolIntens::CurrentTransaction(){
  return s_transaction;
}

/* --------------------------------------------------------------------------- */
/* setNamespace --                                                             */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::setNamespace( const std::string &name ){
  m_namespace = name;
}

/* --------------------------------------------------------------------------- */
/* noNamespace --                                                              */
/* --------------------------------------------------------------------------- */

bool DataPoolIntens::noNamespace(){
  return m_namespace.empty();
}

/* --------------------------------------------------------------------------- */
/* getAndResetNamespace --                                                     */
/* --------------------------------------------------------------------------- */

DataReference *DataPoolIntens::getAndResetNamespace(){
  m_namespace = "";
  m_namespace_dict = 0;
  DataReference *ref = 0;
  if( m_namespace_ref != 0 ){
    ref = m_namespace_ref;
    m_namespace_ref = 0;
  }
  return ref;
}

/* --------------------------------------------------------------------------- */
/* createNamespace --                                                          */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::createNamespace( std::string &parent ){
  if( m_namespace.empty() ){
    return;
  }

  DataPool &datapool = getDataPool();
  parent = "@LocalVariablesOf" + m_namespace;
  if( m_namespace_dict == 0 ){
    m_namespace_dict = datapool.AddToDictionary( "",
                                                 parent,
                                                 DataDictionary::type_StructDefinition );
    assert( m_namespace_dict != 0 );
    DataDictionary *dict = 0;
    dict = datapool.AddToDictionary( "",
                                     m_namespace,
                                     DataDictionary::type_StructVariable,
                                     parent.c_str(),
                                     true ); // global item
    assert( dict != 0 );
  }
}

/* --------------------------------------------------------------------------- */
/* newDataReferenceInNamespace --                                              */
/* --------------------------------------------------------------------------- */

DataReference *DataPoolIntens::newDataReferenceInNamespace( const std::string &name ){
  if( m_namespace.empty() ) return 0;

  // Wir suchen die StrukturVariable
  if( m_namespace_ref == 0 ){
    m_namespace_ref = getDataPool().newDataReference( m_namespace.c_str() );
    if( !m_namespace_ref->RefValid() ){
      // Keine StrukturVariable vorhanden. Es wurde keine lokale Variable
      // deklariert. Wir können den Namespace vorzeitig eliminieren.
      delete m_namespace_ref;
      m_namespace_ref = 0;
      m_namespace = "";
      return 0;
    }
  }

  // OK. Es gibt scheinbar lokale Variablen.
  return getDataReference( m_namespace_ref, name );
}

/* --------------------------------------------------------------------------- */
/* addToDictionary --                                                          */
/* --------------------------------------------------------------------------- */

DataDictionary *DataPoolIntens::addToDictionary( const std::string &structName,
                                                 const std::string &name,
                                                 const DataDictionary::DataType type ){
  std::string parent;
  if( !structName.empty() ){
    parent = structName;
  }
  else{
    // Feature für lokale Variablen von Funktionen und Tasks.
    Instance().createNamespace( parent );
  }

  DataDictionary *dict;
  dict = getDataPool().AddToDictionary( parent.c_str(), name, type );
  assert( dict != 0 );
  return dict;
}

/* --------------------------------------------------------------------------- */
/* addToDictionary --                                                          */
/* --------------------------------------------------------------------------- */

DataDictionary *DataPoolIntens::addToDictionary( const std::string &structName,
                                                 const std::string &name,
                                                 const std::string &structDef ){
  std::string parent;
  if( !structName.empty() ){
    parent = structName;
  }
  else{
    // Feature für lokale Variablen von Funktionen und Tasks.
    Instance().createNamespace( parent );
  }

  DataDictionary *dict;
  dict = getDataPool().AddToDictionary( parent.c_str(), name
					, DataDictionary::type_StructVariable
					, structDef.c_str() );
  assert( dict != 0 );
  return dict;
}

/* --------------------------------------------------------------------------- */
/* addDataVariable --                                                          */
/* --------------------------------------------------------------------------- */

DataDictionary *DataPoolIntens::addDataVariable( const std::string &structName,
                                                 const std::string &name,
                                                 const DataDictionary::DataType type,
                                                 bool isGlobal,
                                                 bool isInternal ){
  std::string parent;
  if( !structName.empty() ){
    parent   = structName;
    isGlobal = false;
  }
  else{
    // Feature für lokale Variablen von Funktionen und Tasks.
    Instance().createNamespace( parent );
  }
  return getDataPool().AddToDictionary( parent.c_str(),
                                        name,
                                        type,
                                        0,
                                        isGlobal,
                                        isInternal );
}

/* --------------------------------------------------------------------------- */
/* addStructureVariable --                                                     */
/* --------------------------------------------------------------------------- */

DataDictionary *DataPoolIntens::addStructureVariable( const std::string &structName,
						      const std::string &name,
                                                      const std::string &structDef,
                                                      bool isGlobal,
                                                      bool isInternal ){
  std::string parent;
  if( !structName.empty() ){
    parent   = structName;
    isGlobal = false;
  }
  else{
    // Feature für lokale Variablen von Funktionen und Tasks.
    Instance().createNamespace( parent );
  }
  return getDataPool().AddToDictionary( parent.c_str(), name,
                                        DataDictionary::type_StructVariable,
                                        structDef.c_str(),
                                        isGlobal,
                                        isInternal );
}

/* --------------------------------------------------------------------------- */
/* addStructureDefinition --                                                   */
/* --------------------------------------------------------------------------- */

DataDictionary *DataPoolIntens::addStructureDefinition( const std::string &name ){
  return getDataPool().AddToDictionary( "",
                                        name.c_str(),
					DataDictionary::type_StructDefinition );
}

/* --------------------------------------------------------------------------- */
/* copyStructureDefinition --                                                  */
/* --------------------------------------------------------------------------- */

bool DataPoolIntens::copyStructureDefinition( DataDictionary *dest, DataDictionary *src ){
  dest->copyDataDictionary( *src );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getDataReference --                                                         */
/* --------------------------------------------------------------------------- */

DataReference *DataPoolIntens::getDataReference( const std::string &name ){
  DataReference *ref = Instance().newDataReferenceInNamespace( name );
  if( ref == 0 ){
    ref = getDataPool().newDataReference( name.c_str() );
  }
  if( ref->RefValid() ){
    return ref;
  }
  delete ref;
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getDataReference --                                                         */
/* --------------------------------------------------------------------------- */

DataReference *DataPoolIntens::getDataReference( const DataReference *root,
                                                 const std::string &name ){
  if( root != 0 ){
    DataReference *ref = DataPool::newDataReference( *root, name.c_str() );
    if( ref->RefValid() ){
      return ref;
    }
    delete ref;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* newDataSet --                                                               */
/* --------------------------------------------------------------------------- */

DataSet *DataPoolIntens::newDataSet( const std::string &name,
                                     bool isGlobal,
                                     bool isInternal ){
  DataSetList::iterator di = m_datasetlist.find( name );
  if( di != m_datasetlist.end() ){
    return 0;
  }
  DataSet *ds = new DataSet;
  DataDictionary *dict;
  dict = getDataPool().AddToDictionary( "",
                                        name,
					DataDictionary::type_StructVariable,
					"SetPair",
                                        isGlobal,
                                        isInternal );
  if( dict == 0 ){
    return 0;
  }
  IdManager::Instance().registerId( name, IdManager::id_DataVariable );
  m_datasetlist.insert( DataSetList::value_type( name, ds ) );
  return ds;
}

/* --------------------------------------------------------------------------- */
/* getDataSet --                                                               */
/* --------------------------------------------------------------------------- */

DataSet *DataPoolIntens::getDataSet( const std::string &name ){
  DataSetList::iterator di = m_datasetlist.find( name );
  if( di == m_datasetlist.end() ){
    return 0;
  }
  return (*di).second;
}

/* --------------------------------------------------------------------------- */
/* newColorSet --                                                              */
/* --------------------------------------------------------------------------- */

ColorSet *DataPoolIntens::newColorSet( const std::string &name ){
  ColorSetList::iterator i = m_colorsetlist.find( name );
  if( i != m_colorsetlist.end() ){
    return 0;
  }
  ColorSet *cs = new ColorSet;

  m_colorsetlist.insert( ColorSetList::value_type( name, cs ) );
  return cs;
}

/* --------------------------------------------------------------------------- */
/* getColorSet --                                                              */
/* --------------------------------------------------------------------------- */

ColorSet *DataPoolIntens::getColorSet( const std::string &name ){
  ColorSetList::iterator i = m_colorsetlist.find( name );
  if( i == m_colorsetlist.end() ){
    return 0;
  }
  return (*i).second;
}

/* --------------------------------------------------------------------------- */
/* BeginDataPoolTransaction --                                                 */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::BeginDataPoolTransaction( TransactionOwner *owner ){
  assert( s_datapool != 0 );

  s_datapool->BeginDataPoolTransaction( owner );
}

/* --------------------------------------------------------------------------- */
/* RollbackDataPoolTransaction --                                              */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::RollbackDataPoolTransaction( TransactionOwner *owner ){
  assert( s_datapool != 0 );
  s_datapool->RollbackDataPoolTransaction( owner );
}

/* --------------------------------------------------------------------------- */
/* CommitDataPoolTransaction --                                                */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::CommitDataPoolTransaction( TransactionOwner *owner ){
  assert( s_datapool != 0 );

  s_datapool->CommitDataPoolTransaction( owner );
  NewTransaction();
}

/* --------------------------------------------------------------------------- */
/* setTargetStream --                                                          */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::setTargetStream( DataReference *ref, BasicStream *str ){
  std::string name;
  ref->getFullName( name );
  m_targets.setTargetStream( name, str );
}

/* --------------------------------------------------------------------------- */
/* fixupItemStreams --                                                         */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::fixupItemStreams( BasicStream &stream, DataReference *ref ){
  std::string name;
  ref->getFullName( name );
  TargetStreamList *slist = m_targets.getTargetStreamList( name );
  if( slist == 0 ){
    return;
  }
  stream.addItemStreams( *slist );
}

/* --------------------------------------------------------------------------- */
/* fixupItemStreams --                                                         */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::fixupItemStreams( BasicStream &stream, const std::string &name ){
  TargetStreamList *slist = m_targets.getTargetStreamList( name );
  if( slist == 0 ){
    return;
  }
  stream.addItemStreams( *slist );
}

/* --------------------------------------------------------------------------- */
/* appendParentTargetStreamList --                                             */
/* --------------------------------------------------------------------------- */

TargetStreamList* DataPoolIntens::appendParentTargetStreamList(std::string& name, TargetStreamList* srcList) {
  std::string::size_type pos;
  // append parent targetStreamList  (eg. name "project.motor.name" look at "project.motor" and "project"
  while ((pos=name.find_last_of(".")) != std::string::npos ) {
    name = name.substr(0, pos);
    TargetStreamList *targets2 = m_targets.getTargetStreamList( name );
    if (targets2) {
      if (!srcList) {
	srcList = targets2;
	continue;
      }
      TargetStreamList::iterator it;
      for( it = targets2->begin(); it != targets2->end(); ++it ){
	if (std::find(srcList->begin(),srcList->end(), (*it))==srcList->end()) {
	  srcList->push_back(*it);
	}
      }
    }
  }
  return srcList;
}

/* --------------------------------------------------------------------------- */
/* checkTargetStreams --                                                       */
/* --------------------------------------------------------------------------- */

bool DataPoolIntens::checkTargetStreams( DataReference *ref, std::string& error_msg ){
  std::string name;
  error_msg.clear();
  ref->getFullName( name );
  BUG_DEBUG( "DataPoolIntens::checkTargetStreams " << name );

  // if noDependencies flag, no check will be made
  if( static_cast<UserAttr*>(ref->getUserAttr())->hasNoDependencies() ){
    BUG_DEBUG(  "hasNoDependencies, ignore Targetstreams" );
    return false;
  }

  TargetStreamList *targets = m_targets.getTargetStreamList( name );
  // append targetStreamLists of parent dataitems
  targets = appendParentTargetStreamList(name, targets);

  if( targets == 0 ){
    BUG_DEBUG(  "no Targetstreams" );
    return false;
  }
  TransactionNumber timestamp = NewTransaction();
  TargetStreamList::iterator it;
  for( it = targets->begin(); it != targets->end(); ++it ){
    if( (*it)->checkTargetStreams( timestamp ) ){
      BUG_INFO(compose(_("a target from input stream named '%1' is valid"), (*it)->Name()) );
      error_msg += "\n" + (*it)->getErrorMessage();
    }
  }
  return error_msg.size() > 0;
}

/* --------------------------------------------------------------------------- */
/* clearTargetStreams --                                                       */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::clearTargetStreams( DataReference *ref ){
  std::string name;
  ref->getFullName( name );
  BUG_DEBUG("DataPoolIntens::clearTargetStreams"<< name );

  TargetStreamList *targets = m_targets.getTargetStreamList( name );
  // append targetStreamLists of parent dataitems
  targets = appendParentTargetStreamList(name, targets);

  if( targets == 0 ){
    BUG_DEBUG(  "no Targetstreams" );
    return;
  }
  TransactionNumber timestamp = NewTransaction();

  TargetStreamList::iterator it;
  for( it = targets->begin(); it != targets->end(); ++it ){
    (*it)->clearTargetStreams( timestamp );
  }
}

/* --------------------------------------------------------------------------- */
/* printTargetContainer --                                                     */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::printTargetContainer() {
  m_targets.print();
}

/* --------------------------------------------------------------------------- */
/* clearCycle --                                                               */
/* --------------------------------------------------------------------------- */

bool DataPoolIntens::clearCycle( int cyclenum ){
  int num = s_datapool->NumCycles();
  if( cyclenum < num ){
    s_datapool->ClearCycle( cyclenum );
    createDataSetItems();
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* newCycle --                                                                 */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::newCycle( const std::string &title ){
  s_datapool->NewCycle();
  int current = s_datapool->GetCurrentCycle();
  if( title.empty() ){
    setCycleName( current, "<no name>" );
  }
  else{
    setCycleName( current, title );
  }
}

/* --------------------------------------------------------------------------- */
/* firstCycle --                                                               */
/* --------------------------------------------------------------------------- */

bool DataPoolIntens::firstCycle(){
  int current = s_datapool->GetCurrentCycle();
  if( current > 0 ){
    s_datapool->SetCycle( 0 );
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* lastCycle --                                                                */
/* --------------------------------------------------------------------------- */

bool DataPoolIntens::lastCycle(){
  int current = s_datapool->GetCurrentCycle();
  int num     = s_datapool->NumCycles();
  if( current + 1 != num ){
    s_datapool->SetCycle( num - 1 );
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* currentCycle --                                                             */
/* --------------------------------------------------------------------------- */

int DataPoolIntens::currentCycle(){
  return s_datapool->GetCurrentCycle();
}

/* --------------------------------------------------------------------------- */
/* numCycles --                                                                */
/* --------------------------------------------------------------------------- */

int DataPoolIntens::numCycles(){
  return s_datapool->NumCycles();
}

/* --------------------------------------------------------------------------- */
/* nextCycle --                                                                */
/* --------------------------------------------------------------------------- */

bool DataPoolIntens::nextCycle(){
  int current = s_datapool->GetCurrentCycle();
  int num     = s_datapool->NumCycles();
  if( ++current < num ){
    s_datapool->SetCycle( current );
    return true;
  }
  s_datapool->NewCycle();
  setCycleName( num, "<no name>" );
  return false;
}

/* --------------------------------------------------------------------------- */
/* goCycle --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataPoolIntens::goCycle( int num, bool clearUndoStack ){
  int cycles = s_datapool->NumCycles();
  if( num < cycles ){
    s_datapool->SetCycle( num, clearUndoStack );
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* removeCycle --                                                              */
/* --------------------------------------------------------------------------- */

bool DataPoolIntens::removeCycle( int num ){
  if( s_datapool->NumCycles() <= 1 ){
    return false;
  }
  int current = s_datapool->GetCurrentCycle();
  if( !s_datapool->RemoveCycle( num ) ){
    return false;
  }
  removeCycleName( num );

  if( num == current ){
    return true; // ein impliziter Switch
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setCycleName --                                                             */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::setCycleName( int num, const std::string &name ){
  m_cycle_ref->SetValue( name, num );
}

/* --------------------------------------------------------------------------- */
/* getCycleName --                                                             */
/* --------------------------------------------------------------------------- */

bool DataPoolIntens::getCycleName( int num, std::string &name ){
  return m_cycle_ref->GetValue( name, num );
}

/* --------------------------------------------------------------------------- */
/* isCycleNameUpdated --                                                       */
/* --------------------------------------------------------------------------- */

bool DataPoolIntens::isCycleNameUpdated( int num ){
  DATAAttributeMask mask = m_cycle_ref->getAttributes( GuiManager::Instance().LastGuiUpdate() );
  return (mask & DATAisGuiUpdated) != 0;
}

/* --------------------------------------------------------------------------- */
/* removeCycleName --                                                          */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::removeCycleName( int num ){
  for( int i=num; i < s_datapool->NumCycles(); i++ ) {
    std::string value;
    m_cycle_ref->GetValue( value, i+1 );
    m_cycle_ref->SetValue( value, i );
  }
}

/* --------------------------------------------------------------------------- */
/* printAttributes --                                                          */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::printAttributes( std::ostream &ostr, DATAAttributeMask mask ){
  std::cout << "isValid(" << ( mask & DATAisValid )
	    << ") isLocked(" << ( mask &  DATAisLocked )
	    << ") isUpdated(" << ( mask & DATAisGuiUpdated )
	    << ") editable(" << ( mask & DATAeditable )
	    << ") optional(" << ( mask & DATAoptional )
	    << ") lockable(" << ( mask & DATAlockable )
	    << ") isEditable(" << ( mask & DATAIsEditable )
	    << ") isReadOnly(" << ( mask & DATAIsReadOnly )
	    << ") color("
	    << ( mask & DATAcolor1 )
	    << "," << ( mask & DATAcolor2 )
	    << "," << ( mask & DATAcolor3 )
	    << "," << ( mask & DATAcolor4 )
	    << "," << ( mask & DATAcolor5 )
	    << "," << ( mask & DATAcolor6 )
	    << "," << ( mask & DATAcolor7 )
	    << "," << ( mask & DATAcolor8 )
	    << ")";
}

/* --------------------------------------------------------------------------- */
/* printItems --                                                               */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::printItems( std::ostream &o ){
  DataDictionary *dD = getDataPool().GetRootDict()->getStructure();
  while ( dD ){
    switch (dD->getDataType() ){
    case DataDictionary::type_Integer:
    case DataDictionary::type_Real:
    case DataDictionary::type_String:
    case DataDictionary::type_Complex:
    case DataDictionary::type_CharData:
    case DataDictionary::type_StructVariable:{
      std::string name = dD->getName();
      if( name.find_first_of( "@" ) == std::string::npos &&
          name.find( "__stream_data_" ) == std::string::npos &&
	  name.find( "__filestream_" ) == std::string::npos ){
	UserAttr *attr =  static_cast<UserAttr*>(getDataReference( name )->getUserAttr());
	if( attr != 0 ){
	  if( attr -> IsPersistent() ){
	    o << "PERSISTENT: ";
	  }
	}
	o << name << std::endl;
      }
      break;
    }
    default:
      break;
    }
    dD = dD->getNext();
  }

}

void DataPoolIntens::lspWrite( std::ostream &ostr ){
  for (auto pair : m_datasetlist) {
    auto name = pair.first;
    auto set = pair.second;

    ostr << "<ITEM name=\"" << name << "\"";
    ostr << " set=\"1\"";
    ostr << " file=\"" << set->LSPFilename() << "\"";
    ostr << " line=\"" << set->LSPLineno() << "\"";
    ostr << ">" << std::endl;
    ostr << "</ITEM>" << std::endl;
  }
  for (auto pair : m_colorsetlist) {
    auto name = pair.first;
    auto set = pair.second;

    ostr << "<ITEM name=\"" << name << "\"";
    ostr << " colorset=\"1\"";
    ostr << " file=\"" << set->LSPFilename() << "\"";
    ostr << " line=\"" << set->LSPLineno() << "\"";
    ostr << ">" << std::endl;
    ostr << "</ITEM>" << std::endl;
  }
}


/* --------------------------------------------------------------------------- */
/* printPersistentItems --                                                     */
/* --------------------------------------------------------------------------- */

void DataPoolIntens::printPersistentItems( std::ostream &o, bool restdb ){
  std::vector< std::string > xmlattrs;
  xmlattrs.push_back( "unit" );
  if( !restdb ){
    xmlattrs.push_back( "label" );
    xmlattrs.push_back( "helptext" );
  }
  xmlattrs.push_back( "scalar" );
  xmlattrs.push_back( "matrix" );
  xmlattrs.push_back( "dbattr" );
  xmlattrs.push_back( "dbunit" );

  o << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
  o << "<persistent_items>\n";
  DataDictionary *dD = getDataPool().GetRootDict()->getStructure();
  DataDictionary::DataType dtype;
  while ( dD ){
    dtype = dD->getDataType();
    switch ( dtype ){
    case DataDictionary::type_Integer:
    case DataDictionary::type_Real:
    case DataDictionary::type_String:
    case DataDictionary::type_Complex:
    case DataDictionary::type_CharData:
    case DataDictionary::type_StructVariable:{
      std::string name = dD->getName();
      if( name.find_first_of( "@" ) == std::string::npos
	  && name.find( "__stream_data_" ) == std::string::npos
          && name.find( "__filestream_" ) == std::string::npos ){

        UserAttr *attr =  static_cast<UserAttr*>(getDataReference( name )->getUserAttr());
        if( attr != 0 ){
          // !AppData::Instance().ParserStartToken()
          // => export complete DataPool !!!
          // e.g.: intens --persistfile persist.xml test.des
          if( !AppData::Instance().ParserStartToken() || attr -> IsPersistent() ){
            if( dtype==DataDictionary::type_StructVariable ){
              DataReference *dref = getDataReference( name );
              printPersistentItems( o, xmlattrs, dref );
            }
            else{
              std::string type(typemap[ dtype ]);
              // maybe String type is a Date?
              if (dtype == DataDictionary::type_String &&
                  (dynamic_cast<UserAttr*>(dD->GetAttr())->StringType() == UserAttr::string_kind_date ||
                   dynamic_cast<UserAttr*>(dD->GetAttr())->StringType() == UserAttr::string_kind_datetime))
                type = "DATE";
              o << "<item name=\"" << name << "\"" ;
              o << " type=\"" << type << "\"";
              attr -> write( o, xmlattrs );
              o << "/>" << std::endl;
            }
          }
        }
      }
    }
      break;

    default:
      break;
    }
    dD = dD->getNext();
  }
  o << "</persistent_items>\n";

}

typedef   std::vector< DataReference * > DataReferenceVector;

/* --------------------------------------------------------------------------- */
/* printPersistentItems --                                                     */
/* --------------------------------------------------------------------------- */

// produce xml output that can be used to generate the db interface
void DataPoolIntens::printPersistentItems( std::ostream &o,
					   std::vector< std::string > &xmlattrs,
					   DataReference *d ){

  DataReference::StructIterator structIter;
  DataDictionary::DataType dtype;

  UserAttr *attr =  static_cast<UserAttr*>(d->getUserAttr());
    if( attr != 0 ){
      if ( attr -> IsDbTransient() ) {
	return;
      }
    }

    o << "<item name=\"" << d->GetDict()->getName() << "\"" ;
    o << " type= \"" << d->GetDict()->structure().getName() << "\"";
    if( attr != 0 ){
      attr -> write( o, xmlattrs );
    }
    o << ">" << std::endl;

    std::ostringstream o_nokey; // print non-key elements last
    for( structIter = d->begin();structIter != d->end();++structIter ){
      std::ostream* os( &o_nokey );

      DataReference *newRef = structIter.NewDataReference();
      if ( newRef != 0 ){
	dtype = newRef->GetDict()->getDataType();
	std::string name = newRef->GetDict()->getName();

	if( dtype != DataDictionary::type_StructVariable ){
	  UserAttr *attr =  static_cast<UserAttr*>(newRef->getUserAttr());
	  if( attr != 0 ){
	    if ( attr -> IsDbTransient() ) {
	      continue;
	    }
	  }
	}

	if( dtype == DataDictionary::type_StructVariable ){
	  printPersistentItems( o, xmlattrs, newRef );
	}
	else {
	  std::string type(typemap[ dtype ]);
	    // maybe String type is a Date?
	  if (dtype == DataDictionary::type_String &&
		  (dynamic_cast<UserAttr*>(newRef->getUserAttr())->StringType() == UserAttr::string_kind_date ||
		   dynamic_cast<UserAttr*>(newRef->getUserAttr())->StringType() == UserAttr::string_kind_datetime))
		type = "DATE";
	  *os << "  <item name= \"" << name << "\"";
	  *os << " type= \"" << type << "\"";
	  UserAttr *attr =  static_cast<UserAttr*>(newRef->getUserAttr());
	  if( attr != 0 ){
	    attr -> write( *os, xmlattrs );
	    *os << "/>" << std::endl;
	  }
	}
	delete newRef;
      }
    }
    o << o_nokey.str(); // print non-key elements last

    o << "</item>" << std::endl;
}
