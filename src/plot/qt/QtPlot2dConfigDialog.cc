
#include <QBoxLayout>
#include <QApplication>
#if QT_VERSION > 0x050600
#include <QScreen>
#else
#include <QDesktopWidget>
#endif
#include <QScrollBar>
#include "gui/GuiFactory.h"
#include "gui/GuiLabel.h"
#include "gui/GuiVoid.h"
#include "gui/qt/GuiQtFieldgroupLine.h"
#include "gui/GuiFieldgroup.h"
#include "gui/qt/GuiQtRadioButton.h"
#include "gui/qt/GuiQtDataField.h"
#include "gui/qt/GuiQtIndex.h"
#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/GuiQtButton.h"
#include "gui/qt/GuiQtButtonbar.h"
#include "gui/qt/GuiQtSeparator.h"
#include "gui/qt/GuiQtOrientationContainer.h"
#include "gui/qt/GuiQtLabel.h"
#include "plot/ConfigDialogListener.h"
#include "app/DataSet.h"
#include "app/UserAttr.h"
#include "plot/qt/QtPlot2dConfigDialog.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferDataItemIndex.h"
#include "utils/gettext.h"

const std::string QtPlot2dConfigDialog::s_datasetName[4] = {
  "@ConfigDialogXItemSet", "@ConfigDialogLineStyleItemSet","@ConfigDialogSymbolStyleItemSet","@ConfigDialogSymbolSizeItemSet"
};
const int SYMBOL_COLOR_COLUMN_INDEX = 5;
const int numCycleAttributes = 5; // [line, symbolColor, lineStyle, symbolStyle, symbolSize]

INIT_LOGGER();

QtPlot2dConfigDialog::QtPlot2dConfigDialog( ConfigDialogListener *listener
                                            , GuiElement *parent
                                            , const std::string &name
                                            , const std::string &title
                                            , const std::vector<std::string>& labels
                                            , bool cycleMode
                                            , GuiEventData *event )
  : GuiQtForm( parent, false )
  , m_resetButton( 0 )
  , m_closeButton( 0 )
  , m_recreate( false )
  , m_created( false )
  , m_listener( listener )
  , m_event( event )
  , m_numItems( 0 )
  , m_combosXfer( 0 )
  , m_xDataField( 0 )
  , m_drefXComboboxValue( 0 )
  , m_drefLineColor( 0 )
  , m_drefSymbolColor( 0 )
  , m_drefLineStyleComboboxValue( 0 )
  , m_drefSymbolStyleComboboxValue( 0 )
  , m_drefSymbolSizeComboboxValue( 0 )
  , m_drefUnitComboboxValue( 0 )
  , m_previousButton(0)
  , m_parent( parent )
  , m_name( name )
  , m_fieldgroup( 0 )
  , m_fieldgroupOld( 0 )
  , m_currentLine( 0 )
  , m_vertCont( 0 )
  , m_columnLabels( labels )
  , m_cycleMode( cycleMode )
{
  setTitle( title );
  resetCycleButton();
  hasCloseButton( false );
  setApplicationModal();
  if( event != 0 )
    event->m_element = this;

  createFieldgroup();
  createLocalData( name );
}

QtPlot2dConfigDialog::~QtPlot2dConfigDialog() {
  delete m_event;
  delete m_combosXfer;
  delete m_xDataField;
  for (std::vector<XferDataItem*>::iterator it = m_xComboItems.begin();
       it != m_xComboItems.end(); ++it)
    delete *it;
}

// --------------------------------------------------------------------------- //
// setCycleMode --                                                             //
// --------------------------------------------------------------------------- //
bool QtPlot2dConfigDialog::setCycleMode(bool cycleMode, const std::vector<int>& showCycleVector) {
  if (m_cycleMode != cycleMode) m_recreate = true;
  if (m_showCycleVector != showCycleVector) m_recreate = true;
  m_cycleMode = cycleMode;
  m_showCycleVector = showCycleVector;
  return m_recreate;
}

