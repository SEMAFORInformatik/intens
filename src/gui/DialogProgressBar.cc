
#include "app/DataPoolIntens.h"
#include "job/JobController.h"
#include "gui/GuiFactory.h"
#include "gui/GuiFieldgroup.h"
#include "gui/GuiFieldgroupLine.h"
#include "gui/DialogProgressBar.h"
#include "datapool/DataVector.h"
#include "datapool/DataStringValue.h"
#include "utils/gettext.h"

INIT_LOGGER();

const std::string DialogProgressBar::DIALOG_NAME("PROGRESS_DIALOG");
const std::string DialogProgressBar::FIELDGROUP_NAME("PROGRESS_FIELDGROUP");

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
DialogProgressBar::DialogProgressBar(const std::string& name)
  : m_controller(0)
{
}

DialogProgressBar:: ~DialogProgressBar() {
};


/* --------------------------------------------------------------------------- */
/* getWindowTitle  --                                                            */
/* --------------------------------------------------------------------------- */
std::string DialogProgressBar::getWindowTitle() {
  std::string s(_("Dialog ProgressBar"));
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.WindowTitle" );
  if ( ref != 0 ) {
    ref->GetValue( s );
    delete ref;
  }
  return s;
};

/* --------------------------------------------------------------------------- */
/* getLoopTitle  --                                                            */
/* --------------------------------------------------------------------------- */
std::string DialogProgressBar::getLoopTitle() {
  std::string s;
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialogLoopTitle" );
  if ( ref != 0 ) {
    ref->GetValue( s );
    delete ref;
  }
  return s;
};

/* --------------------------------------------------------------------------- */
/* getMainTitle  --                                                            */
/* --------------------------------------------------------------------------- */
std::string DialogProgressBar::getMainTitle() {
  std::string s;
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.MainTitle" );
  if ( ref != 0 ) {
    ref->GetValue( s );
    delete ref;
  }
  return s;
};

/* --------------------------------------------------------------------------- */
/* getMainPercentRate  --                                                      */
/* --------------------------------------------------------------------------- */
int DialogProgressBar::getMainPercentRate() {
  int i=0;
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.MainPercent" );
  if ( ref != 0 ) {
    ref->GetValue( i );
    delete ref;
  }
  return i;
};

/* --------------------------------------------------------------------------- */
/* getMainFooter --                                                            */
/* --------------------------------------------------------------------------- */
std::string DialogProgressBar::getMainFooter() {
  std::string s;
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.MainFooter" );
  if ( ref != 0 ) {
    ref->GetValue( s );
    delete ref;
  }
  return s;
};

/* --------------------------------------------------------------------------- */
/* setMainFooter --                                                            */
/* --------------------------------------------------------------------------- */
void DialogProgressBar::setMainFooter(const std::string& s) {
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.MainFooter" );
  if ( ref != 0 ) {
    ref->SetValue( s );
    delete ref;
  }
};

/* --------------------------------------------------------------------------- */
/* getSubTitle  --                                                             */
/* --------------------------------------------------------------------------- */
std::string DialogProgressBar::getSubTitle() {
  std::string s;
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.SubTitle" );
  if ( ref != 0 ) {
    ref->GetValue( s );
    delete ref;
  }
  return s;
};

/* --------------------------------------------------------------------------- */
/* getError  --                                                                */
/* --------------------------------------------------------------------------- */
std::string DialogProgressBar::getError() {
  std::string s;
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.ErrorString" );
  if ( ref != 0 ) {
    ref->GetValue( s );
    delete ref;
  }
  return s;
};

/* --------------------------------------------------------------------------- */
/* getSubPercentRate  --                                                       */
/* --------------------------------------------------------------------------- */
int DialogProgressBar::getSubPercentRate() {
  int i=0;
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.SubPercent" );
  if ( ref != 0 ) {
    ref->GetValue( i );
    delete ref;
  }
  return i;
};

/* --------------------------------------------------------------------------- */
/* getDataTitle --                                                             */
/* --------------------------------------------------------------------------- */
std::string DialogProgressBar::getDataTitle() {
  std::string s;
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.DataTitle" );
  if ( ref != 0 ) {
    ref->GetValue( s );
    delete ref;
  }
  return s;
};

/* --------------------------------------------------------------------------- */
/* getDataFooter --                                                            */
/* --------------------------------------------------------------------------- */
std::string DialogProgressBar::getDataFooter() {
  std::string s;
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.DataFooter" );
  if ( ref != 0 ) {
    ref->GetValue( s );
    delete ref;
  }
  return s;
};

/* --------------------------------------------------------------------------- */
/* getDataLabelVector --                                                       */
/* --------------------------------------------------------------------------- */
void DialogProgressBar::getDataLabelVector(std::vector<std::string>& vec) {
  std::string s;
  vec.clear();
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.DataLabel" );
  if ( ref != 0 ) {
    int id[1] = {-1};
    DataVector dataVec;
    ref->getDataVector(dataVec, 1, id);
    for(DataVector::iterator i=dataVec.begin(); i < dataVec.end(); i++) {
      (*i)->getValue(s);
      vec.push_back(s);
    }
    delete ref;
  }
};

