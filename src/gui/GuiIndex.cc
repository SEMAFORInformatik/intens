
#include "utils/Debugger.h"

#include "utils/gettext.h"
#include "app/DataPoolIntens.h"
#include "job/JobFunction.h"
#include "gui/GuiElement.h"
#include "gui/GuiIndex.h"
#include "gui/GuiFieldgroup.h"

INIT_LOGGER();

GuiIndex::GuiIndexContainer     GuiIndex::s_index_container;

/*=============================================================================*/
/* local class definitions                                                     */
/*=============================================================================*/

class IndexAction : public JobAction
{
public:
  IndexAction( GuiIndex *index, int inx )
    : m_index( index )
      , m_value( inx ){
    setSilentFunction();
  }
  ~IndexAction() {}
public:
  virtual void printLogTitle( std::ostream &ostr ){}
  virtual void serializeXML(std::ostream &os, bool recursive = false){}
  virtual void startJobAction();
  virtual void stopJobAction() {}

protected:
  virtual void backFromJobController( JobResult rslt );

private:
  GuiIndex   *m_index;
  int         m_value;
};

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void IndexAction::startJobAction(){
  BUG_DEBUG("IndexAction::startJobAction: Value=" << m_value );

  m_index->setNewIndex( m_value );
  JobFunction *function = 0; // m_index->getFunction();
  // Wir wollen hier bis auf Weiteres keine eigene Funktion starten bei einem
  // Aufruf aus einer anderen Task oder Function. (20.4.201/bh)
  if( function == 0 ){
    m_index->setNewIndex();
    endJobAction( job_Ok );
    delete this;
    BUG_DEBUG( "no function started" );
    return;
  }

  if( function->isAlreadyInUse() ){
    BUG_DEBUG( "function is already running" );
    // Die Sache ist schlecht gelaufen. Aus unerfindlichen Gründen ist die
    // JobFunction bereits in Betrieb.
    endJobAction( job_Aborted );
    delete this;
    return;
  }
  function->setIndex( m_value );
  function->setReason( JobElement::cll_Select );
  BUG_DEBUG( "start function" );
  startNextJobAction( function );
}

/* --------------------------------------------------------------------------- */
/* backFromJobController --                                                    */
/* --------------------------------------------------------------------------- */

void IndexAction::backFromJobController( JobAction::JobResult rslt ){
  BUG(BugGuiFld,"IndexAction::backFromJobController");

  if( rslt == JobAction::job_Ok ){
    m_index->setNewIndex();
  }
  endJobAction( rslt );
  delete this;
}

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiIndex::GuiIndex( GuiElement *parent, const std::string &name )
  : m_function( 0 )
  , m_disabled( false )
  , m_orientation( GuiElement::orient_Horizontal )
  , m_min_index( 0 )
  , m_max_index( -1 )
  , m_step( 1 )
  , m_index( 0 )
  , m_cloned( false )
  , m_param( 0, 0, 0 )
  , m_new_index( 0 )
  , m_activeFunc(false)
  , m_name( name )
{
}

GuiIndex::GuiIndex( const GuiIndex &index )
  : m_function( index.m_function )
  , m_disabled( index.m_disabled )
  , m_orientation( index.m_orientation )
  , m_min_index(index.m_min_index)
  , m_max_index(index.m_max_index)
  , m_step(index.m_step)
  , m_index( index.m_index )
  , m_cloned( true )
  , m_param( 0, 0, 0 )
  , m_new_index( 0 )
  , m_activeFunc(false)
  , m_name(index.m_name)
{
}

/* --------------------------------------------------------------------------- */
/* getIndexAction --                                                           */
/* --------------------------------------------------------------------------- */

JobAction *GuiIndex::getIndexAction( int inx ){
  BUG_DEBUG("getIndexAction: inx=" << inx );
  return new IndexAction( this, inx );
}

/* --------------------------------------------------------------------------- */
/* getIndexAction --                                                           */
/* --------------------------------------------------------------------------- */

JobAction *GuiIndex::getIndexAction(){
  BUG_DEBUG("getIndexAction");
  return new IndexAction( this, m_index );
}

/* --------------------------------------------------------------------------- */
/* setStep --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiIndex::setStep( int step ){
  m_step = step > 0 ? step : 1;
}

/* --------------------------------------------------------------------------- */
/* clearIndexContainer --                                                      */
/* --------------------------------------------------------------------------- */

void GuiIndex::clearIndexContainer(){
  s_index_container.clear();
}

/* --------------------------------------------------------------------------- */
/* addIndexContainer --                                                        */
/* --------------------------------------------------------------------------- */

void GuiIndex::addIndexContainer( const std::string &name, GuiIndex *index ){
  if( s_index_container.find( name ) == s_index_container.end() ){
    s_index_container.insert( GuiIndexContainer::value_type( name, index ) );
  }
}

/* --------------------------------------------------------------------------- */
/* registerIndexContainer --                                                   */
/* --------------------------------------------------------------------------- */

void GuiIndex::registerIndexContainer( GuiIndexListener *l ){
  GuiIndexContainer::iterator it;
  for( it = s_index_container.begin(); it != s_index_container.end(); ++it ){
    (*it).second->registerIndexedElement( l );
  }
}

