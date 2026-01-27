#include <fstream>
#include <string>

#include "parser/IdManager.h"

#include "gui/GuiElement.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataPool.h"
#include "job/JobCodeStandards.h"
#include "gui/GuiListenerController.h"
#include "job/JobFunction.h"
#include "app/AppData.h"
#include "parser/Flexer.h"

class GuiPlotDataItem;
class DataReference;

#if defined(BISON_USE_PARSER_H_EXTENSION)
#include "parseryacc.h"
#else
#include "parseryacc.hh"
#endif

#include "parser/HMap.h"

 // CHECK THIS
#define YY_CURRENT_BUFFER ( (yy_buffer_stack) \
                           ? (yy_buffer_stack)[(yy_buffer_stack_top)] \
                           : NULL)

//----------------------------------------------------------------------//
// Constructor                                                          //
//----------------------------------------------------------------------//

Flexer::Flexer( std::istream *is, int startToken )
              : yyFlexLexer( is )
              , m_rwTabMap( HMap( 250 ) ){
  initRwTabMap();
  initTokenMap();
  initInclFileList();
  start_token = startToken!=0? startToken: START_DESCRIPTION;
  //std::cerr << "Flexer start token " << start_token << std::endl;
}

Flexer::Flexer():m_rwTabMap( HMap( 250 ) ){}
Flexer::Flexer( const Flexer& ):m_rwTabMap( HMap( 250 ) ){}

int Flexer::startToken(){
  int t = this->start_token;
  this->start_token=0;
  return t;
}
//----------------------------------------------------------------------//
// reserved_word_type                                                   //
//----------------------------------------------------------------------//

int Flexer::reserved_word_type()
{
  RwTabMap::Iterator rwPos = m_rwTabMap.find( YYText() );
    if( rwPos != m_rwTabMap.end() )
      return rwPos->second;

  yylval.val_string = new std::string( YYText() );

  /* Unser Lex liefert bereits bekannte Identifier als eigenes Token */
  /* --------------------------------------------------------------- */

  TokenMap::iterator pos = m_lexIdTokenMap.find( IdManager::Instance().getType( *yylval.val_string ) );
  if( pos != m_lexIdTokenMap.end() ){
     return  pos->second;
  }

  return tIDENTIFIER;
}

//----------------------------------------------------------------------//
// IncludeFile                                                          //
//----------------------------------------------------------------------//

bool Flexer::IncludeFile( const std::string &inclFileName,
                          const int lineNo,
                          int bufSize )
{
  std::string path;
  std::vector<std::string>::iterator iter = m_inclPathList.begin();

  while( iter != m_inclPathList.end() ){
    path = *iter + inclFileName;

    std::ifstream *includefile = new std::ifstream( path.c_str() );
    if( includefile->good() ){// is open
      yyin.rdbuf( includefile->rdbuf() );
      m_inclFileStack.push( new InclFile( YY_CURRENT_BUFFER, m_currentFilename, lineNo ) );
      yy_switch_to_buffer( yy_create_buffer( includefile, bufSize ) );
      m_currentFilename=path;
      return true;
    }
    delete includefile;
    ++iter;
  }

  perror( inclFileName.c_str() );
  return false;
}

//----------------------------------------------------------------------//
// endOfFile                                                            //
//----------------------------------------------------------------------//

bool Flexer::endOfFile( int &lineNo ){
  if ( m_inclFileStack.empty() ){
    return true;
  }

  else {
    //#if yy_buffer_stack
    yy_delete_buffer( YY_CURRENT_BUFFER );
    //#else
    //yy_delete_buffer( yy_current_buffer );
    //#endif
    yy_switch_to_buffer( m_inclFileStack.top()->m_include );
    lineNo = m_inclFileStack.top()->m_lineno;
    m_currentFilename= m_inclFileStack.top()->m_filename;
    delete m_inclFileStack.top();
    m_inclFileStack.pop();
  }
  return false;
}


//----------------------------------------------------------------------//
// initTokenMap                                                         //
//----------------------------------------------------------------------//

void Flexer::initTokenMap(){
  m_lexIdTokenMap.insert( intPair( IdManager::id_None, tIDENTIFIER ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_DataVariable,tID_DATAVARIABLE  ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_DataStructure, tID_DATASTRUCTURE ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_DataSet, tID_DATASET ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_ColorSet, tID_COLORSET ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Tag, tID_TAG ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Form, tID_FORM ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Fieldgroup, tID_FIELDGROUP ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Thermo, tID_THERMO ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Folder, tID_FOLDER ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Foldergroup, tID_FOLDERGROUP ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Index, tID_INDEX ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Uniplot, tID_UNIPLOT ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Psplot, tID_PSPLOT ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_List, tID_LIST ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Table, tID_TABLE ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Listplot, tID_LISTPLOT ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_3dPlot, tID_XRT3DPLOT ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_3dPlot, tID_PLOT3D ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Image, tID_IMAGE ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_LinePlot, tID_LINEPLOT ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_2dPlot, tID_PLOT2D ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Textwindow, tID_TEXTWINDOW ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Navigator, tID_NAVIGATOR ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_MessageQueue, tID_MESSAGE_QUEUE ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_ProcessGroup, tID_PROCESSGROUP ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Process, tID_PROCESS ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Filestream, tID_FILESTREAM ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Reportstream, tID_REPORTSTREAM ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Task, tID_TASK ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Function, tID_FUNCTION ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Python, tID_PYTHON ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Plugin, tID_PLUGIN ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Menu, tID_MENU ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Stream, tID_STREAM ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Socket, tID_SOCKET ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Timer, tID_TIMER ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_TimeTable, tID_TIMETABLE ) );
  m_lexIdTokenMap.insert( intPair( IdManager::id_Form, tID_PROGRESSBAR ) );
}

//----------------------------------------------------------------------//
// initRwTabMap                                                         //
//----------------------------------------------------------------------//

