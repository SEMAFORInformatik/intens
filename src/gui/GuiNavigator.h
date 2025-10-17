
#ifndef GUI_NAVIGATOR_H
#define GUI_NAVIGATOR_H

#include "job/JobStarter.h"
#include "job/JobFunction.h"
#if HAVE_PROTOBUF
#include "protobuf/Navigator.pb.h"
#endif

class GuiNavRoot;
class Scale;
class GuiNavElement;
class GuiPopupMenu;
class GuiQtNavigator;
class GuiXNavigator;

class GuiNavigator
{
 public:
  enum Type
  { type_Default = 0
  , type_IconView
  , type_Diagram
  };
  enum ColumnStyle
  { columnStyle_Default = 0
  , columnStyle_Pixmap
  , columnStyle_Sort
  };
  static const char* MIME_COMPONENT;

  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
  GuiNavigator(GuiNavigator::Type navType);
  virtual ~GuiNavigator();

 private:
  GuiNavigator();
  GuiNavigator( const GuiNavigator &nav );
  void operator=( const GuiNavigator &nav );

  /*=============================================================================*/
  /* public definitions                                                          */
  /*=============================================================================*/
 public:
  //==============
  // Root
  //==============
  class Root : public GuiIndexListener{
  public:
    Root( std::string name, XferDataItem *dataItem )
      : m_name( name )
      , m_dataItem( dataItem )
      , m_node( 0 )
      , m_firstStructFolder( 0 )
      , m_lastLevel( 0 )
      , m_openLevels( 0 )
      , m_autoLevel( false )
      , m_hideEmptyFolder( false ) {}
    virtual ~Root();

    void setIndex( const std::string &name, int inx );
    bool acceptIndex( const std::string &name, int inx );

    void setFirstStructFolder( int firstStructFolder );
    int *getFirstStructFolder(){ return m_firstStructFolder; }
    void setLastLevel( int lastLevel );
    int *getLastLevel(){ return m_lastLevel; }
    void setAutoLevel();
    bool autoLevel(){ return m_autoLevel; }
    void setHideEmptyFolder();
    bool hideEmptyFolder(){ return m_hideEmptyFolder; }
    void setOpenLevels( int openLevels );
    int *getOpenLevels(){ return m_openLevels; }
    bool isUpdated() const;
    bool getChildrenCount( int &actualCount, int &newCount );
    const std::string &getName(){ return m_name; }
    GuiNavElement *getNode() const { return m_node; }
    void setNode( GuiNavElement *node ){ m_node = node; }
    void refresh(int rootIdx);
    void createChildren();
    bool isOpen();
    void addChildren( int n );
    void removeChildren( int n );
    void update( GuiElement::UpdateReason ur );
    XferDataItem *getDataItem(){ return m_dataItem; }
    bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
    bool serializeProtobuf(in_proto::ElementList* eles, in_proto::NavigatorNode* element, bool onlyUpdated = false);
#endif
  private:
    std::string                m_name;
    XferDataItem              *m_dataItem;
    GuiNavElement             *m_node;
    bool                       m_autoLevel;
    bool                       m_hideEmptyFolder;
    int                       *m_lastLevel;
    int                       *m_firstStructFolder;
    int                       *m_openLevels;
  };

  typedef std::vector<Root *>  RootsVector;

  //==============
  // Column
  //==============
  class Column{
  public:
    Column( const std::string &tag
	    , const std::string &label
	    , const int width
	    , const int precision
	    , Scale *scale
	    , const bool thousand_sep
	    , ColumnStyle style)
      : m_tag( tag )
      , m_width( width )
      , m_label( label )
      , m_precision( precision )
      , m_scale( scale )
      , m_thousand_sep( thousand_sep )
      , m_style( style ) {}
    virtual ~Column();
    const std::string &getTag(){ return m_tag; }
    const std::string &getLabel(){ return m_label; }
    std::string getFormatedLabel();
    const int getWidth(){ return m_width; }
    const int getPrecision(){ return m_precision; }
    const double getScale();
    Scale *scale(){ return m_scale; }
    const bool thousandSep(){ return m_thousand_sep; }
    const ColumnStyle style(){ return m_style; }
  private:
    std::string           m_tag;
    std::string           m_label;
    int                   m_width;
    int                   m_precision;
    Scale                *m_scale;
    bool                  m_thousand_sep;
    ColumnStyle           m_style;
  };
  typedef std::vector<Column *> ColsVector;
  typedef ColsVector::iterator ColsIterator;

  //===============
  // ConnectionAttr
  //===============
  struct ConnectionAttr{
    ConnectionAttr(int _lineWidth, const std::string& _lineColor, const std::string& _lineStyle,
                   std::vector<double>& _xpos, std::vector<double>& _ypos, int connectType)
      : lineWidth(_lineWidth),
        lineColor(_lineColor),
        lineStyle(_lineStyle),
        xpos(_xpos),
        ypos(_ypos),
        connectType(connectType)
    {}
    int         lineWidth;
    std::string lineColor;
    std::string lineStyle;
    std::vector<double> xpos, ypos;
    std::vector<int> anchor_xpos, anchor_ypos;
    int connectType;  // enum GuiQtDiagramConnection::ConnectType
  };
  public:
 private:
  //==============
  // Trigger
  //==============
  class Trigger : public JobStarter
    {
    public:
      Trigger( GuiNavigator *navigator, JobFunction *f )
	: JobStarter( f )
	, m_navigator( navigator )
	, m_func(f) {
	if (s_cnt) { setBlockUndo(); }
	++s_cnt;
	m_saveFuncsetUpdateForms = m_func->getUpdateForms();
	m_func->setUpdateForms(false); // we do this job
      }
      virtual ~Trigger() { m_func->setUpdateForms(m_saveFuncsetUpdateForms); }
      virtual void backFromJobStarter( JobAction::JobResult jobResult );
      static int getCountInstances() { return s_cnt; }
    private:
      GuiNavigator *m_navigator;
      JobFunction  *m_func;
      bool          m_saveFuncsetUpdateForms;
      static int    s_cnt;
    };