/* --------------------------------------------------------------------------- */
/* getDataVector --                                                            */
/* --------------------------------------------------------------------------- */
void DialogProgressBar::getDataVector(std::vector<std::string>& vec) {
  std::string s;
  vec.clear();
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.Data" );
  if ( ref != 0 ) {
    DataVector dataVec;
    int id[1] = {-1};
    ref->getDataVector(dataVec, 1, id);
    for(DataVector::iterator i=dataVec.begin(); i < dataVec.end(); i++) {
      (*i)->getValue(s);
      vec.push_back(s);
    }
    delete ref;
  }
};

/* --------------------------------------------------------------------------- */
/* getAbortCommand --                                                          */
/* --------------------------------------------------------------------------- */
std::string DialogProgressBar::getAbortCommand() {
  std::string s;
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialogAbortCommand" );
  if ( ref != 0 ) {
    ref->GetValue( s );
    delete ref;
  }
  return s;
};

/* --------------------------------------------------------------------------- */
/* getPixmapDataReference --                                                   */
/* --------------------------------------------------------------------------- */
DataReference* DialogProgressBar::getPixmapDataReference() {
  return DataPoolIntens::Instance().getDataReference( "ProgressDialogPixmap" );
};

/* --------------------------------------------------------------------------- */
/* getDetailPixmapDataReference --                                             */
/* --------------------------------------------------------------------------- */
DataReference* DialogProgressBar::getDetailPixmapDataReference() {
  return DataPoolIntens::Instance().getDataReference( "ProgressDialogDetailPixmap" );
};

//-------------------------------------------------------------------
// isDataItemUpdated --
//-------------------------------------------------------------------

bool DialogProgressBar::isDataItemUpdated( TransactionNumber trans ) {
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.MainPercent" );
  if ( ref != 0 ) {
    if( ref->isDataItemUpdated(DataReference::ValueUpdated, trans) ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* abortJobController --                                                       */
/* --------------------------------------------------------------------------- */

void DialogProgressBar::abortJobController(){
  BUG( BugGui, "abortJobController" );
  if (m_controller)
    m_controller->cancelButtonPressed();
}

/* --------------------------------------------------------------------------- */
/* resetData --                                                                */
/* --------------------------------------------------------------------------- */
void DialogProgressBar::resetData() {
  // clear all data
  std::vector<std::string> drefs;
  drefs.push_back("ProgressDialogLoopTitle");
  drefs.push_back("ProgressDialogPixmap");
  drefs.push_back("ProgressDialogDetailPixmap");
  drefs.push_back("ProgressDialog.MainTitle");
  drefs.push_back("ProgressDialog.SubTitle");
  drefs.push_back("ProgressDialog.MainPercent");
  drefs.push_back("ProgressDialog.MainFooter");
  drefs.push_back("ProgressDialog.SubPercent");
  drefs.push_back("ProgressDialog.ErrorString");
  drefs.push_back("ProgressDialog.DataTitle");
  drefs.push_back("ProgressDialog.DataFooter");
  drefs.push_back("ProgressDialog.DataLabel");
  drefs.push_back("ProgressDialog.Data");
  for (std::vector<std::string>::iterator it = drefs.begin(); it != drefs.end(); ++it) {
    DataReference* ref = DataPoolIntens::Instance().getDataReference(*it);
    if (ref != 0) {
      ref->clearAllElements();
      delete ref;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* create_fieldgroup --                                                        */
/* --------------------------------------------------------------------------- */

void DialogProgressBar::create_fieldgroup(GuiFieldgroup* fg){
  fg->setFrame(GuiElement::status_ON);

  // progressbar
  GuiFieldgroupLine *line = static_cast<GuiFieldgroupLine*>(fg->addFieldgroupLine());
  GuiElement *pgBar = GuiFactory::Instance()->createProgressBar(line->getElement(), "PROGRESS_BAR");

  // main footer
  line = static_cast<GuiFieldgroupLine*>(fg->addFieldgroupLine());
  DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialog.MainFooter" );
  GuiDataField *field = GuiFactory::Instance()->createDataField( line->getElement(), new XferDataItem(ref) );
  field->setLength( 40 );
  line->attach( field->getElement() );
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool DialogProgressBar::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  getGuiElement()->writeJsonProperties(jsonObj);
  jsonObj["percent"] = getMainPercentRate();
  return true;
}


/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool DialogProgressBar::serializeProtobuf(in_proto::Progressbar* element, bool onlyUpdated) {
  element->set_allocated_base(getGuiElement()->writeProtobufProperties());
  element->set_percent(getMainPercentRate());
  return true;
}
#endif
