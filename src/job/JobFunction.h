
#include "app/LSPItem.h"
#if !defined(JOB_FUNCTION_INCLUDED_H)
#define JOB_FUNCTION_INCLUDED_H

#include "job/JobElement.h"
#include "job/JobAction.h"
#include "gui/GuiListenerController.h"
#include "gui/GuiButtonListener.h"
#include "gui/GuiEventData.h"

#include <sstream>

class GuiToggleListener;
class JobCodeItem;
class JobStackData;
class JobEngine;
class GuiElement;
class XferDataItem;
class DataReference;
class JobController;
class Timer;
class PythonModule;
class XferGenericParameter;

/** JobFunction objects can be executed directly as a Job (FUNCTION) with
    calling execute() or indirectly as function invoked from another JobFunction
    or JobTask. In this latter case its code is accessed with getCodeStream()
    and executed by a JobEngine.

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: JobFunction.h,v 1.43 2007/12/19 15:35:32 amg Exp $
*/
class JobFunction : public JobElement
                  , public JobAction
                  , public GuiListenerController
                  , public LSPItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobFunction( const std::string &name );
  virtual ~JobFunction();

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  ///
  virtual void printLogTitle( std::ostream &ostr );

  /** set the reason of the invocation.
      Can be looked up by a JobFunction.
      @param reason reason of invocation.
  */
  virtual void setReason( CallReason reason ) { m_reason = reason; }

  /** set the gui element
      @param el GUI element
  */
  virtual void setGuiElement( GuiElement *el );

  void setErrorStream( std::ostream *ostr ) { m_errorbox_stream = ostr; }

  void setLocalVariables( DataReference *ref );

  /** Die Funktion wird aufgefordert, die als Parameter uebergebene Aktion
      zu starten.
   */
  void nextJobAction( JobAction *action );

  /** Mit Dieser Funktion wird die JobFunction markiert, sobald sie von irgendwo
      aufgerufen wird.
  */
  void setUsed() { m_used = true; }

  /** Mit Dieser Funktion zeigt an, ob die JobFunction von irgendwo aufgerufen wird.
      @return True: JobFunction wird benützt.
  */
  bool isUsed() { return m_used; }

  /** Diese Funktion gibt Auskunft, ob es sich um eine Task handelt.
      @return True: => Das Objekt ist eine Task.
  */
  virtual bool isTask() { return false; }

  /** Diese Funktion teilt uns mit, ob die JobFunction bereits in Gebrauch ist. In
      diesem Fall ist nichts zu machen.
  */
  bool isAlreadyInUse() { return m_engine != 0; }

  /** Diese Funktion zeigt an, ob bereits Code implementiert wurde. Diese
      Information braucht der JobManager, da er dem Parser noch nicht implementierte
      Funktionen als neu verkauft. (Forward Declaration)
      @return True: => Funktion ist nicht implementiert (leer).
  */
  bool notImplemented() { return m_code.size() == 0; }

  /** Diese Funktion wird jeweils vom JobManager aufgerufen um den CodeStream
      zusammenzubauen. Das JobCodeItem wird am Ende das CodeStreams angehängt.
      @param item Pointer auf das CodeItem
      @return Adresse des CodeItems im CodeStream
  */
  int attach( JobCodeItem *item );

  /** Mit dieser Funktion kann an einer bestimmten Position ein CodeItem im
      CodeStream eingefügt. Der Pointer an dieser Position wird dabei überschrieben.
      @param addr gewünschte Adresse des CodeItems item
      @param item Pointer auf das CodeItem
  */
  void insert( int addr, JobCodeItem *item );

  /** Diese Funktion gibt die nächste freie Code-Adresse zurück.
      @return Code-Adresse
  */
  int nextAddress();

  /** Diese Funktion gibt einen Pointer auf den CodeStream der JobFunction
      zurück. Sie wird benötigt um aus einer JobFunktion eine andere JobFunction
      aufzurufen.
      @return Pointer auf den CodeStream.
  */
  JobCodeStream *getCodeStream() { return &m_code; }

  /** Diese Funktion liefert das GuiElement, aus welchem die Funktion gestartet
      wurde. Der Pointer kann auch 0 sein.
  */
  GuiElement *getGuiElement() { return m_gui_element; }

  /** Diese Funktion liefert eine Referenz auf das std::ostream-Objekt für normale
      Meldungen (PRINT).
      @return Output Stream für Meldung
  */
  std::ostream &PrintStream();

  /** Eine hängige Meldung wird ausgegeben.
   */
  void flushPrintStream( bool final=false );

  /** Diese Funktion liefert eine Referenz auf das std::ostream-Objekt für
      Fehlermeldungen (SET_ERROR).
      @return Output Stream für Fehlermeldung
  */
  std::ostream &ErrorStream();

  /** Eine hängige Fehlermeldung wird ausgegeben.
   */
  void flushErrorStream( bool final=false );

  /** Diese Funktion liefert eine Referenz auf das std::ostringstream-Objekt für
      Logmeldungen.
      @return Output Stream für Logmeldung
  */
  std::ostringstream &LogStream();

  /**  Stream für Logmeldung löschen.
   */
  void clearLogStream();

  /** Mit dieser Funktion wird einer JobFunction die Möglichkeit gegeben, alte
      und neue Werte nach einer Eingabe über das Gui anzuzeigen. Die Funktion
      muss vor dem update mit dem neuen Wert aufgerufen werden.
      @param dataitem Pointer auf das XferDataItem-Objekt im GuiElement
  */
  virtual void setData( XferDataItem *new_data, JobStackData *old_data );
  virtual void replaceData( DataReference *new_data, JobStackData *old_data );
  virtual void setBaseDataItem( XferDataItem * );
  virtual void setSourceDataItem( XferDataItem * );
  virtual void setSourceDataItem( const std::vector<XferDataItem*>& );
  virtual void setSource2DataItem( XferDataItem * );
  virtual void setWebReplyResultDataItem( XferDataItem * );
  virtual void setWebReplyResultDataItemProto();

  /** Mit dieser Funktion wird einer JobFunction ein Index übergeben, welcher
      durch die Expression INDEX wieder ausgegeben wird. Dieser Index wird je
      nach Gebrauch der Funktion unterschiedlich verwendet.
      @param index Index
  */
  virtual void setIndex( int index );

  /** Mit dieser Funktion wird einer JobFunction ein DIAGRAM x-Position übergeben, welcher
      durch die Expression DIAGRAM_XPOS wieder ausgegeben wird.
      @param xpos x position
  */
  virtual void setDiagramXPos( double xpos );

  /** Mit dieser Funktion wird einer JobFunction ein DIAGRAM y-Position übergeben, welcher
      durch die Expression DIAGRAM_YPOS wieder ausgegeben wird.
      @param ypos y position
  */
  virtual void setDiagramYPos( double ypos );

  /** Mit dieser Funktion wird das SortCriteria übergeben, welcher
      durch die Expression SORT_CRITERIA wieder ausgegeben wird.
      @param critera rest service sort criteria
  */
  virtual void setSortCriteria(const std::string& critera);

  /** Diese Funktion liefert einen Pointer auf ein neues JobStackData-Objekt
      mit dem alten Wert eines GuiElements. Wenn die kontrollierende JobFunction
      die notwendigen Informationen nicht besitzt, ist der Wert im Objekt ungültig.
      @return Pointer auf ein neues JobStackData-Objekt.
  */
  JobStackData *getOldData();
  void setOldData(JobStackData* data);

  /** Diese Funktion liefert einen Pointer auf ein neues JobStackData-Objekt
      mit dem neuen Wert eines GuiElements. Wenn die kontrollierende JobFunction
      die notwendigen Informationen nicht besitzt, ist der Wert im Objekt ungültig.
      @return Pointer auf ein neues JobStackData-Objekt.
  */
  JobStackData *getNewData();

  /** Diese Funktion liefert einen Pointer auf ein neues DataReference-Objekt
      mit dem neuen Wert eines GuiElements. Wenn die kontrollierende JobFunction
      die notwendigen Informationen nicht besitzt, ist das Objekt ungültig.
      @return Pointer auf ein neues DataReference-Objekt.
  */
  DataReference *getNewDataReference();
  DataReference *getBaseDataReference();
  DataReference *getSourceDataReference();
  std::vector<DataReference*> getSourceListDataReference();
  DataReference *getSource2DataReference();

  /** Diese Funktion liefert einen Pointer auf ein neues DataReference-Objekt auf die
      Structvariable des Items eines GuiElements. Wenn die kontrollierende JobFunction
      die notwendigen Informationen nicht besitzt, ist das Objekt ungültig.
      @return Pointer auf ein neues DataReference-Objekt.
  */
  DataReference *getNewThisReference();

 /** Diese Funktion liefert den Wert des gewünschten Index.
     @param value Referenz der Variable für das Resultat
     @param level Daten-Level der Inputvariable
     @param index Indexnummer innerhalb des Daten-Levels.
 */
  bool getIndex( int &value, int level, int index );
  bool getSourceIndex( int &value, int level, int index );
  bool getSource2Index( int &value, int level, int index );

 /** Diese Funktion liefert den Wert der gewünschten Index-Nummer.
     @param value Referenz der Variable für das Resultat
 */
  bool getIndex( int &value );

 /** Diese Funktion liefert den Wert der gewünschten Diagram x-Position-Nummer.
     @param value Referenz der Variable für das Resultat
 */
  bool getDiagramXPos( double &value );

 /** Diese Funktion liefert den Wert der gewünschten Diagram y-Position-Nummer.
     @param value Referenz der Variable für das Resultat
 */
  bool getDiagramYPos( double &value );

 /** Diese Funktion liefert den Wert der gewünschten SORT_CRITERIA für den RestService.
     @param sortCriteria Referenz der Variable für das Resultat
 */
  bool getSortCriteria( std::string &sortCriteria );

 /** Diese Funktion sagt uns, ob der angegebene CallReason richtig ist.
     @param reason zu prüfender CallReason (Aufruf-Grund)
     @return True: => der Grund stimmt überein
 */
  bool reasonIsEqual( CallReason reason );

  /** Mit dieser Funktion wird eine Funktion gestartet. Ob die Kontrolle wieder
      zurückgegeben werden soll, bestimmt das Setzen eines JobController.
  */
  virtual void startJobAction();

  /** Mit dieser Funktion versucht jemand, die JobFunction (Task) zu stoppen. Die
      Funktion zeigt mit dem Result an, ob dies gelungen ist.
  */
  virtual void stopJobAction();

  /** Durch diesen Aufruf erhält das Objekt die Kontrolle zurück nach einem
      Aufruf eines JobControllers.
  */
  virtual void backFromJobController( JobAction::JobResult rslt );


  /**   adds Parameter
	@param paramName name of parameter
  */
  void addParameter( const char *paramName );

  void serializeXML(std::ostream &os, bool recursive = false);

