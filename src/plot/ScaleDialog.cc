
#include <iostream>
#include <string>

#include "plot/ScaleDialog.h"
#include "plot/ScaleDialogListener.h"
#include "gui/GuiDataField.h"
#include "gui/GuiButton.h"
#include "gui/GuiFieldgroup.h"
#include "gui/GuiFieldgroupLine.h"
#include "gui/GuiLabel.h"
#include "gui/GuiVoid.h"
#include "gui/GuiButtonbar.h"
#include "gui/GuiEventData.h"
#include "gui/GuiOrientationContainer.h"
#include "utils/gettext.h"
#include "gui/GuiFactory.h"
#include "app/DataPoolIntens.h"
#include "app/DataSet.h"


ScaleDialog::ScaleDialog( ScaleDialogListener *listener
			  , const std::string &itemLabel
			  , GuiEventData *event )
  : m_listener( listener )
  , m_fieldgroup(0)
  , m_aspectRatioTypeCB(0)
  , m_resetButton(0)
  , m_closeButton(0)
  , m_event( event )
  , m_itemLabel( itemLabel )
  , m_drefLineStyleComboboxValue( 0 )
{

}

void ScaleDialog::initialize( const std::string &name, XferDataItem *xferAspectRatioType ){
  m_fieldgroup = GuiFactory::Instance() -> createFieldgroup( this->getElement(), name + " ScaleParameters");
  assert( m_fieldgroup != 0 );
  attach( m_fieldgroup -> getElement() );
  m_fieldgroup->setTitleAlignment( GuiElement::align_Center );

  // first line
  GuiFieldgroupLine *_line = m_fieldgroup->addFieldgroupLine();
  assert( _line != 0 );
  GuiElement *line = _line->getElement();
  GuiLabel *label = GuiFactory::Instance() -> createLabel( line, GuiElement::align_Default );
  label->setTitle( m_itemLabel);
  label = GuiFactory::Instance() -> createLabel( line, GuiElement::align_Default );
  label->setTitle( _("Min. Scale") );
  label = GuiFactory::Instance() -> createLabel( line, GuiElement::align_Default );
  label->setTitle( _("Max. Scale") );
  label = GuiFactory::Instance() -> createLabel( line, GuiElement::align_Default );
  label->setTitle( _("Enable") );
  label = GuiFactory::Instance() -> createLabel( line, GuiElement::align_Default );
  label->setTitle( _("Aspect Ratio") );

  // apect ratio type
  std::string setName("@ScaleDialogAspectRatioTypeItemSet");
  DataPoolIntens &dpi = DataPoolIntens::Instance();
  DataSet* dataset = dpi.newDataSet(setName, false, true );
  if (!dataset) // plot is cloned
    dataset = dpi.getDataSet(setName);
  dataset->resetInvalidEntry();
  dataset->setItemValues( "None", KeepScales );
  dataset->setItemValues( "Fixed", Fixed );
  dataset->setItemValues( "Expanding", Expanding );
  dataset->setItemValues( "Fitting", Fitting);
  dataset->create(setName);

  GuiOrientationContainer* vertCont = GuiFactory::Instance()->createVerticalContainer(getElement());
  label = GuiFactory::Instance() -> createLabel( vertCont->getElement(), GuiElement::align_Default );
  label->setTitle( _("Aspect Ratio Type") );
  xferAspectRatioType->getUserAttr()->SetDataSetName(setName);
  m_aspectRatioTypeCB = GuiFactory::Instance()->createDataField( line, xferAspectRatioType );
  m_aspectRatioTypeCB->setLength(10);
  vertCont->getElement()->attach(m_aspectRatioTypeCB->getElement());
  vertCont->getElement()->setRowSpan(2) ;
  line->attach(vertCont->getElement());
 }

ScaleDialog::~ScaleDialog(){
  delete m_event;
}

// --------------------------------------------------------------------------- //
// addItem --                                                                  //
// --------------------------------------------------------------------------- //

