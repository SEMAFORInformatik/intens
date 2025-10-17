
#include "gui/qt/GuiQtForm.h"
#include "plot/CyclesDialogListener.h"
#include "plot/qt/QtCyclesDialog.h"

QtCyclesDialog::QtCyclesDialog( GuiElement *parent
			      , CyclesDialogListener *listener
			      , const std::string &title
			      , GuiEventData *event )
  : GuiQtForm( parent, false )
  , CyclesDialog( listener, event ){
  setTitle( title );
  resetCycleButton();
  hasCloseButton( false );
  setApplicationModal();
  if( event != 0 )
    event->m_element = this;
}

QtCyclesDialog::~QtCyclesDialog(){
}

// --------------------------------------------------------------------------- //
// create --                                                                   //
// --------------------------------------------------------------------------- //

void QtCyclesDialog::create(){
  CyclesDialog::create();
  GuiQtForm::create();
}

// --------------------------------------------------------------------------- //
// manage --                                                                   //
// --------------------------------------------------------------------------- //

void QtCyclesDialog::manage(){
  setTransaction( DataPoolIntens::NewTransaction() );
  GuiQtForm::manage();
}

// --------------------------------------------------------------------------- //
// update --                                                                   //
// --------------------------------------------------------------------------- //

void QtCyclesDialog::update(UpdateReason reason ){
  if( isDialogUpdated() ){
    getListener()->cyclesUpdate();
  }
  setTransaction( DataPoolIntens::NewTransaction() );
  GuiQtForm::update( reason );
}