// --------------------------------------------------------------------------- //
// recreateFieldgroup --                                                       //
// --------------------------------------------------------------------------- //
void QtPlot2dConfigDialog::recreateFieldgroup() {
  if (!m_recreate) return;

  // delete m_combosXfer;
  //  delete m_xDataField;
  for (std::vector<XferDataItem*>::iterator it = m_xComboItems.begin();
   	   it != m_xComboItems.end(); ++it)
   	delete *it;

  m_combosXfer = 0;
  //  m_xDataField = 0;
  m_xComboItems.clear();
  m_numItems = 0;

  // create new Fieldgroup
  createFieldgroup();
}

void QtPlot2dConfigDialog::replaceFieldgroup() {
  if (m_created && m_fieldgroupOld) {
    // ! fieldgroup not created
    if (!m_fieldgroup->getElement()->getQtElement()->myWidget()) {
      m_fieldgroup->getElement()->create();
      QBoxLayout*  boxLayout =  dynamic_cast<QBoxLayout*>(m_vertCont->myWidget()->layout());
      boxLayout->addWidget(m_fieldgroup->getElement()->getQtElement()->myWidget());
    }
  }
  m_fieldgroupOld = 0;  // job done
}

// --------------------------------------------------------------------------- //
// createFieldgroup --                                                         //
// --------------------------------------------------------------------------- //

GuiFieldgroup* QtPlot2dConfigDialog::createFieldgroup(){
  if (!m_vertCont) {
    m_vertCont = new GuiQtOrientationContainer( orient_Vertical, this );
    m_vertCont->setPanedWindow( GuiElement::status_OFF );
    attach( m_vertCont );
  } else {
    m_fieldgroupOld = m_fieldgroup;
  }

  m_fieldgroup = GuiFactory::Instance() -> createFieldgroup( m_vertCont, "" );
  assert( m_fieldgroup != 0 );
  if (m_fieldgroupOld) {
    m_vertCont->replace(m_fieldgroupOld->getElement(), m_fieldgroup->getElement());
    if (m_vertCont->myWidget()) {
      QWidget* old_widget = m_fieldgroupOld->getElement()->getQtElement()->myWidget();
      QBoxLayout*  boxLayout =  dynamic_cast<QBoxLayout*>(m_vertCont->myWidget()->layout());
      int idx = boxLayout->indexOf( old_widget );
      boxLayout->removeWidget(old_widget);
    }

  } else {
    m_vertCont->attach( m_fieldgroup -> getElement());
  }

  // cycle label line
  if (m_cycleMode) {
    m_currentLine = dynamic_cast<GuiQtFieldgroupLine*>(m_fieldgroup->addFieldgroupLine());
    m_fgLines.push_back(m_currentLine);
    assert( m_currentLine != 0 );
    m_currentLine->attach( new GuiVoid(m_currentLine) );
    m_currentLine->attach( new GuiVoid(m_currentLine) );
    m_currentLine->attach( new GuiVoid(m_currentLine) );
    m_currentLine->attach( new GuiVoid(m_currentLine) );
    std::vector<int>::const_iterator showIt = m_showCycleVector.begin();
    for (int i = 0; i < DataPoolIntens::Instance().numCycles(); ++i, ++showIt) {

      if (m_cycleMode && *showIt != 1) continue;  // this cycle is not shown

      ///	  m_currentLine->attach( new GuiVoid() );
      GuiSeparator *sep = GuiFactory::Instance() -> createSeparator( m_currentLine );
      sep->setOrientation(GuiElement::orient_Vertical);
      m_currentLine->attach( sep->getElement() );

      GuiLabel* label = GuiFactory::Instance()->createLabel(m_currentLine, GuiElement::align_Center);
      label->getElement()->setColSpan(numCycleAttributes);
      //label->getElement()->setAlignment( GuiElement::align_Center );
      std::string cycleName;
      DataPoolIntens::Instance().getCycleName( i, cycleName );
      label->setTitle( cycleName );
    }
  }

  // plot items
  m_currentLine = dynamic_cast<GuiQtFieldgroupLine*>(m_fieldgroup->addFieldgroupLine());
  m_fgLines.push_back(m_currentLine);
  assert( m_currentLine != 0 );
  std::vector<std::string>::iterator iterEnd = m_cycleMode ? m_columnLabels.end()-numCycleAttributes : m_columnLabels.end();
  std::vector<std::string>::iterator iter;
  for( iter = m_columnLabels.begin(); iter != iterEnd; ++iter) {
    GuiLabel* label = GuiFactory::Instance() -> createLabel( m_currentLine, GuiElement::align_Default );
    label->getElement()->setAlignment( GuiElement::align_Center );
    label->setTitle( (*iter) );
  }
  if (m_cycleMode) {
    std::vector<int>::const_iterator showIt = m_showCycleVector.begin();
    for (int i = 0; i < DataPoolIntens::Instance().numCycles(); ++i, ++showIt) {
      if (m_cycleMode && *showIt != 1) continue;  // this cycle is not shown

      GuiSeparator *sep = GuiFactory::Instance() -> createSeparator( m_currentLine );
      sep->setOrientation(GuiElement::orient_Vertical);
      m_currentLine->attach( sep->getElement() );
      for (int j = numCycleAttributes; j > 0; --j) {
        GuiLabel* label = GuiFactory::Instance()->createLabel( m_currentLine, GuiElement::align_Default );
        label->getElement()->setAlignment( GuiElement::align_Center );
        label->setTitle( m_columnLabels[m_columnLabels.size()-j] );
      }
    }
  }
  return m_fieldgroup;
}

