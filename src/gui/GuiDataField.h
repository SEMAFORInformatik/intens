
#if !defined(GUI_DATAFIELD_INCLUDED_H)
#define GUI_DATAFIELD_INCLUDED_H

#include "gui/GuiElement.h"
#include "gui/ConfirmationListener.h"
#include "xfer/XferDataParameter.h"
#include "xfer/XferDataItem.h"
#include "datapool/DataPool.h"
#include "gui/TimerTask.h"
#include "job/JobStarter.h"
#include "job/JobFunction.h"

class XferDataParameter;
class Scale;
class ColorSet;
class Timer;
class GuiDialog;

/** Das GuiDataField ist die Basisklasse aller GuiElemente, welche mit einer
    Datenvariablen verbunden sind, welche auf der Oberfläche gezeigt wird.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiDataField.h,v 1.60 2008/06/20 11:56:52 amg Exp $
*/
class GuiDataField : public ConfirmationListener{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiDataField( );
  GuiDataField( const GuiDataField &field );

  virtual ~GuiDataField();

/*=============================================================================*/
/* public member functions from GuiElement                                     */
/*=============================================================================*/
  virtual GuiElement* getElement() = 0;

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void getVisibleDataPoolValues( GuiValueList& vmap );
  bool disabled(){ return m_disabled; }
  void setDisabled(bool disabled);
  virtual bool setPrecision( int ) { return false; }
  virtual int getPrecision() { return m_param->getPrecision(); }
  virtual bool setThousandSep() { return false; }
  virtual bool setScalefactor( Scale *scale ){ return false; }
  virtual Scale* getScalefactor() { return NULL; }
  virtual bool setLength( int ) { return false; }
  virtual int getLength(){ return 8; }
  virtual void getColorStrings( std::string &background,
                                std::string &foreground ) { return; }
  virtual bool cloneableForFieldgroupTable() { return true; }
  virtual GuiDataField *CloneForFieldgroupTable() { return 0; }
  void setInverted(bool flag) { m_isInverted = flag; }
  virtual bool isInverted() { return m_isInverted; }
  void setWebApiAttrChanged(bool flag) { m_webApiAttrChanged = flag; }
  virtual bool isWebApiAttrChanged() { return m_webApiAttrChanged; }
  JobFunction *getFunction();
  JobFunction *getFocusFunction();
  bool startFocusFunction(bool focus);

  DataDictionary::DataType getDataType(){
    return m_param->DataItem()->getDataType();
  }
  XferParameter::InputStatus checkFormat( const std::string &s ){
    return m_param->checkFormat( s );
  }
  /** Mit dieser Funktion wird das GuiDataField-Objekt mit dem Datapool verbunden.
      Das dataitem muss zu diesem Zeitpunkt bereits eine DataReference haben.
      Nach installDataItem() ist das GuiDataField bereit für create().
      @param initialisiertes XferDataItem-Objekt
      @return True: => erfolgreich
  */
  virtual bool installDataItem( XferDataItem *dataitem );

  /** Mit dieser Funktion kann das DataItem ausgewechselt werden.
      @param initialisiertes XferDataItem-Objekt
      @return ersetztes XferDataItem-Objekt
  */
  virtual XferDataItem *replaceDataItem( XferDataItem *dataitem );

  DataReference *Data() { return m_param->Data(); }
  XferDataItem *DataItem() { return m_param->DataItem(); }
  std::string FullName() { return m_param->getFullName(); }
  UserAttr *Attr() { return m_param->getUserAttr(); }
  XferDataItemIndex *newDataItemIndex( int );
  XferDataItemIndex *newDataItemIndex();

  int getNumOfWildcards();
  XferDataItemIndex *getDataItemIndexWildcard( int );

// FUER DEN GuiTableActionFunctor
  bool doInsertData( const std::string &row, const std::string &col, int inx );
  bool doDeleteData( const std::string &row, const std::string &col, int inx );
  bool doDuplicateData( const std::string &row, const std::string &col, int inx );
  bool doClearData( const std::string &row, const std::string &col, int inx );
  bool doPackData( const std::string &row, const std::string &col, int inx );

  virtual bool FieldIsEditable();
  bool FieldIsLockable();
  bool FieldIsLocked();

  /** Mit den folgenden Funktionen kann auf einfache weise an die Daten gelangen,
      welche mit dem Objekt verknüpft sind. Es werden dabei die momentan gesetzten
      Indizes verwendet. Aenderungen an den Indizes sind nicht möglich.
  */

  bool getValue( int &i )      { return m_param->DataItem()->getValue( i ); }
  bool getValue( double &d )   { return m_param->DataItem()->getValue( d ); }
  bool getValue( std::string &s )   { return m_param->DataItem()->getValue( s ); }
  bool getValue( dComplex &c ) { return m_param->DataItem()->getValue( c ); }

  void setValue( int i )              { m_param->DataItem()->setValue( i ); }
  void setValue( double d )           { m_param->DataItem()->setValue( d ); }
  void setValue( const std::string &s )    { m_param->DataItem()->setValue( s ); }
  void setValue( const dComplex &c )  { m_param->DataItem()->setValue( c ); }

  bool setInvalid() { return m_param->setInvalid(); }

