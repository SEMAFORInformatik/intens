#include <qobject.h>
#include <QApplication>
#include <QClipboard>
#include <QSettings>
#include <QMimeData>
#include <QBuffer>
#include "gui/qt/GuiQwtPlot.h"
#include "gui/qt/GuiQtNavigatorMenu.h"
#include "gui/qt/GuiQtScrolledText.h"
#include "gui/qt/GuiQtScrolledlist.h"
#include "gui/qt/GuiQtPulldownMenu.h"
#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/qt/GuiQtFactory.h"
#include "gui/headless/HeadlessGuiFactory.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtLabel.h"
#include "gui/qt/GuiQtPixmap.h"
#include "gui/qt/GuiQtFieldgroup.h"
#include "gui/qt/GuiQtFieldgroupLine.h"
#include "gui/qt/GuiQtOrientationContainer.h"
#include "gui/qt/GuiQtButton.h"
#include "gui/qt/GuiQtButtonbar.h"
#include "gui/qt/GuiQtMenubar.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/qt/GuiQtMenuToggle.h"
#include "gui/qt/GuiQtSeparator.h"
#include "gui/qt/GuiQtMessageLine.h"
#include "gui/qt/GuiQtApplHeader.h"
#include "gui/qt/QtDialogWorkClock.h"
#include "gui/qt/GuiQtFolder.h"
#include "gui/GuiButtonListener.h"
#include "gui/qt/GuiQtForm.h"
#include "gui/qt/GuiQtTable.h"
#include "gui/qt/GuiQtTableLabelItem.h"
#include "gui/qt/GuiQtTableDataItem.h"
#include "gui/qt/GuiQtTableComboBoxItem.h"
#include "gui/qt/GuiQtList.h"
#include "gui/qt/GuiQtFilterList.h"
#include "gui/qt/GuiQtDataField.h"
#include "gui/qt/QtDialogConfirmation.h"
#include "gui/qt/QtDialogInformation.h"
#include "gui/qt/QtDialogInput.h"
#include "gui/qt/QtDialogWarning.h"
#include "gui/qt/QtDialogFileSelection.h"
#include "gui/qt/QtDialogCopyright.h"
#include "gui/qt/QtDialogInputExt.h"
#include "gui/qt/GuiQtCycleButton.h"
#include "gui/qt/GuiQtIndex.h"
#include "gui/qt/GuiQtIndexMenu.h"
#include "gui/qt/QtTimer.h"
#include "gui/qt/QtSimpleAction.h"
#include "gui/qt/GuiQtCycleDialog.h"
#include "gui/qt/GuiQtTextfield.h"
#include "gui/qt/GuiQtComboBox.h"
#include "gui/qt/GuiQtToggle.h"
#include "gui/qt/GuiQtRadioButton.h"
#include "gui/qt/GuiQtFieldButton.h"
#include "gui/qt/GuiQtText.h"
#include "gui/qt/GuiQtNavigator.h"
#include "gui/qt/QtDialogUserPassword.h"
#include "gui/qt/GuiQwtSlider.h"
#include "gui/qt/GuiQtImage.h"
#include "gui/qt/GuiQtLinePlot.h"
#include "gui/qt/GuiQwtThermo.h"
#include "gui/qt/GuiQtPrinterDialog.h"
#include "gui/qt/QtDialogProgressBar.h"
#include "gui/qt/GuiQtProgressBar.h"
#include "gui/qt/QtDialogCompare.h"

#include "plot/qt/QtListPlot.h"
#include "plot/PSPlot.h"
#include "plot/Plot2dMenuDescription.h"
#include "plot/qt/QtSimpel.h"
#include "gui/qt/GuiQt3dPlot.h"

#include "operator/QtServerSocket.h"
#include "operator/QtClientSocket.h"
#include "operator/MessageQueueThreads.h"

#include "job/JobManager.h"
#include "app/AppData.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"

INIT_LOGGER();

HeadlessGuiFactory* s_headlessGuiFactory=new HeadlessGuiFactory();
#define HEADLESS_CHECK() (JobManager::Instance().getActiveJobController() && s_headlessGuiFactory && AppData::Instance().HeadlessWebMode())

/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */
GuiQtFactory::GuiQtFactory() {
  QtDialogProgressBar::Instance();
  QtDialogCompare::Instance();
}

/* --------------------------------------------------------------------------- */
/* attachEventLoopListener --                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtFactory::attachEventLoopListener( GuiEventLoopListener *lsnr ) {
  return GuiQtManager::Instance().attachEventLoopListener( lsnr );
}

/* --------------------------------------------------------------------------- */
/* createTimer --                                                              */
/* --------------------------------------------------------------------------- */
Timer* GuiQtFactory::createTimer( int interval, bool restart ) {
  return new QtTimer( interval, restart );
}