// --------------------------------------------------------------------------- //
// addItem --                                                                  //
// --------------------------------------------------------------------------- //
bool QtPlot2dConfigDialog::addItem( const std::string &label, std::vector<XferDataItem *> &xfers,
                                    XferDataItem* colorXfer, XferDataItem* symbolColorXfer,
                                    XferDataItem* lineStyleXfer, XferDataItem* symbolStyleXfer,
                                    XferDataItem* symbolSizeXfer, XferDataItem* unitXfer,
                                    eItemStyle itemStyle, bool new_group ){
  assert( m_dataset[0] != 0 );

  std::vector<XferDataItem *>::iterator iter;
  for( iter = xfers.begin(); iter != xfers.end(); ++iter ){
    if( (*iter) == 0 ){
      return false;
    }
  }

  GuiFactory *guifactory = GuiFactory::Instance();
  // new line
  m_currentLine = dynamic_cast<GuiQtFieldgroupLine*>(getFieldgroup()->addFieldgroupLine());
  m_fgLines.push_back(m_currentLine);
  assert(m_currentLine != 0);
  // label
  GuiLabel *guiLabel = guifactory -> createLabel( m_currentLine
                                                  , GuiElement::align_Default );
  guiLabel->setLabel( label );

  GuiDataField *button = 0;
  GuiRadioButton *previousButton = 0;

  for( iter = xfers.begin(); iter != xfers.end(); ++iter ){
    (*iter)->getUserAttr()->SetRadioButton();
    button = guifactory -> createDataField( m_currentLine,  (*iter) );
    button->getElement()->setAlignment( GuiElement::align_Center );
    m_currentLine->attach( button->getElement() );
    if( previousButton != 0 )
      dynamic_cast<GuiRadioButton*>(button)->connectRadioBox( previousButton );
    previousButton = dynamic_cast<GuiRadioButton*>(button);
  }
  XferDataItemIndex *inx = 0;

  if( getStyle() == LIST ){
    if (xfers.size() ) {
      GuiQtRadioButton *radioBtn=0;
      if (!m_combosXfer) {
        m_combosXfer = new XferDataItem(m_drefXComboboxValue );
        m_combosXfer->getUserAttr()->SetRadioButton();
        m_xDataField = GuiFactory::Instance()->createDataField( m_currentLine, m_combosXfer );
        radioBtn =  dynamic_cast<GuiQtRadioButton*>(m_xDataField);
      } else
        radioBtn = dynamic_cast<GuiQtRadioButton*>( dynamic_cast<GuiQtDataField*>(m_xDataField)->CloneForFieldgroupTable()->getElement() );
      radioBtn->setAlignment( GuiElement::align_Center );
      if( (inx = radioBtn->DataItem()->getIndex( m_numItems ) ) == 0 ) {
        inx = radioBtn->newDataItemIndex();
      }
      inx->setIndex( radioBtn->Data(), m_numItems );
      radioBtn->DataItem()->setDimensionIndizes();
      m_currentLine->attach( radioBtn->getElement() );
      radioBtn->getElement()->setParent( m_currentLine );
      if( !new_group && m_previousButton != 0 )
        radioBtn->connectRadioBox( m_previousButton );
      else
        m_previousButton = radioBtn;
    }
  } else {
    if( getStyle() == COMBOBOX &&
        m_numItems == 0 ) {
      assert( m_combosXfer == 0 );
      m_combosXfer = new XferDataItem(m_drefXComboboxValue );
      m_xDataField = dynamic_cast<GuiQtDataField*>(GuiFactory::Instance()->createDataField( m_currentLine, m_combosXfer ));
      m_xDataField->setLength( getComboboxLength() );
      if( ( inx = m_xDataField->DataItem()->getIndex( 0 ) ) == 0 )
        inx = m_xDataField->newDataItemIndex();
      m_currentLine->attach( m_xDataField->getElement() );
    }
    else{
      GuiDataField *combobox = dynamic_cast<GuiQtDataField*>(m_xDataField)->CloneForFieldgroupTable();
      combobox->getElement()->setParent( m_currentLine );
      inx = combobox->DataItem()->getIndex( 0 );
      inx->setIndex( combobox->Data(), m_numItems );
      combobox->DataItem()->setDimensionIndizes();
      m_currentLine->attach( combobox->getElement() );
    }
  }

  m_dataset[0]->setItemValues( label, m_numItems );
  ++m_numItems;

  XferDataItemIndex *cycleInx = 0;
  XferDataItemIndex *cycleSymInx = 0;
  std::vector<int>::const_iterator showIt = m_showCycleVector.begin();
  for (int i = 0; i < (m_cycleMode ? DataPoolIntens::Instance().numCycles() : 1); ++i, ++showIt) {

    if (m_cycleMode && *showIt != 1) continue;  // this cycle is not shown

    // first time, empty columns
    if ((!i || (m_cycleMode && !cycleInx)) && xfers.empty()) {  // insert a void
      m_currentLine->attach( new GuiVoid(m_currentLine) );
      m_currentLine->attach( new GuiVoid(m_currentLine) );
      m_currentLine->attach( new GuiVoid(m_currentLine) );
    }

    // color column
    if (m_cycleMode) m_currentLine->attach( new GuiVoid(m_currentLine) );
    if (itemStyle & SHOW_LINE_COLOR) {
      colorXfer = new XferDataItem(*colorXfer);
      button = guifactory->createDataField( m_currentLine, colorXfer );
      BUG_DEBUG("createDataField cycle["<<i<<"]  xferVN: " << colorXfer->getFullName(true));
      button->getElement()->setAlignment( GuiElement::align_Center );
      button->setLength(4);
      m_currentLine->attach( button->getElement() );

      if (m_cycleMode) {
        if ((cycleInx = button->DataItem()->getIndex(1)) == 0) {
          cycleInx = button->DataItem()->newDataItemIndex();
        }
        cycleInx->setIndex( button->Data(), i );
        button->DataItem()->setDimensionIndizes();
      }
    } else {
      m_currentLine->attach( new GuiVoid(m_currentLine) );
    }

    // symbolColor column
    if (itemStyle & SHOW_SYMBOL_COLOR) {
      symbolColorXfer = new XferDataItem(*symbolColorXfer);
      button = guifactory->createDataField( m_currentLine, symbolColorXfer );
      button->getElement()->setAlignment( GuiElement::align_Center );
      button->setLength(4);
      m_currentLine->attach( button->getElement() );

      if (m_cycleMode) {
        if ((cycleInx = button->DataItem()->getIndex(1)) == 0) {
          cycleInx = button->DataItem()->newDataItemIndex();
        }
        cycleInx->setIndex( button->Data(), i );
        button->DataItem()->setDimensionIndizes();
      }
    } else {
      m_currentLine->attach( new GuiVoid(m_currentLine) );
    }

    // lineStyle column
    XferDataItem* styleXfer;
    if (itemStyle & SHOW_LINE_STYLE) {
      styleXfer = new XferDataItem(*lineStyleXfer);
      button = guifactory->createDataField( m_currentLine, styleXfer );
      button->getElement()->setAlignment( GuiElement::align_Center );
      button->setLength(8);
      m_currentLine->attach( button->getElement() );
      if (m_cycleMode) {
        if ((cycleInx = button->DataItem()->getIndex(1)) == 0) {
          cycleInx = button->DataItem()->newDataItemIndex();
        }
        cycleInx->setIndex( button->Data(), i );
        button->DataItem()->setDimensionIndizes();
      }
    } else {
      m_currentLine->attach( new GuiVoid(m_currentLine) );
    }

    // symbolStyle column
    if (itemStyle & SHOW_SYMBOL_STYLE) {
      symbolStyleXfer = new XferDataItem(*symbolStyleXfer);
      button = guifactory->createDataField( m_currentLine, symbolStyleXfer );
      button->getElement()->setAlignment( GuiElement::align_Center );
      button->setLength(10);
      m_currentLine->attach( button->getElement() );
      if (m_cycleMode) {
        if ((cycleInx = button->DataItem()->getIndex(1)) == 0) {
          cycleInx = button->DataItem()->newDataItemIndex();
        }
        cycleInx->setIndex( button->Data(), i );
        button->DataItem()->setDimensionIndizes();
      }
    } else {
      m_currentLine->attach( new GuiVoid(m_currentLine) );
    }

    // symbolStyle column
    if (itemStyle & SHOW_SYMBOL_STYLE) {
      symbolSizeXfer = new XferDataItem(*symbolSizeXfer);
      button = guifactory->createDataField( m_currentLine, symbolSizeXfer );
      button->getElement()->setAlignment( GuiElement::align_Center );
      button->setLength(4);
      m_currentLine->attach( button->getElement() );
      if (m_cycleMode) {
        if ((cycleInx = button->DataItem()->getIndex(1)) == 0) {
          cycleInx = button->DataItem()->newDataItemIndex();
        }
        cycleInx->setIndex( button->Data(), i );
        button->DataItem()->setDimensionIndizes();
      }
    } else {
      m_currentLine->attach( new GuiVoid(m_currentLine) );
    }

    // unit column
    GuiLabel* label = guifactory->createLabel( m_currentLine, GuiElement::align_Default);
    label->getElement()->setAlignment( GuiElement::align_Center );
    std::string s;
    unitXfer->getValue(s);
    label->setLabel(s);
  }

  return true;
}