  /*=============================================================================*/
  /* public member functions from GuiElement                                     */
  /*=============================================================================*/
 public:
  void getSize( int &width, int &height );
  void serializeXML(std::ostream &os, bool recursive = false);
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

  /*=============================================================================*/
  /* public member functions                                                     */
  /*=============================================================================*/
 public:
  /** liefert das objekt des aktuellen nodes   */
  virtual GuiNavElement *getCurrentNode()=0;
  virtual void freeze()=0;
  bool isNodeValid( XferDataItem *item );
  bool isNodeValid( const DataReference *ref );
  ColsVector &getCols(){ return m_cols; }
  bool setRunningMode();
  JobStarter *getNewTrigger( JobFunction *func );
  int countTrigger();
  void setSize( const int &width, const int &height );
  void setToolTipTag( const std::string &tag );
  void addColumn( const std::string &tag
		  , const std::string &label
		  , const int length
		  , const int precission
		  , Scale *scale
		  , const bool thousand_sep = false
		  , ColumnStyle colStyle = columnStyle_Default);
  virtual void addColumnsExtension(int numColumns) = 0;
  void setMultipleSelection(bool multiSelection);
  bool isMultipleSelection();
  void setExpandable(bool expandable);
  void setScrollable(bool expandable);
  bool isExpandable();
  bool isScrollable();
  /** ein transparentes folder hinzufügen
      alle Kindeelemente werden stattdessen im Parent dargestellt
   */
  void addTransparentFolder( const std::string& hide_folder );
  bool isTransparentFolder(const std::string& folder_name );

  /** erzeugt ein neues root und nimmt es in m_roots auf.
   */
  Root *setRoot( XferDataItem *const item, const std::string &title );
  /** wird fuer die JobFunction der nodes benoetigt
   */
  virtual void doEndOfWork( bool error );
  virtual GuiElement* getElement() = 0;
  virtual GuiQtNavigator *getQtNavigator(){ return 0; }
  virtual GuiXNavigator *getXNavigator(){ return 0; }
  virtual void clearSelection(){}
  virtual void setItemSelection(const std::string& id, bool bSelected=true) = 0;
  virtual void getItemsSelected(std::vector<std::string>& idxs) = 0;
  virtual void getDiagramConnections(std::vector<std::string>& vecA,
                                     std::vector<std::string>& vecB,
                                     std::vector<ConnectionAttr>& attr) = 0;
  virtual void getDiagramSelectItems(std::vector<std::string>& select_items) = 0;
  virtual double getDiagramConnectionRatio() = 0;

  GuiNavigator::Type  getNavigatorType() const { return m_navType; }
  bool         isDefaultSize() const { return m_isDefaultSize; }
  const std::string &getToolTipTag(){ return m_toolTipTag; }
  void setCompareMode() { m_compareMode = true; }
  bool isCompareMode() { return m_compareMode; }

  enum eOutputCompareMode {
      output_Equality
    , output_Inequality
    , output_Complete
  };
  void setOutputCompareMode(eOutputCompareMode mode) { m_outputCompareMode = mode; }
  eOutputCompareMode getOutputCompareMode()          { return m_outputCompareMode; }
  bool isOutputCompareMode(eOutputCompareMode mode)  { return m_outputCompareMode == mode; }
  /*=============================================================================*/
  /* protected member functions                                                  */
  /*=============================================================================*/
 protected:
  virtual void repaint()=0;
  virtual void addActions()=0;
  virtual void updateTree();
  virtual void createTree( Root * const root, int rootIdx )=0;

  /*=============================================================================*/
  /* private member functions                                                    */
  /*=============================================================================*/
 private:
  void unsetRunningMode();
  const std::string StringNavType();
  void getVisibleNodes(Json::Value& jsonObj, Json::Value& jsonResult);

  /*=============================================================================*/
  /* protected Data                                                              */
  /*=============================================================================*/
 protected:
  ColsVector                            m_cols;
  bool                                  m_freezed;
  bool                                  m_updating;
  RootsVector                           m_roots;

  /*=============================================================================*/
  /* private Data                                                                */
  /*=============================================================================*/
 private:
  int                                   m_widgetWidth;
  int                                   m_widgetHeight;
  bool                                  m_isDefaultSize;
  int                                   m_jobRunningKey;
  GuiNavigator::Type                    m_navType;
  std::string                           m_toolTipTag;
  bool                                  m_multiSelection;
  bool                                  m_expandable;
  bool                                  m_scrollable;
  bool                                  m_compareMode;
  std::vector<std::string>              m_transparentFolderNames;
  eOutputCompareMode                    m_outputCompareMode;
};

#endif
