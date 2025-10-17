
#if !defined(MY_QTABLE_INCLUDED_H)
#define MY_QTABLE_INCLUDED_H

#include <QTableView>
#include <QStandardItemModel>
#include <QComboBox>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <map>

#include "gui/GuiMenuButtonListener.h"
#include "gui/DialogWorkClock.h"
#include "job/JobAction.h"
#include "job/JobFunction.h"
#include "job/JobStarter.h"

class QDataWidgetMapper;
class GuiQtList;
class GuiQtTable;
class GuiPopupMenu;
class GuiTableItem;
class XferDataItem;
class GuiFileJobAction;

/*=============================================================================*/
/* private class Column                                                        */
/*=============================================================================*/
class  GuiQtTableViewBase : public  QTableView, public DialogWorkClockListener {
  Q_OBJECT
 public:
  GuiQtTableViewBase( QAbstractItemModel * model,
		      GuiQtList* l, GuiQtTable* t,
		      QHeaderView *horzHeader=NULL, QHeaderView *vertHeader=NULL);
  virtual ~GuiQtTableViewBase();
 private:
  GuiQtTableViewBase( const GuiQtTableViewBase& );

 public:
/*=============================================================================*/
/* public member functions of DialogWorkClockListener                          */
/*=============================================================================*/
  virtual void cancelButtonPressed();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
  /** override keyPressEvent*/
  virtual void keyPressEvent ( QKeyEvent * e );
  /** override timerEvent*/
  virtual void timerEvent ( QTimerEvent * e );
  /** override leaveEvent*/
  virtual void leaveEvent(QEvent * event);

  /** override selectAll */
  virtual void selectAll();

  /** override selectionChanged */
  virtual void selectionChanged( const QItemSelection & selected,
				 const QItemSelection & deselected );
  /** override selectedIndexes */
  virtual QModelIndexList selectedIndexes() const;
  /** override scrollTo */
  virtual void scrollTo(const QModelIndex &index, ScrollHint);
  /** override sizeHintForRow */
  virtual int sizeHintForRow(int row) const;
  /** override sizeHint */
  virtual QSize sizeHint() const;
  /** override minimumSizeHint */
  virtual QSize minimumSizeHint() const;
  /** override focusNextChild */
  virtual bool focusNextChild() { return QTableView::focusNextChild(); }

  void contextMenuEvent(QContextMenuEvent *event);
void paintEvent ( QPaintEvent * event );

 /** facility method: blocked_selectRow */
  void blocked_selectRow ( int row );
  void blocked_selectRows ( std::vector<int> rows );
  /** facility method: scroll_blocked_selectRow */
  void scroll_blocked_selectRow ( int row );
  void scroll_blocked_selectRows ( std::vector<int> rows );

  /** method: setSizeHint */
  void setSizeHint (int w, int h);
  void setScrollBarHint(int w, int h);

  /** copy to clipboard */
  void copy(bool all);

  /** paste from clipboard */
  void paste();

  /** clear selected */
  void clearSelected();

  /** clear all */
  void clearAll();

  /** get Popup Menu*/
  GuiPopupMenu* getMenu();

  /** set section height */
  void setSectionHeight(int height);
  /** set frozen font */
  void setFont ( const QFont & );
  /** set header frozen font */
  void setHeaderFont ( const QFont & );
  /** set frozen itemDelegate */
  void setItemDelegate ( QAbstractItemDelegate * delegate );
  /**  */
  void currentChanged(QModelIndex const&, QModelIndex const&);

public slots:
  void updateSectionWidth(int logicalIndex, int, int newSize);
  void updateSectionHeight(int logicalIndex, int, int newSize);
  void editingFinished();
  void comboxBoxActivate(const QString&);
  void buttonChecked(bool checked);
  void lineEditReturnPressed();
 public:
  /** post initialize for frozen parts */
  void postInit();
  /** set all row height (incl. frozen parts)*/
  void setAllRowHeight(int row, int height);
  /** update for frozen parts */
  void updateFrozenParts();
  /** is inside frozen parts */
  bool isFrozenTablePart(const QModelIndex & index);
  /** getPressedButton */
  const QModelIndex& getPressedButton()   { return m_pressedButton; }
  void nextEditableIndex(int& r, int& c, bool vert);
 private:
  /** initialize for frozen parts */
  void init();
  void getBounds(long& maxValidRow, long& maxValidColumn);

  /** update frozen table geometry */
  void updateFrozenTableGeometry();
  void resizeEvent(QResizeEvent * event);
  QModelIndex moveCursor(CursorAction cursorAction,
			 Qt::KeyboardModifiers modifiers);

  class CopyListener : public GuiMenuButtonListener
  {
  public:
    CopyListener( GuiQtTableViewBase *table, bool alle )
      : m_table( table ), m_all( alle ) {}
    virtual ~CopyListener() {}
    virtual void ButtonPressed(){ m_table->copy( m_all ); }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQtTableViewBase *m_table;
    bool                m_all;
  };

  class PasteListener : public GuiMenuButtonListener
  {
  public:
    PasteListener( GuiQtTableViewBase *table, bool alle=true )
      : m_table( table ), m_all( alle ) {}
    virtual ~PasteListener() {}
    virtual void ButtonPressed(){ m_table->paste(); }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQtTableViewBase *m_table;
    bool                m_all;
  };

