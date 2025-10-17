
#include <qapplication.h>

#include "streamer/Stream.h"
#include "app/DataPoolIntens.h"
#include "utils/gettext.h"

#include "gui/GuiElement.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtForm.h"
#include "gui/qt/GuiQtButton.h"
#include "gui/qt/GuiQtButtonbar.h"
#include "gui/qt/GuiQtMessageLine.h"
#include "gui/qt/GuiQtFieldgroup.h"
#include "gui/qt/GuiQtFieldgroupLine.h"
#include "gui/qt/GuiQtFieldgroup.h"
#include "gui/qt/GuiQtDataField.h"
#include "gui/qt/GuiQtLabel.h"
#include "job/InitialWorker.h"
#include "job/JobManager.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataString.h"
#include "gui/GuiFactory.h"

#include "gui/qt/GuiQtScrolledlist.h"
#include "gui/qt/GuiQtTableViewBase.h"


#include "gui/qt/GuiQtCycleDialog.h"
#include "gui/GuiCycleDialog.h"

INIT_LOGGER();
INIT_DESLOGGER();
INIT_PYTHONLOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtCycleDialog::GuiQtCycleDialog()
  : m_dialog( 0 )
  , m_actcycle( 0 )
  , m_counter( 0 )
  , m_list( 0 )
  , m_okButtonListener( this )
  , m_newButtonListener( this )
  , m_clearButtonListener( this )
  , m_deleteButtonListener( this )
  , m_cancelButtonListener( this )
  , m_inputBox( 0 )
{
}

