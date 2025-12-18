
#if !defined(GUI_QT_LIST_INCLUDED_H)
#define GUI_QT_LIST_INCLUDED_H

#include <vector>
#include <QtCore/QPoint>

#include "gui/GuiList.h"
#include "gui/qt/GuiQtElement.h"
#include "datapool/DataDictionary.h"
#include "xfer/XferDataParameter.h"

class QBoxLayout;
class QVariant;
class QModelIndex;
class QStandardItemModel;
class GuiQtTableViewBase;
class QColor;

class GuiQtList : public GuiQtElement, public GuiList {
 public:
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
  GuiQtList(GuiElement *parent, const std::string &name);
  ~GuiQtList();

private:
  GuiQtList( GuiQtList& );
  GuiQtList( const GuiQtList& );              // nicht implementiert
  GuiQtList &operator=( const GuiQtList &i ); // nicht implementiert

  /*=============================================================================*/
  /* public member functions from GuiList                                        */
  /*=============================================================================*/
 public:
  virtual GuiList *getList(){ return this; }
  virtual int sortOrder();
  virtual bool selectRow(int row, bool recursive=true);
  virtual bool selectRows(std::vector<int> rows, bool recursive=true);
  virtual bool getSelectedRows(std::vector<int>& idxs);
  virtual bool getSortCriteria(std::string& sortColumn);
  virtual void resetSortCriteria();
  virtual void clearSelection();
  virtual void sort(int column, int order);
  virtual Stream* getStream();

  virtual GuiElement *getElement();

  // GuiIndexListener
  virtual bool acceptIndex( const std::string &name, int inx );
  virtual void setIndex( const std::string &name, int inx );

  /** open config dialog **/
  virtual void openConfigDialog( const std::string name = std::string());
  QVariant columnCellValue(int icol, int idx);
  bool columnCellColor(int icol, int idx, QColor& foreground, QColor& background);
  bool columnCellColor(int icol, int idx, std::string& foreground, std::string& background);

/*=============================================================================*/
/* public member functions of GuiQtElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return type_List; }
  virtual void create();
  virtual void manage();
  virtual bool destroy();
  virtual void update( UpdateReason );
  virtual QWidget* myWidget();
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone();
  virtual void getCloneList(std::vector<GuiElement*>& cList) const;
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  /** JSF Corba:: get all visible Datapool Values */
  virtual void getVisibleDataPoolValues( GuiValueList& vmap );
  virtual void writeSettings();
  void readSettings();
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng) {
    return GuiList::variantMethod(method, jsonArgs, eng);
  }

/*=============================================================================*/
/* public method                                                               */
/*=============================================================================*/
public:
  void popup();

/*=============================================================================*/
/* private class ConfigMenuListener                                            */
/*=============================================================================*/
  class ConfigMenuListener : public GuiMenuButtonListener
  {
  public:
    ConfigMenuListener( GuiList *list )
      : m_list( list ) {}
    virtual ~ConfigMenuListener() {}
    virtual void ButtonPressed(){ m_list->openConfigDialog(); }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiList *m_list;
  };

/*=============================================================================*/
/* private method                                                              */
/*=============================================================================*/
 private:
  void setLabels();
  void insertItemList();
  void installPopupMenu();
  void sortColumn(int column);
  void updateItemUnits();

/*=============================================================================*/
/* private slots                                                               */
/*=============================================================================*/
  Q_OBJECT
 private slots:
  void rowActivate( const QModelIndex& index );
 public slots:
  void rowSelect( const QModelIndex& index );
  void rowUnselect( const QModelIndex& index );
/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  QWidget               *m_frame;
  GuiQtTableViewBase    *m_tablewidget;
  ConfigMenuListener     m_configMenu;

  std::vector<GuiQtList*> m_clonedList;  // cloned (children) or parent
  std::vector<int>     m_selectedIdxs;
  int                  m_sort_column;
  std::string          m_sort_criteria;
  bool                 m_sort_block;
};

#endif
