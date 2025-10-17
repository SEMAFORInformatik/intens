
#if !defined(GUI_INDEXMENU_INCLUDED_H)
#define GUI_INDEXMENU_INCLUDED_H

#include "gui/GuiElement.h"
#include "gui/GuiDataField.h"
#include "gui/GuiTableActionFunctor.h"
#include "gui/GuiMenuButtonListener.h"

class GuiPopupMenu;
class GuiLabel;
class GuiIndexLabel;

class GuiIndexMenu
{
  friend class GuiIndexLabel;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  enum HideFlag
  {	HideNone = 0
  , HideMenu = 1
  , HideLabel = 3
  };
  GuiIndexMenu(HideFlag hideFlag )
    : m_menu( 0 )
    , m_menu_title( 0 )
    , m_index_offset( 0 )
    , m_index( 0 )
    , m_min_index( 0 )
    , m_orientation( GuiElement::orient_Horizontal )
    , m_insert( this )
    , m_delete( this )
    , m_duplicate( this )
    , m_clear( this )
    , m_pack( this )
    , m_hideFlag( hideFlag )
    , m_webApiAttrChanged(false)
  {}
  virtual ~GuiIndexMenu(){
  }

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_IndexMenu; }
  virtual void setIndex( const std::string &, int );
  virtual GuiElement * getElement() = 0;


  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setOffset( int offset ) { m_index_offset = offset; }

  /** Diese Funktion setzt den Wert, welcher beim für den Index 0 gezeigt
      werden soll. Er wird nur für die Anzeige benötigt.
  */
  void setMinIndex( int minindex ) { m_min_index = minindex; }
  void setOrientation( GuiElement::Orientation orient ) { m_orientation = orient; }
  void popupMenu();
  /** only to be used for listener and similar clients */
  void insertData();
  void deleteData();
  void duplicateData();
  void clearData();
  void packData();

  void setWebApiAttrChanged(bool flag) { m_webApiAttrChanged = flag; }
  virtual bool isWebApiAttrChanged() { return m_webApiAttrChanged; }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  /* virtual void createPopupMenu(); */

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual void createPopupMenu();

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  class InsertListener : public GuiMenuButtonListener
  {
  public:
    InsertListener( GuiIndexMenu *menu ): m_menu( menu ) { assert( menu != 0 ); }
    virtual ~InsertListener() {}
    virtual void ButtonPressed(){ m_menu->insertData(); }
    JobAction* getAction(){ return 0; }
  private:
    GuiIndexMenu  *m_menu;
  };

  class InsertFunctor : public GuiTableActionFunctor
  {
  public:
    InsertFunctor( const std::string &name, int inx )
      : GuiTableActionFunctor( JobElement::cll_Insert, inx )
      , m_name( name ), m_inx( inx ) {}
    virtual ~InsertFunctor() {}
    virtual GuiTableActionFunctor *clone()
      {	return new InsertFunctor( m_name, m_inx ); }
    virtual bool operator() ( GuiDataField &field )
      {	return field.doInsertData( "", m_name, m_inx ); }
  private:
    std::string   m_name;
    int      m_inx;
  };

  class DeleteListener : public GuiMenuButtonListener
  {
  public:
    DeleteListener( GuiIndexMenu *menu ): m_menu( menu ) { assert( menu != 0 ); }
    virtual ~DeleteListener() {}
    virtual void ButtonPressed(){ m_menu->deleteData(); }
    JobAction* getAction(){ return 0; }
  private:
    GuiIndexMenu  *m_menu;
  };

  class DeleteFunctor : public GuiTableActionFunctor
  {
  public:
    DeleteFunctor( const std::string &name, int inx )
      : GuiTableActionFunctor( JobElement::cll_Delete, inx )
      , m_name( name ), m_inx( inx ) {}
    virtual ~DeleteFunctor() {}
    virtual GuiTableActionFunctor *clone()
      { return new DeleteFunctor( m_name, m_inx ); }
    virtual bool operator() ( GuiDataField &field )
      { return field.doDeleteData( "", m_name, m_inx ); }
  private:
    std::string   m_name;
    int      m_inx;
  };

  class DuplicateListener : public GuiMenuButtonListener
  {
  public:
    DuplicateListener( GuiIndexMenu *menu ): m_menu( menu ) { assert( menu != 0 ); }
    virtual ~DuplicateListener() {}
    virtual void ButtonPressed(){ m_menu->duplicateData(); }
    JobAction* getAction(){ return 0; }
  private:
    GuiIndexMenu  *m_menu;
  };

  class DuplicateFunctor : public GuiTableActionFunctor
  {
  public:
    DuplicateFunctor( const std::string &name, int inx )
      : GuiTableActionFunctor( JobElement::cll_Dupl, inx )
      , m_name( name ), m_inx( inx ) {}
    virtual ~DuplicateFunctor() {}
    virtual GuiTableActionFunctor *clone()
      { return new DuplicateFunctor( m_name, m_inx ); }
    virtual bool operator() ( GuiDataField &field )
      { return field.doDuplicateData( "", m_name, m_inx ); }
  private:
    std::string   m_name;
    int      m_inx;
  };

  class ClearListener : public GuiMenuButtonListener
  {
  public:
    ClearListener( GuiIndexMenu *menu ): m_menu( menu ) { assert( menu != 0 ); }
    virtual ~ClearListener() {}
    virtual void ButtonPressed(){ m_menu->clearData(); }
    JobAction* getAction(){ return 0; }
  private:
    GuiIndexMenu  *m_menu;
  };

  class ClearFunctor : public GuiTableActionFunctor
  {
  public:
    ClearFunctor( const std::string &name, int inx )
      : GuiTableActionFunctor( JobElement::cll_Clear, inx )
      , m_name( name ), m_inx( inx ) {}
    virtual ~ClearFunctor() {}
    virtual GuiTableActionFunctor *clone()
      { return new ClearFunctor( m_name, m_inx ); }
    virtual bool operator() ( GuiDataField &field )
      { return field.doClearData( "", m_name, m_inx ); }
  private:
    std::string   m_name;
    int      m_inx;
  };

  class PackListener : public GuiMenuButtonListener
  {
  public:
    PackListener( GuiIndexMenu *menu ): m_menu( menu ) { assert( menu != 0 );}
    virtual ~PackListener() {}
    virtual void ButtonPressed(){ m_menu->packData(); }
    JobAction* getAction(){ return 0; }
  private:
    GuiIndexMenu  *m_menu;
  };

  class PackFunctor : public GuiTableActionFunctor
  {
  public:
    PackFunctor( const std::string &name, int inx )
      : GuiTableActionFunctor( JobElement::cll_Pack, inx )
      , m_name( name ), m_inx( inx ) {}
    virtual ~PackFunctor() {}
    virtual GuiTableActionFunctor *clone()
      { return new PackFunctor( m_name, m_inx ); }
    virtual bool operator() ( GuiDataField &field )
      { return field.doPackData( "", m_name, m_inx ); }
  private:
    std::string   m_name;
    int      m_inx;
  };

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
protected:
  GuiPopupMenu       *m_menu;
  GuiLabel           *m_menu_title;
  int                 m_index;
  int                 m_min_index;
private:
  int                 m_index_offset;
  GuiElement::Orientation m_orientation;
  bool                m_webApiAttrChanged;

  InsertListener      m_insert;
  DeleteListener      m_delete;
  DuplicateListener   m_duplicate;
  ClearListener       m_clear;
  PackListener        m_pack;

 protected:
  HideFlag            m_hideFlag;
};

#endif
