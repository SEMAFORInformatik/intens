
#if !defined(GUI_TABLE_INCLUDED_H)
#define GUI_TABLE_INCLUDED_H

#if HAVE_PROTOBUF
#include "protobuf/Table.pb.h"
#include <google/protobuf/repeated_field.h>
#endif

#include "gui/GuiElement.h"
#include "gui/GuiDataField.h"
#include "gui/GuiTableActionFunctor.h"
#include "gui/GuiMenuButtonListener.h"
#include "job/JobStarter.h"
#include "job/JobFunction.h"

class GuiTableSize;
class GuiTableLine;
class GuiTableItem;
class GuiLabel;
class Stream;
class DialogWorkClock;

class GuiTable
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  enum TableType
    {   table_xrt = 0
      , table_qt
    };
  GuiTable( GuiElement *parent);
  GuiTable( const GuiTable& tbl );
  virtual ~GuiTable();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  GuiTableLine *addHorizontalLine( const std::string &label, GuiElement::Alignment align );
  GuiTableLine *addVerticalLine( const std::string &label, GuiElement::Alignment align );
  GuiTableLine *addTableLine( const std::string &label, GuiElement::Alignment align );
  GuiTableLine *addTableLineToolTip();
  GuiTableLine *addTableLineColor();
  bool mapTableLine(DataReference *dataref);
  bool unmapTableLine(DataReference *dataref);
  void setTitle( const std::string &title, GuiElement::Alignment align );
  void setTitleHorizontal( const std::string &title );
  void setTitleVertical( const std::string &title );
  bool setHorizontalPlacement( GuiElement::Alignment align );
  bool setVerticalPlacement( GuiElement::Alignment align );
  void setLabelAlignment( GuiElement::Alignment align );
  void setMatrix() { m_table_matrix = true; }
  bool setTable();
  void setTableOrientation( GuiElement::Orientation );
  void setTableNavigation(  GuiElement::Orientation );
  GuiTableSize *newTableSize();
  void setTableRowSize( GuiTableSize *tsz );
  void setTableColSize( GuiTableSize *tsz );
  void nextTableAction( bool error );
  GuiTableItem *getTableItem( int row, int col );
  GuiTableItem *getFilledTableItem( int row, int col, int numRows=-1, int numCols=-1);
  GuiTableItem *getTableItemToolTip( int idx );
  GuiTableItem *getTableItemColor( int idx );

  void syncroniseTableItemLength();
  virtual GuiPopupMenu* PopupMenu(int x, int y, GuiElement::Orientation orientation);
  virtual void hideIndexMenu() { m_hideIndexMenu=true; }
  bool isIndexMenuHidden() { return m_hideIndexMenu; }
  virtual bool isEditable() { return true; }

  /** Only to be used by listeners or similar clients */
  void insertData( bool );
  void duplicateData( bool );
  void deleteData( bool );
  virtual bool getSelectedItems(std::vector<int>& row_idxs, std::vector<int>& column_idxs) = 0;
  virtual bool selectRow(int row, bool recursive=true) = 0;
  virtual bool selectRows(std::vector<int> rows, bool recursive=true) = 0;
  virtual void deleteSelectedData( bool ) {}
  virtual void clearSelection() = 0;
  void clearData( bool );
  void packData( bool );
  void clearAllData();
  void setFunction(JobFunction *func);
  void setAutoWidth() { m_auto_width = true; }
  bool isMultipleSelection() { return false; }

  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
  bool matrixSerializeProtobuf(in_proto::ElementList* eles, google::protobuf::RepeatedPtrField<in_proto::Table::MatrixEntry>* matrix, int numRows, int numCols, bool onlyUpdated = false, int from=0, int to=-1);
