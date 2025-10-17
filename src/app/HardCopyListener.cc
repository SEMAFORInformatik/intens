
#include <string>
/* System headers */
#include "app/HardCopyListener.h"
#include "gui/GuiEventData.h"
#include "gui/GuiPrinterDialog.h"
#include "gui/HardCopyTree.h"
#include "gui/GuiFactory.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
HardCopyListener::HardCopyListener()
  : m_object( 0 )
    , m_data_index( 0 )
    , m_showMenu( true ){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setHardCopyObject --                                                        */
/* --------------------------------------------------------------------------- */

void HardCopyListener::setHardCopyObject( HardCopyObject *obj ){
  m_object = obj;
}

/* --------------------------------------------------------------------------- */
/* getHardCopyObject --                                                        */
/* --------------------------------------------------------------------------- */

HardCopyObject *HardCopyListener::getHardCopyObject(){
  return m_object;
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void HardCopyListener::ButtonPressed(){
  if (AppData::Instance().HeadlessWebMode()) {
    // webapi: print means save file
    this->saveFile( getButton());
    return;
  }

  GuiPrinterDialog::MyEventData event( ReportGen::PRINT );
  GuiFactory::Instance()->createPrinterDialog()->showDialog( this, getButton(), &event );
}

void HardCopyListener::ButtonPressed( GuiEventData *event ){
  GuiFactory::Instance()->createPrinterDialog()->showDialog( this, getButton(), event );
}


/* --------------------------------------------------------------------------- */
/* setPrintFilter --                                                           */
/* --------------------------------------------------------------------------- */

void HardCopyListener::setPrintFilter( const std::string &filter ){
  m_filter = filter;
}

/* --------------------------------------------------------------------------- */
/* PrintFilter --                                                              */
/* --------------------------------------------------------------------------- */

const std::string &HardCopyListener::PrintFilter(){
  return m_filter;
}
