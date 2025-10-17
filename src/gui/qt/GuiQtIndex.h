
#if !defined(GUI_QTINDEX_H)
#define GUI_QTINDEX_H

#include <qobject.h>

#include "gui/qt/GuiQtElement.h"
#include "job/JobController.h"
#include "gui/GuiIndex.h"

class JobFunction;
class QWidget;
class QSpinBox;


class GuiQtIndex : public GuiQtElement, public GuiIndex
{

  Q_OBJECT

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtIndex( GuiElement *parent, const std::string &name );
  GuiQtIndex( GuiQtIndex &index );

  virtual ~GuiQtIndex(){}

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Index; }
/*   virtual void create( Widget parent ); */
  virtual void create();
  virtual void manage();
  virtual void enable();
  virtual void disable();
  virtual bool destroy();
  virtual void getSize( int &x, int &y );
  virtual void update( GuiElement::UpdateReason );
/*   virtual Widget myWidget() { return m_formwidget; } */
  virtual QWidget *myWidget();
  virtual void addTabGroup();
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone();
  virtual void serializeXML(std::ostream &os, bool recursive);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual GuiIndex *getGuiIndex() { return this; }
  virtual GuiQtIndex *getQtIndex() { return this; }
  virtual GuiElement *getElement() { return this; }
  /** JSF Corba:: get all visible Datapool Values */
  virtual void getVisibleDataPoolValues( GuiValueList& vmap );

/*=============================================================================*/
/* public member functions of GuiIndexListener                                 */
/*=============================================================================*/
public:
  virtual bool acceptIndex( const std::string &name, int index) {
    return GuiIndex::acceptIndex(name, index);
  }
  virtual void setIndex( const std::string &name, int index) {
    GuiIndex::setIndex(name, index);
  }
  virtual bool isIndexActivated() {
    return GuiIndex::isIndexActivated();
  }
  bool areIndexActivated();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setDataReference( DataReference * );

  virtual void registerIndexedElement( GuiIndexListener * );
  virtual void unregisterIndexedElement( GuiIndexListener * );

  virtual void setMinIndex( int );
  virtual void setMaxIndex( int );
  int getMinIndex();
  int getIndex();
  void setIndex( int );
  JobController *getIndexController( int inx, GuiElement * );
  void getAttributes( int &, int &, int & );
  virtual void setOrientation( GuiElement::Orientation orient ) { m_orientation = orient; }
  virtual int shiftRight();
  virtual void setPlusMinusStyle();

  /* void setNewIndex( int inx ); */
  /* void setNewIndex(); */

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual void updateMyself();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  GuiQtIndex( const GuiQtIndex &index );

  bool setColors();
  bool sendNewIndex( int );

  void indexChanged();
  void LeftArrow();
  void RightArrow();
  /* bool setDataValue( int index ); */

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
  private slots:
    void slot_indexChanged(int);
  public slots:
    void slot_editingFinished();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:

  QSpinBox               *m_mySpinBox;
  /* int                     m_width; */
  /* int                     m_height; */
  int                     m_textlen;
  bool                    m_CB_IndexIsModified;

  std::vector<GuiQtIndex*> m_clonedIndex;
};

#endif
