
#if !defined(GUI_QT_FIELDGROUP_INCLUDED_H)
#define GUI_QT_FIELDGROUP_INCLUDED_H

#include "gui/GuiFieldgroup.h"
#include "gui/qt/GuiQtElement.h"

class QGroupBox;
class QString;
class QGridLayout;
class QWidget;

class GuiQtFieldgroup : public GuiFieldgroup, public GuiQtElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtFieldgroup( GuiElement *parent, const std::string &name );
  virtual ~GuiQtFieldgroup();
private:
  GuiQtFieldgroup( const GuiQtFieldgroup &fg );
protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Fieldgroup; }

  virtual void create();
  virtual void manage();
  virtual void unmanage();
  virtual void map();
  virtual void unmap();
  virtual void onClicked();
  virtual void enable();
  virtual void disable();
  virtual bool destroy();
  virtual void update( UpdateReason );
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);
  virtual void setFrame( FlagStatus s )  { GuiFieldgroup::setFrame(s); }
  virtual bool withFrame() { return GuiFieldgroup::withFrame(); }
  virtual void setScrollbar( ScrollbarType sb );
  virtual bool withScrollbars()          { return m_container.withScrollbars(); }
  virtual void setUseRuler() { m_container.setUseRuler(); }
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone();
  virtual void getCloneList(std::vector<GuiElement*>& cList) const;
  virtual BasicStream *streamableObject();

  virtual QWidget* myWidget();

  /** Fragt nach der ExpandPolicy des QtElements.
   */
  virtual Qt::Orientations getExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Container.
   */
  virtual GuiElement::Orientation getContainerExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  virtual GuiElement::Orientation getDialogExpandPolicy();

  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual void getVisibleElement(GuiElementList& res);
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng) {
    return GuiFieldgroup::variantMethod(method, jsonArgs, eng);
  }

/*=============================================================================*/
/* public member functions of GuiFieldgroup                                    */
/*=============================================================================*/
public:
  /* virtual bool setTableSize( int ); */
  /* virtual int  getTableSize(); */
  /* virtual bool setTableStep( int ); */

  /* virtual bool setTablePosition( int ); */
  /* virtual int  getTablePosition(); */
  /* virtual bool setTableIndexRange( int, int ); */
  virtual void setOrientation(GuiElement::Orientation o) { GuiFieldgroup::setOrientation(o); }
  virtual void setNavigation( GuiElement::Orientation o) { GuiFieldgroup::setNavigation(o); }
  virtual bool handleTableAction(GuiTableActionFunctor &func) { return GuiFieldgroup::handleTableAction(func); }
  virtual void nextTableAction(bool error) { GuiFieldgroup::nextTableAction(error); }
  virtual void setTitleAlignment(GuiElement::Alignment align);
  /* virtual bool setMargins( int margin, int spacing); */
  /** get stretch factor */
  virtual int getStretchFactor( GuiElement::Orientation orient );
  //  virtual GuiFieldgroupLine *addFieldgroupLine();
  //  virtual bool addArrowbar();
  virtual void attach( GuiElement * ) { assert( false ); }
  /* virtual const std::string &Name() { return m_name; } */

  virtual void showColumn(int columnIdx, bool visible, int rowOffset=0);

  virtual GuiElement * getElement();
  virtual GuiFieldgroup * getFieldgroup() { return this; }
  virtual void serializeContainerElements( std::ostream &os );
#if HAVE_PROTOBUF
  virtual bool serializeContainerElements(in_proto::ElementList* eles, in_proto::FieldGroup* element, bool onlyUpdated = false);
#endif
  virtual bool serializeContainerElements(Json::Value& jsonObj, bool onlyUpdated = false);
/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void getSize(int &x, int &y);

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  QWidget* createContainer( QWidget* parent );
  //  void createArrowbar();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  QWidget              *m_scrollview;
  QWidget              *m_qgroupbox;
  QGridLayout          *m_qgroupboxLayout;

  std::vector<GuiQtFieldgroup*> m_clonedFieldgroup;
};

#endif