/* --------------------------------------------------------------------------- */
/* createSimpleAction --                                                       */
/* --------------------------------------------------------------------------- */
void GuiQtFactory::createSimpleAction( SimpleAction::Object *obj ){
  QtSimpleAction *action = new QtSimpleAction( obj );
  // Das Objekt sorgt für sich selber.
}

/* --------------------------------------------------------------------------- */
/* createManager --                                                            */
/* --------------------------------------------------------------------------- */

GuiManager *GuiQtFactory::createManager(){
  return GuiQtManager::createManager();
}

GuiCycleButton *GuiQtFactory::createCycleButton( GuiElement *parent ){
  return new GuiQtCycleButton( parent );
}

GuiLabel  *GuiQtFactory::createLabel( GuiElement *parent, GuiElement::Alignment a ){
  GuiQtLabel *e = new GuiQtLabel( parent );
  e -> setAlignment( a );
  parent -> attach( e );
  return e;
}

DialogCopyright *GuiQtFactory::createDialogCopyright(){
  return new QtDialogCopyright();
}

GuiForm   *GuiQtFactory::createForm( const std::string& name ){
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  GuiQtManager *mgr = &(GuiQtManager::Instance());
  GuiQtForm *form = new GuiQtForm( mgr, true, name );
  form->setTitle( name );
  form->setParentDialog( mgr );
  return form;
}

GuiButton *GuiQtFactory::createButton( GuiElement *parent, GuiButtonListener *listener, GuiEventData *event = 0 ){
  GuiQtButton *mb = new GuiQtButton( parent, listener, event );

  if( event == 0 ){ // we must link them in reverse direction also (bi-directional)
    if( listener != 0 )
      listener->setButton( mb );
  }
  else
    event->m_element = mb;

  parent -> attach( mb );
  return mb;
}

/* --------------------------------------------------------------------------- */
/* createToggle --                                                             */
/* --------------------------------------------------------------------------- */

GuiDataField *GuiQtFactory::createToggle( GuiElement *parent ){
  return new GuiQtToggle( parent );
}


/* --------------------------------------------------------------------------- */
/* createFieldgroup --                                                         */
/* --------------------------------------------------------------------------- */

// Es existieren 2 createFieldgroup Methoden, da nicht in allen Faellen
// ein attach benoetigt wrid
GuiFieldgroup  *GuiQtFactory::createFieldgroup( GuiElement *parent,
                                                const std::string &name ){
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  GuiQtFieldgroup *e = new GuiQtFieldgroup( parent, name );
  return e;
}

GuiFieldgroup *GuiQtFactory::createFieldgroup( GuiElement *parent,
                                               GuiElement *at,
                                               const std::string &name ){
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  GuiQtFieldgroup *e = new GuiQtFieldgroup( parent, name );
  at -> attach( e );
  return e;
}

/* --------------------------------------------------------------------------- */
/* createFieldgroupLine --                                                     */
/* --------------------------------------------------------------------------- */

GuiFieldgroupLine *GuiQtFactory::createFieldgroupLine(GuiElement *parent){
  return new GuiQtFieldgroupLine(parent);
}

/* --------------------------------------------------------------------------- */
/* createVerticalContainer --                                                  */
/* --------------------------------------------------------------------------- */

GuiOrientationContainer *GuiQtFactory::createVerticalContainer( GuiElement *parent ) {
  return new GuiQtOrientationContainer( GuiElement::orient_Vertical, parent );
}

/* --------------------------------------------------------------------------- */
/* createHorizontalContainer --                                                */
/* --------------------------------------------------------------------------- */

GuiOrientationContainer *GuiQtFactory::createHorizontalContainer( GuiElement *parent ) {
  return new GuiQtOrientationContainer( GuiElement::orient_Horizontal, parent );
}

/* --------------------------------------------------------------------------- */
/* createApplHeader --                                                         */
/* --------------------------------------------------------------------------- */

GuiApplHeader *GuiQtFactory::createApplHeader ( GuiElement *parent ){
  return new GuiQtApplHeader( parent );
}

GuiIndexMenu *GuiQtFactory::createIndexMenu(GuiElement *parent, GuiIndexMenu::HideFlag flag){
  return new GuiQtIndexMenu(parent, flag);
}

GuiMenubar *GuiQtFactory::createMenubar ( GuiElement *parent ){
  return new GuiQtMenubar( parent );
}

GuiPulldownMenu *GuiQtFactory::createPulldownMenu( GuiElement *parent, const std::string &name ){
  return new GuiQtPulldownMenu( parent, name );
}

GuiButtonbar  *GuiQtFactory::createButtonbar( GuiElement *parent ){
  GuiQtButtonbar *bb = new GuiQtButtonbar( parent );
  if ( parent != 0 ) parent -> attach( bb );
  return bb;
}

