
#include <assert.h>
#include <sstream>
#ifdef HAVE_HEADLESS
#include "HeadlessTimer.h"
#include "HeadlessGuiLabel.h"
#include "HeadlessGuiForm.h"
#include "HeadlessGuiPlot2d.h"
#include "HeadlessGuiManager.h"
#include "HeadlessGuiMenubar.h"
#include "HeadlessGuiDataField.h"
#include "HeadlessGuiButtonbar.h"
#include "HeadlessGuiFieldgroup.h"
#include "HeadlessGuiMenuButton.h"
#include "HeadlessGuiApplHeader.h"
#include "HeadlessGuiPulldownMenu.h"
#include "HeadlessGuiScrolledText.h"
#include "HeadlessGuiPrinterDialog.h"
#endif
#include "HeadlessGuiFactory.h"
#include "gui/InformationListener.h"
#include "gui/FileSelectListener.h"
#include "job/JobManager.h"
#include "job/JobController.h"
#include "operator/MessageQueueThreads.h"
#include "operator/FileStream.h"
#include "utils/Debugger.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"
#include "utils/base64.h"
#include "utils/FileUtilities.h"
#include "utils/HTMLConverter.h"

INIT_LOGGER();

HeadlessGuiFactory::HeadlessGuiFactory(){}
HeadlessGuiFactory::~HeadlessGuiFactory(){}

GuiManager *HeadlessGuiFactory::createManager(){
#ifdef HAVE_HEADLESS
  return HeadlessGuiManager::createManager();
#endif
  return 0;
}

bool HeadlessGuiFactory::have2dPlot(){return true;}

Timer              *HeadlessGuiFactory::createTimer( int interval, bool restart ){
#ifdef HAVE_HEADLESS
  return new HeadlessTimer( interval, restart );
#endif
  return 0;
}

void                HeadlessGuiFactory::createSimpleAction( SimpleAction::Object *obj ){}

GuiForm            *HeadlessGuiFactory::createForm( const std::string& name ){
  if( GuiElement::findElement( name ) )
    return 0;
#ifdef HAVE_HEADLESS
  return new HeadlessGuiForm( 0, name );
#endif
  return 0;
}

GuiApplHeader      *HeadlessGuiFactory::createApplHeader( GuiElement *parent ){
#ifdef HAVE_HEADLESS
  return new HeadlessGuiApplHeader(parent);
#endif
  return 0;
}

GuiFolder          *HeadlessGuiFactory::createFolder( GuiElement *parent, const std::string & ){assert(false);return 0;}

GuiIndexMenu *HeadlessGuiFactory::createIndexMenu( GuiElement *parent, GuiIndexMenu::HideFlag flag) {
  assert(false);return 0;
}

GuiMenubar         *HeadlessGuiFactory::createMenubar( GuiElement *parent ){
#ifdef HAVE_HEADLESS
  return new HeadlessGuiMenubar( parent );
#endif
  return 0;
}

GuiPulldownMenu    *HeadlessGuiFactory::createPulldownMenu( GuiElement *parent, const std::string &name ){
#ifdef HAVE_HEADLESS
  return new HeadlessGuiPulldownMenu( parent, name );
#endif
  return 0;
}

GuiPopupMenu       *HeadlessGuiFactory::createPopupMenu( GuiElement *parent ){assert(false);return 0;}
GuiPopupMenu       *HeadlessGuiFactory::createNavigatorMenu( GuiElement *parent ){assert(false);return 0;}

GuiMenuButton      *HeadlessGuiFactory::createMenuButton( GuiElement *parent, GuiButtonListener *listener, GuiEventData *event, bool hide_disabled, const std::string& name ){
#ifdef HAVE_HEADLESS
  return new HeadlessGuiMenuButton( parent, listener, event, hide_disabled, name );
#endif
  return 0;
}

