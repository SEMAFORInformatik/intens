
#include <iostream>
#include <string>

#include "plot/CyclesDialog.h"
#include "plot/CyclesDialogListener.h"
#include "gui/GuiDataField.h"
#include "gui/GuiButton.h"
#include "gui/GuiFieldgroup.h"
#include "gui/GuiFieldgroupLine.h"
#include "gui/GuiLabel.h"
#include "gui/GuiButtonbar.h"
#include "gui/GuiEventData.h"
#include "utils/gettext.h"
#include "gui/GuiFactory.h"


CyclesDialog::CyclesDialog( CyclesDialogListener *listener,
                            GuiEventData *event )
  : m_listener( listener )
  , m_event( event )
{
}

void CyclesDialog::initialize( const std::string &name ){
  m_fieldgroup = GuiFactory::Instance() -> createFieldgroup( this->getElement(), name + " CycleParameters");
  m_fieldgroup->setMargins( 5, 10);
  assert( m_fieldgroup != 0 );
  getElement()->attach( m_fieldgroup -> getElement() );
  m_fieldgroup->setTitleAlignment( GuiElement::align_Center );

  // first line
  GuiFieldgroupLine *_line = m_fieldgroup->addFieldgroupLine();
  assert( _line != 0 );
  GuiElement *line = _line->getElement();
  GuiLabel *label = 0;
  label = GuiFactory::Instance() -> createLabel( line,
                                                 GuiElement::align_Default );
  label->setTitle( _("Case") );
  label = GuiFactory::Instance() -> createLabel( line,
                                                 GuiElement::align_Default );
  label->setTitle( _("Name") );
  label = GuiFactory::Instance() -> createLabel( line,
                                                 GuiElement::align_Default );
}

CyclesDialog::~CyclesDialog(){
  delete m_event;
  delete m_fieldgroup;
  delete m_closeButton;
  std::vector<XferDataItem*>::iterator it;
  for( it = m_xfers.begin(); it != m_xfers.end(); ++it )
    delete (*it);
}

// --------------------------------------------------------------------------- //
// addItem --                                                                  //
// --------------------------------------------------------------------------- //

bool CyclesDialog::addItem( const std::string &label, const std::string &name, XferDataItem *xfer ){
  if( xfer == 0 )
    return false;
  m_xfers.push_back( xfer );
  // new line
  GuiFieldgroupLine *_line = m_fieldgroup->addFieldgroupLine();
  assert( _line != 0);
  GuiElement *line = _line->getElement();
  // label
  GuiLabel *guiLabel = GuiFactory::Instance() -> createLabel( line
							      , GuiElement::align_Default );
  guiLabel->setLabel( label );
  //name
  guiLabel = GuiFactory::Instance() -> createLabel( line
						    , GuiElement::align_Default );
  guiLabel->setLabel( name );
  // show
  GuiDataField *button = GuiFactory::Instance()->createToggle( line );
  button->installDataItem( xfer );
  button->getElement()->setAlignment( GuiElement::align_Center );
  line->attach( button->getElement() );

  return true;
}

// --------------------------------------------------------------------------- //
// create --                                                                   //
// --------------------------------------------------------------------------- //

void CyclesDialog::create(){
  GuiFactory *factory = GuiFactory::Instance();
  // Buttonbar generieren
  GuiButtonbar *bar = factory->createButtonbar( getElement() );
  // Close Button generieren und an das Buttonbar anfuegen
  GuiEventData *event = new GuiEventData();
  event = new GuiEventData();
  m_closeButton = factory->createButton( bar->getElement(), this, event );
  m_closeButton->setLabel( _("Close") );
}

// --------------------------------------------------------------------------- //
// isDialogUpdated --                                                          //
// --------------------------------------------------------------------------- //

bool CyclesDialog::isDialogUpdated(){
  std::vector<XferDataItem*>::iterator it;

  for( it = m_xfers.begin(); it != m_xfers.end(); ++it ){
    if( (*it)->isUpdated(m_trans) && (*it)->isDataItemUpdated(m_trans)){
      int value = 0;
      if( (*it)->isValid() ){
	(*it)->getValue( value );
      }
      if( value != 0 )
	return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getValues --                                                                */
/* --------------------------------------------------------------------------- */

void CyclesDialog::getValues( std::vector<int> &v, int newValue ){
  std::vector<XferDataItem *>::iterator it;
  for( it = m_xfers.begin(); it != m_xfers.end(); ++it ){
    int value;
    if( !((*it)->getValue( value )) )
      value = newValue;
    v.push_back( value );
  }
}

/* --------------------------------------------------------------------------- */
/* setValues --                                                                */
/* --------------------------------------------------------------------------- */

void CyclesDialog::setValues( std::vector<int> &v, int newValue ){
  int value;
  std::vector<XferDataItem *>::iterator it;
  std::vector<int>::iterator intIterator = v.begin();
  for( it = m_xfers.begin(); it != m_xfers.end(); ++it ){
    if( intIterator != v.end() ){
      value = (*intIterator);
      ++intIterator;
    }
    else
      value = newValue;
    (*it)->setValue( value );
  }
}

// --------------------------------------------------------------------------- //
// ButtonPressed --                                                            //
// --------------------------------------------------------------------------- //

void CyclesDialog::ButtonPressed( GuiEventData *event ){
  if( event == 0 )
    return;
  GuiElement* element = event->m_element;
  if( element == 0 )
    return;
  if( element == m_closeButton->getElement() ){
    m_listener->closeEvent( m_event );
    if( isDialogUpdated() )
      m_listener->cyclesUpdate();
  }
  else
     assert( false );
}