/*=============================================================================*/
/* protected Functions                                                         */
/*=============================================================================*/
protected:
  virtual void execute();
  void executeNextOperator( OpStatus op_status );
  virtual void epilog( OpStatus op_status );
  void cleanup();

  /** Diese Funktion soll einen neuen GuiButtonListener erstellen.
      @return Pointer auf neuen GuiButtonListener.
  */
  virtual GuiButtonListener *createButtonListener();
  virtual GuiButtonListener *createNavigatorButtonListener( GuiElement * );
  virtual GuiToggleListener *createToggleListener() { return 0; }
  void start();

/*=============================================================================*/
/* private Function                                                            */
/*=============================================================================*/
private:
  void webApiSetResponse(std::ostream &os, bool updateAlways,
                         int startTransactionId, JobAction::JobResult& rslt);

/*=============================================================================*/
/* protected TimerTask class with tick method                                  */
/*=============================================================================*/
protected:
  class MyTimerTask : public TimerTask {
  public:
    MyTimerTask(JobFunction* jf)
      : m_jobfunction(jf){}
    void tick() { m_jobfunction->start();  }
  private:
    JobFunction* m_jobfunction;
  };

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
protected:
  JobEngine         *m_engine;
private:
  GuiElement        *m_gui_element;
  CallReason         m_reason;
  bool               m_canceled;

  typedef std::vector< XferGenericParameter *> ParameterList;

  JobCodeStream      m_code;
  XferDataItem      *m_new_dataitem;
  XferDataItem      *m_baseDataitem;
  XferDataItem      *m_sourceDataitem;
  XferDataItem      *m_webReplyResultDataitem;
  bool               m_webReplyResultDataitemProto;
  std::vector<XferDataItem*> m_sourceListDataitem;
  XferDataItem      *m_source2Dataitem;
  DataReference     *m_new_this_ref;
  JobStackData      *m_old_data;
  int                m_index;
  double             m_diagramXPos;
  double             m_diagramYPos;
  std::string        m_sortCriteria;
  int                m_source2Index;
  std::ostringstream m_log_stream;
  std::ostringstream *m_print_stream;
  std::ostringstream *m_error_stream;
  std::ostream      *m_errorbox_stream;
  DataReference     *m_local_variables;
  bool               m_used;
  int                m_previousCycleNum;
  std::string        m_previousCycleName;
  int                m_saveWepApiTransaction;

  // fuer den Timer
  Timer             *m_timer;
  TimerTask         *m_task;
  bool               m_isStarted;

  ParameterList     parameterList;
};

#endif