GuiMenuToggle      *HeadlessGuiFactory::createMenuToggle( GuiElement *parent, GuiToggleListener *listener, GuiEventData *event ){assert(false);return 0;}
GuiSeparator       *HeadlessGuiFactory::createSeparator( GuiElement *parent ){assert(false);return 0;}
GuiMessageLine     *HeadlessGuiFactory::createMessageLine( GuiElement *parent ){assert(false);return 0;}
GuiScrolledText    *HeadlessGuiFactory::createScrolledText( GuiElement *parent, const std::string &name ){assert(false);return 0;}
GuiScrolledlist    *HeadlessGuiFactory::createScrolledlist(GuiElement *parent, GuiScrolledlistListener *listener){assert(false);return 0;}

GuiLabel           *HeadlessGuiFactory::createLabel( GuiElement *parent, GuiElement::Alignment a ){
#ifdef HAVE_HEADLESS
  return new HeadlessGuiLabel( parent );
#endif
  return 0;
}

GuiButton          *HeadlessGuiFactory::createButton( GuiElement *parent, GuiButtonListener *listener, GuiEventData *event ){assert(false);return 0;}
GuiDataField       *HeadlessGuiFactory::createToggle( GuiElement *parent ){assert(false);return 0;}

GuiFieldgroup *HeadlessGuiFactory::createFieldgroup( GuiElement *parent,
                                                const std::string &name ){
  if( GuiElement::findElement( name ) )
    return 0;
#ifdef HAVE_HEADLESS
  HeadlessGuiFieldgroup *e = new HeadlessGuiFieldgroup( parent, name );
  return e;
#endif
  return 0;
}

GuiFieldgroup *HeadlessGuiFactory::createFieldgroup( GuiElement *parent,
                                               GuiElement *at,
                                               const std::string &name ){
  if( GuiElement::findElement( name ) )
    return 0;
#ifdef HAVE_HEADLESS
  HeadlessGuiFieldgroup *e = new HeadlessGuiFieldgroup( parent, name );
  at->attach( e );
  return e;
#endif
  return 0;
}
GuiFieldgroupLine *HeadlessGuiFactory::createFieldgroupLine( GuiElement *parent) {
  assert(false);return 0;
}

GuiOrientationContainer *HeadlessGuiFactory::createVerticalContainer( GuiElement *parent ){assert(false);return 0;}
GuiOrientationContainer *HeadlessGuiFactory::createHorizontalContainer( GuiElement *parent ){assert(false);return 0;}

GuiButtonbar       *HeadlessGuiFactory::createButtonbar( GuiElement *parent ){
#ifdef HAVE_HEADLESS
  return new HeadlessGuiButtonbar( parent );
#endif
  return 0;
}

DialogCopyright    *HeadlessGuiFactory::createDialogCopyright(){assert(false);return 0;}
GuiCycleButton     *HeadlessGuiFactory::createCycleButton( GuiElement *parent ){assert(false);return 0;}
GuiTable           *HeadlessGuiFactory::createTable( GuiElement *parent, const std::string &name ){assert(false);return 0;}
GuiTableItem       *HeadlessGuiFactory::createTableLabelItem( GuiElement *parent, const std::string &label, GuiElement::Alignment align ){assert(false);return 0;}
GuiTableItem       *HeadlessGuiFactory::createTableDataItem( GuiElement *parent ){assert(false);return 0;}
GuiTableItem       *HeadlessGuiFactory::createTableComboBoxItem( GuiElement *parent, const std::string& datasetName ){assert(false);return 0;}
GuiList            *HeadlessGuiFactory::createList( GuiElement *parent, const std::string &name ){assert(false);return 0;}
GuiFilterList      *HeadlessGuiFactory::createFilterList( GuiElement *parent, GuiFilterListListener &listener,
							   const std::string &name ){assert(false);return 0;}

GuiPrinterDialog   *HeadlessGuiFactory::createPrinterDialog(){
#ifdef HAVE_HEADLESS
  return new HeadlessGuiPrinterDialog();
#endif
  return 0;
}

