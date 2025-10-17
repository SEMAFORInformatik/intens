
#if !defined(GUI_QTSCROLLEDLIST_INCLUDED_H)
#define GUI_QTSCROLLEDLIST_INCLUDED_H

#include "gui/qt/GuiQtElement.h"
#include "gui/GuiScrolledlist.h"

class QWidget;
class QtTableView;
class QModelIndex;

class GuiQtScrolledlist : public GuiQtElement, public GuiScrolledlist
{

Q_OBJECT

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtScrolledlist( GuiElement *parent, GuiScrolledlistListener *listener )
    : GuiQtElement(parent), GuiScrolledlist(listener)
    , m_myList( 0 )
    , m_visibleItems( 10 )
    , m_keep( false )
    , m_multiple( false ){
#if defined(GUI_STANDALONE_TEST)
    m_helptext = "GuiQtScrolledlist";
#endif
  }

  virtual ~GuiQtScrolledlist(){
  }

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Scrolledlist; }
  virtual void create();
  virtual bool destroy();
  virtual void manage() {}
  virtual void enable();
  virtual void disable();
  virtual void getSize( int &x, int &y, bool hint=true);
  virtual void update( UpdateReason ) {}
  virtual void setTabOrder();
  virtual void addTabGroup();
  virtual void grabFocus();
  virtual QWidget *myWidget();
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng) {
    return GuiScrolledlist::variantMethod(method, jsonArgs, eng);
  }

/*=============================================================================*/
/* public member functions of GuiScrolledlist                                  */
/*=============================================================================*/
  virtual GuiElement *getElement() { return this; }
/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setTitle(const std::string &title) { GuiScrolledlist::setTitle(title); }
  void clear();
  int createSelectionList( bool sorted=false );
  void deleteSelectionList();
  int getSelectedIndex();
  void setSelectedPosition( int pos );
  void editData(int row, int column);
  void commitData(const QModelIndex& index);
  void setEditable(bool flag);

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  bool createSelectList( bool );
  bool createUnsortedList();
  bool createSortedList();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
  private slots:
  void slot_doubleClicked(const QModelIndex& index);

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
public:
  QtTableView             *m_myList;

  int                       m_visibleItems;
  bool	                    m_keep;
  bool	                    m_multiple;
};

#endif
