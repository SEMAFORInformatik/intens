
#if !defined(JOB_MANAGER_INCLUDED_H)
#define JOB_MANAGER_INCLUDED_H

#include <map>
#include <stack>

#include "app/ReportGen.h"

#include "job/JobElement.h"
#include "job/JobCodeStandards.h"
#include "job/JobCodeOperator.h"
#include "job/JobCodePush.h"
#include "job/JobCodeEvalAssign.h"
#include "job/JobCodeMessages.h"
#include "job/JobCodeOthers.h"
#include "job/JobCodeAttributes.h"
#include "JobCodeAttrEditableCycle.h"
#include "job/JobCodeExecute.h"
#include "job/JobCodeCycle.h"
#include "job/JobFunction.h"
#include "job/JobTask.h"
#include "job/JobDataReference.h"
#include "gui/GuiElement.h"

class ProcessGroup;
class DbFunction;
class Plugin;
class GuiElement;
class GuiForm;
class GuiPulldownMenu;
class GuiListenerController;
class GuiScrolledText;
class TimerFunction;
class SubscribeInterface;
class MessageQueueRequest;
class MessageQueuePublisher;
class FileStream;

/** Der JobManager verwaltet alle Tasks und Functions. Er fügt die gewünschten
    OpCodes in den Code der Functionen ein. Er enthält alle mehrfach verwendeten
    JobCode-Objekte als Member-Variablen.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: JobManager.h,v 1.74 2008/06/11 09:36:52 amg Exp $
*/
class JobManager : public JobElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  JobManager()
    : m_job_running_key( 0 )
    , m_job_current_key( 1 )
    , m_op_reason_input( cll_Input )
    , m_op_reason_insert( cll_Insert )
    , m_op_reason_duplicate( cll_Dupl )
    , m_op_reason_clear( cll_Clear )
    , m_op_reason_delete( cll_Delete )
    , m_op_reason_pack( cll_Pack )
    , m_op_reason_select( cll_Select )
    , m_op_reason_unselect( cll_Unselect )
    , m_op_reason_select_point( cll_SelectPoint )
    , m_op_reason_select_rectangle( cll_SelectRectangle )
    , m_op_reason_sort( cll_Sort )
    , m_op_reason_activate( cll_Activate )
    , m_op_reason_open( cll_Open )
    , m_op_reason_close( cll_Close )
    , m_op_reason_drop( cll_Drop )
    , m_op_reason_move( cll_Move )
    , m_op_reason_newConnection( cll_NewConnection )
    , m_op_reason_removeConnection( cll_RemoveConnection )
    , m_op_reason_removeElement( cll_RemoveElement )
    , m_op_reason_cycleClear( cll_CycleClear )
    , m_op_reason_cycleDelete( cll_CycleDelete )
    , m_op_reason_cycleNew( cll_CycleNew )
    , m_op_reason_cycleRename( cll_CycleRename )
    , m_op_reason_cycleSwitch( cll_CycleSwitch )
    , m_op_reason_focusIn( cll_FocusIn )
    , m_op_reason_focusOut( cll_FocusOut )
    , m_op_reason_function( cll_Function )
    , m_op_reason_task( cll_Task )
    , m_op_reason_guiUpdate( cll_GuiUpdate )
    , m_op_messagebox_with_title(true)
    , m_job_gui_element( 0 ){
  }