GuiNavigator       *HeadlessGuiFactory::createNavigator( const std::string &id, GuiNavigator::Type navType ){assert(false);return 0;}
DialogWorkClock    *HeadlessGuiFactory::createDialogWorkClock( GuiElement *element
						     , DialogWorkClockListener *listener
						     , const std::string &title
						     , const std::string &msg ){assert(false);return 0;}

/* --------------------------------------------------------------------------- */
/* showDialogConfirmation --                                                   */
/* --------------------------------------------------------------------------- */
GuiElement::ButtonType HeadlessGuiFactory::showDialogConfirmation(GuiElement *par
                                                , const std::string &title
                                                , const std::string &message
                                                , ConfirmationListener *listener
                                                , const std::map<GuiElement::ButtonType, std::string>&  buttonText
                                                , bool cancelBtn
                                                , int defaultBtn)
{
  BUG_INFO("showDialogConfirmation: " << compose("title[%1], message[%2]", title, message));
  GuiElement::ButtonType buttonType(GuiElement::button_Cancel);

  // webapi query
  JobController* jobContrl = JobManager::Instance().getActiveJobController();
  if (jobContrl) {
     MessageQueueReplyThread *mq_reply = jobContrl->getMessageQueueReplyThread();
     if (mq_reply) {
       Json::Value jsonObj = Json::Value(Json::objectValue);
       jsonObj["command"] = cancelBtn ? "confirm_cancel" : "confirm";
       std::string msg(message);
       jsonObj["message"] = msg;
       jsonObj["title"] = title;
       std::map<GuiElement::ButtonType, std::string>::const_iterator it = buttonText.begin();
       if (buttonText.size() > 0) {
         Json::Value jsonAry = Json::Value(Json::arrayValue);
         for (;it != buttonText.end(); ++it) {
           jsonAry.append((*it).second);
         }
         jsonObj["button"] = jsonAry;
       }
       std::vector<std::string> expected;
       expected.push_back("button_pressed");
       Json::Value retObj = mq_reply->doQuery(jsonObj, expected);
       BUG_INFO("showDialogConfirmation reply, got answer["<<ch_semafor_intens::JsonUtils::value2string(retObj)<<"]");
       if ( retObj.isMember("button_pressed") ) {
         std::string btn_text(lower(retObj["button_pressed"].asString()));
         if (btn_text == "yes") {
           if (listener)
             listener->confirmYesButtonPressed();
           // first: yes or save
           if(buttonText.find(GuiElement::button_Save) != buttonText.end()){
             buttonType = GuiElement::button_Save;
           }else{
             buttonType = GuiElement::button_Yes;
           }
         } else if (btn_text == "no") {
            if (listener)
              listener->confirmNoButtonPressed();
            // second: no, discard, apply or open
            if(buttonText.find(GuiElement::button_Discard) != buttonText.end()){
              buttonType = GuiElement::button_Discard;
            }else if(buttonText.find(GuiElement::button_Apply) != buttonText.end()){
              buttonType = GuiElement::button_Apply;
            }else if(buttonText.find(GuiElement::button_Open) != buttonText.end()){
              buttonType = GuiElement::button_Open;
            }else{
              buttonType = GuiElement::button_No;
            }
         } else { //if (btn_text == "cancel")
           if (listener)
             listener->confirmCancelButtonPressed();
           // third: cancel, abort or discard
           if(buttonText.find(GuiElement::button_Cancel) != buttonText.end()){
             buttonType = GuiElement::button_Cancel;
           }else if(buttonText.find(GuiElement::button_Abort) != buttonText.end()){
             buttonType = GuiElement::button_Abort;
           }else if(buttonText.find(GuiElement::button_Discard) != buttonText.end()){
             buttonType = GuiElement::button_Discard;
           }
           // else: GuiElement::button_Cancel is initialize value of buttonType (above)
         }
       } else {
         BUG_DEBUG("showDialogConfirmation mq reply, no button_pressed attribute ");
           if (listener)
             listener->confirmCancelButtonPressed();
       }
       return buttonType;
     }
  }
  return GuiElement::button_None;
}