#endif
  bool matrixSerializeJson(Json::Value& rowAry, int numRows, int numCols, bool onlyUpdated = false, int from=0, int to=-1);
  /** Diese Funktion ruft eine variante Methode auf.
      Die Argumente werden diese Methode übergeben.
      Das Result wird als Json-String zurückgeben.
  */
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng);

  void getHeaderLabels(std::vector<std::string>& vertLblAry,
		       std::vector<std::string>& horzLblAry);

  /** alternate color function */
  static void alternateColor(std::string& backgroundStr, int row);

/*=============================================================================*/
/* public abtract member functions of GuiTable                                 */
/*=============================================================================*/
public:
  virtual GuiElement* getElement() = 0;
/*   virtual TableType getTableType() = 0; */
  virtual Stream* getStream();
  enum SizeType
    {   size_default = 0
      , size_init = 1
      , size_visible = 2
    };
  void getTableSize( int &, int &, SizeType size_type=size_default);

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:

  void getTableVerticalSize(int &, int &, SizeType size_type=size_default);
  void getTableHorizontalSize(int &, int &, SizeType size_type=size_default);
  bool isTableVerticalFixed();
  bool isTableHorizontalFixed();
  void getTableMatrixSize(int &, int &, SizeType size_type=size_default);
  void resetEmptyRowColumnFlag() { m_emptyRow = false; m_emptyColumn = false;}
  void setEmptyRowFlag() { m_emptyRow = true; }
  void setEmptyColumnFlag() { m_emptyColumn = true; }