  class ClearListener : public GuiMenuButtonListener
  {
  public:
    ClearListener( GuiQtTableViewBase *table, bool alle )
      : m_table( table ), m_all( alle ) {}
    virtual ~ClearListener() {}
    virtual void ButtonPressed();
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQtTableViewBase *m_table;
    bool                m_all;
  };

  class ResetSortingListener : public GuiMenuButtonListener
  {
  public:
    ResetSortingListener( GuiQtTableViewBase *table )
      : m_table( table ) {}
    virtual ~ResetSortingListener() {}
    virtual void ButtonPressed();
    virtual JobAction* getAction() { return 0; }
  private:
    GuiQtTableViewBase *m_table;
    bool                m_all;
  };

  struct PasteData {
    PasteData(int r, int c, std::string val)
      : col(c), row(r), newValue(val) {}
    int col, row;
    std::string newValue;
  };
  class MyTimerTask : public TimerTask {
  public:
    MyTimerTask( GuiQtTableViewBase* t )
      : m_tableView( t ){}
    void tick() { m_tableView->workPaste();  }
  private:
    GuiQtTableViewBase* m_tableView;
  };

  //==============
  // Trigger
  //==============
  class Trigger : public JobStarter
  {
  public:
    Trigger( GuiQtTableViewBase *tbl, JobFunction *f )
      : JobStarter( f )
      , m_table( tbl ){
    }
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult jobResult ) {}
  private:
    GuiQtTableViewBase *m_table;
  };

  bool edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event );
  void workPaste();
  void resetSorting();

private:
  GuiQtList* m_list;
  GuiQtTable* m_table;
  bool       m_blockSelection;
  bool       m_blockScrollTo;
  Timer             *m_timer;
  TimerTask         *m_task;
  std::vector<PasteData> m_pasteDataList;
  DialogWorkClock   *m_dialog;
  QModelIndex m_pressedButton;
  QModelIndex m_nextEditable;
  QModelIndex m_lastIndexWidget;

  QSize m_hs;
  int m_horz_sb_height, m_vert_sb_width;

  GuiPopupMenu       *m_menu;
  GuiPopupMenu       *m_menu_readonly;  // disabled table needs shorter menu
  CopyListener        m_copyAll;
  CopyListener        m_copyAll_readonly;
  CopyListener        m_copy;
  CopyListener        m_copy_readonly;
  PasteListener       m_paste;
  ClearListener    m_clearSelected;
  ClearListener    m_clearAll;
  ResetSortingListener m_resetSorting;
  QTableView *m_vertFrozenTableView;
  QTableView *m_horzFrozenTableView;
  QTableView *m_vertHorzFrozenTableView;
  int         m_timerId;

 public:
  static const int COLUMN_MARGIN_WIDTH;

  friend class SpreadSheetDelegate;
  friend class GuiQtTable;
};

class SpreadSheetDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
    SpreadSheetDelegate(GuiQtTable* table, QObject *parent = 0);
    SpreadSheetDelegate(GuiQtList* list, int countDecimalPlace, QObject *parent = 0);
    static QWidget* createEditorWidget(GuiQtTable *table, GuiTableItem *item, bool activate,
                                      const QModelIndex& index, QWidget* parent, const QObject* obj);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
			  const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem& option, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const;
    /** set own formatted double values in qt4 */
    QString displayText(const QVariant &value, const QLocale &locale) const;
    /** set own formatted double values in qt5 */
    void 	initStyleOption(QStyleOptionViewItem * option, const QModelIndex & index) const;
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    bool editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index );
    bool eventFilter(QObject *editor, QEvent *event);
private slots:
    void comboxBoxActivate(const QString&);
    void lineEditReturnPressed();
    void buttonChecked(bool checked);
private:
  class MyQLineEdit : public QLineEdit {
  public:
 MyQLineEdit(QWidget* p, GuiQtTableViewBase* table)
   : QLineEdit(p), m_table(table) {}
  void keyPressEvent(QKeyEvent* e);
 private:
  GuiQtTableViewBase* m_table;
 };
 class MyQComboBox : public QComboBox {
 public:
 MyQComboBox(QWidget* p, GuiQtTableViewBase* table)
   : QComboBox(p), m_table(table) {}
   void keyPressEvent(QKeyEvent* e);
 private:
   GuiQtTableViewBase* m_table;
 };
private:
  GuiQtTable* m_table;
  GuiQtList* m_list;
  int        m_countDecimalPlace;
  bool        m_mouseButtonPressed;
};

class  QtStandardItemModel : public QStandardItemModel {
public:
  QtStandardItemModel(QObject* parent = 0 );
  QtStandardItemModel(int rows, int columns, QObject* parent = 0 );
  void beginResetModel();
  void endResetModel();
  void signalDataChanged(const QModelIndex&, const QModelIndex&);

  virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
  virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
  QModelIndex parent( const QModelIndex &child) const;
  Qt::ItemFlags flags ( const QModelIndex & index ) const;
private:
  GuiQtTable* m_table;
  GuiQtList*  m_list;
};

class QtHeaderView : public QHeaderView {
public:
  QtHeaderView ( GuiQtTable* tbl, Qt::Orientation orientation, QWidget * parent = 0 );
  QtHeaderView ( GuiQtList* list, Qt::Orientation orientation, QWidget * parent = 0 );

private:
  void contextMenuEvent ( QContextMenuEvent * e );
  void paintSection ( QPainter * painter, const QRect & rect, int logicalIndex ) const;

private:
  GuiQtTable* m_table;
  GuiQtList*  m_list;
};

#endif
