
#include <typeinfo>
#include <algorithm>
#include <vector>

#include "parser/Flexer.h"
#include "utils/gettext.h"

#include "xfer/Scale.h"
#include "xfer/XferScale.h"
#include "InterpreterConfigurator.h"
#include "utils/FileUtilities.h"
#include "gui/GuiManager.h"
#include "gui/GuiPrinterDialog.h"
#include "app/UiManager.h"
#include "app/HelpFile.h"
#include "app/HelpManager.h"
#include "parser/IdManager.h"
#include "app/UserAttr.h"
#include "app/DataSet.h"
#include "app/AppData.h"
#include "app/App.h"
#include "gui/GuiFactory.h"
#include "app/ColorSet.h"
#include "gui/GuiFieldgroup.h"
#include "gui/GuiFieldgroupLine.h"
#include "gui/GuiLabel.h"
#include "gui/GuiPixmap.h"
#include "gui/GuiVoid.h"
#include "gui/GuiStretch.h"
#include "gui/GuiIndex.h"
#include "gui/GuiDataField.h"
#include "gui/GuiForm.h"
#include "gui/GuiOrientationContainer.h"
#include "gui/GuiPulldownMenu.h"
#include "gui/GuiPopupMenu.h"
#include "gui/GuiElement.h"
#include "gui/UnitManager.h"
#include "job/JobFunction.h"
#include "job/JobManager.h"
#include "xfer/XferDataItem.h"
#include "datapool/DataReference.h"
#include "parser/errorhandler.h"
#include "streamer/Stream.h"
#include "streamer/StreamParameter.h"
#include "streamer/StreamManager.h"
#include "streamer/PlotGroupStreamParameter.h"
#include "streamer/DataStreamParameter.h"
#include "streamer/VarStreamParameter.h"
#include "app/Plugin.h"
#include "operator/IntensServerSocket.h"
#include "operator/MFMServerSocket.h"
#include "operator/MessageQueue.h"
#include "operator/MessageQueueSubscriber.h"
#include "operator/MessageQueueReply.h"
#include "operator/FileStream.h"
#include "gui/GuiTimeTable.h"
#include "gui/GuiTable.h"
#include "gui/GuiList.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiSeparator.h"
#include "plot/ListPlot.h"
#include "plot/ListGraph.h"
#include "plot/Plot2dMenuDescription.h"
#include "gui/GuiPlotDataItem.h"
#include "plot/Simpel.h"
#include "plot/PSPlot.h"
#include "gui/Gui3dPlot.h"
#include "gui/GuiImage.h"
#include "gui/GuiThermo.h"
#include "gui/Gui2dPlot.h"
#include "gui/GuiNavigator.h"
#include "gui/GuiFolderGroup.h"
#include "gui/GuiFolder.h"
#include "gui/GuiTableLine.h"
#include "gui/GuiDialog.h"
#include "gui/GuiTableItem.h"
#include "gui/GuiSlider.h"
#include "gui/HardCopyTree.h"
#include "operator/MatlabProcess.h"
#include "operator/ProcessGroup.h"
#include "operator/ReportStream.h"
#include "operator/DaemonProcess.h"
#include "operator/TimerFunction.h"
#include "job/JobVarDataReference.h"

#include "xml/XMLXPathJobAction.h"
#include "utils/Debugger.h"
#include "utils/Date.h"
#include "utils/StringUtils.h"

INIT_LOGGER();

extern std::string ErrorItemName;
#define Roundint(x) (int) floor((x) + 0.5)

//**********************************************************************//
// InterpreterConfigurator_Rep                                          //
//**********************************************************************//
class InterpreterConfigurator_Rep {
public:
  InterpreterConfigurator_Rep ()
    : fieldgroup( 0 )
    , fieldgroupline( 0 )
    , labelfield( 0 )
    , voidfield( 0 )
    , indexfield( 0 )
    , datafield( 0 )
    , datapixmap( 0 )
    , form( 0 )
    , container( 0 )
    , newcontainer( 0 )
    , userAttr( 0 )
    , parentName( "" )
    , dictIsGlobal( false )
    , dictWithoutTTrail( false )
    , dictitem( 0 )
    , dictstruct( 0 )
    , dataitem( 0 )
    , dataitemindex( 0 )
    , xfer_web_response( 0 )
    , dataref( 0 )
    , datalevel( 0 )
    , dataset( 0 )
    , invalid( true )
    , colorset( 0 )
    , indexed( false )
    , nsetItems( 0 )
    , indexnum( 0 )
    , helpFile( 0 )
    , stream( 0 )
    , streamXMLFlag( false )
    , streamLatexFlag( false )
    , streamUrlFlag( false )
    , streamJSONFlag( false )
    , streamLocaleFlag( false )
    , streamFileFlag( false )
    , streamDelimiter( '\0' )
    , streamIndent( 0 )
    , streamShowHidden(false )
    , streamShowTransient( true )
    , streamAppendOption( false )
    , streamNoGzOption( false )
    , tempref( 0 )
    , element( 0 )
    , lengthvalue( 0 )
    , prec( 0 )
    , thousandsep( false )
    , streamparameter( 0 )
    , plugin ( 0 )
    , fieldalign ( GuiElement::align_Default )
    , fieldlength ( 0 )
    , fieldprec ( 0 )
    , fieldthousandsep ( false )
    , filestream ( 0 )
    , varStreamParameter( 0 )
    , list ( 0 )
    , helpkey ( 0 )
    , scrolledtext ( 0 )
    , listplot ( 0 )
    , listgraph ( 0 )
    , plotaxis( 0 )
    , simpel( 0 )
    , psplot( 0 )
    , psstream( 0 )
    , plot3d( 0 )
    , image( 0 )
    , thermo(0)
    , plot2d( 0 )
    , plotitem ( 0 )
    , xplotitem ( 0 )
    , navigator ( 0 )
    , root ( 0 )
    , folder ( 0 )
    , folderTabHidden(false)
    , table ( 0 )
    , tableline ( 0 )
    , tablesize ( 0 )
    , tabledataitem ( 0 )
    , tableitem ( 0 )
    , tableitemOptional( false )
    , printfolder ( 0 )
    , printobject ( 0 )
    , filemenulist ( 0 )
    , process ( 0 )
    , processgroup ( 0 )
    , processgroup_dependencies ( true )
    , processgroup_autoclear_dependencies ( false )
    , reportstream ( 0 )
    , timer_maxPendingFunctions( 0 )
    , func_silent( false )
    , func_update_forms( true )
    , func_high_priority( false )
    , function ( 0 )
    , timer ( 0 )
    , task ( 0 )
    , jobdataref ( 0 )
    , jobdataref2 ( 0 )
    , jobdatarefallowwildcards( false )
    , jobaction( 0 )
      //    , dbfunction( 0 )
      //, dbgroup( 0 )
      //, dbgroupname( "" )
      //, sfilter( 0 )
      //, dbtable( 0 )
      //, dboutrowcount( 0 )
      //, dbfilterpartcount( 0 )
      //, sfilterpart( 0 )
      //, bindtable( 0 )
      //, dbinrowcount( 0 )
      //, err( "" )
      //, dbdata( 0 )
      //, dboutitemcount( 0 )
      //, dbdepot( 0 )
    , joblistenercontroller( 0 )
    , jobenabledisableelement( 0 )
    , jobifexpr( 0 )
    , jobwhileexpr( 0 )
    , isDataRefVar( false )
    , navType( GuiNavigator::type_Default )
    , allCycles( false )
    , plot2dMenuDescription( 0 )
      //, dbms( "" )
    , funcHandler( 0 )
    , composeArgumentCount( 0 )
  {
    guifactory = GuiFactory::Instance();
    initContainerStack();
  }

  virtual ~InterpreterConfigurator_Rep(){
  }
  typedef std::vector<Stream*> Streams;
private:
  //  InterpreterConfigurator_Rep( const InterpreterConfigurator_Rep & );
  InterpreterConfigurator_Rep &operator=(const InterpreterConfigurator_Rep &);

public:
  UserAttr * getUserAttr(){
    return static_cast<UserAttr*>( dictitem -> GetAttr() );
  }

  bool registerId( const std::string &id, IdManager::IdentifierType type );

  //**********************************************************************//
  // TargetStreams                                                        //
  //**********************************************************************//
  struct TargetStreams {
    TargetStreams()
      : auto_clear( false )
      , no_dependencies( false )
    {
    }
    ~TargetStreams(){
      clear();
    }

    Streams inp_stream_list;
    Streams out_stream_list;
    Streams stream_list;
    bool    auto_clear;
    bool    no_dependencies;

    void clear(){
      inp_stream_list.clear();
      out_stream_list.clear();
      stream_list.clear();
      auto_clear = false;
      no_dependencies = false;
    }
    void setAutoClear(){
      auto_clear = true;
    }
    void setNoDependencies() {
      no_dependencies = true;
    }
    void addInputStream( Stream * stream ){
      inp_stream_list.push_back( stream );
    }
    void addOutputStream( Stream * stream ){
      out_stream_list.push_back( stream );
    }
    void addStream( Stream * stream ){
      stream_list.push_back( stream );
    }
    void moveStreamsToInput(){
      inp_stream_list.insert( inp_stream_list.end(),
                              stream_list.begin(),
                              stream_list.end() );
      stream_list.clear();
    }
    void moveStreamsToOutput(){
      out_stream_list.insert( out_stream_list.end(),
                              stream_list.begin(),
                              stream_list.end() );
      stream_list.clear();
    }
    void addTargetStreams(bool dep){
      if (no_dependencies || dep ||
          inp_stream_list.size() == 0 ||
          out_stream_list.size() == 0) {
        return;
      }
// if ((inp_stream_list.size() == 0 && out_stream_list.size() > 0) ||
//     (inp_stream_list.size() > 0 && out_stream_list.size() == 0)) {
//   ParserError(_("A TargetStream needs InputStream(s) and OutStream(s)."));
// }
      for( Streams::iterator inp = inp_stream_list.begin();
           inp != inp_stream_list.end();
           ++inp ){
        for( Streams::iterator out = out_stream_list.begin();
             out != out_stream_list.end();
             ++out ){
          BUG_DEBUG("add Targetstream " << (*out)->Name() <<
                    " to Stream " << (*inp)->Name());
          (*inp)->addTargetStream( (*out), auto_clear );
        }
      }
    }
  };
  TargetStreams targetStreams;

  //**********************************************************************//
  // IntensSocketRequestData                                              //
  //**********************************************************************//
  struct IntensSocketRequestData {
    IntensSocketRequestData()
      : func(0)
      , out_stream(0)
      , in_stream(0) {
      s_requests.reserve(11);
    }
    IntensSocketRequestData(const IntensSocketRequestData& r)
      : func(r.func)
      , out_stream(r.out_stream)
      , in_stream(r.in_stream)
      , out_streamVector(r.out_streamVector)
      , in_streamVector(r.in_streamVector)
      , out_pluginVector(r.out_pluginVector)
      , header(r.header) {
    }
    ~IntensSocketRequestData() {
    }
    void clear(bool clear_list=false) {
      func=0, out_stream=0, in_stream=0, header.clear();
      out_streamVector.clear();
      in_streamVector.clear();
      out_pluginVector.clear();
      if (clear_list) s_requests.clear();
    }
    void append() {
      s_requests.push_back(*this);
      clear();
    }
    long size() {
      return s_requests.size();
    }
    bool readBack() {
      if (!s_requests.size())
        return false;
      *this = s_requests.back();
      s_requests.pop_back();
      return true;
    }
    void clearAllRequests() {
      clear(false);
      s_requests.clear();
    }
    JobFunction *func;
    Stream      *out_stream;
    Stream      *in_stream;
    std::vector<Stream*> out_streamVector;
    std::vector<Stream*> in_streamVector;
    std::vector<Plugin*> out_pluginVector;
    std::string  header;
    static std::vector<IntensSocketRequestData> s_requests;
  };
  IntensSocketRequestData socketRequest;

  //**********************************************************************//
  // IntensSocket                                                         //
  //**********************************************************************//
  struct IntensSocket{
    IntensSocket()
      : host("localhost")
      , hostxfer(0)
      , port(-1)
      , portxfer(0)
      , func(0)
      , on_eos(0)
      , out_stream(0)
      , in_stream(0)
      , header("")
      , xfer(0)
      , MFM(false)
      , thumb_xfer(0)
      , thumb_width(0)
      , thumb_height(0){}
    void clear(){
      host="localhost";
      hostxfer=0;
      port=-1;
      portxfer=0;
      func=0;
      on_eos=0;
      out_stream=0;
      in_stream=0;
      header="";
      xfer=0;
      MFM=false;
      thumb_xfer=0;
      thumb_width=0;
      thumb_height=0;
    }
    std::string  host;
    XferDataItem *hostxfer;
    int          port;
    XferDataItem *portxfer;
    JobFunction *func;
    JobFunction *on_eos;
    Stream      *out_stream;
    Stream      *in_stream;
    std::string  header;
    XferDataItem *xfer;
    bool MFM;
    XferDataItem *thumb_xfer;
    int thumb_width;
    int thumb_height;
  };
  IntensSocket socket;

  //**********************************************************************//
  // TransferData                                                         //
  //**********************************************************************//
  struct TransferData{
    TransferData()
      : type(MessageQueue::type_Undefined)
      , host("localhost")
      , hostxfer(0)
      , port(-1)
      , portRequest(-1)
      , portRequestXfer(0)
      , portxfer(0)
      , func(0)
      , header("")
      , request(0)
      , publisher(0)
      , plugin(0)
      , no_dependencies(false)
      , timeout(-1) {
    }
    void clear(){
      type=MessageQueue::type_Undefined;
      host="localhost";
      hostxfer=0;
      port=-1;
      portxfer=0;
      portRequest=-1;
      portRequestXfer=0;
      func=0;
      out_streamVector.clear();
      in_streamVector.clear();
      streamVector.clear();
      pluginVector.clear();
      no_dependencies = false;
      header="";
      timeout=-1;
      request=0;
      publisher=0;
      plugin=0;
    }
    MessageQueue::Type type;
    std::string  host;
    XferDataItem *hostxfer;
    int          port;
    XferDataItem *portxfer;
    int          portRequest;
    XferDataItem *portRequestXfer;
    JobFunction *func;
    std::vector<Stream*> out_streamVector;
    std::vector<Stream*> in_streamVector;
    std::vector<Stream*> streamVector;
    bool    no_dependencies;
    std::vector<Plugin*> pluginVector;
    std::string  header;
    int          timeout;
    MessageQueueRequest* request;
    MessageQueuePublisher* publisher;
    Plugin* plugin;
  };
  TransferData transferData;
  IntensSocketRequestData msgQueueHeader;

  //**********************************************************************//
  // IntensRestService                                                    //
  //**********************************************************************//
  struct IntensRestService{
    IntensRestService()
      : path("")
      , pathXfer(0)
      , pathStream(0)
      , filterStream(0)
      , dataStream(0)
      , responseStream(0)
      , setDbTimestamp(false)
      , baseUrlXfer(0)
      , usernameXfer(0)
      , passwordXfer(0)
      , messageXfer(0)
      , jwtXfer(0) {
    }
    void clear(){
      path="";
      pathXfer=0;
      pathStream=0;
      filterStream=0;
      dataStream=0;
      responseStream=0;
      setDbTimestamp=false;
      baseUrlXfer=0;
      usernameXfer=0;
      passwordXfer=0;
      messageXfer=0;
      jwtXfer=0;
    }
    std::string   path;
    XferDataItem *pathXfer;
    Stream       *pathStream;
    Stream       *filterStream;
    Stream       *dataStream;
    Stream       *responseStream;
    bool          setDbTimestamp;
    XferDataItem *baseUrlXfer;
    XferDataItem *usernameXfer;
    XferDataItem *passwordXfer;
    XferDataItem *messageXfer;
    XferDataItem *jwtXfer;
  };
  IntensRestService restService;

  //**********************************************************************//
  // TimerFunc                                                            //
  //**********************************************************************//
  struct TimerFunc{
    TimerFunc()
      : period(60)
      , periodxfer(0)
      , delay(0)
      , delayxfer(0){
    }
    void clear(){
      period=60;
      periodxfer=0;
      delay=0;
      delayxfer=0;
    }
    double        period;
    XferDataItem *periodxfer;
    double        delay;
    XferDataItem *delayxfer;
  };
  TimerFunc timerFunc;

  //**********************************************************************//
  // FileDialog                                                           //
  //**********************************************************************//
  struct FileDialog{
    FileDialog()
      : dirname("")
      , dirnamexfer(0)
      , filtername("")
      , openMode(true) {
    }
    void clear(){
      filtername="";
      dirname="";
      dirnamexfer=0;
      openMode=true;
    }
    std::string   filtername;
    std::string   dirname;
    XferDataItem *dirnamexfer;
    bool          openMode;
  };
  FileDialog filedialog;

  //**********************************************************************//
  // FileOption                                                           //
  //**********************************************************************//
  struct FileOption{
    FileOption()
      : filename("")
      , hasFilenameXfer(false)
      , hasBaseFilenameXfer(false) {
    }
    void clear(){
      filename="";
      base_filename="";
      hasFilenameXfer=false;
      hasBaseFilenameXfer=false;
    }
    std::string  filename;
    std::string  base_filename;
    bool         hasFilenameXfer;
    bool         hasBaseFilenameXfer;
  };
  FileOption fileoption;

  //**********************************************************************//
  // SerializeOption                                                      //
  //**********************************************************************//
  struct SerializeOption{
    SerializeOption()
      : elementName("")
      , outputFilename("")
      , type(AppData::serialize_JSON) {
    }
    void clear(){
      elementName="";
      outputFilename="";
      type = AppData::serialize_JSON;
    }
    AppData::SerializeType type;
    std::string  elementName;
    std::string  outputFilename;
  };
  SerializeOption serializeoption;

  // Data-Item List ------------------------------------------------------
public:
  std::vector<XferDataItem *>   dataitemList;
  std::vector<XferDataItemIndex *> dataitemIndexList;
  std::vector<JobCodeOpIf *>    jobIFexprList;
  std::vector<JobCodeOpWhile *> jobWHILEexprList;
public:
  Scale *getScale();
  void addScale( char _operator, double value );
  void addScale( char _operator, XferDataItem* item );
  bool isScaleEmpty(){
    return scale.empty();
  }
  void                        pushDataItem();
  void                        pushDataItemIndex();
  XferDataItem               *popDataItem();
  XferDataItemIndex          *popDataItemIndex();
  void                        pushJobIFexpr();
  JobCodeOpIf                *popJobIFexpr();
  void                        pushJobWHILEexpr();
  JobCodeOpWhile             *popJobWHILEexpr();

  /** Der Parser benoetigt für das Erstellen der FORM's einen Stack mit
      GuiContainer, da vertikale und horizontale Container beliebig verschachtelt
      werden können.
  */
  void pushContainerStack( GuiElement * );
  GuiElement *popContainerStack();
private:
  void initContainerStack();
  typedef std::stack<GuiElement *> GuiContainerStack;
  GuiContainerStack     m_container_stack;

public:
  // Datapool ------------------------------------------------------------
  DataSet                 *dataset;
  bool                     invalid;
  ColorSet                *colorset;
  bool                     indexed;
  int                      nsetItems;
  int                      indexnum;
  UserAttr                *userAttr;
  DataDictionary::DataType datatype;
  std::string              parentName;
  bool                     dictIsGlobal;
  bool                     dictWithoutTTrail;
  //
  GuiFieldgroup           *fieldgroup;
  GuiFieldgroupLine       *fieldgroupline;
  GuiLabel                *labelfield;
  GuiVoid                 *voidfield;
  GuiIndex                *indexfield;
  GuiDataField            *datafield;
  GuiPixmap               *datapixmap;
  GuiForm                 *form;
  GuiElement              *container;
  GuiElement              *newcontainer;
  DataDictionary          *dictitem;
  DataDictionary          *dictstruct;
  JobFunction             *function;
  TimerFunction            *timer;
  bool                     func_silent;
  bool                     func_update_forms;
  bool                     func_high_priority;
  XferDataItem            *xfer_web_response;
  bool                     xfer_web_response_proto;
  XferDataItem            *dataitem;
  XferDataItemIndex       *dataitemindex;
  int                      datalevel;
  DataReference           *dataref;
  DataReference           *tempref;
  // HelpFile ------------------------------------------------------------
  HelpFile                *helpFile;
  // Streamer ------------------------------------------------------------
  Stream                  *stream;
  bool                     streamXMLFlag;
  bool                     streamLatexFlag;
  bool                     streamUrlFlag;
  bool                     streamJSONFlag;
  bool                     streamLocaleFlag;
  bool                     streamFileFlag;
  char                     streamDelimiter;
  int                      streamIndent;
  bool                     streamShowHidden;
  bool                     streamShowTransient;
  std::string              streamProcessName;
  bool                     streamAppendOption;
  bool                     streamNoGzOption;
  GuiElement              *element;
  int                      lengthvalue;
  int                      prec;
  bool                     thousandsep;
  StreamParameter         *streamparameter;
  Plugin                  *plugin;
  std::vector<std::string> xmlAttributes;
  std::string              xml_schema;
  std::string              xml_namespace;
  std::string              xml_version;
  std::string              xml_stylesheet;
  bool                     allCycles;
  bool                     isDataRefVar;
  VarStreamParameter      *varStreamParameter;
  std::vector<std::string> stSerializeFormIdList;
  // Ui_Manager ----------------------------------------------------------
  GuiElement::Alignment    fieldalign;
  int                      fieldlength;
  int                      fieldprec;
  bool                     fieldthousandsep;
  FileStream              *filestream;
  GuiList                 *list;
  HelpManager::HelpKey    *helpkey;
  GuiScrolledText         *scrolledtext;
  ListPlot                *listplot;
  ListGraph               *listgraph;
  GuiPlotDataItem         *plotaxis;
  Simpel                  *simpel;
  PSPlot                  *psplot;
  PSPlot::PSStream        *psstream;
  Gui3dPlot               *plot3d;
  GuiImage                *image;
  GuiThermo               *thermo;
  GuiTimeTable            *timeTable;
  Gui2dPlot               *plot2d;
  Gui2dPlot::PlotItem     *plotitem;
  Gui2dPlot::PlotItem     *xplotitem;
  Plot2dMenuDescription   *plot2dMenuDescription;
  GuiNavigator::Type       navType;
  GuiNavigator            *navigator;
  GuiNavigator::Root      *root;
  GuiFolder               *folder;
  std::string              folderTabIcon;
  bool                     folderTabHidden;
  GuiTable                *table;
  GuiTableLine            *tableline;
  GuiTableSize            *tablesize;
  GuiTableItem            *tabledataitem;
  GuiTableItem            *tableitem;
  bool                     tableitemOptional;
  HardCopyFolder          *printfolder;
  FileButtonList          *filemenulist;
  HardCopyListener        *printobject;
  // Operator ------------------------------------------------------------
  Process                 *process;
  ProcessGroup            *processgroup;
  bool                    processgroup_dependencies;
  bool                    processgroup_autoclear_dependencies;
  ReportStream            *reportstream;
  int                     timer_maxPendingFunctions;
  // Job -----------------------------------------------------------------
  JobTask                 *task;
  JobDataReference        *jobdataref;
  JobDataReference        *jobdataref2;
  bool                     jobdatarefallowwildcards;
  JobAction               *jobaction;
  GuiListenerController   *joblistenercontroller;
  GuiElement              *jobenabledisableelement;
  JobCodeOpIf             *jobifexpr;
  JobCodeOpWhile          *jobwhileexpr;
  // DB_Manager ----------------------------------------------------------
  /*
  DbFunction              *dbfunction;
  DBgroup                 *dbgroup;
  std::string              dbgroupname;
  DialogSfilter           *sfilter;
  DBtable                 *dbtable;
  int                      dboutrowcount;
  int                      dbfilterpartcount;
  SfilterPart             *sfilterpart;
  DBbindTable             *bindtable;
  int                      dbinrowcount;
  std::string              err;
  DBDataParameter         *dbdata;
  int                      dboutitemcount;
  DBdepot                 *dbdepot;
  std::string              dbms;
  */
  int                      composeArgumentCount;
  // ButtonText
  std::map<GuiElement::ButtonType, std::string> buttonText;
private:
  std::vector<Scale*> scale;
public:
  GuiFactory  *guifactory;

    FunctionHandler *funcHandler;
};
std::vector<InterpreterConfigurator_Rep::IntensSocketRequestData> InterpreterConfigurator_Rep::IntensSocketRequestData::s_requests;

extern int   PAlineno;
/* --------------------------------------------------------------------------- */
/* initContainerStack --                                                       */
/* --------------------------------------------------------------------------- */

void InterpreterConfigurator_Rep::initContainerStack(){
  while( !m_container_stack.empty() ) m_container_stack.pop();
}

/* --------------------------------------------------------------------------- */
/* pushContainerStack --                                                       */
/* --------------------------------------------------------------------------- */

void InterpreterConfigurator_Rep::pushContainerStack( GuiElement *cont ){
  m_container_stack.push( cont );
}

/* --------------------------------------------------------------------------- */
/* popContainerStack --                                                        */
/* --------------------------------------------------------------------------- */

GuiElement *InterpreterConfigurator_Rep::popContainerStack(){
  assert( !m_container_stack.empty() );
  GuiElement *cont = m_container_stack.top();
  m_container_stack.pop();
  return cont;
}

//---registerId-----------------------------------------------------------
bool InterpreterConfigurator_Rep::registerId( const std::string &id,
                                              IdManager::IdentifierType type ){
  if( !IdManager::Instance().registerId( id, type ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), id) );
    return false;
  }
  return true;
}

Scale *InterpreterConfigurator_Rep::getScale(){
  if( scale.empty() )
    assert( false );
  Scale *s = scale.back();
  scale.pop_back();
  return s;
}
void InterpreterConfigurator_Rep::addScale( char _operator, double value ){
  scale.push_back( new Scale( value, _operator ) );
}
void InterpreterConfigurator_Rep::addScale( char _operator, XferDataItem* item ){
  scale.push_back( new XferScale( item, _operator ) );
}

//---Data Item Stack------------------------------------------------------
void InterpreterConfigurator_Rep::pushDataItem(){ // PUSH DataItem
  dataitemList.push_back(dataitem);
}
void InterpreterConfigurator_Rep::pushDataItemIndex(){ // PUSH DataItem
  dataitemIndexList.push_back(dataitemindex);
}
XferDataItem *InterpreterConfigurator_Rep::popDataItem(){
  if( dataitemList.empty() )
    return 0;
  XferDataItem *temp_returnvalue = dataitem = dataitemList.back();
  dataitemList.pop_back();
  return temp_returnvalue;
}
XferDataItemIndex *InterpreterConfigurator_Rep::popDataItemIndex(){
  if( dataitemIndexList.empty() )
    return 0;
  XferDataItemIndex *temp_returnvalue = dataitemIndexList.back();
  dataitemIndexList.pop_back();
  return temp_returnvalue;
}
//---IF expression (op_code) Stack----------------------------------------
void InterpreterConfigurator_Rep::pushJobIFexpr(){ // PUSH IF expression
  jobIFexprList.push_back(jobifexpr);
}
JobCodeOpIf *InterpreterConfigurator_Rep::popJobIFexpr(){ // POP  IF expression
  JobCodeOpIf *temp_returnvalue = jobIFexprList.back();
  jobIFexprList.pop_back();
  return temp_returnvalue;
}
//---WHILE expression (op_code) Stack-------------------------------------
void InterpreterConfigurator_Rep::pushJobWHILEexpr(){ // PUSH WHILE expression
  jobWHILEexprList.push_back(jobwhileexpr);
}
JobCodeOpWhile *InterpreterConfigurator_Rep::popJobWHILEexpr(){ // POP  WHILE expression
  JobCodeOpWhile *temp_returnvalue = jobWHILEexprList.back();
  jobWHILEexprList.pop_back();
  return temp_returnvalue;
}
//======================================================================//
// - Constructor / Destrtuctor
//======================================================================//
InterpreterConfigurator::InterpreterConfigurator()
  : m_rep( new InterpreterConfigurator_Rep ){
  BUG_DEBUG("Constructor of InterpreterConfigurator");
}

InterpreterConfigurator::~InterpreterConfigurator(){
  BUG_DEBUG("Destructor of InterpreterConfigurator");
  delete m_rep;
}

//======================================================================//
// - dispatchPendingEvents
//======================================================================//
bool InterpreterConfigurator::dispatchPendingEvents(){
  GuiManager::Instance().dispatchPendingEvents();
  return true;
}
//======================================================================//
// - setApplicationTitle
//======================================================================//
bool InterpreterConfigurator::setApplicationTitle( const std::string &title ){
  dispatchPendingEvents();
  UImanager::Instance().setApplicationTitle( title );
  /*********************************************************/
  /* Ich ( who ? ) versuche, den Name der Applikation als Default-   */
  /* Applikations-Msg-File zu setzten, indem ich die exten-*/
  /* sion auf ".msg" setze.                                */
  /*********************************************************/
  std::string tmpfil = title;
  std::string tmpext;
  FileUtilities::ChopFilenameExtension(tmpfil,tmpext);
  return true;
}
//======================================================================//
// - end
//======================================================================//
bool InterpreterConfigurator::end(){
  assert( m_rep->isScaleEmpty() );
//   if( AppData::Instance().MlString() )
//     MultiLanguage().Instance().writeNewApplMsgFile();
  return true;
}

//======================================================================//
// - opCurrentIsoTime
//======================================================================//
bool InterpreterConfigurator::opCurrentIsoTime(){
  JobManager::Instance().opCurrentIsoTime( m_rep->function );
  return true;
}
//======================================================================//
// - opCurrentIsoDate
//======================================================================//
bool InterpreterConfigurator::opCurrentIsoDate(){
  JobManager::Instance().opCurrentIsoDate( m_rep->function );
  return true;
}
//======================================================================//C
// - opCurrentIsoDateTime
//======================================================================//
bool InterpreterConfigurator::opCurrentIsoDatetime(){
  JobManager::Instance().opCurrentIsoDatetime( m_rep->function );
  return true;
}

//**********************************************************************//
// DATAPOOL
//**********************************************************************//
//======================================================================//
// - newDataSet
//======================================================================//
bool InterpreterConfigurator::newDataSet( const std::string &id ){
  if( !m_rep -> registerId( id, IdManager::id_DataSet ) )
    return false;
  if ( ( m_rep -> dataset = DataPoolIntens::Instance().newDataSet( id,
                                                                   m_rep->dictIsGlobal,
                                                                   false ) ) == 0 ){
    ParserError( compose(_("Item '%1' is already declared."),id) );
    return false;
  }
  if( !m_rep->invalid ){
    m_rep -> dataset -> resetInvalidEntry();
    m_rep -> invalid = true;
  }
  m_rep -> indexed = false;
  m_rep -> nsetItems = 0;
  if (AppData::Instance().LspWorker()) {
    auto filename = App::Instance().getFlexer()->getCurrentFilename();
    if (filename.ends_with('"')) {
      filename.pop_back();
    }
    m_rep->dataset->setLSPFilename(filename);
    int lineNo = PAlineno -(*(App::Instance().getFlexer()->YYText() ) == '\n' || ! *(App::Instance().getFlexer()->YYText()));
    m_rep->dataset->setLSPLineno(lineNo);
  }
  return true;
}
//======================================================================//
// - setDataSetItemValues
//======================================================================//
bool InterpreterConfigurator::setDataSetItemValues( const std::string &id ){
  if ( m_rep -> nsetItems == 0 )
    m_rep -> indexed = true;
  else {
    if ( !m_rep -> indexed ){
      ParserError( _("This dataset item is automatically enumerated.") );
      return false;
    }
  }
  if ( !m_rep -> dataset -> setItemValues( id, (double)m_rep -> nsetItems ) ){
    ParserError( compose(_("Item '%1' is already declared."), id) );
    return false;
  }
  m_rep -> nsetItems++;
  return true;
}
//======================================================================//
// - setDataSetItemValues
//======================================================================//
bool InterpreterConfigurator::setDataSetItemValues( const std::string &id,  const double value ){
  if ( ( m_rep -> nsetItems > 0 ) && ( m_rep -> indexed ) ){
    ParserError( compose(_("Value of dataset item '%1' missing."),id) );
    return false;
  }
  if ( !m_rep -> dataset -> setItemValues( id, value ) ){
    ParserError( compose(_("Item '%1' is already declared."), ErrorItemName) );
    return false;
  }
  m_rep -> nsetItems++;
  return true;
}
//======================================================================//
// - setDataSetItemValues
//======================================================================//
bool InterpreterConfigurator::setDataSetItemValues( const std::string &id, const std::string &value ){
  if ( ( m_rep -> nsetItems > 0 ) && ( m_rep -> indexed ) ){
    ParserError( compose(_("Value of dataset item '%1' missing."),id) );
    return false;
  }
  if ( !m_rep -> dataset -> setItemValues( id, value ) ){
    ParserError( compose(_("Item '%1' is already declared."), ErrorItemName) );
    return false;
  }
  m_rep -> nsetItems++;
  return true;
}
//======================================================================//
// - newColorSet
//======================================================================//
bool InterpreterConfigurator::newColorSet( const std::string &id ){
  if( !m_rep -> registerId( id, IdManager::id_ColorSet ) )
    return false;
  if ( ( m_rep -> colorset = DataPoolIntens::Instance().newColorSet( id ) ) == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), id) );
    return false;
  }
  if (AppData::Instance().LspWorker()) {
    auto filename = App::Instance().getFlexer()->getCurrentFilename();
    if (filename.ends_with('"')) {
      filename.pop_back();
    }
    m_rep->colorset->setLSPFilename(filename);
    int lineNo = PAlineno -(*(App::Instance().getFlexer()->YYText() ) == '\n' || ! *(App::Instance().getFlexer()->YYText()));
    m_rep->colorset->setLSPLineno(lineNo);
  }
  return true;
}
//======================================================================//
// - setColorSetItemInvalid
//======================================================================//
bool InterpreterConfigurator::setColorSetItemInvalid(){
  m_rep -> colorset -> setInvalid();
  return true;
}
//======================================================================//
// - setColorSetItemAlways
//======================================================================//
bool InterpreterConfigurator::setColorSetItemAlways(){
  m_rep -> colorset -> setAlways();
  return true;
}
//======================================================================//
// - setColorSetItemLowerValue
//======================================================================//
bool InterpreterConfigurator::setColorSetItemLowerValue( int what, double val ){
  m_rep -> colorset -> setLowerbound( what, val );
  return true;
}
//======================================================================//
// - setColorSetItemUpperValue
//======================================================================//
bool InterpreterConfigurator::setColorSetItemUpperValue( int what, double val ){
  return m_rep -> colorset -> setUpperbound( what, val );
}

//======================================================================//
// - setColorSetItemLowerValue
//======================================================================//
bool InterpreterConfigurator::setColorSetItemLowerXfer( int what ){
  m_rep -> colorset -> setLowerbound( what, m_rep->popDataItem(), m_rep->getScale() );
  return true;
}
//======================================================================//
// - setColorSetItemUpperXfer
//======================================================================//
bool InterpreterConfigurator::setColorSetItemUpperXfer( int what ){
  return m_rep -> colorset -> setUpperbound( what, m_rep->popDataItem(), m_rep->getScale() );
}
//======================================================================//
// - setColorSetItemLowerValue
//======================================================================//
bool InterpreterConfigurator::setColorSetItemLowerValue( int what, std::string &val ){
  m_rep -> colorset -> setLowerbound( what, val );
  return true;
}
//======================================================================//
// - setColorSetItemUpperValue
//======================================================================//
bool InterpreterConfigurator::setColorSetItemUpperValue( int what, std::string &val ){
  return m_rep -> colorset -> setUpperbound( what, val );
}

//======================================================================//
// - setColorSetItemColors
//======================================================================//
bool InterpreterConfigurator::setColorSetItemColors( const std::string &bg,
                                                     const std::string &fg ){
  m_rep -> colorset -> setColors( bg, fg );
  return true;
}

//======================================================================//
// - setColorSetItemColors
//======================================================================//
bool InterpreterConfigurator::setColorSetItemColorsXfer(XferDataItem *bgXfer, XferDataItem *fgXfer){
  m_rep -> colorset -> setColors( bgXfer, fgXfer );
  return true;
}

//======================================================================//
// - setDictInitialise
//======================================================================//
bool InterpreterConfigurator::setDictInitialise(){
  m_rep -> dictIsGlobal      = false;
  m_rep -> dictWithoutTTrail = false;
  return true;
}

//======================================================================//
// - setDictIsGlobal
//======================================================================//
bool InterpreterConfigurator::setDictIsGlobal( bool status ){
  if ( status && !m_rep -> parentName.empty() ) { // GLOBAL is not allowed inside a struct definition
    ParserError( _("GLOBAL is not allowed inside a struct definition.") );
    return false;
  }
  m_rep -> dictIsGlobal = status;
  return true;
}

//======================================================================//
// - setDictWithoutTTrail
//======================================================================//
bool InterpreterConfigurator::setDictWithoutTTrail(){
  m_rep -> dictWithoutTTrail = true;
  return true;
}
//======================================================================//
// - setUserAttrNoDependencies
//======================================================================//
bool InterpreterConfigurator::setUserAttrNoDependencies(){
  m_rep -> userAttr ->SetNoDependencies();
  return true;
}
//======================================================================//
// - setUserAttrLockable
//======================================================================//
bool InterpreterConfigurator::setUserAttrLockable(){
  m_rep -> userAttr -> SetLockable();
  return true;
}
//======================================================================//
// - setUserAttrEditable
//======================================================================//
bool InterpreterConfigurator::setUserAttrEditable(){
  m_rep -> userAttr -> SetEditable();
  return true;
}
//======================================================================//
// - setUserAttrOptional
//======================================================================//
bool InterpreterConfigurator::setUserAttrOptional(){
  m_rep -> userAttr -> SetOptional();
  return true;
}
//======================================================================//
// - setUserAttrClassname
//======================================================================//
bool InterpreterConfigurator::setUserAttrClassname( const std::string &name ){
  m_rep -> userAttr -> SetClassname( name );
  return true;
}
//======================================================================//
// - setUserAttrMaxOccurs
//======================================================================//
bool InterpreterConfigurator::setUserAttrMaxOccurs( const int num ){
  m_rep -> userAttr -> SetMaxOccurs( num );
  return true;
}
//======================================================================//
// - setUserAttrScalar
//======================================================================//
bool InterpreterConfigurator::setUserAttrScalar(){
  m_rep -> userAttr -> SetScalar();
  return true;
}
//======================================================================//
// - setUserAttrMatrix
//======================================================================//
bool InterpreterConfigurator::setUserAttrMatrix(){
  m_rep -> userAttr -> SetMatrix();
  return true;
}
//======================================================================//
// - setUserAttrPersistent
//======================================================================//
bool InterpreterConfigurator::setUserAttrPersistent(){
  m_rep -> userAttr -> SetPersistent();
  return true;
}
//======================================================================//
// - setUserAttrDbTransient
//======================================================================//
bool InterpreterConfigurator::setUserAttrDbTransient( ){
  m_rep -> userAttr -> SetDbTransient( );
  return true;
}
//======================================================================//
// - setUserAttrCell
//======================================================================//
bool InterpreterConfigurator::setUserAttrCell(){
  m_rep -> userAttr -> SetCell();
  return true;
}

//======================================================================//
// - setDataType
//======================================================================//
bool InterpreterConfigurator::setDataType( DataDictionary::DataType type ){
  InterpreterConfigurator::m_rep -> datatype = type;
  return true;
}

//======================================================================//
// - getDataType
//======================================================================//
DataDictionary::DataType InterpreterConfigurator::getDataType(){
  return m_rep -> datatype;
}

//======================================================================//
// - getFieldgroup
//======================================================================//
GuiFieldgroup * InterpreterConfigurator::getFieldgroup(){
  return m_rep -> fieldgroup;
}

//======================================================================//
// - deleteUserAttr
//======================================================================//
bool InterpreterConfigurator::deleteUserAttr(){
  delete m_rep -> userAttr;
  m_rep -> userAttr = 0;
  return true;
}

//======================================================================//
// - setUserAttr
//======================================================================//
bool InterpreterConfigurator::setUserAttr( UserAttr *attr ){
  m_rep -> userAttr = attr;
  return true;
}

//======================================================================//
// - getUserAttr
//======================================================================//
UserAttr *InterpreterConfigurator::getUserAttr(){
  return m_rep -> userAttr;
}

//======================================================================//
// - newUserAttr
//======================================================================//
bool InterpreterConfigurator::newUserAttr(){
  m_rep -> userAttr = new UserAttr();
  return true;
}

//======================================================================//
// - addDataVariable
//======================================================================//
bool InterpreterConfigurator::addDataVariable( const std::string &id ){
  dispatchPendingEvents();
  if( m_rep -> parentName.empty() && DataPoolIntens::Instance().noNamespace() )
    if( !m_rep -> registerId( id, IdManager::id_DataVariable ) )
      return false;
  /* get new dictionary item */
  m_rep -> dictitem = DataPoolIntens::addDataVariable( m_rep->parentName,
                                                       id,
                                                       m_rep->datatype,
                                                       m_rep->dictIsGlobal,
                                                       false );
  if( m_rep -> dictitem == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), id) );
    return false;
  }
  if( m_rep->dictWithoutTTrail ){
    m_rep->dictitem->setItemWithoutTTrail();
  }

  if( m_rep -> userAttr != 0 )
    *static_cast<UserAttr*>( m_rep -> dictitem -> GetAttr() )= *m_rep -> userAttr;

  // add extra data for when we run in lsp mode
  if (AppData::Instance().LspWorker()) {
    auto filename = App::Instance().getFlexer()->getCurrentFilename();
    if (filename.ends_with('"')) {
      filename.pop_back();
    }
    m_rep->dictitem->setLSPFilename(filename);
    int lineNo = PAlineno -(*(App::Instance().getFlexer()->YYText() ) == '\n' || ! *(App::Instance().getFlexer()->YYText()));
    m_rep->dictitem->setLSPLineno(lineNo);
  }
  return true;
}
//======================================================================//
// - setIndexNum
//======================================================================//
bool InterpreterConfigurator::setIndexNum( const int num ){
  m_rep -> indexnum = num;
  return true;
}
//======================================================================//
// - unsetDictItemFolder
//======================================================================//
bool InterpreterConfigurator::unsetDictItemFolder(){
  m_rep -> dictitem -> unsetFolder();
  return true;
}
//======================================================================//
// - setDictItemRange
//======================================================================//
bool InterpreterConfigurator::setDictItemRange( double min, double max ){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetRange( min, max );
  return true;
}

//======================================================================//
// - setDictItemStep
//======================================================================//
bool InterpreterConfigurator::setDictItemStep( double step ){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetStep( step );
  return true;
}

//======================================================================//
// - setDictItemDefaultDimension
//======================================================================//
bool InterpreterConfigurator::setDictItemDefaultDimension( const int dim ){
  m_rep -> indexnum++;
  m_rep -> dictitem -> SetDefaultDimension( m_rep -> indexnum, dim );
  return true;
}
//======================================================================//
// - setDataSetName
//======================================================================//
bool InterpreterConfigurator::setDataSetName( const std::string &name, bool bIndexed ){
  if ( ( m_rep -> dataset = DataPoolIntens::Instance().getDataSet( name ) ) == 0 ){
    ParserError( compose( _("Undeclared SET '%1'."), name ) );
    return false;
  }
  if ( m_rep -> dataset -> DataType() == DataDictionary::type_String )
    if ( m_rep -> datatype != DataDictionary::type_String ){
      ParserError( _("The dataset has items of type string.") );
      return false;
    }
  static_cast<UserAttr*>( m_rep -> dictitem -> GetAttr() ) -> SetDataSetName( name, bIndexed );
  static_cast<UserAttr*>( m_rep -> dictitem -> GetAttr() ) -> SetCombobox();
  return true;
}
//======================================================================//
// - setDataSetNoInvalidEntry
//======================================================================//
bool InterpreterConfigurator::setDataSetNoInvalidEntry(){
  m_rep -> invalid = false;
  return true;
}
//======================================================================//
// - setColorSetName
//======================================================================//
bool InterpreterConfigurator::setColorSetName( const std::string &name ){
  if (name.size()) {
	static_cast<UserAttr*>( m_rep -> dictitem -> GetAttr() ) -> setColorSetName( name );
  } else {
	if ( m_rep -> datatype != DataDictionary::type_String ){
	  ParserError( _("The color picker item must be of type string.") );
	  return false;
	}
	static_cast<UserAttr*>( m_rep -> dictitem -> GetAttr() ) -> setColorPicker();
	static_cast<UserAttr*>( m_rep -> dictitem -> GetAttr() ) -> SetButton();
  }
  return true;
}
//======================================================================//
// - setNoColorBit
//======================================================================//
bool InterpreterConfigurator::setNoColorBit(){
  static_cast<UserAttr*>( m_rep -> dictitem -> GetAttr() ) -> setNoColorBit();
  return true;
}
//======================================================================//
// - setDictItemFunction
//======================================================================//
bool InterpreterConfigurator::setDictItemFunction(){
  static_cast<UserAttr*>( m_rep -> dictitem -> GetAttr() ) -> setFunction( m_rep->function );
  return true;
}
//======================================================================//
// - setDictItemFocusFunction
//======================================================================//
bool InterpreterConfigurator::setDictItemFocusFunction(){
  static_cast<UserAttr*>( m_rep -> dictitem -> GetAttr() ) -> setFocusFunction( m_rep->function );
  return true;
}
//======================================================================//
// - setDictItemLabel
//======================================================================//
bool InterpreterConfigurator::setDictItemLabel( const std::string &label ){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetLabel( label );
  return true;
}
//======================================================================//
// - setDictItemUnits
//======================================================================//
bool InterpreterConfigurator::setDictItemUnits( const std::string &units ){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetUnit( units );
  return true;
}
//======================================================================//
// - setDictItemPattern
//======================================================================//
bool InterpreterConfigurator::setDictItemPattern( const std::string &pattern ){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetPattern( compose("(%1)?",pattern) );
  return true;
}
//======================================================================//
// - setDictItemDate
//======================================================================//
bool InterpreterConfigurator::setDictItemDate(UserAttr::STRINGtype type) {
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetStringType(type);
  return true;
}
//======================================================================//
// - setDictItemHelpText
//======================================================================//
bool InterpreterConfigurator::setDictItemHelpText( const std::string &helpText ){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetHelptext( helpText );
  return true;
}
//======================================================================//
// - setDictItemButton
//======================================================================//
bool InterpreterConfigurator::setDictItemButton(){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetButton();
  return true;
}
//======================================================================//
// - setDictItemSlider
//======================================================================//
bool InterpreterConfigurator::setDictItemSlider(){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetSlider();
  return true;
}
//======================================================================//
// - setDictItemProgress
//======================================================================//
bool InterpreterConfigurator::setDictItemProgress(){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetProgress();
  return true;
}
//======================================================================//
// - setDictItemToggle
//======================================================================//
bool InterpreterConfigurator::setDictItemToggle(){
  if ( m_rep -> datatype == DataDictionary::type_String ){
    ParserError( _("Type of toggle item is string. (Should be REAL or INTEGER)") );
    return false;
  }
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetToggle();
  return true;
}
//======================================================================//
// - setDictItemRadio
//======================================================================//
bool InterpreterConfigurator::setDictItemRadio(){
  if ( m_rep -> datatype == DataDictionary::type_String ){
    ParserError( _("Type of radio item is string. (Should be REAL or INTEGER)") );
    return false;
  }
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetRadioButton();
  return true;
}
//======================================================================//
// - setDictItemCombobox
//======================================================================//
bool InterpreterConfigurator::setDictItemCombobox(){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetCombobox();
  return true;
}
//======================================================================//
// - setDictItemLabelType
//======================================================================//
bool InterpreterConfigurator::setDictItemLabelType(){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetLabelType();
  return true;
}
//======================================================================//
// - setDictItemClassName
//======================================================================//
bool InterpreterConfigurator::setDictItemClassName( const std::string &name ){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetClassname( name );
  return true;
}
//======================================================================//
// - setUserAttrPlaceholder
//======================================================================//
bool InterpreterConfigurator::setDictItemPlaceholder( const std::string &text ){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetPlaceholder( text );
  return true;
}
//======================================================================//
// - setDictItemScalar
//======================================================================//
bool InterpreterConfigurator::setDictItemScalar(){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetScalar();
  return true;
}
//======================================================================//
// - setDictItemMatrix
//======================================================================//
bool InterpreterConfigurator::setDictItemMatrix(){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetMatrix();
  return true;
}
//======================================================================//
// - setDictItemDbTransient
//======================================================================//
bool InterpreterConfigurator::setDictItemDbTransient( ){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetDbTransient( );
  return true;
}
//======================================================================//
// - setDictItemDbAttr
//======================================================================//
bool InterpreterConfigurator::setDictItemDbAttr( const std::string &name ){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetDbAttr( name );
  return true;
}
//======================================================================//
// - setDictItemDbUnit
//======================================================================//
bool InterpreterConfigurator::setDictItemDbUnit( const std::string &name ){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetDbUnit( name );
  return true;
}
//======================================================================//
// - setDictItemCell
//======================================================================//
bool InterpreterConfigurator::setDictItemCell(){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetCell();
  return true;
}
//======================================================================//
// - setDictItemHidden
//======================================================================//
bool InterpreterConfigurator::setDictItemHidden(){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetHidden();
  return true;
}
//======================================================================//
// - setDictItemPersistent
//======================================================================//
bool InterpreterConfigurator::setDictItemPersistent(){
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetPersistent();
  return true;
}
//======================================================================//
// - setDictItemTag
//======================================================================//
bool InterpreterConfigurator::setDictItemTag( const std::string &id, bool regist ){
  if( regist )
    if( !m_rep -> registerId( id, IdManager::id_Tag ) )
      return false;
  static_cast<UserAttr*>( m_rep -> dictitem -> GetAttr() ) -> setTag( id );
  return true;
}

//======================================================================//
// - setDictItemWheelEvent
//======================================================================//
bool InterpreterConfigurator::setDictItemWheelEvent() {
  static_cast<UserAttr*>(m_rep -> dictitem -> GetAttr() )->SetWheelEvent();
  return true;
}
//======================================================================//
// - setDictStruct
//======================================================================//
bool InterpreterConfigurator::setDictStruct( const std::string &id, bool check ){
  m_rep -> dictstruct = DataPoolIntens::getDataPool().FindDictEntry( id.c_str() );
  if( check ){
    if( m_rep -> dictstruct == 0 ){
      ParserError( compose(_("Undeclared STRUCT '%1'."), id) );
      return true;
    }
    if( m_rep -> dictstruct -> getDataType() != DataDictionary::type_StructDefinition ){
      ParserError( compose(_("Type of '%1' is not structure definition."), id) );
      return false;
    }
  }
  return true;
}

//======================================================================//
// - addStructureVariable
//======================================================================//
bool InterpreterConfigurator::addStructureVariable( const std::string &id ){
  if( m_rep -> parentName.empty() && DataPoolIntens::Instance().noNamespace() )
    if( !m_rep -> registerId( id, IdManager::id_DataVariable ) )
      return false;
  /* get new dictionary item */
  m_rep -> dictitem = DataPoolIntens::addStructureVariable( m_rep -> parentName,
                                                            id,
                                                            m_rep -> dictstruct->getName(),
                                                            m_rep -> dictIsGlobal,
                                                            false );
  if( m_rep -> dictitem == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), id) );
    return false;
  }
  if( m_rep->dictWithoutTTrail ){
    m_rep->dictitem->setItemWithoutTTrail();
  }

  if( m_rep -> userAttr != 0 )
    *static_cast<UserAttr*>( m_rep -> dictitem -> GetAttr() ) = *m_rep -> userAttr;

  if (AppData::Instance().LspWorker()) {
    auto filename = App::Instance().getFlexer()->getCurrentFilename();
    if (filename.ends_with('"')) {
      filename.pop_back();
    }
    m_rep->dictitem->setLSPFilename(filename);
    int lineNo = PAlineno -(*(App::Instance().getFlexer()->YYText() ) == '\n' || ! *(App::Instance().getFlexer()->YYText()));
    m_rep->dictitem->setLSPLineno(lineNo);
  }
  return true;
}
//======================================================================//
// - resetDictItem
//======================================================================//
bool InterpreterConfigurator::resetDictItem(){
  m_rep -> dictitem = 0;
  return true;
}

//======================================================================//
// - addStructureDefinition
//======================================================================//
bool InterpreterConfigurator::addStructureDefinition( const std::string &id ){
  if( !m_rep -> registerId( id, IdManager::id_DataStructure ) )
    return false;
  /* get new dictionary item */
  assert( m_rep -> parentName.empty() );
  m_rep -> dictitem = DataPoolIntens::addStructureDefinition( id );
  if( m_rep -> dictitem == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), id) );
    return false;
  }
  m_rep -> parentName = id;

  if (AppData::Instance().LspWorker()) {
    auto filename = App::Instance().getFlexer()->getCurrentFilename();
    if (filename.ends_with('"')) {
      filename.pop_back();
    }
    m_rep->dictitem->setLSPFilename(filename);
    int lineNo = PAlineno -(*(App::Instance().getFlexer()->YYText() ) == '\n' || ! *(App::Instance().getFlexer()->YYText()));
    m_rep->dictitem->setLSPLineno(lineNo);
  }

  return true;
}
//======================================================================//
// - copyStructureDefinition
//======================================================================//
bool InterpreterConfigurator::copyStructureDefinition( const std::string &id ){
  m_rep->dictstruct = DataPoolIntens::getDataPool().FindDictEntry( id.c_str() );
  if( m_rep->dictstruct == 0 ){
    ParserError( compose(_("Undeclared STRUCT '%1'."), id) );
    return false;
  }
  if( m_rep->dictstruct->getDataType() != DataDictionary::type_StructDefinition ){
    ParserError( compose(_("Type of '%1' is not structure definition."), id) );
    return false;
  }
  if( !DataPoolIntens::copyStructureDefinition( m_rep->dictitem, m_rep->dictstruct ) ){
    ParserError( compose(_("%1: copy of structure definition failed."), id) );
    return false;
  }
  return true;
}

//======================================================================//
// - eraseParentName
//======================================================================//
bool InterpreterConfigurator::eraseParentName(){
  m_rep -> parentName.erase();
  return true;
}

//**********************************************************************//
// END DATAPOOL
//**********************************************************************//


//**********************************************************************//
// HELPFILE
//**********************************************************************//
//======================================================================//
// - installHelpText
//======================================================================//
bool InterpreterConfigurator::installHelpText( const std::string &filename ){
  m_rep -> helpFile = HelpManager::Instance().installHelpText( filename );
  return installHelpCallback();
}
//======================================================================//
// - installHelpHTML
//======================================================================//
bool InterpreterConfigurator::installHelpHTML( const std::string &filename ){
  m_rep -> helpFile = HelpManager::Instance().installHelpHTML( filename );
  return true;
}
//======================================================================//
// - installHelpURL
//======================================================================//
bool InterpreterConfigurator::installHelpURL( const std::string &filename ){
  m_rep -> helpFile = HelpManager::Instance().installHelpURL( filename );
  return true;
}
//======================================================================//
// - installHelpCallback
//======================================================================//
bool InterpreterConfigurator::installHelpCallback(){
  if( m_rep -> helpFile == 0 ){
    return false;
  }
  else {
    m_rep -> helpFile -> installHelpCallback();
    return true;
  }
}
//======================================================================//
// - hideHelpMenu
//======================================================================//
bool InterpreterConfigurator::hideHelpMenu(){
  if( m_rep -> helpFile == 0 )
    return false;
  else {
    m_rep -> helpFile -> hideHelpMenu();
    return true;
  }
}
//======================================================================//
// - installHelpKey
//======================================================================//
bool InterpreterConfigurator::installHelpKey( const std::string &anchor ){
  if( m_rep -> helpFile == 0 )
    return false;
  else {
    m_rep -> helpFile -> installHelpKey( anchor );
    return true;
  }
}

//**********************************************************************//
// END HELPFILE
//**********************************************************************//

//**********************************************************************//
// STREAMER
//**********************************************************************//
//======================================================================//
// - newStream
//======================================================================//
Stream * InterpreterConfigurator::newStream( const std::string &id ){
  BUG_DEBUG("newStream(" << id << ")");
  dispatchPendingEvents();
  if( !IdManager::Instance().registerId( id, IdManager::id_Stream ) )
    ParserError( compose(_("Item '%1' is already declared."), id) );
  if( (m_rep -> stream = StreamManager::Instance().newStream( id )) == NULL )
    ParserError( compose(_("Item '%1' is already declared."), ErrorItemName) );
  stApplyOptions();
  if (AppData::Instance().LspWorker()) {
    auto filename = App::Instance().getFlexer()->getCurrentFilename();
    if (filename.ends_with('"')) {
      filename.pop_back();
    }
    m_rep->stream->setLSPFilename(filename);
    int lineNo = PAlineno -(*(App::Instance().getFlexer()->YYText() ) == '\n' || ! *(App::Instance().getFlexer()->YYText()));
    m_rep->stream->setLSPLineno(lineNo);
  }
  return m_rep -> stream;
}
//======================================================================//
// - stApplyOptions
//======================================================================//
void InterpreterConfigurator::stApplyOptions() {
  BUG_DEBUG("stApplyOptions");
  // check incompatible options
  if ( m_rep->streamLatexFlag && m_rep->streamUrlFlag ) {
    ParserError( _("LaTeX filter and URL filter cannot be combined.") );
  }
  if ( m_rep->streamUrlFlag && m_rep->streamDelimiter ) {
    ParserError( _("DELIMITER is not used in URL stream.") );
  }
  if ( m_rep->streamJSONFlag && m_rep->streamDelimiter ) {
    ParserError( _("DELIMITER is not used in JSON stream.") );
  }
  if ( m_rep->streamJSONFlag && m_rep->streamLatexFlag ) {
    ParserError( _("LaTeX filter is not used in JSON stream.") );
  }
  if ( m_rep->streamJSONFlag && m_rep->streamUrlFlag ) {
    ParserError( _("URL filter is not used in JSON stream.") );
  }

  // apply options
  if (m_rep->streamLatexFlag) {
    m_rep->stream->setLatexFlag();
    m_rep->streamLatexFlag = false;
  }
  if (m_rep->streamXMLFlag) {
    m_rep->stream->setXMLFlag();
    m_rep->streamXMLFlag = false;
  }
  if (m_rep->streamUrlFlag) {
    m_rep->stream->setUrlFlag();
    m_rep->streamUrlFlag = false;
  }
  if (m_rep->streamJSONFlag) {
    m_rep->stream->setJSONFlag();
    m_rep->streamJSONFlag = false;
    // JSON options
    if (m_rep->streamIndent > 0) {
      m_rep->stream->setIndentation( m_rep->streamIndent );
      m_rep->streamIndent = 0;
    }
    m_rep->stream->setHidden( m_rep->streamShowHidden );
    m_rep->streamShowHidden = false;
    m_rep->stream->setTransient( m_rep->streamShowTransient );
    m_rep->streamShowTransient = true;
  }
  if (m_rep->streamLocaleFlag) {
    m_rep->stream->setLocaleFlag();
    m_rep->streamLocaleFlag = false;
  }
  if (m_rep->streamFileFlag) {
    m_rep->stream->setFileFlag();
    m_rep->streamFileFlag = false;
  }
  if (m_rep->streamDelimiter != '\0') {
    m_rep->stream->setDelimiter( m_rep->streamDelimiter );
    m_rep->streamDelimiter = '\0';
  }
  if (m_rep->streamProcessName.size()) {
    m_rep->stream->setProcess( m_rep->streamProcessName );
    m_rep->streamProcessName.clear();
  }
  if (m_rep->streamAppendOption) {
    m_rep->stream->setAppendOption( m_rep->streamAppendOption );
    m_rep->streamAppendOption = false;
  }
  if (m_rep->streamNoGzOption) {
    m_rep->stream->setNoGzOption( m_rep->streamNoGzOption );
    m_rep->streamNoGzOption = false;
  }
}
//======================================================================//
// - setStreamXMLFlag
//======================================================================//
bool InterpreterConfigurator::setStreamXMLFlag(){
  m_rep -> streamXMLFlag = true;
  return true;
}
//======================================================================//
// - setStreamLatexFlag
//======================================================================//
bool InterpreterConfigurator::setStreamLatexFlag(){
  m_rep -> streamLatexFlag = true;
  return true;
}
//======================================================================//
// - setStreamUrlFlag
//======================================================================//
bool InterpreterConfigurator::setStreamUrlFlag(){
  m_rep -> streamUrlFlag = true;
  return true;
}
//======================================================================//
// - setStreamJSONFlag
//======================================================================//
bool InterpreterConfigurator::setStreamJSONFlag(){
  BUG_DEBUG("setStreamJSONFlag");
  m_rep->streamJSONFlag = true;
  return true;
}
//======================================================================//
// - setStreamLocaleFlag
//======================================================================//
bool InterpreterConfigurator::setStreamLocaleFlag(){
  m_rep->streamLocaleFlag = true;
  return true;
}
//======================================================================//
// - setStreamFileFlag
//======================================================================//
bool InterpreterConfigurator::setStreamFileFlag(){
  m_rep->streamFileFlag = true;
  return true;
}
//======================================================================//
// - setStreamDelimiter
//======================================================================//
bool InterpreterConfigurator::setStreamDelimiter(char delim){
  m_rep -> streamDelimiter = delim;
  return true;
}
//======================================================================//
// - setStreamIndent
//======================================================================//
bool InterpreterConfigurator::setStreamIndent(int i){
  m_rep -> streamIndent = i;
  return true;
}
//======================================================================//
// - setStreamerProcess
//======================================================================//
bool InterpreterConfigurator::setStreamerProcess( const std::string &processname ){
  m_rep -> streamProcessName = processname;
  return true;
}
//======================================================================//
// - setStreamAppendOption
//======================================================================//
bool InterpreterConfigurator::setStreamAppendOption() {
  m_rep -> streamAppendOption = true;
  return true;
}
//======================================================================//
// - setStreamNoGzOption
//======================================================================//
bool InterpreterConfigurator::setStreamNoGzOption() {
  m_rep -> streamNoGzOption = true;
  return true;
}
//======================================================================//
// - setStreamHidden
//======================================================================//
bool InterpreterConfigurator::setStreamHidden(bool yes){
  BUG_DEBUG("setStreamHidden(" << yes << ")");
  m_rep -> streamShowHidden = yes;
  return true;
}
//======================================================================//
// - setStreamTransient
//======================================================================//
bool InterpreterConfigurator::setStreamTransient(bool yes){
  BUG_DEBUG("setStreamTransient(" << yes << ")");
  m_rep -> streamShowTransient = yes;
  return true;
}
//======================================================================//
// - resetStream
//======================================================================//
bool InterpreterConfigurator::resetStream(){
  m_rep -> stream = 0;
  return true;
}
//======================================================================//
// - stBeginGroup
//======================================================================//
bool InterpreterConfigurator::stBeginGroup(){
  m_rep -> stream  -> beginGroup( );
  return true;
}
//======================================================================//
// - stEndGroup
//======================================================================//
bool InterpreterConfigurator::stEndGroup(){
  m_rep -> stream  -> endGroup( );
  return true;
}
//======================================================================//
// - stNewXferDataItem
//======================================================================//
bool InterpreterConfigurator::stNewXferDataItem( const std::string &id ){
  m_rep -> dataref = DataPoolIntens::getDataReference( id );
  if( m_rep -> dataref == 0 )
    ParserError( compose(_("Undeclared identifier '%1'."), id) );
  m_rep -> dataitem = new XferDataItem();
  m_rep -> datalevel = 0;
  m_rep -> indexnum=0;
  GuiIndex::clearIndexContainer();
  return true;
}
//======================================================================//
// - resetDataItem
//======================================================================//
bool InterpreterConfigurator::resetDataItem(){
  m_rep ->dataitem = 0;
  return true;
}
//======================================================================//
// - stSetVarDataReference
//======================================================================//
bool InterpreterConfigurator::stSetVarDataReference(){
  if( m_rep->isDataRefVar )
    ParserError( "" );
  stSetDataReference();
  // Hier fängt der Variable Teil der Datenreferenz an
  m_rep -> indexnum= 0;
  m_rep -> datalevel++;
  m_stack.push( new InterpreterConfigurator_Rep( *m_rep ) );
  m_stack.top()->isDataRefVar = true;
  return true;
}
//======================================================================//
// - stSetDataReference
//======================================================================//
bool InterpreterConfigurator::stSetDataReference(){
  if( !m_rep->isDataRefVar ){
    m_rep -> dataitem->setDataReference( m_rep -> dataref );
    m_rep -> dataitem->setDimensionIndizes();
  }
  return true;
}
//======================================================================//
// - pushDataItem
//======================================================================//
bool InterpreterConfigurator::pushDataItem(){
  m_rep -> pushDataItem();
  m_rep -> pushDataItemIndex();
  return true;
}

//======================================================================//
// - popDataItem
//======================================================================//
bool InterpreterConfigurator::popDataItem(){
  m_rep -> dataitem = m_rep -> popDataItem();
  m_rep -> dataitemindex = m_rep -> popDataItemIndex();
  return true;
}


//======================================================================//
// - getNextDataReference
//======================================================================//
bool InterpreterConfigurator::getNextDataReference( const std::string &id ){
  if( !m_rep->isDataRefVar ){
    m_rep -> tempref = DataPoolIntens::getDataReference( m_rep -> dataref, id );
    if( m_rep -> tempref == 0 )
      ParserError( compose(_("Undeclared structure field '%1'"), id) );
    delete m_rep -> dataref;
    m_rep -> dataref = m_rep -> tempref;
    m_rep -> tempref = 0;
  }
  // Es handelt sich um eine Variable Datenreferenz. Der Stream muss
  // die Referenz selbst auflösen
  else{
    assert( m_rep->varStreamParameter != 0 );
    m_rep->varStreamParameter->addLevel( id );
  }
  m_rep -> indexnum= 0;
  m_rep -> datalevel++;
  return true;
}

//======================================================================//
// - newDataItemIndex
//======================================================================//
bool InterpreterConfigurator::newDataItemIndex( const std::string &name ){
  m_rep -> dataitemindex = m_rep -> dataitem->newDataItemIndex( m_rep -> datalevel );
  m_rep -> dataitemindex->setIndexNum( m_rep -> indexnum );
  m_rep -> dataitemindex->setIndexName( name, "" );
  m_rep -> indexnum++;
  return true;
}
bool InterpreterConfigurator::newDataItemIndex( int val ){
  m_rep -> dataitemindex = m_rep -> dataitem->newDataItemIndex( m_rep -> datalevel );
  m_rep -> dataitemindex->setIndexNum( m_rep -> indexnum );
  m_rep -> dataitemindex->setLowerbound( val );
  m_rep -> indexnum++;
  return true;
}
bool InterpreterConfigurator::newDataItemIndex(){
  m_rep->dataitemindex = m_rep->dataitem->newDataItemIndex( m_rep->datalevel );
  return true;
}
//======================================================================//
// - addIndexContainer
//======================================================================//
bool InterpreterConfigurator::addIndexContainer( const std::string &indexID ){
  m_rep -> element = GuiElement::findElement( indexID );
  if( m_rep -> element == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), indexID) );
  }
  if( m_rep -> element->Type() != GuiElement::type_Index ){
    ParserError( compose(_("'%1' is not of type index."), indexID) );
  }
  m_rep -> dataitemindex = m_rep -> dataitem->newDataItemIndex( m_rep -> datalevel, true );
  m_rep -> dataitemindex->setIndexNum( m_rep -> indexnum );
  m_rep -> dataitemindex->setIndexName( indexID, "" );
  m_rep -> indexnum++;
//   m_rep -> indexfield = static_cast<GuiIndex *>(m_rep -> element);
  m_rep -> indexfield = m_rep -> element->getGuiIndex();
  GuiIndex::addIndexContainer( indexID, m_rep -> indexfield );
  return true;
}
//======================================================================//
// - addParameter
//======================================================================//
bool InterpreterConfigurator::addParameter( const bool optflag ){
  if( m_rep->isDataRefVar ){
    if( optflag )
      ParserError( _("Validation check with VAR() is not supported.") );
    m_rep->isDataRefVar = false;
    assert( m_rep->varStreamParameter != 0 );
    m_rep->varStreamParameter -> setConverterValues( m_rep->lengthvalue,
                                                     m_rep->prec, m_rep->getScale(),
                                                     m_rep->thousandsep, m_rep->streamLocaleFlag );
  }
  else{
    StreamParameter *p=m_rep -> stream -> addParameter( getDataItem(),
                                                        m_rep->lengthvalue, m_rep->prec
                                                        , m_rep->getScale(),
                                                        m_rep->thousandsep, optflag, false );
    DataPoolIntens::Instance().setTargetStream( getDataItem()->Data(), m_rep->stream );
    GuiIndex::registerIndexContainer( p );
  }
  return true;
}
//======================================================================//
// - addVarParameter
//======================================================================//
bool InterpreterConfigurator::addVarParameter(){
  if( getDataItem()->Data()->getDataType() != DataDictionary::type_String )
    ParserError( compose(_("'%1' is not of type STRING."),
                         getDataItem()->getFullName(true)) );
  XferDataItem *d=m_stack.empty() ? 0 : m_stack.top()->dataitem;
  m_rep->varStreamParameter =
    static_cast<VarStreamParameter*>( m_rep -> stream -> addVarParameter( getDataItem(), d ) );

  DataPoolIntens::Instance().setTargetStream( getDataItem()->Data(),
                                              m_rep->stream );
  GuiIndex::registerIndexContainer( m_rep->varStreamParameter );
  if( !m_stack.empty() ){
    m_stack.top()->varStreamParameter = m_rep->varStreamParameter;
    m_rep = m_stack.top();
    m_stack.pop();
  }
  else{
    m_rep -> dataitem = new XferDataItem();
    m_rep -> varStreamParameter -> setItem( m_rep->dataitem );
    m_rep -> datalevel = 0;
    m_rep -> indexnum=0;
    GuiIndex::clearIndexContainer();
    m_rep->isDataRefVar = true;
  }
  return true;
}

//======================================================================//
// - setXMLSchema
//======================================================================//
bool InterpreterConfigurator::setXMLSchema( const std::string &schema ){
  m_rep->xml_schema=schema;
  return true;
}

//======================================================================//
// - setXMLNamespace
//======================================================================//
bool InterpreterConfigurator::setXMLNamespace( const std::string &name ){
  m_rep->xml_namespace=name;
  return true;
}
//======================================================================//
// - setXMLVersion
//======================================================================//
bool InterpreterConfigurator::setXMLVersion( const std::string &ver ){
  m_rep->xml_version=ver;
  return true;
}
//======================================================================//
// - setXMLStylesheet
//======================================================================//
bool InterpreterConfigurator::setXMLStylesheet( const std::string &xsl ){
  m_rep->xml_stylesheet=xsl;
  return true;
}

//======================================================================//
// - addXMLParameter
//======================================================================//
bool InterpreterConfigurator::addXMLParameter(){
  StreamParameter *param = m_rep->stream->addXMLParameter( getDataItem()
                                                           , m_rep->xmlAttributes
                                                           , m_rep->allCycles
                                                           , m_rep->xml_schema
                                                           , m_rep->xml_namespace
                                                           , m_rep->xml_version
                                                           , m_rep->xml_stylesheet );
  m_rep -> xmlAttributes.clear();
  m_rep->xml_schema="";
  m_rep->xml_namespace="";
  m_rep->xml_version="";
  m_rep->xml_stylesheet="";
  m_rep -> allCycles = false;
  if( getDataItem() != 0 ){
    DataPoolIntens::Instance().setTargetStream( getDataItem()->Data(), m_rep->stream );
    GuiIndex::registerIndexContainer( param );
  }
  return true;
}
//======================================================================//
// - addJSONParameter
//======================================================================//
bool InterpreterConfigurator::addJSONParameter(){
  StreamParameter *param = m_rep->stream->addJSONParameter( getDataItem()
                                                            , m_rep->allCycles
                                                            );
  m_rep -> allCycles = false;
  if( getDataItem() != 0 ){
    DataPoolIntens::Instance().setTargetStream( getDataItem()->Data(), m_rep->stream );
    GuiIndex::registerIndexContainer( param );
  }
  return true;
}
//======================================================================//
// - withAllCycles
//======================================================================//
bool InterpreterConfigurator::withAllCycles(){
  m_rep->allCycles = true;
  return true;
}
//======================================================================//
// - addXMLAttr
//======================================================================//
bool InterpreterConfigurator::addXMLAttr( const std::string &attr ){
  m_rep -> xmlAttributes.push_back( attr );
  return true;
}
//======================================================================//
// - addMatrixParameter
//======================================================================//
bool InterpreterConfigurator::addMatrixParameter( const bool optflag,
                                                  const char delimiter ){
  if( delimiter != '\0' &&
      getDataItem()->Data()->getDataType() != DataDictionary::type_String )
    ParserError( compose(_("'%1' is not of type STRING."), getDataItem()->getFullName(true)) );
  StreamParameter *p = m_rep -> stream -> addMatrixParameter( getDataItem(),
                                                              m_rep->lengthvalue,
                                                              m_rep->prec,
                                                              m_rep->getScale(),
                                                              m_rep->thousandsep,
                                                              delimiter, optflag );
  if( p == 0)
    ParserError( compose(_("Undeclared identifier '%1'."), "") );
  DataPoolIntens::Instance().setTargetStream( getDataItem()->Data(), m_rep->stream );
  GuiIndex::registerIndexContainer( p );
  //  DataPoolIntens::Instance().setTargetStream( m_rep->dataref, m_rep->stream );
  return true;
}
//======================================================================//
// - addDatasetParameter
//======================================================================//
bool InterpreterConfigurator::addDatasetParameter(){
  if( m_rep -> stream -> addDatasetParameter( getDataItem() ) == NULL) {
    ParserError( _("Cannot add DATASET_TEXT from item that has no DATASET.") );
    return false;
  }
  DataPoolIntens::Instance().setTargetStream( getDataItem()->Data(), m_rep->stream );
  return true;
}
//======================================================================//
// - addIndexParameter
//======================================================================//
bool InterpreterConfigurator::addIndexParameter( const std::string &name, int length ){
  m_rep -> stream -> addIndexParameter( name, length );
  return true;
}
//======================================================================//
// - addPlotGroupParameter
//======================================================================//
bool InterpreterConfigurator::addPlotGroupParameter( const std::string &plotId ){
  m_rep -> streamparameter = m_rep -> stream -> addPlotGroupParameter( plotId );
  if ( m_rep -> streamparameter == 0 )
    ParserError( _("Cannot add plotgroup.") );
  return true;
}
//======================================================================//
// - setPlotGroupParameterTransparentBackground
//======================================================================//
bool InterpreterConfigurator::setPlotGroupParameterTransparentBackground( bool transparent ){
  if( typeid(*m_rep->streamparameter) == typeid(PlotGroupStreamParameter) ) {
    dynamic_cast<PlotGroupStreamParameter *>(m_rep->streamparameter) -> setTransparentBackground( transparent );
  }
  else {
    ParserError( compose(_("'%1' is not a PLOTGROUP."), m_rep->streamparameter->getName()) );
  }
  return true;
}
//======================================================================//
// - setXRangeReal
//======================================================================//
bool InterpreterConfigurator::setXRangeReal( const double value1, const double value2 ){
  if( typeid(*m_rep->streamparameter) == typeid(PlotGroupStreamParameter) ) {
    dynamic_cast<PlotGroupStreamParameter *>(m_rep->streamparameter) -> setXRange( value1, value2 );
  }
  else {
    ParserError( compose(_("'%1' is not a PLOTGROUP."), m_rep->streamparameter->getName()) );
  }
  return true;
}
//======================================================================//
// - setXRangeDataRef
//======================================================================//
bool InterpreterConfigurator::setXRangeDataRef(){
  if( typeid(*m_rep->streamparameter) == typeid(PlotGroupStreamParameter) ) {
    XferDataItem *xitem2=m_rep->popDataItem();
    XferDataItem *xitem1=m_rep->popDataItem();
    if ( ! dynamic_cast<PlotGroupStreamParameter *>(m_rep->streamparameter) -> setXRange( xitem1, xitem2,  m_rep->getScale(), m_rep->getScale(), m_rep->stream->getCurrentGroup() ) ) {
      ParserError( _("DataReference is not numeric.") );
    }
  }
  else {
    ParserError( compose(_("'%1' is not a PLOTGROUP."), m_rep->streamparameter->getName()) );
  }
  return true;
}
//======================================================================//
// - addXMLPlotGroupStreamParameter
//======================================================================//
bool InterpreterConfigurator::addXMLPlotGroupStreamParameter( const std::string &pluginId ){
  m_rep -> plugin = GuiManager::Instance().getPlugin( pluginId );
  if( m_rep -> plugin == 0 ){
    ParserError( compose(_("'%1' is not a PLUGIN."), pluginId) );
  }
  m_rep -> streamparameter = m_rep -> stream -> addXMLPlotGroupStreamParameter( m_rep -> plugin );
  if ( m_rep -> streamparameter == 0 )
    ParserError( _("Cannot add plotgroup.") );
  return true;
}
//======================================================================//
// - addToken
//======================================================================//
bool InterpreterConfigurator::addToken( const std::string &token, int length ){
  bool setFormat = false;
  if (m_rep->dataref)
    if (UserAttr *u = dynamic_cast<UserAttr*>(m_rep->dataref->getUserAttr()) )
      if (u->Unit(false) == token || u->Label(m_rep->dataref) == token)
        setFormat = true;
  if ( m_rep -> stream ->addToken( token, length, setFormat ) == 0 )
    ParserError( _("Cannot allocate token.") );
  return true;
}
//======================================================================//
// - addGuiIndexParameter
//======================================================================//
bool InterpreterConfigurator::addGuiIndexParameter( const std::string &indexId ){
  m_rep->indexfield = m_rep->guifactory->getGuiIndex( indexId );
  if( m_rep->indexfield == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), indexId) );
  }
  m_rep -> stream -> addGuiIndexParameter( m_rep->indexfield );
  return true;
}
//======================================================================//
// - addSerializedFormParameter
//======================================================================//
bool InterpreterConfigurator::addSerializedFormParameter( const std::string &xslFile, const std::string &formId ){
  if( ! formId.empty() ) addSerializeFormId( formId );
  m_rep->stream->addSerializedFormParameter( m_rep->stSerializeFormIdList, xslFile );
  while( ! m_rep->stSerializeFormIdList.empty() ){
    m_rep->stSerializeFormIdList.pop_back();
  }
  return true;
}
//======================================================================//
// - addSerializedFormId
//======================================================================//
bool InterpreterConfigurator::addSerializeFormId( const std::string &formId ){
  m_rep->stSerializeFormIdList.push_back( formId );
  return true;
}
//======================================================================//
// - addStringDateParameter
//======================================================================//
bool InterpreterConfigurator::addStringDateParameter(UserAttr::STRINGtype type, int length) {
  m_rep -> stream -> addStringDateParameter( getDataItem(), type, length );
  DataPoolIntens::Instance().setTargetStream( getDataItem()->Data(), m_rep->stream );
  return true;
}
//**********************************************************************//
// END STREAMER
//**********************************************************************//

//**********************************************************************//
// FIELD CONVERSION
//**********************************************************************//
bool InterpreterConfigurator::resetConversionVariables(){
  m_rep -> lengthvalue = 0;
  m_rep -> prec = -1;
  m_rep -> thousandsep = false;
  return true;
}
bool InterpreterConfigurator::setConversionVarLengthvalue( const int sign, const int value){
  m_rep -> lengthvalue = sign * value;
  return true;
}
bool InterpreterConfigurator::setConversionVarPrec( const int precision ){
  m_rep -> prec = precision;
  return true;
}
bool InterpreterConfigurator::setConversionVarThousandsep( const bool showsep ){
  m_rep -> thousandsep = showsep;
  return true;
}
bool InterpreterConfigurator::setScaleFactor( double value ){
  m_rep->addScale( '*', value );
  return true;
}
// bool InterpreterConfigurator::setXferScaleFactor(){
//   XferDataItem *xfer = m_rep->popDataItem();
//   if( xfer->Data()->GetDataType() != DataDictionary::type_Real
//       && xfer->Data()->GetDataType() != DataDictionary::type_Integer )
//     ParserError( _("DataReference is not numeric.") );
//   m_rep->addScale( '*', xfer );
//   return true;
// }
bool InterpreterConfigurator::setScaleFactor( XferDataItem *xfer ){
  if( xfer->Data()->getDataType() != DataDictionary::type_Real
      && xfer->Data()->getDataType() != DataDictionary::type_Integer )
    ParserError( _("DataReference is not numeric.") );
  m_rep->addScale( '*', xfer );
  return true;
}
bool InterpreterConfigurator::setScaleDivisor( double value ){
  m_rep->addScale( '/', value );
  return true;
}
// bool InterpreterConfigurator::setXferScaleDivisor(){
//   XferDataItem *xfer = m_rep->popDataItem();
//   if( xfer->Data()->GetDataType() != DataDictionary::type_Real
//       && xfer->Data()->GetDataType() != DataDictionary::type_Integer )
//     ParserError( _("DataReference is not numeric.") );
//   m_rep->addScale( '/', xfer );
//   return true;
// }
bool InterpreterConfigurator::setScaleDivisor( XferDataItem *xfer ){
  if( xfer->Data()->getDataType() != DataDictionary::type_Real
      && xfer->Data()->getDataType() != DataDictionary::type_Integer )
    ParserError( _("DataReference is not numeric.") );
  m_rep->addScale( '/', xfer );
  return true;
}

bool InterpreterConfigurator::setFieldColSpan( int colspan, bool last ) {
  if (colspan < 1)
    return false;
  if ( last ) {
    GuiElement *elem = m_rep->fieldgroupline->getLastElement();
    if (elem) {
      elem->setColSpan( colspan );
    } else assert( false );
  } else {
    m_rep -> datafield->getElement()->setColSpan( colspan );
  }
  return true;
}

bool InterpreterConfigurator::setFieldRowSpan( int rowspan, bool last ) {
  if (rowspan < 1)
    return false;
  if ( last ) {
    GuiElement *elem = m_rep->fieldgroupline->getLastElement();
    if (elem) {
      elem->setRowSpan( rowspan );
    } else assert( false );
  } else {
    m_rep -> datafield->getElement()->setRowSpan( rowspan );
  }
  return true;
}

//======================================================================//
// - setFieldOrientation
//======================================================================//
bool InterpreterConfigurator::setFieldOrientation( GuiElement::Orientation orientation, bool last ){
  GuiElement *elem = 0;
  if (last) {
	if (!elem && m_rep->fieldgroupline)
	  elem = m_rep->fieldgroupline->getLastElement();
  } else {
	if (!elem && m_rep->datafield)
	  elem = m_rep->datafield->getElement();
  }
  if (elem) {
    elem->setOrientation( orientation );
  }

  return true;
}

//======================================================================//
// - setFieldRotation
//======================================================================//
bool InterpreterConfigurator::setFieldRotation(int rotation, bool last){
  GuiElement *elem = 0;
  if (last) {
	if (!elem && m_rep->fieldgroupline)
	  elem = m_rep->fieldgroupline->getLastElement();
  } else {
	if (!elem && m_rep->datafield)
	  elem = m_rep->datafield->getElement();
  }
  if (elem) {
    elem->setRotation(rotation);
  }
  return true;
}

//======================================================================//
// - setFieldHelptextStream
//======================================================================//
bool InterpreterConfigurator::setFieldHelptextStream( const std::string &streamId, bool last ) {
  GuiElement *elem = 0;
  if (last) {
    if (!elem && m_rep->fieldgroupline)
      elem = m_rep->fieldgroupline->getLastElement();
  } else {
    if (!elem && m_rep->datafield)
      elem = m_rep->datafield->getElement();
  }
  if (elem) {
    elem->setHelptextStream( streamId );
  }
  return true;
}

//======================================================================//
// - setFieldClass
//======================================================================//
bool InterpreterConfigurator::setFieldClass(const std::string &classname, bool last){
  GuiElement *elem = 0;
  if( classname.empty() ){
    ParserError( _("Invalid Fieldgroup Field Class.") );
  }
  if (last) {
    if (!elem && m_rep->fieldgroupline)
      elem = m_rep->fieldgroupline->getLastElement();
  } else {
    if (!elem && m_rep->datafield)
      elem = m_rep->datafield->getElement();
  }
  if (elem) {
    elem->setClass( classname );
  }

  return true;
}

//======================================================================//
// - getDataReference
//======================================================================//
bool InterpreterConfigurator::getDataReference( const std::string &id ){
  m_rep -> dataref = DataPoolIntens::getDataReference( id );
  if( m_rep -> dataref == 0 )
    ParserError( compose(_("Undeclared identifier '%1'."), id) );
  return true;
}
//======================================================================//
// - getDataReferenceStruct
//======================================================================//
bool InterpreterConfigurator::getDataReferenceStruct( const std::string &id ){
  m_rep -> tempref = DataPoolIntens::getDataReference( m_rep -> dataref, id );
  if( m_rep -> tempref == 0 )
    ParserError( compose(_("Undeclared structure field '%1'"), id) );
  delete m_rep -> dataref;
  m_rep -> dataref = m_rep -> tempref;
  m_rep -> tempref = 0;
  m_rep -> datalevel++;
  return true;
}

//**********************************************************************//
// UI_MANAGER
//**********************************************************************//
/* --------------------------------------------------------------------------- */
/* UI Datafield Attribute Functions                                            */
/* --------------------------------------------------------------------------- */

//======================================================================//
// - setFieldAttributes
//======================================================================//
bool InterpreterConfigurator::setFieldAttributes(){
  m_rep -> fieldalign        = GuiElement::align_Default;
  m_rep -> fieldlength       = 0;
  m_rep -> fieldprec         = -1;
  m_rep -> addScale( '*', 1.0 );
  m_rep -> fieldthousandsep = false;
  return true;
}
bool InterpreterConfigurator::setFieldAttributes( const int len, GuiElement::Alignment align, const bool tsep ){
  m_rep -> fieldalign       = align;
  m_rep -> fieldlength      = len;
  m_rep -> fieldthousandsep = tsep;
  return true;
}
//======================================================================//
// - setDataFieldLabel
//======================================================================//
bool InterpreterConfigurator::setDataFieldLabel( const std::string &label ){
  m_rep -> datafield->getElement()->setLabel( label );
  return true;
}
//======================================================================//
// - setDataFieldPixmap
//======================================================================//
bool InterpreterConfigurator::setDataFieldPixmap( const std::string &id ){
  m_rep -> datafield->getElement()->setPixmap( id );
  return true;
}
//======================================================================//
// - setDataFieldSetGuiElementSize
//======================================================================//
bool InterpreterConfigurator::setDataFieldSetGuiElementSize( const int width, const int height ){
  m_rep -> datafield->getElement()->setElementSize( width, height );
  return true;
}
//======================================================================//
// - setDataFieldScrollbar
//======================================================================//
bool InterpreterConfigurator::setDataFieldScrollbar(){
  m_rep -> datafield->getElement()->setScrollbar( GuiElement::scrollbar_AS_NEEDED );
  return true;
}
//======================================================================//
// - setDataFieldAutoScroll
//======================================================================//
bool InterpreterConfigurator::setDataFieldAutoScroll(){
  m_rep -> datafield->getElement()->setAutoScroll();
  return true;
}
//======================================================================//
// - setDataFieldExpandable
//======================================================================//
bool InterpreterConfigurator::setDataFieldExpandable(){
  m_rep -> datafield->getElement()->setExpandable( GuiElement::status_ON );
  return true;
}
//======================================================================//
// - setDataFieldAttributes
//======================================================================//
bool InterpreterConfigurator::setDataFieldAttributes(){
  m_rep->datafield->getElement()->setAlignment( m_rep->fieldalign );
  m_rep->datafield->setLength( m_rep->fieldlength );
  if( m_rep->fieldprec >= 0 )
    m_rep->datafield->setPrecision( m_rep->fieldprec );
  m_rep->datafield->setScalefactor( m_rep->getScale() );
  if( m_rep->fieldthousandsep )
    m_rep->datafield->setThousandSep();
  return true;
}
//======================================================================//
// - setFieldPrecision
//======================================================================//
bool InterpreterConfigurator::setFieldPrecision( const int precision ){
  m_rep->fieldprec = precision;
  return true;
}
//======================================================================//
// - setVoidfieldSize
//======================================================================//
bool InterpreterConfigurator::setVoidfieldSize( const int xvalue, const int yvalue ){
  m_rep->voidfield->setWidth( xvalue );
  m_rep->voidfield->setHeight( yvalue );
  return true;
}
//======================================================================//
// - setVoidfieldDisplayPercentSize
//======================================================================//
bool InterpreterConfigurator::setVoidfieldDisplayPercentSize( const int xvalue, const int yvalue ){
  m_rep->voidfield->setDisplayPercentWidth( xvalue );
  m_rep->voidfield->setDisplayPercentHeight( yvalue );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI Datafield Data Definition                                                */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - uiNewXferDataItem
//======================================================================//
bool InterpreterConfigurator::uiNewXferDataItem( const std::string &id, bool clearIndexContainer ){
  m_rep -> dataref = DataPoolIntens::getDataReference( id );
  if( m_rep -> dataref == 0 )
    ParserError( compose(_("Undeclared identifier '%1'."), id) );
  m_rep -> dataitem = new XferDataItem();
  m_rep -> datalevel = 0;
  if( clearIndexContainer )
    GuiIndex::clearIndexContainer();
  return true;
}
//======================================================================//
// - uiNewXferDataItemComplexReal1387
//======================================================================//
bool InterpreterConfigurator::uiNewXferDataItemComplexReal( const std::string &id ){
  m_rep -> dataref = DataPoolIntens::getDataReference( id );
  if( m_rep -> dataref == 0 )
    ParserError( compose(_("Undeclared identifier '%1'."), id) );
  m_rep -> dataitem = new XferDataItem();
  m_rep -> dataitem -> setTypeComplexReal();
  m_rep -> datalevel = 0;
  GuiIndex::clearIndexContainer();
  return true;
}
//======================================================================//
// - uiNewXferDataItemComplexImag
//======================================================================//
bool InterpreterConfigurator::uiNewXferDataItemComplexImag( const std::string &id ){
  m_rep -> dataref = DataPoolIntens::getDataReference( id );
  if( m_rep -> dataref == 0 )
    ParserError( compose(_("Undeclared identifier '%1'."), id) );
  m_rep -> dataitem = new XferDataItem();
  m_rep -> dataitem -> setTypeComplexImag();
  m_rep -> datalevel = 0;
  GuiIndex::clearIndexContainer();
  return true;
}
//======================================================================//
// - uiNewXferDataItemComplexAbs
//======================================================================//
bool InterpreterConfigurator::uiNewXferDataItemComplexAbs( const std::string &id ){
  m_rep -> dataref = DataPoolIntens::getDataReference( id );
  if( m_rep -> dataref == 0 )
    ParserError( compose(_("Undeclared identifier '%1'."), id) );
  m_rep -> dataitem = new XferDataItem();
  m_rep -> dataitem -> setTypeComplexAbs();
  m_rep -> datalevel = 0;
  GuiIndex::clearIndexContainer();
  return true;
}
//======================================================================//
// - uiNewXferDataItemComplexArg
//======================================================================//
bool InterpreterConfigurator::uiNewXferDataItemComplexArg( const std::string &id ){
  m_rep -> dataref = DataPoolIntens::getDataReference( id );
  if( m_rep -> dataref == 0 )
    ParserError( compose(_("Undeclared identifier '%1'."), id) );
  m_rep -> dataitem = new XferDataItem();
  m_rep -> dataitem -> setTypeComplexArg();
  m_rep -> datalevel = 0;
  GuiIndex::clearIndexContainer();
  return true;
}
//======================================================================//
// - uiNewXferDataItemFilestream
//======================================================================//
bool InterpreterConfigurator::uiNewXferDataItemFilestream( const std::string &id ){
  m_rep -> filestream = FileStream::find( id );
  if( m_rep -> filestream == 0 )
    ParserError( compose(_("FILESTREAM '%1' not found."), id) );
  m_rep -> dataref = m_rep -> filestream->getDataFilename();
  m_rep -> dataitem = new XferDataItem();
  m_rep -> datalevel = 0;
  GuiIndex::clearIndexContainer();
  m_rep -> dataitem->setDataReference( m_rep->dataref );
  return true;
}
//======================================================================//
// - uiSetDataReference
//======================================================================//
bool InterpreterConfigurator::uiSetDataReference(){
  m_rep->dataitem->setDataReference( m_rep->dataref );
  return true;
}
//======================================================================//
// - setWildcard
//======================================================================//
bool InterpreterConfigurator::setWildcard(){
  m_rep->dataitemindex->setWildcard();
  return true;
}
//======================================================================//
// - setLowerbound
//======================================================================//
bool InterpreterConfigurator::setLowerbound( const int indexConstant ){
  m_rep->dataitemindex->setLowerbound( indexConstant );
  return true;
}
//======================================================================//
// - setUpperbound
//======================================================================//
bool InterpreterConfigurator::setUpperbound( const int indexConstant ){
  m_rep->dataitemindex->setUpperbound( indexConstant );
  return true;
}
//======================================================================//
// - uiAddIndexContainer
//======================================================================//
bool InterpreterConfigurator::uiAddIndexContainer( const std::string &indexId ){
  m_rep->element = GuiElement::findElement( indexId );
  if( m_rep->element == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), indexId) );
  }
  if( m_rep->element->Type() != GuiElement::type_Index ){
    ParserError( compose(_("'%1' is not of type index."), indexId) );
  }
  m_rep->dataitemindex->setIndexName( indexId, "" );
//   m_rep->indexfield = static_cast<GuiIndex *>(m_rep->element);
  m_rep -> indexfield = m_rep -> element->getGuiIndex();
  GuiIndex::addIndexContainer( indexId, m_rep->indexfield );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI Fieldgroup Definition                                                    */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newFieldgroup
//======================================================================//
bool InterpreterConfigurator::newFieldgroup( const std::string &fieldgroupId ){
  if( !IdManager::Instance().registerId( fieldgroupId, IdManager::id_Fieldgroup ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), fieldgroupId) );
  }
  m_rep->fieldgroup = m_rep->guifactory->createFieldgroup( 0, fieldgroupId );
  if( m_rep->fieldgroup == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), fieldgroupId) );
  }
  return true;
}
//======================================================================//
// - hideFieldgroupIndexMenu
//======================================================================//
bool InterpreterConfigurator::hideFieldgroupIndexMenu(){
  if( m_rep->fieldgroup != 0 )
    m_rep->fieldgroup->hideIndexMenu();
  return true;
}
//======================================================================//
// - hideFieldgroupIndexLabel
//======================================================================//
bool InterpreterConfigurator::hideFieldgroupIndexLabel(){
  if( m_rep->fieldgroup != 0 )
    m_rep->fieldgroup->hideIndexLabel();
  return true;
}
//======================================================================//
// - setTitleHelpfile
//======================================================================//
bool InterpreterConfigurator::setTitleHelpfile( const std::string &title ){
  m_rep -> helpFile -> setMenuTitle( title );
  return true;
}
//======================================================================//
// - setTitleFieldgroup
//======================================================================//
bool InterpreterConfigurator::setTitleFieldgroup( const std::string &title,
                                                  GuiElement::Alignment alignment ){
  m_rep->fieldgroup->setTitle( UnitManager::extractValue(title) );
  m_rep->fieldgroup->setTitleAlignment( alignment );
  return true;
}
//======================================================================//
// - setTitlelist
//======================================================================//
bool InterpreterConfigurator::setTitlelist( const std::string &title,
                                            GuiElement::Alignment alignment ){
  m_rep->list->setTitle( UnitManager::extractValue(title), alignment );
  return true;
}
//======================================================================//
// - setTitleForm
//======================================================================//
bool InterpreterConfigurator::setTitleForm( const std::string &title ){
  m_rep->form->setTitle( UnitManager::extractValue(title) );
  return true;
}
//======================================================================//
// - setTitleListplot
//======================================================================//
bool InterpreterConfigurator::setTitleListplot( const std::string &title ){
  m_rep->listplot->setTitle( UnitManager::extractValue(title) );
  return true;
}
//======================================================================//
// - setTitleListgraph
//======================================================================//
bool InterpreterConfigurator::setTitleListgraph( const std::string &title ){
  if (!m_rep->listgraph->setStringTitle( UnitManager::extractValue(title) ))
    ParserError( compose(_("Undeclared identifier '%1'."), title) );
  return true;
}
//======================================================================//
// - setTitleListgraphStream
//======================================================================//
bool InterpreterConfigurator::setTitleListgraphStream( const std::string &title ){
  if (!m_rep->listgraph->setStreamTitle( UnitManager::extractValue(title) ))
    ParserError( compose(_("Undeclared identifier '%1'."), title) );
  return true;
}
//======================================================================//
// - setTitleSimpel
//======================================================================//
bool InterpreterConfigurator::setTitleSimpel( const std::string &title ){
  m_rep->simpel->setTitle( UnitManager::extractValue(title) );
  return true;
}
//======================================================================//
// - setTitlePsplot
//======================================================================//
bool InterpreterConfigurator::setTitlePsplot( const std::string &title ){
  m_rep->psplot->setTitle( UnitManager::extractValue(title) );
  return true;
}
//======================================================================//
// - setTitleTable
//======================================================================//
bool InterpreterConfigurator::setTitleTable( const std::string &title, GuiElement::Alignment alignment ){
  m_rep->table->setTitle( UnitManager::extractValue(title), alignment );
  return true;
}
//======================================================================//
// - setTitleTableHorizontal
//======================================================================//
bool InterpreterConfigurator::setTitleTableHorizontal( const std::string &title ){
  m_rep->table->setTitleHorizontal( UnitManager::extractValue(title) );
  return true;
}
//======================================================================//
// - setTitleTableVertical
//======================================================================//
bool InterpreterConfigurator::setTitleTableVertical( const std::string &title ){
  m_rep->table->setTitleVertical( UnitManager::extractValue(title) );
  return true;
}
//======================================================================//
// - setTitleReportstream
//======================================================================//
bool InterpreterConfigurator::setTitleReportstream( const std::string &title ){
  m_rep->reportstream->setTitle( UnitManager::extractValue(title) );
  return true;
}
//======================================================================//
// - setHiddenReportstream
//======================================================================//
bool InterpreterConfigurator::setHiddenReportstream(){
  m_rep->reportstream->setHidden();
  return true;
}
//======================================================================//
// - setFieldgroupTableSize
//======================================================================//
bool InterpreterConfigurator::setFieldgroupTableSize( const int tablesize, const int tablestep ){
  if( tablestep > tablesize ){
    ParserError( compose(_("Table step is bigger than tablesize. (Step: %1, Size: %2)"),tablestep,tablesize) );
  }
  m_rep->fieldgroup->setTableSize( tablesize );
  m_rep->fieldgroup->setTableStep( tablestep );
  return true;
}
//======================================================================//
// - setFieldgroupOrientation
//======================================================================//
bool InterpreterConfigurator::setFieldgroupOrientation( GuiElement::Orientation orientation ){
  m_rep->fieldgroup->setOrientation( orientation );
  return true;
}
//======================================================================//
// - setFieldgroupNavigation
//======================================================================//
bool InterpreterConfigurator::setFieldgroupNavigation( GuiElement::Orientation orientation ){
  m_rep->fieldgroup->setNavigation( orientation );
  return true;
}
//======================================================================//
// - setTablePosition
//======================================================================//
bool InterpreterConfigurator::setTablePosition( const int position ){
  m_rep->fieldgroup->setTablePosition( position );
  return true;
}
//======================================================================//
// - setTableIndexRange
//======================================================================//
bool InterpreterConfigurator::setTableIndexRange( const int start,
                                                  const int end ){
  if( !m_rep->fieldgroup->setTableIndexRange( start, end ) ){
    ParserError( _("Invalid Table-Index-Range.") );
  }
  return true;
}
//======================================================================//
// - fieldgroupSetMargin
//======================================================================//
bool InterpreterConfigurator::fieldgroupSetMargin( const int margin,
                                                   const int spacing ){
  if( !m_rep->fieldgroup->setMargins( margin, spacing ) ){
    ParserError( _("Invalid Fieldgroup Margins.") );
  }
  return true;
}
//======================================================================//
// - fieldgroupSetGuiIndex
//======================================================================//
bool InterpreterConfigurator::fieldgroupSetGuiIndex(const std::string &indexId){
  m_rep->indexfield = m_rep->guifactory->getGuiIndex( indexId );
  if( m_rep->indexfield == 0 ){
    ParserError( _("Invalid Fieldgroup GUI Index.") );
  }
  m_rep->fieldgroup->setGuiIndex( m_rep->indexfield );
  return true;
}
//======================================================================//
// - fieldgroupSetClass
//======================================================================//
bool InterpreterConfigurator::fieldgroupSetClass(const std::string &classname){
  if( classname.empty() ){
    ParserError( _("Invalid Fieldgroup Class.") );
  }
  m_rep->fieldgroup->getElement()->setClass( classname );
  return true;
}
//======================================================================//
// - fieldgroupSetOverlay
//======================================================================//
bool InterpreterConfigurator::fieldgroupSetOverlay(const int xpos, const int ypos,
                                                   const int width, const int height){
  m_rep->fieldgroup->setOverlayGeometry(xpos, ypos, width, height);
  return true;
}
//======================================================================//
// - fieldgroupWithScrollBar
//======================================================================//
bool InterpreterConfigurator::fieldgroupWithScrollBar(){
  m_rep->fieldgroup->getElement()->setScrollbar( GuiElement::scrollbar_AS_NEEDED );
  return true;
}

//======================================================================//
// - setAlignment
//======================================================================//
bool InterpreterConfigurator::setAlignment( GuiElement::Alignment alignment ){
  m_rep->fieldgroup->getElement()->setAlignment( alignment );
  return true;
}
//======================================================================//
// - setAlignFields
//======================================================================//
bool InterpreterConfigurator::setAlignFields(){
  m_rep->fieldgroup->setAlignFields();
  return true;
}
//======================================================================//
// - newFieldgroupLine
//======================================================================//
bool InterpreterConfigurator::newFieldgroupLine(){
  m_rep->fieldgroupline = m_rep->fieldgroup->addFieldgroupLine();
  return true;
}
//======================================================================//
// - addArrowBar
//======================================================================//
bool InterpreterConfigurator::addArrowBar(){
  if ( m_rep->fieldgroup->getTableSize() == 0 ){
    ParserError( _("Only allowed in tables. (Fieldgroup must have TABLESIZE set)") );
  }
  m_rep->fieldgroup->addArrowbar();
  return true;
}
//======================================================================//
// - fieldgroupSetFrameOn
//======================================================================//
bool InterpreterConfigurator::fieldgroupSetFrameOn(){
  m_rep->fieldgroup->getElement()->setFrame( GuiElement::status_ON );
  return true;
}
//======================================================================//
// - fieldgroupSetUseRuler
//======================================================================//
bool InterpreterConfigurator::fieldgroupSetUseRuler(){
  m_rep->fieldgroup->getElement()->setUseRuler();
  return true;
}

//======================================================================//
// - attachStringConstant
//======================================================================//
bool InterpreterConfigurator::attachStringConstant( const std::string &labelstring,
                                                    GuiElement::Alignment alignment ){
  m_rep->labelfield = m_rep->guifactory->createLabel( m_rep->fieldgroupline->getElement(),
                                                      alignment );
  m_rep->labelfield->setLabel( labelstring );

  return true;
}
//======================================================================//
// - attachPixmap
//======================================================================//
bool InterpreterConfigurator::attachPixmap( const std::string &pixmapstring,
                                            GuiElement::Alignment alignment ){
  m_rep->labelfield = m_rep->guifactory->createLabel( m_rep->fieldgroupline->getElement(),
                                                      alignment );
  m_rep->labelfield->setPixmap( pixmapstring );
  return true;
}
//======================================================================//
// - attachVoidField
//======================================================================//
bool InterpreterConfigurator::attachVoidField(){
  m_rep->voidfield = new GuiVoid(m_rep->fieldgroupline->getElement());
  m_rep->fieldgroupline->attach( m_rep->voidfield );
  return true;
}
//======================================================================//
// - attachColumnStretchField
//======================================================================//
bool InterpreterConfigurator::attachColumnStretchField( int hfac, int vfac ){
  m_rep->fieldgroupline->attach( new GuiStretch(hfac, vfac) );
  return true;
}

//======================================================================//
// - attachIndexField
//======================================================================//
bool InterpreterConfigurator::attachIndexField( const std::string &indexId,
                                                GuiElement::Alignment alignment ){
  m_rep->indexfield = m_rep->guifactory->getGuiIndex( indexId );
  if( m_rep->indexfield == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), indexId) );
  }
  m_rep->indexfield->getElement()->setParent( m_rep->fieldgroupline->getElement() );
  m_rep->indexfield->getElement()->setAlignment( alignment );
  m_rep->fieldgroupline->attach( m_rep->indexfield->getElement() );
  return true;
}
//======================================================================//
// - newDataField
//======================================================================//
bool InterpreterConfigurator::newDataField(){
  m_rep->datafield = m_rep->guifactory->createDataField( m_rep->fieldgroupline->getElement(),
                                                         m_rep->dataitem, m_rep->fieldprec );
  if( m_rep->datafield == 0 ){
    ParserError( compose(_("Cannot assign structure '%1' to gui field."),
                         m_rep->dataitem->getFullName( false )) );
  }
  GuiIndex::registerIndexContainer( m_rep->datafield->getElement() );
  return true;
}
//======================================================================//
// - attachDataField
//======================================================================//
bool InterpreterConfigurator::attachDataField(){
  m_rep->fieldgroupline->attach( m_rep->datafield->getElement() );
  return true;
}
//======================================================================//
// - attachNewPixmap
//======================================================================//
bool InterpreterConfigurator::attachNewPixmap(){
  m_rep->datapixmap = m_rep->guifactory->createPixmap( m_rep->fieldgroupline->getElement(), m_rep->dataitem );
  if( m_rep->datapixmap == 0 ){
    ParserError( compose(_("Cannot assign structure '%1' to gui field."), m_rep->dataitem->getFullName( false )) );
  }
  GuiIndex::registerIndexContainer( m_rep->datapixmap->getElement() );
  m_rep->fieldgroupline->attach( m_rep->datapixmap->getElement() );
  return true;
}
//======================================================================//
// - setPixmapSize
//======================================================================//
bool InterpreterConfigurator::setPixmapSize( int w, int h ){
  m_rep->datapixmap->setSize( w, h );
  return true;
}
//======================================================================//
// - attachFieldgroup
//======================================================================//
bool InterpreterConfigurator::attachFieldgroup( const std::string &elementId,
                                                GuiElement::Alignment alignment ){
  m_rep->element = GuiElement::findElement( elementId );
  if( m_rep->element == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), elementId) );
  }
  if( m_rep->element->Type() != GuiElement::type_Fieldgroup ){
    ParserError( compose(_("'%1' is not a fieldgroup"), elementId) );
  }
  if( m_rep->element->installed() ){
    if( m_rep->element->cloneable() ){
      m_rep->element = m_rep->element->clone();
    }
    else{
      ParserError( compose(_("Fieldgroup '%1' is already installed."), elementId) );
    }
  }
  m_rep->element->setParent( m_rep->fieldgroupline->getElement() );
  if (alignment != GuiElement::align_Default)
    m_rep->element->setAlignment( alignment );
  m_rep->fieldgroupline->attach( m_rep->element );
  return true;
}

//======================================================================//
// - attachThermo
//======================================================================//
bool InterpreterConfigurator::attachThermo( const std::string &elementId,
                                            GuiElement::Alignment alignment ){
  m_rep->element = GuiElement::findElement( elementId );
  if( m_rep->element == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), elementId) );
  }
  if( m_rep->element->Type() != GuiElement::type_Thermo ){
    ParserError( compose(_("'%1' is not a thermo"), elementId) );
  }
  if( m_rep->element->installed() ){
    if( m_rep->element->cloneable() ){
      m_rep->element = m_rep->element->clone();
    }
    else{
      ParserError( compose(_("Thermo '%1' is already installed."), elementId) );
    }
  }
  m_rep->element->setParent( m_rep->fieldgroupline->getElement() );
  m_rep->element->setAlignment( alignment );
  m_rep->fieldgroupline->attach( m_rep->element );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI List Definition                                                          */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newList
//======================================================================//
bool InterpreterConfigurator::newList( const std::string &listId ){
  if ( GuiFactory::Instance()->haveList() ) {
    if( !IdManager::Instance().registerId( listId, IdManager::id_List ) ){
      ParserError( compose(_("Identifier '%1' is already declared."), listId) );
    }
    m_rep->list = GuiFactory::Instance()->createList( 0, listId );
    if( m_rep->list == 0 ){
      ParserError( compose(_("Item '%1' is already declared."), listId) );
    }
    return true;
  }
  ParserError( compose(_("%1: No licence found to use tables."), listId) );
  return false;
}
//======================================================================//
// - attachList
//======================================================================//
bool InterpreterConfigurator::attachList( const std::string &elementId,
                                          GuiElement::Alignment alignment ){
  m_rep->element = GuiElement::findElement( elementId );
  if( m_rep->element == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), elementId) );
  }
  if( m_rep->element->Type() != GuiElement::type_List ){
    ParserError( compose(_("'%1' is not a list"), elementId) );
  }
  if( m_rep->element->installed() ){
    if( m_rep->element->cloneable() ){
      m_rep->element = m_rep->element->clone();
    }
    else{
      ParserError( compose(_("List '%1' is already installed."), elementId) );
    }
  }
  m_rep->element->setParent( m_rep->fieldgroupline->getElement() );
  m_rep->element->setAlignment( alignment );
  m_rep->fieldgroupline->attach( m_rep->element );
  return true;
}
//======================================================================//
// - setListTableSize
//======================================================================//
bool InterpreterConfigurator::setListTableSize( const int tablesize ){
  m_rep->list->setTableSize( tablesize );
  return true;
}
//======================================================================//
// - setListResizeable
//======================================================================//
bool InterpreterConfigurator::setListResizeable(){
  m_rep->list->setResizeable();
  return true;
}
//======================================================================//
// - setListMultipleSelection
//======================================================================//
bool InterpreterConfigurator::setListMultipleSelection(){
  m_rep->list->setMultipleSelection();
  return true;
}
//======================================================================//
// - setListFunction
//======================================================================//
bool InterpreterConfigurator::setListFunction(){
  m_rep->list->setFunction( m_rep->function );
  return true;
}
//======================================================================//
// - setListEnableSort
//======================================================================//

bool InterpreterConfigurator::setListEnableSort( bool ok ){
  m_rep->list->setEnableSort( ok );
  return true;
}
//======================================================================//
// - withIndex
//======================================================================//
bool InterpreterConfigurator::withIndex( const int fieldlen ){
  m_rep->list->withIndex( fieldlen == 0 ? 8 : fieldlen );
  return true;
}
//======================================================================//
// - addListColumn
//======================================================================//
bool InterpreterConfigurator::addListColumn( const std::string &columntext ){
  if( m_rep->dataitem->getDataItemIndexWildcard( 1 ) == 0 )
    ParserError( compose(_("Column '%1': WILDCARD missing."),columntext) );
  m_rep->list->addColumn( columntext, m_rep->dataitem, m_rep->fieldalign, m_rep->fieldlength,
                          m_rep->fieldprec, m_rep->getScale(), m_rep->fieldthousandsep,
                          m_rep->tableitemOptional);
  GuiIndex::registerIndexContainer( m_rep->list->getElement() );
  m_rep->tableitemOptional = false;  // reset
  return true;
}
//======================================================================//
// - addListColumn
//======================================================================//
bool InterpreterConfigurator::addListColumn( XferDataItem *item ){
  if( m_rep->dataitem->getDataItemIndexWildcard( 1 ) == 0 )
    ParserError( _("Column: WILDCARD missing.") );
  m_rep->list->addColumn( item, m_rep->dataitem, m_rep->fieldalign, m_rep->fieldlength,
                          m_rep->fieldprec, m_rep->getScale(), m_rep->fieldthousandsep,
                          m_rep->tableitemOptional );
  GuiIndex::registerIndexContainer( m_rep->list->getElement() );
  m_rep->tableitemOptional = false;  // reset
  return true;
}
//======================================================================//
// - addListColumnOptional
//======================================================================//
bool InterpreterConfigurator::addListColumnOptional(){
  m_rep->tableitemOptional = true;
  return true;
}
//======================================================================//
// - addListColumnToolTip
//======================================================================//
bool InterpreterConfigurator::addListColumnToolTip(){
  if( m_rep->dataitem->getDataItemIndexWildcard( 1 ) == 0 )
    ParserError( _("Column TOOL_TIP: WILDCARD missing.") );
  m_rep->list->addColumnToolTip( m_rep->dataitem );
  GuiIndex::registerIndexContainer( m_rep->list->getElement() );
  return true;
}
//======================================================================//
// - addListColumnColor
//======================================================================//
bool InterpreterConfigurator::addListColumnColor(){
  if( m_rep->dataitem->getDataItemIndexWildcard( 1 ) == 0 )
    ParserError( _("Column COLOR: WILDCARD missing.") );
  m_rep->list->addColumnColor( m_rep->dataitem );
  GuiIndex::registerIndexContainer( m_rep->list->getElement() );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI Index Definition                                                         */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newIndexField
//======================================================================//
bool InterpreterConfigurator::newIndexField( const std::string &indexId ){
  if( !IdManager::Instance().registerId( indexId, IdManager::id_Index ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), indexId) );
  }
  m_rep->indexfield = m_rep->guifactory->createGuiIndex( 0, indexId );
  if( m_rep->indexfield == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), indexId) );
  }
  return true;
}
//======================================================================//
// - setIndexfieldOrientation
//======================================================================//
bool InterpreterConfigurator::setIndexfieldOrientation( GuiElement::Orientation orientation ){
  m_rep->indexfield->setOrientation( orientation );
  return true;
}
//======================================================================//
// - setIndexfieldRange
//======================================================================//
bool InterpreterConfigurator::setIndexfieldRange( const int start, const int end ){
  m_rep->indexfield->setMinIndex( start );
  m_rep->indexfield->setMaxIndex( end );
  return true;
}
//======================================================================//
// - setIndexfieldStep
//======================================================================//
bool InterpreterConfigurator::setIndexfieldStep( const int stepwidth ){
  m_rep->indexfield->setStep( stepwidth );
  return true;
}
//======================================================================//
// - setIndexfieldFunction
//======================================================================//
bool InterpreterConfigurator::setIndexfieldFunction(){
  m_rep->indexfield->setFunction( m_rep->function );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI Form Definition                                                          */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newGuiForm
//======================================================================//
bool InterpreterConfigurator::newGuiForm( const std::string &formId ){
  if( !IdManager::Instance().registerId( formId, IdManager::id_Form ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), formId) );
  }
  m_rep->form = GuiFactory::Instance()->createForm( formId );
  if( m_rep->form == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), formId) );
  }
  m_rep->container = m_rep->form->getElement();
  return true;
}

//======================================================================//
// - formSetFunction
//======================================================================//

bool InterpreterConfigurator::formSetFunction( JobFunction *func ){
  assert( func != 0 );
  if( m_rep->form != 0 )
    m_rep->form->setJobFunction( func );
  return true;
}

//======================================================================//
// - form_hasCloseButton
//======================================================================//

bool InterpreterConfigurator::form_hasCloseButton( bool cb ){
  if( m_rep->form != 0 )
    m_rep->form->hasCloseButton( cb );
  return true;
}

//======================================================================//
// - formUseSettings
//======================================================================//

bool InterpreterConfigurator::formUseSettings( bool us ){
  if( m_rep->form != 0 )
    m_rep->form->useSettings( us );
  return true;
}

//======================================================================//
// - createButtonbar
//======================================================================//
bool InterpreterConfigurator::createButtonbar(){
  assert( m_rep->container->Type() == GuiElement::type_Form ||
          m_rep->container->Type() == GuiElement::type_Main );
  if( m_rep->form->getElement()->Type() == GuiElement::type_Form ){
    m_rep->form->createButtonbar();
  }
  return true;
}
//======================================================================//
// - setMain
//======================================================================//
bool InterpreterConfigurator::setMain(){
  if( GuiElement::getMainForm() != 0 ){
    ParserError( compose(_("Item '%1' is already declared."), "MAIN") );
  }
  m_rep->form->setMain();
  return true;
}
//======================================================================//
// - setApplicationModal
//======================================================================//
bool InterpreterConfigurator::setApplicationModal(){
  m_rep->form->getDialog()->setApplicationModal();
  return true;
}
//======================================================================//
// - formSetMenuInstalled
//======================================================================//
bool InterpreterConfigurator::formSetMenuInstalled(){
  m_rep->form->getListenerController()->setMenuInstalled();
  return true;
}
//======================================================================//
// - formSetScrollbarOn
//======================================================================//
bool InterpreterConfigurator::formSetScrollbarOn(){
  m_rep->form->setOptionScrollbar( GuiElement::scrollbar_AS_NEEDED );
  return true;
}
//======================================================================//
// - formSetScrollbarOff
//======================================================================//
bool InterpreterConfigurator::formSetScrollbarOff(){
  m_rep->form->setOptionScrollbar( GuiElement::scrollbar_OFF );
  return true;
}
//======================================================================//
// - formSetPanedWindowOn
//======================================================================//
bool InterpreterConfigurator::formSetPanedWindowOn(){
  m_rep->form->setOptionPanedWindow( GuiElement::status_ON );
  return true;
}
//======================================================================//
// - formSetPanedWindowOff
//======================================================================//
bool InterpreterConfigurator::formSetPanedWindowOff(){
  m_rep->form->setOptionPanedWindow( GuiElement::status_OFF );
  return true;
}
//======================================================================//
// - formSetUseRuler
//======================================================================//
bool InterpreterConfigurator::formSetUseRuler(){
  m_rep->form->setOptionUseRuler();
  return true;
}
//======================================================================//
// - formSetUseRuler
//======================================================================//
bool InterpreterConfigurator::formSetExpandPolicy(GuiForm::DialogExpandPolicy expand_policy) {
  m_rep->form->setDialogExpandPolicy(expand_policy);
  return true;
}

//======================================================================//
// - resetCycleButton
//======================================================================//
bool InterpreterConfigurator::resetCycleButton(){
  m_rep->form->resetCycleButton();
  return true;
}
//======================================================================//
// - setHelpkey
//======================================================================//
bool InterpreterConfigurator::setHelpkey( const std::string &anchorId ){
  if( (m_rep->helpkey=HelpManager::Instance().getHelpKey( anchorId )) == 0 )
//     fprintf( stderr, "Helpkey %s not available\n", anchorId.c_str() );
    ParserError( compose(_("Helpkey %1 not available"), anchorId) );
  else
    m_rep->form->setHelpkey( m_rep->helpkey );
  return true;
}
//======================================================================//
// - formSetHelptext
//======================================================================//
bool InterpreterConfigurator::formSetHelptext( const std::string &text){
  m_rep->form->getElement()->setHelptext( text );
  return true;
}
//======================================================================//
// - setButtonsPerLine
//======================================================================//
bool InterpreterConfigurator::setButtonsPerLine( const int maxbuttons ){
  m_rep->form->setButtonsPerLine( maxbuttons );
  return true;
}
/* --------------------------------------------------------------------------- */
/* UI Form Container Options                                                   */
/* --------------------------------------------------------------------------- */

//======================================================================//
// - containerSetScrollbarOn
//======================================================================//
bool InterpreterConfigurator::containerSetScrollbarOn( GuiElement::Orientation o,
						       GuiElement::ScrollbarType sb ){
  m_rep->container->setScrollbar( o, sb );
  return true;
}
//======================================================================//
// - containerSetScrollbarOff
//======================================================================//
bool InterpreterConfigurator::containerSetScrollbarOff(){
  m_rep->container->setScrollbar( GuiElement::scrollbar_OFF );
  return true;
}
//======================================================================//
// - containerSetPanedWindowOn
//======================================================================//
bool InterpreterConfigurator::containerSetPanedWindowOn(){
  m_rep->container->setPanedWindow( GuiElement::status_ON );
  return true;
}
//======================================================================//
// - containerSetPanedWindowOff
//======================================================================//
bool InterpreterConfigurator::containerSetPanedWindowOff(){
  m_rep->container->setPanedWindow( GuiElement::status_OFF );
  return true;
}
//======================================================================//
// - containerSetFrameOn
//======================================================================//
bool InterpreterConfigurator::containerSetFrameOn(){
  m_rep->container->setFrame( GuiElement::status_ON );
  return true;
}
//======================================================================//
// - containerSetUseRuler
//======================================================================//
bool InterpreterConfigurator::containerSetUseRuler(){
  m_rep->container->setUseRuler();
  return true;
}
//======================================================================//
// - containerSetTitle
//======================================================================//
bool InterpreterConfigurator::containerSetTitle( const std::string &title ){
  m_rep->container->setTitle( UnitManager::extractValue(title) );
  return true;
}
//======================================================================//
// - containerSetHelptext
//======================================================================//
bool InterpreterConfigurator::containerSetHelptext( const std::string &textstring ){
  m_rep->container->setHelptext( textstring );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI Form Vertical Container Definition                                       */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newContainer
//======================================================================//
bool InterpreterConfigurator::newContainer( GuiElement::Orientation horizontalVertical ){
  m_rep -> pushContainerStack( m_rep->container );
  if( horizontalVertical == GuiElement::orient_Horizontal )
    m_rep->newcontainer = m_rep->guifactory->createHorizontalContainer( m_rep->container )->getElement();
  if( horizontalVertical == GuiElement::orient_Vertical )
    m_rep->newcontainer = m_rep->guifactory->createVerticalContainer( m_rep->container )->getElement();
  m_rep->container->attach( m_rep->newcontainer );
  m_rep->container = m_rep->newcontainer;

  return true;
}
//======================================================================//
// - popContainerStack
//======================================================================//
bool InterpreterConfigurator::popContainerStack(){
  m_rep->container = m_rep -> popContainerStack();
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI Form Container Element Definition                                        */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - cloneElement
//======================================================================//
bool InterpreterConfigurator::cloneElement( const std::string &elementId ){
  m_rep->element = GuiElement::findElement( elementId );
  if( m_rep->element == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), elementId) );
  }
  if( m_rep->element->installed() ){
    if( m_rep->element->cloneable() ){
      m_rep->element = m_rep->element->clone();
    }
    else{
      ParserError( compose(_("Form '%1' is already installed."), elementId) );
    }
  }
  m_rep->element->setParent( m_rep->container );
  m_rep->container->attach( m_rep->element );
  return true;
}
//======================================================================//
// - attachStandardWindow
//======================================================================//
bool InterpreterConfigurator::attachStandardWindow(){
  m_rep->scrolledtext = GuiFactory::Instance()->getStandardWindow();
  assert( m_rep->scrolledtext != 0 );

  if( m_rep->scrolledtext->getElement()->installed() ){
    ParserError( compose(_("Form '%1' is already installed."), _("Standard Window")) );
  }
  m_rep->scrolledtext->getElement()->setParent( m_rep->container );
  m_rep->container->attach( m_rep->scrolledtext->getElement() );
  return true;
}
//======================================================================//
// - attachLogWindow
//======================================================================//
bool InterpreterConfigurator::attachLogWindow(){
  m_rep->scrolledtext = GuiFactory::Instance()->getLogWindow();
  assert( m_rep->scrolledtext != 0 );

  if( m_rep->scrolledtext->getElement()->installed() ){
    ParserError( compose(_("Form '%1' is already installed."), _("Log Window")) );
  }
  m_rep->scrolledtext->getElement()->setParent( m_rep->container );
  m_rep->container->attach( m_rep->scrolledtext->getElement() );
  return true;
}
//======================================================================//
// - configStandardWindow
//======================================================================//
bool InterpreterConfigurator::configStandardWindow( bool show ){
  GuiElement *e = GuiFactory::Instance()->getStandardWindow()->getElement();
  assert( e != 0 );
  HardCopyListener *hcl = e->getHardCopyListener();
  assert( hcl != 0 );
  hcl->showMenu( show );
  return true;
}
//======================================================================//
// - configLogWindow
//======================================================================//
bool InterpreterConfigurator::configLogWindow( bool show ){
  GuiElement *e = GuiFactory::Instance()->getLogWindow()->getElement();
  assert( e != 0 );
  HardCopyListener *hcl = e->getHardCopyListener();
  assert( hcl != 0 );
  hcl->showMenu( show );
  return true;
}

//======================================================================//
// - newSeparator
//======================================================================//
bool InterpreterConfigurator::newSeparator( bool bcontainer,
                                            GuiElement::Alignment alignment ){
  GuiElement* container = 0;
  container = (bcontainer) ? m_rep->container : m_rep->fieldgroupline->getElement();
  assert(container != 0);
  GuiSeparator *sep = m_rep->guifactory->createSeparator( container );
  sep->getElement()->setAlignment( bcontainer ? container->getAlignment() : alignment );
  container->attach( sep->getElement() );
  return true;
}
//======================================================================//
// - newVoid
//======================================================================//
bool InterpreterConfigurator::newVoid( int sz ){
  GuiVoid *v = new GuiVoid(m_rep->fieldgroupline->getElement());
  GuiElement::Orientation orientation = GuiElement::orient_Vertical;

  if (dynamic_cast<GuiOrientationContainer*>(m_rep->container)) {
    orientation = dynamic_cast<GuiOrientationContainer*>(m_rep->container)->getOrientation();
  }

  if (orientation == GuiElement::orient_Vertical)
    v->setHeight( sz );
  else
    v->setWidth( sz );
  m_rep->container->attach( v );
  return true;
}
//======================================================================//
// - newStretch
//======================================================================//
bool InterpreterConfigurator::newStretch( int sz ){
  GuiStretch *v = new GuiStretch( sz );
  m_rep->container->attach( v );
  return true;
}
/* --------------------------------------------------------------------------- */
/* UI LISTPLOT Definition                                                      */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newListPlot
//======================================================================//
bool InterpreterConfigurator::newListPlot( const std::string &listplotId ){
  if( !IdManager::Instance().registerId( listplotId, IdManager::id_Listplot ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), listplotId) );
  }
  m_rep->listplot = GuiFactory::Instance()->createListPlot( 0, listplotId );
  if( m_rep->listplot == 0 ) {
    ParserError( compose(_("Item '%1' is already declared."), listplotId) );
  }
  return true;
}
//======================================================================//
// - setCaptionStream
//======================================================================//
bool InterpreterConfigurator::setCaptionStream( const std::string &streamId ){
  m_rep->listplot->setCaptionStream( streamId );
  return true;
}
//======================================================================//
// - addHGraph
//======================================================================//
bool InterpreterConfigurator::addHGraph(){
  m_rep->listplot->addHGraph();
  return true;
}
//======================================================================//
// - addGraph
//======================================================================//
bool InterpreterConfigurator::addGraph( const std::string &listplotitemId ){
  m_rep->listgraph = m_rep->listplot->addGraph( listplotitemId );
  return true;
}
//======================================================================//
// - checkGraph
//======================================================================//
bool InterpreterConfigurator::checkGraph(){
  if( !m_rep->listgraph->checkGraph() )
     ParserError( compose(_("Graph '%1': LOG_X without XAXIS"), m_rep->listgraph->getName()) );
  return true;
}
//======================================================================//
// - setXaxis
//======================================================================//
bool InterpreterConfigurator::setXaxis(){
  if( (m_rep->plotaxis = new GuiPlotDataItem( m_rep->dataitem, 1 )) == 0 )
    ParserError( _("Undeclared identifier.") );
  m_rep->plotaxis->setScale( m_rep->getScale() );
  GuiIndex::registerIndexContainer( m_rep->listplot->getElement() );
  m_rep->listgraph->setXaxis( m_rep->plotaxis );
  return true;
}
//======================================================================//
// - listplot_setSize
//======================================================================//
bool InterpreterConfigurator::listplot_setSize( int width, int height){
  m_rep->listplot->setSize( width, height );
  return true;
}
//======================================================================//
// - listgraphSetAxisOriginY
//======================================================================//
bool InterpreterConfigurator::listgraphSetAxisOriginY( const double originyaxis ){
  m_rep->listgraph->setAxesOriginY( originyaxis );
  return true;
}
//======================================================================//
// - listgraphSetLogX
//======================================================================//
bool InterpreterConfigurator::listgraphSetLogX(){
  m_rep->listgraph->setLogX();
  return true;
}
//======================================================================//
// - listgraphSetLogY
//======================================================================//
bool InterpreterConfigurator::listgraphSetLogY(){
  m_rep->listgraph->setLogY();
  return true;
}
//======================================================================//
// - setSameYRange
//======================================================================//
bool InterpreterConfigurator::setSameYRange(){
  m_rep->listgraph->setSameYRange();
  return true;
}
//======================================================================//
// - addGraphItem
//======================================================================//
bool InterpreterConfigurator::addGraphItem(){
  if( (m_rep->plotaxis = new GuiPlotDataItem( m_rep->dataitem, 1 )) == 0 )
    ParserError( _("Undeclared identifier.") );
  m_rep->plotaxis->setScale( m_rep->getScale() );
  GuiIndex::registerIndexContainer( m_rep->listplot->getElement() );
  m_rep->listgraph->addGraphItem( m_rep->plotaxis );
  return true;
}
//======================================================================//
// - setUnit
//======================================================================//
bool InterpreterConfigurator::setUnit( const std::string &unitsstring ){
  m_rep->plotaxis->setUnit( UnitManager::extractValue(unitsstring) );
  return true;
}
//======================================================================//
// - plotaxisSetLabel
//======================================================================//
bool InterpreterConfigurator::plotaxisSetLabel( const std::string &label ){
  m_rep->plotaxis->setLabel( UnitManager::extractValue(label) );
  return true;
}
//======================================================================//
// - plotaxisSetLabel
//======================================================================//

bool InterpreterConfigurator::plotaxisSetLabel( XferDataItem * dataitem ){
  assert( m_rep->plotaxis != 0 );
  int num = dataitem->getNumOfWildcards();
  if( dataitem->getNumOfWildcards() >=  m_rep->plotaxis->XferData()->getNumOfWildcards()){
    ParserError( _("Number of WILDCARDS of plot axis label does not make sense.") );
  }
  m_rep->plotaxis->setLabel( dataitem );
  return true;
}

//======================================================================//
// - plotAxisSetAnnoTitle
//======================================================================//
bool InterpreterConfigurator::plotAxisSetAnnoTitle( const std::string &title ){
  m_rep->plotaxis->setAnnotationTitle( title );
  return true;
}
//======================================================================//
// - plotAxisSetAnnoAngle
//======================================================================//
bool InterpreterConfigurator::plotAxisSetAnnoAngle( double angle ){
  m_rep->plotaxis->setAnnotationAngle( angle );
  return true;
}
//======================================================================//
// - plotaxisSetLabelStream
//======================================================================//
bool InterpreterConfigurator::plotaxisSetLabelStream( const std::string &streamId ){
  m_rep->plotaxis->setLabelStream( streamId );
  return true;
}
//======================================================================//
// - plotaxisSetUnitStream
//======================================================================//
bool InterpreterConfigurator::plotaxisSetUnitStream( const std::string &streamId ){
  m_rep->plotaxis->setUnitStream( streamId );
  return true;
}
//======================================================================//
// - plotaxisSetLineStyle
//======================================================================//
bool InterpreterConfigurator::plotaxisSetLineStyle( GuiPlotDataItem::eLineStyle linestyle ){
  m_rep->plotaxis->setLineStyle( linestyle );
  return true;
}

//======================================================================//
// - plot3dAxis_setScale
//======================================================================//
bool InterpreterConfigurator::plot3dAxis_setScale( double min, double max ) {
  m_rep->plotaxis->setMinRange(min);
  m_rep->plotaxis->setMaxRange(max);
  return true;
}

//======================================================================//
// - plot3dAxis_setScaleDataRef
//======================================================================//
bool InterpreterConfigurator::plot3dAxis_setScaleDataRef( XferDataItem *min_xfer,
                                                          XferDataItem *max_xfer ){
  m_rep->plotaxis->setMaxXferRange( max_xfer );
  m_rep->plotaxis->setMinXferRange( min_xfer  );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI UNIPLOT Definition                                                       */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newSimpelPlot
//======================================================================//
bool InterpreterConfigurator::newSimpelPlot( const std::string &uniplotId ){
  if( !IdManager::Instance().registerId( uniplotId, IdManager::id_Uniplot ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), uniplotId) );
  }
  m_rep->simpel = m_rep->guifactory->newSimpelPlot( 0, uniplotId );
  if( m_rep->simpel == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), uniplotId) );
  }
  UImanager::Instance().addHardCopy( uniplotId, m_rep->simpel->getListPlot() );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI PSPLOT Definition                                                        */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newPSStream
//======================================================================//
bool InterpreterConfigurator::newPSStream( const std::string &streamId ){
  if( !IdManager::Instance().registerId( streamId, IdManager::id_Psplot ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), streamId) );
  }
  m_rep->psstream = StreamManager::Instance().newPSStream( streamId );
  if( m_rep->psstream == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), streamId) );
  }
  m_rep->psplot = GuiFactory::Instance()->createPSPlot( m_rep->psstream );
  if( m_rep->psplot == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), streamId) );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI XRT3DPLOT Definition                                                     */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newGui3dPlot
//======================================================================//
bool InterpreterConfigurator::newGui3dPlot( const std::string &plotId ){
  if( !GuiFactory::Instance()->have3dPlot() )
    ParserError( compose(_("%1: No licence found to use 3d-Plot."), plotId) );
  if( !IdManager::Instance().registerId( plotId, IdManager::id_3dPlot ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), plotId) );
  }
  m_rep->plot3d = GuiFactory::Instance()->create3dPlot( 0, plotId );
  if( m_rep->plot3d == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), plotId) );
  }
  UImanager::Instance().addHardCopy( plotId, m_rep->plot3d );
  return true;
}

//======================================================================//
// - newImage
//======================================================================//
bool InterpreterConfigurator::newImage( const std::string &imgId ){
  if( !IdManager::Instance().registerId( imgId, IdManager::id_Image ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), imgId) );
  }
  m_rep->image = GuiFactory::Instance() -> createImage( 0, imgId, 2 );
  if( m_rep->image == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), imgId) );
  }
  return true;
}

//======================================================================//
// - newLinePlot
//======================================================================//
bool InterpreterConfigurator::newLinePlot( const std::string &imgId ){
  if( !IdManager::Instance().registerId( imgId, IdManager::id_LinePlot ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), imgId) );
  }
  m_rep->image = GuiFactory::Instance() -> createLinePlot( 0, imgId );
  if( m_rep->image == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), imgId) );
  }
  return true;
}

//======================================================================//
// - imageSetSize
//======================================================================//
bool InterpreterConfigurator::imageSetSize( const int width, const int height ){
  if ( m_rep->image != 0 ) {
    m_rep->image->setSize( width, height );
  }
  return true;
}

//======================================================================//
// - imageSetSocket
//======================================================================//

bool  InterpreterConfigurator::imageSetSocket( const std::string &socket_id ){
  assert( m_rep->image != 0 );
  IntensServerSocket *socket = IntensServerSocket::getServer( socket_id );
  if( socket==0 ){
    ParserError( _("Undeclared identifier.") );
  }
  MFMImageMapper::addImage( m_rep->image, static_cast<MFMServerSocket*>(socket) );
  return true;
}

//======================================================================//
// - imageSetImageSettings
//======================================================================//

bool InterpreterConfigurator::imageSetImageSettings( XferDataItem * settings ){
  assert( m_rep->image != 0 );
  m_rep->image->setSettings( settings );
  return true;
}

//======================================================================//
// - newThermo
//======================================================================//
bool InterpreterConfigurator::newThermo( const std::string &id ){
  if( !IdManager::Instance().registerId( id, IdManager::id_Thermo ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), id) );
  }
  m_rep->thermo = GuiFactory::Instance() -> createThermo( 0, id );
  if( m_rep->thermo == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), id) );
  }
  return true;
}

//======================================================================//
// - thermo_setRange
//======================================================================//
bool InterpreterConfigurator::thermo_setRange( const double min, const double max){
  if( min == 0 && max == 0 ){
    XferDataItem *max_xfer = m_rep->popDataItem();
    Scale *max_scale = m_rep->getScale();
    m_rep->thermo->setRange(  m_rep->popDataItem(), m_rep->getScale(), max_xfer, max_scale );
  }
  else{
    m_rep->thermo->setRange( min, max );
  }
  return true;
}

//======================================================================//
// - thermo_setOffset
//======================================================================//
bool InterpreterConfigurator::thermo_setOffset( const double offset ){
  if( offset == 0){
    m_rep->thermo->setOffset(  m_rep->popDataItem(), m_rep->getScale() );
  }
  else{
    m_rep->thermo->setOffset( offset );
  }
  return true;
}

//======================================================================//
// - thermo_hideLabel
//======================================================================//
bool InterpreterConfigurator::thermo_hideLabel(){
  m_rep->thermo->hideLabel();
  return true;
}

//======================================================================//
// - thermo_hideUnits
//======================================================================//
bool InterpreterConfigurator::thermo_hideUnits(){
  m_rep->thermo->hideUnits();
  return true;
}

//======================================================================//
// - thermo_setLabel
//======================================================================//
bool InterpreterConfigurator::thermo_setLabel( const std::string &label ){
  m_rep->thermo->setLabel( label );
  return true;
}

//======================================================================//
// - thermo_setUnits
//======================================================================//
bool InterpreterConfigurator::thermo_setUnits( const std::string &label ){
  m_rep->thermo->setUnits( label );
  return true;
}

//======================================================================//
// - thermo_setLabel
//======================================================================//
bool InterpreterConfigurator::thermo_setLabel( XferDataItem *item ){
  m_rep->thermo->setLabel( item );
  return true;
}
//======================================================================//
// - thermo_setUnits
//======================================================================//
bool InterpreterConfigurator::thermo_setUnits( XferDataItem *item ){
  m_rep->thermo->setUnits( item );
  return true;
}

//======================================================================//
// - thermo_setScaleFormat
//======================================================================//
bool InterpreterConfigurator::thermo_setScaleFormat( const std::string &format ){
  m_rep->thermo->setScaleFormat( format );
  return true;
}

//======================================================================//
// - thermo_setColorSetName
//======================================================================//
bool InterpreterConfigurator::thermo_setColorSetName( const std::string &name ){
  m_rep->thermo -> setColorsetName( name );
  return true;
}

//======================================================================//
// - thermo_setColorScaleOption
//======================================================================//
bool InterpreterConfigurator::thermo_setColorScaleOption(){
  m_rep->thermo -> setColorScaleOption( true );
  return true;
}

//======================================================================//
// - thermo_setColorScaleOptionScaled
//======================================================================//
bool InterpreterConfigurator::thermo_setColorScaleOptionScaled(){
  m_rep->thermo -> setColorScaleOptionScaled( true );
  return true;
}

//======================================================================//
// - thermo_setSize
//======================================================================//
bool InterpreterConfigurator::thermo_setSize( int width, int height){
  m_rep->thermo->setSize( width, height );
  return true;
}

//======================================================================//
// - thermo_setOrientation
//======================================================================//
bool InterpreterConfigurator::thermo_setOrientation( GuiElement::Orientation o ){
  m_rep->thermo->setOrientation( o );
  return true;
}

//======================================================================//
// - thermo_setXfer
//======================================================================//
bool InterpreterConfigurator::thermo_setXfer(){
  m_rep->thermo->setXfer( m_rep->dataitem, m_rep->getScale() );
  return true;
}

//======================================================================//
// - thermo_setAlarmLevel
//======================================================================//
bool InterpreterConfigurator::thermo_setAlarmLevel( const double dbl ) {
  if( dbl == 0 ){
    m_rep->thermo->setAlarmLevel( m_rep->popDataItem(), m_rep->getScale() );
  }
  else{
    m_rep->thermo->setAlarmLevel( dbl );
  }
  return true;
}

//======================================================================//
// - thermo_setAlarmColorName
//======================================================================//
bool InterpreterConfigurator::thermo_setAlarmColorName( const std::string &name ) {
  m_rep->thermo -> setAlarmColorName( name );
  return true;
}

//======================================================================//
// - thermo_setInverted
//======================================================================//
bool InterpreterConfigurator::thermo_setInverted(bool inv) {
  m_rep->thermo->setInverted(inv);
  return true;
}

//======================================================================//
// - newTimeTable
//======================================================================//
bool InterpreterConfigurator::newTimeTable( const std::string &id ){
  if( !IdManager::Instance().registerId( id, IdManager::id_TimeTable ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), id) );
  }
  m_rep->timeTable = GuiFactory::Instance() -> createTimeTable( 0, id );
  if( m_rep->timeTable == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), id) );
  }
  return true;
}

//======================================================================//
// - timeTableSetSize
//======================================================================//
bool InterpreterConfigurator::timeTableSetSize( const int width, const int height ){
  if ( m_rep->timeTable != 0 ) {
    m_rep->timeTable->setSize( width, height );
  }
  return true;
}

//======================================================================//
// - timeTableStringDateTime
//======================================================================//
bool InterpreterConfigurator::timeTableSetFromStringDateTime(){
  if (!m_rep->timeTable->setFromStringDateTime( m_rep->dataitem ))
    ParserError( compose(_("Wrong type of dataitem '%1' => Please use a STRING_DATETIME dataitem.")
                         , m_rep->dataitem->getFullName( false )) );
  return true;
}

//======================================================================//
// - timeTableStringDateTime
//======================================================================//
bool InterpreterConfigurator::timeTableSetToStringDateTime(){
  if (!m_rep->timeTable->setToStringDateTime( m_rep->dataitem ))
    ParserError( compose(_("Wrong type of dataitem '%1' => Please use a STRING_DATETIME dataitem.")
                         , m_rep->dataitem->getFullName( false )) );
  return true;
}

//======================================================================//
// - timeTableStringDateTime
//======================================================================//
bool InterpreterConfigurator::timeTableSetToStringTime(){
  if (!m_rep->timeTable->setToStringTime( m_rep->dataitem ))
    ParserError( compose(_("Wrong type of dataitem '%1' => Please use a STRING_TIME dataitem.")
                         , m_rep->dataitem->getFullName( false )) );
  return true;
}

//======================================================================//
// - timeTableString
//======================================================================//
bool InterpreterConfigurator::timeTableSetString(){
  if (!m_rep->timeTable->setAppointment( m_rep->dataitem ))
    ParserError( compose(_("Wrong type of dataitem '%1' => Please use a STRING dataitem.")
                         , m_rep->dataitem->getFullName( false )) );

  return true;
}

//======================================================================//
// - plot3dSetMenuText
//======================================================================//
bool InterpreterConfigurator::plot3dSetMenuText( const std::string &menulabel ){
  m_rep->plot3d->setMenuText( UnitManager::extractValue(menulabel) );
  return true;
}
//======================================================================//
// - plot3dSetMenuInstalled
//======================================================================//
bool InterpreterConfigurator::plot3dSetMenuInstalled( ){
  assert( m_rep->plot3d != 0 );
  m_rep->plot3d->setMenuInstalled();
  return true;
}
//======================================================================//
// - plot3dSetFooterStream
//======================================================================//
bool InterpreterConfigurator::plot3dSetFooterStream( const std::string &streamId ){
  if( !m_rep->plot3d->setFooterStream( streamId ) )
    ParserError( compose(_("Undeclared format '%1'."), streamId) );
  return true;
}
//======================================================================//
// - plot3dSetFooterText
//======================================================================//
bool InterpreterConfigurator::plot3dSetFooterText( const std::string &stringConstant ){
  if( !m_rep->plot3d->setFooterText( UnitManager::extractValue(stringConstant) ) )
    ParserError( compose(_("Undeclared format '%1'."), stringConstant) );
  return true;
}
//======================================================================//
// - plot3dSetSize
//======================================================================//
bool InterpreterConfigurator::plot3dSetSize(int width, int height) {
  m_rep->plot3d->setSize(width , height);
  return true;
}
//======================================================================//
// - setPlotStyleBar
//======================================================================//
bool InterpreterConfigurator::setPlotStyleBar(){
  m_rep->plot3d->setPlotStyleBar();
  return true;
}
//======================================================================//
// - setPlotStyleSurface
//======================================================================//
bool InterpreterConfigurator::setPlotStyleSurface(){
  m_rep->plot3d->setPlotStyleSurface();
  return true;
}
//======================================================================//
// - setPlotStyleContour
//======================================================================//
bool InterpreterConfigurator::setPlotStyleContour(){
  m_rep->plot3d->setPlotStyleContour();
  return true;
}
//======================================================================//
// - plot3dSetHeaderText
//======================================================================//
bool InterpreterConfigurator::plot3dSetHeaderText( const std::string &headertext ){
  if (!m_rep->plot3d->setHeaderText( UnitManager::extractValue(headertext) ))
    ParserError( compose(_("Undeclared identifier '%1'."), headertext) );
  return true;
}
//======================================================================//
// - plot3dSetHeaderStream
//======================================================================//
bool InterpreterConfigurator::plot3dSetHeaderStream( const std::string &streamId ){
  if (!m_rep->plot3d->setHeaderStream( streamId ))
    ParserError( compose(_("Undeclared identifier '%1'."), streamId) );
  return true;
}
//======================================================================//
// - putDataItemXaxis
//======================================================================//
bool InterpreterConfigurator::putDataItemXaxis(){
  m_rep->plot3d->putDataItem( "XAXIS", m_rep->plotaxis );
  return true;
}
//======================================================================//
// - putDataItemYaxis
//======================================================================//
bool InterpreterConfigurator::putDataItemYaxis(){
  m_rep->plot3d->putDataItem( "YAXIS", m_rep->plotaxis );
  return true;
}
//======================================================================//
// - newPlotDataItem
//======================================================================//
bool InterpreterConfigurator::newPlotDataItem(){
  if( (m_rep->plotaxis = new GuiPlotDataItem()) == 0 )
    ParserError( _("Undeclared identifier.") );
  return true;
}
//======================================================================//
// - plot3dRegisterIndexContainer
//======================================================================//
bool InterpreterConfigurator::plot3dRegisterIndexContainer(){
  if( (m_rep->plotaxis = new GuiPlotDataItem( m_rep->dataitem, 1 )) == 0 )
    ParserError( _("Undeclared identifier.") );
  m_rep->plotaxis->setScale( m_rep->getScale() );
  GuiIndex::registerIndexContainer( m_rep->plot3d->getElement() );
  return true;
}
//======================================================================//
// - setPlot3dMarker
//======================================================================//
bool InterpreterConfigurator::setPlot3dMarker(){
  if( m_rep->dataitem->getNumOfWildcards() > 1 ){
    ParserError( _("Too many WILDCARDS.") );
  }
  m_rep->plotaxis->setMarkerLineLabels(m_rep->dataitem);
  return true;
}
//======================================================================//
// - setPlot3dMarkerXAxis
//======================================================================//
bool InterpreterConfigurator::setPlot3dMarkerXAxis(){
  if( m_rep->dataitem->getNumOfWildcards() > 1 ){
    ParserError( _("Too many WILDCARDS.") );
  }
  m_rep->plotaxis->setMarkerLineXAxis(m_rep->dataitem, m_rep->getScale());
  return true;
}
//======================================================================//
// - putDataItemZaxis
//======================================================================//
bool InterpreterConfigurator::putDataItemZaxis(){
  if( (m_rep->plotaxis = new GuiPlotDataItem( m_rep->dataitem, 2 )) == 0 )
    ParserError( _("Undeclared identifier.") );
  m_rep->plotaxis->setScale( m_rep->getScale() );

  // maybe add more zAxis
  int i = 0;
  if (!m_rep->plot3d->putDataItem( "ZAXIS", m_rep->plotaxis ))
    while (++i < 100) {
      if (m_rep->plot3d->putDataItem( compose("ZAXIS%1", i),
                                      m_rep->plotaxis ))
        break;
  }
  GuiIndex::registerIndexContainer( m_rep->plot3d->getElement() );
  return true;
}
//======================================================================//
// - showPlot3dAnnotationLabels
//======================================================================//
bool InterpreterConfigurator::showPlot3dAnnotationLabels(bool xAxis){
  m_rep->plot3d->showAnnotationLabels( xAxis, true );
  if (!m_rep->plot3d->hasAnnotationOption(xAxis))
    ParserError("no annotation defined for this activation");

  return true;
}

/* --------------------------------------------------------------------------- */
/* UI XRTGRAPH Definition                                                      */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newXrtGraph
//======================================================================//
bool InterpreterConfigurator::newXrtGraph( const std::string &plotId ){
    BUG_WARN(  "XRTGRAPH is deprecated : Use PLOT2D instead " << plotId );
  if( !GuiFactory::Instance()->have2dPlot() )
    ParserError( compose(_("'%1': No licence found to use PLOT2D."),plotId) );
  if( !IdManager::Instance().registerId( plotId, IdManager::id_2dPlot ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), plotId) );
  }
  try {
    m_rep->plot2d = GuiFactory::Instance()->create2dPlot( plotId );
    if( m_rep->plot2d == 0 ){
      ParserError( compose(_("Item '%1' is already declared."), plotId) );
    }
    UImanager::Instance().addHardCopy( plotId, m_rep->plot2d );
    return true;
  }
  catch( const char * msg ){
    ParserError( compose(_("Item '%1' is already declared."), plotId) );
  }
  return false;
}
//======================================================================//
// - set visible characteristic
//======================================================================//
bool InterpreterConfigurator::xrtgraphSetMINMAX( bool show, int precision ){
  m_rep->plot2d->showMINMAX( show, precision );
  return true;
}

bool InterpreterConfigurator::xrtgraphSetAVG( bool show, int precision ){
  m_rep->plot2d->showAVG( show, precision );
  return true;
}

bool InterpreterConfigurator::xrtgraphSetRMS( bool show, int precision ){
  m_rep->plot2d->showRMS( show, precision );
  return true;
}
//======================================================================//
// - xrtgraphSetMenuText
//======================================================================//
bool InterpreterConfigurator::xrtgraphSetMenuText( const std::string &menulabel ){
  assert( m_rep->plot2d != 0 );
  m_rep->plot2d->setMenuText( menulabel );
  return true;
}
//======================================================================//
// - xrtgraphSetMenuInstalled
//======================================================================//
bool InterpreterConfigurator::xrtgraphSetMenuInstalled(  ){
  assert( m_rep->plot2d != 0 );
  m_rep->plot2d->setMenuInstalled();
  return true;
}
//======================================================================//
// - xrtgraphSetFooterStream
//======================================================================//
bool InterpreterConfigurator::xrtgraphSetFooterStream( const std::string &streamId ){
  if( !m_rep->plot2d->setFooterStream( streamId ) ){
    ParserError( compose(_("Undeclared format '%1'."), streamId) );
  }
  return true;
}
//======================================================================//
// - xrtgraphSetFooterText
//======================================================================//
bool InterpreterConfigurator::xrtgraphSetFooterText( const std::string &stringconstant ){
  if( !m_rep->plot2d->setFooterText( UnitManager::extractValue(stringconstant) ) ){
    ParserError( compose(_("Undeclared format '%1'."), stringconstant) );
  }
  return true;
}
//======================================================================//
// - xrtgraphSetHeaderText
//======================================================================//
bool InterpreterConfigurator::xrtgraphSetHeaderText( const std::string &headertext ){
  if (!m_rep->plot2d->setHeaderText( UnitManager::extractValue(headertext) ))
    ParserError( compose(_("Undeclared identifier '%1'."), headertext) );
  return true;
}
//======================================================================//
// - xrtgraphSetHeaderStream
//======================================================================//
bool InterpreterConfigurator::xrtgraphSetHeaderStream( const std::string &streamId ){
  if (!m_rep->plot2d->setHeaderStream( streamId ))
    ParserError( compose(_("Undeclared identifier '%1'."), streamId) );
  return true;
}
//======================================================================//
// - xrtgraphSetPrintStyle
//======================================================================//
bool InterpreterConfigurator::xrtgraphSetPrintStyle( bool state ){
  m_rep->plot2d->setPrintStyle( state );
  return true;
}

//======================================================================//
// - addDataItem
//======================================================================//
bool InterpreterConfigurator::addDataItem(){
if( ( m_rep->plotitem = m_rep->plot2d->addDataItem( m_rep->plotaxis, 0 ) ) == 0 )
   ParserError( _("Undeclared identifier.") );
 m_rep->plotitem->setAxisType( Gui2dPlot::XAXIS );
 return true;
}
//======================================================================//
// - setAllCycles
//======================================================================//
bool InterpreterConfigurator::setAllCycles(){
  m_rep->plot2d->setAllCycles( true );
  return true;
}
//======================================================================//
// - setOriginXAxis
//======================================================================//
bool InterpreterConfigurator::setOriginXAxis( const double originxaxis ){
  m_rep->plot2d->setOriginXAxis( originxaxis );
  return true;
}
//======================================================================//
// - setOriginYAxis
//======================================================================//
bool InterpreterConfigurator::setOriginYAxis( const double originyaxis ){
  m_rep->plot2d->setOriginYAxis( originyaxis );
  return true;
}
//======================================================================//
// - setLogX
//======================================================================//
bool InterpreterConfigurator::setLogX(){
  m_rep->plot2d->setLogX();
  return true;
}
//======================================================================//
// - setLogY
//======================================================================//
bool InterpreterConfigurator::setLogY(){
  m_rep->plot2d->setLogY();
  return true;
}
//======================================================================//
// - setConfigDialogStyle
//======================================================================//
bool InterpreterConfigurator::setConfigDialogStyle( int length ){
  m_rep->plot2d->setConfigDialogStyle( ConfigDialog::COMBOBOX, length );
  return true;
}
//======================================================================//
// - setWithScrollBar
//======================================================================//
bool InterpreterConfigurator::setWithScrollBar(){
  m_rep->plot2d->setWithScrollBar( true );
  return true;
}
//======================================================================//
// - showAnnotationLabels
//======================================================================//
bool InterpreterConfigurator::showAnnotationLabels(){
  m_rep->plot2d->showAnnotationLabels( true );
  return true;
}
//======================================================================//
// - setYPlotStyle
//======================================================================//
bool InterpreterConfigurator::setYPlotStyle( Gui2dPlot::eStyle style, Gui2dPlot::eAxisType axistype ){
  m_rep->plot2d->setYPlotStyle( style, axistype );
  return true;
}

bool InterpreterConfigurator::setYPlotStyle( Gui2dPlot::eStyle style ){
  m_rep->plot2d->setYPlotStyle( style, Gui2dPlot::Y1AXIS );
  m_rep->plot2d->setYPlotStyle( style, Gui2dPlot::Y2AXIS );
  return true;
}

//======================================================================//
// - setPlot2dItemIsMarker
//======================================================================//
bool InterpreterConfigurator::setPlot2dItemIsMarker(){
  m_rep->plotitem->setIsMarker();
  return true;
}

//======================================================================//
// - setPlot2dItemColorSetName
//======================================================================//
bool InterpreterConfigurator::setPlot2dItemColorSetName(const std::string& name){
  m_rep->plotitem->setColorsetName(name);
  return true;
}

//======================================================================//
// - setPlot2dItemWildcardIndex
//======================================================================//
bool InterpreterConfigurator::setPlot2dItemWildcardIndex( int wildcardIdx ){
  if(  m_rep->plotaxis->XferData()->getNumOfWildcards() < 2 ||
       m_rep->plotaxis->XferData()->getNumOfWildcards() < wildcardIdx ){
    ParserError( _("Too few WILDCARDS.") );
  }
  m_rep->plotitem->setAxisWildcardIndex( wildcardIdx );
  return true;
}

//======================================================================//
// - setPlot2dItemLegendHide
//======================================================================//
bool InterpreterConfigurator::setPlot2dItemLegendHide(bool hide) {
  m_rep->plotitem->setLegendHide(hide);
  return true;
}

//======================================================================//
// - setPlot2dAxisHide
//======================================================================//
bool InterpreterConfigurator::setPlot2dAxisHide(Gui2dPlot::eAxisType axistype) {
  m_rep->plot2d->setAxisHide(axistype);
  return true;
}

//======================================================================//
// - setPlot2dFunction
//======================================================================//
bool InterpreterConfigurator::setPlot2dFunction() {
  m_rep->plot2d->setFunction( m_rep->function );
  return true;
}

//======================================================================//
// - setPlot2dColorSetName
//======================================================================//
bool InterpreterConfigurator::setPlot2dColorSetName( const std::string &name ) {
  m_rep->plot2d->setColorsetName( name );
  return true;
}

//======================================================================//
// - setPlot2dMarker
//======================================================================//
bool InterpreterConfigurator::setPlot2dMarker(){
  if( m_rep->dataitem->getNumOfWildcards() > 1 ){
    ParserError( _("Too many WILDCARDS.") );
  }
  m_rep->plotitem->setMarker( m_rep->dataitem );
  return true;
}

//======================================================================//
// - xrtgraphRegisterIndexContainer
//======================================================================//
bool InterpreterConfigurator::xrtgraphRegisterIndexContainer(){
  if( (m_rep->plotaxis = new GuiPlotDataItem( m_rep->dataitem, 1 )) == 0 )
    ParserError( _("Undeclared identifier.") );
  m_rep->plotaxis->setScale( m_rep->getScale() );
  GuiIndex::registerIndexContainer( m_rep->plot2d->getElement() );
  return true;
}
//======================================================================//
// - xrtgraphSetLabel
//======================================================================//
bool InterpreterConfigurator::xrtgraphSetLabel( const std::string &labelstring, Gui2dPlot::eAxisType axistype ){
  m_rep->plot2d->setLabel( UnitManager::extractValue(labelstring), axistype );
  return true;
}
//======================================================================//
// - xrtgraphSetLabelStream
//======================================================================//
bool InterpreterConfigurator::xrtgraphSetLabelStream( const std::string &streamId, Gui2dPlot::eAxisType axistype ){
  m_rep->plot2d->setLabelStream( streamId, axistype );
  return true;
}
//======================================================================//
// - xrtgraphSetLabelDataRef
//======================================================================//
bool InterpreterConfigurator::xrtgraphSetLabelDataRef( Gui2dPlot::eAxisType axistype ){
  std::string label( static_cast<UserAttr*>(m_rep->dataref->getUserAttr())->Label( m_rep->dataref ) );
  m_rep->plot2d->setLabel( label, axistype );
  return true;
}
//======================================================================//
// - xrtgraphNewPlotDataItem
//======================================================================//
bool InterpreterConfigurator::xrtgraphNewPlotDataItem(){
  if( (m_rep->plotaxis = new GuiPlotDataItem( m_rep->dataitem, 1 )) == 0 )
    ParserError( _("Undeclared identifier.") );
  if( m_rep->dataitem->getNumOfWildcards() != 1 )
    ParserError( _("Number of WILDCARDS is not 1.") );
  if( ( m_rep->plotitem = m_rep->plot2d->addDataItem( m_rep->plotaxis, 0 ) ) == 0 )
    ParserError( _("Undeclared identifier.") );
  m_rep->plotaxis->setScale( m_rep->getScale() );
  GuiIndex::registerIndexContainer( m_rep->plot2d->getElement() );
  return true;
}
//======================================================================//
// - setAxisType
//======================================================================//
bool InterpreterConfigurator::setAxisType( Gui2dPlot::eAxisType axistype ){
  m_rep->plotitem->setAxisType( axistype );
  return true;
}
//======================================================================//
// - withAnnotationOption
//======================================================================//
bool InterpreterConfigurator::withAnnotationOption( bool setvalue ){
  m_rep->plot2d->withAnnotationOption( setvalue );
  GuiIndex::registerIndexContainer( m_rep->plot2d->getElement() );
  return true;
}
//======================================================================//
// - withPlot3dAnnotationOption
//======================================================================//
bool InterpreterConfigurator::withPlot3dAnnotationOption( bool setvalue ){
  m_rep->plot3d->withAnnotationOption( m_rep->plotaxis, setvalue );
  GuiIndex::registerIndexContainer( m_rep->plot3d->getElement() );
  return true;
}
//======================================================================//
// - setAnnotationValues
//======================================================================//
bool InterpreterConfigurator::setAnnotationValues(){
  m_rep->plotaxis->setAnnotationValues( m_rep->dataitem, m_rep->getScale() );
  return true;
}
//======================================================================//
// - setAnnotationLabels
//======================================================================//
bool InterpreterConfigurator::setAnnotationLabels(){
  m_rep->plotaxis->setAnnotationLabels( m_rep->dataitem );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI PLOT2D Definition                                                        */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - new2dPlot
//======================================================================//
bool InterpreterConfigurator::new2dPlot( const std::string &plotId ){

  if( !GuiFactory::Instance()->have2dPlot() )
    ParserError( compose(_("'%1': No licence found to use PLOT2D."),plotId) );
  if( !IdManager::Instance().registerId( plotId, IdManager::id_2dPlot ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), plotId) );
  }
  m_rep->plot2d = GuiFactory::Instance()->create2dPlot( plotId, true );
  if( m_rep->plot2d == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), plotId) );
  }
  return true;
}
//======================================================================//
// - addDummyItem
//======================================================================//
bool InterpreterConfigurator::addDummyItem(){
  // add a dummy item
  m_rep->xplotitem = m_rep->plot2d->addDataItem( 0, 0 );
  m_rep->xplotitem->setAxisType( Gui2dPlot::XAXIS );
  return true;
}

//======================================================================//
// - plot2d_setScale
//======================================================================//
bool InterpreterConfigurator::plot2d_setScale( double min, double max,  Gui2dPlot::eAxisType axistype ){
  m_rep->plot2d->setPlotAxisScale( min, max, axistype );
  return true;
}

//======================================================================//
// - plot2d_setScaleDataRef
//======================================================================//
bool InterpreterConfigurator::plot2d_setScaleDataRef( Gui2dPlot::eAxisType axistype ){
  XferDataItem *max_xfer = m_rep->popDataItem();
  Scale *max_scale = m_rep->getScale();
  m_rep->plot2d->setPlotAxisScale(  m_rep->popDataItem(), m_rep->getScale(), max_xfer, max_scale, axistype );
  return true;
}

//======================================================================//
// - plot2d_setScaleFormat
//======================================================================//
bool InterpreterConfigurator::plot2d_setScaleFormat( Gui2dPlot::eAxisType axistype, const std::string &format ){
  m_rep->plot2d->setPlotAxisFormat( axistype, format );
  return true;
}

//======================================================================//
// - plot2d_setSize
//======================================================================//
bool InterpreterConfigurator::plot2d_setSize( int width, int height ){
  m_rep->plot2d->setSize( width, height );
  return true;
}

//======================================================================//
// - plot2d_setAspectRatio
//======================================================================//
bool InterpreterConfigurator::plot2d_setAspectRatio(Gui2dPlot::eAxisType axistype, double value) {
  m_rep->plot2d->setAxisAspectRatio(axistype, value);
  return true;
}

//======================================================================//
// - plot2d_setAspectRatioXfer
//======================================================================//
bool InterpreterConfigurator::plot2d_setAspectRatioXfer(Gui2dPlot::eAxisType axistype) {
  m_rep->plot2d->setAxisAspectRatioXfer(axistype, getDataItem());
  return true;
}

//======================================================================//
// - plot2d_setAspectRatioReferenceAxis
//======================================================================//
bool InterpreterConfigurator::plot2d_setAspectRatioReferenceAxis(Gui2dPlot::eAxisType axistype) {
  m_rep->plot2d->setAxisAspectRatioReferenceAxis(axistype);
  return true;
}

//======================================================================//
// - plot2dAddDataItemX
//======================================================================//
bool InterpreterConfigurator::plot2dAddDataItemX(){
  if( (m_rep->plotaxis = new GuiPlotDataItem( m_rep->dataitem, 1 ) ) == 0 )
    ParserError( _("Undeclared identifier.") );
  int wildcards( m_rep->dataitem->getNumOfWildcards() );
  if( m_rep->dataitem->getNumOfWildcards() >  1+1 ){ // maybe + + if wildcardIndex
    ParserError( _("Too many WILDCARDS.") );
  }
  addDummyItem();
  if( ( m_rep->plotitem = m_rep->plot2d->addDataItem( m_rep->plotaxis, m_rep->xplotitem ) ) == 0 )
    ParserError( _("Undeclared identifier.") );
  m_rep->xplotitem = m_rep->plotitem;
  m_rep->xplotitem->setAxisType( Gui2dPlot::XAXIS );
  m_rep->plotaxis->setScale( m_rep->getScale() );
  GuiIndex::registerIndexContainer( m_rep->plot2d->getElement() );
  return true;
}
//======================================================================//
// - plot2dAddDataItemY
//======================================================================//
bool InterpreterConfigurator::plot2dAddDataItemY(){
  if( (m_rep->plotaxis = new GuiPlotDataItem( m_rep->dataitem,1 )) == 0 )
    ParserError( _("Undeclared identifier.") );
  int wildcards( m_rep->dataitem->getNumOfWildcards() );
  if( m_rep->dataitem->getNumOfWildcards() > 1+1 ){ // maybe + + if wildcardIndex
    ParserError( _("Too many WILDCARDS.") );
  }
  if( ( m_rep->plotitem = m_rep->plot2d->addDataItem( m_rep->plotaxis, m_rep->xplotitem ) ) == 0 )
    ParserError( _("Undeclared identifier.") );
  m_rep->plotaxis->setScale( m_rep->getScale() );
  GuiIndex::registerIndexContainer( m_rep->plot2d->getElement() );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI Navigator                                                                */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - set Navigator Type Xrt(Default) / Xm / IconView
//======================================================================//
bool InterpreterConfigurator::setNavigatorType(GuiNavigator::Type type) {
  m_rep -> navType = type;
  return true;
}

//======================================================================//
// - newNavigator
//======================================================================//
bool InterpreterConfigurator::newNavigator( const std::string &navigatorId ){
  if( !IdManager::Instance().registerId( navigatorId, IdManager::id_Navigator ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), navigatorId) );
  }

  m_rep->navigator = GuiFactory::Instance()->createNavigator( navigatorId, m_rep->navType );

  if( m_rep->navigator == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), navigatorId) );
  }
  return true;
}

//======================================================================//
// - navigatorSetSize
//======================================================================//
bool InterpreterConfigurator::navigatorSetSize( const int width, const int height ){
  if(m_rep->navigator!=0)
    m_rep->navigator->setSize( width, height );
  return true;
}
//======================================================================//
// - navigatorSetSize
//======================================================================//
bool InterpreterConfigurator::navigatorSetToolTip( const std::string &toolTipTagId ){
  if(m_rep->navigator!=0)
    m_rep->navigator->setToolTipTag( toolTipTagId );
  return true;
}
//======================================================================//
// - navigatorAddColumn
//======================================================================//
bool InterpreterConfigurator::navigatorAddColumn( const std::string &tagId, const std::string &labeltext,
                                                  GuiNavigator::ColumnStyle colStyle){
  m_rep->navigator->addColumn( tagId, labeltext, m_rep->fieldlength,
                               m_rep->fieldprec, m_rep->getScale(), m_rep->fieldthousandsep, colStyle );
  return true;
}
//======================================================================//
// - navigatorMultipleSelection
//======================================================================//
bool InterpreterConfigurator::navigatorMultipleSelection() {
  if(m_rep->navigator!=0)
    m_rep->navigator->setMultipleSelection(true);
  return true;
}
//======================================================================//
// - navigatorSetExpandable
//======================================================================//
bool InterpreterConfigurator::navigatorSetExpandable() {
  if(m_rep->navigator!=0)
    m_rep->navigator->setExpandable(true);
  return true;
}
//======================================================================//
// - navigatorSetScrollable
//======================================================================//
bool InterpreterConfigurator::navigatorSetScrollable() {
  if(m_rep->navigator!=0)
    m_rep->navigator->setScrollable(true);
  return true;
}
//======================================================================//
// - navigatorSetCompareMode
//======================================================================//
bool InterpreterConfigurator::navigatorSetCompareMode(){
  if(m_rep->navigator!=0)
    m_rep->navigator->setCompareMode();
  return true;
}

//======================================================================//
// - setRoot
//======================================================================//
bool InterpreterConfigurator::setRoot( const std::string &labeltext ){
  m_rep->root = m_rep->navigator->setRoot( m_rep->dataitem, labeltext );
  GuiIndex::registerIndexContainer(m_rep->navigator->getElement() );
  return true;
}
//======================================================================//
// - setLastlevel
//======================================================================//
bool InterpreterConfigurator::setLastlevel( const double level ){
  if(m_rep->navigator!=0)
    m_rep->root->setLastLevel( Roundint(level) );
  return true;
}
//======================================================================//
// - setFirstStructFolder
//======================================================================//
bool InterpreterConfigurator::setFirstStructFolder( const double level ){
  if(m_rep->navigator!=0)
    m_rep->root->setFirstStructFolder( Roundint(level) );
  return true;
}
//======================================================================//
// - setAutoLevel
//======================================================================//
bool InterpreterConfigurator::setAutoLevel(){
  if(m_rep->navigator!=0)
    m_rep->root->setAutoLevel();
  return true;
}
//======================================================================//
// - setHideEmptyFolder
//======================================================================//
bool InterpreterConfigurator::setHideEmptyFolder(){
  if(m_rep->navigator!=0)
    m_rep->root->setHideEmptyFolder();
  return true;
}
//======================================================================//
// - addTransparentFolder
//======================================================================//
bool InterpreterConfigurator::addTransparentFolder( const std::string& hide_folder ){
  if(m_rep->navigator!=0)
    m_rep->navigator->addTransparentFolder( hide_folder );
  return true;
}
//======================================================================//
// - setOpenLevels
//======================================================================//
bool InterpreterConfigurator::setOpenLevels( const double level ){
  if(m_rep->navigator!=0)
    m_rep->root->setOpenLevels( Roundint(level) );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI Form Textwindow                                                          */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newGuiScrolledText
//======================================================================//
bool InterpreterConfigurator::newGuiScrolledText( const std::string &textwindowId ){
  if( !IdManager::Instance().registerId( textwindowId, IdManager::id_Textwindow ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), textwindowId) );
  }
  m_rep->scrolledtext = GuiFactory::Instance()->createScrolledText( 0, textwindowId );
  if( m_rep->scrolledtext == 0 ){
    ParserError( compose(_("Item '%1' is already declared."), textwindowId) );
  }
  UImanager::Instance().addHardCopy( textwindowId, m_rep->scrolledtext->getElement()->getHardCopyListener() );
  UImanager::Instance().addSaveButton( textwindowId, m_rep->scrolledtext->getSaveListener() );
  return true;
}
//======================================================================//
// - scrolledtextSetSize
//======================================================================//
bool InterpreterConfigurator::scrolledtextSetSize( const double lines, const double length ){
  m_rep->scrolledtext->setOptionLength( Roundint(length) );
  m_rep->scrolledtext->setOptionLines( Roundint(lines) );
  return true;
}
//======================================================================//
// - scrolledtextSetLabel
//======================================================================//
bool InterpreterConfigurator::scrolledtextSetLabel( const std::string &labeltext ){
  m_rep->scrolledtext->setOptionLabel( labeltext );
  return true;
}
//======================================================================//
// - setWordwrap
//======================================================================//
bool InterpreterConfigurator::setWordwrap( bool setvalue ){
  m_rep->scrolledtext->setOptionWordwrap( setvalue );
  return true;
}
//======================================================================//
// - setFormatFortran
//======================================================================//
bool InterpreterConfigurator::setFormatFortran(){
  m_rep->scrolledtext->setOptionFormat( GuiScrolledText::format_Fortran );
  return true;
}
//======================================================================//
// - scrolledtextSetPrintFilter
//======================================================================//
bool InterpreterConfigurator::scrolledtextSetPrintFilter( const std::string &filtertext ){
  m_rep->scrolledtext->setOptionPrintFilter( filtertext );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI Folder Definitions                                                       */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newGuiFolder
//======================================================================//
bool InterpreterConfigurator::newGuiFolder( const std::string &folderId ){
  if( !GuiFactory::Instance()->haveFolder() /*&& XmVersion < 2002*/ ){
    ParserError( _("No licence found to use folders") );
  }
  if( !IdManager::Instance().registerId( folderId, IdManager::id_Folder ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), folderId) );
  }
   m_rep->folder = GuiFactory::Instance()->createFolder( 0, folderId );
  if( m_rep->folder == 0 )
    ParserError( compose(_("Item '%1' is already declared."), folderId) );
  return true;
}
//======================================================================//
// - attachFolder
//======================================================================//
bool InterpreterConfigurator::attachFolder( const std::string &elementId, GuiElement::Alignment alignment ){
  m_rep->element = GuiElement::findElement( elementId );
  if( m_rep->element == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), elementId) );
  }
  if( m_rep->element->Type() != GuiElement::type_Folder ){
    ParserError( compose(_("Undeclared identifier '%1'."), elementId) );
  }
  if( m_rep->element->installed() ){
    if( m_rep->element->cloneable() ){
      m_rep->element = m_rep->element->clone();
    }
    else{
      ParserError( compose(_("Folder '%1' is already installed."), elementId) );
    }
  }
  m_rep->element->setParent( m_rep->fieldgroupline->getElement() );
  m_rep->element->setAlignment( alignment );
  m_rep->fieldgroupline->attach( m_rep->element );
  return true;
}
//======================================================================//
// - testNumberOfPages
//======================================================================//
bool InterpreterConfigurator::testNumberOfPages(){
  if( m_rep->folder->NumberOfPages() < 1 )
    ParserError( _("A Folder must have one member") );
  return true;
}
//======================================================================//
// - setButtonPlacement
//======================================================================//
bool InterpreterConfigurator::setButtonPlacement( GuiElement::Alignment alignment ){
  m_rep->folder->setButtonPlacement( alignment );
  return true;
}
//======================================================================//
// - setButtonText
//======================================================================//
bool InterpreterConfigurator::setButtonText( GuiElement::ButtonType type, const std::string &text ) {
  m_rep->buttonText[type] = text;
  return true;
}
//======================================================================//
// - setLabelOrientation
//======================================================================//
bool InterpreterConfigurator::setLabelOrientation( GuiElement::Orientation orient ){
  m_rep->folder->setLabelOrientation( orient );
  return true;
}
//======================================================================//
// - setTabStretch
//======================================================================//
bool InterpreterConfigurator::setTabStretch(){
  m_rep->folder->setTabStretch( true );
  return true;
}
//======================================================================//
// - setTabExpandable
//======================================================================//
bool InterpreterConfigurator::setTabExpandable(){
  m_rep->folder->setTabExpandable( true );
  return true;
}
//======================================================================//
// - setTabMovable
//======================================================================//
bool InterpreterConfigurator::setTabMovable(){
  m_rep->folder->setTabMovable( true );
  return true;
}
//======================================================================//
// - hideFolderButtons
//======================================================================//
bool InterpreterConfigurator::hideFolderButtons(){
  m_rep->folder->hideButtons();
  return true;
}
//======================================================================//
// - showAlwaysFolderButtons
//======================================================================//
bool InterpreterConfigurator::showAlwaysFolderButtons(){
  m_rep->folder->showAlwaysButtons();
  return true;
}
//======================================================================//
// - setFolderTabHidden
//======================================================================//
bool InterpreterConfigurator::setFolderTabHidden(const std::string& hidden){
  m_rep->folderTabHidden |= toBool(hidden);
  return true;
}
//======================================================================//
// - setFolderTabIcon
//======================================================================//
bool InterpreterConfigurator::setFolderTabIcon(const std::string& iconname){
  m_rep->folderTabIcon = iconname;
  return true;
}
//======================================================================//
// - addFolderPage
//======================================================================//
bool InterpreterConfigurator::addFolderPage( const std::string &buttonlabel){
  assert( m_rep->folder != 0 );
  bool bhide(m_rep->folderTabHidden);
  GuiOrientationContainer * folderPage = m_rep->folder->addFolderPage( buttonlabel, m_rep->folderTabIcon, bhide );
  m_rep->folderTabHidden = false;
  m_rep->folderTabIcon.clear();
  m_rep->container = folderPage->getElement();
  if (bhide) return true;  // silently return
  if( folderPage == 0 )
    ParserError( compose(_("Page '%1' is already declared."), buttonlabel) );
  if( m_rep->container == 0 )
    ParserError( compose(_("Item '%1' is already declared."), buttonlabel) );
  return true;
}
// //======================================================================//
// // - uiFolderDefinitionFolderGroup
// //======================================================================//
// bool InterpreterConfigurator::uiFolderDefinitionFolderGroup(){
// //  m_rep->i++; // Wird nur im Parser benötigt (data_V1...)
//   return true;
// }
//======================================================================//
// - registerFolderGroupId
//======================================================================//
bool InterpreterConfigurator::registerFolderGroupId( const std::string &folderGroupId ){
  if( !IdManager::Instance().registerId( folderGroupId, IdManager::id_Foldergroup ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), folderGroupId) );
  }
  return true;
}
//======================================================================//
// - addFolderGroupEntry
//======================================================================//
bool InterpreterConfigurator::addFolderGroupEntry( const std::string &folderGroupId ){
  if( !GuiFolderGroup::addFolderGroupEntry( folderGroupId, m_rep->folder, m_rep->folder->NumberOfPages() -1, m_rep->function ) ){
    ParserError( compose(_("%1: Only one tab of the same tab-group is allowed"), folderGroupId) );
  }
  return true;
}
//======================================================================//
// - resetFunction
//======================================================================//
bool InterpreterConfigurator::resetFunction(){
  m_rep->function=0;
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI Plugin                                                                   */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - addNamedElement
//======================================================================//
bool InterpreterConfigurator::addNamedElement( const std::string &pluginId ){
  if( !IdManager::Instance().registerId( pluginId, IdManager::id_Plugin ) ){
    ParserError( compose(_("Identifier '%1' is not declared."), pluginId) );
  }
  try {
    if( (m_rep->plugin = Plugin::open( pluginId )) == 0 ){
      ParserError( compose(_("Item '%1' cannot be found."), pluginId) );
    }
  }
  catch( const char * msg ){
    ParserError( compose(_("Item '%1' is already declared."), pluginId) );
  }
  return true;
}
//======================================================================//
// - pluginSetLabel
//======================================================================//
bool InterpreterConfigurator::pluginSetLabel( const std::string &labelstring ){
  m_rep->plugin->GuiMenuButtonListener::setLabel( labelstring );
  return true;
}
//======================================================================//
// - pluginAttachGuiElement
//======================================================================//
bool InterpreterConfigurator::pluginAttachGuiElement( const std::string &elementId ){
  m_rep->element = GuiElement::findElement( elementId );
  if( m_rep->element == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), elementId) );
  }
  if( m_rep->element->installed() ){
    if( m_rep->element->cloneable() ){
      m_rep->element = m_rep->element->clone();
    }
    else{
      ParserError( compose(_("Form '%1' is already installed."), elementId) );
    }
  }
  m_rep->element->setParent( m_rep->plugin );
  m_rep->plugin->attach( m_rep->element );
  return true;
}
//======================================================================//
// - setPluginXslFileName
//======================================================================//
bool InterpreterConfigurator::setPluginXslFileName( const std::string &filename ){
  m_rep->plugin->setXSLFileName( filename );
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI XRT Table                                                                */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newGuiTable
//======================================================================//
bool InterpreterConfigurator::newGuiTable( const std::string &tableId ){
  if( !GuiFactory::Instance()->haveTable() )
    ParserError( compose(_("%1: No licence found to use tables."), tableId) );
  if( !IdManager::Instance().registerId( tableId, IdManager::id_Table ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), tableId) );
  }
  m_rep->table = GuiFactory::Instance()->createTable( 0, tableId );
  if( m_rep->table == 0 )
    ParserError( compose(_("Item '%1' is already declared."), tableId) );
  return true;
}
//======================================================================//
// - attachTable
//======================================================================//
bool InterpreterConfigurator::attachTable( const std::string &elementId, GuiElement::Alignment alignment ){
  m_rep->element = GuiElement::findElement( elementId );
  if( m_rep->element == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), elementId) );
  }
  if( m_rep->element->Type() != GuiElement::type_Table ){
    ParserError( compose(_("'%1' is not a list"), elementId) );
  }
  if( m_rep->element->installed() ){
    if( m_rep->element->cloneable() ){
      m_rep->element = m_rep->element->clone();
    }
    else{
      ParserError( compose(_("List '%1' is already installed."), elementId) );
    }
  }
  m_rep->element->setParent( m_rep->fieldgroupline->getElement() );
  m_rep->element->setAlignment( alignment );
  m_rep->fieldgroupline->attach( m_rep->element );
  return true;
}
//======================================================================//
// - attachPlot2d
//======================================================================//
bool InterpreterConfigurator::attachPlot2d( const std::string &elementId, GuiElement::Alignment alignment ){
  m_rep->element = GuiElement::findElement( elementId );
  if( m_rep->element == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), elementId) );
  }
  if( m_rep->element->Type() != GuiElement::type_QWTPlot ){
    ParserError( compose(_("'%1' is not a plot2d"), elementId) );
  }
  if( m_rep->element->installed() ){
    if( m_rep->element->cloneable() ){
      m_rep->element = m_rep->element->clone();
    }
    else{
      ParserError( compose(_("List '%1' is already installed."), elementId) );
    }
  }
  m_rep->element->setParent( m_rep->fieldgroupline->getElement() );
  m_rep->element->setAlignment( alignment );
  m_rep->fieldgroupline->attach( m_rep->element );
  return true;
}

//======================================================================//
// - setLabelAlignment
//======================================================================//

bool InterpreterConfigurator::setLabelAlignment( GuiElement::Alignment alignment ){
  m_rep->table->setLabelAlignment( alignment );
  return true;
}

//======================================================================//
// - newTableSize
//======================================================================//
bool InterpreterConfigurator::newTableSize(){
  m_rep->tablesize = m_rep->table->newTableSize();
  return true;
}
//======================================================================//
// - setTableRowSize
//======================================================================//
bool InterpreterConfigurator::setTableRowSize(){
  m_rep->table->setTableRowSize( m_rep->tablesize );
  delete m_rep->tablesize;
  return true;
}
//======================================================================//
// - setTableColSize
//======================================================================//
bool InterpreterConfigurator::setTableColSize(){
  m_rep->table->setTableColSize( m_rep->tablesize );
  delete m_rep->tablesize;
  return true;
}
//======================================================================//
// - setTableOrientation
//======================================================================//
bool InterpreterConfigurator::setTableOrientation( GuiElement::Orientation orientation ){
  m_rep->table->setTableOrientation( orientation );
  return true;
}
//======================================================================//
// - setTableNavigation
//======================================================================//
bool InterpreterConfigurator::setTableNavigation( GuiElement::Orientation orientation ){
  m_rep->table->setTableNavigation( orientation );
  return true;
}
//======================================================================//
// - setTableFunction
//======================================================================//
bool InterpreterConfigurator::setTableFunction(){
  m_rep->table->setFunction( m_rep->function );
  return true;
}
//======================================================================//
// - setTableAutoWidth
//======================================================================//
bool InterpreterConfigurator::setTableAutoWidth(){
  m_rep->table->setAutoWidth();
  return true;
}
//======================================================================//
// - tablesizeSetSize
//======================================================================//
bool InterpreterConfigurator::tablesizeSetSize( const double size ){
  m_rep->tablesize->setSize( Roundint(size) );
  return true;
}
//======================================================================//
// - tablesizeSetRange
//======================================================================//
bool InterpreterConfigurator::tablesizeSetRange( int from, int to ){
  m_rep->tablesize->setRange( from, to );
  return true;
}
//======================================================================//
// - tablesizeHideHeader
//======================================================================//
bool InterpreterConfigurator::tablesizeHideHeader(){
  m_rep->tablesize->hideHeader();
  return true;
}
//======================================================================//
// - hideTableIndexMenu
//======================================================================//
bool InterpreterConfigurator::hideTableIndexMenu(){
  if( m_rep->table != 0 )
    m_rep->table->hideIndexMenu();
  return true;
}
//======================================================================//
// - setHorizontalPlacement
//======================================================================//
bool InterpreterConfigurator::setHorizontalPlacement( GuiElement::Alignment alignment ){
  if( !m_rep->table->setHorizontalPlacement( alignment ) )
    ParserError( _("Only the Options TOP or BOTTOM are allowed here") );
  return true;
}
//======================================================================//
// - setVerticalPlacement
//======================================================================//
bool InterpreterConfigurator::setVerticalPlacement( GuiElement::Alignment alignment ){
  if( !m_rep->table->setVerticalPlacement( alignment ) )
    ParserError( _("Only the Options LEFT or RIGHT are allowed here") );
  return true;
}
//======================================================================//
// - setTable
//======================================================================//
bool InterpreterConfigurator::setTable(){
  if( !m_rep->table->setTable() )
    ParserError( _("The TABLE is already specified") );
  return true;
}
//======================================================================//
// - addHorizontalLine
//======================================================================//
bool InterpreterConfigurator::addHorizontalLine( const std::string &label, GuiElement::Alignment alignment ){
  if( (m_rep->tableline=m_rep->table->addHorizontalLine( UnitManager::extractValue(label), alignment )) == 0 ){
    ParserError( _("Unknown error.") ); /* MEMORY ? */
  }
  return true;
}
//======================================================================//
// - tablelineCheck
//======================================================================//
bool InterpreterConfigurator::tablelineCheck(){
  if( m_rep->tableline->check() )
    ParserError( _("tableline type mismatch.") );
  return true;
}
//======================================================================//
// - addTableDataItem
//======================================================================//
bool InterpreterConfigurator::addTableDataItem(){
  m_rep->tabledataitem = m_rep->tableline->addTableDataItem( m_rep->dataitem );
  if( m_rep->tabledataitem == 0 ){
    ParserError( _("Unknown error.") );  /* MEMORY ? */
  }
  if( m_rep->dataitem->getNumOfWildcards() > 1 ){
    ParserError( _("Number of WILDCARDS is > 1.") );
  }
  m_rep->datafield = m_rep->tabledataitem->getDataField();
  GuiIndex::registerIndexContainer( m_rep->tabledataitem->getElement() );
  return true;
}
//======================================================================//
// - addTableLabelItem
//======================================================================//
bool InterpreterConfigurator::addTableLabelItem( const std::string &label, GuiElement::Alignment alignment ){
  //  m_rep->tableitem =
m_rep->tableline->addTableLabelItem( label, alignment );
  return true;
}

//======================================================================//
// - addVerticalLine
//======================================================================//
bool InterpreterConfigurator::addVerticalLine( const std::string &label, GuiElement::Alignment alignment ){
  m_rep->tableline = m_rep->table->addVerticalLine( UnitManager::extractValue(label), alignment );
  if( m_rep->tableline == 0 )
    ParserError( _("Unknown error.") );  /* MEMORY ? */
  return true;
}
//======================================================================//
// - addTableLine
//======================================================================//
bool InterpreterConfigurator::addTableLine( const std::string &label, GuiElement::Alignment alignment ){
  m_rep->tableline = m_rep->table->addTableLine( UnitManager::extractValue(label), alignment );
  if( m_rep->tableline == 0 ){
    ParserError( _("Unknown error.") );  /* MEMORY ? */
  }
  return true;
}
//======================================================================//
// - addTableLineToolTip
//======================================================================//
bool InterpreterConfigurator::addTableLineToolTip(){
  m_rep->tableline = m_rep->table->addTableLineToolTip();
  if( m_rep->tableline == 0 ){
    ParserError( _("Unknown error.") );  /* MEMORY ? */
  }
  return true;
}
//======================================================================//
// - addTableLineColor
//======================================================================//
bool InterpreterConfigurator::addTableLineColor(){
  m_rep->tableline = m_rep->table->addTableLineColor();
  if( m_rep->tableline == 0 ){
    ParserError( _("Unknown error.") );  /* MEMORY ? */
  }
  return true;
}
//======================================================================//
// - addTableDataItemMatrix
//======================================================================//
bool InterpreterConfigurator::addTableDataItemMatrix(){
  m_rep->tableline = m_rep->table->addTableLine( "", GuiElement::align_Default );
  if( m_rep->tableline == 0 ){
    ParserError( _("Unknown error.") );  /* MEMORY ? */
  }
  m_rep->tabledataitem = m_rep->tableline->addTableDataItem( m_rep->dataitem );
  if( m_rep->tabledataitem == 0 ){
    ParserError( _("Unknown error.") );  /* MEMORY ? */
  }
  if( m_rep->dataitem->getNumOfWildcards() != 2 ){
    ParserError( _("Number of WILDCARDS is not 2.") );
  }
  m_rep->datafield = m_rep->tabledataitem->getDataField();
  m_rep->table->setMatrix();
  GuiIndex::registerIndexContainer( m_rep->tabledataitem->getElement() );
  return true;
}

/* --------------------------------------------------------------------------- */
/* Menu Global Functions                                                       */
/* --------------------------------------------------------------------------- */

//======================================================================//
// - createNewMenu
//======================================================================//
bool InterpreterConfigurator::createNewMenu( const std::string &label ){
  GuiPulldownMenu *menu = UImanager::Instance().createNewMenu( label );
  m_rep->container = menu->getElement();
  return true;
}
//======================================================================//
// - registerMenuId
//======================================================================//
bool InterpreterConfigurator::registerMenuId( const std::string &menuId ){
  if( !IdManager::Instance().registerId( menuId, IdManager::id_Menu ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), menuId) );
  }
  GuiPulldownMenu *menu = UImanager::Instance().createNewMenu( menuId, menuId );
  m_rep->container = menu->getElement();
  assert( m_rep->container != 0 );
  return true;
}
//======================================================================//
// - attachMenu
//======================================================================//
bool InterpreterConfigurator::attachMenu(){
  UImanager::Instance().attachMenu( m_rep->container );
  return true;
}
//======================================================================//
// - getExistingMenu
//======================================================================//
bool InterpreterConfigurator::getExistingMenu( const std::string &menuId ){
  GuiPulldownMenu *menu = UImanager::Instance().getExistingMenu( menuId );
  m_rep->container = menu->getElement();
  assert( m_rep->container != 0 );
  return true;
}
//======================================================================//
// - createNewSubMenu
//======================================================================//
bool InterpreterConfigurator::createNewSubMenu( const std::string &label ){
  GuiPulldownMenu *menu = UImanager::Instance().createNewSubMenu( m_rep->container, label );
  m_rep->container = menu->getElement();
  return true;
}
//======================================================================//
// - registerSubMenuId
//======================================================================//
bool InterpreterConfigurator::registerSubMenuId( const std::string &menuId ){
  if( !IdManager::Instance().registerId( menuId, IdManager::id_Menu ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), menuId) );
  }
  GuiPulldownMenu *menu = UImanager::Instance().createNewSubMenu( m_rep->container, menuId, menuId );
  m_rep->container = menu->getElement();
  assert( m_rep->container != 0 );
  return true;
}
//======================================================================//
// - pulldownmenuSetLabel
//======================================================================//
bool InterpreterConfigurator::pulldownmenuSetLabel( const std::string &label ){
  dynamic_cast<GuiPulldownMenu*>(m_rep->container)->setLabel( label );
  return true;
}
//======================================================================//
// - pulldownmenuSetHelptext
//======================================================================//
bool InterpreterConfigurator::pulldownmenuSetHelptext( const std::string &helptext ){
  m_rep->container->setHelptext( helptext );
  return true;
}
//======================================================================//
// - formAttachMenu
//======================================================================//
bool InterpreterConfigurator::formAttachMenu( const std::string &formId ){
  m_rep->form = GuiElement::getForm( formId );
  if( m_rep->form != 0 ){
    // Wenn es sich um die MAIN-Form handelt, erhalten wir hier keinen Pointer,
    // d.h. der Befehl wird ignoriert.
    m_rep->form->attachMenu( m_rep->container );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* UI Menu                                                                     */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - getParentMenu
//======================================================================//
bool InterpreterConfigurator::getParentMenu(){
  GuiElement *parent = m_rep->container->getParent();
  assert( parent != 0 );
  if( parent->Type() == GuiElement::type_PulldownMenu ){
    m_rep->container = parent;
  }
  else if( parent->Type() == GuiElement::type_PopupMenu ){
    m_rep->container = parent;
  }
  else
    assert( false );
  return true;
}

//======================================================================//
// - addFormMenuButton
//======================================================================//
bool InterpreterConfigurator::addFormMenuButton( const std::string &formId, const std::string &label, bool toggle){
  if( !UImanager::Instance().addFormMenuButton( m_rep->container, formId, label, toggle ) ){
    ParserError( compose(_("Undeclared form '%1'."), formId) );
  }
  return true;
}
//======================================================================//
// - addFunctionMenuButton
//======================================================================//
bool InterpreterConfigurator::addFunctionMenuButton( const std::string &funcId, const std::string &label ){
  if( !UImanager::Instance().addFunctionMenuButton( m_rep->container, funcId, label ) ){
    ParserError( compose(_("Undeclared function '%1'."), funcId) );
  }
  return true;
}
//======================================================================//
// - addFunctionNavigatorMenuButton
//======================================================================//
bool InterpreterConfigurator::addFunctionNavigatorMenuButton( const std::string &funcId, const std::string &label ){
  if( !UImanager::Instance().addFunctionNavigatorMenuButton( m_rep->container, funcId, label ) ){
    ParserError( compose(_("Undeclared function '%1'."), funcId) );
  }
  return true;
}
//======================================================================//
// - getOpenMenu
//======================================================================//
bool InterpreterConfigurator::getOpenMenu( bool with_remaining ){
  GuiPulldownMenu *menu = UImanager::Instance().getOpenMenu( with_remaining );
  m_rep->container = menu->getElement();
  m_rep->filemenulist = UImanager::Instance().getOpenButtonList();
  return true;
}
//======================================================================//
// - resetPulldownmenu
//======================================================================//
bool InterpreterConfigurator::resetPulldownmenu(){
  m_rep->container = 0;
  return true;
}
//======================================================================//
// - getSaveMenu
//======================================================================//
bool InterpreterConfigurator::getSaveMenu( bool with_remaining ){
  GuiPulldownMenu *menu = UImanager::Instance().getSaveMenu( with_remaining );
  m_rep->container = menu->getElement();
  m_rep->filemenulist = UImanager::Instance().getSaveButtonList();
  return true;
}
//======================================================================//
// - createFileButton
//======================================================================//
bool InterpreterConfigurator::createFileButton( const std::string &name, const std::string &label ){
  if( !UImanager::Instance().createFileButton( m_rep->container, name, label, m_rep->filemenulist ) ){
    ParserError( compose(_("Undeclared identifier '%1'."), name) );
  }
  return true;
}
//======================================================================//
// - getHardcopyFolder
//======================================================================//
bool InterpreterConfigurator::getHardcopyFolder( bool with_remaining ){
  m_rep->printfolder = GuiFactory::Instance()->createPrinterDialog()->getFolder( with_remaining );
  return true;
}
//======================================================================//
// - resetPrintfolder
//======================================================================//
bool InterpreterConfigurator::resetPrintfolder(){
  m_rep->printfolder = 0;
  return true;
}
//======================================================================//
// - addHardcopyFolder
//======================================================================//
bool InterpreterConfigurator::addHardcopyFolder( const std::string &label ){
  m_rep->printfolder = m_rep->printfolder->addFolder( label );
  return true;
}
//======================================================================//
// - getParentFolder
//======================================================================//
bool InterpreterConfigurator::getParentFolder(){
  m_rep->printfolder = m_rep->printfolder->parent();
  return true;
}
//======================================================================//
// - printfolderAddSeparator
//======================================================================//
bool InterpreterConfigurator::printfolderAddSeparator(){
  m_rep->printfolder->addSeparator();
  return true;
}
//======================================================================//
// - addObject
//======================================================================//
bool InterpreterConfigurator::addObject( const std::string &name, const std::string &label ){
  m_rep->printobject = GuiFactory::Instance()->createPrinterDialog()->getHardCopy( name );
  if( m_rep->printobject == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), name) );
  }
  m_rep->printfolder->addObject( label, m_rep->printobject );
  return true;
}
//======================================================================//
// - newFormElementMenu
//======================================================================//
bool InterpreterConfigurator::newFormElementMenu( const std::string &elementId ){
  m_rep->element = GuiElement::findElement( elementId );
  assert( m_rep->element != 0 );
  if( m_rep->element->Type() != GuiElement::type_List &&
      m_rep->element->Type() != GuiElement::type_Navigator &&
      m_rep->element->Type() != GuiElement::type_NavDiagram &&
      m_rep->element->Type() != GuiElement::type_NavIconView &&
      m_rep->element->Type() != GuiElement::type_Thermo){
    ParserError( compose(_("not yet implemented menu feature for GuiElement: '%1'."), elementId) );
    return false;
  }
  m_rep->container = m_rep->element->newPopupMenu()->getElement();
  return true;
}
//======================================================================//
// - newStructureMenu
//======================================================================//
bool InterpreterConfigurator::newStructureMenu( const std::string &structure, unsigned int idx ){
  DataDictionary *dd = DataPoolIntens::getDataPool().FindDictEntry( structure.c_str() );
  if( dd == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), structure) );
    return false;
  }
  if( dd -> getDataType() != DataDictionary::type_StructDefinition ){
    ParserError( compose(_("'%1' is not of type STRUCT."), structure) );
    return false;
  }
  UserAttr *attr = static_cast<UserAttr*>( dd -> GetAttr() );
  GuiPopupMenu *menu =  m_rep->guifactory->createNavigatorMenu( 0 );
  menu -> resetMenuPost();
  attr -> setPopupMenu( menu, idx );
  m_rep -> container =  menu->getElement();

  return true;
}
//======================================================================//
// - newStructureDropMenu
//======================================================================//
bool InterpreterConfigurator::newStructureDropMenu( const std::string &structure ){
  DataDictionary *dd = DataPoolIntens::getDataPool().FindDictEntry( structure.c_str() );
  if( dd == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), structure) );
    return false;
  }
  if( dd -> getDataType() != DataDictionary::type_StructDefinition ){
    ParserError( compose(_("'%1' is not of type STRUCT."), structure) );
    return false;
  }
  UserAttr *attr = static_cast<UserAttr*>( dd -> GetAttr() );
  GuiPopupMenu *menu =  m_rep->guifactory->createNavigatorMenu( 0 );
  menu -> resetMenuPost();
  attr -> setPopupDropMenu( menu );
  m_rep -> container =  menu->getElement();

  return true;
}

//======================================================================//
// - newPlot2dMenuDescription
//======================================================================//
bool InterpreterConfigurator::newPlot2dMenuDescription(){
  if( m_rep -> plot2dMenuDescription != 0 ){
    ParserError( compose(_("The menu '%1' already exists."), "PLOT2D") );
    return false;
  }
  m_rep -> plot2dMenuDescription = m_rep->guifactory->getPlot2dMenuDescription();
  return true;
}

//======================================================================//
// - addPlot2dMenuEntry
//======================================================================//
bool InterpreterConfigurator::addPlot2dMenuEntry( const std::string &entry ){
//   if( !Plot2dMenuDescription::Instance().addEntry( entry ) ){
  if( !m_rep->guifactory->getPlot2dMenuDescription()->addEntry( entry ) ){
    ParserError( compose(_("Entry '%1' is not valid."), entry) );
    return false;
  }
  return true;
}

//**********************************************************************//
// END UI_MANAGER
//**********************************************************************//

//**********************************************************************//
// OPERATOR
//**********************************************************************//

//======================================================================//
// - newMsgQueue
//======================================================================//
bool InterpreterConfigurator::newMsgQueue( const std::string& id ){
  int lineNo;
  std::string filename;
  if( !IdManager::Instance().registerId( id, IdManager::id_MessageQueue ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), id) );
  }
  if( m_rep->transferData.type == MessageQueue::type_Undefined){
    ParserError( _("No message queue type defined.") );
  }

  // checks for subscriber or request type
  if ( m_rep->transferData.type == MessageQueue::type_Request ||
       m_rep->transferData.type == MessageQueue::type_Subscriber ) {
    if( m_rep->transferData.port == -1 && m_rep->transferData.portRequest == -1){
      ParserError( _("No port and port_request defined") );
    }

    if( m_rep->transferData.port != -1 &&  m_rep->msgQueueHeader.size() == 0){
      ParserError( _("No subscriber header(s) defined.") );
    }
  }
  if ( m_rep->transferData.type == MessageQueue::type_Reply ||
       m_rep->transferData.type == MessageQueue::type_Publisher ) {
    if( m_rep->transferData.port == -1 &&
        m_rep->transferData.type == MessageQueue::type_Publisher){
      ParserError( _("No publisher port defined") );
    }
    if( m_rep->transferData.portRequest == -1 &&
        m_rep->transferData.type == MessageQueue::type_Reply){
      ParserError( _("No port request defined") );
    }
    if( m_rep->transferData.portRequest != -1 &&
        (m_rep->msgQueueHeader.size() == 0 && m_rep->transferData.out_streamVector.size() == 0)){
      ParserError( _("No publisher header(s) defined. (or no default response stream)") );
    }
    if( m_rep->transferData.portRequest == -1 &&
        (m_rep->msgQueueHeader.size() > 0 || m_rep->transferData.out_streamVector.size() != 0)){
      ParserError( _("No request port set. (but header(s) or default response stream)") );
    }
  }

  if (AppData::Instance().LspWorker()) {
    filename = App::Instance().getFlexer()->getCurrentFilename();
    if (filename.ends_with('"')) {
      filename.pop_back();
    }
    lineNo = PAlineno -(*(App::Instance().getFlexer()->YYText() ) == '\n' || ! *(App::Instance().getFlexer()->YYText()));
  }

  // create message queue
  int p = m_rep->transferData.portRequest;

  // default timeout:
  if ( m_rep->transferData.timeout == -1 ) // -1 : not set in description file
    m_rep->transferData.timeout = 10;      // -> use default (10 seconds)

  //
  // Request
  //
  if (m_rep->transferData.type == MessageQueue::type_Request) {
    MessageQueue::Instance().createRequest(id, m_rep->transferData.host,
                                           m_rep->transferData.portRequest, m_rep->transferData.timeout, lineNo, filename);
  }
  //
  // Subscriber
  //
  else if (m_rep->transferData.type == MessageQueue::type_Subscriber) {
    MessageQueueSubscriber *subscriber;
    subscriber = MessageQueue::Instance().createSubscriber(id,
							   m_rep->transferData.host,
							   m_rep->transferData.port, lineNo, filename);
    // neuer SocketServer mit Header(n)
    if( m_rep->msgQueueHeader.size()){
      while (m_rep->msgQueueHeader.readBack()) {
        subscriber->addHeader(m_rep->msgQueueHeader.header,
                              m_rep->msgQueueHeader.in_streamVector,
                              m_rep->msgQueueHeader.out_streamVector,
                              m_rep->msgQueueHeader.out_pluginVector,
                              m_rep->msgQueueHeader.func);
      }
      m_rep->msgQueueHeader.clearAllRequests();
    }
    subscriber->start();
  }
  //
  // Reply
  //
  else if (m_rep->transferData.type == MessageQueue::type_Reply) {
    MessageQueueReply *reply;
    reply = MessageQueue::Instance().createReply(id, m_rep->transferData.host,
                                                 m_rep->transferData.portRequest,
                                                 m_rep->transferData.in_streamVector,
                                                 m_rep->transferData.out_streamVector,
                                                 m_rep->transferData.func, lineNo, filename);
    // neuer SocketServer mit Header(n)
    if( m_rep->msgQueueHeader.size()){
      while (m_rep->msgQueueHeader.readBack()) {
        reply->addHeader(m_rep->msgQueueHeader.header,
                              m_rep->msgQueueHeader.in_streamVector,
                              m_rep->msgQueueHeader.out_streamVector,
                              m_rep->msgQueueHeader.out_pluginVector,
                              m_rep->msgQueueHeader.func);
      }
      m_rep->msgQueueHeader.clearAllRequests();
    }
    reply->start();
  }
  //
  // Publish
  //
  else if (m_rep->transferData.type == MessageQueue::type_Publisher) {
    MessageQueue::Instance().createPublisher(id, m_rep->transferData.host,
                                             m_rep->transferData.port, lineNo, filename);
  }
  m_rep->transferData.clear();
  return true;
}

bool InterpreterConfigurator::transferData_setHost( const::std::string &host ){
  m_rep->transferData.host=host;
  return true;
}

bool InterpreterConfigurator::transferData_setHostXfer(){
  m_rep->transferData.hostxfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::transferData_setPort( int port ){
  m_rep->transferData.port=port;
  return true;
}

bool InterpreterConfigurator::transferData_setPortXfer(){
  m_rep->transferData.portxfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::transferData_setPortRequest( int port ){
  m_rep->transferData.portRequest=port;
  return true;
}

bool InterpreterConfigurator::transferData_setPortRequestXfer(){
  m_rep->transferData.portRequestXfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::transferData_setNoDependencies() {
  m_rep->transferData.no_dependencies = true;
  return true;
}

bool InterpreterConfigurator::transferData_setTimeout(int timeout) {
  m_rep->transferData.timeout=timeout;
  return true;
}

bool InterpreterConfigurator::transferData_setHeader( const::std::string &header ){
  m_rep->transferData.header=header;
  return true;
}

bool InterpreterConfigurator::transferData_setOutStream( const::std::string &out_stream, int dependenciesInt ){
  bool dependencies = AppData::Instance().getMessageQueueDependencies(dependenciesInt);
  Stream *s= StreamManager::Instance().getStream( out_stream );
  if ( s == 0 )
    ParserError( compose(_("Undeclared format '%1'."), out_stream) );
  m_rep->transferData.out_streamVector.push_back(s);
  if( dependencies ){
    m_rep->targetStreams.addStream( s );
    m_rep->targetStreams.moveStreamsToOutput();
  }
  return true;
}

bool InterpreterConfigurator::transferData_setInStream( const::std::string &in_stream, int dependenciesInt ){
  bool dependencies = AppData::Instance().getMessageQueueDependencies(dependenciesInt);
  Stream *s= StreamManager::Instance().getStream( in_stream );
  if ( s == 0 )
    ParserError( compose(_("Undeclared format '%1'."), in_stream) );
  m_rep->transferData.in_streamVector.push_back(s);
  if( dependencies ){
    m_rep->targetStreams.addStream( s );
    m_rep->targetStreams.moveStreamsToInput();
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* transferData_addStream --                                                   */
/* --------------------------------------------------------------------------- */

bool InterpreterConfigurator::transferData_addStream( const::std::string &streamId,
                                                      int dependenciesInt ){
  bool dependencies = AppData::Instance().getMessageQueueDependencies(dependenciesInt);
  Stream* stream = StreamManager::Instance().getStream( streamId );
  if( stream == 0 ){
    ParserError( compose(_("Undeclared format '%1'."), streamId) );
  }
  m_rep->transferData.streamVector.push_back( stream );
  if( dependencies ){
    m_rep->targetStreams.addStream( stream );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* transferData_setOutStreams --                                               */
/* --------------------------------------------------------------------------- */

bool InterpreterConfigurator::transferData_setOutStreams() {
  m_rep->transferData.out_streamVector = m_rep->transferData.streamVector;
  m_rep->transferData.streamVector.clear();
  m_rep->targetStreams.moveStreamsToOutput();
  return true;
}

/* --------------------------------------------------------------------------- */
/* transferData_setInStreams --                                                */
/* --------------------------------------------------------------------------- */

bool InterpreterConfigurator::transferData_setInStreams() {
  m_rep->transferData.in_streamVector = m_rep->transferData.streamVector;
  m_rep->transferData.streamVector.clear();
  m_rep->targetStreams.moveStreamsToInput();
  return true;
}

bool InterpreterConfigurator::transferData_addPlugin( const::std::string &pluginId ) {
  Plugin* plugin = GuiManager::Instance().getPlugin( pluginId );
  if( plugin == 0 ) {
    ParserError( compose(_("Cannot find a plugin '%1'."), pluginId) );
  }

  m_rep->transferData.pluginVector.push_back( plugin );
  return true;
}

bool InterpreterConfigurator::transferData_setFunc( const::std::string &funcId ){
  if( (m_rep->transferData.func=JobManager::Instance().getFunction( funcId, true )) == 0 ){
    ParserError( compose(_("Cannot add function '%1'."), funcId) );
  }
  return true;
}

bool InterpreterConfigurator::transferData_setType(MessageQueue::Type type) {
  m_rep -> transferData.type = type;
  return true;
}

bool InterpreterConfigurator::transferData_setMessageQueue( const::std::string &msgQueueId ){
  m_rep->transferData.publisher = MessageQueue::getPublisher(msgQueueId);
  m_rep->transferData.request= MessageQueue::getRequest(msgQueueId);

  if( m_rep->transferData.publisher == 0 &&
      m_rep->transferData.request == 0  ) {
    ParserError( compose(_("Cannot find a message queue '%1'."), msgQueueId) );
  }
  return true;
}

bool InterpreterConfigurator::msgQueueHeader_setOutStreams() {
  m_rep->msgQueueHeader.out_streamVector = m_rep->transferData.streamVector;
  m_rep->transferData.streamVector.clear();
  return true;
}

bool InterpreterConfigurator::msgQueueHeader_setInStreams() {
  m_rep->msgQueueHeader.in_streamVector = m_rep->transferData.streamVector;
  m_rep->transferData.streamVector.clear();
  return true;
}

bool InterpreterConfigurator::msgQueueHeader_setOutPlugins() {
  m_rep->msgQueueHeader.out_pluginVector = m_rep->transferData.pluginVector;
  m_rep->transferData.pluginVector.clear();
  return true;
}

/** add new server request data to request list*/
bool InterpreterConfigurator::msgQueueHeader_append(){
  m_rep->msgQueueHeader.append();
  return true;
}

  /** add header request data*/
bool InterpreterConfigurator::msgQueueHeader_setHeader( const::std::string &header ){
  m_rep->msgQueueHeader.header=header;
  return true;
}

/* --------------------------------------------------------------------------- */
/* msgQueueHeader_setOutStream --                                              */
/* --------------------------------------------------------------------------- */

bool InterpreterConfigurator::msgQueueHeader_setOutStream( const::std::string &out_stream,
                                                           int dependenciesInt ){
  bool dependencies = AppData::Instance().getMessageQueueDependencies(dependenciesInt);
  Stream* stream = StreamManager::Instance().getStream( out_stream );
  if( stream == 0 ){
    ParserError( compose(_("Undeclared response format '%1'."), out_stream) );
  }
  m_rep->msgQueueHeader.out_streamVector.push_back( stream );
  if( dependencies ){
    m_rep->targetStreams.addOutputStream( stream );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* msgQueueHeader_setInStream --                                               */
/* --------------------------------------------------------------------------- */

bool InterpreterConfigurator::msgQueueHeader_setInStream( const::std::string &in_stream,
                                                          int dependenciesInt ){
  bool dependencies = AppData::Instance().getMessageQueueDependencies(dependenciesInt);
  Stream* stream = StreamManager::Instance().getStream( in_stream );
  if( stream == 0 ){
    ParserError( compose(_("Undeclared request format '%1'."), in_stream) );
  }
  m_rep->msgQueueHeader.in_streamVector.push_back( stream );
  if( dependencies ){
    m_rep->targetStreams.addInputStream( stream );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* msgQueueHeader_setPluginId --                                               */
/* --------------------------------------------------------------------------- */

bool InterpreterConfigurator::msgQueueHeader_setPluginId( const::std::string &pluginId ){
  Plugin* plugin = GuiManager::Instance().getPlugin( pluginId );
  if( plugin == 0 ){
    ParserError( compose(_("'%1' is not a PLUGIN."), pluginId) );
  }
  m_rep->msgQueueHeader.out_pluginVector.push_back( plugin );
  return true;
}

/* --------------------------------------------------------------------------- */
/* msgQueueHeader_setFunc --                                                   */
/* --------------------------------------------------------------------------- */

bool InterpreterConfigurator::msgQueueHeader_setFunc( const::std::string &funcId ){
  if( (m_rep->msgQueueHeader.func=JobManager::Instance().getFunction( funcId, true )) == 0 ){
    ParserError( compose(_("Cannot add request function '%1'."), funcId) );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* targets_addInputStream --                                                   */
/* --------------------------------------------------------------------------- */

bool InterpreterConfigurator::targets_addInputStream( const::std::string &in_stream,
                                                      int dependenciesInt ){
  bool dependencies = AppData::Instance().getMessageQueueDependencies(dependenciesInt);
  if( !dependencies ){
    return true;
  }
  Stream* stream = StreamManager::Instance().getStream( in_stream );
  if( stream == 0 ){
    ParserError( compose(_("Undeclared request format '%1'."), in_stream) );
    return false;
  }
  m_rep->targetStreams.addInputStream( stream );
  return true;
}

/* --------------------------------------------------------------------------- */
/* targets_addOutputStream --                                                  */
/* --------------------------------------------------------------------------- */

bool InterpreterConfigurator::targets_addOutputStream( const::std::string &out_stream,
                                                       int dependenciesInt ){
  bool dependencies = AppData::Instance().getMessageQueueDependencies(dependenciesInt);
  if( !dependencies ){
    return true;
  }
  Stream* stream = StreamManager::Instance().getStream( out_stream );
  if( stream == 0 ){
    ParserError( compose(_("Undeclared request format '%1'."), out_stream) );
    return false;
  }
  m_rep->targetStreams.addOutputStream( stream );
  return true;
}

/* --------------------------------------------------------------------------- */
/* targets_clearTargetStreams --                                               */
/* --------------------------------------------------------------------------- */

void InterpreterConfigurator::targets_clearTargetStreams(){
  m_rep->targetStreams.clear();
}

/* --------------------------------------------------------------------------- */
/* targets_setAutoClear --                                                     */
/* --------------------------------------------------------------------------- */

void InterpreterConfigurator::targets_setAutoClear(){
  m_rep->targetStreams.setAutoClear();
}

/* --------------------------------------------------------------------------- */
/* targets_setNoDependencies --                                                */
/* --------------------------------------------------------------------------- */

void InterpreterConfigurator::targets_noDependencies(){
  m_rep->targetStreams.setNoDependencies();
}

/* --------------------------------------------------------------------------- */
/* transferData_setPlugin --                                                   */
/* --------------------------------------------------------------------------- */

bool InterpreterConfigurator::transferData_setPlugin( const::std::string &pluginId) {
   m_rep->transferData.plugin = GuiManager::Instance().getPlugin( pluginId );

  if( m_rep->transferData.plugin == 0 ) {
    ParserError( compose(_("Cannot find a plugin '%1'."), pluginId) );
  }
  return true;
}

//======================================================================//
// - Rest Service
//======================================================================//
void InterpreterConfigurator::opRestServiceCheckPath(){
  if( m_rep->restService.pathXfer == 0 &&
      m_rep->restService.path.empty() &&
      m_rep->restService.pathStream == 0 ) {
    ParserError( _("No PATH defined") );
  }
}

int InterpreterConfigurator::opRestServiceGet(){
  opRestServiceCheckPath();
  int ret = JobManager::Instance().opRestServiceGet( m_rep->function
                                                     , m_rep->restService.path
                                                     , m_rep->restService.pathXfer
                                                     , m_rep->restService.pathStream
                                                     , m_rep->restService.filterStream
                                                     , m_rep->restService.responseStream );
  m_rep->restService.clear();
  return ret;
}

int InterpreterConfigurator::opRestServiceDelete(){
  opRestServiceCheckPath();
  int ret = JobManager::Instance().opRestServiceDelete( m_rep->function
                                                     , m_rep->restService.path
                                                     , m_rep->restService.pathXfer
                                                     , m_rep->restService.pathStream
                                                     , m_rep->restService.filterStream
                                                     , m_rep->restService.responseStream );
  m_rep->restService.clear();
  return ret;
}

int InterpreterConfigurator::opRestServicePut(){
  opRestServiceCheckPath();
  int ret = JobManager::Instance().opRestServicePut( m_rep->function
                                                     , m_rep->restService.path
                                                     , m_rep->restService.pathXfer
                                                     , m_rep->restService.pathStream
                                                     , m_rep->restService.dataStream
                                                     , m_rep->restService.responseStream
                                                     , m_rep->restService.setDbTimestamp );
  m_rep->restService.clear();
  return ret;
}

int InterpreterConfigurator::opRestServicePost(){
  opRestServiceCheckPath();
  int ret = JobManager::Instance().opRestServicePost( m_rep->function
                                                      , m_rep->restService.path
                                                      , m_rep->restService.pathXfer
                                                      , m_rep->restService.pathStream
                                                      , m_rep->restService.filterStream
                                                      , m_rep->restService.dataStream
                                                      , m_rep->restService.responseStream
                                                      , m_rep->restService.setDbTimestamp );
  m_rep->restService.clear();
  return ret;
}

bool InterpreterConfigurator::opRestServiceLogon() {
  int ret = JobManager::Instance().opRestServiceLogon( m_rep->function,
                                                       m_rep->restService.baseUrlXfer,
                                                       m_rep->restService.usernameXfer,
                                                       m_rep->restService.passwordXfer,
                                                       m_rep->restService.messageXfer,
                                                       m_rep->restService.responseStream
                                                       );
  m_rep->restService.clear();
  return ret;
}

bool InterpreterConfigurator::opRestServiceJwtLogon() {
  int ret = JobManager::Instance().opRestServiceJwtLogon( m_rep->function,
							  m_rep->restService.baseUrlXfer,
							  m_rep->restService.jwtXfer,
							  m_rep->restService.messageXfer,
							  m_rep->restService.responseStream
							  );
  m_rep->restService.clear();
  return ret;
}

bool InterpreterConfigurator::opRestServiceLogoff() {
  return JobManager::Instance().opRestServiceLogoff( m_rep->function );
}

bool InterpreterConfigurator::restService_setPath( const::std::string &path ){
  m_rep->restService.path = path;
  return true;
}

bool InterpreterConfigurator::restService_setPathXfer(){
  if( m_rep->dataitem->Data()->getDataType() != DataDictionary::type_String ){
    ParserError( compose(_("'%1' is not of type STRING."), m_rep->dataitem->getFullName(true)) );
  }
  m_rep->restService.pathXfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::restService_setPathStream( const::std::string &pathStream ){
  if( (m_rep->restService.pathStream = StreamManager::Instance().getStream( pathStream )) == 0 ){
    ParserError( compose(_("Undeclared format '%1'."), pathStream) );
  }
  return true;
}

bool InterpreterConfigurator::restService_setPathStream( ) {
  m_rep->restService.pathStream = m_rep->stream;
  return true;
}

bool InterpreterConfigurator::restService_setFilterStream( const::std::string &filterStream ){
  if( (m_rep->restService.filterStream = StreamManager::Instance().getStream( filterStream )) == 0 ){
    ParserError( compose(_("Undeclared format '%1'."), filterStream) );
  }
  return true;
}

bool InterpreterConfigurator::restService_setFilterStream(){
  m_rep->restService.filterStream = m_rep->stream;
  return true;
}

bool InterpreterConfigurator::restService_setDataStream( const::std::string &dataStream ){
  if( (m_rep->restService.dataStream = StreamManager::Instance().getStream( dataStream )) == 0 ){
    ParserError( compose(_("Undeclared format '%1'."), dataStream) );
  }
  return true;
}

bool InterpreterConfigurator::restService_setDataStream(){
  m_rep->restService.dataStream = m_rep->stream;
  return true;
}

bool InterpreterConfigurator::restService_setResponseStream( const::std::string &responseStream ){
  if( (m_rep->restService.responseStream = StreamManager::Instance().getStream( responseStream )) == 0 ){
    ParserError( compose(_("Undeclared format '%1'."), responseStream) );
  }
  return true;
}

bool InterpreterConfigurator::restService_setResponseStream(){
  m_rep->restService.responseStream = m_rep->stream;
  return true;
}

bool InterpreterConfigurator::restService_setSetDbTimestamp(){
  m_rep->restService.setDbTimestamp = true;
  return true;
}

bool InterpreterConfigurator::restService_setBaseUrlXfer(){
  if( m_rep->dataitem->Data()->getDataType() != DataDictionary::type_String ){
    ParserError( compose(_("'%1' is not of type STRING."), m_rep->dataitem->getFullName(true)) );
  }
  m_rep->restService.baseUrlXfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::restService_setUsernameXfer(){
  if( m_rep->dataitem->Data()->getDataType() != DataDictionary::type_String ){
    ParserError( compose(_("'%1' is not of type STRING."), m_rep->dataitem->getFullName(true)) );
  }
  m_rep->restService.usernameXfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::restService_setPasswordXfer(){
  if( m_rep->dataitem->Data()->getDataType() != DataDictionary::type_String ){
    ParserError( compose(_("'%1' is not of type STRING."), m_rep->dataitem->getFullName(true)) );
  }
  m_rep->restService.passwordXfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::restService_setMessageXfer(){
  if( m_rep->dataitem->Data()->getDataType() != DataDictionary::type_String ){
    ParserError( compose(_("'%1' is not of type STRING."), m_rep->dataitem->getFullName(true)) );
  }
  m_rep->restService.messageXfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::restService_setJwtXfer(){
  if( m_rep->dataitem->Data()->getDataType() != DataDictionary::type_String ){
    ParserError( compose(_("'%1' is not of type STRING."), m_rep->dataitem->getFullName(true)) );
  }
  m_rep->restService.jwtXfer=m_rep->dataitem;
  return true;
}

//======================================================================//
// - newSocket
//======================================================================//
bool InterpreterConfigurator::newSocket( const std::string& id ){
  if( !IdManager::Instance().registerId( id, IdManager::id_Socket ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), id) );
  }

  if( m_rep->socket.in_stream == 0 ){
    //        ParserError( _("No stream defined") );
  }

  if( m_rep->socket.port == -1 ){
    ParserError( _("No port defined") );
  }
  if( m_rep->socket.MFM ){
    if( m_rep->socket.xfer == 0 ){
      ParserError( _("MFMSocket needs a dataitem") );
    }
    new MFMServerSocket( id
                         , m_rep->socket.port
                         , m_rep->socket.in_stream
                         , m_rep->socket.func
                         , m_rep->socket.on_eos
                         , m_rep->socket.xfer
                         , m_rep->socket.thumb_xfer
                         , m_rep->socket.thumb_width
                         , m_rep->socket.thumb_height );
  }
  else{
    if( m_rep->socket.xfer != 0 ){
      ParserError( _("Only MFMSocket supports dataitem") );
    }
    if( m_rep->socket.on_eos != 0 ){
      ParserError( _("Only MFMSocket supports onEos") );
    }
    if( m_rep->socket.thumb_xfer != 0 ){
      ParserError( _("Only MFMSocket supports Thumbnail") );
    }

    // neuer SocketServer mit Header(n)
    if( m_rep->socketRequest.size()){
      IntensServerSocket * server  = new IntensServerSocket( id, m_rep->socket.port );
      while (m_rep->socketRequest.readBack()) {
        server->setRequestTypeData(m_rep->socketRequest.header, m_rep->socketRequest.in_stream,
                                   m_rep->socketRequest.out_stream, m_rep->socketRequest.func);
      }
      m_rep->socketRequest.clearAllRequests();
    } else
      new IntensServerSocket( id, m_rep->socket.port, m_rep->socket.in_stream, m_rep->socket.func );
  }
  m_rep->socket.clear();
  return true;
}

bool InterpreterConfigurator::socket_setFunc( const::std::string &funcId ){
  if( (m_rep->socket.func=JobManager::Instance().getFunction( funcId, true )) == 0 ){
    ParserError( compose(_("Cannot add function '%1'."), funcId) );
  }
  return true;
}


bool InterpreterConfigurator::socket_setOnEos( const::std::string &funcId ){
  if( (m_rep->socket.on_eos=JobManager::Instance().getFunction( funcId, true )) == 0 ){
    ParserError( compose(_("Cannot add function '%1'."), funcId) );
  }
  return true;
}

bool InterpreterConfigurator::socket_setOnViewAction( const::std::string &funcId ){
  JobFunction *f=JobManager::Instance().getFunction( funcId, true );
  if( f==0 ){
    ParserError( compose(_("Cannot add function '%1'."), funcId) );
  }
  GuiImage::setOnViewAction( f );
  return true;
}

bool InterpreterConfigurator::socket_setDataref( XferDataItem *xfer ){
  if( xfer->getNumOfWildcards() < 3 ){
    ParserError( _("Socket dataitem needs three wildcards") );
  }
  m_rep->socket.xfer=xfer;
  return true;
}

bool InterpreterConfigurator::socket_setThumbnail( XferDataItem *xfer, const int width, const int height ){
  if( xfer->getNumOfWildcards() < 3 ){
    ParserError( _("Socket thumbnail needs three wildcards") );
  }
  if ( width < 1 || width > 1000 ) {
    ParserError( _("Socket thumbnail width must be between 1 and 1000") );
  }
  if ( height < 1 || height > 1000 ) {
    ParserError( _("Socket thumbnail height must be between 1 and 1000") );
  }
  m_rep->socket.thumb_xfer=xfer;
  m_rep->socket.thumb_width=width;
  m_rep->socket.thumb_height=height;
  return true;
}

bool InterpreterConfigurator::socket_setMFM(){
  m_rep->socket.MFM=true;
  return true;
}

//======================================================================//
// - newTimer
//======================================================================//
bool InterpreterConfigurator::newTimer( const std::string& id ){
  if( !IdManager::Instance().registerId( id, IdManager::id_Timer ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), id) );
  }
  if( m_rep->function == 0 ){
    ParserError( _("Need Function Parameter.") );
  }
  new TimerFunction(id, m_rep->function, m_rep->timer_maxPendingFunctions);
  m_rep->timer_maxPendingFunctions = 0;
  return true;
}

bool InterpreterConfigurator::timer_setMaxPendingFunctions( int nMax ){
  m_rep->timer_maxPendingFunctions = nMax;
  return true;
}

bool InterpreterConfigurator::timer_setTimer( const::std::string &timer ){
  if( (m_rep->timer=TimerFunction::getTimer( timer )) == 0 ){
    ParserError( compose(_("Cannot set timer function '%1'."), timer) );
  }
  return true;
}

bool InterpreterConfigurator::timer_setPeriod( double p ){
  m_rep->timerFunc.period=p;
  return true;
}

bool InterpreterConfigurator::timer_setPeriodXfer(){
  m_rep->timerFunc.periodxfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::timer_setDelay( int d ){
  m_rep->timerFunc.delay=d;
  return true;
}

bool InterpreterConfigurator::timer_setDelayXfer(){
  m_rep->timerFunc.delayxfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::opTimerStart(){
  int ret =  JobManager::Instance().opTimerStart(
                                                 m_rep->function
                                                 , m_rep->timer
                                                 , m_rep->timerFunc.period
                                                 , m_rep->timerFunc.periodxfer
                                                 , m_rep->timerFunc.delay
                                                 , m_rep->timerFunc.delayxfer);
  m_rep->timerFunc.clear();
  return true;
}

bool InterpreterConfigurator::opTimerStop(){
  int ret =  JobManager::Instance().opTimerStop(
                                                m_rep->function
                                                , m_rep->timer);
  return true;
}


//======================================================================//
// - setUiUpdate
//======================================================================//
bool InterpreterConfigurator::setUiUpdate( int interval ){
  if( m_rep->processgroup != 0 )
    m_rep->processgroup->setUiUpdate( interval );
  return true;
}

/* --------------------------------------------------------------------------- */
/* PROCESS                                                                     */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newBatchProcess
//======================================================================//
bool InterpreterConfigurator::newBatchProcess( const std::string &processId, const std::string &cmd ){
  if( !IdManager::Instance().registerId( processId, IdManager::id_Process ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), processId) );
  }
  if ( (m_rep->process = new BatchProcess( processId ) ) == 0 ){
    ParserError( _("Cannot change process type.") );
  }
  if ( !m_rep->process->setExecCmd( cmd ) ) {
    ParserError( _("Cannot set process.") );
  }
  return true;
}
//======================================================================//
// - newDaemonProcess
//======================================================================//
bool InterpreterConfigurator::newDaemonProcess( const std::string &processId, const std::string &cmd ){
  if( !IdManager::Instance().registerId( processId, IdManager::id_Process ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), processId) );
  }
  if ( (m_rep->process = new BatchProcess( processId, true ) ) == 0 ){
    ParserError( _("Cannot change process type.") );
  }
  if ( !m_rep->process->setExecCmd( cmd ) ) {
    ParserError( _("Cannot set process.") );
  }
  return true;
}
//======================================================================//
// - newMatlabProcess
//======================================================================//
bool InterpreterConfigurator::newMatlabProcess( const std::string &processId ){
  if( !IdManager::Instance().registerId( processId, IdManager::id_Process ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), processId) );
  }
  if ( (m_rep->process = new MatlabProcess( processId ) ) == 0 )
    ParserError( _("Cannot change process type.") );
  return true;
}
//======================================================================//
// - fifoCreateName
//======================================================================//
std::string *InterpreterConfigurator::fifoCreateName( const std::string &fifoName ){
#ifndef __MINGW32__
  return Fifo::createName( fifoName );
#else
  BUG_ERROR( "InterpreterConfigurator::fifoCreateName not yet implemented '" << fifoName << "'");
#endif
  return new std::string(fifoName);
}

/* --------------------------------------------------------------------------- */
/* PROCESSGROUP                                                                */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newProcessGroup
//======================================================================//
bool InterpreterConfigurator::newProcessGroup( const std::string &processgroupId ){
  m_rep->processgroup_dependencies = true;
  m_rep->processgroup_autoclear_dependencies = false;
  if( !IdManager::Instance().registerId( processgroupId, IdManager::id_ProcessGroup ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), processgroupId) );
  }
  if( (m_rep->processgroup = new ProcessGroup( processgroupId )) == 0 )
    ParserError( compose(_("Cannot add processgroup '%1'."), processgroupId) );
  return true;
}
//======================================================================//
// - addProcessActionButton
//======================================================================//
bool InterpreterConfigurator::addProcessActionButton(){
  UImanager::Instance().addProcessActionButton( m_rep->processgroup );
  return true;
}
//======================================================================//
// - processgroupSetLabel
//======================================================================//
bool InterpreterConfigurator::processgroupSetLabel( const std::string &labeltext ){
  m_rep->processgroup->setLabel( labeltext );
  return true;
}
//======================================================================//
// - processgroupUnsetForm
//======================================================================//
bool InterpreterConfigurator::processgroupUnsetForm(){
  m_rep->processgroup->unsetForm();
  return true;
}
//======================================================================//
// - processgroupSetForm
//======================================================================//
bool InterpreterConfigurator::processgroupSetForm( const std::string &formId ){
  m_rep->processgroup->setForm( formId );
  return true;
}
//======================================================================//
// - processgroupSetMenuInstalled
//======================================================================//
bool InterpreterConfigurator::processgroupSetMenuInstalled(){
  m_rep->processgroup->setMenuInstalled();
  return true;
}
//======================================================================//
// - processgroupSetHelptext
//======================================================================//
bool InterpreterConfigurator::processgroupSetHelptext( const std::string &helptext ){
  m_rep->processgroup->setHelptext( helptext );
  return true;
}
//======================================================================//
// - processgroupSetSilent
//======================================================================//
bool InterpreterConfigurator::processgroupSetSilent(){
  m_rep->processgroup->setSilent();
  return true;
}
//======================================================================//
// - processgroupSetLogOff
//======================================================================//
bool InterpreterConfigurator::processgroupSetLogOff(){
  m_rep->processgroup->setLogOff();
  return true;
}
//======================================================================//
// - processgroupNoDependencies
//======================================================================//
bool InterpreterConfigurator::processgroupNoDependencies(){
  m_rep->processgroup_dependencies = false;
  return true;
}
//======================================================================//
// - processgroupNoDependencies
//======================================================================//
bool InterpreterConfigurator::processgroupAutoClearDependencies(){
  m_rep->processgroup_autoclear_dependencies = true;
  return true;
}
//======================================================================//
// - setOutputBasicStreams
//======================================================================//
bool InterpreterConfigurator::setOutputBasicStreams( const std::string &processId ){
  if ( (m_rep->process = m_rep->processgroup-> appendProcess( processId )) == 0 ){
    ParserError( _("Undeclared process.") );
  }
  m_rep->process->setOutputBasicStreams();
  m_rep->stream = 0;
  return true;
}
//======================================================================//
// - setTargets
//======================================================================//
bool InterpreterConfigurator::setTargets(){
  m_rep->stream = 0;
  if ( m_rep->processgroup_dependencies) {
    m_rep->process->setTargets( m_rep->processgroup_autoclear_dependencies );
  }
  return true;
}
//======================================================================//
// - newStream
//======================================================================//
bool InterpreterConfigurator::newStream(){
  m_rep->stream=StreamManager::Instance().newStream();
  stApplyOptions();
  return true;
}
//======================================================================//
// - registerBasicStream
//======================================================================//
bool InterpreterConfigurator::registerBasicStream(){
  Process::registerBasicStream( m_rep->stream, "" );
  return true;
}
//======================================================================//
// - getStream
//======================================================================//
bool InterpreterConfigurator::getStream( const std::string &streamId, const std::string &fifoname ){
  if( (m_rep->stream = StreamManager::Instance().getStream( streamId )) == 0 ){
    ParserError( compose(_("Undeclared format '%1'."), streamId) );
  }
  if( fifoname.empty() ){
    m_rep->stream->setTextWindow( GuiFactory::Instance()->getStandardWindow() );
  }
  Process::registerBasicStream( m_rep->stream, fifoname );
  return true;
}
//======================================================================//
// - getStreamableGuiElement
//======================================================================//
bool InterpreterConfigurator::getStreamableGuiElement( const std::string &guiElemId ){
  m_rep->element = GuiElement::findElement( guiElemId );
  if( m_rep->element == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), guiElemId) );
  }
  if( m_rep->element->streamableObject() == 0 ){
    ParserError( compose(_("GuiElement is not streamable, identifier: '%1'."), guiElemId) );
  }

  Process::registerBasicStream( m_rep->element->streamableObject(), "" );
  return true;
}
//======================================================================//
// - getSimpelPlot
//======================================================================//
bool InterpreterConfigurator::getSimpelPlot( const std::string &uniplotId ){
  m_rep->simpel = m_rep->guifactory->getSimpelPlot( uniplotId );
  if( m_rep->simpel == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), uniplotId) );
  }
  Process::registerBasicStream( m_rep->simpel->getBasicStream(), "" );
  return true;
}
//======================================================================//
// - getPSStream
//======================================================================//
bool InterpreterConfigurator::getPSStream( const std::string &psplotId ){
  m_rep->psstream = StreamManager::Instance().getPSStream( psplotId );
  if(m_rep->psstream == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), psplotId) );
  }
  Process::registerBasicStream( m_rep->psstream, "" );
  return true;
}
//======================================================================//
// - setTextWindow
//======================================================================//
bool InterpreterConfigurator::setTextWindow( const std::string &windowname ){
  if( windowname.size() > 0 ){
    m_rep->scrolledtext = GuiElement::getScrolledText( windowname );
    if( m_rep->scrolledtext == 0 )
      ParserError( compose(_("Undeclared format '%1'."), windowname) );
  }
  else{
    m_rep->scrolledtext = 0;
  }
  m_rep->stream->setTextWindow( m_rep->scrolledtext );
  return true;
}
//======================================================================//
// - addInputStream
//======================================================================//
bool InterpreterConfigurator::addInputStream( const std::string &streamId, const std::string &fifoname ){
  switch( m_rep->process -> addInputStream( streamId, fifoname ) ){
  case Process::O_STREAM_UNDECLARED:
    ParserError( compose(_("Undeclared format '%1'."), streamId) );
    break;
  case Process::O_FIFO_IS_LOCKED:
    ParserError( compose(_("FIFO '%1' already exists."), fifoname) );
    break;
  default:
    break;
  }
  return true;
}
//======================================================================//
// - addNewInputStream
//======================================================================//
bool InterpreterConfigurator::addNewInputStream(){
  m_rep->stream = StreamManager::Instance().newStream();
  m_rep->process->addInputStream( m_rep->stream, "" );
  return true;
}
//======================================================================//
// - getStandardWindow
//======================================================================//
bool InterpreterConfigurator::getStandardWindow(){
  m_rep->scrolledtext = GuiFactory::Instance()->getStandardWindow();
  return true;
}
//======================================================================//
// - resetScrolledtext
//======================================================================//
bool InterpreterConfigurator::resetScrolledtext(){
  m_rep->scrolledtext = 0;
  return true;
}

/* --------------------------------------------------------------------------- */
/* FILESTREAM    OLD                                                           */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - opOLDfilestream
//======================================================================//
bool InterpreterConfigurator::opOLDfilestream( const std::string &streamId ){
  if( (m_rep->stream = StreamManager::Instance().getStream( streamId )) == NULL ){
    ParserError( compose(_("Undeclared format '%1'."), streamId) );
  }
  m_rep->filestream = FileStream::newFileStream( streamId, m_rep->stream );
  assert( m_rep->filestream != 0 );
  return true;
}
//======================================================================//
// - opOLDfilestreamOptions
//======================================================================//
bool InterpreterConfigurator::opOLDfilestreamOptions(){
  if( !m_rep->filestream->install() )
    ParserError( compose(_("Duplicate FILESTREAM '%1' found."), m_rep->filestream->Name()) );
  return true;
}

/* --------------------------------------------------------------------------- */
/* REPORTSTREAM  OLD                                                           */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - opOLDreportstream
//======================================================================//
bool InterpreterConfigurator::opOLDreportstream( const std::string &reportstreamId ){
  if( (m_rep->stream = StreamManager::Instance().getStream( reportstreamId )) == NULL )
    ParserError( compose(_("Undeclared format '%1'."), reportstreamId) );
  m_rep->reportstream = new ReportStream( reportstreamId, m_rep->stream );
  assert( m_rep->reportstream != 0 );
  return true;
}

/* --------------------------------------------------------------------------- */
/* FILESTREAM                                                                  */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newFileStream
//======================================================================//
bool InterpreterConfigurator::newFileStream( const std::string &fstreamId, const std::string &streamId ){
  if( !IdManager::Instance().registerId( fstreamId, IdManager::id_Filestream ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), fstreamId) );
  }
  if( (m_rep->stream = StreamManager::Instance().getStream( streamId )) == NULL ){
    ParserError( compose(_("Undeclared format '%1'."), streamId) );
  }
  m_rep->filestream = FileStream::newFileStream( fstreamId, m_rep->stream );
  return true;
}
//======================================================================//
// - filestreamInstall
//======================================================================//
bool InterpreterConfigurator::filestreamInstall(){
  if( !m_rep->filestream -> install() )
    ParserError( compose(_("Duplicate FILESTREAM '%1' found."), m_rep->filestream->Name()) );
  return true;
}
//======================================================================//
// - installReportStream
//======================================================================//
bool InterpreterConfigurator::installReportStream(){
  if( !m_rep->reportstream -> install() )
    ParserError( compose(_("Duplicate reportstream '%1' found."), m_rep->reportstream->getName()) );
  return true;
}

//======================================================================//
// - filestreamSetLabel
//======================================================================//
bool InterpreterConfigurator::filestreamSetLabel( const std::string &labelname ){
  m_rep->filestream->setLabel( labelname );
  return true;
}

//======================================================================//
// - filestream_SetDirname
//======================================================================//
bool  InterpreterConfigurator::filestream_SetDirname( const std::string &dirname ){
  if( m_rep->filestream == 0 )
    ParserError( compose(_("Cannot set dirname '%1'."), dirname) );
  m_rep->filestream->setDirname( dirname );
  return true;
}


//======================================================================//
// - setFilter
//======================================================================//
bool InterpreterConfigurator::setFilter( const std::string &filtername ){
  if( m_rep->filestream == 0 )
    ParserError( compose(_("Cannot set filter '%1'."), filtername) );
  m_rep->filestream->setFilter( filtername );
  return true;
}
//======================================================================//
// - setProcess
//======================================================================//
bool InterpreterConfigurator::setProcess( const std::string &processname ){
  if( m_rep->filestream == 0 )
    ParserError( _("Cannot set stream label.") );
  if ( !m_rep->filestream->setProcess( processname ) ) {
    ParserError( _("Cannot set stream label.") );
  }
  return true;
}
//======================================================================//
// - setReset
//======================================================================//
bool InterpreterConfigurator::setReset(){
  m_rep->filestream -> setReset( );
  return true;
}
//======================================================================//
// - setReadOnly
//======================================================================//
bool InterpreterConfigurator::setReadOnly(){
  m_rep->filestream->setReadOnly();
  return true;
}
//======================================================================//
// - setWriteOnly
//======================================================================//
bool InterpreterConfigurator::setWriteOnly(){
  m_rep->filestream->setWriteOnly( );
  return true;
}
//======================================================================//
// - filestreamSetLogOff
//======================================================================//
bool InterpreterConfigurator::filestreamSetLogOff(){
  m_rep->filestream->setLogOff();
  return true;
}
//======================================================================//
// - setHidden
//======================================================================//
bool InterpreterConfigurator::setHidden(){
  m_rep->filestream->setHidden();
  return true;
}
//======================================================================//
// - setBinaryMode
//======================================================================//
bool InterpreterConfigurator::setBinaryMode(){
  m_rep->filestream->setBinaryMode();
  return true;
}
//======================================================================//
// - setExtension
//======================================================================//
bool InterpreterConfigurator::setExtension( const std::string &extension ){
  m_rep->filestream->setExtension( extension );
  return true;
}

/* --------------------------------------------------------------------------- */
/* REPORTSTREAM                                                                */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newReportStream
//======================================================================//
bool InterpreterConfigurator::newReportStream( const std::string &rstreamId, const std::string &streamId ){
  if( !IdManager::Instance().registerId( rstreamId, IdManager::id_Reportstream ) ){
    ParserError( compose(_("Identifier '%1' is already declared."), rstreamId) );
  }
  if( (m_rep->stream=StreamManager::Instance().getStream( streamId )) == 0 )
    ParserError( compose(_("Undeclared format '%1'."), streamId) );
  m_rep->reportstream = new ReportStream( rstreamId, m_rep->stream );
  assert( m_rep->reportstream != 0 );
  return true;
}

//======================================================================//
// - reportstream_SetDirname
//======================================================================//
bool  InterpreterConfigurator::reportstream_SetDirname( const std::string &dirname ){
  m_rep->reportstream->setDirname( dirname );
  return true;
}

//======================================================================//
// - setLatexFlag
//======================================================================//
bool InterpreterConfigurator::setLatexFlag(){
  m_rep->reportstream->setLatexFlag();
  return true;
}
//======================================================================//
// - setPrintFilter
//======================================================================//
bool InterpreterConfigurator::setPrintFilter( const std::string &filterId){
  m_rep->reportstream->setPrintFilter( filterId );
  return true;
}
//======================================================================//
// - setXmlFileFormat
//======================================================================//
bool InterpreterConfigurator::setXmlFileFormat(){
  m_rep->reportstream->setXmlFileFormat();
  return true;
}
//======================================================================//
// - setXslFileName
//======================================================================//
bool InterpreterConfigurator::setXslFileName( const std::string &xslFile){
  m_rep->reportstream->setXslFileName( xslFile );
  return true;
}
//======================================================================//
// - setTemplateFileName
//======================================================================//
bool InterpreterConfigurator::setTemplateFileName( const std::string &templateFile){
  m_rep->reportstream->setTemplateFileName( templateFile );
  return true;
}

/* --------------------------------------------------------------------------- */
/* OP Process Menu Definitions                                                 */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - addProcessMenuButton
//======================================================================//
bool InterpreterConfigurator::addProcessMenuButton( const std::string &processgroupId, const std::string &labeltext){
  if( !UImanager::Instance().addProcessMenuButton( m_rep->container, processgroupId, labeltext ) ){
    ParserError( compose(_("Undeclared processgroup '%1'."), processgroupId) );
  }
  return true;
}
//======================================================================//
// - addTaskMenuButton
//======================================================================//
bool InterpreterConfigurator::addTaskMenuButton( const std::string &taskId, const std::string &labeltext){
  if( !UImanager::Instance().addTaskMenuButton( m_rep->container, taskId, labeltext ) ){
    ParserError( compose(_("Undeclared processgroup '%1'."), taskId) );
  }
  return true;
}
//**********************************************************************//
// END OPERATOR
//**********************************************************************//

//**********************************************************************//
// JOB
//**********************************************************************//
/* --------------------------------------------------------------------------- */
/* Function Definitions                                                        */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newFunction
//======================================================================//
bool InterpreterConfigurator::newFunction( const std::string &id, bool registerId, bool setNamespace ){
  if( registerId ){
    if( !IdManager::Instance().registerId( id, IdManager::id_Function ) ){
      ParserError( compose(_("Identifier '%1' is already declared."), id) );
      return false;
    }
  }
  if( (m_rep->function=JobManager::Instance().newFunction( id )) == 0 ){
    ParserError( compose(_("Cannot add function '%1'."), id) );
    return false;
  }
  if (AppData::Instance().LspWorker()) {
    auto filename = App::Instance().getFlexer()->getCurrentFilename();
    if (filename.ends_with('"')) {
      filename.pop_back();
    }
    m_rep->function->setLSPFilename(filename);
    int lineNo = PAlineno -(*(App::Instance().getFlexer()->YYText() ) == '\n' || ! *(App::Instance().getFlexer()->YYText()));
    m_rep->function->setLSPLineno(lineNo);
  }
  if( setNamespace ){
    DataPoolIntens::Instance().setNamespace( id );
  }
  if( m_rep -> func_silent )
    m_rep->function->setSilentFunction();
  if( !m_rep -> func_update_forms )
    m_rep->function->setUpdateForms( false );
  if( m_rep -> func_high_priority )
    m_rep->function->setHighPriority();
  if(m_rep -> xfer_web_response) {
    m_rep->function->setWebReplyResultDataItem(m_rep -> xfer_web_response);
    m_rep -> xfer_web_response = 0;
  }
  if(m_rep -> xfer_web_response_proto) {
    m_rep->function->setWebReplyResultDataItemProto();
    m_rep -> xfer_web_response_proto = false;
  }

  dispatchPendingEvents();
  return true;
}

//======================================================================//
// - func_setHighPrority
//======================================================================//
bool InterpreterConfigurator::func_setHighPriority( bool state ){
  m_rep -> func_high_priority = state;
  return true;
}

//======================================================================//
// - setFuncSilent
//======================================================================//
bool InterpreterConfigurator::setFuncSilent( bool silent ){
  m_rep -> func_silent = silent;
  return true;
}

//======================================================================//
// - setFuncUpdateForms
//======================================================================//
bool InterpreterConfigurator::setFuncUpdateForms( bool update ){
  m_rep -> func_update_forms = update;
  return true;
}

//======================================================================//
// - setFuncWebResponse
//======================================================================//
bool InterpreterConfigurator::setFuncWebResponse(XferDataItem *xferResponse){
  m_rep -> xfer_web_response = xferResponse;
  return true;
}

//======================================================================//
// - setFuncWebResponseProto
//======================================================================//
bool InterpreterConfigurator::setFuncWebResponseProto(){
  m_rep -> xfer_web_response_proto = true;
  return true;
}

//======================================================================//
// - opSetCurrentFormByWebApi
//======================================================================//
int InterpreterConfigurator::opSetCurrentFormByWebApi( XferDataItem *xfer ) {
  if (xfer) {
    if( xfer->getDataType() != DataDictionary::type_String ){
      ParserError( _("Parameter is not of type STRING") );
    }
  }
  return JobManager::Instance().opSetCurrentForm( m_rep->function, xfer );
}

//======================================================================//
// - opSend
//======================================================================//
int InterpreterConfigurator::opSend(){
  if( m_rep->socket.portxfer != 0 ){
    if( m_rep->socket.portxfer->Data()->getDataType() != DataDictionary::type_Integer ){
      ParserError( compose(_("'%1' is not of type INTEGER."), m_rep->socket.portxfer->getFullName(true)) );
    }
  }
  else if( m_rep->socket.port == -1 ){
    ParserError( _("No port defined") );
  }

  if( m_rep->socket.hostxfer != 0 ){
    if( m_rep->socket.hostxfer->Data()->getDataType() != DataDictionary::type_String ){
      ParserError( compose(_("'%1' is not of type STRING."), m_rep->socket.hostxfer->getFullName(true)) );
    }
  }

  int ret =  JobManager::Instance().opSend( m_rep->function
                                            , m_rep->socket.host
                                            , m_rep->socket.hostxfer
                                            , m_rep->socket.port
                                            , m_rep->socket.portxfer
                                            , m_rep->socket.header
                                            , m_rep->socket.out_stream
                                            , m_rep->socket.in_stream );
  m_rep->socket.clear();
  return ret;
}

bool InterpreterConfigurator::socket_setHost( const::std::string &host ){
  m_rep->socket.host=host;
  return true;
}

bool InterpreterConfigurator::socket_setHostXfer(){
  m_rep->socket.hostxfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::socket_setPort( int port ){
  m_rep->socket.port=port;
  return true;
}

bool InterpreterConfigurator::socket_setPortXfer(){
  m_rep->socket.portxfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::socket_setHeader( const::std::string &header ){
  m_rep->socket.header=header;
  return true;
}

bool InterpreterConfigurator::socket_setOutStream( const::std::string &out_stream ){
  if( (m_rep->socket.out_stream = StreamManager::Instance().getStream( out_stream )) == 0 ){
    ParserError( compose(_("Undeclared format '%1'."), out_stream) );
  }
  return true;
}

bool InterpreterConfigurator::socket_setInStream( const::std::string &in_stream ){
  if( (m_rep->socket.in_stream = StreamManager::Instance().getStream( in_stream )) == 0 ){
    ParserError( compose(_("Undeclared format '%1'."), in_stream) );
  }
  return true;
}

/** add new server request data to request list*/
bool InterpreterConfigurator::socketRequest_append(){
  m_rep->socketRequest.append();
  return true;
}

  /** add header request data*/
bool InterpreterConfigurator::socketRequest_setHeader( const::std::string &header ){
  m_rep->socketRequest.header=header;
  return true;
}

/** add response stream request data*/
bool InterpreterConfigurator::socketRequest_setOutStream( const::std::string &out_stream ){
  if( (m_rep->socketRequest.out_stream = StreamManager::Instance().getStream( out_stream )) == 0 ){
    ParserError( compose(_("Undeclared response format '%1'."), out_stream) );
  }
  return true;
}

/** add request stream request data*/
bool InterpreterConfigurator::socketRequest_setInStream( const::std::string &in_stream ){
  if( (m_rep->socketRequest.in_stream = StreamManager::Instance().getStream( in_stream )) == 0 ){
    ParserError( compose(_("Undeclared request format '%1'."), in_stream) );
  }
  return true;
}

/** add function request data*/
bool InterpreterConfigurator::socketRequest_setFunc( const::std::string &funcId ){
  if( (m_rep->socketRequest.func=JobManager::Instance().getFunction( funcId, true )) == 0 ){
    ParserError( compose(_("Cannot add request function '%1'."), funcId) );
  }
  return true;
}

//======================================================================//
// - opRequest
//======================================================================//

 int InterpreterConfigurator::opRequest(){
  if( m_rep->transferData.request == 0 && m_rep->transferData.plugin == 0){
    ParserError( _("No MessageQueue or Plugin defined") );
  }
  int ret;
  if( m_rep->transferData.request ){
    ret = JobManager::Instance().opRequest( m_rep->function,
                                            m_rep->transferData.request,
                                            m_rep->transferData.out_streamVector,
                                            m_rep->transferData.in_streamVector,
                                            m_rep->transferData.header,
                                            m_rep->transferData.timeout );
    // set Target Streams
    m_rep->targetStreams.addTargetStreams(m_rep->transferData.no_dependencies);
  }
  else{
    ret =  JobManager::Instance().opRequest( m_rep->function,
                                             m_rep->transferData.plugin,
                                             m_rep->transferData.out_streamVector,
                                             m_rep->transferData.in_streamVector,
                                             m_rep->transferData.header,
                                             m_rep->transferData.timeout );
  }
  m_rep->transferData.clear();
  m_rep->targetStreams.clear();
  return ret;
}

//======================================================================//
// - opPublish
//======================================================================//
int InterpreterConfigurator::opPublish(){
  if( m_rep->transferData.publisher == 0 ){
    ParserError( _("No MessageQueue defined") );
  }
  if( m_rep->transferData.out_streamVector.size() == 0 ){
    ParserError( _("No Publish Stream defined") );
  }
  int ret =  JobManager::Instance().opPublish( m_rep->function
                                               , m_rep->transferData.publisher
                                               , m_rep->transferData.out_streamVector
                                               , m_rep->transferData.header );
  m_rep->transferData.clear();
  return ret;
}

//======================================================================//
// - opSubscribe
//======================================================================//
int InterpreterConfigurator::opSubscribe(){
  if( m_rep->transferData.plugin == 0){
    ParserError( _("No Plugin defined") );
  }
  if( m_rep->transferData.out_streamVector.size() == 0 ||
      m_rep->transferData.header.size() == 0 ) {
    ParserError( _("No Subscribe Header and Stream defined") );
  }
  int ret;
  if ( m_rep->transferData.plugin)
    ret =  JobManager::Instance().opSubscribe( m_rep->function
                                               , m_rep->transferData.plugin
                                               , m_rep->transferData.out_streamVector
                                               , m_rep->transferData.header
                                               , m_rep->transferData.func );
  m_rep->transferData.clear();
  return ret;
}

//======================================================================//
// - opSetMessageQueueHost
//======================================================================//
 int InterpreterConfigurator::opSetMessageQueueHost(){
  return JobManager::Instance().opSetMessageQueueHost( m_rep->function );
}

//======================================================================//
// - getAndResetNamespace
//======================================================================//
bool InterpreterConfigurator::getAndResetNamespace(){
  JobManager::Instance().opEndOfFunction( m_rep->function );
  m_rep->function->setLocalVariables( DataPoolIntens::Instance().getAndResetNamespace() );
  dispatchPendingEvents();
  return true;
}
//======================================================================//
// - getFunction
//======================================================================//
bool InterpreterConfigurator::getFunction( const std::string &funcId ){
  if( (m_rep->function=JobManager::Instance().getFunction( funcId )) == 0 ){
    ParserError( compose(_("Cannot add function '%1'."), funcId) );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* Task Definitions                                                            */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - newTask
//======================================================================//
bool InterpreterConfigurator::newTask( const std::string &id, bool registerId ){
  if( registerId ){
    if( !IdManager::Instance().registerId( id, IdManager::id_Task ) ){
      ParserError( compose(_("Identifier '%1' is already declared."), id) );
    }
  }
  if( (m_rep->task=JobManager::Instance().newTask( id )) == 0 ){
    ParserError( compose(_("Task '%1' already declared."), id) );
  }
  m_rep->function = m_rep->task;
  if (AppData::Instance().LspWorker()) {
    auto filename = App::Instance().getFlexer()->getCurrentFilename();
    if (filename.ends_with('"')) {
      filename.pop_back();
    }
    m_rep->task->setLSPFilename(filename);
    int lineNo = PAlineno -(*(App::Instance().getFlexer()->YYText() ) == '\n' || ! *(App::Instance().getFlexer()->YYText()));
    m_rep->task->setLSPLineno(lineNo);
  }
  return true;
}
//======================================================================//
// - isNOTTaskImplemented
//======================================================================//
bool InterpreterConfigurator::isNOTTaskImplemented(){
  if( m_rep->task->notImplemented() ){
    return true;
  }
  return false;
}
//======================================================================//
// - setNamespace
//======================================================================//
bool InterpreterConfigurator::setNamespace(){
  DataPoolIntens::Instance().setNamespace( m_rep->function->Name() );
  return true;
}
bool InterpreterConfigurator::setNamespace( const std::string &funcId ){
  DataPoolIntens::Instance().setNamespace( funcId );
  return true;
}
//======================================================================//
// - addTaskActionButton
//======================================================================//
bool InterpreterConfigurator::addTaskActionButton(){
  JobManager::Instance().opEndOfFunction( m_rep->function );
  m_rep->function->setLocalVariables( DataPoolIntens::Instance().getAndResetNamespace() );
  UImanager::Instance().addTaskActionButton( m_rep->task );
  return true;
}
//======================================================================//
// - taskSetLabel
//======================================================================//
bool InterpreterConfigurator::taskSetLabel( const std::string &labeltext ){
  m_rep->task->setLabel( labeltext );
  return true;
}
//======================================================================//
// - taskUnsetForm
//======================================================================//
bool InterpreterConfigurator::taskUnsetForm(){
  m_rep->task->unsetForm();
  return true;
}
//======================================================================//
// - taskSetForm
//======================================================================//
bool InterpreterConfigurator::taskSetForm( const std::string &formId ){
  m_rep->task->setForm( formId );
  return true;
}
//======================================================================//
// - taskSetMenuInstalled
//======================================================================//
bool InterpreterConfigurator::taskSetMenuInstalled(){
  m_rep->task->setMenuInstalled();
  return true;
}
//======================================================================//
// - taskSetHelptext
//======================================================================//
bool InterpreterConfigurator::taskSetHelptext( const std::string &helptext ){
  m_rep->task->setHelptext( helptext );
  return true;
}
//======================================================================//
// - taskSetLogOff
//======================================================================//
bool InterpreterConfigurator::taskSetLogOff(){
  m_rep->task->setLogOff();
  return true;
}
//======================================================================//
// - taskSetSilent
//======================================================================//
bool InterpreterConfigurator::taskSetSilent(){
  m_rep->task->setSilent();
  return true;
}

/* --------------------------------------------------------------------------- */
/* Job Data Definition                                                         */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - opPushVariable
//======================================================================//
bool InterpreterConfigurator::opPushVariable(){
  JobManager::Instance().opPushVariable( m_rep->function, m_rep->jobdataref );
  return true;
}
//======================================================================//
// - opSetCycleNumber
//======================================================================//
bool InterpreterConfigurator::opSetCycleNumber(){
  JobManager::Instance().opSetCycleNumber( m_rep->function );
  return true;
}
//======================================================================//
// - opPushNewValue
//======================================================================//
int InterpreterConfigurator::opPushNewValue(){
  return JobManager::Instance().opPushNewValue( m_rep->function );
}
//======================================================================//
// - opSetThis
//======================================================================//
bool InterpreterConfigurator::opSetThis(){
  JobManager::Instance().opSetThis( m_rep->function, m_rep->jobdataref );
  JobManager::Instance().opPushVariable( m_rep->function, m_rep->jobdataref );
  return true;
}
//======================================================================//
// - assignFuncIndex
//======================================================================//
bool InterpreterConfigurator::assignFuncIndex(){
  JobManager::Instance().opAssignFuncIndex( m_rep->function );
  return true;
}
//======================================================================//
// - assignFuncReason
//======================================================================//
bool InterpreterConfigurator::assignFuncReason(){
  JobManager::Instance().opAssignFuncReason( m_rep->function );
  return true;
}
//======================================================================//
// - assignFuncArgs
//======================================================================//
bool InterpreterConfigurator::assignFuncArgs(){
  JobManager::Instance().opAssignFuncArgs( m_rep->function );
  return true;
}
//======================================================================//
// - opSetBase
//======================================================================//
bool InterpreterConfigurator::opSetBase(){
  JobManager::Instance().opSetBase( m_rep->function, m_rep->jobdataref );
  JobManager::Instance().opPushVariable( m_rep->function, m_rep->jobdataref );
  return true;
}
//======================================================================//
// - opSetSource
//======================================================================//
bool InterpreterConfigurator::opSetSource(){
  // if ( m_rep->jobdataref2)
  JobManager::Instance().opSetSource( m_rep->function, m_rep->jobdataref, m_rep->jobdataref2 );
  JobManager::Instance().opPushVariable( m_rep->function, m_rep->jobdataref );
  m_rep->jobdataref2 = 0;
  return true;
}
//======================================================================//
// - opSetSource2
//======================================================================//
bool InterpreterConfigurator::opSetSource2(){
  JobManager::Instance().opSetSource2( m_rep->function, m_rep->jobdataref );
  JobManager::Instance().opPushVariable( m_rep->function, m_rep->jobdataref );
  return true;
}
//======================================================================//
// - newJobDataReference
//======================================================================//
bool InterpreterConfigurator::newJobDataReference( const std::string &dataId, bool dataset,
                                                   UserAttr::STRINGtype type ){
  m_rep->jobdataref = new JobDataReference();
  if( m_rep->jobdatarefallowwildcards ){
    m_rep->jobdataref->allowWildcards();
    m_rep->jobdatarefallowwildcards = false;
  }
  m_rep->datalevel = 0;
  if( m_rep->jobdataref->newDataReferenceInNamespace( dataId, dataset ) ){
    m_rep->datalevel = 1;
  }
  else
  if( !m_rep->jobdataref->newDataReference( dataId, dataset ) ){
    ParserError( compose(_("Undeclared identifier '%1'."), dataId) );
  }
  if (type != UserAttr::string_kind_none) {
     m_rep->jobdataref->setStringType(type);
  }
  return true;
}
//======================================================================//
// - newJobDataReference2
//======================================================================//
bool InterpreterConfigurator::newJobDataReference2( const std::string &dataId, bool dataset,
                                                    UserAttr::STRINGtype type ){
  m_rep->jobdataref2 = new JobDataReference();
  // m_rep->datalevel = 0;
  if( m_rep->jobdataref2->newDataReferenceInNamespace( dataId, dataset ) ){
    // m_rep->datalevel = 1;
  }
  else
  if( !m_rep->jobdataref2->newDataReference( dataId, dataset ) ){
    ParserError( compose(_("Undeclared identifier '%1'."), dataId) );
  }
  return true;
}
//======================================================================//
// - newJobVarDataReference
//======================================================================//
bool InterpreterConfigurator::newJobVarDataReference(){
  m_rep->jobdataref = new JobVarDataReference();
  if( m_rep->jobdatarefallowwildcards ){
    m_rep->jobdataref->allowWildcards();
    m_rep->jobdatarefallowwildcards = false;
  }
  m_rep->datalevel = 0;
  return true;
}
//======================================================================//
// - pushDataRef
//======================================================================//
bool InterpreterConfigurator::pushDataRef(){
  JobManager::Instance().pushDataRef( m_rep->jobdataref, m_rep->datalevel );
  return true;
}
//======================================================================//
// - popDataRef
//======================================================================//
bool InterpreterConfigurator::popDataRef(){
  m_rep->jobdataref = JobManager::Instance().popDataRef( m_rep->datalevel );
  return true;
}
//======================================================================//
// - jobdatarefAddVariableName
//======================================================================//
bool InterpreterConfigurator::jobdatarefAddVariableName(bool bParent){
  m_rep->jobdataref = new JobVarDataReference(true, bParent);
  m_rep->datalevel = 0;
  m_rep->jobdataref->addVariableName();
  return true;
}
//======================================================================//
// - incrementDatalevel
//======================================================================//
bool InterpreterConfigurator::incrementDatalevel(){
  m_rep->datalevel++;
  return true;
}
//======================================================================//
// - extendDataReference
//======================================================================//
bool InterpreterConfigurator::extendDataReference( const std::string &dataId ){
  if( m_rep->jobdataref->isVariable() ){
    JobManager::Instance().opPushString( m_rep->function, dataId );
    m_rep->jobdataref->addVariableName();
  }
  else
    if( !m_rep->jobdataref->extendDataReference( dataId ) ){
      ParserError( compose(_("Undeclared structure field '%1'"), dataId) );
    }
  return true;
}
//======================================================================//
// - convertToVar
//======================================================================//
bool InterpreterConfigurator::convertToVar(){
  m_rep->jobdataref->addVariableName();
  if( !m_rep->jobdataref->isVariable() ){
    m_rep->jobdataref = JobVarDataReference::convertToVar( m_rep->jobdataref );
  } else
    ParserError( _("Multipled VAR's not supported") );
  return true;
}
//======================================================================//
// - assignThis
//======================================================================//
bool InterpreterConfigurator::assignThis(){
  JobManager::Instance().opAssignThis( m_rep->function );
  return true;
}
//======================================================================//
// - addIndex
//======================================================================//
bool InterpreterConfigurator::addIndex(){
  m_rep->jobdataref->addIndex( m_rep->datalevel );
  return true;
}

//======================================================================//
// - allowDataWildcards
//======================================================================//
bool InterpreterConfigurator::allowDataWildcards(){
  m_rep->jobdatarefallowwildcards = true;
  return true;
}

//======================================================================//
// - setDataWildcard
//======================================================================//
bool InterpreterConfigurator::setDataWildcard(){
  if( !m_rep->jobdataref->WildcardsAreAllowed() ){
    ParserError( _("WILDCARDS not allowed here.") );
  }
  m_rep->jobdataref->addWildcard( m_rep->datalevel );
  JobManager::Instance().opPushInteger( m_rep->function, -1 );

  return true;
}

/* --------------------------------------------------------------------------- */
/* Statements                                                                  */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - opAssignIndex
//======================================================================//
bool InterpreterConfigurator::opAssignIndex( const std::string &indexId ){
  JobManager::Instance().opAssignIndex( m_rep->function, indexId );
  JobManager::Instance().opActionResult( m_rep->function );
  return true;
}
//======================================================================//
// - opAbort
//======================================================================//
bool InterpreterConfigurator::opAbort(){
  JobManager::Instance().opAbort( m_rep->function );
  //  JobManager::Instance().opPopAndDelete( m_rep->function );
  return true;
}
//======================================================================//
// - opExitIntens
//======================================================================//
bool InterpreterConfigurator::opExitIntens(bool force){
  JobManager::Instance().opExitIntens( m_rep->function, force );
  return true;
}
//======================================================================//
// - opBeep
//======================================================================//
bool InterpreterConfigurator::opBeep(){
  JobManager::Instance().opBeep( m_rep->function );
  return true;
}
//======================================================================//
// - opResetError
//======================================================================//
int InterpreterConfigurator::opResetError(){
  return JobManager::Instance().opResetError( m_rep->function );
}
//======================================================================//
// - opNewCycle
//======================================================================//
bool InterpreterConfigurator::opNewCycle(){
  JobManager::Instance().opNewCycle( m_rep->function );
  return true;
}
//======================================================================//
// - opDeleteCycle
//======================================================================//
int InterpreterConfigurator::opDeleteCycle(){
  return JobManager::Instance().opDeleteCycle( m_rep->function );
}
//======================================================================//
// - opClearCycle
//======================================================================//
int InterpreterConfigurator::opClearCycle(){
  return JobManager::Instance().opClearCycle( m_rep->function );
}
//======================================================================//
// - opPrintLog
//======================================================================//
int InterpreterConfigurator::opPrintLog(){
  return JobManager::Instance().opPrintLog( m_rep->function );
}

//======================================================================//
// - opFirstCycle
//======================================================================//
int InterpreterConfigurator::opFirstCycle(){
  return JobManager::Instance().opFirstCycle( m_rep->function );
}
//======================================================================//
// - opLastCycle
//======================================================================//
int InterpreterConfigurator::opLastCycle(){
  return JobManager::Instance().opLastCycle( m_rep->function );
}
//======================================================================//
// - opNextCycle
//======================================================================//
int InterpreterConfigurator::opNextCycle(){
  return JobManager::Instance().opNextCycle( m_rep->function );
}
//======================================================================//
// - opGoCycle
//======================================================================//
bool InterpreterConfigurator::opGoCycle(){
  JobManager::Instance().opGoCycle( m_rep->function );
  return true;
}
//======================================================================//
// - opGetCycleName
//======================================================================//
int InterpreterConfigurator::opGetCycleName(){
  return JobManager::Instance().opGetCycleName( m_rep->function );
}
//======================================================================//
// - opSetCycleName
//======================================================================//
int  InterpreterConfigurator::opSetCycleName(){
  return JobManager::Instance().opSetCycleName( m_rep->function );
}
//======================================================================//
// - opUpdateForms
//======================================================================//
int InterpreterConfigurator::opUpdateForms(){
  return JobManager::Instance().opUpdateForms( m_rep->function );
}
//======================================================================//
// - opSize
//======================================================================//
bool InterpreterConfigurator::opSize(){
  JobManager::Instance().opSize( m_rep->function );
  return true;
}
//======================================================================//
// - opDataSize
//======================================================================//
bool InterpreterConfigurator::opDataSize(){
  JobManager::Instance().opDataSize( m_rep->function );
  return true;
}
//======================================================================//
// - opIndex
//======================================================================//
bool InterpreterConfigurator::opIndex(){
  JobManager::Instance().opIndex( m_rep->function );
  return true;
}
//======================================================================//
// - opIcon
//======================================================================//
bool InterpreterConfigurator::opIcon(){
  JobManager::Instance().opIcon( m_rep->function );
  return true;
}
//======================================================================//
// - opCompare
//======================================================================//
bool InterpreterConfigurator::opCompare( bool bCycle ){
  JobManager::Instance().opCompare( m_rep->function, bCycle );
  return true;
}
//======================================================================//
// - opCompose
//======================================================================//
bool InterpreterConfigurator::opCompose(){
  JobManager::Instance().opCompose(m_rep->function, m_rep->composeArgumentCount);
  m_rep->composeArgumentCount = 0;
  return true;
}
//======================================================================//
// - opComposeNew
//======================================================================//
bool InterpreterConfigurator::opComposeNew(){
  if(m_rep->composeArgumentCount != 0) {
    ParserError(_("COMPOSE is not allowed inside COMPOSE."));
  }
  return true;
}
//======================================================================//
// - opComposeIncreaseArg
//======================================================================//
bool InterpreterConfigurator::opComposeIncreaseArg(){
  if(m_rep->composeArgumentCount >= 16) {
    ParserError(_("COMPOSE: The maximal number of values is 15."));
  }
  ++m_rep->composeArgumentCount;
  return true;
}
//======================================================================//
// - opAssignConsistency
//======================================================================//
bool InterpreterConfigurator::opAssignConsistency(){
  JobManager::Instance().opAssignConsistency(m_rep->function);
  return true;
}
//======================================================================//
// - opSetResource
//======================================================================//
 bool InterpreterConfigurator::opSetResource( const std::string &key, const std::string &value, bool useValue ){
   JobManager::Instance().opSetResource( m_rep->function, key, value, useValue );
  return true;
}
//======================================================================//
// - opSetGuiFieldgroupRange
//======================================================================//
bool InterpreterConfigurator::opSetGuiFieldgroupRange( const std::string &fieldgroupId ){
  m_rep->fieldgroup = GuiFieldgroup::getFieldgroup( fieldgroupId );
  JobManager::Instance().opSetGuiFieldgroupRange( m_rep->function, m_rep->fieldgroup );
  return true;
}
//======================================================================//
// - opMessageBox
//======================================================================//
bool InterpreterConfigurator::opMessageBox(bool with_title){
  if (with_title) {
    JobManager::Instance().opMessageBoxWithTitle( m_rep->function );
  } else {
    JobManager::Instance().opMessageBox( m_rep->function );
  }
  return true;
}
//======================================================================//
// - opReplaceGuiElement
//======================================================================//
int InterpreterConfigurator::opReplaceGuiElement( const std::string &fgIdOld, const std::string &fgIdNew ){
  if( fgIdOld == fgIdNew ){
    ParserError( compose(_("It makes no sense to replace Fieldgoup '%1' by same fieldgoup '%2'."),fgIdOld,fgIdNew) );
  }
  GuiElement *old_el = GuiElement::findElement( fgIdOld );
  GuiElement *new_el = GuiElement::findElement( fgIdNew );
  return JobManager::Instance().opReplaceGuiElement( m_rep->function, old_el, new_el );
}
//======================================================================//
// - opClass
//======================================================================//
bool InterpreterConfigurator::opClass(const std::string& guiElem, const std::string& className){
  GuiElement *gui_el = GuiElement::findElement(guiElem);
  JobManager::Instance().opClass( m_rep->function, gui_el, className );
  return true;
}
//======================================================================//
// - opGuiElementMethod
//======================================================================//
int InterpreterConfigurator::opGuiElementMethod(std::string guiElem){
  GuiElement *gui_el = GuiElement::findElement(guiElem);
  return JobManager::Instance().opGuiElementMethod( m_rep->function, gui_el);
}
//======================================================================//
// - opEndOfFunction
//======================================================================//
int InterpreterConfigurator::opEndOfFunction(){
  return JobManager::Instance().opEndOfFunction( m_rep->function );
}
//======================================================================//
// - opBeginTransaction
//======================================================================//
int InterpreterConfigurator::opBeginTransaction(){
  return JobManager::Instance().opBeginTransaction( m_rep->function );
}
//======================================================================//
// - opCommitTransaction
//======================================================================//
int InterpreterConfigurator::opCommitTransaction(){
  return JobManager::Instance().opCommitTransaction( m_rep->function );
}
//======================================================================//
// - opAbortTransaction
//======================================================================//
int InterpreterConfigurator::opAbortTransaction(){
  return JobManager::Instance().opAbortTransaction( m_rep->function );
}
//======================================================================//
// - opAssignValue
//======================================================================//
bool InterpreterConfigurator::opAssignValue(){
  JobManager::Instance().opAssignValue( m_rep->function );
  return true;
}
//======================================================================//
// - opIncrementValue
//======================================================================//
bool InterpreterConfigurator::opIncrementValue(){
  JobManager::Instance().opIncrementValue( m_rep->function );
  return true;
}
//======================================================================//
// - opDecrementValue
//======================================================================//
bool InterpreterConfigurator::opDecrementValue(){
  JobManager::Instance().opDecrementValue( m_rep->function );
  return true;
}
//======================================================================//
// - opAccumulateValue
//======================================================================//
bool InterpreterConfigurator::opAccumulateValue(){
  JobManager::Instance().opAccumulateValue( m_rep->function );
  return true;
}

/* --------------------------------------------------------------------------- */
/* RUN                                                                         */
/* --------------------------------------------------------------------------- */
//======================================================================//
// - opActionResult
//======================================================================//
bool InterpreterConfigurator::opActionResult(){
  JobManager::Instance().opActionResult( m_rep->function );
  return true;
}
//======================================================================//
// - opExecuteTask
//======================================================================//
int InterpreterConfigurator::opExecuteTask( const std::string &taskId ){
  return JobManager::Instance().opExecuteTask( m_rep->function, taskId );
}
//======================================================================//
// - opExecuteFunction
//======================================================================//
int InterpreterConfigurator::opExecuteFunction( const std::string &funcId ){
  return JobManager::Instance().opExecuteFunction( m_rep->function, funcId );
}
//======================================================================//
// - opExecuteFunction
//======================================================================//
int InterpreterConfigurator::opExecuteFunction( XferDataItem *xfer ) {
  if (xfer) {
    if( xfer->getDataType() != DataDictionary::type_String ){
      ParserError( _("Parameter is not of type STRING") );
    }
  }
  return JobManager::Instance().opExecuteFunction( m_rep->function, xfer );
}
//======================================================================//
// - opExecuteProcess
//======================================================================//
int InterpreterConfigurator::opExecuteProcess( const std::string &processgroupId ){
  if( (m_rep->processgroup = ProcessGroup::find( processgroupId )) == NULL ){
    ParserError( compose(_("Undeclared processgroup '%1'."), processgroupId) );
  }
  return JobManager::Instance().opExecuteProcess( m_rep->function, m_rep->processgroup );
}
//======================================================================//
// - opPluginInitialise
//======================================================================//
int InterpreterConfigurator::opPluginInitialise( const std::string &pluginId ){
  m_rep->plugin = GuiManager::Instance().getPlugin( pluginId );
  if( m_rep->plugin == 0 ){
    ParserError( compose(_("'%1' is not a PLUGIN."), pluginId) );
  }
  return JobManager::Instance().opPluginInitialise( m_rep->function, m_rep->plugin );
}
//======================================================================//
// - opExecutePlugin
//======================================================================//
bool InterpreterConfigurator::opExecutePlugin(){
  JobManager::Instance().opExecutePlugin( m_rep->function, m_rep->plugin );
  return true;
}
//======================================================================//
// - opPluginParameter
//======================================================================//
bool InterpreterConfigurator::opPluginParameter(){
  JobManager::Instance().opPluginParameter( m_rep->function, m_rep->plugin );
  return true;
}
//======================================================================//
// - opExecuteJobActionOpen
//======================================================================//
int InterpreterConfigurator::opExecuteJobActionOpen( const std::string &filestreamId){
  m_rep->filestream = FileStream::find( filestreamId );
  if( m_rep->filestream == 0 ){
    ParserError( compose(_("Undeclared FILESTREAM '%1'."), filestreamId) );
  }
  // m_rep->jobaction = m_rep->filestream->getOpenAction( xfer );
  if( m_rep->filestream && m_rep->filestream->WriteOnly() ){
	ParserError( compose(_("FILESTREAM '%1' is writeonly."), filestreamId) );
  }
  int ret = JobManager::Instance().opOpenFile( m_rep->function, m_rep->filestream,
											m_rep->fileoption.hasFilenameXfer,
											m_rep->fileoption.filename);
  m_rep->fileoption.clear();
  return ret;
}

//======================================================================//
// - fileOption_setXferFilename
//======================================================================//
int InterpreterConfigurator::fileOption_setXferFilename() {
  if( m_rep->jobdataref->Data().getDataType() != DataDictionary::type_String ){
    ParserError( compose(_("'%1' is not of type STRING."), m_rep->dataitem->getFullName(true)) );
  }
  m_rep->fileoption.hasFilenameXfer = m_rep->jobdataref != 0;
  return true;
}
//======================================================================//
// - fileOption_setFilename
//======================================================================//
int InterpreterConfigurator::fileOption_setFilename(const std::string& filename) {
  m_rep->fileoption.filename = filename;
  return true;
}
//======================================================================//
// - fileOption_setXferBaseFilename
//======================================================================//
int InterpreterConfigurator::fileOption_setXferBaseFilename() {
  if( m_rep->jobdataref->Data().getDataType() != DataDictionary::type_String ){
    ParserError( compose(_("'%1' is not of type STRING."), m_rep->dataitem->getFullName(true)) );
  }
  m_rep->fileoption.hasBaseFilenameXfer = m_rep->jobdataref != 0;
  return true;
}
//======================================================================//
// - fileOption_setBaseFilename
//======================================================================//
int InterpreterConfigurator::fileOption_setBaseFilename(const std::string& filename) {
  m_rep->fileoption.base_filename = filename;
  return true;
}

//======================================================================//
// - opExecuteJobActionSave
//======================================================================//
int InterpreterConfigurator::opExecuteJobActionSave( const std::string &filestreamId){
  GuiElement *el = GuiElement::findElement( filestreamId );
  m_rep->filestream = FileStream::find( filestreamId );
  if( m_rep->filestream == 0 &&  el == 0){
    ParserError( compose(_("Undeclared FILESTREAM '%1'."), filestreamId) );
  }

  // get jobaction
  if( m_rep->filestream && m_rep->filestream->ReadOnly() ){
    ParserError( compose(_("FILESTREAM '%1' is readonly."), filestreamId) );
  }

  // JobManager
  int ret = JobManager::Instance().opSaveFile( m_rep->function, m_rep->filestream, el,
											m_rep->fileoption.hasFilenameXfer,
                      m_rep->fileoption.filename,
											m_rep->fileoption.hasBaseFilenameXfer,
											m_rep->fileoption.base_filename);
  m_rep->fileoption.clear();
  return ret;
}

//======================================================================//
// - opGetFileName
//======================================================================//
int InterpreterConfigurator::opGetFileName( const std::string &filestreamId ){
  m_rep->filestream = FileStream::find( filestreamId );
  if( m_rep->filestream == 0 ){
    ParserError( compose(_("Undeclared FILESTREAM '%1' for %2."), filestreamId, "FILENAME") );
  }
  m_rep -> dataref = m_rep->filestream->getDataFilename();
  assert( m_rep->dataref != 0 );
  m_rep->jobdataref = new JobDataReference();
  m_rep->jobdataref->newDataReference( m_rep -> dataref );
  opPushVariable( );
  return   JobManager::Instance().opGetFileName( m_rep->function );
}
//======================================================================//
// - opGetBaseName
//======================================================================//
int InterpreterConfigurator::opGetBaseName( const std::string &filestreamId ){
  if( filestreamId.empty() ){
    m_rep -> dataref = getDataItem()->Data();
  }
  else {
    m_rep->filestream = FileStream::find( filestreamId );
    if( m_rep->filestream == 0 ){
      ParserError( compose(_("Undeclared FILESTREAM '%1' for %2."), filestreamId, "BASENAME") );
    }
    m_rep -> dataref = m_rep->filestream->getDataFilename();
  }
  assert( m_rep->dataref != 0 );
  m_rep->jobdataref = new JobDataReference();
  m_rep->jobdataref->newDataReference( m_rep -> dataref );
  opPushVariable( );
  return   JobManager::Instance().opGetBaseName( m_rep->function );
}
//======================================================================//
// - opGetDirName
//======================================================================//
int InterpreterConfigurator::opGetDirName( const std::string &filestreamId ){
  if( filestreamId.empty() ){
    m_rep -> dataref = getDataItem()->Data();
    //std::cerr << "dirname " << getDataItem()->getFullName(false) << std::endl;
  }
  else {
    m_rep->filestream = FileStream::find( filestreamId );
    if( m_rep->filestream == 0 ){
      ParserError( compose(_("Undeclared FILESTREAM '%1' for %2."), filestreamId, "DIRNAME") );
    }
    m_rep -> dataref = m_rep->filestream->getDataFilename();
  }
  assert( m_rep->dataref != 0 );
  m_rep->jobdataref = new JobDataReference();
  m_rep->jobdataref->newDataReference( m_rep -> dataref );
  opPushVariable();
  return   JobManager::Instance().opGetDirName( m_rep->function );
}

//======================================================================//
// - opFileDialog
//======================================================================//
int InterpreterConfigurator::opFileDialog( bool dironly ){
  if( m_rep->filedialog.dirnamexfer != 0 ) {
    if( m_rep->filedialog.dirnamexfer->Data()->getDataType() != DataDictionary::type_String ){
      ParserError( compose(_("'%1' is not of type STRING."), m_rep->filedialog.dirnamexfer->getFullName(true)) );
    }
  }
  int i = JobManager::Instance().opFileDialog( m_rep->function
                                               , m_rep->filedialog.dirname
                                               , m_rep->filedialog.filtername
                                               , dironly
                                               , m_rep->filedialog.openMode
                                               , m_rep->filedialog.dirnamexfer );
  m_rep->filedialog.clear();
  return i;
}
bool InterpreterConfigurator::opFileDialog_setDirname( const::std::string &dirname ){
  m_rep->filedialog.dirname=dirname;
  return true;
}
bool InterpreterConfigurator::opFileDialog_setFilter( const::std::string &filtername ){
  m_rep->filedialog.filtername=filtername;
  return true;
}

bool InterpreterConfigurator::opFileDialog_setDirnameXfer(){
  m_rep->filedialog.dirnamexfer=m_rep->dataitem;
  return true;
}

bool InterpreterConfigurator::opFileDialog_setOpenMode( bool open ){
  m_rep->filedialog.openMode = open;
  return true;
}
//======================================================================//
// - opExecuteJobActionXMLXPath
//======================================================================//
int  InterpreterConfigurator::opExecuteJobActionXMLXPath( const std::string &filestreamId,
                                                          XferDataItem *xferdataitemQry,
                                                          XferDataItem *xferdataitemDest) {
  m_rep->filestream = FileStream::find( filestreamId );
  if( m_rep->filestream == 0 ){
    ParserError( compose(_("Undeclared FILESTREAM '%1'."), filestreamId) );
  }
  //m_rep->filestream->getOpenAction();
  m_rep->jobaction = new XMLXPathJobAction( m_rep->filestream, xferdataitemQry, xferdataitemDest);
  int retval=JobManager::Instance().opExecuteJobAction( m_rep->function, m_rep->jobaction );
  return  retval;
}

//======================================================================//
// - opExecuteJobActionReport
//======================================================================//
int InterpreterConfigurator::opExecuteJobActionReport( ReportGen::Mode mode, const std::string &reportId ){
  HardCopyListener *hcl;
  hcl = GuiFactory::Instance()->createPrinterDialog()->getHardCopy( reportId );
  if( hcl == 0 ){
    ParserError( compose(_("Undeclared report '%1'."), reportId) );
  }

  int addr = JobManager::Instance().opSetHardcopy( m_rep->function, reportId );

  JobManager::Instance().opStartHardcopy( m_rep->function, mode, hcl );
  return addr;
}

//======================================================================//
// - opParse
//======================================================================//
bool InterpreterConfigurator::opParse( XferDataItem *xferdataitem ){
  return JobManager::Instance().opParse( m_rep->function, xferdataitem );
}

//======================================================================//
// - opParse
//======================================================================//
 bool InterpreterConfigurator::opParse( const std::string& filename ){
  return JobManager::Instance().opParse( m_rep->function, filename );
}
//======================================================================//
// - opSerializeGui
//======================================================================//
int InterpreterConfigurator::opSerializeGui(){
  GuiElement* elem = 0;

  if( m_rep->serializeoption.elementName.size() > 0) {
	GuiForm* form = GuiElement::getForm( m_rep->serializeoption.elementName );
	if (form) {
	  elem = form->getElement();
	} else {
	  elem = GuiElement::findElement( m_rep->serializeoption.elementName );
	}

	if (!elem)
	  ParserError( compose(_("Undeclared Form or GuiElement '%1'."),
						   m_rep->serializeoption.elementName) );
  }
  int ret = JobManager::Instance().opSerializeElement( m_rep->function,
													   elem,
													   m_rep->serializeoption.type,
													   m_rep->serializeoption.outputFilename );

  m_rep->serializeoption.clear();
  return ret;
}
//======================================================================//
// - setSerializeType
//======================================================================//
bool InterpreterConfigurator::setSerializeType(AppData::SerializeType type) {
  m_rep->serializeoption.type = type;
  return true;
}
//======================================================================//
// - setSerializeElement
//======================================================================//
bool InterpreterConfigurator::setSerializeElement( const std::string &elemId ) {
  m_rep->serializeoption.elementName = elemId;
  return true;
}
//======================================================================//
// - setSerializeOutputFilename
//======================================================================//
bool InterpreterConfigurator::setSerializeOutputFilename( const std::string &filename ) {
  m_rep->serializeoption.outputFilename = filename;
  return true;
}
//======================================================================//
// - opSerializeForm
//======================================================================//
int InterpreterConfigurator::opSerializeForm( const std::string &formId ){
  if( (m_rep->form=GuiElement::getForm( formId )) == 0 ){
    ParserError( compose(_("Undeclared form '%1'."), formId) );
  }
  return JobManager::Instance().opSerializeForm( m_rep->function, m_rep->form );
}
//======================================================================//
// - opSerializeGuiElement
//======================================================================//
int InterpreterConfigurator::opSerializeGuiElement( const std::string &guiElemId ){
  if( (m_rep->element=GuiElement::findElement( guiElemId )) == 0 ){
    ParserError( compose(_("Undeclared GuiElement '%1'."), guiElemId) );
  }
  AppData::SerializeType serialType;
  if (m_rep->streamJSONFlag) {
    serialType = AppData::serialize_JSON;
    m_rep->streamJSONFlag = false;
  } else   if (m_rep->streamXMLFlag) {
    serialType = AppData::serialize_XML;
    m_rep->streamXMLFlag = false;
  } else {
    ParserError( _("Undeclared serialize format type") );
  }

  int ret = JobManager::Instance().opSerializeGuiElement( m_rep->function, m_rep->element, serialType,
														  m_rep->fileoption.hasFilenameXfer,
														  m_rep->fileoption.filename);
  m_rep->fileoption.clear();
  return ret;
}
//======================================================================//
// - opWriteSettings
//======================================================================//
int InterpreterConfigurator::opWriteSettings(){
  return JobManager::Instance().opWriteSettings( m_rep->function );
  return true;
}
//======================================================================//
// - opMap
//======================================================================//
int InterpreterConfigurator::opMap(XferDataItem *xferdataitem, int nargs){
  return JobManager::Instance().opMap(m_rep->function, xferdataitem, nargs);
}
//======================================================================//
// - opMapForm
//======================================================================//
int InterpreterConfigurator::opMapForm( const std::string &formId ){
  if( (m_rep->form=GuiElement::getForm( formId )) == 0 ){
    ParserError( compose(_("Undeclared form '%1'."), formId) );
  }
  return JobManager::Instance().opMapForm( m_rep->function, m_rep->form );
}
//======================================================================//
// - opMapFolder
//======================================================================//
int InterpreterConfigurator::opMapFolder( const std::string &folderId, int nargs){
  GuiElement *elem = GuiElement::findElement(folderId);
  if(!elem){
    ParserError( compose(_("Undeclared GuiElement '%1'."), folderId) );
  }
  return JobManager::Instance().opMap(m_rep->function, elem, nargs);
}
//======================================================================//
// - opMapFolderGroup
//======================================================================//
int InterpreterConfigurator::opMapFolderGroup( const std::string &folderGroupId,
                                               GuiFolderGroup::OmitMap omit_map ){
  if( !GuiFolderGroup::FolderGroupExists( folderGroupId ) ){
    ParserError( compose(_("Undeclared Foldergroup '%1'."), folderGroupId) );
  }
  return JobManager::Instance().opMapFolderGroup( m_rep->function, folderGroupId, omit_map );
}
//======================================================================//
// - opMapGuiElement
//======================================================================//
 int InterpreterConfigurator::opMapGuiElement( const std::string &guiElementId ){
  if( GuiElement::findElement( guiElementId ) == 0 ){
    ParserError( compose(_("Undeclared GuiElement '%1'."), guiElementId) );
  }
  return JobManager::Instance().opMapGuiElement( m_rep->function, guiElementId );
}
//======================================================================//
// - opMapTableLine
//======================================================================//
int InterpreterConfigurator::opMapTableLine( const std::string &tableId ){
  GuiElement *el = GuiElement::findElement(tableId);
  if(el == 0){
    ParserError( compose(_("Undeclared GuiElement '%1'."), tableId) );
  }
  if (el->Type() != GuiElement::type_Table) {
    ParserError( compose(_("'%1' is not of type TABLE."), tableId) );
  }
  return JobManager::Instance().opMapTableLine( m_rep->function, tableId );
}
//======================================================================//
// - opUnmap
//======================================================================//
int InterpreterConfigurator::opUnmap( ){
  return JobManager::Instance().opUnmap( m_rep->function );
}
//======================================================================//
// - opUnmapForm
//======================================================================//
int InterpreterConfigurator::opUnmapForm( const std::string &formId ){
  if( (m_rep->form=GuiElement::getForm( formId )) == 0 ){
    ParserError( compose(_("Undeclared form '%1'."), formId) );
  }
  return JobManager::Instance().opUnmapForm( m_rep->function, m_rep->form );
}
//======================================================================//
// - opUnmapFolderGroup
//======================================================================//
int InterpreterConfigurator::opUnmapFolderGroup( const std::string &folderGroupId ){
  if( !GuiFolderGroup::FolderGroupExists( folderGroupId ) ){
    ParserError( compose(_("Undeclared Foldergroup '%1'."), folderGroupId) );
  }
  return JobManager::Instance().opUnmapFolderGroup( m_rep->function, folderGroupId );
}
//======================================================================//
// - opUnmapGuiElement
//======================================================================//
int InterpreterConfigurator::opUnmapGuiElement( const std::string &guiElementId ){
  if( GuiElement::findElement( guiElementId ) == 0 ){
    ParserError( compose(_("Undeclared GuiElement '%1'."), guiElementId) );
  }
  return JobManager::Instance().opUnmapGuiElement( m_rep->function, guiElementId );
}
//======================================================================//
// - opUnmapTableLine
//======================================================================//
int InterpreterConfigurator::opUnmapTableLine( const std::string &tableId ){
  GuiElement *el = GuiElement::findElement(tableId);
  if(el == 0){
    ParserError( compose(_("Undeclared GuiElement '%1'."), tableId) );
  }
  if (el->Type() != GuiElement::type_Table) {
    ParserError( compose(_("'%1' is not of type TABLE."), tableId) );
  }
  return JobManager::Instance().opUnmapTableLine( m_rep->function, tableId );
}
//======================================================================//
// - opVisible
//======================================================================//
int InterpreterConfigurator::opVisible( XferDataItem *xferdataitem ){
  return JobManager::Instance().opVisible( m_rep->function, xferdataitem );
}
//======================================================================//
// - opVisibleForm
//======================================================================//
int InterpreterConfigurator::opVisibleForm( const std::string &formId ){
  if( (m_rep->form=GuiElement::getForm( formId )) == 0 ){
    ParserError( compose(_("Undeclared form '%1'."), formId) );
  }
  return JobManager::Instance().opVisibleForm( m_rep->function, m_rep->form );
}
//======================================================================//
// - opVisibleFolderGroup
//======================================================================//
int InterpreterConfigurator::opVisibleFolderGroup( const std::string &folderGroupId ){
  if( !GuiFolderGroup::FolderGroupExists( folderGroupId ) ){
    ParserError( compose(_("Undeclared Foldergroup '%1'."), folderGroupId) );
  }
  return JobManager::Instance().opVisibleFolderGroup( m_rep->function, folderGroupId );
}
//======================================================================//
// - opEditable
//======================================================================//
int InterpreterConfigurator::opEditable(){
  return JobManager::Instance().opEditable( m_rep->function, 0 );
}
//======================================================================//
// - opEditable
//======================================================================//
int InterpreterConfigurator::opEditable(const std::string &elemId){
  GuiElement *e = GuiElement::findElement( elemId );
  if( e == 0 ){
    ParserError( compose(_("Undeclared GuiElement '%1'."), elemId) );
  }
  return JobManager::Instance().opEditable(m_rep->function, e);
}
//======================================================================//
//- opAllow
//======================================================================//
int InterpreterConfigurator::opAllow(){
  return JobManager::Instance().opAllow( m_rep->function, m_rep->joblistenercontroller );
}
//======================================================================//
// - opDisallow
//======================================================================//
int InterpreterConfigurator::opDisallow(){
  return JobManager::Instance().opDisallow( m_rep->function, m_rep->joblistenercontroller );
}
//======================================================================//
// - getForm
//======================================================================//
GuiForm *InterpreterConfigurator::getForm( const std::string &formId ){
  GuiForm *form = GuiElement::getForm( formId );
  m_rep->joblistenercontroller = form->getListenerController();
  return form;
}
//======================================================================//
// - getTask
//======================================================================//
JobTask *InterpreterConfigurator::getTask( const std::string &taskId ){
  JobTask *task = JobManager::Instance().getTask( taskId );
  m_rep->joblistenercontroller = task;
  return task;
}
//======================================================================//
// - jobGetFunction
//======================================================================//
JobFunction *InterpreterConfigurator::jobGetFunction( const std::string &funcId ){
  JobFunction *func = JobManager::Instance().getFunction( funcId, true );
  m_rep->joblistenercontroller = func;
  return func;
}
//======================================================================//
// - findProcessGroup
//======================================================================//
ProcessGroup *InterpreterConfigurator::findProcessGroup( const std::string &pgId ){
  ProcessGroup *proc = ProcessGroup::find( pgId );
  m_rep->joblistenercontroller = proc;
  return proc;
}
//======================================================================//
// - opEnableGuiElement
//======================================================================//
int InterpreterConfigurator::opEnableGuiElement(){
  return JobManager::Instance().opEnableGuiElement( m_rep->function, m_rep->jobenabledisableelement );
}
//======================================================================//
// - opDisableGuiElement
//======================================================================//
int InterpreterConfigurator::opDisableGuiElement(){
  return JobManager::Instance().opDisableGuiElement( m_rep->function, m_rep->jobenabledisableelement );
}
//======================================================================//
// - opDisableDragGuiElement
//======================================================================//
int InterpreterConfigurator::opDisableDragGuiElement(){
  return JobManager::Instance().opDisableDragGuiElement( m_rep->function, m_rep->jobenabledisableelement );
}
//======================================================================//
// - opSetEditable
//======================================================================//
int InterpreterConfigurator::opSetEditable(){
  return JobManager::Instance().opSetEditable( m_rep->function );
}
//======================================================================//
// - opSetEditableCycle
//======================================================================//
int InterpreterConfigurator::opSetEditableCycle(){
  return JobManager::Instance().opSetEditableCycle( m_rep->function );
}
//======================================================================//
// - opSetLocked
//======================================================================//
int InterpreterConfigurator::opSetLocked(){
  return JobManager::Instance().opSetLocked( m_rep->function );
}
//======================================================================//
// - opSetColor
//======================================================================//
int InterpreterConfigurator::opSetColor(){
  return JobManager::Instance().opSetColor( m_rep->function );
}
//======================================================================//
// - opSetColorBit
//======================================================================//
int InterpreterConfigurator::opSetColorBit(){
  return JobManager::Instance().opSetColorBit( m_rep->function );
}
//======================================================================//
// - opUnsetColorBit
//======================================================================//
int InterpreterConfigurator::opUnsetColorBit(){
  return JobManager::Instance().opUnsetColorBit( m_rep->function );
}
//======================================================================//
// - opSetDataTimestamp
//======================================================================//
int InterpreterConfigurator::opSetDataTimestamp(){
  return JobManager::Instance().opSetDataTimestamp( m_rep->function );
}
//======================================================================//
// - opSetValueTimestamp
//======================================================================//
int InterpreterConfigurator::opSetValueTimestamp(){
  return JobManager::Instance().opSetValueTimestamp( m_rep->function );
}
//======================================================================//
// - opSetStylesheet
//======================================================================//
int InterpreterConfigurator::opSetStylesheet() {
  return JobManager::Instance().opSetStylesheet( m_rep->function );
}
//======================================================================//
// - opSetStylesheet
//======================================================================//
int InterpreterConfigurator::opSetStylesheet(const std::string &elemId) {
  GuiElement *e = GuiElement::findElement( elemId );
  if( e == 0 ){
    ParserError( compose(_("Undeclared GuiElement '%1'."), elemId) );
  }
  return JobManager::Instance().opSetStylesheet( m_rep->function, e );
}

//======================================================================//
// - getNamedElement
//======================================================================//
GuiElement *InterpreterConfigurator::getNamedElement( const std::string &elementId ){
  m_rep->jobenabledisableelement = GuiElement::findElement( elementId );
  return m_rep->jobenabledisableelement;
}
//======================================================================//
// - opUpdateGuiElement
//======================================================================//
int InterpreterConfigurator::opUpdateGuiElement( const std::string &elementId ){
  m_rep->element = GuiElement::findElement( elementId );
  return JobManager::Instance().opUpdateGuiElement( m_rep->function, m_rep->element );
}
//======================================================================//
// - opClearValues
//======================================================================//
bool InterpreterConfigurator::opClearValues(){
  JobManager::Instance().opClearValues( m_rep->function );
  return true;
}
//======================================================================//
// - opEraseValues
//======================================================================//
bool InterpreterConfigurator::opEraseValues(){
  JobManager::Instance().opEraseValues( m_rep->function );
  return true;
}
//======================================================================//
// - opClearTextWindow
//======================================================================//
bool InterpreterConfigurator::opClearTextWindow( const std::string &textwindow){
  if( !textwindow.empty() ){
    m_rep->scrolledtext = GuiElement::getScrolledText( textwindow );
    if( m_rep->scrolledtext == 0 )
      ParserError( compose(_("Undeclared format '%1'."), textwindow) );
  }
  else{
    m_rep->scrolledtext = 0;
  }
  JobManager::Instance().opClearTextWindow( m_rep->function, m_rep->scrolledtext );
  return true;
}
//======================================================================//
// - opPackValues
//======================================================================//
bool InterpreterConfigurator::opPackValues( int packRow ){
  JobManager::Instance().opPackValues( m_rep->function, packRow != 0 );
  return true;
}
//======================================================================//
// - opCopy
//======================================================================//
int InterpreterConfigurator::opCopy( const std::string &streamId ){
  GuiElement *el = GuiElement::findElement( streamId );
  if (el) {
    m_rep->stream = 0;
    if (el->Type() == GuiElement::type_List) {
      m_rep->stream = el->getList()->getStream();
    } else
    if (el->Type() == GuiElement::type_Table) {
      m_rep->stream = el->getTable()->getStream();
    }
    // if (!m_rep->stream)
    //   ParserError( compose(_("Undeclared Table or List '%1'."), streamId) );
  } else
  if( (m_rep->stream = StreamManager::Instance().getStream( streamId )) == NULL ){
    ParserError( compose(_("Undeclared Stream '%1'."), streamId) );
  }
  // if stream is 0, GuiElement screenshot will be copied to clipboard
  return JobManager::Instance().opCopy( m_rep->function, m_rep->stream, el);
}
//======================================================================//
// - opPaste
//======================================================================//
int InterpreterConfigurator::opPaste( const std::string &streamId ){
  GuiElement *el = GuiElement::findElement( streamId );
  if (el) {
    if (el->Type() == GuiElement::type_List) {
      m_rep->stream = el->getList()->getStream();
    } else
    if (el->Type() == GuiElement::type_Table) {
      m_rep->stream = el->getTable()->getStream();
    }
    if (!m_rep->stream)
      ParserError( compose(_("Undeclared Table or List '%1'."), streamId) );
  } else
  if( (m_rep->stream = StreamManager::Instance().getStream( streamId )) == NULL ){
    ParserError( compose(_("Undeclared Stream '%1'."), streamId) );
  }
  return JobManager::Instance().opPaste( m_rep->function, m_rep->stream );
}
//======================================================================//
// - opSelectRows
//======================================================================//
bool InterpreterConfigurator::opSelectRows( const std::string &guiElementId, bool dataIsInt ){
  GuiElement *e = GuiElement::findElement( guiElementId );
  if( e == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), guiElementId) );
  }
  if( e->Type() != GuiElement::type_NavDiagram &&
      e->Type() != GuiElement::type_List &&
      e->Type() != GuiElement::type_Table ){
    ParserError( compose(_("'%1' is not of type list, table or navigator."), guiElementId) );
  }

  // MULTIPLE_SELECTION Liste/Navigator braucht einen Wildcard,
  // normale Liste/Navigator keinen
  bool multiSelec = e->Type() == GuiElement::type_List ? e->getList()->isMultipleSelection() :
    e->Type() == GuiElement::type_NavDiagram ? e->getNavigator()->isMultipleSelection() :
	e->Type() == GuiElement::type_Table ?  e->getTable()->isMultipleSelection() : false;
  if ( multiSelec ) {
    if ( !dataIsInt && m_rep->jobdataref->numWildcards() != 1 ) {
      ParserError( _("Exactly one WILDCARD is needed.") );
    }
  }
  return JobManager::Instance().opSelectRows( m_rep->function, guiElementId, dataIsInt );
}
//======================================================================//
// - opGetSelection
//======================================================================//
bool InterpreterConfigurator::opGetSelection( const std::string &guiElementId ){
  GuiElement *e = GuiElement::findElement( guiElementId );
  if( e == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), guiElementId) );
  }
  if( e->Type() != GuiElement::type_QWTPlot &&
      e->Type() != GuiElement::type_List &&
      e->Type() != GuiElement::type_Table &&
      e->Type() != GuiElement::type_Navigator ){
    ParserError( compose(_("'%1' is not of type LIST, TABLE, NAVIGATOR or PLOT2D."), guiElementId) );
  }
  return JobManager::Instance().opGetSelection( m_rep->function, guiElementId );
}

//======================================================================//
// - opGetSortCriteria
//======================================================================//
bool InterpreterConfigurator::opGetSortCriteria( const std::string &guiElementId ){
  GuiElement *e = GuiElement::findElement( guiElementId );
  if( e == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), guiElementId) );
  }
  if( e->Type() != GuiElement::type_List ){
    ParserError( compose(_("'%1' is not of type LIST."), guiElementId) );
  }
  return JobManager::Instance().opGetSortCriteria( m_rep->function, guiElementId );
}

//======================================================================//
// - opClearSelection
//======================================================================//
bool InterpreterConfigurator::opClearSelection( const std::string &guiElementId ){
  GuiElement *e = GuiElement::findElement( guiElementId );
  if( e == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), guiElementId) );
  }
  if( e->Type() != GuiElement::type_Table &&
      e->Type() != GuiElement::type_List &&
      e->Type() != GuiElement::type_Navigator &&
      e->Type() != GuiElement::type_NavDiagram &&
      e->Type() != GuiElement::type_NavIconView ){
    ParserError( compose(_("'%1' is not of type list, table or navigator."), guiElementId) );
  }
  return JobManager::Instance().opClearSelection( m_rep->function, guiElementId );
}
//======================================================================//
// - opAssignCorresponding
//======================================================================//
bool InterpreterConfigurator::opAssignCorresponding(){
  JobManager::Instance().opAssignCorresponding( m_rep->function );
  return true;
}
//======================================================================//
// - opGetTimestamp
//======================================================================//
bool InterpreterConfigurator::opGetTimestamp(){
  JobManager::Instance().opGetTimestamp( m_rep->function );
  return true;
}
//======================================================================//
// - opGetDbModified
//======================================================================//
bool InterpreterConfigurator::opGetDbModified(){
  JobManager::Instance().opGetDbModified( m_rep->function );
  return true;
}
//======================================================================//
// - opSetDbTimestamp
//======================================================================//
int InterpreterConfigurator::opSetDbTimestamp(){
  return JobManager::Instance().opSetDbTimestamp( m_rep->function );
}
//======================================================================//
// - opGetClassname
//======================================================================//
bool InterpreterConfigurator::opGetClassname(){
  JobManager::Instance().opGetClassname( m_rep->function );
  return true;
}
//======================================================================//
// - opGetNodename
//======================================================================//
bool InterpreterConfigurator::opGetNodename(){
  return JobManager::Instance().opGetNodename( m_rep->function );
}
//======================================================================//
// - setElseAddress
//======================================================================//
bool InterpreterConfigurator::setElseAddress( int nextaddress ){
  JobCodeOpIf* JobIFexpr = m_rep->popJobIFexpr();
  (JobIFexpr)->setElseAddress( nextaddress );
  (JobIFexpr)->setNextAddress( m_rep->function->nextAddress() );
  return true;
}
//======================================================================//
// - opIfOperator
//======================================================================//
JobCodeOpIf *InterpreterConfigurator::opIfOperator(){
  m_rep->jobifexpr = JobManager::Instance().opIfOperator( m_rep->function );
  m_rep->pushJobIFexpr();
  return m_rep->jobifexpr;
}
//======================================================================//
// - opBranch
//======================================================================//
bool InterpreterConfigurator::opBranch(){
  JobManager::Instance().opBranch( m_rep->function );
  return true;
}
//======================================================================//
// - setNextAddress
//======================================================================//
bool InterpreterConfigurator::setNextAddress( int nextaddress ){
  JobManager::Instance().opBranch( m_rep->function, nextaddress );
  JobCodeOpWhile* WHILEexpr = m_rep->popJobWHILEexpr();
  (WHILEexpr)->setNextAddress( m_rep->function->nextAddress() );
  return true;
}
//======================================================================//
// - opWhileOperator
//======================================================================//
JobCodeOpWhile *InterpreterConfigurator::opWhileOperator(){
  m_rep->jobwhileexpr = JobManager::Instance().opWhileOperator( m_rep->function );
  m_rep->pushJobWHILEexpr();
  return m_rep->jobwhileexpr;
}
//======================================================================//
// - opPushReal
//======================================================================//
int InterpreterConfigurator::opPushReal( const double realvalue ){
  return JobManager::Instance().opPushReal( m_rep->function, realvalue );
}
//======================================================================//
// - opPushInteger
//======================================================================//
int InterpreterConfigurator::opPushInteger( const int intvalue ){
  return JobManager::Instance().opPushInteger( m_rep->function, intvalue );
}
//======================================================================//
// - opPushSortOrder
//======================================================================//
int  InterpreterConfigurator::opPushSortOrder( const std::string &listId ){
  GuiElement *e = GuiElement::findElement( listId );
  if( e == 0 ){
    ParserError( compose(_("Undeclared identifier '%1'."), listId) );
  }
  if( e->Type() != GuiElement::type_List ){
    ParserError( compose(_("'%1' is not of type LIST."), listId) );
  }
  return JobManager::Instance().opPushSortOrder( m_rep->function, listId );
}
//======================================================================//
// - opPushString
//======================================================================//
int InterpreterConfigurator::opPushString( const std::string &stringconst ){
  return JobManager::Instance().opPushString( m_rep->function, stringconst );
}
//======================================================================//
// - opPushEndOfLine
//======================================================================//
int InterpreterConfigurator::opPushEndOfLine(){
  return JobManager::Instance().opPushEndOfLine( m_rep->function );
}
//======================================================================//
// - opPushInvalid
//======================================================================//
int InterpreterConfigurator::opPushInvalid(){
  return JobManager::Instance().opPushInvalid( m_rep->function );
}
//======================================================================//
// - opPushOldValue
//======================================================================//
int InterpreterConfigurator::opPushOldValue(){
  return JobManager::Instance().opPushOldValue( m_rep->function );
}
//======================================================================//
// - opPushIndexNumber
//======================================================================//
int InterpreterConfigurator::opPushIndexNumber(){
  return JobManager::Instance().opPushIndexNumber( m_rep->function );
}
//======================================================================//
// - opPushColumn
//======================================================================//
int InterpreterConfigurator::opPushColumn(){
  return JobManager::Instance().opPushColumn( m_rep->function );
}
//======================================================================//
// - opPushRow
//======================================================================//
int InterpreterConfigurator::opPushRow(){
  return JobManager::Instance().opPushRow( m_rep->function );
}
//======================================================================//
// - opPushDiagramXPos
//======================================================================//
int InterpreterConfigurator::opPushDiagramXPos(){
  return JobManager::Instance().opPushDiagramXPos( m_rep->function );
}
//======================================================================//
// - opPushDiagramYPos
//======================================================================//
int InterpreterConfigurator::opPushDiagramYPos(){
  return JobManager::Instance().opPushDiagramYPos( m_rep->function );
}
//======================================================================//
// - opPushSortCriteria
//======================================================================//
int InterpreterConfigurator::opPushSortCriteria(){
  return JobManager::Instance().opPushSortCriteria( m_rep->function );
}
//======================================================================//
// - opPushError
//======================================================================//
int InterpreterConfigurator::opPushError(){
  return JobManager::Instance().opPushError( m_rep->function );
}
//======================================================================//
// - opPushReason
//======================================================================//
int InterpreterConfigurator::opPushReason( JobElement::CallReason reason ){
  return JobManager::Instance().opPushReason( m_rep->function, reason );
}
//======================================================================//
// - opPushCycle
//======================================================================//
int InterpreterConfigurator::opPushCycle(){
  return JobManager::Instance().opPushCycle( m_rep->function );
}
//======================================================================//
// - opPushMaxCycle
//======================================================================//
int InterpreterConfigurator::opPushMaxCycle(){
  return JobManager::Instance().opPushMaxCycle( m_rep->function );
}
//======================================================================//
// - opPushIndex
//======================================================================//
int InterpreterConfigurator::opPushIndex( const std::string &indexId ){
  return JobManager::Instance().opPushIndex( m_rep->function, indexId );
}
//======================================================================//
// - opPushUnits
//======================================================================//
int InterpreterConfigurator::opPushUnits(){
  return   JobManager::Instance().opPushUnits( m_rep->function );
}
//======================================================================//
// - opPushLabel
//======================================================================//
int InterpreterConfigurator::opPushLabel(){
  return  JobManager::Instance().opPushLabel( m_rep->function );
}
//======================================================================//
// - opPushIndex
//======================================================================//
bool InterpreterConfigurator::opPushIndex(){
  JobManager::Instance().opPushIndex( m_rep->function );
  return true;
}
//======================================================================//
// - opPushIndexOfLevel
//======================================================================//
bool InterpreterConfigurator::opPushIndexOfLevel(){
  JobManager::Instance().opPushIndexOfLevel( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalValid
//======================================================================//
bool InterpreterConfigurator::opEvalValid(){
  JobManager::Instance().opEvalValid( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalComplexValue
//======================================================================//
bool InterpreterConfigurator::opEvalComplexValue(){
  JobManager::Instance().opEvalComplexValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalComplexRealValue
//======================================================================//
bool InterpreterConfigurator::opEvalComplexRealValue(){
  JobManager::Instance().opEvalComplexRealValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalComplexImagValue
//======================================================================//
bool InterpreterConfigurator::opEvalComplexImagValue(){
  JobManager::Instance().opEvalComplexImagValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalAbsValue
//======================================================================//
bool InterpreterConfigurator::opEvalAbsValue(){
  JobManager::Instance().opEvalAbsValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalAbsValue
//======================================================================//
bool InterpreterConfigurator::opEvalLengthValue(){
  JobManager::Instance().opEvalLengthValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalSinValue
//======================================================================//
bool InterpreterConfigurator::opEvalSinValue(){
  JobManager::Instance().opEvalSinValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalCosValue
//======================================================================//
bool InterpreterConfigurator::opEvalCosValue(){
  JobManager::Instance().opEvalCosValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalTanValue
//======================================================================//
bool InterpreterConfigurator::opEvalTanValue(){
  JobManager::Instance().opEvalTanValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalASinValue
//======================================================================//
bool InterpreterConfigurator::opEvalASinValue(){
  JobManager::Instance().opEvalASinValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalACosValue
//======================================================================//
bool InterpreterConfigurator::opEvalACosValue(){
  JobManager::Instance().opEvalACosValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalATanValue
//======================================================================//
bool InterpreterConfigurator::opEvalATanValue(){
  JobManager::Instance().opEvalATanValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalATan2Value
//======================================================================//
bool InterpreterConfigurator::opEvalATan2Value(){
  JobManager::Instance().opEvalATan2Value( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalLogValue
//======================================================================//
bool InterpreterConfigurator::opEvalLogValue(){
  JobManager::Instance().opEvalLogValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalLog10Value
//======================================================================//
bool InterpreterConfigurator::opEvalLog10Value(){
  JobManager::Instance().opEvalLog10Value( m_rep->function );
  return true;
}

//======================================================================//
// - opEvalSqrtValue
//======================================================================//
bool InterpreterConfigurator::opEvalSqrtValue(){
  JobManager::Instance().opEvalSqrtValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalArgValue
//======================================================================//
bool InterpreterConfigurator::opEvalArgValue(){
  JobManager::Instance().opEvalArgValue( m_rep->function );
  return true;
}
//======================================================================//
// - opEvalChanged
//======================================================================//
bool InterpreterConfigurator::opEvalChanged( const std::string &guiElemId, const bool filter ){
  return JobManager::Instance().opEvalChangedValue( m_rep->function,guiElemId, filter );
}
//======================================================================//
// - opRound
//======================================================================//
bool InterpreterConfigurator::opRound(){
  JobManager::Instance().opRound( m_rep->function );
  return true;
}
//======================================================================//
// - opRound5
//======================================================================//
bool InterpreterConfigurator::opRound5(){
  JobManager::Instance().opRound5( m_rep->function );
  return true;
}
//======================================================================//
// - opRound10
//======================================================================//
bool InterpreterConfigurator::opRound10(){
  JobManager::Instance().opRound10( m_rep->function );
  return true;
}
//======================================================================//
// - opConfirm
//======================================================================//
bool InterpreterConfigurator::opConfirm(bool cancelBtn){
  JobManager::Instance().opConfirm( m_rep->function, cancelBtn, m_rep->buttonText );
  m_rep->buttonText.clear();
  return true;
}
//======================================================================//
// - opGetText
//======================================================================//
bool InterpreterConfigurator::opGetText(){
  JobManager::Instance().opGetText( m_rep->function);
  return true;
}
//======================================================================//
// - opAdd
//======================================================================//
bool InterpreterConfigurator::opAdd(){
  JobManager::Instance().opAdd( m_rep->function );
  return true;
}
//======================================================================//
// - opSub
//======================================================================//
bool InterpreterConfigurator::opSub(){
  JobManager::Instance().opSub( m_rep->function );
  return true;
}
//======================================================================//
// - opMul
//======================================================================//
bool InterpreterConfigurator::opMul(){
  JobManager::Instance().opMul( m_rep->function );
  return true;
}
//======================================================================//
// - opDiv
//======================================================================//
bool InterpreterConfigurator::opDiv(){
  JobManager::Instance().opDiv( m_rep->function );
  return true;
}
//======================================================================//
// - opDiv
//======================================================================//
bool InterpreterConfigurator::opModulo(){
  JobManager::Instance().opModulo( m_rep->function );
  return true;
}
//======================================================================//
// - opPower
//======================================================================//
bool InterpreterConfigurator::opPower(){
  JobManager::Instance().opPower( m_rep->function );
  return true;
}
//======================================================================//
// - opNegate
//======================================================================//
bool InterpreterConfigurator::opNegate(){
  JobManager::Instance().opNegate( m_rep->function );
  return true;
}
//======================================================================//
// - opGtr
//======================================================================//
bool InterpreterConfigurator::opGtr(){
  JobManager::Instance().opGtr( m_rep->function );
  return true;
}
//======================================================================//
// - opGeq
//======================================================================//
bool InterpreterConfigurator::opGeq(){
  JobManager::Instance().opGeq( m_rep->function );
  return true;
}
//======================================================================//
// - opLss
//======================================================================//
bool InterpreterConfigurator::opLss(){
  JobManager::Instance().opLss( m_rep->function );
  return true;
}
//======================================================================//
// - opLeq
//======================================================================//
bool InterpreterConfigurator::opLeq(){
  JobManager::Instance().opLeq( m_rep->function );
  return true;
}
//======================================================================//
// - opEql
//======================================================================//
bool InterpreterConfigurator::opEql(){
  JobManager::Instance().opEql( m_rep->function );
  return true;
}
//======================================================================//
// - opNeq
//======================================================================//
bool InterpreterConfigurator::opNeq(){
  JobManager::Instance().opNeq( m_rep->function );
  return true;
}
//======================================================================//
// - opAnd
//======================================================================//
bool InterpreterConfigurator::opAnd(){
  JobManager::Instance().opAnd( m_rep->function );
  return true;
}
//======================================================================//
// - opOr
//======================================================================//
bool InterpreterConfigurator::opOr(){
  JobManager::Instance().opOr( m_rep->function );
  return true;
}
//======================================================================//
// - opNot
//======================================================================//
bool InterpreterConfigurator::opNot(){
  JobManager::Instance().opNot( m_rep->function );
  return true;
}
//======================================================================//
// - opPrint
//======================================================================//
bool InterpreterConfigurator::opPrint(){
  JobManager::Instance().opPrint( m_rep->function );
  return true;
}
//======================================================================//
// - opLog
//======================================================================//
int InterpreterConfigurator::opLog( const std::string &level ) {
  return JobManager::Instance().opLog( m_rep->function, level );
}
//======================================================================//
// - opLogMsg
//======================================================================//
bool InterpreterConfigurator::opLogMsg() {
  JobManager::Instance().opLogMsg( m_rep->function);
  return true;
}
//======================================================================//
// - opSetError
//======================================================================//
int InterpreterConfigurator::opSetError(){
  return JobManager::Instance().opSetError( m_rep->function );
}
//======================================================================//
// - opSetErrorMsg
//======================================================================//
bool InterpreterConfigurator::opSetErrorMsg(){
  JobManager::Instance().opSetErrorMsg( m_rep->function );
  return true;
}
//======================================================================//
// - opSetMessage
//======================================================================//
bool InterpreterConfigurator::opSetMessage(){
  JobManager::Instance().opSetMessage( m_rep->function );
  return true;
}
//======================================================================//
// - opMessage
//======================================================================//
bool InterpreterConfigurator::opMessage(){
  JobManager::Instance().opMessage( m_rep->function );
  return true;
}
//**********************************************************************//
// END JOB
//**********************************************************************//


//**********************************************************************//
// GET-functions
//**********************************************************************//
//======================================================================//
// - getDataRef
//======================================================================//
DataReference *InterpreterConfigurator::getDataRef(){
  return m_rep -> dataref;
}
//======================================================================//
// - getDataItem
//======================================================================//
XferDataItem *InterpreterConfigurator::getDataItem(){
  return m_rep -> dataitem;
}
//======================================================================//
// - getDataItemIndex
//======================================================================//
XferDataItemIndex *InterpreterConfigurator::getDataItemIndex(){
  return m_rep -> dataitemindex;
}
//======================================================================//
// - getDataLevel
//======================================================================//
int InterpreterConfigurator::getDataLevel(){
  return m_rep -> datalevel;
}
//======================================================================//
// - getFileStream
//======================================================================//
FileStream *InterpreterConfigurator::getFileStream(){
  return m_rep -> filestream;
}
//======================================================================//
// - getGuiElement
//======================================================================//
GuiElement *InterpreterConfigurator::getGuiElement(){
  return m_rep -> element;
}
//======================================================================//
// - getGuiDataField
//======================================================================//
GuiDataField *InterpreterConfigurator::getGuiDataField(){
  return m_rep -> datafield;
}
//======================================================================//
// - getGuiVoidField
//======================================================================//
GuiVoid *InterpreterConfigurator::getGuiVoidField(){
  return m_rep -> voidfield;
}
//======================================================================//
// - getGuiIndexField
//======================================================================//
GuiIndex *InterpreterConfigurator::getGuiIndexField(){
  return m_rep -> indexfield;
}
//======================================================================//
// - getGuiFieldGroup
//======================================================================//
GuiFieldgroup *InterpreterConfigurator::getGuiFieldGroup(){
  return m_rep -> fieldgroup;
}
//======================================================================//
// - getGuiFieldgroupLine
//======================================================================//
GuiFieldgroupLine *InterpreterConfigurator::getGuiFieldgroupLine(){
  return m_rep -> fieldgroupline;
}
//======================================================================//
// - getGuiLabelField
//======================================================================//
GuiLabel *InterpreterConfigurator::getGuiLabelField(){
  return m_rep -> labelfield;
}
//======================================================================//
// - getGuiForm
//======================================================================//
GuiForm *InterpreterConfigurator::getGuiForm(){
  return m_rep -> form;
}
//======================================================================//
// - getGuiScrolledText
//======================================================================//
GuiScrolledText *InterpreterConfigurator::getGuiScrolledText(){
  return m_rep -> scrolledtext;
}
//======================================================================//
// - getGuiList
//======================================================================//
GuiList *InterpreterConfigurator::getGuiList(){
  return m_rep -> list;
}
//======================================================================//
// - getHelpkey
//======================================================================//
HelpManager::HelpKey *InterpreterConfigurator::getHelpkey(){
  return m_rep -> helpkey;
}
//======================================================================//
// - getListplot
//======================================================================//
ListPlot *InterpreterConfigurator::getListplot(){
  return m_rep -> listplot;
}
//======================================================================//
// - getListgraph
//======================================================================//
ListGraph *InterpreterConfigurator::getListgraph(){
  return m_rep -> listgraph;
}
//======================================================================//
// - getGuiPlotaxis
//======================================================================//
GuiPlotDataItem *InterpreterConfigurator::getGuiPlotaxis(){
  return m_rep -> plotaxis;
}
//======================================================================//
// - getSimpel
//======================================================================//
Simpel *InterpreterConfigurator::getSimpel(){
  return m_rep -> simpel;
}
//======================================================================//
// - getPsPlot
//======================================================================//
PSPlot *InterpreterConfigurator::getPsPlot(){
  return m_rep -> psplot;
}
//======================================================================//
// - getPsStream
//======================================================================//
PSPlot::PSStream *InterpreterConfigurator::getPsStream(){
  return m_rep -> psstream;
}
//======================================================================//
// - getPlot3d
//======================================================================//
Gui3dPlot *InterpreterConfigurator::getPlot3d(){
  return m_rep -> plot3d;
}
//======================================================================//
// - getGuiXrtgraph
//======================================================================//
Gui2dPlot *InterpreterConfigurator::getGui2dPlot(){
  return m_rep -> plot2d;
}
//======================================================================//
// - getGuiPlotitem
//======================================================================//
Gui2dPlot::PlotItem *InterpreterConfigurator::getGuiPlotitem(){
  return m_rep -> plotitem;
}
//======================================================================//
// - getGuiXPlotitem
//======================================================================//
Gui2dPlot::PlotItem *InterpreterConfigurator::getGuiXPlotitem(){
  return m_rep -> xplotitem;
}
//======================================================================//
// - getGuiFolder
//======================================================================//
GuiFolder *InterpreterConfigurator::getGuiFolder(){
  return m_rep -> folder;
}
//======================================================================//
// - getGuiTable
//======================================================================//
GuiTable *InterpreterConfigurator::getGuiTable(){
  return m_rep -> table;
}
//======================================================================//
// - getGuiTableLine
//======================================================================//
GuiTableLine *InterpreterConfigurator::getGuiTableLine(){
  return m_rep -> tableline;
}
//======================================================================//
// - getGuiTablesize
//======================================================================//
GuiTableSize *InterpreterConfigurator::getGuiTablesize(){
  return m_rep -> tablesize;
}
//======================================================================//
// - getGuiTableDataitem
//======================================================================//
GuiTableItem *InterpreterConfigurator::getGuiTableDataitem(){
  return m_rep -> tabledataitem;
}
//======================================================================//
// - getGuiTableItem
//======================================================================//
GuiTableItem *InterpreterConfigurator::getGuiTableItem(){
  return m_rep -> tableitem;
}
//======================================================================//
// - getFilemenulist
//======================================================================//
FileButtonList *InterpreterConfigurator::getFilemenulist(){
  return m_rep -> filemenulist;
}
//======================================================================//
// - getPlugin
//======================================================================//
Plugin *InterpreterConfigurator::getPlugin(){
  return m_rep -> plugin;
}
//======================================================================//
// - getPrintfolder
//======================================================================//
HardCopyFolder *InterpreterConfigurator::getPrintfolder(){
  return m_rep -> printfolder;
}
//======================================================================//
// - getPrintobject
//======================================================================//
HardCopyListener *InterpreterConfigurator::getPrintobject(){
  return m_rep -> printobject;
}
//======================================================================//
// - getProcess
//======================================================================//
Process *InterpreterConfigurator::getProcess(){
  return m_rep -> process;
}
//======================================================================//
// - getProcessgroup
//======================================================================//
ProcessGroup *InterpreterConfigurator::getProcessgroup(){
  return m_rep -> processgroup;
}
//======================================================================//
// - getStream
//======================================================================//
Stream *InterpreterConfigurator::getStream(){
  return m_rep -> stream;
}
//======================================================================//
// - getIndexnum
//======================================================================//
int InterpreterConfigurator::getIndexnum(){
  return m_rep -> indexnum;
}
//======================================================================//
// - getReportStream
//======================================================================//
ReportStream *InterpreterConfigurator::getReportStream(){
  return m_rep -> reportstream;
}
//======================================================================//
// - getFunction
//======================================================================//
JobFunction *InterpreterConfigurator::getFunction(){
  return m_rep -> function;
}
//======================================================================//
// - getTask
//======================================================================//
JobTask *InterpreterConfigurator::getTask(){
  return m_rep -> task;
}
//======================================================================//
// - getJobDataRef
//======================================================================//
JobDataReference *InterpreterConfigurator::getJobDataRef(){
  return m_rep -> jobdataref;
}
//======================================================================//
// - getJobAction
//======================================================================//
JobAction *InterpreterConfigurator::getJobAction(){
  return m_rep -> jobaction;
}
//======================================================================//
// - getFunctionNextAddress
//======================================================================//
int InterpreterConfigurator::getFunctionNextAddress(){
  return m_rep -> function->nextAddress();
}
//**********************************************************************//
// END GET-functions
//**********************************************************************//

void InterpreterConfigurator::setFunctionHandler( FunctionHandler *f ){
    m_rep -> funcHandler = f;
}
