
#ifndef GUI_QT_NAVIGATOR_H
#define GUI_QT_NAVIGATOR_H

#include <qobject.h>
#include <QTreeWidget>
#include <QStackedWidget>
#include<QDragEnterEvent>

#include "gui/qt/GuiQtElement.h"
#include "gui/GuiNavigator.h"
#include "gui/GuiToggleListener.h"
#include "gui/GuiMenuToggle.h"

class QPixmap;
class GuiQtListView;
class GuiQtDiagram;
class GuiQtDiagramScene;
class GuiQtPopup;
class GuiQtPopupMenu;

class GuiQtNavigator : public GuiQtElement, public GuiNavigator{
  Q_OBJECT
class MyQTreeWidget;
public:
  GuiQtNavigator( const std::string & name, GuiNavigator::Type navType );
  virtual ~GuiQtNavigator();

public slots:
  void setExpanded( QTreeWidgetItem *);
  void setCollapsed( QTreeWidgetItem *);
  bool menuRequested( const QPoint & pos=QPoint() );
private slots:
  void activated( QTreeWidgetItem*, int col );
  void selected();
  bool headerMenuRequested( const QPoint & pos=QPoint() );
  void slot_destroyedAction(QObject * obj);

public:
  virtual GuiElement *getElement(){ return this; }

private:
  class NavigatorView : public QStackedWidget{
  public:

    NavigatorView( GuiQtNavigator *nav, QWidget *parent );
    virtual ~NavigatorView(){}

    virtual void startDrag(Qt::DropActions supportedActions);
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dragLeaveEvent(QDragLeaveEvent* event);
    virtual void dragMoveEvent(QDragMoveEvent* event);
    virtual void dropEvent(QDropEvent* event);
    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void contextMenuEvent ( QContextMenuEvent * event );
    virtual void resizeEvent ( QResizeEvent * event );
    virtual QSize sizeHint() const;
    bool event ( QEvent * event );
    void initShortcuts();

    // addition interface methods
    QWidget* getContentWidget();
    void clearSelection();
    void setItemSelection(const std::string& id, bool bSelected=true);
    void getItemsSelected(std::vector<std::string>& varnameList);
    QTreeWidgetItem* takeTopLevelItem(int index);
    void addTopLevelItem(QTreeWidgetItem* tl);
    void insertTopLevelItem(int rootIdx, QTreeWidgetItem* tl);
    void setRootIsDecorated ( bool show );
    void setSortingEnabled( bool show );
    void setDropIndicatorShown( bool show );
    void setSelectionMode( QAbstractItemView::SelectionMode mode );

    void setColumnCount(int c);
    QTreeWidgetItem* headerItem() const;
    QTreeWidgetItem* currentItem() const;
    QTreeWidgetItem* itemAt(const QPoint& p, bool checkHeader=false) const;
    QList<QTreeWidgetItem *> selectedItems() const;
    QHeaderView* header () const;
    virtual void setMinimumSize(int w, int h);
    void setSizePolicy ( QSizePolicy pol );
    void resetLastDragMoveSelectItem() { m_lastDragMoveSelectItem = 0; }
  private:
    void dragMoveSelectItem(QTreeWidgetItem *item);
    void getItemClassName(std::vector<std::string>& itemClassNames, const QMimeData* mimeData);

    GuiQtNavigator*     m_parent;
    MyQTreeWidget*      m_treeWidget;
    GuiQtListView*      m_listView;
    GuiQtDiagram*       m_diagramView;
    GuiQtDiagramScene*  m_diagramScene;
    QTreeWidgetItem*    m_lastDragMoveSelectItem;
  };

  class MyQTreeWidget : public QTreeWidget{
  public:
  MyQTreeWidget(NavigatorView *parent)
    : QTreeWidget(parent), m_navView(parent)  {}
    virtual void keyPressEvent ( QKeyEvent * event ) {
      QTreeWidget::keyPressEvent(event);
      // setSizePolicy( QSizePolicy(  QSizePolicy::Fixed, QSizePolicy::MinimumExpanding)  );
    }

    virtual void startDrag(Qt::DropActions supportedActions) {
      m_navView->startDrag(supportedActions);
    }
    virtual void contextMenuEvent ( QContextMenuEvent * event ) {
      m_navView->contextMenuEvent(event);
    }
    NavigatorView* m_navView;
  };