  void serializeXML(std::ostream &os, bool recursive = false);
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::DataField* field, bool onlyUpdated = false);
#endif

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
public:
  bool setRunningMode();
  void unsetRunningMode();
  bool isRunning();
  void setAttribute( DATAAttributeMask mask );
  bool getAttributes();
  bool lockValue();
  bool isLockable() const;
  bool unlockValue();
  bool isLocked() const;
  void confirm(std::string msg=std::string());
  bool isEditable() const;
  bool isUpdated() const;
  static bool areTargetsCleared(){
    return s_TargetsAreCleared;
  }
  static void setTargetsCleared( bool flag ){
    s_TargetsAreCleared = flag;
  }
  static bool isDialogActive(){
    return s_DialogIsAktive;
  }
  static void setDialogActive( bool flag ){
    s_DialogIsAktive = flag;
  }
  static int defaultFieldLength(){
    return s_defaultFieldLength;
  }
  virtual void doFinalWork() {}
  virtual void doEndOfWork( bool error, bool updateForms=true );

protected:
  void registerIndex();
  void unregisterIndex();
  virtual void protect() {}
  virtual void EndOfWorkFinished() = 0;

 void protectField();

/*   // Die folgenden Funktionen arbeiten all mit den Atributen in m_attr_mask */

  bool getAttribute( DATAAttributeMask mask );
  void resetAttribute( DATAAttributeMask mask );
  const std::string& getStylesheet() { return m_stylesheet; };
  void setUpdated();
  void resetUpdated();
  bool isOptional() const;
  bool isTypeLabel() const;
  bool isValid() const;
  bool GuiUpdate() { return m_guiUpdate; }
public:
  bool useColorSet();
  bool colorBitIsSet();
  void setGuiUpdate(bool guiUpdate=true) { m_guiUpdate = guiUpdate; }
  static bool getColorsetColor(XferDataParameter& param, ColorSet* colorset, std::string &background, std::string &foreground );
protected:
  bool getColorsetColor( std::string &background, std::string &foreground );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void callConfirmationDialog(const std::string& msg);

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  class InsertFunctor : public XferEditDataItemFunctor
  {
  public:
    InsertFunctor() {}
    virtual ~InsertFunctor() {}
    virtual void operator() ( XferDataItemIndex &index, int inx, const DataReference &ref )
      { index.insertData( ref, inx ); }
  };

  class DeleteFunctor : public XferEditDataItemFunctor
  {
  public:
    DeleteFunctor() {}
    virtual ~DeleteFunctor() {}
    virtual void operator() ( XferDataItemIndex &index, int inx, const DataReference &ref )
      { index.deleteData( ref, inx ); }
  };

  class DuplicateFunctor : public XferEditDataItemFunctor
  {
  public:
    DuplicateFunctor() {}
    virtual ~DuplicateFunctor() {}
    virtual void operator() ( XferDataItemIndex &index, int inx, const DataReference &ref )
      { index.duplicateData( ref, inx ); }
  };

  class ClearFunctor : public XferEditDataItemFunctor
  {
  public:
    ClearFunctor() {}
    virtual ~ClearFunctor() {}
    virtual void operator() ( XferDataItemIndex &index, int inx, const DataReference &ref )
      { index.clearData( ref, inx ); }
  };

  class PackFunctor : public XferEditDataItemFunctor
  {
  public:
    PackFunctor() {}
    virtual ~PackFunctor() {}
    virtual void operator() ( XferDataItemIndex &index, int inx, const DataReference &ref )
      { index.packData( ref, inx ); }
  };

protected:
  class Trigger : public JobStarter
  {
  public:
    Trigger( GuiDataField *datafield, JobFunction *f )
      : JobStarter( f )
      , m_datafield( datafield )
      , m_updateForms(datafield->getFunction() ?
                      datafield->getFunction()->getUpdateForms() : f->getUpdateForms()){
    }
    virtual ~Trigger() {}
  protected:
    virtual void backFromJobStarter( JobAction::JobResult rslt );
    virtual void prepareToStartJob(){
      m_datafield->prepareToStartJob( this );
    }
  private:
    GuiDataField  *m_datafield;
    bool           m_updateForms;
  };

public:
  void prepareToStartJob( Trigger * );
/*=============================================================================*/
/* private TimerTask class with tick method                                    */
/*=============================================================================*/
private:
  class MyTimerTask : public TimerTask {
  public:
    enum TaskType{
      task_ConfirmationDialog,
      task_ProtectAction,
      task_None
    };
    MyTimerTask(GuiDataField* df)
      : m_datafield(df)
      , m_type(task_None){}
    void setType(TaskType type);
    void setMessage(const std::string& msg);
    void tick();
  private:
    GuiDataField *m_datafield;
    TaskType           m_type;
    std::string   m_message;
  };

/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
  XferDataParameter  *m_param;
  XferDataParameter  *m_paramInputStructFunc;
  XferDataParameter  *m_paramInputStructFocusFunc;
  DATAAttributeMask   m_attr_mask;
  std::string         m_stylesheet;
  ColorSet           *m_colorset;
  int                 m_running_key;
  bool                m_isInverted;
  bool                m_webApiAttrChanged;

  static bool         s_DialogIsAktive;
  static bool         s_TargetsAreCleared;
  static const int    s_defaultFieldLength;

  Timer              *m_timer;
  MyTimerTask          *m_task;

private:
  bool                m_disabled;
  bool                m_guiUpdate;
};

#endif
