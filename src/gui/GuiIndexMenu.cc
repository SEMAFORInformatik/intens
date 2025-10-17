
#include <QMouseEvent>
#include <qlabel.h>
#include "gui/GuiFactory.h"
#include "gui/GuiIndexMenu.h"
#include "gui/GuiPopupMenu.h"
#include "gui/GuiLabel.h"
#include "gui/GuiSeparator.h"
#include "gui/GuiMenuButton.h"
#include "utils/gettext.h"
#include "utils/Debugger.h"

/* --------------------------------------------------------------------------- */
/* createPopupMenu --                                                          */
/* --------------------------------------------------------------------------- */
void GuiIndexMenu::createPopupMenu(){
  m_menu = GuiFactory::Instance()->createPopupMenu(this->getElement());
  m_menu->setTitle("Titel");
  m_menu->resetMenuPost();

  // set title label and set popup font
  m_menu_title = GuiFactory::Instance()->createLabel(m_menu->getElement(), GuiElement::align_Left);
  m_menu_title->setLabel(" Index -9999");
  m_menu_title->getElement()->create();

  m_menu->getElement()->attach(GuiFactory::Instance()->createSeparator(m_menu->getElement())->getElement());

  GuiMenuButton *button = 0;
  if( m_orientation == GuiElement::orient_Horizontal ) {
    button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_insert );
    button->setLabel( _("Insert empty column") );
    m_menu->getElement()->attach( button->getElement() );

    button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_delete );
    button->setLabel( _("Delete column") );
    m_menu->getElement()->attach( button->getElement() );

    button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_duplicate );
    button->setLabel( _("Duplicate column") );
    m_menu->getElement()->attach( button->getElement() );

    button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_clear );
    button->setLabel( _("Clear column") );
    m_menu->getElement()->attach( button->getElement() );

    button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_pack );
    button->setLabel( _("Pack all columns") );
    m_menu->getElement()->attach( button->getElement() );
  }
  else{
    button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_insert );
    button->setLabel( _("Insert empty row") );
    m_menu->getElement()->attach( button->getElement() );

    button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_delete );
    button->setLabel( _("Delete row") );
    m_menu->getElement()->attach( button->getElement() );

    button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_duplicate );
    button->setLabel( _("Duplicate row") );
    m_menu->getElement()->attach( button->getElement() );

    button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_clear );
    button->setLabel( _("Clear row") );
    m_menu->getElement()->attach( button->getElement() );

    button = GuiFactory::Instance()->createMenuButton( m_menu->getElement(), &m_pack );
    button->setLabel( _("Pack all rows") );
    m_menu->getElement()->attach( button->getElement() );
  }
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiIndexMenu::setIndex( const std::string &name, int inx ){
  m_index = m_index_offset + inx;
  setWebApiAttrChanged(true);
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* popupMenu --                                                                */
/* --------------------------------------------------------------------------- */
void GuiIndexMenu::popupMenu(){
  if( m_menu == 0 ) return;
  std::ostringstream title;
  title << " Index " << (m_index + m_min_index);
  m_menu_title->setLabel( title.str() );
  m_menu->popup();
}

/* --------------------------------------------------------------------------- */
/* insertData --                                                               */
/* --------------------------------------------------------------------------- */
void GuiIndexMenu::insertData(){
  InsertFunctor functor( "@fg_index@", m_index );
  getElement()->handleTableAction( functor );
}

/* --------------------------------------------------------------------------- */
/* deleteData --                                                               */
/* --------------------------------------------------------------------------- */
void GuiIndexMenu::deleteData(){
  DeleteFunctor functor( "@fg_index@", m_index );
  getElement()->handleTableAction( functor );
}

/* --------------------------------------------------------------------------- */
/* duplicateData --                                                            */
/* --------------------------------------------------------------------------- */
void GuiIndexMenu::duplicateData(){
  DuplicateFunctor functor( "@fg_index@", m_index );
  getElement()->handleTableAction( functor );
}

/* --------------------------------------------------------------------------- */
/* clearData --                                                                */
/* --------------------------------------------------------------------------- */
void GuiIndexMenu::clearData(){
  ClearFunctor functor( "@fg_index@", m_index );
  getElement()->handleTableAction( functor );
}

/* --------------------------------------------------------------------------- */
/* packData --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiIndexMenu::packData(){
  PackFunctor functor( "@fg_index@", m_index );
  getElement()->handleTableAction( functor );
}


/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiIndexMenu::serializeXML(std::ostream &os, bool recursive){
//   os << "<GuiIndexMenu></GuiIndexMenu>"<<endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiIndexMenu::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  if (onlyUpdated && !isWebApiAttrChanged()) return false;
  std::ostringstream number;
  if( m_hideFlag != HideLabel){
    number << (m_index + m_min_index);
  }
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["label"] = number.str();
  jsonObj["menu"] = !(m_hideFlag & HideMenu);
  if( !(m_hideFlag & HideMenu) && m_menu){
    // popup base menu
    Json::Value jsonElem = Json::Value(Json::objectValue);
    m_menu->serializeJson(jsonElem, onlyUpdated);
    jsonObj["popup_menu"] = jsonElem;
    jsonObj["guielement"] = getElement()->getParent()->getName();
  }

  setWebApiAttrChanged(false);  // reset WebApi Update
  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiIndexMenu::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  if (onlyUpdated && !isWebApiAttrChanged()) return false;
  std::ostringstream number;
  if( m_hideFlag != HideLabel){
    number << (m_index + m_min_index);
  }
  auto element = eles->add_index_menus();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_label(number.str());
  element->set_menu(!(m_hideFlag & HideMenu));
  if( !(m_hideFlag & HideMenu) && m_menu){
    // popup base menu
    m_menu->serializeProtobuf(eles, element->mutable_popup_menu(), onlyUpdated);
    element->set_guielement(getElement()->getParent()->getName());
  }

  setWebApiAttrChanged(false);  // reset WebApi Update
  return true;
}
#endif