/* --------------------------------------------------------------------------- */
/* showDialogWarning --                                                        */
/* --------------------------------------------------------------------------- */
bool HeadlessGuiFactory::showDialogWarning(GuiElement *e
                                           , const std::string &title
                                           , const std::string &message
                                           , InformationListener * listener)
{
  BUG_INFO("showDialogWarning: " << compose("title[%1], message[%2]", title, message));
  // webapi query
  JobController* jobContrl = JobManager::Instance().getActiveJobController();
  if (jobContrl) {
     MessageQueueReplyThread *mq_reply = jobContrl->getMessageQueueReplyThread();
     if (mq_reply) {
       Json::Value jsonObj = Json::Value(Json::objectValue);
       jsonObj["command"] = "warning";
       std::string msg(message);
       jsonObj["message"] = msg;
       jsonObj["title"] = title;
       Json::Value retObj = mq_reply->doQuery(jsonObj);
       BUG_INFO("showDialogWarning mq reply, got answer["<<ch_semafor_intens::JsonUtils::value2string(retObj)<<"]");
       if (listener) listener->informOkButtonPressed();
       return true;
     }
  }

  return false;
}

/* --------------------------------------------------------------------------- */
/* showDialogInformation --                                                    */
/* --------------------------------------------------------------------------- */
bool HeadlessGuiFactory::showDialogInformation(GuiElement *e
                                               , const std::string &title
                                               , const std::string &message
                                               , InformationListener *listener)
{
  BUG_INFO("showDialogInformation: " << compose("title[%1], message[%2]", title, message));
  // webapi query
  JobController* jobContrl = JobManager::Instance().getActiveJobController();
  if (jobContrl) {
     MessageQueueReplyThread *mq_reply = jobContrl->getMessageQueueReplyThread();
     if (mq_reply) {
       Json::Value jsonObj = Json::Value(Json::objectValue);
       jsonObj["command"] = "information";
       std::string msg(message);
       jsonObj["message"] = msg;
       jsonObj["title"] = title;
       Json::Value retObj = mq_reply->doQuery(jsonObj);
       BUG_INFO("showDialogInformation mq reply, got answer["<<ch_semafor_intens::JsonUtils::value2string(retObj)<<"]");
       if (listener) listener->informOkButtonPressed();
       return true;
     }
  }
  return false;
}