void Flexer::initRwTabMap(){
  m_rwTabMap.insert(   "ABORT"           ,tABORT       );
  m_rwTabMap.insert(   "ABORTTRANSACTION" ,tABORTTRANSACTION );
  m_rwTabMap.insert(   "ABS"             ,tABS         );
  m_rwTabMap.insert(   "ACCORDION"       ,tACCORDION   );
  m_rwTabMap.insert(   "ACOS"            ,tACOS        );
  m_rwTabMap.insert(   "ALARM_COLOR"     ,tALARM_COLOR );
  m_rwTabMap.insert(   "ALARM_LEVEL"     ,tALARM_LEVEL );
  m_rwTabMap.insert(   "ALIGN"           ,tALIGN       );
  m_rwTabMap.insert(   "ALLCYCLES"       ,tALLCYCLES   );
  m_rwTabMap.insert(   "ALLOW"           ,tALLOW       );
  m_rwTabMap.insert(   "ALWAYS"          ,tALWAYS      );
  m_rwTabMap.insert(   "ANNOTATION"      ,tANNOTATION  );
  m_rwTabMap.insert(   "APP_MODAL"       ,tAPP_MODAL   );
  m_rwTabMap.insert(   "APPEND"          ,tAPPEND      );
  m_rwTabMap.insert(   "AREA"            ,tAREA        );
  m_rwTabMap.insert(   "ARG"             ,tARG         );
  m_rwTabMap.insert(   "ARROWS"          ,tARROWS      );
  m_rwTabMap.insert(   "ASCII"           ,tASCII       );
  m_rwTabMap.insert(   "ASIN"            ,tASIN        );
  m_rwTabMap.insert(   "ASPECT_RATIO"    ,tASPECT_RATIO );
  m_rwTabMap.insert(   "ASPECT_RATIO_REF_AXIS",tASPECT_RATIO_REF_AXIS );
  m_rwTabMap.insert(   "ASSIGN_CONSISTENCY", tASSIGN_CONSISTENCY);
  m_rwTabMap.insert(   "ASSIGN_CORR"     ,tASSIGN_CORR );
  m_rwTabMap.insert(   "ATAN"            ,tATAN        );
  m_rwTabMap.insert(   "ATAN2"           ,tATAN2       );
  m_rwTabMap.insert(   "ATTRS"           ,tATTRS );
  m_rwTabMap.insert(   "AUTO_SCROLL"     ,tAUTO_SCROLL );
  m_rwTabMap.insert(   "AUTOCLEAR_DEPENDENCIES" ,tAUTOCLEAR_DEPENDENCIES );
  m_rwTabMap.insert(   "AUTOLEVEL"       ,tAUTOLEVEL   );
  m_rwTabMap.insert(   "AUTO_WIDTH"      ,tAUTO_WIDTH  );
  m_rwTabMap.insert(   "AVG"             ,tAVG         );
  m_rwTabMap.insert(   "AXES_ORIGIN"     ,tAXES_ORIGIN );
  m_rwTabMap.insert(   "AXES_ORIGIN_X"   ,tAXES_ORIGIN_X );
  m_rwTabMap.insert(   "AXES_ORIGIN_Y"   ,tAXES_ORIGIN_Y );

  m_rwTabMap.insert(   "BAR"             ,tBAR         );
  m_rwTabMap.insert(   "BASE"            ,tBASE        );
  m_rwTabMap.insert(   "BASENAME"        ,tBASENAME    );
  m_rwTabMap.insert(   "BATCH"           ,tBATCH       );
  m_rwTabMap.insert(   "BEEP"            ,tBEEP        );
  m_rwTabMap.insert(   "BEGINTRANSACTION" ,tBEGINTRANSACTION );
  m_rwTabMap.insert(   "BINARY"          ,tBINARY      );
  m_rwTabMap.insert(   "BOTTOM"          ,tBOTTOM      );
  m_rwTabMap.insert(   "BUTTON"          ,tBUTTON      );
  m_rwTabMap.insert(   "BUTTON_CANCEL"   ,tBUTTON_CANCEL);
  m_rwTabMap.insert(   "BUTTON_NO"       ,tBUTTON_NO   );
  m_rwTabMap.insert(   "BUTTON_YES"      ,tBUTTON_YES  );
  m_rwTabMap.insert(   "BUTTONS"         ,tBUTTONS     );
  m_rwTabMap.insert(   "BY"              ,tBY          );

  m_rwTabMap.insert(   "CAPTION"         ,tCAPTION     );
  m_rwTabMap.insert(   "CDATA"           ,tCDATA       );
  m_rwTabMap.insert(   "CELL"            ,tCELL        );
  m_rwTabMap.insert(   "CENTER"          ,tJUSTCENTER  );
  m_rwTabMap.insert(   "CHANGED"         ,tCHANGED     );
  m_rwTabMap.insert(   "CLASS"           ,tCLASS       );
  m_rwTabMap.insert(   "CLASSNAME"       ,tCLASSNAME   );
  m_rwTabMap.insert(   "CLEAR"           ,tCLEAR       );
  m_rwTabMap.insert(   "CLEAR_SELECTION" ,tCLEAR_SELECTION );
  m_rwTabMap.insert(   "CLEARCYCLE"      ,tCLEARCYCLE  );
  m_rwTabMap.insert(   "CLOSE_BUTTON"    ,tCLOSE_BUTTON);
  m_rwTabMap.insert(   "COL"             ,tCOL         );
  m_rwTabMap.insert(   "COLOR"           ,tCOLOR       );
  m_rwTabMap.insert(   "COLOR_SCALE"     ,tCOLOR_SCALE );
  m_rwTabMap.insert(   "COLORBIT"        ,tCOLORBIT    );
  m_rwTabMap.insert(   "COLSPAN"         ,tCOLSPAN     );
  m_rwTabMap.insert(   "COMBOBOX"        ,tCOMBOBOX    );
  m_rwTabMap.insert(   "COMMITTRANSACTION" ,tCOMMITTRANSACTION );
  m_rwTabMap.insert(   "COMPARE"         ,tCOMPARE     );
  m_rwTabMap.insert(   "COMPLEX"         ,tCOMPLEX     );
  m_rwTabMap.insert(   "COMPOSE"         ,tCOMPOSE     );
  m_rwTabMap.insert(   "COMPOSE_STRING"  ,tCOMPOSE_STRING);
  m_rwTabMap.insert(   "CONFIRM"         ,tCONFIRM     );
  m_rwTabMap.insert(   "CONFIRM_CANCEL"  ,tCONFIRM_CANCEL);
  m_rwTabMap.insert(   "CONTOUR"         ,tCONTOUR     );
  m_rwTabMap.insert(   "COPY"            ,tCOPY        );
  m_rwTabMap.insert(   "COS"             ,tCOS         );
  m_rwTabMap.insert(   "CURRENT_DATE"    ,tCURRENT_DATE);
  m_rwTabMap.insert(   "CURRENT_DATETIME",tCURRENT_DATETIME);
  m_rwTabMap.insert(   "CURRENT_TIME"    ,tCURRENT_TIME);
  m_rwTabMap.insert(   "CYCLE"           ,tCYCLE       );
  m_rwTabMap.insert(   "CYCLENAME"       ,tCYCLENAME   );

  m_rwTabMap.insert(   "DAEMON"          ,tDAEMON      );
  m_rwTabMap.insert(   "DATA"            ,tDATA        );
  m_rwTabMap.insert(   "DATA_SIZE"       ,tDATA_SIZE   );
  m_rwTabMap.insert(   "DATAPOOL"        ,tDATAPOOL    );
  m_rwTabMap.insert(   "DATASET_TEXT"    ,tDATASET_TEXT);
  m_rwTabMap.insert(   "DBATTR"          ,tDBATTR );
  m_rwTabMap.insert(   "DBUNIT"          ,tDBUNIT );
  m_rwTabMap.insert(   "DEADLY_SILENT"   ,tDEADLY_SILENT);
  m_rwTabMap.insert(   "DEBUG"           ,tDEBUG       );
  m_rwTabMap.insert(   "DELAY"           ,tDELAY       );
  m_rwTabMap.insert(   "DELETE"          ,tDELETE );
  m_rwTabMap.insert(   "DELETECYCLE"     ,tDELETECYCLE );
  m_rwTabMap.insert(   "DELIMITER"       ,tDELIMITER   );
  m_rwTabMap.insert(   "DEPENDENCIES"    ,tDEPENDENCIES);
  m_rwTabMap.insert(   "DESCRIPTION"     ,tDESCRIPTION );
  m_rwTabMap.insert(   "DESCRIPTION_FILE",tDESCRIPTION_FILE );
  m_rwTabMap.insert(   "DIAGRAM"         ,tDIAGRAM     );
  m_rwTabMap.insert(   "DIAGRAM_XPOS"    ,tDIAGRAM_XPOS);
  m_rwTabMap.insert(   "DIAGRAM_YPOS"    ,tDIAGRAM_YPOS);
  m_rwTabMap.insert(   "SORT_CRITERIA"   ,tSORT_CRITERIA);
  m_rwTabMap.insert(   "DIRNAME"         ,tDIRNAME     );
  m_rwTabMap.insert(   "DISABLE"         ,tDISABLE     );
  m_rwTabMap.insert(   "DISABLE_DRAG"    ,tDISABLE_DRAG);
  m_rwTabMap.insert(   "DISALLOW"        ,tDISALLOW    );
  m_rwTabMap.insert(   "DISCRETE"        ,tDISCRETE    );
  m_rwTabMap.insert(   "DISPLAY"         ,tDISPLAY     );
  m_rwTabMap.insert(   "DOTS"            ,tDOTS        );
  m_rwTabMap.insert(   "DROP"            ,tDROP        );

  m_rwTabMap.insert(   "EDITABLE"        ,tEDITABLE    );
  m_rwTabMap.insert(   "ELSE"            ,tELSE        );
  m_rwTabMap.insert(   "ENABLE"          ,tENABLE      );
  m_rwTabMap.insert(   "END"             ,tEND         );
  m_rwTabMap.insert(   "EOLN"            ,tEOLN        );
  m_rwTabMap.insert(   "ERASE"           ,tERASE       );
  m_rwTabMap.insert(   "ERROR"           ,tERROR       );
  m_rwTabMap.insert(   "EXIT"            ,tEXIT        );
  m_rwTabMap.insert(   "EXPAND"          ,tEXPAND      );
  m_rwTabMap.insert(   "SCROLL"          ,tSCROLL      );
  m_rwTabMap.insert(   "EXPLICIT"        ,tEXPLICIT    );
  m_rwTabMap.insert(   "EXTENSION"       ,tEXTENSION   );

  m_rwTabMap.insert(   "FALSE"           ,tFALSE       );
  m_rwTabMap.insert(   "FATAL"           ,tFATAL       );
  m_rwTabMap.insert(   "FIELDGROUP"      ,tFIELDGROUP  );
  m_rwTabMap.insert(   "FILE"            ,tFILE        );
  m_rwTabMap.insert(   "FILENAME"        ,tFILENAME    );
  m_rwTabMap.insert(   "FILESTREAM"      ,tFILESTREAM  );
  m_rwTabMap.insert(   "FILTER"          ,tFILTER      );
  m_rwTabMap.insert(   "FIRSTCYCLE"      ,tFIRSTCYCLE  );
  m_rwTabMap.insert(   "FIRSTLEVEL"      ,tFIRSTLEVEL  );
  m_rwTabMap.insert(   "FOLDER"          ,tFOLDER      );
  m_rwTabMap.insert(   "FORM"            ,tFORM        );
  m_rwTabMap.insert(   "FORMAT"          ,tFORMAT      );
  m_rwTabMap.insert(   "FORTRAN"         ,tFORTRAN     );
  m_rwTabMap.insert(   "FRAME"           ,tFRAME       );
  m_rwTabMap.insert(   "FROM_STRING_DATETIME" ,tFROM_STRING_DATETIME);
  m_rwTabMap.insert(   "FUNC"            ,tFUNC        );
  m_rwTabMap.insert(   "FOCUS_FUNC"      ,tFOCUS_FUNC  );
  m_rwTabMap.insert(   "FUNCTIONS"       ,tFUNCTIONS   );

  m_rwTabMap.insert(   "GET"             ,tGET         );
  m_rwTabMap.insert(   "GET_SELECTION"   ,tGET_SELECTION);
  m_rwTabMap.insert(   "GET_SORT_CRITERIA", tGET_SORT_CRITERIA);
  m_rwTabMap.insert(   "GETCYCLE"        ,tGETCYCLE    );
  m_rwTabMap.insert(   "GETTEXT"         ,tGETTEXT     );
  m_rwTabMap.insert(   "GLOBAL"          ,tGLOBAL      );
  m_rwTabMap.insert(   "GOCYCLE"         ,tGOCYCLE     );
  m_rwTabMap.insert(   "GUI_ELEMENT_METHOD", tGUI_ELEMENT_METHOD);

  m_rwTabMap.insert(   "HEADER"          ,tHEADER      );
  m_rwTabMap.insert(   "HELPFILE"        ,tHELPFILE    );
  m_rwTabMap.insert(   "HELPKEY"         ,tHELPKEY     );
  m_rwTabMap.insert(   "HELPTEXT"        ,tHELPTEXT    );
  m_rwTabMap.insert(   "HIDDEN"          ,tHIDDEN      );
  m_rwTabMap.insert(   "HIDE_CYCLE"      ,tHIDE_CYCLE  );
  m_rwTabMap.insert(   "HIDE_EMPTY_FOLDER",tHIDE_EMPTY_FOLDER );
  m_rwTabMap.insert(   "HIGH"            ,tHIGH        );
  m_rwTabMap.insert(   "HORIZONTAL"      ,tHORIZONTAL  );
  m_rwTabMap.insert(   "HOST"            ,tHOST        );

  m_rwTabMap.insert(   "ICON"            ,tICON        );
  m_rwTabMap.insert(   "ICONVIEW"        ,tICONVIEW    );
  m_rwTabMap.insert(   "IF"              ,tIF          );
  m_rwTabMap.insert(   "IGNORE"          ,tIGNORE      );
  m_rwTabMap.insert(   "IMAG"            ,tIMAG        );
  m_rwTabMap.insert(   "IMAGE"           ,tIMAGE       );
  m_rwTabMap.insert(   "IN"              ,tIN          );
  m_rwTabMap.insert(   "INDENT"          ,tINDENT      );
  m_rwTabMap.insert(   "INDEX"           ,tINDEX       );
  m_rwTabMap.insert(   "INDEXED_SET"     ,tINDEXED_SET );
  m_rwTabMap.insert(   "INFO"            ,tINFO        );
  m_rwTabMap.insert(   "INPUT"           ,tINPUT       );
  m_rwTabMap.insert(   "INT"             ,tINTEGER     );
  m_rwTabMap.insert(   "INTEGER"         ,tINTEGER     );
  m_rwTabMap.insert(   "INVALID"         ,tINVALID     );
  m_rwTabMap.insert(   "INVERTED"        ,tINVERTED    );

  m_rwTabMap.insert(   "JSON"            ,tJSON        );
  m_rwTabMap.insert(   "PROTO"           ,tPROTO       );
  m_rwTabMap.insert(   "JUSTIFY"         ,tJUSTIFY     );
  m_rwTabMap.insert(   "JUSTLEFT"        ,tJUSTLEFT    );
  m_rwTabMap.insert(   "JUSTRIGHT"       ,tJUSTRIGHT   );

  m_rwTabMap.insert(   "LABEL"           ,tLABEL       );
  m_rwTabMap.insert(   "LASTCYCLE"       ,tLASTCYCLE   );
  m_rwTabMap.insert(   "LASTLEVEL"       ,tLASTLEVEL   );
  m_rwTabMap.insert(   "LATEX"           ,tLATEX       );
  m_rwTabMap.insert(   "LATEXREPORT"     ,tLATEXREPORT );
  m_rwTabMap.insert(   "LEFT"            ,tLEFT        );
  m_rwTabMap.insert(   "LEGEND"          ,tLEGEND      );
  m_rwTabMap.insert(   "LENGTH"          ,tLENGTH      );
  m_rwTabMap.insert(   "LINEAR"          ,tLINEAR      );
  m_rwTabMap.insert(   "LINEPLOT"        ,tLINEPLOT    );
  m_rwTabMap.insert(   "LINESTYLE"       ,tLINESTYLE   );
  m_rwTabMap.insert(   "LIST"            ,tLIST        );
  m_rwTabMap.insert(   "LISTPLOT"        ,tLISTPLOT    );
  m_rwTabMap.insert(   "LOAD"            ,tLOAD        );
  m_rwTabMap.insert(   "LOCALE"          ,tLOCALE      );
  m_rwTabMap.insert(   "LOCK"            ,tLOCK        );
  m_rwTabMap.insert(   "LOCKABLE"        ,tLOCKABLE    );
  m_rwTabMap.insert(   "LOG"             ,tLOG         );
  m_rwTabMap.insert(   "LOG10"           ,tLOG10       );
  m_rwTabMap.insert(   "LOG_WINDOW"      ,tLOG_WINDOW  );
  m_rwTabMap.insert(   "LOG_X"           ,tLOG_X       );
  m_rwTabMap.insert(   "LOG_Y"           ,tLOG_Y       );
  m_rwTabMap.insert(   "LOGOFF"          ,tLOGOFF      );
  m_rwTabMap.insert(   "LOGON"           ,tLOGON       );

  m_rwTabMap.insert(   "MAIN"            ,tMAIN        );
  m_rwTabMap.insert(   "MAP"             ,tMAP         );
  m_rwTabMap.insert(   "MARGIN"          ,tMARGIN      );
  m_rwTabMap.insert(   "MARKER"          ,tMARKER      );
  m_rwTabMap.insert(   "MATLAB"          ,tMATLAB      );
  m_rwTabMap.insert(   "MATRIX"          ,tMATRIX      );
  m_rwTabMap.insert(   "MAX_OCCURS"      ,tMAX_OCCURS  );
  m_rwTabMap.insert(   "MAX_PENDING_FUNCTIONS" ,tMAX_PENDING_FUNCTIONS);
  m_rwTabMap.insert(   "MAXCYCLE"        ,tMAXCYCLE    );
  m_rwTabMap.insert(   "MENU"            ,tMENU        );
  m_rwTabMap.insert(   "MESSAGE_QUEUE"   ,tMESSAGE_QUEUE);
  m_rwTabMap.insert(   "MESSAGEBOX"      ,tMESSAGEBOX  );
  m_rwTabMap.insert(   "MFM"             ,tMFM         );
  m_rwTabMap.insert(   "MINMAX"          ,tMINMAX      );
  m_rwTabMap.insert(   "MODIFIED"        ,tMODIFIED    );
  m_rwTabMap.insert(   "MODIFY"          ,tMODIFY      );
  m_rwTabMap.insert(   "MOVE"            ,tMOVE        );
  m_rwTabMap.insert(   "MULTIPLE_SELECTION", tMULTIPLE_SELECTION);

  m_rwTabMap.insert(   "NAMESPACE"       ,tNAMESPACE   );
  m_rwTabMap.insert(   "NAVIGATION"      ,tNAVIGATION  );
  m_rwTabMap.insert(   "NAVIGATOR"       ,tNAVIGATOR   );
  m_rwTabMap.insert(   "NEWCYCLE"        ,tNEWCYCLE    );
  m_rwTabMap.insert(   "NEXTCYCLE"       ,tNEXTCYCLE   );
  m_rwTabMap.insert(   "NO_COLORBIT"     ,tNO_COLORBIT );
  m_rwTabMap.insert(   "NO_DEPENDENCIES" ,tNO_DEPENDENCIES );
  m_rwTabMap.insert(   "NO_GZ"           ,tNO_GZ       );
  m_rwTabMap.insert(   "NO_LOG"          ,tNO_LOG      );
  m_rwTabMap.insert(   "NO_PANEDWINDOW"  ,tNO_PANEDWINDOW);
  m_rwTabMap.insert(   "NO_SCROLLBARS"   ,tNO_SCROLLBARS);
  m_rwTabMap.insert(   "NODE"            ,tNODE        );
  m_rwTabMap.insert(   "NONE"            ,tNONE        );
  m_rwTabMap.insert(   "NP"              ,tNO_PANEDWINDOW);
  m_rwTabMap.insert(   "NS"              ,tNO_SCROLLBARS);

  m_rwTabMap.insert(   "OF"              ,tOF          );
  m_rwTabMap.insert(   "OFFSET"          ,tOFFSET      );
  m_rwTabMap.insert(   "OLDVALUE"        ,tOLDVALUE    );
  m_rwTabMap.insert(   "OMIT_TTRAIL"     ,tOMIT_TTRAIL );
  m_rwTabMap.insert(   "OMIT_ACTIVATE"   ,tOMIT_ACTIVATE);
  m_rwTabMap.insert(   "ON_EOS"          ,tON_EOS      );
  m_rwTabMap.insert(   "ON_VIEW_ACTION"  ,tON_VIEW_ACTION );
  m_rwTabMap.insert(   "OPEN"            ,tOPEN        );
  m_rwTabMap.insert(   "OPEN_FILE"       ,tOPEN_FILE   );
  m_rwTabMap.insert(   "OPEN_URL"        ,tOPEN_URL    );
  m_rwTabMap.insert(   "OPENLEVELS"      ,tOPENLEVELS  );
  m_rwTabMap.insert(   "OPERATOR"        ,tOPERATOR    );
  m_rwTabMap.insert(   "OPTIONAL"        ,tOPTIONAL    );
  m_rwTabMap.insert(   "ORIENTATION"     ,tORIENTATION );
  m_rwTabMap.insert(   "OUT"             ,tOUT         );
  m_rwTabMap.insert(   "OVERLAY"         ,tOVERLAY     );

  m_rwTabMap.insert(   "PACK"            ,tPACK        );
  m_rwTabMap.insert(   "PANEDWINDOW"     ,tPANEDWINDOW );
  m_rwTabMap.insert(   "PARENT"          ,tPARENT      );
  m_rwTabMap.insert(   "PASSWORD"        ,tPASSWORD    );
  m_rwTabMap.insert(   "PASTE"           ,tPASTE       );
  m_rwTabMap.insert(   "PATH"            ,tPATH        );
  m_rwTabMap.insert(   "PATTERN"         ,tPATTERN     );
  m_rwTabMap.insert(   "PERIOD"          ,tPERIOD      );
  m_rwTabMap.insert(   "PERSISTENT"      ,tPERSISTENT  );
  m_rwTabMap.insert(   "PIXMAP"          ,tPIXMAP      );
  m_rwTabMap.insert(   "PLACEHOLDER"     ,tPLACEHOLDER );
  m_rwTabMap.insert(   "PLOT"            ,tPLOT        );
  m_rwTabMap.insert(   "PLOT2D"          ,tPLOT2D      );
  m_rwTabMap.insert(   "PLOT3D"          ,tPLOT3D      );
  m_rwTabMap.insert(   "PLOTGROUP"       ,tPLOTGROUP   );
  m_rwTabMap.insert(   "PLUGIN"          ,tPLUGIN      );
  m_rwTabMap.insert(   "POLAR"           ,tPOLAR       );
  m_rwTabMap.insert(   "PORT"            ,tPORT        );
  m_rwTabMap.insert(   "PORT_REQUEST"    ,tPORT_REQUEST);
  m_rwTabMap.insert(   "POSITION"        ,tPOSITION    );
  m_rwTabMap.insert(   "POST"            ,tPOST        );
  m_rwTabMap.insert(   "POSTGRES"        ,tPOSTGRES    );
  m_rwTabMap.insert(   "PREVIEW"         ,tPREVIEW     );
  m_rwTabMap.insert(   "PRINT"           ,tPRINT       );
  m_rwTabMap.insert(   "PRINT_LOG"       ,tPRINT_LOG   );
  m_rwTabMap.insert(   "PRINTSTYLE"      ,tPRINTSTYLE  );
  m_rwTabMap.insert(   "PRIORITY"        ,tPRIORITY    );
  m_rwTabMap.insert(   "PROCESS"         ,tPROCESS     );
  m_rwTabMap.insert(   "PROCESSGROUP"    ,tPROCESSGROUP);
  m_rwTabMap.insert(   "PSPLOT"          ,tPSPLOT      );
  m_rwTabMap.insert(   "PUBLISH"         ,tPUBLISH     );
  m_rwTabMap.insert(   "PUT"             ,tPUT         );
  m_rwTabMap.insert(   "PW"              ,tPANEDWINDOW );

  m_rwTabMap.insert(   "QUERY"           ,tQUERY       );

  m_rwTabMap.insert(   "RADIO"           ,tRADIO       );
  m_rwTabMap.insert(   "RANGE"           ,tRANGE       );
  m_rwTabMap.insert(   "READONLY"        ,tREADONLY    );
  m_rwTabMap.insert(   "REAL"            ,tREAL        );
  m_rwTabMap.insert(   "REASON"          ,tREASON      );
  m_rwTabMap.insert(   "REASON_ACTIVATE" ,tREASON_ACTIVATE);
  m_rwTabMap.insert(   "REASON_CLEAR"    ,tREASON_CLEAR);
  m_rwTabMap.insert(   "REASON_CLOSE"    ,tREASON_CLOSE);
  m_rwTabMap.insert(   "REASON_CONNECTION",tREASON_NEW_CONNECTION);
  m_rwTabMap.insert(   "REASON_CYCLE_CLEAR"    ,tREASON_CYCLE_CLEAR);
  m_rwTabMap.insert(   "REASON_CYCLE_DELETE"    ,tREASON_CYCLE_DELETE);
  m_rwTabMap.insert(   "REASON_CYCLE_NEW"    ,tREASON_CYCLE_NEW);
  m_rwTabMap.insert(   "REASON_CYCLE_RENAME"    ,tREASON_CYCLE_RENAME);
  m_rwTabMap.insert(   "REASON_CYCLE_SWITCH"    ,tREASON_CYCLE_SWITCH);
  m_rwTabMap.insert(   "REASON_DROP"     ,tREASON_DROP );
  m_rwTabMap.insert(   "REASON_DUPLICATE",tREASON_DUPLICATE);
  m_rwTabMap.insert(   "REASON_FOCUS"    ,tREASON_FOCUS);
  m_rwTabMap.insert(   "REASON_FOCUS_OUT",tREASON_FOCUS_OUT);
  m_rwTabMap.insert(   "REASON_FUNCTION" ,tREASON_FUNCTION );
  m_rwTabMap.insert(   "REASON_GUI_UPDATE" ,tREASON_GUI_UPDATE );
  m_rwTabMap.insert(   "REASON_INPUT"    ,tREASON_INPUT);
  m_rwTabMap.insert(   "REASON_INSERT"   ,tREASON_INSERT);
  m_rwTabMap.insert(   "REASON_MOVE"     ,tREASON_MOVE );
  m_rwTabMap.insert(   "REASON_OPEN"     ,tREASON_OPEN );
  m_rwTabMap.insert(   "REASON_PACK"     ,tREASON_PACK );
  m_rwTabMap.insert(   "REASON_REMOVE"   ,tREASON_REMOVE);
  m_rwTabMap.insert(   "REASON_REMOVE_CONNECTION",tREASON_REMOVE_CONNECTION);
  m_rwTabMap.insert(   "REASON_REMOVE_ELEMENT",tREASON_REMOVE_ELEMENT);
  m_rwTabMap.insert(   "REASON_SELECT_POINT",     tREASON_SELECT_POINT);
  m_rwTabMap.insert(   "REASON_SELECT_RECTANGLE", tREASON_SELECT_RECTANGLE);
  m_rwTabMap.insert(   "REASON_SELECT"   ,tREASON_SELECT);
  m_rwTabMap.insert(   "REASON_SORT"     ,tREASON_SORT);
  m_rwTabMap.insert(   "REASON_TASK"     ,tREASON_TASK );
  m_rwTabMap.insert(   "REASON_UNSELECT" ,tREASON_UNSELECT);

  m_rwTabMap.insert(   "REPLACE"         ,tREPLACE     );
  m_rwTabMap.insert(   "REPLY"           ,tREPLY       );
  m_rwTabMap.insert(   "REPORT"          ,tREPORT      );
  m_rwTabMap.insert(   "REPORTSTREAM"    ,tREPORTSTREAM);
  m_rwTabMap.insert(   "REQUEST"         ,tREQUEST     );
  m_rwTabMap.insert(   "RESET"           ,tRESET       );
  m_rwTabMap.insert(   "RESET_ERROR"     ,tRESET_ERROR );
  m_rwTabMap.insert(   "RESPONSE"        ,tRESPONSE    );
  m_rwTabMap.insert(   "REST_JWT_LOGON"  ,tREST_JWT_LOGON);
  m_rwTabMap.insert(   "REST_LOGOFF"     ,tREST_LOGOFF );
  m_rwTabMap.insert(   "REST_LOGON"      ,tREST_LOGON  );
  m_rwTabMap.insert(   "RETURN"          ,tRETURN      );
  m_rwTabMap.insert(   "RIGHT"           ,tRIGHT       );
  m_rwTabMap.insert(   "RMS"             ,tRMS         );
  m_rwTabMap.insert(   "ROTATE_180"      ,tROTATE_180  );
  m_rwTabMap.insert(   "ROUND"           ,tROUND       );
  m_rwTabMap.insert(   "ROUND10"         ,tROUND10     );
  m_rwTabMap.insert(   "ROUND5"          ,tROUND5      );
  m_rwTabMap.insert(   "ROW"             ,tROW         );
  m_rwTabMap.insert(   "ROWSPAN"         ,tROWSPAN     );
  m_rwTabMap.insert(   "RUN"             ,tRUN         );

  m_rwTabMap.insert(   "SAME_YRANGE"     ,tSAME_YRANGE );
  m_rwTabMap.insert(   "SAVE"            ,tSAVE        );
  m_rwTabMap.insert(   "SB"              ,tSCROLLBARS  );
  m_rwTabMap.insert(   "SCALAR"          ,tSCALAR      );
  m_rwTabMap.insert(   "SCALE"           ,tSCALE       );
  m_rwTabMap.insert(   "SCHEMA"          ,tSCHEMA      );
  m_rwTabMap.insert(   "SCROLLBARS"      ,tSCROLLBARS  );
  m_rwTabMap.insert(   "SELECT_LIST"     ,tSELECT_LIST );
  m_rwTabMap.insert(   "SEND"            ,tSEND        );
  m_rwTabMap.insert(   "SEPARATOR"       ,tSEPARATOR   );
  m_rwTabMap.insert(   "SERIALIZE"       ,tSERIALIZE   );
  m_rwTabMap.insert(   "SERIALIZE_FORM"  ,tSERIALIZE_FORM);
  m_rwTabMap.insert(   "SET"             ,tSET         );
  m_rwTabMap.insert(   "SET_CURRENT_FORM",tSET_CURRENT_FORM);
  m_rwTabMap.insert(   "SET_DB_TIMESTAMP",tSET_DB_TIMESTAMP);
  m_rwTabMap.insert(   "SET_ERROR"       ,tSET_ERROR   );
  m_rwTabMap.insert(   "SET_FUNC_ARGS"   ,tSET_FUNC_ARGS);
  m_rwTabMap.insert(   "SET_INDEX"       ,tSET_INDEX   );
  m_rwTabMap.insert(   "SET_MQ_HOST"     ,tSET_MQ_HOST );
  m_rwTabMap.insert(   "SET_MSG"         ,tSET_MSG     );
  m_rwTabMap.insert(   "SET_REASON"      ,tSET_REASON  );
  m_rwTabMap.insert(   "SET_RESOURCE"    ,tSET_RESOURCE);
  m_rwTabMap.insert(   "SET_THIS"        ,tSET_THIS    );
  m_rwTabMap.insert(   "SETTINGS"        ,tSETTINGS    );
  m_rwTabMap.insert(   "SILENT"          ,tSILENT      );
  m_rwTabMap.insert(   "SIN"             ,tSIN         );
  m_rwTabMap.insert(   "SIZE"            ,tSIZE        );
  m_rwTabMap.insert(   "SKIP"            ,tSKIP        );
  m_rwTabMap.insert(   "SLIDER"          ,tSLIDER      );
  m_rwTabMap.insert(   "PROGRESS"        ,tPROGRESS    );
  m_rwTabMap.insert(   "SOCKET"          ,tSOCKET      );
  m_rwTabMap.insert(   "SORT"            ,tSORT        );
  m_rwTabMap.insert(   "SORTORDER"       ,tSORTORDER   );
  m_rwTabMap.insert(   "SOURCE"          ,tSOURCE      );
  m_rwTabMap.insert(   "SOURCE2"         ,tSOURCE2     );
  m_rwTabMap.insert(   "SQRT"            ,tSQRT        );
  m_rwTabMap.insert(   "STACKING_BAR"    ,tSTACKING_BAR);
  m_rwTabMap.insert(   "START"           ,tSTART       );
  m_rwTabMap.insert(   "STD_WINDOW"      ,tSTD_WINDOW  );
  m_rwTabMap.insert(   "STEP"            ,tSTEP        );
  m_rwTabMap.insert(   "STOP"            ,tSTOP        );
  m_rwTabMap.insert(   "STREAM"          ,tSTREAM      );
  m_rwTabMap.insert(   "STREAMER"        ,tSTREAMER    );
  m_rwTabMap.insert(   "STRETCH"         ,tSTRETCH     );
  m_rwTabMap.insert(   "STRING"          ,tSTRING      );
  m_rwTabMap.insert(   "STRING_DATE"     ,tSTRING_DATE );
  m_rwTabMap.insert(   "STRING_DATETIME" ,tSTRING_DATETIME);
  m_rwTabMap.insert(   "STRING_TIME"     ,tSTRING_TIME );
  m_rwTabMap.insert(   "STRING_VALUE"    ,tSTRING_VALUE);
  m_rwTabMap.insert(   "STRUCT"          ,tSTRUCT      );
  m_rwTabMap.insert(   "STYLE"           ,tSTYLE       );
  m_rwTabMap.insert(   "STYLESHEET"      ,tSTYLESHEET  );
  m_rwTabMap.insert(   "SUBSCRIBE"       ,tSUBSCRIBE   );
  m_rwTabMap.insert(   "SURFACE"         ,tSURFACE     );

  m_rwTabMap.insert(   "TABLE"           ,tTABLE       );
  m_rwTabMap.insert(   "TABLESIZE"       ,tTABLESIZE   );
  m_rwTabMap.insert(   "TAG"             ,tTAG         );
  m_rwTabMap.insert(   "TAN"             ,tTAN         );
  m_rwTabMap.insert(   "TASK"            ,tTASK        );
  m_rwTabMap.insert(   "TEMPLATE"        ,tTEMPLATE    );
  m_rwTabMap.insert(   "TEXT_WINDOW"     ,tTEXT_WINDOW );
  m_rwTabMap.insert(   "THERMO"          ,tTHERMO      );
  m_rwTabMap.insert(   "THIS"            ,tTHIS        );
  m_rwTabMap.insert(   "THUMBNAIL"       ,tTHUMBNAIL   );
  m_rwTabMap.insert(   "TIMEOUT"         ,tTIMEOUT     );
  m_rwTabMap.insert(   "TIMER"           ,tTIMER       );
  m_rwTabMap.insert(   "TIMESTAMP"       ,tTIMESTAMP   );
  m_rwTabMap.insert(   "TIMETABLE"       ,tTIMETABLE   );
  m_rwTabMap.insert(   "TO_STRING_DATETIME"   ,tTO_STRING_DATETIME);
  m_rwTabMap.insert(   "TO_STRING_TIME"  ,tTO_STRING_TIME);
  m_rwTabMap.insert(   "TOGGLE"          ,tTOGGLE      );
  m_rwTabMap.insert(   "TOOLTIP"         ,tTOOLTIP     );
  m_rwTabMap.insert(   "TOP"             ,tTOP         );
  m_rwTabMap.insert(   "TOUCH"           ,tTOUCH       );
  m_rwTabMap.insert(   "TRANSACTION"     ,tTRANSACTION );
  m_rwTabMap.insert(   "TRANSIENT"       ,tTRANSIENT   );
  m_rwTabMap.insert(   "TRANSPARENT"     ,tTRANSPARENT   );
  m_rwTabMap.insert(   "TRUE"            ,tTRUE        );
  m_rwTabMap.insert(   "TSEP"            ,tTSEP        );

  m_rwTabMap.insert(   "UI_MANAGER"      ,tUI_MANAGER  );
  m_rwTabMap.insert(   "UI_UPDATE"       ,tUI_UPDATE   );
  m_rwTabMap.insert(   "UNIPLOT"         ,tUNIPLOT     );
  m_rwTabMap.insert(   "UNIT"            ,tUNIT        );
  m_rwTabMap.insert(   "UNITS"           ,tUNIT        );
  m_rwTabMap.insert(   "UNMAP"           ,tUNMAP       );
  m_rwTabMap.insert(   "UNSET"           ,tUNSET       );
  m_rwTabMap.insert(   "UPDATE_FORMS"    ,tUPDATE_FORMS);
  m_rwTabMap.insert(   "URL"             ,tURL         );

  m_rwTabMap.insert(   "VALID"           ,tVALID       );
  m_rwTabMap.insert(   "VAR"             ,tVAR         );
  m_rwTabMap.insert(   "VERSION"         ,tVERSION     );
  m_rwTabMap.insert(   "VERTICAL"        ,tVERTICAL    );
  m_rwTabMap.insert(   "VISIBLE"         ,tVISIBLE     );
  m_rwTabMap.insert(   "VOID"            ,tVOID        );

  m_rwTabMap.insert(   "WARN"            ,tWARN        );
  m_rwTabMap.insert(   "WEBAPI"          ,tWEBAPI      );
  m_rwTabMap.insert(   "WHEEL_EVENT"     ,tWHEEL_EVENT );
  m_rwTabMap.insert(   "WHILE"           ,tWHILE       );
  m_rwTabMap.insert(   "WRAP"            ,tWRAP        );
  m_rwTabMap.insert(   "WRITE_SETTINGS"  ,tWRITE_SETTINGS);
  m_rwTabMap.insert(   "WRITEONLY"       ,tWRITEONLY   );

  m_rwTabMap.insert(   "XANNOTATION"     ,tXANNOTATION );
  m_rwTabMap.insert(   "XAXIS"           ,tXAXIS       );
  m_rwTabMap.insert(   "XAXIS2"          ,tXAXIS2      );
  m_rwTabMap.insert(   "XML"             ,tXML         );
  m_rwTabMap.insert(   "XMLGROUP"        ,tXMLGROUP    );
  m_rwTabMap.insert(   "XRT3DPLOT"       ,tXRT3DPLOT   );
  m_rwTabMap.insert(   "XRTGRAPH"        ,tXRTGRAPH    );

  m_rwTabMap.insert(   "Y1_STYLE"        ,tY1_STYLE    );
  m_rwTabMap.insert(   "Y2_STYLE"        ,tY2_STYLE    );
  m_rwTabMap.insert(   "YANNOTATION"     ,tYANNOTATION );
  m_rwTabMap.insert(   "YAXIS"           ,tYAXIS       );
  m_rwTabMap.insert(   "YAXIS1"          ,tYAXIS1      );
  m_rwTabMap.insert(   "YAXIS2"          ,tYAXIS2      );
}

void Flexer::initInclFileList(){
  std::string inclFiles( AppData::Instance().IncludePath() );
  std::string newPath;
  if( !inclFiles.empty() ){
    std::string::size_type startPos = 0, endPos = 0;

    while( endPos != std::string::npos ){
      endPos = inclFiles.find( ":" , startPos );
      if( endPos == std::string::npos ){
        newPath = std::string( inclFiles, startPos, inclFiles.size() - startPos );
      }
      else{
        newPath = std::string( inclFiles, startPos, endPos - startPos );
        startPos = endPos + 1;
      }
      if( !newPath.empty() && newPath.at( ( newPath.length() - 1) ) != *("/") )
        newPath+= "/";
      m_inclPathList.push_back( newPath );
    }
  }
  if( m_inclPathList.empty() )
    m_inclPathList.push_back( std::string() );
}

Flexer::InclFile::InclFile( yy_buffer_state *include, const std::string &filename, int lineno )
  : m_include( include )
  , m_filename( filename )
  , m_lineno( lineno ){
}
