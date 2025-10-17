
#if !defined(GUI_QT_CYCLEDIALOG_INCLUDED_H)
#define GUI_QT_CYCLEDIALOG_INCLUDED_H

#include <vector>
#include <string>

#include "gui/GuiCycleDialog.h"
#include <qwidget.h>

class GuiQtForm;
class GuiQtScrolledlist;
class GuiQtDataField;
class GuiQtCycleInputBox;

class DataReference;


/**Mit diesem Dialog k\"onnen Cycles erstellt, gel\"oscht oder entfernt werden.
  *@author Anton Gerber
  */


class GuiQtCycleDialog : public GuiCycleDialog
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtCycleDialog();
  ~GuiQtCycleDialog();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Implementierung der Methode ButtonPressed vom GuiButtonListener */
  virtual void ButtonPressed();
  /** Implementierung der Methode start vom GuiScrolledlistListener */
  virtual bool start() { return true; }
  /** Implementierung der Methode getNextItem vom GuiScrolledlistListener */
  virtual bool getNextItem( std::string &label );
  /** Implementierung der Methode activated vom GuiScrolledlistListener */
  virtual void activated( int index );
  /** Implementierung der Methode selected vom GuiScrolledlistListener */
  virtual void selected( int index, bool  ) { };
  /** Implementierung der Methode selected vom GuiScrolledlistListener */
  virtual void setValue(int, const std::string&);
  /** \"Offnet ein Eingabedialog */
  void openInputBox( int cycle );
  /** \"Offnet den Dialog */
  void openDialog();
  /** Ereignis vom Button 'Ok' des Konfigurationsdialoges */
  void okEvent();
  /** Ereignis vom Button 'New' des Konfigurationsdialoges */
  void newEvent();
  /** Ereignis vom Button 'Clear' des Konfigurationsdialoges */
  void clearEvent();
  /** Ereignis vom Button 'Delete' des Konfigurationsdialoges */
  void deleteEvent();
  /** Ereignis vom Button 'Cancel' des Konfigurationsdialoges */
  void cancelEvent();
  /** Only to be used by internal classes*/
  void updateList( int select );

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  /// Klasse zur Verarbeitung von Ereignissen vom Button 'ok'
  class OkButtonListener : public GuiButtonListener {
  public:
    OkButtonListener( GuiQtCycleDialog *dialog ) : m_dialog( dialog ) {}
    virtual void ButtonPressed() {
      if( m_dialog != 0 ) m_dialog->okEvent(); };
    JobAction* getAction(){ return 0; }
  private:
    GuiQtCycleDialog *m_dialog;
  };

  /// Klasse zur Verarbeitung von Ereignissen vom Button 'New'
  class NewButtonListener : public GuiButtonListener {
  public:
    NewButtonListener( GuiQtCycleDialog *dialog ) : m_dialog( dialog ) {}
    virtual void ButtonPressed() {
      if( m_dialog != 0 ) m_dialog->newEvent(); };
    JobAction* getAction(){ return 0; }
  private:
    GuiQtCycleDialog *m_dialog;
  };

  /// Klasse zur Verarbeitung von Ereignissen vom Button 'Clear'
  class ClearButtonListener : public GuiButtonListener {
  public:
    ClearButtonListener( GuiQtCycleDialog *dialog ) : m_dialog( dialog ) {}
    virtual void ButtonPressed() {
      if( m_dialog != 0 ) m_dialog->clearEvent(); };
    JobAction* getAction(){ return 0; }
  private:
    GuiQtCycleDialog *m_dialog;
  };

  /// Klasse zur Verarbeitung von Ereignissen vom Button 'Delete'
  class DeleteButtonListener : public GuiButtonListener {
  public:
    DeleteButtonListener( GuiQtCycleDialog *dialog ) : m_dialog( dialog ) {}
    virtual void ButtonPressed() {
      if( m_dialog != 0 ) m_dialog->deleteEvent(); };
    JobAction* getAction(){ return 0; }
  private:
    GuiQtCycleDialog *m_dialog;
  };

  /// Klasse zur Verarbeitung von Ereignissen vom Button 'Cancel'
  class CancelButtonListener : public GuiButtonListener {
  public:
    CancelButtonListener( GuiQtCycleDialog *dialog ) : m_dialog( dialog ) {}
    virtual void ButtonPressed() {
      if( m_dialog != 0 ) m_dialog->cancelEvent(); };
    JobAction* getAction(){ return 0; }
  private:
    GuiQtCycleDialog *m_dialog;
  };

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void buildDialog();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::vector<std::string> CycleList;

  DataReference        *m_dref;
  int                   m_actcycle;
  int                   m_counter;
  CycleList             m_cyclelist;
  GuiQtForm            *m_dialog;
  GuiQtScrolledlist    *m_list;
  OkButtonListener      m_okButtonListener;
  NewButtonListener     m_newButtonListener;
  ClearButtonListener   m_clearButtonListener;
  DeleteButtonListener  m_deleteButtonListener;
  CancelButtonListener  m_cancelButtonListener;

  GuiQtCycleInputBox   *m_inputBox;

};



class GuiQtCycleInputBox
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtCycleInputBox( GuiQtCycleDialog* cycdialog = 0 );
  ~GuiQtCycleInputBox() {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void open( int cycle );
  void okEvent();
  void cancelEvent();

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  class OkListener : public GuiButtonListener {
  public:
    OkListener( GuiQtCycleInputBox *dialog ) : m_dialog( dialog ) {}
    virtual void ButtonPressed();
    JobAction* getAction(){ return 0; }
  private:
    GuiQtCycleInputBox *m_dialog;
  };

private: // Microsoft VC want this class Declaration public (I don't know why)
  class CancelListener : public GuiButtonListener {
  public:
    CancelListener( GuiQtCycleInputBox *dialog ) : m_dialog( dialog ) {}
    virtual void ButtonPressed();
    JobAction* getAction(){ return 0; }
  private:
    GuiQtCycleInputBox *m_dialog;
  };

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiQtForm        *m_dialog;
  GuiQtDataField   *m_field;
  GuiQtCycleDialog *m_cycdialog;
  int                  m_cycle;
  OkListener           m_okButtonListener;
  CancelListener       m_cancelButtonListener;
};

#endif