  //----------
  // CompareModeMenu
  //----------
  class CompareModeMenu : public GuiToggleListener {
  public:
    class MyEvent : public ::GuiEventData{
    public:
      MyEvent( eOutputCompareMode modeType ) : m_modeType( modeType ){}
      eOutputCompareMode m_modeType;
    };
  CompareModeMenu( GuiNavigator *nav )
      : m_nav( nav )
      , m_equal( 0 )
      , m_inequal( 0 )
      , m_all( 0 ) {}
    virtual void ToggleStatusChanged( GuiEventData *event );
    void setMenuButtons( eOutputCompareMode style );
    GuiNavigator     *m_nav;
    GuiMenuToggle *m_equal;
    GuiMenuToggle *m_inequal;
    GuiMenuToggle *m_all;
  private:
    CompareModeMenu( const CompareModeMenu &s );
    void operator=(const CompareModeMenu &s );
    /* eStyle m_currentStyle; */
  };

  // ---------------------------------------------------------
  // member functions of GuiNavigator
  // ---------------------------------------------------------
public:
  virtual void doEndOfWork( bool error );
  virtual GuiNavElement *getCurrentNode();
  virtual void freeze(){}
  virtual GuiQtNavigator *getQtNavigator(){ return this; }
  NavigatorView *getNavView(){ return m_navView; }
  virtual void clearSelection(){
    if( m_navView )
      m_navView->clearSelection();
  }
  virtual void setItemSelection(const std::string& id, bool bSelected=true) {
    if( m_navView )
      m_navView->setItemSelection(id, bSelected);
  }
  virtual void getItemsSelected(std::vector<std::string>& varnameList) {
    if( m_navView )
      m_navView->getItemsSelected(varnameList);
  }
  virtual void getDiagramConnections(std::vector<std::string>& vecA,
                                     std::vector<std::string>& vecB,
                                     std::vector<ConnectionAttr>& attr);
  virtual void getDiagramSelectItems(std::vector<std::string>& select_items);
  virtual double getDiagramConnectionRatio();
  virtual void addColumnsExtension(int numColumns);
protected:
  virtual void repaint();
  virtual void addActions();
  virtual void createTree( Root * const root, int rootIdx );

  // ---------------------------------------------------------
  // member functions of GuiElement
  // ---------------------------------------------------------
 public:
  virtual ElementType Type();
  virtual void create();
  virtual void manage();
  virtual void getSize( int &width, int &height );
  virtual void update( UpdateReason ur);
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);
  virtual QWidget *myWidget();
  virtual void grabShortcutAction( std::string& accelerator_key, QAction * );

  virtual void serializeXML(std::ostream &os, bool recursive = false){
    GuiNavigator::serializeXML(os, recursive);
  }
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false) {
    return GuiNavigator::serializeJson(jsonObj, onlyUpdated);
  }
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false) {
    return GuiNavigator::serializeProtobuf(eles, onlyUpdated);
  }
#endif
  virtual GuiNavigator *getNavigator(){ return this; }
  /** JSF Corba:: get all visible Datapool Values */
  virtual void getVisibleDataPoolValues( GuiValueList& vmap );

  // GuiIndexListener
  virtual bool acceptIndex( const std::string &name, int inx );
  virtual void setIndex( const std::string &name, int inx );

  /// File save functions
  virtual void writeFile( const std::string &filename, bool bPrinter );
  virtual void writeFile(QIODevice* ioDevice, const std::string &filename, bool bPrinter ) {
    writeFile(filename, bPrinter);
  }

  /** show drop menu */
  bool dropMenuRequested( QTreeWidgetItem *item, const QPoint & pos, std::string& data );
  // public members
public:
  //  QPixmap *getFolderClosed(){ return m_folderClosed; }
  //  QPixmap *getFolderOpen(){ return m_folderOpen; }
private:
  void installPopupMenu();
  void createWebTree( XferDataItem *xfer, bool root, GuiValueList& vmap );
  void callGrabShortcutAction(const QKeySequence& key_seq);
  /** generate a unique key of the tree item
      @return a unique key
  */
  std::string getTreeItemKey(QTreeWidgetItem *item);
  GuiPopupMenu* getHeaderMenu();

  // ---------------------------------------------------------
  // private members
  // ---------------------------------------------------------
private:
  NavigatorView *m_navView;
  QPixmap *m_folderClosed;
  QPixmap *m_folderOpen;
  QPixmap *m_file;
  std::map<int, QAction*> m_grabActions;
  std::vector<QString> m_collapseItems;
  QPoint   m_menuRequestedPosition;

  GuiQtPopupMenu* m_headerMenu;
  CompareModeMenu m_compareModeMenu;

};

#endif