bool ScaleDialog::addItem( const std::string &label
			   , XferDataItem *xferMin
			   , Scale *min_scale
			   , XferDataItem *xferMax
			   , Scale *max_scale
			   , XferDataItem *xferScaleEnable
			   , XferDataItem *xferAspectRatio
			   , int fieldLength
			   , int precision ){
  if( xferMin == 0 || xferMax == 0 || xferScaleEnable == 0 )
    return false;

  XferSet *xferSet = new XferSet;
  xferSet->push_back( xferMin );
  xferSet->push_back( xferMax );
  xferSet->push_back( xferScaleEnable );
  xferSet->push_back( xferAspectRatio );
  m_xferSets.push_back( xferSet );
  // new line
  GuiFieldgroupLine *_line = m_fieldgroup->addFieldgroupLine();
  assert( _line != 0 );
  GuiElement *line = _line->getElement();
  // label
  GuiLabel *guiLabel = GuiFactory::Instance() -> createLabel( line
							      , GuiElement::align_Default );
  guiLabel->setLabel( label );
  // min. data field
  GuiDataField *field = GuiFactory::Instance()->createDataField( line, xferMin );
  field->setLength( fieldLength );
  field->setPrecision( precision );
  field->setScalefactor( min_scale );
  line->attach( field->getElement() );
  // max. data field
  field = GuiFactory::Instance()->createDataField( line, xferMax );
  field->setLength( fieldLength );
  field->setPrecision( precision );
  field->setScalefactor( max_scale );
  line->attach( field->getElement() );
  // scaleEnableButton
  GuiDataField *button = GuiFactory::Instance()->createToggle( line );
  button->installDataItem( xferScaleEnable );
  button->getElement()->setAlignment( GuiElement::align_Center );
  line->attach( button->getElement() );
  // max. data field
  field = GuiFactory::Instance()->createDataField( line, xferAspectRatio );
  field->setLength( fieldLength );
  field->setPrecision( precision );
  line->attach( field->getElement() );

  return true;
}

// --------------------------------------------------------------------------- //
// create --                                                                   //
// --------------------------------------------------------------------------- //

void ScaleDialog::create(){
  GuiFactory *factory = GuiFactory::Instance();
  // Buttonbar generieren
  GuiButtonbar *bar = factory->createButtonbar( getElement() );
  // Reset Button generieren und an das Buttonbar anfuegen
  GuiEventData *event = new GuiEventData();
  m_resetButton = factory->createButton( bar->getElement(), this, event );
  m_resetButton->setLabel( _("Reset") );
  // Close Button generieren und an das Buttonbar anfuegen
  event = new GuiEventData();
  m_closeButton = factory->createButton( bar->getElement(), this, event );
  m_closeButton->setLabel( _("Close") );
}

// --------------------------------------------------------------------------- //
// isDialogUpdated --                                                          //
// --------------------------------------------------------------------------- //

bool ScaleDialog::isDialogUpdated(){
  std::vector<XferSet*>::iterator it;
  if( !m_xferSets.empty() )
    if( (*(m_xferSets.front()))[2]->isDataItemUpdated(m_trans) )
      return true;

  for( it = m_xferSets.begin(); it != m_xferSets.end(); ++it ){
    if( (*(*it))[0]->isUpdated(m_trans) || (*(*it))[1]->isUpdated(m_trans) ){
      	int value = 0;
	if( (*(*it))[2]->isValid() ){
	  (*(*it))[2]->getValue( value );
	}
	if( value != 0 )
	  return true;
    }
  }
  return false;
}

// --------------------------------------------------------------------------- //
// ButtonPressed --                                                            //
// --------------------------------------------------------------------------- //

void ScaleDialog::ButtonPressed( GuiEventData *event ){
  if( event == 0 )
    return;
  GuiElement* element = event->m_element;
  if( element == 0 )
    return;
  if( element == m_closeButton->getElement() ){
    m_listener->closeEvent( m_event );
    if( isDialogUpdated() )
      m_listener->scaleUpdate();
  }
  else if( element == m_resetButton->getElement() )
    m_listener->resetEvent( m_event );
  else
    assert( false );
}