/* --------------------------------------------------------------------------- */
/* createTable --                                                              */
/* --------------------------------------------------------------------------- */

GuiTable* GuiQtFactory::createTable( GuiElement *parent, const std::string &name ) {
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  return new GuiQtTable( parent, name );
}

/* --------------------------------------------------------------------------- */
/* createTableLabelItem --                                                     */
/* --------------------------------------------------------------------------- */

GuiTableItem* GuiQtFactory::createTableLabelItem( GuiElement *parent, const std::string &label, GuiElement::Alignment align ) {
  return new GuiQtTableLabelItem( parent, label, align );
}

/* --------------------------------------------------------------------------- */
/* createTableDataItem --                                                      */
/* --------------------------------------------------------------------------- */

GuiTableItem* GuiQtFactory::createTableDataItem( GuiElement *parent ) {
  return new GuiQtTableDataItem( parent );
}

/* --------------------------------------------------------------------------- */
/* createTableComboBoxItem --                                                  */
/* --------------------------------------------------------------------------- */

GuiTableItem* GuiQtFactory::createTableComboBoxItem( GuiElement *parent, const std::string& datasetName ) {
  GuiQtTableComboBoxItem *cbi = new GuiQtTableComboBoxItem( parent );
  cbi->setSetName(datasetName);
  return cbi;
}

/* --------------------------------------------------------------------------- */
/* create2dPlot --                                                             */
/* --------------------------------------------------------------------------- */

Gui2dPlot* GuiQtFactory::create2dPlot( const std::string &name, bool isPlot2d ) {
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  Gui2dPlot *plot=0;
  plot = new GuiQWTPlot( name );
  return plot;
}

/* --------------------------------------------------------------------------- */
/* create3dPlot --                                                             */
/* --------------------------------------------------------------------------- */
Gui3dPlot*  GuiQtFactory::create3dPlot( GuiElement *parent, const std::string &name ) {
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  Gui3dPlot *plot=0;
  plot = new GuiQt3dPlot( parent, name );
  return plot;
}

/* --------------------------------------------------------------------------- */
/* createList --                                                               */
/* --------------------------------------------------------------------------- */

GuiList* GuiQtFactory::createList( GuiElement *parent, const std::string &name ) {
  return new GuiQtList( parent, name );
}

/* --------------------------------------------------------------------------- */
/* createFilterList --                                                         */
/* --------------------------------------------------------------------------- */

GuiFilterList*  GuiQtFactory::createFilterList ( GuiElement *parent,
						    GuiFilterListListener &listener,
						    const std::string &name ) {
  return new GuiQtFilterList(parent, listener, name);
}

/* --------------------------------------------------------------------------- */
/* createFolder --                                                             */
/* --------------------------------------------------------------------------- */
GuiFolder *GuiQtFactory::createFolder( GuiElement *parent, const std::string &name ){
  return new GuiQtFolder( parent, name );
}

/* --------------------------------------------------------------------------- */
/* createPopupMenu --                                                          */
/* --------------------------------------------------------------------------- */

GuiPopupMenu *GuiQtFactory::createPopupMenu( GuiElement *parent ){
  return new GuiQtPopupMenu( parent );
}

/* --------------------------------------------------------------------------- */
/* createNavigatorMenu --                                                      */
/* --------------------------------------------------------------------------- */

GuiPopupMenu *GuiQtFactory::createNavigatorMenu( GuiElement *parent ){
  return new GuiQtNavigatorMenu( parent );
}


/* --------------------------------------------------------------------------- */
/* createMenuButton --                                                         */
/* --------------------------------------------------------------------------- */

GuiMenuButton *GuiQtFactory::createMenuButton( GuiElement *parent, GuiButtonListener *listener, GuiEventData *event, bool hide_disabled, const std::string& name ){
  return new GuiQtMenuButton( parent, listener, event, hide_disabled, name );
}

/* --------------------------------------------------------------------------- */
/* createMenuToggle --                                                         */
/* --------------------------------------------------------------------------- */

GuiMenuToggle *GuiQtFactory::createMenuToggle( GuiElement *parent, GuiToggleListener *listener, GuiEventData *event ){
  return new GuiQtMenuToggle( parent, listener, event );
}

/* --------------------------------------------------------------------------- */
/* createSeparator --                                                          */
/* --------------------------------------------------------------------------- */

GuiSeparator *GuiQtFactory::createSeparator( GuiElement *parent ){
  return new GuiQtSeparator( parent );
}

/* --------------------------------------------------------------------------- */
/* createMessageLine --                                                        */
/* --------------------------------------------------------------------------- */

GuiMessageLine *GuiQtFactory::createMessageLine( GuiElement *parent ){
  return new GuiQtMessageLine( parent );
}