/* --------------------------------------------------------------------------- */
/* unregisterIndexContainer --                                                 */
/* --------------------------------------------------------------------------- */

void GuiIndex::unregisterIndexContainer( GuiIndexListener *l ){
  GuiIndexContainer::iterator it;
  for( it = s_index_container.begin(); it != s_index_container.end(); ++it ){
    (*it).second->unregisterIndexedElement( l );
  }
}

/* --------------------------------------------------------------------------- */
/* startFunction --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiIndex::startFunction(){
  BUG_DEBUG("startFunction");

  if( getFunction() == 0 ){
    BUG_DEBUG("No Function attached");
    return false;
  }
  BUG_DEBUG( "setValue " << m_new_index );
  m_param.DataItem()->setValue( m_new_index );

  ( new Trigger( this ) )->startNextJobAction( m_function );
  // wir werden mit backFromJobController wieder aufgerufen
  m_activeFunc = true;
  return true;
}

/* --------------------------------------------------------------------------- */
/* backFromJobConroller --                                                     */
/* --------------------------------------------------------------------------- */

void GuiIndex::backFromJobController( JobAction::JobResult rslt ){
  BUG_DEBUG("backFromJobController");

  bool update = true;
  doEndOfWork( rslt != JobAction::job_Ok, update );
}

/* --------------------------------------------------------------------------- */
/* doEndOfWork --                                                              */
/* --------------------------------------------------------------------------- */

void GuiIndex::doEndOfWork( bool error, bool update ){
  BUG_DEBUG("doEndOfWork");
  m_activeFunc = false;
  if( error ){
    getElement()->updateForms( GuiElement::reason_Cancel );
    getElement()->printMessage( _("Error in function execution."), GuiElement::msg_Information );
    updateMyself();
    return;
  }

  if( update ){
    getElement()->updateForms( GuiElement::reason_FieldInput );
  }

  if( !m_cloned ){
    setNewIndex();
    m_index = m_new_index;
    updateMyself();
    getElement()->clearMessage();
  }
}

/* --------------------------------------------------------------------------- */
/* backFromJobController --                                                    */
/* --------------------------------------------------------------------------- */