/*=============================================================================*/
/* protected Definitions                                                       */
/*=============================================================================*/
protected:
  class TableActionFunctor : public GuiTableActionFunctor
  {
  public:
    TableActionFunctor( JobElement::CallReason reason )
      : GuiTableActionFunctor( reason, -1 )
      , m_inx( 0 ) {}  //wegen der unbelannten IDX von GuiTableActionFunctor
    TableActionFunctor( JobElement::CallReason reason, const std::string &row, const std::string &col, int inx )
      : GuiTableActionFunctor( reason, inx )
      , m_row( row ), m_col( col ), m_inx( inx ){}
    virtual ~TableActionFunctor() {}
    virtual TableActionFunctor *clone( const std::string &row, const std::string &col, int inx ) = 0;
  protected:
    std::string   m_row;
    std::string   m_col;
    int      m_inx;
  };

  class InsertListener : public GuiMenuButtonListener
  {
  public:
    InsertListener( GuiTable *table, bool use_row )
      : m_table( table ), m_use_row( use_row ) {}
    virtual ~InsertListener() {}
    virtual void ButtonPressed() {
      m_table->insertData( m_use_row );
    }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiTable  *m_table;
    bool       m_use_row;
  };

  class InsertFunctor : public GuiTable::TableActionFunctor
  {
  public:
    InsertFunctor()
      : TableActionFunctor( JobElement::cll_Insert ){}
    InsertFunctor( const std::string &row, const std::string &col, int inx )
      : TableActionFunctor( JobElement::cll_Insert, row, col, inx ) {}
    virtual ~InsertFunctor() {}
    TableActionFunctor *clone( const std::string &row, const std::string &col, int inx )
      { return new InsertFunctor( row, col, inx ); }
    virtual bool operator() ( GuiDataField &field )
      { return field.doInsertData( m_row, m_col, m_inx ); }
  };

  class DeleteListener : public GuiMenuButtonListener
  {
  public:
  DeleteListener( GuiTable *table, bool use_row, bool allSelected=false )
    : m_table( table ), m_use_row( use_row ), m_allSelected( allSelected ) {}
    virtual ~DeleteListener() {}
    virtual void ButtonPressed();
    virtual JobAction* getAction() { return 0; }
  private:
    GuiTable  *m_table;
    bool       m_use_row;
    bool       m_allSelected;
  };

  class DeleteFunctor : public GuiTable::TableActionFunctor
  {
  public:
    DeleteFunctor()
      : TableActionFunctor( JobElement::cll_Delete ){}
    DeleteFunctor( const std::string &row, const std::string &col, int inx )
      : TableActionFunctor( JobElement::cll_Delete, row, col, inx ) {}
    virtual ~DeleteFunctor() {}
    TableActionFunctor *clone( const std::string &row, const std::string &col, int inx )
      { return new DeleteFunctor( row, col, inx ); }
    virtual bool operator() ( GuiDataField &field )
      { return field.doDeleteData( m_row, m_col, m_inx ); }
  };

  class DuplicateListener : public GuiMenuButtonListener
  {
  public:
    DuplicateListener( GuiTable *table, bool use_row )
      : m_table( table ), m_use_row( use_row ) {}
    virtual ~DuplicateListener() {}
    virtual void ButtonPressed() {
      m_table->duplicateData( m_use_row );
    }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiTable      *m_table;
    bool           m_use_row;
  };

  class DuplicateFunctor : public GuiTable::TableActionFunctor
  {
  public:
    DuplicateFunctor()
      : TableActionFunctor( JobElement::cll_Dupl ){}
    DuplicateFunctor( const std::string &row, const std::string &col, int inx )
      : TableActionFunctor( JobElement::cll_Dupl, row, col, inx ) {}
    virtual ~DuplicateFunctor() {}
    TableActionFunctor *clone( const std::string &row, const std::string &col, int inx )
      { return new DuplicateFunctor( row, col, inx ); }
    virtual bool operator() ( GuiDataField &field )
      { return field.doDuplicateData( m_row, m_col, m_inx ); }
  };

  class ClearListener : public GuiMenuButtonListener
  {
  public:
    ClearListener( GuiTable *table, bool use_row )
      : m_table( table ), m_use_row( use_row ) {}
    virtual ~ClearListener() {}
    virtual void ButtonPressed() {
      m_table->clearData( m_use_row );
    }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiTable      *m_table;
    bool           m_use_row;
  };

  class ClearFunctor : public GuiTable::TableActionFunctor
  {
  public:
    ClearFunctor()
      : TableActionFunctor( JobElement::cll_Clear ){}
    ClearFunctor( const std::string &row, const std::string &col, int inx )
      : TableActionFunctor( JobElement::cll_Clear, row, col, inx ) {}
    virtual ~ClearFunctor() {}
    TableActionFunctor *clone( const std::string &row, const std::string &col, int inx )
      { return new ClearFunctor( row, col, inx ); }
    virtual bool operator() ( GuiDataField &field )
    { return field.doClearData( m_row, m_col, m_inx ); }
  };

  class PackListener : public GuiMenuButtonListener
  {
  public:
    PackListener( GuiTable *table, bool use_row )
      : m_table( table ), m_use_row( use_row ) {}
    virtual ~PackListener() {}
    virtual void ButtonPressed() {
      m_table->packData( m_use_row );
    }
    virtual JobAction* getAction() { return 0; }
  private:
    GuiTable      *m_table;
    bool           m_use_row;
  };

  class PackFunctor : public GuiTable::TableActionFunctor
  {
  public:
    PackFunctor()
      : TableActionFunctor( JobElement::cll_Pack ){}
    PackFunctor( const std::string &row, const std::string &col, int inx )
      : TableActionFunctor( JobElement::cll_Pack, row, col, inx ) {}
    virtual ~PackFunctor() {}
    TableActionFunctor *clone( const std::string &row, const std::string &col, int inx )
      { return new PackFunctor( row, col, inx ); }
    virtual bool operator() ( GuiDataField &field )
      { return field.doPackData( m_row, m_col, m_inx ); }
  };

  class Trigger : public JobStarter
  {
  public:
    Trigger( GuiTable *tb, JobFunction *f )
      : JobStarter( f )
      , m_table( tb ){}
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult rslt );
  private:
    GuiTable      *m_table;
  };

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  bool isDataItemUpdated( TransactionNumber trans );
  bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);

  void editData( bool use_row, TableActionFunctor &func, bool forAll=false,
		 const std::set<int>& set=std::set<int>() );
  // TOP, BOTTOM
  void getTableActionHorizontal( TableActionFunctor &func
				 , const std::string &row, const std::string &col, int inx );
  // LEFT, RIGHT
  void getTableActionVertical( TableActionFunctor &func
			       , const std::string &row, const std::string &col, int inx );
  // one matrix row (needs m_row, set it using setCurrentRow before)
  void getTableActionWithRow( TableActionFunctor &func
			      , const std::string &row, const std::string &col, int inx );
  // one matrix column (needs m_col, set it using setCurrentColumn before)
  void getTableActionWithColumn( TableActionFunctor &func
				 , const std::string &row, const std::string &col, int inx );
  JobFunction* getFunction() { return m_function; }
  bool hasAutoWidth() { return m_auto_width; }