/* --------------------------------------------------------------------------- */
/* createScrolledText --                                                       */
/* --------------------------------------------------------------------------- */

GuiScrolledText *GuiQtFactory::createScrolledText( GuiElement *parent, const std::string &name ){
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  GuiQtScrolledText *text = new GuiQtScrolledText( parent, name );
  if( !text->createDataReference( name ) ){
    delete text;
    return 0;
  }
  return text;
//   return new GuiQtScrolledText( parent, name );
}

/* --------------------------------------------------------------------------- */
/* createScrolledlist --                                                       */
/* --------------------------------------------------------------------------- */

GuiScrolledlist *GuiQtFactory::createScrolledlist( GuiElement *parent, GuiScrolledlistListener *listener) {
  GuiQtScrolledlist *list = new GuiQtScrolledlist(parent, listener);
  return list;
}

/* --------------------------------------------------------------------------- */
/* createDialogWorkClock --                                                    */
/* --------------------------------------------------------------------------- */

DialogWorkClock  *GuiQtFactory::createDialogWorkClock( GuiElement *element
						       , DialogWorkClockListener *listener
						       , const std::string &title
						       , const std::string &msg ){
  return new QtDialogWorkClock( listener );
}

/* --------------------------------------------------------------------------- */
/* showDialogConfirmation --                                                   */
/* --------------------------------------------------------------------------- */

GuiElement::ButtonType GuiQtFactory::showDialogConfirmation( GuiElement *e
                                                             , const std::string &title
                                                             , const std::string &message
                                                             , ConfirmationListener *listener
                                                             , const std::map<GuiElement::ButtonType, std::string>& buttonText
                                                             , bool cancelBtn
                                                             , int defaultBtn ) {
  BUG_DEBUG("showDialogConfirmation: " << compose("title[%1], message[%2]", title, message));

  // webapi query
  if (HEADLESS_CHECK()) {
    GuiElement::ButtonType ret;
    ret = s_headlessGuiFactory->showDialogConfirmation(e, title, message, listener, buttonText, cancelBtn, defaultBtn);
    if (ret != GuiElement::button_None)
      return ret;
  }

  return QtDialogConfirmation::showDialog(e, title, message, listener, buttonText, cancelBtn, defaultBtn);
}


/* --------------------------------------------------------------------------- */
/* showDialogInformation --                                                    */
/* --------------------------------------------------------------------------- */

bool GuiQtFactory::showDialogInformation( GuiElement *e
					  , const std::string &title
					  , const std::string &message
					  , InformationListener *listener) {
  BUG_DEBUG("showDialogInformation: " << compose("title[%1], message[%2]", title, message));

  // webapi query
  if (HEADLESS_CHECK()) {
    if (s_headlessGuiFactory->showDialogInformation(e, title, message, listener))
      return true;
  }

  return QtDialogInformation::showDialog(e, title, message, listener);
}


/* --------------------------------------------------------------------------- */
/* showDialogTextInput --                                                      */
/* --------------------------------------------------------------------------- */
std::string GuiQtFactory::showDialogTextInput(GuiElement *e
					     , const std::string &title
					     , const std::string &message
					     , const std::string &label
					     , bool &ok
					     , ConfirmationListener* listener ) {

  // webapi query
  if (HEADLESS_CHECK()) {
    std::string s = s_headlessGuiFactory->showDialogTextInput(e, title, message, label, ok, listener);
    return s;
  }

  //
  return QtDialogInput::getText(e, title, message, label, listener, ok);
}

/* --------------------------------------------------------------------------- */
/* showDialogTextInputExt --                                                   */
/* --------------------------------------------------------------------------- */
std::string GuiQtFactory::showDialogTextInputExt(GuiElement *e
					     , const std::string &title
					     , const std::string &message
					     , const std::string &label
               , const std::map<GuiElement::ButtonType, std::string>& buttonText
					     , GuiElement::ButtonType &buttonClicked
					     , ConfirmationListener* listener ) {

  // webapi query
  if (HEADLESS_CHECK()) {
    std::string ret = s_headlessGuiFactory->showDialogTextInputExt(e, title, message, label, buttonText, buttonClicked, listener);
    if (buttonClicked != GuiElement::button_None)
      return ret;
  }

  //
  std::string save = buttonText.find(GuiElement::button_Yes) != buttonText.end() ? buttonText.find(GuiElement::button_Yes)->second : _("Yes");
  std::string cancel = buttonText.find(GuiElement::button_Cancel) != buttonText.end() ? buttonText.find(GuiElement::button_Cancel)->second : _("Cancel");
  std::string apply = buttonText.find(GuiElement::button_Apply) != buttonText.end() ? buttonText.find(GuiElement::button_Apply)->second : _("Apply");
	QtDialogInputExt dialog(e ? e->getQtElement()->myWidget() : NULL,
                          QString::fromStdString(title),
                          QString::fromStdString(message),
                          QString::fromStdString(label),
                          QLineEdit::Normal,
                          QString::fromStdString(save),
                          QString::fromStdString(cancel),
                          QString::fromStdString(apply));
	buttonClicked = (GuiElement::ButtonType) dialog.exec();
  if (listener) {
    switch (buttonClicked) {
    case GuiElement::button_Yes:
      listener->confirmYesButtonPressed();
      break;
    case GuiElement::button_Apply:
      listener->confirmNoButtonPressed();
      break;
    case GuiElement::button_Cancel:
      listener->confirmCancelButtonPressed();
      break;
    default:
      listener->confirmCancelButtonPressed();
    }
  }
	return dialog.getText();
}

