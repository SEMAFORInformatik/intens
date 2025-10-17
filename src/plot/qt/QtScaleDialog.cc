
#include "gui/qt/GuiQtForm.h"
#include "plot/ScaleDialogListener.h"
#include "plot/qt/QtScaleDialog.h"

QtScaleDialog::QtScaleDialog( GuiElement *parent
			      , ScaleDialogListener *listener
			      , const std::string &title
			      , const std::string &itemLabel
			      , GuiEventData *event )
  : GuiQtForm( parent, false )
  , ScaleDialog( listener, itemLabel, event ){
  setTitle( title );
  resetCycleButton();
  hasCloseButton( false );
  setApplicationModal();
  if( event != 0 )
    event->m_element = this;
}

QtScaleDialog::~QtScaleDialog(){
}

// --------------------------------------------------------------------------- //
// create --                                                                   //
// --------------------------------------------------------------------------- //

void QtScaleDialog::create(){
  ScaleDialog::create();
  GuiQtForm::create();
}

// --------------------------------------------------------------------------- //
// manage --                                                                   //
// --------------------------------------------------------------------------- //

void QtScaleDialog::manage(){
  setTransaction( DataPoolIntens::NewTransaction() );
  GuiQtForm::manage();
}

// --------------------------------------------------------------------------- //
// update --                                                                   //
// --------------------------------------------------------------------------- //

void QtScaleDialog::update(UpdateReason reason ){
  if( isDialogUpdated() ){
    getListener()->scaleUpdate();
  }
  setTransaction( DataPoolIntens::NewTransaction() );
  GuiQtForm::update( reason );
}