std::string HeadlessGuiFactory::showDialogTextInput(GuiElement *
					 , const std::string &title
					 , const std::string &message
					 , const std::string &label
					 , bool &ok
					 , ConfirmationListener*listener ) {
  // webapi query
  JobController* jobContrl = JobManager::Instance().getActiveJobController();
  ok = false;
  if (jobContrl) {
    MessageQueueReplyThread *mq_reply = jobContrl->getMessageQueueReplyThread();
    if (mq_reply) {
      GuiElement::ButtonType buttonType(GuiElement::button_Yes);
      Json::Value jsonObj = Json::Value(Json::objectValue);
      jsonObj["command"] = "getText";
      jsonObj["title"] = title;
      std::string msg(message);
      jsonObj["message"] = msg;
      jsonObj["label"] = label;
      std::vector<std::string> expected;
      expected.push_back("button_pressed");
      expected.push_back("value");
      Json::Value retObj = mq_reply->doQuery(jsonObj, expected);
      BUG_INFO("showDialogTextInput reply, got answer["<<ch_semafor_intens::JsonUtils::value2string(retObj)<<"]");
      if ( retObj.isMember("button_pressed") ) {
        std::string btn_text(lower(retObj["button_pressed"].asString()));
        if (btn_text == "yes" || btn_text == "ok") {
          ok = true;
          if (listener)
            listener->confirmYesButtonPressed();
          buttonType = GuiElement::button_Yes;
        } else if (btn_text == "no") {
          if (listener)
            listener->confirmNoButtonPressed();
          buttonType = GuiElement::button_No;
        } else if (btn_text == "cancel") {
          if (listener)
            listener->confirmCancelButtonPressed();
          buttonType = GuiElement::button_Cancel;
        }
      } else {
        BUG_DEBUG("showDialogTextInput mq reply, no button_pressed attribute ");
        if (listener)
          listener->confirmNoButtonPressed();
      }
      return retObj.isMember("value") ? retObj["value"].asString() : "member 'value' is missing";
     }
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* showDialogTextInputExt --                                                   */
/* --------------------------------------------------------------------------- */
std::string HeadlessGuiFactory::showDialogTextInputExt(GuiElement *e
                                                       , const std::string &title
                                                       , const std::string &message
                                                       , const std::string &label
                                                       , const std::map<GuiElement::ButtonType, std::string>& buttonText
                                                       , GuiElement::ButtonType &buttonClicked
                                                       , ConfirmationListener* listener ) {
  // webapi query
  JobController* jobContrl = JobManager::Instance().getActiveJobController();
  if (jobContrl) {
     MessageQueueReplyThread *mq_reply = jobContrl->getMessageQueueReplyThread();
     if (mq_reply) {
       GuiElement::ButtonType buttonType(GuiElement::button_Yes);
       Json::Value jsonObj = Json::Value(Json::objectValue);
       jsonObj["command"] = "getText";
       jsonObj["title"] = title;
       std::string msg(message);
       jsonObj["message"] = msg;
       jsonObj["label"] = label;
       if (buttonText.size() > 0) {
         Json::Value jsonAry = Json::Value(Json::arrayValue);
         std::map<GuiElement::ButtonType, std::string>::const_iterator it = buttonText.begin();
         for (;it != buttonText.end(); ++it) {
           jsonAry.append((*it).second);
         }
         jsonObj["button"] = jsonAry;
       }
       BUG_INFO("showDialogTextInputExt try, listener: "<< listener);
       std::vector<std::string> expected;
       expected.push_back("button_pressed");
       expected.push_back("data");
       Json::Value retObj = mq_reply->doQuery(jsonObj, expected);
       BUG_INFO("showDialogTextInputExt reply, got answer["<<ch_semafor_intens::JsonUtils::value2string(retObj)<<"]");
       if ( retObj.isMember("button_pressed") ) {
         std::string btn_text(lower(retObj["button_pressed"].asString()));
         std::map<GuiElement::ButtonType, std::string>::const_iterator it = buttonText.begin();
         for (;it != buttonText.end(); ++it) {
           if ((*it).second == btn_text) {
             buttonType = (*it).first;
             break;
           }
         }
       }
       buttonClicked = buttonType;

       if (buttonType == GuiElement::button_Save ||
           buttonType == GuiElement::button_Yes) {
         if (listener)
           listener->confirmYesButtonPressed();
       } else if (buttonType == GuiElement::button_No ||
                  buttonType == GuiElement::button_Apply ||
                  buttonType == GuiElement::button_Discard ||
                  buttonType == GuiElement::button_Open) {
         if (listener)
           listener->confirmNoButtonPressed();
       } else {
         BUG_DEBUG("showDialogTextInputExt mq reply, no button_pressed attribute ");
         if (listener)
           listener->confirmCancelButtonPressed();
       }
       return retObj.isMember("value") ? retObj["value"].asString() : "member 'value' is missing";
     }
  }
  buttonClicked = GuiElement::button_None;
  return "";
}

bool HeadlessGuiFactory::showDialogFileSelection( GuiElement *element
					, const std::string &title
					, const std::string &filter
					, const std::string &directory
					, FileSelectListener *listener
					, DialogFileSelection::Type type
					, const std::vector<HardCopyListener::FileFormat> * const formats
					, DialogFileSelection::Mode mode
					, const std::string &basename)
{
  // WebReply Aufruf, do WebQuery
	std::string s;
  JobController* jobContrl = JobManager::Instance().getActiveJobController();
  FileStream::SaveListener* saveListener = dynamic_cast<FileStream::SaveListener*>(listener);
  FileSelectListener* fileSelectListener = dynamic_cast<FileSelectListener*>(listener);
  FileStream::OpenListener* openListener = dynamic_cast<FileStream::OpenListener*>(listener);
  std::string requestDataType = "file_handle";
  Stream* streamArg(0);
  if (jobContrl) {
    MessageQueueReplyThread *mq_reply = jobContrl->getMessageQueueReplyThread();
     if (mq_reply) {
       Json::Value jsonObj = Json::Value(Json::objectValue);
       jsonObj["directory"] = directory;
       jsonObj["basename"] = basename;
       jsonObj["filter"] = filter;
       jsonObj["title"] = title;
       jsonObj["mode"] = mode == DialogFileSelection::ExistingFile ? "ExistingFile" :
         mode == DialogFileSelection::ExistingFiles ? "ExistingFiles" :
         mode == DialogFileSelection::Directory ? "Directory" :
         mode == DialogFileSelection::DirectoryOnly ? "DirectoryOnly" : "AnyFile";

       std::string sendData;
       if (type == DialogFileSelection::Save) {
         jsonObj["type"] =  "save";
         jsonObj["command"] = "file_save";
         jsonObj["message"] = "Downloaded a file!";
         std::ostringstream os;
         HardCopyListener::FileFormat format(formats && formats->size() ? (*formats)[0] : HardCopyListener::NONE);
         if (saveListener != nullptr) {
           if (saveListener->getStreamFileFlag()){
             os << saveListener->getStreamFilename();
           }else{
             saveListener->writeStreamData(os, format);
           }
           BUG_DEBUG("SAVE DATA len["<<os.str().size()<<"] format["<<format<<"]\n");
         } else if (fileSelectListener) {
           std::string f("/tmp/streamXXXXXX." + ReportGen::Instance().getSuffix(format));
           char fn[200];
           strcpy(fn, f.c_str());
           int fpP = mkstemp(fn);
           remove(fn);
           close(fpP);
           fileSelectListener->FileSelected(fn, format, "");
           std::ifstream ifs(fn);
           os << ifs.rdbuf();
           ifs.close();
           remove(fn);
         }
         switch(format) {
         case (HardCopyListener::JSON):
           jsonObj["mimetype"] = "application/json";
           break;
         case (HardCopyListener::XML):
           jsonObj["mimetype"] = "application/xml";
           break;
         case (HardCopyListener::HTML):
           jsonObj["mimetype"] = "text/html";
           break;
         case (HardCopyListener::PDF):
           jsonObj["mimetype"] = "application/pdf";
           break;
         default:
           if (saveListener == nullptr || saveListener->getStreamFileFlag()){
             jsonObj["mimetype"] = "application/octet-stream";
           }else{
             jsonObj["mimetype"] = FileUtilities::getDataMimeType(os.str()); //"text/plain";
           }
         }

         // try to add missing suffix to basename
         if(FileUtilities::getSuffix(basename).empty()) {
           std::string suffix(FileUtilities::getPreferredSuffixOfMimeType(jsonObj["mimetype"].asString()));
           if(!suffix.empty()) {
             jsonObj["basename"] = basename + "." + suffix;
           }
         }

         sendData = os.str();
         streamArg = (saveListener != nullptr && saveListener->getStreamFileFlag()) ?  saveListener->getStream(): 0;
         BUG_DEBUG("SAVE DATA mimetype["<<jsonObj["mimetype"].asString()<<"] len["<<os.str().size()<<"]\n");
       } else {
         jsonObj["type"] =  "open";
         jsonObj["command"] = "file_open";
         jsonObj["message"] = "Please upload a file!";
       }
       std::vector<std::string> expected;
       expected.push_back("data");
       expected.push_back("type");
       Json::Value retObj = mq_reply->doQuery(jsonObj, expected, sendData, streamArg);
       BUG_INFO("FileSaveDialog mq reply, Valid RetObj: " << !retObj.isNull());
       BUG_INFO("FileSaveDialog mq reply, got answer["<<ch_semafor_intens::JsonUtils::value2string(retObj)<<"]");
       // answer
       if (type == DialogFileSelection::Open) {
         if (!retObj.isNull() &&  retObj.isMember("data")) {  // json data
           s = retObj["data"].asString();
         } else if (mq_reply->getQueryData().size() == 1) {   // kein json, Dateiinhalt
           s = mq_reply->getQueryData()[0];
         } else {
           BUG_WARN("Warning, no single answer data received." << mq_reply->getQueryData().size());
         }
         requestDataType = retObj.isMember("type") ? retObj["type"].asString() : "stream";
       }

       // answer
       if (type == DialogFileSelection::Save) {
         // nothing todo
         listener->FileSelected( "", HardCopyListener::NONE, "" );
         return true;
       } else {
         if( !s.empty() ){
           if(requestDataType == "file_handle") {
             listener->FileSelected( s, HardCopyListener::NONE, "" );
           } else if(requestDataType == "stream") {
             BUG_DEBUG("Reading from stream");
             // filename check
             std::string filename = FileUtilities::getDataFilename(s);

             // read
             std::string header;
             FileStream::decodeBase64data(s, header);

             std::istringstream iStringStream(s);
             if (openListener)
               openListener->readStreamData(iStringStream);
           }
           return true;
         }
       }
     }
  }

  return false;
}
void HeadlessGuiFactory::showDialogUserPassword( const std::string &, const std::string &
				       , UserPasswordListener *, DialogUnmapListener *unmap
				       , std::string errorMessage ){}

GuiDataField    *HeadlessGuiFactory::createDataField( GuiElement *parent, XferDataItem *dataitem, int prec ){
  DataReference *data = dataitem->Data();
  assert( data != 0 );
#ifdef HAVE_HEADLESS
  return new HeadlessGuiDataField( parent, *static_cast<UserAttr*>( data->getUserAttr() ) );
#endif
  return 0;
}

GuiPixmap       *HeadlessGuiFactory::createPixmap( GuiElement *parent, XferDataItem *dataitem ){assert(false);return 0;}
GuiIndex        *HeadlessGuiFactory::createGuiIndex( GuiElement *parent, const std::string &name ){assert(false);return 0;}
GuiIndex        *HeadlessGuiFactory::getGuiIndex( const std::string &name ){assert(false);return 0;}

Gui2dPlot* HeadlessGuiFactory::create2dPlot( const std::string &name, bool isPlot2d ){
  if( GuiElement::findElement( name ) )
    return 0;
#ifdef HAVE_HEADLESS
  return new HeadlessGuiPlot2d(0,name);
#endif
  return 0;
}

Gui3dPlot* HeadlessGuiFactory::create3dPlot( GuiElement *, const std::string & ){assert(false);return 0;}

ListPlot*        HeadlessGuiFactory::createListPlot( GuiElement *parent, const std::string &name ){assert(false);return 0;}
PSPlot*          HeadlessGuiFactory::createPSPlot( PSPlot::PSStream *stream ){assert(false);return 0;}

Plot2dMenuDescription *HeadlessGuiFactory::getPlot2dMenuDescription(){assert(false);return 0;}


Simpel *HeadlessGuiFactory::newSimpelPlot( GuiElement *, const std::string & ){assert(false);return 0;}
Simpel *HeadlessGuiFactory::getSimpelPlot( const std::string & ){assert(false);return 0;}

GuiImage  *HeadlessGuiFactory::createImage( GuiElement *, const std::string &, int ){assert(false);return 0;}
GuiImage  *HeadlessGuiFactory::createLinePlot( GuiElement *, const std::string & ){assert(false);return 0;}
GuiThermo  *HeadlessGuiFactory::createThermo( GuiElement *, const std::string & ){assert(false);return 0;}
GuiTimeTable *HeadlessGuiFactory::createTimeTable( GuiElement *, const std::string & ){assert(false);return 0;}

void HeadlessGuiFactory::attachEventLoopListener( GuiEventLoopListener *lsnr ){}
GuiCycleDialog  *HeadlessGuiFactory::createCycleDialog(){assert(false);return 0;}

GuiScrolledText *HeadlessGuiFactory::getStandardWindow(){
  GuiElement *element = GuiElement::findElement( "STD_WINDOW" );
#ifdef HAVE_HEADLESS
  HeadlessGuiScrolledText *text;
  if( !element ){
    text = new HeadlessGuiScrolledText( 0, "STD_WINDOW" );
//    text->createDataReference( "STD_WINDOW@" );
  }else{
    assert( element->Type() == GuiElement::type_ScrolledText );
    text = static_cast<HeadlessGuiScrolledText *>(element);
  }
  return text;
#endif
  return 0;
}

GuiScrolledText *HeadlessGuiFactory::getLogWindow(){
  GuiElement *element = GuiElement::findElement( "LOG_WINDOW" );
#ifdef HAVE_HEADLESS
  HeadlessGuiScrolledText *text;
  if( element == 0 ){
    text = new HeadlessGuiScrolledText( 0, "LOG_WINDOW" );
//    text->createDataReference( "LOG_WINDOW@" );
  }
  else{
    assert( element->Type() == GuiElement::type_ScrolledText );
    text = static_cast<HeadlessGuiScrolledText *>(element);
  }
  return text;
#endif
  return 0;
}

GuiScrolledText *HeadlessGuiFactory::getHtmlWindow(){assert(false);return 0;}

void HeadlessGuiFactory::installDialogsWaitCursor(GuiDialog* diag_modal){}
void HeadlessGuiFactory::removeDialogsWaitCursor(GuiDialog* diag_modal){}
DialogProgressBar *HeadlessGuiFactory::getDialogProgressBar(){assert(false);return 0;}
GuiElement *HeadlessGuiFactory::createProgressBar(GuiElement *, const std::string & ){assert(false);return 0;}

ServerSocket *HeadlessGuiFactory::createServerSocket( IntensServerSocket *server, int port ){assert(false);return 0;}
ClientSocket *HeadlessGuiFactory::createClientSocket( const std::string &header
					    , Stream *out_stream
					    , Stream *in_stream )
{
  assert(false);return 0;
}

bool HeadlessGuiFactory::doCopy( Stream *out_stream, GuiElement* elem){
  return false;
}

bool HeadlessGuiFactory::doPaste( Stream *in_stream){
  std::string clipboard;

  // get clipboard content from webtens
  JobController* jobContrl = JobManager::Instance().getActiveJobController();
  if (jobContrl) {
    MessageQueueReplyThread *mq_reply = jobContrl->getMessageQueueReplyThread();
    Json::Value jsonObj = Json::Value(Json::objectValue);
    jsonObj["command"] = "getClipboard";
    std::vector<std::string> expected;
    expected.push_back("clipboard");
    Json::Value retObj = mq_reply->doQuery(jsonObj, expected);
    if(retObj.isMember("clipboard")) {
      clipboard = retObj["clipboard"].asString();
    }
  }

  // write clipboard content to datapool (using in_stream)
  if(clipboard.size()) {
    std::istringstream is(clipboard + "\n");
    in_stream->clearRange();
    in_stream->read(is);
    return true;
  }

  return false;
}

char HeadlessGuiFactory::getDelimiter(){
  return '\t';
}

bool HeadlessGuiFactory::replace( GuiElement *old_el, GuiElement *new_el ){
  return false;
}