/* --------------------------------------------------------------------------- */
/* showDialogWarning --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiQtFactory::showDialogWarning( GuiElement *e
					  , const std::string &title
					  , const std::string &message
					  , InformationListener *listener) {
  BUG_DEBUG("showDialogWarning: " << compose("title[%1], message[%2]", title, message));

  // webapi query
  if (HEADLESS_CHECK()) {
    if (s_headlessGuiFactory->showDialogWarning(e, title, message, listener))
      return true;
  }

  return QtDialogWarning::showDialog(e, title, message, listener);
}


/* --------------------------------------------------------------------------- */
/* showDialogFileSelection --                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtFactory::showDialogFileSelection( GuiElement *element
					    , const std::string &title
					    , const std::string &filter
					    , const std::string &directory
					    , FileSelectListener *listener
					    , DialogFileSelection::Type type
					    , const std::vector<HardCopyListener::FileFormat> * const formats
					    , DialogFileSelection::Mode mode
              , const std::string &basename){
  BUG_DEBUG("showDialogFileSelection: " << compose("tilte[%1], filter[%2]", title, filter));

  // webapi query
  if (HEADLESS_CHECK()) {
    if (s_headlessGuiFactory->showDialogFileSelection(element, title, filter, directory,
                                                      listener, type, formats, mode, basename)) {
      BUG_DEBUG("showDialogFileSelection return true");
      return true;
    }
  }

  return QtDialogFileSelection::showDialog( element
					    , title
					    , filter
					    , directory
					    , listener
					    , type
					    , formats
					    , mode );
}

/* --------------------------------------------------------------------------- */
/* createPrinterDialog --                                                      */
/* --------------------------------------------------------------------------- */
GuiPrinterDialog *GuiQtFactory::createPrinterDialog(){
  return &GuiQtPrinterDialog::Instance();
}

/* --------------------------------------------------------------------------- */
/* createNavigator --                                                          */
/* --------------------------------------------------------------------------- */
GuiNavigator *GuiQtFactory::createNavigator( const std::string &id, GuiNavigator::Type navType ){
  if( GuiElement::findElement( id ) != 0 )
    return 0;
  return new GuiQtNavigator( id, navType );
}