private:

  virtual void setFieldAttributes() = 0;
  virtual void setFieldPicklists() = 0;

  void finalTableAction( bool ok );
  bool setRunningMode();
  void unsetRunningMode();

  // webApi aux func
  void pushArgRowColumnNumber(int rowColumn);
  int  popArgRowColumnNumber();
  void pushArgSelected(const Json::Value& jsonAry);
  bool popArgSelected(int& x0, int& y0, int& x1, int& y1);
  void pushArgClipboardText(const std::string data);
  std::string popArgClipboardText();

  void getHeaderLabels(Json::Value& vertLblAry,
                       Json::Value& horzLblAry, int& maxVertLen);
  void getHeaderLabels(std::vector<std::string>& vertLblVec,
                       std::vector<std::string>& horzLblVec, int& maxVertLen);

  void removeExtentedData(Json::Value& jsonData);
/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:


/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
public:
  GuiElement::Alignment  getHorzLinePlacement() { return m_hline_placement; }
  GuiElement::Alignment  getVertLinePlacement() { return m_vline_placement; }
  bool getTableMatrix() { return m_table_matrix; }
  GuiElement::Orientation getOrientation() { return m_orientation; }
  GuiElement::Orientation getNavigation() { return m_navigation; }

 protected:
  const std::string&  getTitle()             { return m_title; }
  std::string&        getTitleHorizontal()   { return m_title_horizontal; }
  const std::string&  getTitleVertical()     { return m_title_vertical; }
  GuiElement::Alignment  getTitleAlignment() { return m_title_alignment; }

  int getTableRows()         { return m_table_rows; }
  void setTableRows(int r)   { m_table_rows = r; }
  int getTableRowRangeFrom() { return m_row_range_from; }
  int getTableRowRangeTo()   { return m_row_range_to; }
  int getTableCols()         { return m_table_cols; }
  void setTableCols(int c)   { m_table_cols = c; }
  int getTableColRangeFrom() { return m_col_range_from; }
  int getTableColRangeTo()   { return m_col_range_to; }
  int getHorizontalHeaderLines() { return m_horzHeaderLines; }
  std::vector<int>& getVerticalHeaderLines() { return m_vertHeaderLines; }

  void getRowMenu();
  void getColumnMenu();

  typedef std::vector<GuiTableLine *> GuiTableLineList;
  GuiElement::Alignment LabelAlignment() { return m_label_alignment; }

public:

  GuiTableLineList&   getVLineList() { return  m_vline_list; }
  GuiTableLineList&   getHLineList() { return  m_hline_list; }

  bool isTableRowHeaderHidden() { return m_row_header_hidden; }
  bool isTableColHeaderHidden() { return m_col_header_hidden; }
protected:
  void                setCurrentRow(int r) { m_row=r; };
  void                setCurrentColumn(int c) { m_col=c; };
  void                setRowCount(int r) { m_cntRow=r; };
  void                setColumnCount(int c) { m_cntCol=c; };
  int                 getRowCount() { return m_cntRow; };
  int                 getColumnCount() { return m_cntCol; };
  GuiTableLineList&   getTableList() { return  m_table_list; }
  GuiTableLine*       getToolTipLine() { return  m_tooltip_line; }
  GuiTableLine*       getColorLine() { return  m_color_line; }
  int                 getSizeVLineList(SizeType size_type=size_default);
  int                 getSizeHLineList(SizeType size_type=size_default);
  int                 getSizeTableList(SizeType size_type=size_default);

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/