// --------------------------------------------------------------------------- //
// addSeparator --                                                             //
// --------------------------------------------------------------------------- //

bool QtPlot2dConfigDialog::addSeparator(){
  // new line
  m_currentLine = dynamic_cast<GuiQtFieldgroupLine*>(getFieldgroup()->addFieldgroupLine());
  m_fgLines.push_back(m_currentLine);
  assert( m_currentLine != 0 );
  // label
  GuiSeparator *sep = GuiFactory::Instance() -> createSeparator( m_currentLine );
  sep->setOrientation(GuiElement::orient_Horizontal);
  m_currentLine->attach( sep->getElement() );

  return true;
}

/* --------------------------------------------------------------------------- */
/* createLocalData --                                                          */
/* --------------------------------------------------------------------------- */
void QtPlot2dConfigDialog::createLocalData( const std::string &name ) {
  DataPoolIntens &dpi = DataPoolIntens::Instance();

  std::string stritem[7] = {
    compose("@ConfigDialogXComboboxValue%1", name),
    compose("@ConfigDialogLineStyleComboboxValue%1", name),
    compose("@ConfigDialogSymbolStyleComboboxValue%1", name),
    compose("@ConfigDialogSymbolSizeComboboxValue%1", name),
    compose("@ConfigDialogLineColor%1", name),
    compose("@ConfigDialogSymbolColor%1", name),
    compose("@ConfigDialogUnitComboboxValue%1", name),
  };
  DataDictionary *dict = 0;
  DataReference* dref;
  for (int i=0; i < 7; ++i) {
    std::string ostrsetStr;

    if (i <= 3) { // only these have are combobox
      // dataset
      ostrsetStr = compose("%1_%2", s_datasetName[i], name);
      m_dataset[i] = dpi.newDataSet( ostrsetStr, true, true );
      if (!m_dataset[i]) // plot is cloned
        m_dataset[i] = dpi.getDataSet( ostrsetStr );
      m_dataset[i]->resetInvalidEntry();
    }

    // data reference
    if ((dref = dpi.getDataReference(stritem[i]))) {
      // parent is a cloned GuiElement
      delete dref;
    } else {
      dict = dpi.getDataPool().AddToDictionary( "",
                                                stritem[i],
                                                i <= 2 ? DataDictionary::type_Integer
                                                : DataDictionary::type_String );
    }
    if (dict) { // plot is not cloned
      dict->setItemProtected();
      static_cast<UserAttr*>(dict->GetAttr())->SetEditable();
      if (i <= 3) { // only these have are combobox
        static_cast<UserAttr*>(dict->GetAttr())->SetDataSetName( ostrsetStr );
        static_cast<UserAttr*>(dict->GetAttr())->SetCombobox();
      } else {
        static_cast<UserAttr*>(dict->GetAttr())->SetButton();
        static_cast<UserAttr*>(dict->GetAttr())->setColorPicker();
        static_cast<UserAttr*>(dict->GetAttr())->SetLabel(" ");
      }
    }
  }

  // x combox
  m_drefXComboboxValue = dpi.getDataReference( compose("@ConfigDialogXComboboxValue%1", name) );
  dict = m_drefXComboboxValue->GetDict();
  assert( dict != 0 );
  dict->setItemProtected();
  assert( m_drefXComboboxValue != 0 );

  // line color
  m_drefLineColor = dpi.getDataReference( compose("@ConfigDialogLineColor%1", name) );
  dict = m_drefLineColor->GetDict();
  assert( dict != 0 );
  dict->setItemProtected();
  assert( m_drefLineColor != 0 );
  // symbol color
  m_drefSymbolColor = dpi.getDataReference( compose("@ConfigDialogSymbolColor%1", name) );
  dict = m_drefSymbolColor->GetDict();
  assert( dict != 0 );
  dict->setItemProtected();
  assert( m_drefSymbolColor != 0 );
  // line style
  m_drefLineStyleComboboxValue = dpi.getDataReference( compose("@ConfigDialogLineStyleComboboxValue%1", name) );
  dict = m_drefLineStyleComboboxValue->GetDict();
  assert( dict != 0 );
  dict->setItemProtected();
  assert( m_drefLineStyleComboboxValue != 0 );
  // line style dataset
  m_dataset[1]->setItemValues( "No", 0 );
  m_dataset[1]->setItemValues( "Solid", 1 );
  m_dataset[1]->setItemValues( "Dash", 2 );
  m_dataset[1]->setItemValues( "Dot", 3 );
  m_dataset[1]->setItemValues( "DashDot", 4 );
  m_dataset[1]->setItemValues( "DashDotDot", 5 );

  // symbol style
  m_drefSymbolStyleComboboxValue = dpi.getDataReference( compose("@ConfigDialogSymbolStyleComboboxValue%1", name) );
  dict = m_drefSymbolStyleComboboxValue->GetDict();
  assert( dict != 0 );
  dict->setItemProtected();
  assert( m_drefSymbolStyleComboboxValue != 0 );

  // symbol style dataset
  m_dataset[2]->setItemValues( "NoSymbol", -1 );
  m_dataset[2]->setItemValues( "Ellipse", 0 );
  m_dataset[2]->setItemValues( "Rect", 1 );
  m_dataset[2]->setItemValues( "Diamond", 2 );
  m_dataset[2]->setItemValues( "Triangle", 3 );
  m_dataset[2]->setItemValues( "DTriangle", 4 );
  m_dataset[2]->setItemValues( "UTriangle", 5 );
  m_dataset[2]->setItemValues( "LTriangle", 6 );
  m_dataset[2]->setItemValues( "RTriangle", 7 );
  m_dataset[2]->setItemValues( "Cross", 8 );
  m_dataset[2]->setItemValues( "XCross", 9 );
  m_dataset[2]->setItemValues( "HLine", 10 );
  m_dataset[2]->setItemValues( "VLine", 11 );
  m_dataset[2]->setItemValues( "Star1", 12 );
  m_dataset[2]->setItemValues( "Star2", 13 );
  m_dataset[2]->setItemValues( "Hexagon", 14 );

  // symbol size
  m_drefSymbolSizeComboboxValue = dpi.getDataReference( compose("@ConfigDialogSymbolSizeComboboxValue%1", name) );
  dict = m_drefSymbolSizeComboboxValue->GetDict();
  assert( dict != 0 );
  dict->setItemProtected();
  assert( m_drefSymbolSizeComboboxValue != 0 );

  // symbol style dataset
  m_dataset[3]->setItemValues( "3", 3 );
  m_dataset[3]->setItemValues( "6", 6 );
  m_dataset[3]->setItemValues( "9", 9 );
  m_dataset[3]->setItemValues( "12", 12 );
  m_dataset[3]->setItemValues( "16", 16 );
  m_dataset[3]->setItemValues( "20", 20 );

  // unit dataset
  m_drefUnitComboboxValue = dpi.getDataReference( compose("@ConfigDialogUnitComboboxValue%1", name) );
}

