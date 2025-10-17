
#if !defined(GUI_INDEX_INCLUDED_H)
#define GUI_INDEX_INCLUDED_H

#include "xfer/XferParameterInteger.h"
#include "job/JobAction.h"
#include "gui/GuiElement.h"

class JobFunction;

class GuiIndex
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiIndex( GuiElement *parent, const std::string &name );
  GuiIndex( const GuiIndex &index );

  virtual ~GuiIndex(){}

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  class Trigger : public JobAction
  {
  public:
    Trigger( GuiIndex *index )
      : m_index( index ){}
    virtual ~Trigger() {}
  public:
    virtual void printLogTitle( std::ostream &ostr ){}
    virtual void serializeXML(std::ostream &os, bool recursive = false) {}
    virtual void startJobAction(){}
    virtual void stopJobAction(){}

  protected:
    virtual void backFromJobController( JobResult rslt );

  private:
    GuiIndex  *m_index;
  };

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual bool cloneable() = 0;
  virtual GuiElement *clone() = 0;
  virtual GuiElement *getElement() = 0;
  /** JSF Corba:: get all visible Datapool Values */
  void getVisibleDataPoolValues( GuiValueList& vmap );
  virtual void serializeXML(std::ostream &os, bool recursive);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

/*=============================================================================*/
/* public member functions of static index lists                               */
/*=============================================================================*/
public:
  void setName( const std::string &name ) { m_name = name; }
  virtual const std::string& getName() { return m_name; }

  /** Beim Lesen der Syntax müssen Index-Elemente, welche in Indexlisten von
      Daten vorkommen, zwischengespeichert werden, da das Registrieren des
      GuiDataField bei den GuiIndex erst am Ende möglich ist.
  */
  static void clearIndexContainer();
  static void addIndexContainer( const std::string &, GuiIndex * );
  static void registerIndexContainer( GuiIndexListener * );
  static void unregisterIndexContainer( GuiIndexListener * );
  virtual void setPlusMinusStyle() {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setFunction( JobFunction *f ) { m_function = f; };
  JobFunction *getFunction() { return m_function; }
  virtual void registerIndexedElement( GuiIndexListener * ) = 0;
  virtual void unregisterIndexedElement( GuiIndexListener * ) = 0;
  virtual void setMinIndex( int ) = 0;
  virtual void setMaxIndex( int ) = 0;
  virtual int getIndex() = 0;
  virtual void setIndex( int ) = 0;
  JobAction *getIndexAction( int inx );
  JobAction *getIndexAction();
  virtual void setStep( int );
  virtual void setOrientation( GuiElement::Orientation orient ) = 0;
  virtual int shiftRight() = 0;

  virtual bool isIndexAccepted( int );
  virtual void setNewIndex( int inx );
  virtual void setNewIndex();
  bool isValid();
  bool setDataValue( int index );

/*=============================================================================*/
/* public member functions of GuiIndexListener                                 */
/*=============================================================================*/
public:
  virtual bool acceptIndex( const std::string &, int );
  virtual void setIndex( const std::string &, int );
  virtual bool isIndexActivated();

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  void backFromJobController( JobAction::JobResult rslt );
  bool startFunction();
  void doEndOfWork( bool error, bool update );

  virtual void updateMyself(){}
/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
protected:
  typedef std::list<GuiIndexListener *> GuiIndexListenerList;
  GuiIndexListenerList    m_listeners;

  bool                    m_disabled;
  GuiElement::Orientation m_orientation;
  int                     m_min_index;
  int                     m_max_index;
  int                     m_step;
  int                     m_index;
  bool                    m_cloned;
  int                     m_new_index;
  XferParameterInteger    m_param;
  bool                    m_activeFunc;

private:
  std::string             m_name;
  typedef std::map<std::string, GuiIndex *> GuiIndexContainer;
  static GuiIndexContainer     s_index_container;
  JobFunction                 *m_function;
};

#endif