private:
  typedef std::list< std::pair< TableActionFunctor *, GuiDataField *> > TableActionList;

  std::string            m_title;
  std::string            m_title_horizontal;
  std::string            m_title_vertical;
  GuiElement::Alignment  m_title_alignment;
  GuiElement::Alignment    m_label_alignment;
  GuiElement::Alignment    m_index_alignment;
  GuiElement::Orientation  m_navigation;
  GuiElement::Orientation  m_orientation;

  GuiElement::Alignment  m_hline_placement;

  GuiElement::Alignment  m_vline_placement;

  int                 m_cntRow;
  int                 m_cntCol;
  int                 m_row;
  int                 m_col;
  bool                m_emptyRow;
  bool                m_emptyColumn;
  GuiTableLineList    m_hline_list;
  GuiTableLineList    m_vline_list;
  GuiTableLineList    m_table_list;
  GuiTableLine*       m_tooltip_line;
  GuiTableLine*       m_color_line;
  int                 m_horzHeaderLines;
  std::vector<int>    m_vertHeaderLines;
  bool                m_hideIndexMenu;

  bool                m_table_matrix;
  int                 m_table_cols;
  int                 m_table_rows;

  int                 m_row_range_from;
  int                 m_row_range_to;
  bool                m_row_header_hidden;
  GuiPopupMenu       *m_row_menu;
  GuiMenuButton      *m_row_menu_title;

  int                 m_col_range_from;
  int                 m_col_range_to;
  bool                m_col_header_hidden;
  GuiPopupMenu       *m_col_menu;
  GuiMenuButton      *m_col_menu_title;

  TableActionList           m_tableaction_list;
  TableActionList::iterator m_tableaction_iter;
  int                       m_running_key;

  InsertListener      m_row_insert;
  DeleteListener      m_row_delete;
  DeleteListener      m_row_delete_selected;
  DuplicateListener   m_row_duplicate;
  ClearListener       m_row_clear;
  PackListener        m_row_pack;

  InsertListener      m_col_insert;
  DeleteListener      m_col_delete;
  DeleteListener      m_col_delete_selected;
  DuplicateListener   m_col_duplicate;
  ClearListener       m_col_clear;
  PackListener        m_col_pack;

  // Stream of Table
  Stream*              m_stream;
  /** Pointer to JobFunction */
  JobFunction         *m_function;
  bool                 m_auto_width;
  /** used only by WebApi */
  int m_argRowColumn;
  std::vector<int> m_argSelected;
  std::string m_clipboardText;

  int                 m_maxRowLen;  // used of WebApi
  friend class GuiQtTableViewBase;  // WebApi
};

class GuiTableSize
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
 public:
  GuiTableSize()
    : m_size( 5 )
    , m_range_from( 0 )
    , m_range_to( 0 )
    , m_hideHeader(false) {
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  inline void setSize( int sz )       { m_size = sz; }
  inline void setRange( int from, int to ) { m_range_from = from; m_range_to = to; }
  inline void hideHeader() { m_hideHeader = true; }
  inline int Size()      { return m_size; }
  inline int RangeFrom() { return m_range_from; }
  inline int RangeTo()   { return m_range_to; }
  inline bool IsHeaderHidden()   { return m_hideHeader; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  int     m_size;
  int     m_range_from;
  int     m_range_to ;
  bool    m_hideHeader ;
};

class GuiTableTrigger : public JobStarter{
public:
  GuiTableTrigger( JobFunction *f )
    : JobStarter( f ){}
  virtual ~GuiTableTrigger() {}
  virtual void backFromJobStarter( JobAction::JobResult rslt ) {
    if( rslt == JobAction::job_Ok ){
      GuiManager::Instance().update( GuiElement::reason_Process );
    }
  }
};

#endif