public:
  virtual ~JobManager(){}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  /** Der JobManager ist eine Singleton Class. Es darf nur ein Objekt
      instanziert werden. Diese Funktion liefert den Pointer des Job-Managers.
   */
  static JobManager &Instance();

  void setJobGuiElement( GuiElement *e ){ m_job_gui_element = e; }
  void setActiveJobController(JobController* cntrl) { m_active_controller = cntrl; }
  JobController* getActiveJobController() { return m_active_controller; }

  void printMessage( const std::string &, GuiElement::MessageType, time_t delay = 2 );

  /** Die Funktion liefert ein JobAction-Object, welches unmittelbar nach
      dem Initialisieren des Gui abgerufen werden soll.
   */
  JobAction *getInitialAction( const std::string &name );

  /** Diese Funktion teilt mit, ob bereits ein Job läuft. Ist dies nicht der Fall
      und der Parameter ist nicht 0, so wird das running-Flag gesetzt. Nur mit
      dem entsprechenden Schlüssel kann ein unlock gemacht werden.
   */
  int setRunningMode( int key = 0 );
  void unsetRunningMode( int key );
  bool isRunning();

  /** Diese Funktion liefert einen Pointer auf ein neues JobFunktion-Objekt. Falls
      bereits ein Objekt mit diesem Namen existiert, aber noch kein Code installiert
      worden ist, liefert sie den Pointer auf dieses Objekt. Andernfalls wird als
      Resultat 0 geliefert. So werden auf einfache Weise Forward-Deklarationen möglich.
      @param name Name des gewünschten JobFunktion-Objekts
      @return Pointer auf das JobFunktion-Objekt
  */
  JobFunction *newFunction( const std::string &name );

  /** Diese Funktion liefert einen Pointer auf ein exisitierendes JobFunktion-Objekt.
      Gibt es kein JobFunktion-Objekt mit diesem Namen, so wird als Resultat
      0 oder ein neues JobFunktion-Objekt erzeugt und ein Pointer darauf geliefert.
      @param name Name des gewünschten JobFunktion-Objekts
      @param create bei true wird ein neues JobFunction-Objekt erzeugt falls es noch nicht existiert
      @return Pointer auf das JobFunktion-Objekt
  */
  JobFunction *getFunction( const std::string &name, bool create=false );

  /** Diese Funktion prüft, ob alle Funktionen implementiert worden sind.
      Für jede nicht implementierte Funktion wird eine Warnung ausgegeben.
      @return True: => Nicht implementierte Funktion existiert
  */
  bool printUnimplementedFunctions( std::ostream &ostr );

  /** Diese Funktion liefert einen Pointer auf ein neues JobTask-Objekt. Falls
      bereits ein Objekt mit diesem Namen existiert, aber noch kein Code installiert
      worden ist, liefert sie den Pointer auf dieses Objekt. Andernfalls wird als
      Resultat 0 geliefert. So werden auf einfache Weise Forward-Deklarationen möglich.
      @param name Name des gewünschten JobTask-Objekts
      @return Pointer auf das JobTask-Objekt
  */
  JobTask *newTask( const std::string &name );

  /** Diese Funktion liefert einen Pointer auf ein exisitierendes JobTask-Objekt,
      welches bereits Code enthält. Gibt es kein JobTask-Objekt mit diesem Namen
      oder besitzt das Objekt noch keinen ausführbaren Code, so wird als Resultat
      0 geliefert.
      @param name Name des gewünschten JobTask-Objekts
      @return Pointer auf das JobTask-Objekt
  */
  JobTask *getTask( const std::string &name );

  /** Diese Funktion hängt an das gewünschte PulldownMenu alle noch nicht
      installierten Tasks zum Starten derselben. Somit wird sichergestellt,
      dass alle Tasks, falls nicht anders bestimmt,  mit einem Menubutton
      verbunden sind und damit aufgerufen werden können.
      @param menu bereits vorhandenes Menu für attach().
  */
  void createTaskMenu( GuiPulldownMenu *menu );

  /** Diese Funktion legt den Pointer eines JobDataReference-Objekts zusammen mit
      dem Parameter level auf einem Stack ab. Mit popDataRef() wird er wieder vom
      Stack abgerufen. Diese Funktion wird vom Parser benötigt.
      @param ref Pointer auf ein JobDataReference-Objekt.
      @param level Data-Level Nummer
  */
  void pushDataRef( JobDataReference *ref, int level );

  /** Diese Funktion ruft ein JobDataReference-Objekt und den zugehörigen Data-Level
      vom Stack ab. (siehe auch pushDataRef() ). Falls der Stack leer ist, liefert
      die Funktion als Resultat 0.
      @param level Referenz einer Variablen für den Data-Level
      @return Pointer auf das JobDataReference-Objekt
  */
  JobDataReference *popDataRef( int &level );

  /// Pop-Operator
  //@{
  /** Die Funktion hängt ein neues JobCodePopAndDelete-Objekt im Code der Funktion func an.
      Das Code-Objekt macht nichts anderes als einen Wert auf dem Stack entfernen und löschen.
      @param func Im Aufbau befindliche Funktion
  */
  int opPopAndDelete( JobFunction *func ){ return func->attach( &m_op_pop_and_delete ); }
  //@}

  /// Push-Operatoren
  //@{
  /** Die Funktion hängt ein neues JobCodePushVariable-Objekt mit dem gewünschen
      JobDataReference-Objekt im Code der Funktion func an. Das Code-Objekt übernimmt
      die Verantwortung für das JobDataReference-Objekt.
      @param func Im Aufbau befindliche Funktion
      @param ref Pointer auf das JobDataReference-Objekt
      @return Code-Adresse
  */
  int opPushVariable( JobFunction *func, JobDataReference * );
  int opSetThis( JobFunction *func, JobDataReference * );
  int opAssignThis( JobFunction *func );
  int opAssignFuncIndex( JobFunction *func );
  int opAssignFuncReason( JobFunction *func );
  int opAssignFuncArgs( JobFunction *func );
  int opSetBase( JobFunction *func, JobDataReference * );
  int opSetSource( JobFunction *func, JobDataReference *, JobDataReference * );
  int opSetSource2( JobFunction *func, JobDataReference * );
  /** Die Funktion hängt ein neues JobCodePushReal-Objekt mit dem konstanten Wert d
      im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @param d konstanter Wert
      @return Code-Adresse
  */
  int opPushReal( JobFunction *func, double d );

  /** Die Funktion hängt ein neues JobCodePushInteger-Objekt mit dem konstanten Wert i
      im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @param i konstanter Wert
      @return Code-Adresse
  */
  int opPushInteger( JobFunction *func, int i );

  /** Die Funktion hängt ein neues JobCodePushString-Objekt mit dem konstanten String s
      im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @param s konstanter String
      @return Code-Adresse
  */
  int opPushString( JobFunction *func, const std::string & );

  /** Die Funktion hängt ein neues JobCodePushEndOfLine-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushEndOfLine( JobFunction *func );

  /** Die Funktion hängt das JobCodePushInvalid-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushInvalid( JobFunction *func ) { return func->attach( &m_op_push_inv ); }

  /** Die Funktion hängt das JobCodePushNewValue-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushNewValue( JobFunction *func ) { return func->attach( &m_op_push_newvalue ); }

  /** Die Funktion hängt das JobCodePushNewValid-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushNewValueValid( JobFunction *func ) { return func->attach( &m_op_push_newvalid ); }

  /** Die Funktion hängt das JobCodePushOldValue-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushOldValue( JobFunction *func ) { return func->attach( &m_op_push_oldvalue ); }

  /** Die Funktion hängt das JobCodePushOldValid-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushOldValueValid( JobFunction *func ) { return func->attach( &m_op_push_oldvalid ); }

  /** Die Funktion hängt das JobCodePushColIndex-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushColumn( JobFunction *func ) { return func->attach( &m_op_push_col_index ); }

  /** Die Funktion hängt das JobCodePushRowIndex-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushRow( JobFunction *func ) { return func->attach( &m_op_push_row_index ); }
   /** Die Funktion hängt das JobCodePushDiagramXPos-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushDiagramXPos( JobFunction *func ) { return func->attach( &m_op_push_diagram_xpos ); }
   /** Die Funktion hängt das JobCodePushDiagramYPos-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushDiagramYPos( JobFunction *func ) { return func->attach( &m_op_push_diagram_ypos ); }
   /** Die Funktion hängt das JobCodePushSortCriteria-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushSortCriteria( JobFunction *func ) { return func->attach( &m_op_push_sort_criteria ); }
  /** Die Funktion hängt das JobCodeLabel-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushLabel( JobFunction *func ) { return func->attach( &m_op_push_label ); }
  /** Die Funktion hängt das JobCodeUnits-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushUnits( JobFunction *func ) { return func->attach( &m_op_push_units ); }
  /** Die Funktion hängt das JobCodePushIndex-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushIndex( JobFunction *func ) { return func->attach( &m_op_push_index ); }

  /** Die Funktion hängt das JobCodePushIndexOfLevel-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushIndexOfLevel( JobFunction *func ) { return func->attach( &m_op_push_index_level ); }

  /** Die Funktion hängt das JobCodePushIndexNumber-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushIndexNumber( JobFunction *func ) { return func->attach( &m_op_push_indexnumber ); }

  /** Die Funktion hängt das JobCodePushError-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPushError( JobFunction *func ) { return func->attach( &m_op_push_error ); }

  /** Die Funktion hängt das JobCodePushReason-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @param reason Abgefragter Grund des Funktionsaufrufs.
      @return Code-Adresse
  */
  int opPushReason( JobFunction *func, CallReason reason );

  /** Die Funktion hängt das JobCodePushIndex-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @param reason Abgefragter Grund des Funktionsaufrufs.
      @return Code-Adresse
  */
  int opPushIndex( JobFunction *func, const std::string &name );

  int opPushSortOrder( JobFunction *func, const std::string &listId );

  /** Die Funktion hängt das JobCodeSetCycleNumber-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opSetCycleNumber( JobFunction *func ) { return func->attach( &m_op_set_cyclenumber ); }

  /** Die Funktion setzt im Hardcopy-Dialog das entsprechende Hardcopy-Element.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opSetHardcopy( JobFunction *func, const std::string &reportId ) { return func->attach( new JobCodeSetHardcopy(reportId) ); }

  /** Die Funktion startet den Hardcopy-Dialog und setzt das entsprechende Hardcopy-Element.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opStartHardcopy( JobFunction *func, ReportGen::Mode mode, HardCopyListener* hcl ) {
    return func->attach( new JobCodeStartHardcopy(mode, hcl) );
  }
  //@}

  /** Im folgenden Abschnitt werden alle Assign- und Evaluations-Operatoren beschrieben.
   */
  //@{
  /** Die Funktion hängt das JobCodeAssign-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opAssignValue( JobFunction *func ) { return func->attach( &m_op_assign ); }

  /** Die Funktion hängt das JobCodeAccumulate-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opAccumulateValue( JobFunction *func ) { return func->attach( &m_op_accumulate ); }

  /** Die Funktion hängt das JobCodeIncrement-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opIncrementValue( JobFunction *func ) { return func->attach( &m_op_increment ); }

  /** Die Funktion hängt das JobCodeDecrement-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opDecrementValue( JobFunction *func ) { return func->attach( &m_op_decrement ); }

  /** Die Funktion hängt das JobCodeEvalValue-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalValue( JobFunction *func ) { return func->attach( &m_op_eval_value ); }

  /** Die Funktion hängt das JobCodeEvalValid-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalValid( JobFunction *func ) { return func->attach( &m_op_eval_valid ); }

  /** Die Funktion hängt das JobCodeEvalComplex-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalComplexValue( JobFunction *func ) { return func->attach( &m_op_eval_complex ); }

  /** Die Funktion hängt das JobCodeEvalReal-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalComplexRealValue( JobFunction *func ) { return func->attach( &m_op_eval_real ); }

  /** Die Funktion hängt das JobCodeEvalImag-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalComplexImagValue( JobFunction *func ) { return func->attach( &m_op_eval_imag ); }

  /** Die Funktion hängt das JobCodeEvalLength-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalLengthValue( JobFunction *func ) { return func->attach( &m_op_eval_length ); }

  /** Die Funktion hängt das JobCodeEvalAbs-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalAbsValue( JobFunction *func ) { return func->attach( &m_op_eval_abs ); }

  /** Die Funktion hängt das JobCodeEvalSqrt-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalSqrtValue( JobFunction *func ) { return func->attach( &m_op_eval_sqrt ); }

  /** Die Funktion hängt das JobCodeEvalSin-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalSinValue( JobFunction *func ) { return func->attach( &m_op_eval_sin ); }

  /** Die Funktion hängt das JobCodeEvalCos-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalCosValue( JobFunction *func ) { return func->attach( &m_op_eval_cos ); }

  /** Die Funktion hängt das JobCodeEvalTan-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalTanValue( JobFunction *func ) { return func->attach( &m_op_eval_tan ); }

  /** Die Funktion hängt das JobCodeEvalASin-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalASinValue( JobFunction *func ) { return func->attach( &m_op_eval_asin ); }

  /** Die Funktion hängt das JobCodeEvalACos-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalACosValue( JobFunction *func ) { return func->attach( &m_op_eval_acos ); }

  /** Die Funktion hängt das JobCodeEvalATan-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalATanValue( JobFunction *func ) { return func->attach( &m_op_eval_atan ); }

  /** Die Funktion hängt das JobCodeEvalATan2-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalATan2Value( JobFunction *func ) { return func->attach( &m_op_eval_atan2 ); }

  /** Die Funktion hängt das JobCodeEvalLog-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */

  int opEvalLogValue( JobFunction *func ) { return func->attach( &m_op_eval_log ); }

  /** Die Funktion hängt das JobCodeEvalLog10-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalLog10Value( JobFunction *func ) { return func->attach( &m_op_eval_log10 ); }
  /** Die Funktion hängt das JobCodeCurrentIsoTime im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opCurrentIsoTime( JobFunction *func ) { return func->attach( &m_op_isotime ); }
  /** Die Funktion hängt das JobCodeCurrentIsoDate im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opCurrentIsoDate( JobFunction *func ) { return func->attach( &m_op_isodate ); }
  /** Die Funktion hängt das JobCodeCurrentIsoDateTime im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opCurrentIsoDatetime( JobFunction *func ) { return func->attach( &m_op_isodatetime ); }

  /** Die Funktion hängt das JobCodeEvalArc-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEvalArgValue( JobFunction *func ) { return func->attach( &m_op_eval_arg ); }

  /** Die Funktion hängt das JobCodeEvalChanged-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @param guiElemId gui element id
      @param filter with filter
      @return Code-Adresse
  */
  int opEvalChangedValue( JobFunction *func, const std::string& guiElemId,
                          const bool filter ) {
    return func->attach( new JobCodeEvalChanged(guiElemId, filter) );
  }

  /** Die Funktion hängt das JobCodeClearValues-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opClearValues( JobFunction *func ) { return func->attach( &m_op_clear_values ); }

  /** Die Funktion hängt das JobCodeEraseValues-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opEraseValues( JobFunction *func ) { return func->attach( &m_op_erase_values ); }

  /** Die Funktion hängt das JobCodeClearTextWindows-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @param textwindow zu löschendes Text-Fenster
      @return Code-Adresse
  */
  int opClearTextWindow( JobFunction *func, GuiScrolledText * );

  /** Die Funktion hängt das JobCodePackValues-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opPackValues( JobFunction *func, bool packRow ) {
    if( packRow ) return func->attach( &m_op_pack_row_values );
    else return func->attach( &m_op_pack_col_values );
  }

  /** Die Funktion hängt das JobCodeSelectRows-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @param guiElementId list, navigator or table id
      @return Code-Adresse
  */
  int opSelectRows( JobFunction *func, const std::string &guiElementId, bool dataIsInt ) {
    return func->attach( new JobCodeSelectRows(guiElementId, dataIsInt) );
  }

  /** Diese Funktion fügt den OpCode JobCodeGetSelection in den Code ein.
      @param func zu bearbeitende Funktion
      @param guiElementId list, navigator, table or plot2d id
      @return Adresse des OpCodes
   */
  int opGetSelection( JobFunction *func, const std::string &guiElementId )    { return func->attach( new JobCodeGetSelection(guiElementId) ); }

  /** Diese Funktion fügt den OpCode JobCodeGetSortCriteria in den Code ein.
      @param func zu bearbeitende Funktion
      @param guiElementId list
      @return Adresse des OpCodes
   */
  int opGetSortCriteria( JobFunction *func, const std::string &guiElementId )    { return func->attach( new JobCodeGetSortCriteria(guiElementId) ); }

  /** Diese Funktion fügt den OpCode JobCodeClearSelection in den Code ein.
      @param func zu bearbeitende Funktion
      @param guiElementId list, navigator or table id
      @return Adresse des OpCodes
   */
  int opClearSelection( JobFunction *func, const std::string &guiElementId )    { return func->attach( new JobCodeClearSelection(guiElementId) ); }

  /** Die Funktion hängt das JobCodeIndex-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opAssignIndex( JobFunction *func, const std::string &name );

  int opBeginTransaction( JobFunction *func ) { return func->attach( &m_op_begin_transaction ); }
  int opCommitTransaction( JobFunction *func ) { return func->attach( &m_op_commit_transaction ); }
  int opAbortTransaction( JobFunction *func ) { return func->attach( &m_op_abort_transaction ); }

  //@}

  /// Branch-Operatoren
  //@{
  /** Die Funktion hängt das JobCodeBranch-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @return Code-Adresse
  */
  int opBranch( JobFunction *func ) { return func->attach( &m_op_branch ); }

  /** Die Funktion hängt ein neues JobCodeBranchWithAddr-Objekt im Code der Funktion func an.
      @param func Im Aufbau befindliche Funktion
      @param addr gewünschte Sprung-Adresse
      @return Code-Adresse
  */
  int opBranch( JobFunction *func, int addr );
  //@}

  /// Spezialisten-Operatoren
  //@{
  int opSend( JobFunction *
              , const std::string &
              , XferDataItem *
              , int
              , XferDataItem *
              , const std::string &
              , Stream *
              , Stream * );
  int opRequest( JobFunction *
                 , Plugin *
                 , const std::vector<Stream*>&
                 , const std::vector<Stream*>&
                 , const std::string& header
                 , const int timeout );
  int opRequest( JobFunction *
                 , MessageQueueRequest *
                 , const std::vector<Stream*>&
                 , const std::vector<Stream*>&
                 , const std::string& header
                 , const int timeout );
  int opPublish( JobFunction *
                 , MessageQueuePublisher *
                 , const std::vector<Stream*>&
                 , const std::string& header );
  int opSubscribe( JobFunction *
                   , SubscribeInterface *
                   , const std::vector<Stream*>&
                   , const std::string& header
                   , JobFunction * );
  int opSetMessageQueueHost( JobFunction * );
  int opRestServiceGet( JobFunction *
                        , const std::string &path
                        , XferDataItem *pathXfer
                        , Stream *pathStream
                        , Stream *filterStream
                        , Stream *responseStream );
  int opRestServiceDelete( JobFunction *
                           , const std::string &path
                           , XferDataItem *pathXfer
                           , Stream *pathStream
                           , Stream *filterStream
                           , Stream *responseStream );
  int opRestServicePut( JobFunction *
                        , const std::string &path
                        , XferDataItem *pathXfer
                        , Stream *pathStream
                        , Stream *dataStream
                        , Stream *responseStream
                        , bool setDbTimestamp );
  int opRestServicePost( JobFunction *
                         , const std::string &path
                         , XferDataItem *pathXfer
                         , Stream *pathStream
                         , Stream *filterStream
                         , Stream *dataStream
                         , Stream *responseStream
                         , bool setDbTimestamp );
  int opRestServiceLogon( JobFunction *
                          , XferDataItem *baseUrl
                          , XferDataItem *username
                          , XferDataItem *password
                          , XferDataItem *message
                          , Stream *responseStream);
  int opRestServiceJwtLogon( JobFunction *
                          , XferDataItem *baseUrl
                          , XferDataItem *jwt
                          , XferDataItem *message
                          , Stream *responseStream);
  int opRestServiceLogoff( JobFunction * );
  int opTimerStart( JobFunction *
                    , TimerFunction *
                    , double period
                    , XferDataItem *periodXfer
                    , double delay
                    , XferDataItem *delayXfer);
  int opTimerStop( JobFunction *, TimerFunction* );

  int opExecuteJobAction( JobFunction *, JobAction * );
  int opExecuteProcess( JobFunction *, ProcessGroup * );
  int opExecuteDatabase( JobFunction *, DbFunction * );
  int opExecutePlugin( JobFunction *, Plugin * );
  int opPluginInitialise( JobFunction *, Plugin * );
  int opPluginParameter( JobFunction *, Plugin * );
  int opActionResult( JobFunction *func ) { return func->attach( &m_op_result ); }
  int opExecuteTask( JobFunction *, const std::string &name );
  int opExecuteFunction( JobFunction *, const std::string &name );
  int opExecuteFunction( JobFunction *, XferDataItem* xferDataItem );
  int opSerializeForm( JobFunction *, GuiForm * );
  int opSerializeGuiElement( JobFunction *, GuiElement *, AppData::SerializeType type,
							 bool readStack=false, const std::string &filename=std::string() );
  int opSerializeElement( JobFunction *, GuiElement *, AppData::SerializeType type,
						  const std::string &filename=std::string() );
  int opWriteSettings( JobFunction * );
  int opMap(JobFunction *, XferDataItem* xferDataItem, int nargs);
  int opMap(JobFunction *, GuiElement *elem, int nargs);
  int opMapForm( JobFunction *, GuiForm * );
  int opMapFolderGroup( JobFunction *, const std::string &, GuiFolderGroup::OmitMap omit_map );
  int opMapTableLine( JobFunction *, const std::string & );
  int opMapGuiElement( JobFunction *, const std::string & );
  int opUnmap( JobFunction * );
  int opUnmapForm( JobFunction *, GuiForm * );
  int opUnmapFolderGroup( JobFunction *, const std::string & );
  int opUnmapGuiElement( JobFunction *, const std::string & );
  int opUnmapTableLine( JobFunction *, const std::string & );
  int opVisible( JobFunction *, XferDataItem* xferDataItem );
  int opVisibleForm( JobFunction *, GuiForm * );
  int opVisibleFolderGroup( JobFunction *, const std::string & );
  int opEditable( JobFunction *, GuiElement* elem=0);
  int opUpdateForms( JobFunction *func );
  int opUpdateGuiElement( JobFunction *func, GuiElement *el );
  int opEnableGuiElement( JobFunction *func, GuiElement *el );
  int opDisableGuiElement( JobFunction *func, GuiElement *el );
  int opDisableDragGuiElement( JobFunction *func, GuiElement *el );
  /** Diese Funktion fügt den OpCode JobCodeSetStylesheet in den Code ein.
      @param func zu bearbeitende Funktion
      @param listId list id
      @return Adresse des OpCodes
   */
  int opSetStylesheet( JobFunction *func, GuiElement *el ) {
    return func->attach( new JobCodeSetStylesheet(el) );
  }
  int opSetStylesheet( JobFunction *func ) {
    return func->attach( new JobCodeSetStylesheet(NULL) );
  }
  int opSetEditable( JobFunction *func ) { return func->attach( &m_op_editable ); }
  int opSetEditableCycle( JobFunction *func ) { return func->attach( &m_op_editable_cycle ); }
  int opSetLocked( JobFunction *func ) { return func->attach( &m_op_locked ); }
  int opSetColor( JobFunction *func ) { return func->attach( &m_op_color ); }
  int opSetColorBit( JobFunction *func ) { return func->attach( &m_op_color_setbit ); }
  int opUnsetColorBit( JobFunction *func ) { return func->attach( &m_op_color_unsetbit ); }
  int opSetDataTimestamp( JobFunction *func ) { return func->attach( &m_op_data_timestamp ); }
  int opSetValueTimestamp( JobFunction *func ) { return func->attach( &m_op_value_timestamp ); }
  int opGuiElementMethod( JobFunction *func, GuiElement *gui_el);
  int opReplaceGuiElement( JobFunction *func, GuiElement *old_el, GuiElement *new_el );
  int opSetGuiFieldgroupRange( JobFunction *func, GuiFieldgroup *fg );
  int opAllow( JobFunction *func, GuiListenerController *ctrl );
  int opDisallow( JobFunction *func, GuiListenerController *ctrl );
  int opRound( JobFunction *func ) { return func->attach( &m_op_round ); }
  int opRound5( JobFunction *func ) { return func->attach( &m_op_round5 ); }
  int opRound10( JobFunction *func ) { return func->attach( &m_op_round10 ); }
  int opGetTimestamp( JobFunction *func ) { return func->attach( &m_op_get_timestamp ); }
  int opGetDbModified( JobFunction *func ) { return func->attach( &m_op_get_db_modified ); }
  int opSetDbTimestamp( JobFunction *func ) { return func->attach( &m_op_set_db_timestamp ); }
  int opGetClassname( JobFunction *func ) { return func->attach( &m_op_get_classname ); }
  int opGetNodename( JobFunction *func ) { return func->attach( &m_op_get_nodename ); }
  int opSize( JobFunction *func ) { return func->attach( &m_op_size ); }
  int opDataSize( JobFunction *func ) { return func->attach( &m_op_data_size ); }
  int opIndex( JobFunction *func ) { return func->attach( &m_op_index ); }
  int opIcon( JobFunction *func ) { return func->attach( &m_op_icon ); }
  int opCompare( JobFunction *func, bool bCycle ) { return func->attach( new JobCodeCompare(bCycle) ); }
  int opCompose( JobFunction *func, int numArgs ) { return func->attach( new JobCodeCompose(numArgs) ); }
  int opAssignConsistency(JobFunction *func) { return func->attach( new JobCodeAssignConsistency() ); }
  int opClass( JobFunction *func, GuiElement *gui_el, const std::string &className) { return func->attach(new JobCodeClass(gui_el, className) ); }
  int opSetResource( JobFunction *func, const std::string &key, const std::string &value, bool useValue) {
	return func->attach( new JobCodeSetResource(key, value, useValue) );
  }
  int opAssignCorresponding( JobFunction *func ) { return func->attach( &m_op_assign_corr ); }
  int opConfirm( JobFunction *func, bool cancelBtn,
				 const std::map<GuiElement::ButtonType, std::string>& buttonText );
  int opGetText( JobFunction *func );
  int opOpenFile( JobFunction *func, FileStream *filesteam,
				  bool readStack=false, const std::string &filename=std::string());
  int opSaveFile( JobFunction *func, FileStream *filesteam, GuiElement* guiElem,
                  bool readStack=false, const std::string &filename=std::string(),
                  bool readBaseStack=false, const std::string &base_filename=std::string());
  int opFileDialog( JobFunction *
                    , const std::string &d
                    , const std::string &f
                    , bool dironly
                    , bool open
                    , XferDataItem * );
  int opMessageBox( JobFunction *func ) { return func->attach( &m_op_messagebox ); }
  int opMessageBoxWithTitle( JobFunction *func ) { return func->attach( &m_op_messagebox_with_title ); }
  int opMessage( JobFunction *func ) { return func->attach( &m_op_message ); }
  int opSetMessage( JobFunction *func ) { return func->attach( &m_op_setmessage ); }
  int opEndOfFunction( JobFunction *func ) { return func->attach( &m_op_eof ); }
  int opAbort( JobFunction *func ) { return func->attach( &m_op_abort ); }
  int opExitIntens( JobFunction *func, bool force) { return func->attach( new JobCodeExitIntens(force) ); }
  int opBeep( JobFunction *func ) { return func->attach( &m_op_beep ); }
  int opCopy( JobFunction *, Stream *, GuiElement* );
  int opPaste( JobFunction *, Stream * );
  int opParse( JobFunction *, XferDataItem * );
  int opParse( JobFunction *, const std::string& filename );
  //@}

  /** Im folgenden Abschnitt werden alle Operatoren beschrieben welche Log- und
      Fehler-Meldungen schreiben.
  */
  //@{
  int opPrint( JobFunction *func ) { return func->attach( &m_op_print ); }
  int opLog( JobFunction *func, const std::string& level );
  int opLogMsg( JobFunction *func )  { return func->attach( &m_op_log_msg  ); }
  int opSetError( JobFunction *func ) { return func->attach( &m_op_seterror ); }
  int opResetError( JobFunction *func ) { return func->attach( &m_op_reseterror ); }
  int opSetErrorMsg( JobFunction *func ) { return func->attach( &m_op_seterror_msg  ); }
  //@}

  /// Cycle-Functionen
  //@{
  int opNewCycle( JobFunction *func ) { return func->attach( &m_op_newcycle ); }
  int opDeleteCycle( JobFunction *func ) { return func->attach( &m_op_deletecycle ); }
  int opClearCycle( JobFunction *func ) { return func->attach( &m_op_clearcycle ); }
  int opPrintLog( JobFunction *func ) { return func->attach( &m_op_printlog ); }
  int opFirstCycle( JobFunction *func ) { return func->attach( &m_op_firstcycle ); }
  int opNextCycle( JobFunction *func ) { return func->attach( &m_op_nextcycle ); }
  int opLastCycle( JobFunction *func ) { return func->attach( &m_op_lastcycle ); }
  int opGoCycle( JobFunction *func ) { return func->attach( &m_op_gocycle ); }
  int opPushCycle( JobFunction *func ) { return func->attach( &m_op_getcycle ); }
  int opPushMaxCycle( JobFunction *func ) { return func->attach( &m_op_maxcycle ); }
  int opGetCycleName( JobFunction *func ) { return func->attach( &m_op_getcyclename ); }
  int opSetCycleName( JobFunction *func ) { return func->attach( &m_op_setcyclename ); }
  int opGetFileName( JobFunction *func ) {
    return func->attach( &m_op_getfilename );
  }
  int opGetDirName( JobFunction *func ) {
    return func->attach( &m_op_getdirname );
  }
  int opGetBaseName( JobFunction *func ) {
    return func->attach( &m_op_getbasename );
  }

  //@}

  /** Im folgeden Abschnitt werden die If- und While-Operatoren beschrieben.
   */
  //@{
  /** Diese Funktion fügt einen neuen While-Operator in den Code ein.
      @param func zu bearbeitende Funktion
      @return Pointer auf den neuen While-Operator
  */
  JobCodeOpWhile *opWhileOperator( JobFunction *func );

  /** Diese Funktion fügt einen neuen If-Operator in den Code ein.
      @param func zu bearbeitende Funktion
      @return Pointer auf den neuen If-Operator
  */
  JobCodeOpIf *opIfOperator( JobFunction *func );
  //@}

  /** Im folgenden Abschnitt werden arythmetischen Operatoren beschrieben.
   */
  //@{
  /** Diese Funktion fügt den OpCode JobCodeOpAdd in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opAdd( JobFunction *func )    { return func->attach( &m_op_add ); }

  /** Diese Funktion fügt den OpCode JobCodeOpSubtract in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opSub( JobFunction *func )    { return func->attach( &m_op_sub ); }

  /** Diese Funktion fügt den OpCode JobCodeOpMultiply in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opMul( JobFunction *func )    { return func->attach( &m_op_mul ); }

  /** Diese Funktion fügt den OpCode JobCodeOpDivide in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opDiv( JobFunction *func )    { return func->attach( &m_op_div ); }

  /** Diese Funktion fügt den OpCode JobCodeOpModulo in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opModulo( JobFunction *func )    { return func->attach( &m_op_modulo ); }

  /** Diese Funktion fügt den OpCode JobCodeOpPower in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opPower( JobFunction *func )  { return func->attach( &m_op_pow ); }

  /** Diese Funktion fügt den OpCode JobCodeOpNegate in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opNegate( JobFunction *func ) { return func->attach( &m_op_negate ); }
  //@}

  /** Im folgenden Abschnitt werden alle boolschen Operatoren beschrieben.
   */
  //@{
  /** Diese Funktion fügt den OpCode JobCodeOpGtr in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opGtr( JobFunction *func )    { return func->attach( &m_op_gtr ); }

  /** Diese Funktion fügt den OpCode JobCodeOpGeq in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opGeq( JobFunction *func )    { return func->attach( &m_op_geq ); }

  /** Diese Funktion fügt den OpCode  in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opLss( JobFunction *func )    { return func->attach( &m_op_lss ); }

  /** Diese Funktion fügt den OpCode JobCodeOpLss in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opLeq( JobFunction *func )    { return func->attach( &m_op_leq ); }

  /** Diese Funktion fügt den OpCode JobCodeOpEql in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opEql( JobFunction *func )    { return func->attach( &m_op_eql ); }

  /** Diese Funktion fügt den OpCode JobCodeOpNeq in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opNeq( JobFunction *func )    { return func->attach( &m_op_neq ); }

  /** Diese Funktion fügt den OpCode JobCodeOpAnd in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opAnd( JobFunction *func )    { return func->attach( &m_op_and ); }

  /** Diese Funktion fügt den OpCode JobCodeOpOr in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opOr( JobFunction *func )     { return func->attach( &m_op_or ); }

  /** Diese Funktion fügt den OpCode JobCodeOpNot in den Code ein.
      @param func zu bearbeitende Funktion
      @return Adresse des OpCodes
   */
  int opNot( JobFunction *func )    { return func->attach( &m_op_not ); }
  //@}
  void serializeFunctions( std::ostream &os, AppData::SerializeType type, bool recursive=false );
  /** set current form (used by webapix)
      @param func zu bearbeitende Funktion
      @param xferDataItem dataReference with form name
      @return Adresse des OpCodes
   */
  int opSetCurrentForm(JobFunction *func, XferDataItem* xferDataItem);

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::map< std::string, JobFunction * >            JobFunctionList;
  typedef std::stack< std::pair< JobDataReference *, int> > JobDataRefStack;

  static JobManager        *s_instance;

  int                       m_job_running_key;
  int                       m_job_current_key;
  JobFunctionList           m_function_list;
  JobDataRefStack           m_dataref_stack;

  JobCodePopAndDelete       m_op_pop_and_delete;
  JobCodePushInvalid        m_op_push_inv;
  JobCodePushNewValue       m_op_push_newvalue;
  JobCodePushNewValid       m_op_push_newvalid;
  JobCodePushOldValue       m_op_push_oldvalue;
  JobCodePushOldValid       m_op_push_oldvalid;
  JobCodePushColIndex       m_op_push_col_index;
  JobCodePushRowIndex       m_op_push_row_index;
  JobCodePushDiagramXPos    m_op_push_diagram_xpos;
  JobCodePushDiagramYPos    m_op_push_diagram_ypos;
  JobCodePushSortCriteria   m_op_push_sort_criteria;
  JobCodePushUnits          m_op_push_units;
  JobCodePushLabel          m_op_push_label;
  JobCodePushIndex          m_op_push_index;
  JobCodePushIndexOfLevel   m_op_push_index_level;
  JobCodePushIndexNumber    m_op_push_indexnumber;
  JobCodePushError          m_op_push_error;
  JobCodeSetCycleNumber     m_op_set_cyclenumber;

  JobCodePushReason         m_op_reason_input;
  JobCodePushReason         m_op_reason_insert;
  JobCodePushReason         m_op_reason_duplicate;
  JobCodePushReason         m_op_reason_clear;
  JobCodePushReason         m_op_reason_delete;
  JobCodePushReason         m_op_reason_pack;
  JobCodePushReason         m_op_reason_select;
  JobCodePushReason         m_op_reason_unselect;
  JobCodePushReason         m_op_reason_select_point;
  JobCodePushReason         m_op_reason_select_rectangle;
  JobCodePushReason         m_op_reason_sort;
  JobCodePushReason         m_op_reason_activate;
  JobCodePushReason         m_op_reason_open;
  JobCodePushReason         m_op_reason_close;
  JobCodePushReason         m_op_reason_drop;
  JobCodePushReason         m_op_reason_move;
  JobCodePushReason         m_op_reason_newConnection;
  JobCodePushReason         m_op_reason_removeConnection;
  JobCodePushReason         m_op_reason_removeElement;
  JobCodePushReason         m_op_reason_cycleClear;
  JobCodePushReason         m_op_reason_cycleDelete;
  JobCodePushReason         m_op_reason_cycleNew;
  JobCodePushReason         m_op_reason_cycleRename;
  JobCodePushReason         m_op_reason_cycleSwitch;
  JobCodePushReason         m_op_reason_focusIn;
  JobCodePushReason         m_op_reason_focusOut;
  JobCodePushReason         m_op_reason_function;
  JobCodePushReason         m_op_reason_task;
  JobCodePushReason         m_op_reason_guiUpdate;

  JobCodeAssign        m_op_assign;
  JobCodeAssignCorr    m_op_assign_corr;
  JobCodeAccumulate    m_op_accumulate;
  JobCodeIncrement     m_op_increment;
  JobCodeDecrement     m_op_decrement;
  JobCodeEvalValue     m_op_eval_value;
  JobCodeEvalComplex   m_op_eval_complex;
  JobCodeEvalReal      m_op_eval_real;
  JobCodeEvalImag      m_op_eval_imag;
  JobCodeEvalAbs       m_op_eval_abs;
  JobCodeEvalLength       m_op_eval_length;
  JobCodeEvalSin       m_op_eval_sin;
  JobCodeEvalCos       m_op_eval_cos;
  JobCodeEvalTan       m_op_eval_tan;
  JobCodeEvalASin      m_op_eval_asin;
  JobCodeEvalACos      m_op_eval_acos;
  JobCodeEvalATan      m_op_eval_atan;
  JobCodeEvalATan2     m_op_eval_atan2;
  JobCodeEvalLog       m_op_eval_log;
  JobCodeEvalLog10     m_op_eval_log10;
  JobCodeEvalSqrt      m_op_eval_sqrt;
  JobCodeEvalArg       m_op_eval_arg;
  JobCodeEvalValid     m_op_eval_valid;
  JobCodeClearValues   m_op_clear_values;
  JobCodeEraseValues   m_op_erase_values;
  JobCodePackRowValues m_op_pack_row_values;
  JobCodePackColValues m_op_pack_col_values;

  JobCodeAttrEditable          m_op_editable;
  JobCodeAttrEditableCycle     m_op_editable_cycle;
  JobCodeAttrLock              m_op_locked;
  JobCodeAttrColor             m_op_color;
  JobCodeAttrColorSetBit       m_op_color_setbit;
  JobCodeAttrColorUnsetBit     m_op_color_unsetbit;
  JobCodeAttrSetDataTimestamp  m_op_data_timestamp;
  JobCodeAttrSetValueTimestamp m_op_value_timestamp;

  JobCodeBeginTransaction      m_op_begin_transaction;
  JobCodeCommitTransaction     m_op_commit_transaction;
  JobCodeAbortTransaction      m_op_abort_transaction;

  JobCodeRound         m_op_round;
  JobCodeRound5        m_op_round5;
  JobCodeRound10       m_op_round10;
  JobCodeTimestamp     m_op_get_timestamp;
  JobCodeDbModified    m_op_get_db_modified;
  JobCodeSetDbTimestamp m_op_set_db_timestamp;
  JobCodeClassname     m_op_get_classname;
  JobCodeNodename     m_op_get_nodename;
  JobCodeSize          m_op_size;
  JobCodeDataSize      m_op_data_size;
  JobCodeIndex         m_op_index;
  JobCodeIcon          m_op_icon;
  JobCodeMessageBox    m_op_messagebox;
  JobCodeMessageBox    m_op_messagebox_with_title;
  JobCodeMessage       m_op_message;
  JobCodeSetMessage    m_op_setmessage;

  JobCodeOpAdd         m_op_add;
  JobCodeOpSubtract    m_op_sub;
  JobCodeOpMultiply    m_op_mul;
  JobCodeOpDivide      m_op_div;
  JobCodeOpModulo      m_op_modulo;
  JobCodeOpPower       m_op_pow;
  JobCodeOpNegate      m_op_negate;
  JobCodeOpGtr         m_op_gtr;
  JobCodeOpGeq         m_op_geq;
  JobCodeOpLss         m_op_lss;
  JobCodeOpLeq         m_op_leq;
  JobCodeOpEql         m_op_eql;
  JobCodeOpNeq         m_op_neq;
  JobCodeOpAnd         m_op_and;
  JobCodeOpOr          m_op_or;
  JobCodeOpNot         m_op_not;

  JobCodePrint         m_op_print;
  JobCodeLogMsg        m_op_log_msg;
  JobCodeSetError      m_op_seterror;
  JobCodeResetError    m_op_reseterror;
  JobCodeSetErrorMsg   m_op_seterror_msg;

  JobCodeActionResult  m_op_result;
  JobCodeBranch        m_op_branch;
  JobCodeEndOfFunction m_op_eof;
  JobCodeAbort         m_op_abort;
  JobCodeBeep          m_op_beep;

  JobCodeNewCycle      m_op_newcycle;
  JobCodeDeleteCycle   m_op_deletecycle;
  JobCodeClearCycle    m_op_clearcycle;
  JobCodePrintLog      m_op_printlog;
  JobCodeFirstCycle    m_op_firstcycle;
  JobCodeLastCycle     m_op_lastcycle;
  JobCodeNextCycle     m_op_nextcycle;
  JobCodeGoCycle       m_op_gocycle;
  JobCodeGetCycle      m_op_getcycle;
  JobCodeMaxCycle      m_op_maxcycle;
  JobCodeGetCycleName  m_op_getcyclename;
  JobCodeSetCycleName  m_op_setcyclename;
  JobCodeGetFileName   m_op_getfilename;
  JobCodeGetBaseName   m_op_getbasename;
  JobCodeGetDirName    m_op_getdirname;
  JobCodeIsoTime       m_op_isotime;
  JobCodeIsoDate       m_op_isodate;
  JobCodeIsoDateTime   m_op_isodatetime;

  GuiElement          *m_job_gui_element;
  JobController       *m_active_controller;
};

#endif