/* --------------------------------------------------------------------------- */
/* showDialogUserPassword --                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtFactory::showDialogUserPassword( const std::string &database, const std::string &username
					   , UserPasswordListener *listener, DialogUnmapListener *unmap
					   , std::string errorMessage )
{
  BUG_DEBUG("showDialogUserPassword: " << compose("database[%1], username[%2]", database, username));
  QtDialogUserPassword* dlg = dynamic_cast<QtDialogUserPassword*>(QtDialogUserPassword::Instance(listener, unmap));
  if (dlg) dlg->showDialog(database, username, errorMessage);
}

/* --------------------------------------------------------------------------- */
/* createDataField --                                                          */
/* --------------------------------------------------------------------------- */
GuiDataField* GuiQtFactory::createDataField  ( GuiElement *parent, XferDataItem *dataitem, int prec ) {
  GuiQtDataField  *field = 0;
  DataReference *data = dataitem->Data();
  assert( data != 0 );
  UserAttr *attr = static_cast<UserAttr*>( data->getUserAttr() );

  switch( attr->GuiKind() ){
  case UserAttr::gui_kind_field:
  case UserAttr::gui_kind_label:
    if( (data->getDataType() == DataDictionary::type_CharData && prec > 1) ||
        (data->getDataType() == DataDictionary::type_String && prec > 1) ){
      field = new GuiQtText( parent );
    }
    else{
      field = new GuiQtTextfield( parent );
    }
    break;
  case UserAttr::gui_kind_button:
    field = new GuiQtFieldButton( parent );
    break;
  case UserAttr::gui_kind_slider:
    field = new GuiQwtSlider( parent );
    break;
  case UserAttr::gui_kind_progress:
    field = new GuiQtProgressBar(parent);
    break;
  case UserAttr::gui_kind_toggle:
    field = new GuiQtToggle( parent );
    break;
  case UserAttr::gui_kind_radiobutton:
    field = new GuiQtRadioButton( parent );
    break;
  case UserAttr::gui_kind_combobox:
    {
      GuiQtComboBox *combo = new GuiQtComboBox( parent );
      combo->setSetName( attr->DataSetName() );
      field = combo;
    }
    break;
  default:
    std::cerr << "unknown Gui Type" << std::endl;
    exit(1);
  }
  if( field->installDataItem( dataitem ) ){
    field->setHelptext( attr->Helptext() );
    return field;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* createPixmap --                                                             */
/* --------------------------------------------------------------------------- */
GuiPixmap* GuiQtFactory::createPixmap  ( GuiElement *parent, XferDataItem *dataitem ) {
  GuiQtPixmap    *field = new GuiQtPixmap( parent );
  DataReference  *data = dataitem->Data();
  UserAttr       *attr = static_cast<UserAttr*>( data->getUserAttr() );
  if( field->installDataItem( dataitem ) ){
    field->setHelptext( attr->Helptext() );
    return field;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* createGuiIndex --                                                           */
/* --------------------------------------------------------------------------- */
GuiIndex* GuiQtFactory::createGuiIndex  ( GuiElement *parent, const std::string &name ){
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  GuiQtIndex *index = new GuiQtIndex( parent, name );
  std::string indexname = "@index@" + name;
  DataPoolIntens::getDataPool().AddToDictionary( "", indexname
                                               , DataDictionary::type_Integer );
  DataReference *ref = DataPoolIntens::getDataReference( indexname );
  assert( ref != 0 );
  index->setDataReference( ref );
  return index;
}


/* --------------------------------------------------------------------------- */
/* getGuiIndex --                                                          */
/* --------------------------------------------------------------------------- */
GuiIndex* GuiQtFactory::getGuiIndex  ( const std::string &name ){
  GuiElement *element = GuiElement::findElement( name );
  if( element->Type() != GuiElement::type_Index ){
    return 0;
  }
  GuiQtIndex *index = element->getQtElement()->getQtIndex();
  if( index->installed() ){
    if (index->cloneable())
      return dynamic_cast<GuiQtIndex*>(index->clone());
    else assert(false);
  }
  return index;
}


/* --------------------------------------------------------------------------- */
/* createCycleDialog --                                                        */
/* --------------------------------------------------------------------------- */
GuiCycleDialog *GuiQtFactory::createCycleDialog(){
  static GuiCycleDialog *cycleDlg = 0;
  if (!cycleDlg)
    cycleDlg = new GuiQtCycleDialog();
  return cycleDlg;
}

/* --------------------------------------------------------------------------- */
/* createListPlot    --                                                        */
/* --------------------------------------------------------------------------- */
ListPlot* GuiQtFactory::createListPlot( GuiElement *parent, const std::string &name ) {
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  ListPlot *plot = new QtListPlot( parent, name );
  return plot;
}

/* --------------------------------------------------------------------------- */
/* createPSPlot    --                                                          */
/* --------------------------------------------------------------------------- */
PSPlot* GuiQtFactory::createPSPlot(  PSPlot::PSStream *stream ) {
  assert( false );
  return NULL;
}

/* --------------------------------------------------------------------------- */
/* getPlot2dMenuDescription    --                                              */
/* --------------------------------------------------------------------------- */
Plot2dMenuDescription *GuiQtFactory::getPlot2dMenuDescription() {
  return &Plot2dMenuDescription::Instance();
  return NULL;
}

/* --------------------------------------------------------------------------- */
/* newSimpelPlot --                                                            */
/* --------------------------------------------------------------------------- */
Simpel *GuiQtFactory::newSimpelPlot( GuiElement *parent, const std::string &name ){
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  return new QtSimpel( parent, name );
}

/* --------------------------------------------------------------------------- */
/* getSimpelPlot --                                                            */
/* --------------------------------------------------------------------------- */
Simpel *GuiQtFactory::getSimpelPlot( const std::string &name ){
  GuiElement *element = GuiElement::findElement( name );
  if( element->Type() != GuiElement::type_Simpel ){
    return 0;
  }
  return static_cast<QtSimpel *>(element);
//   assert( false );
//   return NULL;
}

/* --------------------------------------------------------------------------- */
/* createImage --                                                              */
/* --------------------------------------------------------------------------- */
GuiImage *GuiQtFactory::createImage( GuiElement *parent,
				     const std::string &name,
				     int frame_size ){
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  GuiImage *img = new GuiQtImage( parent, name, frame_size );
  return img;
}
/* --------------------------------------------------------------------------- */
/* createLinePlot --                                                              */
/* --------------------------------------------------------------------------- */
GuiImage *GuiQtFactory::createLinePlot( GuiElement *parent,
				     const std::string &name ){
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  GuiImage *img = new GuiQtLinePlot( parent, name );
  return img;
}

/* --------------------------------------------------------------------------- */
/* createThermo --                                                             */
/* --------------------------------------------------------------------------- */
GuiThermo *GuiQtFactory::createThermo( GuiElement *parent, const std::string &name ){
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  return new GuiQwtThermo( parent, name );
}

/* --------------------------------------------------------------------------- */
/* createTimeTable --                                                          */
/* --------------------------------------------------------------------------- */
GuiTimeTable *GuiQtFactory::createTimeTable( GuiElement *parent, const std::string &name ){
  if( GuiElement::findElement( name ) != 0 ){
    return 0;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getStandardWindow -                                                         */
/* --------------------------------------------------------------------------- */
GuiScrolledText *GuiQtFactory::getStandardWindow() {
  GuiElement *element = GuiElement::findElement( "STD_WINDOW" );
  GuiQtScrolledText *text;
  if( element == 0 ){
    text = new GuiQtScrolledText( 0, "STD_WINDOW" );
    text->showMenu( false );
    text->setMaxLines( 0 );
    text->createDataReference( "STD_WINDOW@" );
  }
  else{
    assert( element->Type() == GuiElement::type_ScrolledText );
    text = static_cast<GuiQtScrolledText *>(element);
  }
  return text;
}

/* --------------------------------------------------------------------------- */
/* getLogWindow --                                                             */
/* --------------------------------------------------------------------------- */
GuiScrolledText *GuiQtFactory::getLogWindow() {
  GuiElement *element = GuiElement::findElement( "LOG_WINDOW" );
  GuiQtScrolledText *text;
  if( element == 0 ){
    text = new GuiQtScrolledText( 0, "LOG_WINDOW" );
    text->showMenu( false );
    text->setMaxLines( AppData::Instance().MaxLines() );
    text->createDataReference( "LOG_WINDOW@" );
  }
  else{
    assert( element->Type() == GuiElement::type_ScrolledText );
    text = static_cast<GuiQtScrolledText *>(element);
  }
  return text;
}

/* --------------------------------------------------------------------------- */
/* getHtmlWindow --                                                             */
/* --------------------------------------------------------------------------- */
GuiScrolledText *GuiQtFactory::getHtmlWindow() {
  GuiElement *element = GuiElement::findElement( "HTML_WINDOW" );
  GuiQtScrolledText *text;
  if( element == 0 ){
    text = new GuiQtScrolledText( 0, "HTML_WINDOW" );
    text->showMenu( false );
    text->setMaxLines( AppData::Instance().MaxLines() );
    text->createDataReference( "HTML_WINDOW@" );
  }
  else{
    assert( element->Type() == GuiElement::type_ScrolledText );
    text = static_cast<GuiQtScrolledText *>(element);
  }
  return text;
}

/* --------------------------------------------------------------------------- */
/* installDialogsWaitCursor --                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtFactory::installDialogsWaitCursor(GuiDialog* diag_modal) {
  GuiQtDialog::installDialogsWaitCursor( diag_modal );
}

/* --------------------------------------------------------------------------- */
/* removeDialogsWaitCursor --                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtFactory::removeDialogsWaitCursor(GuiDialog* diag_modal) {
  GuiQtDialog::removeDialogsWaitCursor( diag_modal );
}

/* --------------------------------------------------------------------------- */
/* getDialogProgressBar --                                                     */
/* --------------------------------------------------------------------------- */

DialogProgressBar *GuiQtFactory::getDialogProgressBar() {
  return &QtDialogProgressBar::Instance();
}

/* --------------------------------------------------------------------------- */
/* createProgressBar --                                                        */
/* --------------------------------------------------------------------------- */
GuiElement *GuiQtFactory::createProgressBar( GuiElement *parent, const std::string &name ){
  GuiElement* e = new GuiQtProgressBar( parent, name );
  parent -> attach( e );
  return e;
}

/* --------------------------------------------------------------------------- */
/* createServerSocket --                                                       */
/* --------------------------------------------------------------------------- */
ServerSocket *GuiQtFactory::createServerSocket( IntensServerSocket *server, int port ){
  return new QtServerSocket( server, port );
}

/* --------------------------------------------------------------------------- */
/* createClientSocket --                                                       */
/* --------------------------------------------------------------------------- */

ClientSocket *GuiQtFactory::createClientSocket(	const std::string &header
						, Stream *out_stream
						, Stream *in_stream ){
  return new QtClientSocket( header
			     , out_stream
			     , in_stream );
}

/* --------------------------------------------------------------------------- */
/* doCopy --                                                                   */
/* --------------------------------------------------------------------------- */

bool GuiQtFactory::doCopy(Stream *out_stream, GuiElement* elem){
  QClipboard *clipboard  = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();

  std::ostringstream ostr;
  if( out_stream ){
    out_stream->write( ostr );
    clipboard->setText( QString::fromStdString(ostr.str()) );
    clipboard->setText( QString::fromStdString(ostr.str()), QClipboard::Selection );
  } else if (elem) {
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    dynamic_cast<GuiQtElement*>(elem)->writeFile(&buffer, "", true);
    QImage image(QImage::fromData(byteArray));
    clipboard->setImage(image, QClipboard::Clipboard);
  }
  //std::cout << "fb["<<clipboard->supportsFindBuffer()<<"] ss["<<clipboard->supportsSelection()<<"]  CLIPboard Copy: [" << clipboard->text().toStdString()<<"]"<<std::endl<< std::flush;
  // std::cout << "  html["<<clipboard->mimeData()->hasHtml()<<"] text["<<clipboard->mimeData()->hasText()<<"] url["<<clipboard->mimeData()->hasUrls()<<"] color["<<clipboard->mimeData()->hasColor()<<"] formats["<<clipboard->mimeData()->formats().join("=").toStdString()<<"]"<<std::endl<< std::flush;
  return ostr.str().size() ? true : false;
}

/* --------------------------------------------------------------------------- */
/* doPaste --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtFactory::doPaste(Stream *in_stream){
  // webapi query
  if (HEADLESS_CHECK()) {
    return s_headlessGuiFactory->doPaste(in_stream);
  }

  QClipboard *clipboard  = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();

  //std::cout << "  Paste delim["<<in_stream->getDelimiter()<<"] CLIPboard: [" << clipboard->mimeData()->text().toStdString()<<"]"<<std::endl<< std::flush;
  BUG_DEBUG("Paste delim["<<in_stream->getDelimiter()<<"] CLIPboard: [" << clipboard->mimeData()->text().toStdString()<<"]");

  if (mimeData->hasText()) {
    if( in_stream ){
      if( clipboard->text().size() ){
        std::istringstream is( clipboard->text().toStdString() +"\n");
        in_stream->clearRange( ); // no longer done in slot_connected()
        in_stream->read( is );
      }
    }
    return true;
  }

  BUG_DEBUG("Clipboard: html["<<clipboard->mimeData()->hasHtml());
  BUG_DEBUG("Clipboard: text["<<clipboard->mimeData()->hasText());
  BUG_DEBUG("Clipboard: url["<<clipboard->mimeData()->hasUrls());
  BUG_DEBUG("Clipboard: color["<<clipboard->mimeData()->hasColor());
  return clipboard->mimeData()->hasHtml();
}

char GuiQtFactory::getDelimiter() {
  if (GuiQtManager::Settings()) {
    std::string sep_char=GuiQtManager::Settings()->value
      ( "Intens/Delimiter", "\t").toString().toStdString();
    if (sep_char.size() == 1)
      return sep_char[0];
  }
  return '\t';
}

bool GuiQtFactory::replace( GuiElement *old_el, GuiElement *new_el ) {
  BUG( BugGui, "GuiQtManager::replace");
  if (old_el == 0 || new_el == 0) {
    BUG_EXIT("Element(s) are null ");
    return false;
  }

  // if cloned element, it is complicate
  if (old_el->cloneable()) {
    // create clone of new element if it created already (used somewhere else)
    if (new_el->getQtElement()->myWidget()) {
      new_el = new_el->clone();
    }

    std::vector<GuiElement*> cloneListOld;
    std::vector<GuiElement*> cloneListNew;
    old_el->getCloneList(cloneListOld);
    new_el->getCloneList(cloneListNew);

    // replace base element
    if (!old_el->getQtElement()->myWidget()) {
      BUG_MSG("create FORM from old base old base Element");
      if (old_el->myParent( GuiElement::type_Form ))
        old_el->myParent( GuiElement::type_Form )->create();
    }
    GuiQtManager::Instance().replace( old_el, new_el );

    // replace clones
    std::vector<GuiElement*>::iterator it =  cloneListOld.begin();
    std::vector<GuiElement*>::iterator itNew =  cloneListNew.begin();
    for (; it != cloneListOld.end(); ++it) {
      if (!(*it)->getQtElement()->myWidget()) {
        BUG_MSG("create new FORM");
        (*it)->myParent( GuiElement::type_Form )->create();
      }
      if ((*it)->getQtElement()) {
        GuiQtManager::Instance().replace((*it), itNew == cloneListNew.end() ? new_el->clone() : (*itNew));
      }
      if (itNew != cloneListNew.end())
        ++itNew;
    }

    return true;
  } else
    return GuiQtManager::Instance().replace( old_el, new_el );
}