GuiQtCycleDialog::~GuiQtCycleDialog() {
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtCycleDialog::ButtonPressed() {
  openDialog();
}

/* --------------------------------------------------------------------------- */
/* getNextItem --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQtCycleDialog::getNextItem( std::string &label ) {
  if( m_counter < m_cyclelist.size() ) {
    label = m_cyclelist[m_counter];
    m_counter++;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* activated --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtCycleDialog::activated( int index ) {
  openInputBox( index );
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtCycleDialog::setValue(int index, const std::string& value) {
  std::string name(value);
  int _cycle(index);

  if( name.empty() ){
    std::ostringstream s;
    s << "<case #";
    if( _cycle >= DataPoolIntens::getDataPool().NumCycles() ){
      s << DataPoolIntens::getDataPool().NumCycles()+1 << ">";
    }
    else{
      s << _cycle+1 << ">";
    }
    name = s.str();
  }

  bool newCycle(false);
  int oldCycleNum(-1);
  std::string oldCycleName;
  if( _cycle >= DataPoolIntens::getDataPool().NumCycles() ){
    oldCycleNum = DataPoolIntens::Instance().currentCycle();
    DataPoolIntens::Instance().newCycle( name );
    newCycle = true;
    PYLOG_INFO(compose(PYLOG_NEWCYCLE, name));
    DES_INFO(compose("  NEWCYCLE(\"%1\");", name));
  }
  else{
    DataPoolIntens::Instance().getCycleName( _cycle, oldCycleName );
    if (oldCycleName == name) return; // nothing to do
    DataPoolIntens::Instance().setCycleName( _cycle, name );
  }

  ///  if( this != 0 ) this->updateList( _cycle );
  GuiQtManager::Instance().update( GuiElement::reason_Cycle );
  // m_dialog->unmanage();

  // Wenn ein neuer Cycle kreiert wurde, auch ON_CYCLE_SWITCH aufrufen
  JobAction *action = JobManager::Instance().getInitialAction( "ON_CYCLE_SWITCH" );
  if( action != 0 ){
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
    InitialWorker::Instance().startJobAction();
  }

  // new ON_CYCLE_EVENT
  action = JobManager::Instance().getInitialAction( "ON_CYCLE_EVENT" );
  if( action != 0 ){
    BUG_INFO("setValue(" << index << ", " << value << ") newCycle: " << newCycle <<  " oldCycleNum: " << oldCycleNum << ", oldName: " << oldCycleName);
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
    if (newCycle) {
      InitialWorker::Instance().startJobAction(JobElement::cll_CycleNew, new JobStackDataInteger(oldCycleNum+1));
    } else {
      InitialWorker::Instance().startJobAction(JobElement::cll_CycleRename, new JobStackDataString(oldCycleName));
    }
  }
}

/* --------------------------------------------------------------------------- */
/* openInputBox --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtCycleDialog::openInputBox( int cycle ) {
  updateList(cycle);
}

/* --------------------------------------------------------------------------- */
/* openDialog --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtCycleDialog::openDialog() {
  if( m_dialog == 0 ){
    buildDialog();
  }
  updateList( DataPoolIntens::Instance().currentCycle()-1 );
  m_dialog->manage();
}

/* --------------------------------------------------------------------------- */
/* buildDialog --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtCycleDialog::buildDialog() {
  assert( m_dialog == 0 );

  // Form generieren
  m_dialog = new GuiQtForm(GuiElement::getMainForm()->getElement(), false );
  m_dialog->setTitle( _("CaseDialog") );
  m_dialog->resetCycleButton();
  m_dialog->hasCloseButton( false );
  m_dialog->setApplicationModal();

  // Listenfeld generieren
  m_list = new GuiQtScrolledlist( m_dialog, this );
  m_list->setTitle( _("Cases") );
  m_dialog->attach( m_list );

  // MessageLine generieren
  m_dialog->attach( new GuiQtMessageLine( m_dialog ) );

  // Buttonbar generieren
  GuiQtButtonbar *bar = new GuiQtButtonbar( m_dialog );
  m_dialog->attach( bar );
  GuiButton *button;

  // Button 'Ok' generieren und an das Buttonbar anfuegen
  button = GuiFactory::Instance() -> createButton( bar, &m_okButtonListener );
  button->setLabel( _("OK") );

  // Button 'New' generieren und an das Buttonbar anfuegen
  button = GuiFactory::Instance() -> createButton( bar, &m_newButtonListener );
  button->setLabel( _("New") );

  // Button 'Clear' generieren und an das Buttonbar anfuegen
  button = GuiFactory::Instance() -> createButton( bar, &m_clearButtonListener );
  button->setLabel( _("Clear") );

  // Button 'Delete' generieren und an das Buttonbar anfuegen
  button = GuiFactory::Instance() -> createButton( bar, &m_deleteButtonListener );
  button->setLabel( _("Delete") );

  // Button 'Cancel' generieren und an das Buttonbar anfuegen
  button = GuiFactory::Instance() -> createButton( bar, &m_cancelButtonListener );
  button->setLabel( _("Close") );

  m_dialog->create();
  if (m_dialog->myWidget()->pos() == QPoint(0,0))
    m_dialog->myWidget()->move( QApplication::activeWindow()->cursor().pos() );
}

/* --------------------------------------------------------------------------- */
/* okEvent --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtCycleDialog::okEvent() {
  int selId =  m_list->getSelectedIndex();
  if (selId < 0) {
    m_dialog->printMessage( "Please select a case", GuiElement::msg_Warning, 0 );
    return;
  }
  assert( m_dialog != 0 );
  m_dialog->clearMessage();
  m_dialog->unmanage();

  DataPoolIntens &dp = DataPoolIntens::Instance();
  DataPool &datapool = DataPoolIntens::getDataPool();

  int oldCycleNum = DataPoolIntens::Instance().currentCycle();
  if( oldCycleNum == selId || !dp.goCycle( selId ) ){
    return;
  }

  GuiQtManager::Instance().update( GuiElement::reason_Cycle );

  JobAction *action = JobManager::Instance().getInitialAction( "ON_CYCLE_SWITCH" );
  if( action != 0 ){
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
    InitialWorker::Instance().startJobAction();
  }

  // new ON_CYCLE_EVENT
  action = JobManager::Instance().getInitialAction( "ON_CYCLE_EVENT" );
  if( action != 0 ){
    BUG_DEBUG("okEvent() old: " << oldCycleNum << ", new: " << selId);
    action->setReason(JobElement::cll_CycleSwitch);
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
    InitialWorker::Instance().startJobAction(JobElement::cll_CycleSwitch, new JobStackDataInteger(oldCycleNum+1));
  }
}

/* --------------------------------------------------------------------------- */
/* newEvent --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtCycleDialog::newEvent() {
  m_dialog->clearMessage();
  int nCycle = DataPoolIntens::getDataPool().NumCycles();
  openInputBox(nCycle);
  m_list->editData(nCycle, 1);
}

/* --------------------------------------------------------------------------- */
/* clearEvent --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtCycleDialog::clearEvent() {
  assert( m_list != 0 );
  int selId =  m_list->getSelectedIndex();
  if (selId < 0) {
    m_dialog->printMessage( "Please select a case", GuiElement::msg_Warning, 0 );
    return;
  }

  // goto selected cycle
  DataPoolIntens &dp = DataPoolIntens::Instance();
  if( selId != dp.currentCycle() && !dp.goCycle( selId ) ){
    return;
  }

  m_dialog->clearMessage();
  DataPoolIntens::Instance().clearCycle( selId );
  GuiQtManager::Instance().update( GuiElement::reason_Cycle );

  JobAction *action = JobManager::Instance().getInitialAction( "INIT" );
  if( action != 0 ){
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
  }
  if (action != 0)
    InitialWorker::Instance().startJobAction();

  JobAction *actionCE = JobManager::Instance().getInitialAction( "ON_CYCLE_EVENT" );
  if( actionCE != 0 ){
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( actionCE );
    InitialWorker::Instance().startJobAction(JobElement::cll_CycleClear);
  }
}

/* --------------------------------------------------------------------------- */
/* deleteEvent --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtCycleDialog::deleteEvent() {
  assert( m_list != 0 );
  m_dialog->clearMessage();

  DataPoolIntens &dp = DataPoolIntens::Instance();
  if( dp.numCycles() < 2 ){
    m_dialog->printMessage( "one case has to remain", GuiElement::msg_Warning, 0 );
    return;
  }

  int pos =  m_list->getSelectedIndex();
  if (pos < 0) {
    m_dialog->printMessage( "Please select a case", GuiElement::msg_Warning, 0 );
    return;
  }
  int oldCycleNum = DataPoolIntens::Instance().currentCycle();
  bool switched = DataPoolIntens::Instance().removeCycle( pos );
  updateList( pos - 1 );
  GuiQtManager::Instance().update( GuiElement::reason_Cycle );

  // new ON_CYCLE_EVENT
  JobAction* action = JobManager::Instance().getInitialAction( "ON_CYCLE_EVENT" );
  if( action != 0 ){
    BUG_DEBUG("deleteEvent() cycle: " << pos);
    action->setReason(JobElement::cll_CycleDelete);
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
    InitialWorker::Instance().startJobAction(JobElement::cll_CycleDelete, new JobStackDataInteger(pos+1));
  }

  if( !switched ){
    return;
  }

  action = JobManager::Instance().getInitialAction( "ON_CYCLE_SWITCH" );
  if( action != 0 ){
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
    InitialWorker::Instance().startJobAction();
  }

}

/* --------------------------------------------------------------------------- */
/* cancelEvent --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtCycleDialog::cancelEvent() {
  m_dialog->clearMessage();
  m_dialog->unmanage();
}

/* --------------------------------------------------------------------------- */
/* updateList --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtCycleDialog::updateList( int select ) {
  assert( m_list != 0 );

  m_cyclelist.clear();
  m_list->clear();
  m_counter = 0;

  DataPoolIntens &dp = DataPoolIntens::Instance();
  int numcyc = dp.getDataPool().NumCycles();

  std::string name;
  for( int cyc=0; cyc < std::max(numcyc, select+1); cyc++ ) {
    std::ostringstream ostr;
    if( dp.getCycleName( cyc, name ) ){
      if( !name.empty() ){
        ostr << name;
      }
    } else {
      ostr << "<case #" << cyc+1 << ">";
    }
    m_cyclelist.push_back( ostr.str() );
  }
  m_list->createSelectionList();
  m_list->setSelectedPosition( select );
}

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtCycleInputBox::GuiQtCycleInputBox( GuiQtCycleDialog* cycdialog )
  : m_dialog( 0 )
  , m_field( 0 )
  , m_cycle( 0 )
  , m_cycdialog( cycdialog )
  , m_okButtonListener( this )
  , m_cancelButtonListener( this ){

  // Form
  m_dialog = new GuiQtForm( GuiElement::getMainForm()->getElement(), false );
  m_dialog->setTitle( "InputBox" );
  m_dialog->resetCycleButton();
  m_dialog->hasCloseButton( false );
  m_dialog->setApplicationModal();

  // InputField
  GuiFieldgroup *group = new GuiQtFieldgroup( m_dialog, "" );
  m_dialog->attach( group -> getElement() );
  GuiQtFieldgroupLine *line = static_cast<GuiQtFieldgroupLine*>(group->addFieldgroupLine());
  assert( line != 0 );
  GuiLabel *label = GuiFactory::Instance() -> createLabel( line, GuiElement::align_Default );
  label->setLabel( _("Modify comment:") );
  line = static_cast<GuiQtFieldgroupLine*>(group->addFieldgroupLine());
  DataReference *dref = DataPoolIntens::getDataReference( INTERNAL_CYCLE_NAME );
  assert( dref != 0 );
  XferDataItem *dataitem = new XferDataItem();
  dataitem->setDataReference( dref );
  m_field = dynamic_cast<GuiQtDataField*>(GuiFactory::Instance()->createDataField( line, dataitem ));
  m_field->setLength( 30 );
  m_field->GuiElement::setAlignment( GuiElement::align_Left );
  line->attach( m_field );

  // Buttonbar generieren
  GuiQtButtonbar *bar = new GuiQtButtonbar( m_dialog );
  m_dialog->attach( bar );
  GuiButton *button;

  // Button 'Ok' generieren und an das Buttonbar anfuegen
  button = GuiFactory::Instance() -> createButton( bar, &m_okButtonListener );
  button->setLabel( _("OK") );

  // Button 'Cancel' generieren und an das Buttonbar anfuegen
  button = GuiFactory::Instance() -> createButton( bar, &m_cancelButtonListener );
  button->setLabel( _("Cancel") );

  m_dialog->create();
  if (m_dialog->myWidget()->pos() == QPoint(0,0))
    m_dialog->myWidget()->move( QApplication::activeWindow()->cursor().pos() );
}

/* --------------------------------------------------------------------------- */
/* open --                                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtCycleInputBox::open( int cycle ){
  m_cycle = cycle;
  std::string value;
  if( cycle < DataPoolIntens::getDataPool().NumCycles() ){
    DataPoolIntens::Instance().getCycleName( cycle, value );
  }
  m_field->setValue( value );
  m_dialog->manage();
}

/* --------------------------------------------------------------------------- */
/* okEvent --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtCycleInputBox::okEvent(){
  std::string name;
  m_field->getValue( name );

  if( name.empty() ){
    std::ostringstream s;
    s << "<case #";
    if( m_cycle >= DataPoolIntens::getDataPool().NumCycles() ){
      s << DataPoolIntens::getDataPool().NumCycles()+1 << ">";
    }
    else{
      s << m_cycle+1 << ">";
    }
    name = s.str();
  }

  bool newCycle(false);
  int oldCycleNum(-1);
  std::string oldCycleName;
  if( m_cycle >= DataPoolIntens::getDataPool().NumCycles() ){
    oldCycleNum = DataPoolIntens::Instance().currentCycle();
    DataPoolIntens::Instance().newCycle( name );
    newCycle = true;
    PYLOG_INFO(compose(PYLOG_NEWCYCLE, name));
    DES_INFO(compose("  NEWCYCLE(\"%1\");", name));
  }
  else{
    DataPoolIntens::Instance().getCycleName( m_cycle, oldCycleName );
    DataPoolIntens::Instance().setCycleName( m_cycle, name );
  }

  if( m_cycdialog != 0 ) m_cycdialog->updateList( m_cycle );
  GuiQtManager::Instance().update( GuiElement::reason_Cycle );
  m_dialog->unmanage();

  // Wenn ein neuer Cycle kreiert wurde, auch ON_CYCLE_SWITCH aufrufen
  JobAction *action = JobManager::Instance().getInitialAction( "ON_CYCLE_SWITCH" );
  if( action != 0 ){
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
    InitialWorker::Instance().startJobAction();
  }

  // new ON_CYCLE_EVENT
  action = JobManager::Instance().getInitialAction( "ON_CYCLE_EVENT" );
  if( action != 0 ){
    BUG_DEBUG("GuiQtCycleInputBox::okEvent() newCycle: " << newCycle <<  "oldCycleNum" << oldCycleNum);
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
    if (newCycle) {
      InitialWorker::Instance().startJobAction(JobElement::cll_CycleNew, new JobStackDataInteger(oldCycleNum+1));
    } else {
      InitialWorker::Instance().startJobAction(JobElement::cll_CycleRename, new JobStackDataString(oldCycleName));
    }
  }
}

/* --------------------------------------------------------------------------- */
/* cancelEvent --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtCycleInputBox::cancelEvent(){
  m_dialog->unmanage();
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtCycleInputBox::OkListener::ButtonPressed(){
  if( m_dialog != 0 ) m_dialog->okEvent();
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtCycleInputBox::CancelListener::ButtonPressed(){
  if( m_dialog != 0 ) m_dialog->cancelEvent();
}
