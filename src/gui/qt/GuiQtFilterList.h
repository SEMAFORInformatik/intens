
#if !defined(GUI_QT_FILTER_LIST_INCLUDED_H)
#define GUI_QT_FILTER_LIST_INCLUDED_H

#include "gui/GuiFilterList.h"
#include "gui/GuiFilterListListener.h"
#include "gui/qt/GuiQtElement.h"

class QWidget;
class QTableView;
class QModelIndex;
class QLabel;

/**
 * Mit der Klasse GuiFilterList koennen Daten in einer Liste dargestellt
 * und ausgewaehlt werden.
 * @short Ein Listenobjekt
 * @author Daniel Kessler
 */

class GuiQtFilterList : public GuiQtElement, public GuiFilterList
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtFilterList( GuiElement *parent, GuiFilterListListener &listener, const std::string &name );
  virtual ~GuiQtFilterList(){}
 private:
  GuiQtFilterList( const GuiQtFilterList & );
  GuiQtFilterList &operator=( const GuiQtFilterList & );
  void timerEvent(QTimerEvent* );

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return type_List; }
  virtual void create();
  virtual void enable();
  virtual void disable();
  virtual void manage();
  virtual bool destroy();
  virtual void update( UpdateReason ){}
  virtual QWidget* myWidget();
  virtual GuiElement* getElement() { return this; }
  virtual void serializeXML(std::ostream&, bool recursive = false);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual int getSelection();
  virtual int rebuild();
  virtual void clear();
/*   void addRows(const int &rows); */
  /** Fragt die Konfiguration beim Listener ab.
   */
  //  void getListConf();

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
 protected:
/*   virtual void rowActivate( int row = (-1) ); */
/*   virtual void selected( int row ); */
/*   virtual void unselected( int row ); */
/*   virtual void dblClick( int row ){ rowActivate( row ); } */

/*=============================================================================*/
/* private slots                                                               */
/*=============================================================================*/
 private:
  Q_OBJECT
 private slots:
  void rowClicked(const QModelIndex& index);

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void doListConfig();
  void setTitle();
  void setLabels();
  int fillList(int selectedRowIndex = -1);

 /*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  QWidget                          *m_frameWidget;
  QTableView                       *m_listWidget;
  QLabel                           *m_titleWidget;
  GuiFilterListListener::RowsVector m_rows;
};

#endif