void GuiIndex::Trigger::backFromJobController( JobAction::JobResult rslt ){
  BUG_DEBUG("Trigger::backFromJobController");
  m_index->backFromJobController( rslt );
  delete this;
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiIndex::isValid(){
  return m_param.DataItem()->isValid();
}

/* --------------------------------------------------------------------------- */
/* setDataValue --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiIndex::setDataValue( int index ){
  BUG_DEBUG("setDataValue: index=" << index );

  if( getIndex() == index ){
    BUG_DEBUG( "Value has not changed" );
    return false;
  }

  m_param.DataItem()->setValue( index );
  BUG_DEBUG( "New value set" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiIndex::acceptIndex( const std::string &name, int index ){
  BUG_DEBUG("acceptIndex: Name = " << name << ", Index = " << index);

  if( m_cloned ){
    // The Master has called me.
    BUG_DEBUG("ok. The cloned Index accepts always");
    return true;
  }
  if( name != GuiIndex::getName() ){
    BUG_DEBUG( "ok. its not for me" );
    return true;
  }
  GuiIndexListenerList::iterator lsnr;
  for( lsnr = m_listeners.begin(); lsnr != m_listeners.end(); ++lsnr ){
    if( !(*lsnr)->acceptIndex( GuiIndex::getName(), index ) ){
      BUG_DEBUG("not accepted => Exit");
      return false;
    }
  }
  BUG_DEBUG( "accepted => exit" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiIndex::setIndex( const std::string &name, int index ){
  BUG_DEBUG("setIndex: Name = " << name << ", Index = " << index);

  if( name != GuiIndex::getName() ){
    BUG_DEBUG( "Name doesnt match -> EXIT" );
    return;
  }
  m_index = index;

  updateMyself();

  if( m_cloned ){
    // Ein Clone kann nur vom Master-Index aufgerufen werden, da ihn sonst
    // keiner kennt. Er muss nur sich selber auf den neusten Stand bringen.
    BUG_DEBUG( "Clone" );
    return;
  }

  if( getIndex() == m_index ){
    BUG_DEBUG( "Value has not changed" );
    //    return;
  }
  else {
    setDataValue( m_index );
  }

  // Achtung: Eine Aenderung des Index wird forsiert. Vermutlich ein Clone.
  GuiIndexListenerList::iterator lsnr;
  for( lsnr = m_listeners.begin(); lsnr != m_listeners.end(); ++lsnr ){
    (*lsnr)->setIndex( GuiIndex::getName(), m_index );
  }
  BUG_DEBUG( "Index " << index << " set" );
}

/* --------------------------------------------------------------------------- */
/* isIndexActivated --                                                         */
/* --------------------------------------------------------------------------- */

bool GuiIndex::isIndexActivated(){
  if( m_cloned ){
    return false;
  }
  if( m_listeners.size() == 0 ){
    return false;
  }
  GuiIndexListenerList::iterator lsnr;
  for( lsnr = m_listeners.begin(); lsnr != m_listeners.end(); ++lsnr ){
    if( (*lsnr)->isIndexActivated() ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getVisibleDataPoolValues --                                                 */
/* --------------------------------------------------------------------------- */

void GuiIndex::getVisibleDataPoolValues( GuiValueList& vmap ) {
  std::ostringstream os;
  os << getIndex();
  vmap.insert( GuiValueList::value_type(m_param.DataItem()->getFullName(false), os.str()) );
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiIndex::serializeXML(std::ostream &os, bool recursive){
  os<<"<intens:Index";
  os << " gui=\"qt\"";
  os << " name=\"" << getName() << "\"";
  os << ">";
  os<<"</intens:Index>"<<std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiIndex::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
///TODO amg2020-10-12  if(!m_param.DataItem()->isUpdated( GuiManager::Instance().LastUpdate(), true))
///    return false;
  if (getElement()){
    getElement()->update( GuiElement::reason_Always );
  }
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["fullName"] = m_param.getFullName();
  jsonObj["disabled"] = m_disabled;
  jsonObj["min"] = m_min_index;
  jsonObj["max"] = m_max_index;
  jsonObj["step"] = m_step;
  jsonObj["index"] = m_index + m_min_index;
  jsonObj["new_index"] = m_new_index;
  jsonObj["orientation"] = m_orientation == GuiElement::orient_Vertical ? "vertical" : "horizontal";
  if (getElement()->getParent()->Type() == GuiElement::type_Fieldgroup) {
    GuiFieldgroup* fg = getElement()->getParent()->getFieldgroup();
    jsonObj["visible"] = fg->getTableSize() > 0 && fg->getTableMax() == fg->getTableMin() ? false : true;
  }

  // optional action
  JobFunction *func = getFunction();
  if( func != 0 ){
    jsonObj["action"] = func->Name();
  }

  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiIndex::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  if (getElement()){
    getElement()->update( GuiElement::reason_Always );
  }
  auto element = eles->add_indexs();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_full_name(m_param.getFullName());
  element->set_enabled(!m_disabled);
  auto range = element->mutable_range();
  range->set_min(m_min_index);
  range->set_max(m_max_index);
  range->set_step(m_step);
  element->set_index(m_index + m_min_index);
  element->set_new_index(m_new_index);
  element->set_orientation(m_orientation == GuiElement::orient_Vertical ?
                            in_proto::GuiElement::Orientation::GuiElement_Orientation_Vertical :
                            in_proto::GuiElement::Orientation::GuiElement_Orientation_Horizontal);
  if (getElement()->getParent()->Type() == GuiElement::type_Fieldgroup) {
    GuiFieldgroup* fg = getElement()->getParent()->getFieldgroup();
    element->set_visible(!(fg->getTableSize() > 0 && fg->getTableMax() == fg->getTableMin() ));
  }

  // optional action
  JobFunction *func = getFunction();
  if( func != 0 ){
    element->set_action(func->Name());
  }

  return true;
}
#endif
/* --------------------------------------------------------------------------- */
/* isIndexAccepted --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiIndex::isIndexAccepted( int index ){
  BUG_DEBUG("isIndexAccepted: index=" << index );

  GuiIndexListenerList::iterator lsnr;
  for( lsnr = m_listeners.begin(); lsnr != m_listeners.end(); ++lsnr ){
    if( !(*lsnr)->acceptIndex( GuiIndex::getName(), index ) ){
      BUG_DEBUG( "index " << index << " is not accepted" );
      return false;
    }
  }
  BUG_DEBUG( "index " << index << " is accepted" );
  m_new_index = index;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setNewIndex --                                                              */
/* --------------------------------------------------------------------------- */

void GuiIndex::setNewIndex( int index ){
  BUG_DEBUG("setNewIndex: Index old: " << index << " new: " << m_new_index );
  m_new_index = index;
  setDataValue( index );
}

/* --------------------------------------------------------------------------- */
/* setNewIndex --                                                              */
/* --------------------------------------------------------------------------- */

void GuiIndex::setNewIndex(){
  BUG_DEBUG("Begin of setNewIndex: Call SetIndex(" <<
            GuiIndex::getName() << "," << m_new_index <<
            ")" );

  GuiIndexListenerList::iterator lsnr;
  for( lsnr = m_listeners.begin(); lsnr != m_listeners.end(); ++lsnr ){
    BUG_DEBUG(" -- call setIndex()");
    (*lsnr)->setIndex( GuiIndex::getName(), m_new_index );
    BUG_DEBUG(" -- setIndex() done");

  }

  if( !m_cloned ){
    // Ein Clone braucht das nicht zu tun. Er wird ueber setIndex vom Master
    // wieder aufgerufen.
    BUG_DEBUG("setNewIndex: CALL updateMyself()");
    BUG_DEBUG(" -- Index Old: " << m_index << ", New: " << m_new_index);
    m_index = m_new_index;
    updateMyself();
    BUG_DEBUG("setNewIndex: updateMyself() done");
  }
  BUG_DEBUG("End of setNewIndex");
}
