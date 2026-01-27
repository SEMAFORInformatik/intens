#if !defined(GUI_FIELDGROUP_INCLUDED_H)
#define GUI_FIELDGROUP_INCLUDED_H

#include "gui/GuiElement.h"
#include "gui/GuiContainer.h"
#include "job/JobStarter.h"
#include "job/JobFunction.h"

#if HAVE_PROTOBUF
#include "protobuf/FieldGroup.pb.h"
#endif

class GuiFieldgroupLine;
class GuiIndex;
class GuiDataField;
class JobFunction;

class GuiFieldgroup
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiFieldgroup( GuiElement *parent, const std::string &name );
  virtual ~GuiFieldgroup();
protected:
  GuiFieldgroup( const GuiFieldgroup &fg );

/*=============================================================================*/
/* public member functions of GuiFieldgroup                                    */
/*=============================================================================*/
public:
  virtual bool setTableSize(int);
  virtual int  getTableSize();
  virtual bool setTableStep(int);
  int getTableMin() { return m_tablemin; }
  int getTableMax() { return m_tablemax; }

  GuiFieldgroupLine *addFieldgroupLine();

  virtual bool withFrame() { return m_use_frame; }
  virtual void setFrame( GuiElement::FlagStatus s ) {
    m_use_frame = (s == GuiElement::status_ON ? true : false);
  }

  virtual bool setTablePosition( int );
  virtual int  getTablePosition();
  virtual bool setTableIndexRange( int, int );
  virtual void setOrientation(GuiElement::Orientation o);
  virtual void setNavigation(GuiElement::Orientation n) { m_navigation = n; };
  virtual void setTitle(const std::string &title);
  virtual void setTitleAlignment(GuiElement::Alignment align) { m_title_alignment = align; }

  virtual void setIndexAlignment( GuiElement::Alignment align );
  virtual void setAlignFields() { m_align_fields = true; }
  virtual bool setMargins( int margin, int spacing);
  virtual bool setGuiIndex(GuiIndex* index );
  /** get stretch factor */
  virtual int getStretchFactor( GuiElement::Orientation orient ) = 0;
  ///  virtual GuiFieldgroupLine *addFieldgroupLine() = 0;
  virtual bool addArrowbar();
  virtual void createArrowbar();
  ///  virtual void attach( GuiElement * ) = 0;
  const std::string &Name() const { return m_name; }

  virtual bool handleTableAction(GuiTableActionFunctor &func);
  virtual void nextTableAction(bool error);
  virtual void hideIndexMenu();
  virtual void hideIndexLabel();
  virtual void showColumn(int columnIdx, bool visible, int rowOffset=0) = 0;
  virtual BasicStream *streamableObject();

  virtual GuiElement * getElement() = 0;
  JobFunction* getFunction() const { return m_function; }
  virtual void setFunction( JobFunction *func );

  static GuiFieldgroup *getFieldgroup( const std::string &id );
  void serializeXML(std::ostream &os, bool recursive = false);
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
  virtual bool serializeContainerElements(in_proto::ElementList* eles, in_proto::FieldGroup* element, bool onlyUpdated = false)=0;
#endif
  virtual void serializeContainerElements( std::ostream &os )=0;
  virtual bool serializeContainerElements(Json::Value& jsonObj, bool onlyUpdated = false)=0;
  const std::string getTitle() { return m_title; }
  virtual void setOverlayGeometry(int xpos, int ypos, int width=-1, int height=-1);
  struct OverlayGeometry{
    OverlayGeometry()
      : xpos(-1), ypos(-1), width(-1), height(-1) {}
    OverlayGeometry(int xpos, int ypos, int width, int height)
      : xpos(xpos), ypos(ypos), width(width), height(height) {}
    bool isValid() { return xpos >= 0 || ypos >= 0 || width >= 0 || height >= 0; }
    int xpos, ypos, width, height;
  };

  OverlayGeometry& getOverlayGeometry() { return m_overlayGeometry; }
  /** set Accordion option */
  void setAccordion(bool accordion, bool open=false) { m_accordion = accordion; m_accordion_open = open; }
  bool hasAccordion() { return m_accordion; }
  bool isAccordionOpen() { return m_accordion_open; }

/*=============================================================================*/
/* protected                                                                   */
/*=============================================================================*/
public:
  class FieldgroupTrigger : public JobStarter
  {
    public:
      FieldgroupTrigger(JobFunction *f)
        : JobStarter(f){}

      virtual ~FieldgroupTrigger() {}
      virtual void backFromJobStarter(JobAction::JobResult rslt);
  };

 protected:
  int getTableStep() { return m_tablestep; }
  int getMargin() { return m_margin; }
  int getSpacing() { return m_spacing; }
  GuiElement::Alignment getTitleAlignment() { return m_title_alignment; }
  GuiIndex* getGuiIndex() { return m_index; }
  void setName(const std::string &name) { m_name = name; }
  void doNormalisation();
  /** Diese Funktion ruft eine variante Methode auf.
      Die Argumente werden diese Methode übergeben.
      Das Result wird als Json-String zurückgeben.
  */
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng);
/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  class Trigger : public JobStarter
  {
  public:
    Trigger( GuiFieldgroup *fg, JobFunction *f )
      : JobStarter( f )
      , m_fieldgroup(fg) {}
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult rslt );
  private:
    GuiFieldgroup *m_fieldgroup;
    JobFunction   *m_function;
  };

/*=============================================================================*/
/* private                                                                     */
/*=============================================================================*/
 private:
  bool setRunningMode();
  void unsetRunningMode();
  void finalTableAction( bool ok );

/*=============================================================================*/
/* protected and private data                                                  */
/*=============================================================================*/
protected:
  typedef std::list<GuiDataField *> TableActionList;

  bool                    m_indexMenu;
  bool                    m_indexLabel;
  GuiElement::Alignment   m_index_alignment;
  GuiElement::Orientation           m_navigation;
  GuiElement::Orientation           m_orientation;
  TableActionList           m_tableaction_list;
  TableActionList::iterator m_tableaction_iter;
  GuiTableActionFunctor    *m_tableaction_func;
  GuiContainer            m_container;

 private:
  JobFunction         *m_function;

  GuiIndex*               m_index;
  int                   m_tablesize;
  int                   m_tablestep;
  int                   m_tablemin;
  int                   m_tablemax;
  int                   m_tableposition;
  bool                  m_use_frame;
  std::string           m_name;
  std::string           m_title;
  int                   m_margin;
  int                   m_spacing;
  bool                  m_align_fields;

  GuiElement::Alignment m_title_alignment;
  int                   m_running_key;
  std::vector<std::string> m_cssGridTemplateColumns;
  std::vector<std::string> m_stretchFactorColumns;
#if HAVE_PROTOBUF
  std::vector<in_proto::FieldGroup::GridTemplateField> m_cssGridTemplateColumns_proto;
#endif

  typedef std::map< std::string, GuiFieldgroup * > FieldgroupMap;
  static FieldgroupMap s_fieldgroupmap;
  OverlayGeometry      m_overlayGeometry;
  bool                 m_accordion;
  bool                 m_accordion_open;
};

#endif
