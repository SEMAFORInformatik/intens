
#if !defined(GUI_QT_FIELDGROUP_INCLUDED_H)
#define GUI_QT_FIELDGROUP_INCLUDED_H

#include "gui/GuiFieldgroup.h"
#include "gui/qt/GuiQtElement.h"

class QGroupBox;
class QString;
class QGridLayout;
class QWidget;
class QLabel;
class QPushButton;
class MessageQueuePublisher;

class GuiQtFieldgroup : public GuiQtElement, public GuiFieldgroup
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
  GuiElement::ElementType Type() override { return GuiElement::type_Fieldgroup; }

  void create() override;
  void manage() override;
  void unmanage() override;
  void map() override;
  void unmap() override;
  virtual void onClicked();
  void enable() override;
  void disable() override;
  bool destroy() override;
  void update( UpdateReason ) override;
  /** change indicator */
  bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false) override;
  void setFrame( FlagStatus s ) override  { GuiFieldgroup::setFrame(s); }
  bool withFrame() override { return GuiFieldgroup::withFrame(); }
  void setScrollbar( ScrollbarType sb ) override;
  virtual bool withScrollbars()          { return m_container.withScrollbars(); }
  void setUseRuler() override { m_container.setUseRuler(); }
  bool cloneable() override { return true; }
  GuiElement *clone() override;
  void getCloneList(std::vector<GuiElement*>& cList) const override;
  BasicStream *streamableObject() override;

  QWidget* myWidget() override;

  /** Fragt nach der ExpandPolicy des QtElements.
   */
  Qt::Orientations getExpandPolicy() override;
  /** Fragt nach der ExpandPolicy des QtElements fuer den Container.
   */
  GuiElement::Orientation getContainerExpandPolicy() override;
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  GuiElement::Orientation getDialogExpandPolicy() override;

  void serializeXML(std::ostream &os, bool recursive = false) override;
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false) override;
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false) override;
#endif
  void getVisibleElement(GuiElementList& res) override;
  std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng) override {
    return GuiFieldgroup::variantMethod(method, jsonArgs, eng);
  }

/*=============================================================================*/
/* public member functions of GuiFieldgroup                                    */
/*=============================================================================*/
public:
  void setOrientation(GuiElement::Orientation o) override { GuiFieldgroup::setOrientation(o); }
  void setNavigation( GuiElement::Orientation o) override { GuiFieldgroup::setNavigation(o); }
  bool handleTableAction(GuiTableActionFunctor &func) override { return GuiFieldgroup::handleTableAction(func); }
  void nextTableAction(bool error) override { GuiFieldgroup::nextTableAction(error); }
  /** get stretch factor */
  int getStretchFactor( GuiElement::Orientation orient ) override;
  void attach( GuiElement * ) override { assert( false ); }

  void showColumn(int columnIdx, bool visible, int rowOffset=0) override;

  GuiElement * getElement() override;
  GuiFieldgroup * getFieldgroup() override { return this; }
  void serializeContainerElements( std::ostream &os ) override;
#if HAVE_PROTOBUF
  bool serializeContainerElements(in_proto::ElementList* eles, in_proto::FieldGroup* element, bool onlyUpdated = false) override;
#endif
  bool serializeContainerElements(Json::Value& jsonObj, bool onlyUpdated = false) override;
  void setFieldgroupTitle(const std::string &title) override;
  void setAccordionExpanded(bool open) override;
/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void getSize(int &x, int &y);
  void timerEvent (QTimerEvent * event) override;
  void publishData();

/*=============================================================================*/
/* private slots                                                               */
/*=============================================================================*/
  Q_OBJECT
 private slots:
  void slot_accordion(bool checked);

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  QWidget* createContainer( QWidget* parent );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  QWidget              *m_scrollview;
  QWidget              *m_qgroupbox;
  QGridLayout          *m_qgroupboxLayout;
  QLabel               *m_titleLabel;
  QPushButton          *m_accordionButton;
  std::vector<GuiQtFieldgroup*> m_clonedFieldgroup;
  MessageQueuePublisher* m_publisher;
  static int            s_timerId;
};

#endif
