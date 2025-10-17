
#if !defined(GUI_COMBOBOX_H)
#define GUI_COMBOBOX_H

class DataReference;
class XferDataParameter;
class XferDataItem;
class GuiDataField;
class GuiElement;
class Scale;

#include "xfer/XferParameter.h"
#include "gui/GuiElement.h"

class GuiComboBox
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiComboBox();
  GuiComboBox( GuiComboBox & );

  virtual ~GuiComboBox();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  XferDataItem *replaceDataItem( XferDataItem *dataitem );
  bool setScalefactor( Scale *scale );
  bool installDataItem( XferDataItem *dataitem );
  virtual XferDataParameter *param() = 0;
  void lock();
  void unlock();
  void switchlock();
  void serializeXML(std::ostream &os, bool recursive = false);
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  bool setSetName( const std::string &name );
  bool setInputValue();
  void FinalWork();
  virtual void setColors() = 0;
  void confirmYesButtonPressed();
  void confirmNoButtonPressed();
  void createPickList();
  bool useInput(){ return m_use_input; }
  ///not used?   void setCurrentIndex( int idx ){ m_currentIndex = idx; }
  /** JSF Corba:: get all visible Datapool Values */
  void getVisibleDataPoolValues( GuiValueList& vmap );
  TransactionNumber getPickList_timestamp() { return m_pickList_timestamp; }
  void setPickList_timestamp(TransactionNumber trans) { m_pickList_timestamp = trans; }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  XferParameter::InputStatus checkFormat();
  void FinalWorkOk();
  void FinalWorkCancel();
  virtual void fill( int n ) = 0;
  void setAlarmColors();
  virtual GuiDataField *datafield() = 0;
  virtual GuiElement *getElement() = 0;
  virtual bool created() = 0;
  void convertInput( std::string & );
  bool hasInvalidEntry() const;
  virtual void rebuildPickList() = 0;

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  bool             m_use_input;

protected:
  DataReference   *m_set_input;
  DataReference   *m_set_output;
  bool             m_reCreate;  // flag to prevent resetting dataref indices during create
  int              m_position;
  bool             m_alarmColor;

private:
  DataReference   *m_dataset;
  DataReference   *m_set_strings;
  //not used?  int              m_currentIndex;
  TransactionNumber m_pickList_timestamp;
};

#endif