/* --------------------------------------------------------------------------- */
/* createDataset --                                                            */
/* --------------------------------------------------------------------------- */
void QtPlot2dConfigDialog::createDataset(){
  // eigentlich übernimmt das der Datapool für uns, doch leider sind wir zu spät....
  for (int i=0; i < 4; ++i) {
    if( m_dataset[i] != 0 ) {
      m_dataset[i]->create( compose("%1_%2", s_datasetName[i], getName()) );
    }
  }
}

// --------------------------------------------------------------------------- //
// ButtonPressed() --                                                          //
// --------------------------------------------------------------------------- //

void QtPlot2dConfigDialog::ButtonPressed( GuiEventData *event ){
  if( event == 0 )
    return;
  GuiElement *element = event->m_element;
  if( element == 0 )
    return;
  if( element == m_closeButton->getElement() )
    m_listener->closeEvent( m_event );
  else if( element == m_resetButton->getElement() )
    m_listener->resetEvent( m_event );
  else
    assert( false );
}

// --------------------------------------------------------------------------- //
// create --                                                                   //
// --------------------------------------------------------------------------- //

void QtPlot2dConfigDialog::create(){
  if (!m_created) {
    createDataset();
    // Buttonbar generieren
    GuiQtButtonbar *bar = new GuiQtButtonbar( this );
    attach( bar );
    // Reset Button generieren und an das Buttonbar anfuegen
    GuiEventData *event = new GuiEventData();
    m_resetButton = GuiFactory::Instance() -> createButton( bar, this, event );
    m_resetButton->setLabel( _("Reset") );
    // Close Button generieren und an das Buttonbar anfuegen
    event = new GuiEventData();
    m_closeButton = GuiFactory::Instance() -> createButton( bar, this, event );
    m_closeButton->setLabel( _("Close") );

    GuiQtForm::create();
    setDialogExpandPolicy(expand_AtMapTime);
    ///	getDialogWidget()->setParent( getParent()->getQtElement()->myWidget() );
    m_created = true;
    getDialogWidget()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_vertCont->myWidget()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  }
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void QtPlot2dConfigDialog::manage() {

  GuiQtForm::manage();

  QSize sh(getDialogWidget()->sizeHint());
#if QT_VERSION > 0x050600
  QRect maxDesktop= QGuiApplication::primaryScreen()->availableGeometry();
#else
  QRect maxDesktop= QApplication::desktop()->availableGeometry();
#endif

  // verkleinern der Höhe
  if (sh.height() > maxDesktop.height()) {
    sh.setHeight( maxDesktop.height() );
    getDialogWidget()->move(getDialogWidget()->x(), maxDesktop.y());
  }
  // verkleinern der Breite
  if (sh.width() > maxDesktop.width()) {
    sh.setWidth( maxDesktop.width() );
    getDialogWidget()->move(maxDesktop.x(), getDialogWidget()->y());
  }
  QScrollArea *scrollView = getDialogWidget()->findChild<QScrollArea *>("FormScrollView");
  if (scrollView) {
    int add = scrollView->verticalScrollBar()->sizeHint().width();
    if ((sh.height()+add) < maxDesktop.height())
      sh.setHeight(sh.height() + add);
    if ((sh.width()+add) < maxDesktop.width())
      sh.setWidth(sh.width() + add);
  }
  getDialogWidget()->resize(sh.width(), sh.height());
  BUG_DEBUG("QtPlot2dConfigDialog New Size: "<< sh.width() << ", " << sh.height());
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void QtPlot2dConfigDialog::update( UpdateReason reason ){
  if ( myWidget() && !myWidget()->isVisible() ) // only update visible dialog
    return;
  if (m_xDataField)
    m_xDataField->getElement()->update( reason_Always );

  GuiQtForm::update( reason );
  if (GuiQtForm::myWidget()) {
    // GuiQtForm::myWidget()->setMaximumSize( GuiQtForm::myWidget()->sizeHint() );
  }

  // update parent plot2d
  // only really needed if we change the xaxis
  // and avoid recursive call
  if (m_parent) {
    static bool m=true;
    if (m) {
      m = false;
      m_parent->update( reason_Always );
      m = true;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* isUpdated --                                                                */
/* --------------------------------------------------------------------------- */
bool QtPlot2dConfigDialog::isUpdated( TransactionNumber trans ){
  if( m_drefXComboboxValue != 0 ){
    if( m_drefXComboboxValue->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
      return true;
    }
  }
  if( m_drefLineColor != 0 ){
    if( m_drefLineColor->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
      return true;
    }
  }
  if( m_drefSymbolColor != 0 ){
    if( m_drefSymbolColor->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
      return true;
    }
  }
  if( m_drefLineStyleComboboxValue != 0 ){
    if( m_drefLineStyleComboboxValue->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
      return true;
    }
  }
  if( m_drefSymbolStyleComboboxValue != 0 ){
    if( m_drefSymbolStyleComboboxValue->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
      return true;
    }
  }
  if( m_drefSymbolSizeComboboxValue != 0 ){
    if( m_drefSymbolSizeComboboxValue->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
      return true;
    }
  }
  return false; //QtConfigDialog::isUpdated( trans );
}

/* --------------------------------------------------------------------------- */
/* showSymbolColorColumn --                                                    */
/* --------------------------------------------------------------------------- */
void QtPlot2dConfigDialog::showSymbolColorColumn(bool visible, int columnOffset) {
  int offset = m_cycleMode ? SYMBOL_COLOR_COLUMN_INDEX+1 : SYMBOL_COLOR_COLUMN_INDEX;
  for (int i = 0; i < (m_cycleMode ? DataPoolIntens::Instance().numCycles() : 1); ++i) {
    int idx = columnOffset + offset + i*(numCycleAttributes+1);
    getFieldgroup()->showColumn(idx, visible, (m_cycleMode ? 2 : 1));
  }
}
