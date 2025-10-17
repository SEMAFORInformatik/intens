
#include "job/JobIncludes.h"

#include "job/JobDataReference.h"
#include "job/JobStackData.h"
#include "job/JobEngine.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataComplex.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataString.h"
#include "job/JobStackDataStructure.h"
#include "datapool/DataReference.h"
#include "streamer/JSONStreamParameter.h"
#include "gui/GuiManager.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "app/DataPoolIntens.h"
#include "app/DataAlterGetDbModified.h"
#include "app/DataAlterSetDbModified.h"
#include "app/ItemAttr.h"
#include "app/UserAttr.h"
#include "utils/Date.h"
#include "utils/JsonUtils.h"
#include "utils/gettext.h"

INIT_LOGGER();

#ifdef _DEBUG
int JobDataReference::s_ref_count = 0;
#endif

/*=============================================================================*/
/* Local Class Definitions                                                     */
/*=============================================================================*/

class JobDataCycleSwitch
{
public:
  JobDataCycleSwitch( JobDataReference *data );
  ~JobDataCycleSwitch();
private:
  JobDataReference  *m_data;
};

//#define JOB_CYCLE_SWITCH JobDataCycleSwitch cycleswitch = JobDataCycleSwitch( this )
#define JOB_CYCLE_SWITCH

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobDataReference::JobDataReference()
  : m_allow_wildcards( false )
  , m_wildcards( 0 )
  , m_data( 0 )
  , m_valid( false )
  , m_dataset( 0 )
  , m_isDataset( false )
  , m_stringType(UserAttr::string_kind_none) {
#ifdef _DEBUG
  s_ref_count++;
#endif
}

JobDataReference::JobDataReference( JobDataReference &ref )
  : m_allow_wildcards( ref.m_allow_wildcards )
  , m_wildcards( ref.m_wildcards )
  , m_index_list( ref.m_index_list )
  , m_data( ref.m_data )
  , m_valid( false )
  , m_dataset( ref.m_dataset )
  , m_isDataset( ref.m_isDataset )
  , m_stringType( ref.m_stringType) {
  ref.m_data = 0;
  ref.m_dataset = 0;
#ifdef _DEBUG
  s_ref_count++;
#endif
}

JobDataReference::~JobDataReference(){
  if( m_data != 0 ){
    delete m_data;
  }
#ifdef _DEBUG
  s_ref_count--;
#endif
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* JobDataCycleSwitch (Constructor) --                                         */
/* --------------------------------------------------------------------------- */

JobDataCycleSwitch::JobDataCycleSwitch( JobDataReference *data )
  : m_data( data ){
}

/* --------------------------------------------------------------------------- */
/* JobDataCycleSwitch (Destructor) --                                          */
/* --------------------------------------------------------------------------- */

JobDataCycleSwitch::~JobDataCycleSwitch(){
  assert( m_data != 0 );
  m_data->initCycleNumber();
}

/* --------------------------------------------------------------------------- */
/* getDataType --                                                              */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType JobDataReference::getDataType(){
  if( m_data == 0 ){
    return DataGlobals::type_None;
  }
  return m_data->getDataType();
}

/* --------------------------------------------------------------------------- */
/* getJobStackData --                                                          */
/* --------------------------------------------------------------------------- */

JobStackData *JobDataReference::getJobStackData(){
  BUG(BugJob,"JobDataReference::getJobStackData");

  JobStackData *item = 0;
  JOB_CYCLE_SWITCH;

  // Nur wenn die Indizes korrekt sind geht es weiter.
  if( !m_valid ){
    return new JobStackDataInteger();
  }

  if( !m_dataset && m_isDataset ){
    std::string dsname = static_cast<UserAttr*>(m_data->getUserAttr())->DataSetName();
    if( dsname.empty() ){
      m_isDataset = false;
    }
    else{
      m_dataset = DataPoolIntens::Instance().getDataSet( dsname );
    }
  }
  if( m_dataset ){
    std::string s;
    m_data->GetValue_PreInx( s );
    if( !m_dataset->getInputValue( s, *m_data ) ){
      s=""; // not found
    }
    return new JobStackDataString( s );
  }

  switch( m_data->getDataType() ){
  case DataDictionary::type_Integer:{
      int i;
      if( m_data->GetValue_PreInx( i ) ){
        item = new JobStackDataInteger( i );
        BUG_MSG("Integer Value " << i);
      }
    }
    break;

  case DataDictionary::type_Real:{
      double d;
      if( m_data->GetValue_PreInx( d ) ){
	switch (m_stringType) {
	case UserAttr::string_kind_date:
	case UserAttr::string_kind_time:
	case UserAttr::string_kind_datetime:
	  {
	    // specical case date string
	    std::string s = convertToISODate(d, m_stringType);
	    item = new JobStackDataString( s );
	    break;
	  }
	default:
	  item = new JobStackDataReal( d );
	  BUG_MSG("Real Value " << d);
	}
      }
    }
    break;

  case DataDictionary::type_Complex:{
      dComplex c;
      if( m_data->GetValue_PreInx( c ) ){
        item = new JobStackDataComplex( c );
        BUG_MSG("Complex Value " << c);
      }
    }
    break;

  case DataDictionary::type_String:
    {
      std::string s;
      if( m_data->GetValue_PreInx( s ) ){
        switch (m_stringType) {
        case UserAttr::string_kind_date:
        case UserAttr::string_kind_time:
        case UserAttr::string_kind_datetime:
          // specical case date string
          s = convertFromISODate(s, m_stringType);
          break;
        case UserAttr::string_kind_value:
          try {
            Json::Value valueObject = ch_semafor_intens::JsonUtils::parseJsonObjectComboBox(s);
            if ( valueObject.isObject() && valueObject.isMember("value") )
              s= valueObject["value"].isString() ? valueObject["value"].asString() : "";
            break;
          } catch ( const ch_semafor_intens::JsonException& e ) {
            // do nothing
          }
        default:
          break;
        }
        item = new JobStackDataString( s );
        BUG_MSG("String Value " << s);
      }
    }
    break;

  case DataDictionary::type_CharData:
    {
      std::string s;
      if( m_data->GetValue_PreInx( s ) ){
	item = new JobStackDataString( s );
	BUG_MSG("String Value type_CharData: " << s);
      }
    }
    break;

  case DataDictionary::type_StructVariable:
    {
      DataReference *data = DataPool::newDataReference( *m_data );
      item = new JobStackDataStructure( data );
      BUG_MSG("Struct Value");
    }
    break;

  default:
    BUG_MSG("unknown Data Type");
    break;
  }
  if( item == 0 ){
    item = new JobStackDataInteger();
  }
  return item;
}

/* --------------------------------------------------------------------------- */
/* assign --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::assign( JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobDataReference::assign");

  JOB_CYCLE_SWITCH;

  if( !m_valid ){
    BUG_EXIT("i am not valid");
    return op_Warning;
  }

  if( dat->isInvalid() ){
    std::string leftValue, rightValue("<INVALID>");
    m_data->GetValue_PreInx(leftValue);
    bool replaced = ch_semafor_intens::JsonUtils::assignJsonObjectComboBox(leftValue, rightValue, false);
    if ( replaced ) {
      m_data->SetValue_PreInx( leftValue );
    } else {
      if (!leftValue.empty())
        m_data->SetValue_PreInx(leftValue);  // nachfolgender Aufruf funktioniert dann wirklich
      m_data->SetItemInvalid_PreInx();
    }
    BUG_EXIT("Source DataReference is invalid");
    return op_Ok;
  }

  switch( m_data->getDataType() ){
  case DataDictionary::type_Integer:{
      int i;
      if( dat->getIntegerValue( i ) ){
        m_data->SetValue_PreInx( i );
        BUG_EXIT("Integer Value " << i << " set");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_Real:{
      double d;
      if( dat->getRealValue( d ) ){
        m_data->SetValue_PreInx( d );
        BUG_EXIT("Real Value " << d << " set");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_Complex:{
      dComplex c;
      if( dat->getComplexValue( c ) ){
        m_data->SetValue_PreInx( c );
        BUG_EXIT("Complex Value " << c << " set");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_CharData:
  case DataDictionary::type_String:{
      std::string s;
      if( dat->getStringValue( s ) ){
        std::string leftValue;
        m_data->GetValue_PreInx(leftValue);
        bool replaced = ch_semafor_intens::JsonUtils::assignJsonObjectComboBox(leftValue,s);
        assert ( replaced );
        m_data->SetValue_PreInx( leftValue );
        BUG_EXIT("String Value " << leftValue << " set");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_StructVariable:{
      DataReference *ref = 0;
      if( dat->getStructureValue( ref, m_data ) ){
	m_data->AssignDataElement( *ref );
	BUG_EXIT("Structure Value " << ref->fullName( true ) << " set");
	return op_Ok;
      }
      else {
        if (dat->getDataType() == DataDictionary::type_String) {
          std::string s;
          if( dat->getStringValue( s ) ){
            XferDataItem* xfer = new XferDataItem(DataReference::newDataReference(*m_data));
            JSONStreamParameter jsonStreamParameter(xfer);
            bool parsingSuccessful =  jsonStreamParameter.readString(s);
            delete xfer;
            return parsingSuccessful ? op_Ok : op_Warning;
          }
        }
        return op_Ok;
      }
      m_data->clearAllElements();
      BUG_EXIT("item cleared");
      return op_Ok;
    }
    break;

  default:
    BUG_MSG("unknown Data Type");
    break;
  }
  BUG_EXIT("set item invalid");
  m_data->SetItemInvalid_PreInx();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* assignCorresponding --                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::assignCorresponding( JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobDataReference::assignCorresponding");

  JOB_CYCLE_SWITCH;

  if( !m_valid ){
    return op_Warning;
  }

  switch( m_data->getDataType() ){
  case DataDictionary::type_StructVariable:{
      DataReference *ref = 0;
      if( dat->getStructureValue( ref ) ){
        // list assign
        if (m_data->numberOfWildcards() == 1 &&
            m_data->numberOfWildcards() == ref->numberOfWildcards()) {
          XferDataItem* xferL = new XferDataItem(DataReference::newDataReference(*m_data));
          XferDataItem* xferR = new XferDataItem(DataReference::newDataReference(*ref));
          XferDataItemIndex *indexL = xferL->newDataItemIndex();
          XferDataItemIndex *indexR = xferR->newDataItemIndex();
          for (int i=0; i<indexR->getDimensionSize(ref); ++i) {
            indexL->setIndex(xferL->Data(), i);
            xferL->setDimensionIndizes();
            indexR->setIndex(xferR->Data(), i);
            xferR->setDimensionIndizes();
            BUG_DEBUG("List Assign_Corr: " <<i << ". " << xferL->getFullName(true)
                      << " << " << xferR->getFullName(true));
            xferL->Data()->AssignCorrespondingDataItem( *xferR->Data() );
          }
          delete xferL;
          delete xferR;
        }else{
          // scalar assign
          m_data->AssignCorrespondingDataItem( *ref );
        }
        BUG_EXIT("Structure Value " << ref->fullName( true ) << " set");
        delete ref;
        return op_Ok;
      }
      BUG_EXIT("getStructureValue failed");
    }
    break;

  default:
    BUG_MSG("unknown Data Type");
    break;
  }
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* getTimestamp --                                                             */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::getTimestamp( JobEngine *eng ){
  if( !m_valid ){
    eng->pushInvalid();
    return op_Warning;
  }
  eng->push( new JobStackDataInteger( m_data->getDataTimestamp( DataReference::ValueUpdated ) ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* getDbModified --                                                            */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::getDbModified( JobEngine *eng ){
  if( !m_valid ){
    eng->pushInvalid();
    return op_Warning;
  }
  DataAlterGetDbModified modified;
  m_data->alterData( modified );
  eng->push( new JobStackDataInteger( modified.getModified() ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* increment --                                                                */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::increment(){
  BUG(BugJob,"JobDataReference::increment");

  JOB_CYCLE_SWITCH;

  if( !m_valid ){
    return op_Warning;
  }

  switch( m_data->getDataType() ){
  case DataDictionary::type_Integer:{
      int i;
      if( m_data->GetValue_PreInx( i ) ){
        m_data->SetValue_PreInx( i + 1 );
        BUG_MSG("Integer Value " << i+1 << " set");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_Real:{
      double d;
      if( m_data->GetValue_PreInx( d ) ){
        m_data->SetValue_PreInx( d + 1.0 );
        BUG_MSG("Real Value " << d+1 << " set");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_Complex:{
      dComplex c;
      if( m_data->GetValue_PreInx( c ) ){
        m_data->SetValue_PreInx( dComplex( c.real()+1, c.imag() ) );
        BUG_MSG("Complex Value incremented");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_String:
    BUG_MSG("String Value");
    break;

  case DataDictionary::type_StructVariable:
    BUG_MSG("StructVariable");
    break;

  default:
    BUG_MSG("unknown Data Type");
    break;
  }
  m_data->SetItemInvalid_PreInx();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* decrement --                                                                */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::decrement(){
  BUG(BugJob,"JobDataReference::decrement");

  JOB_CYCLE_SWITCH;

  if( !m_valid ){
    return op_Warning;
  }

  switch( m_data->getDataType() ){
  case DataDictionary::type_Integer:{
      int i;
      if( m_data->GetValue_PreInx( i ) ){
        m_data->SetValue_PreInx( i - 1 );
        BUG_MSG("Integer Value " << i-1 << " set");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_Real:{
      double d;
      if( m_data->GetValue_PreInx( d ) ){
        m_data->SetValue_PreInx( d - 1.0 );
        BUG_MSG("Real Value " << d-1 << " set");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_Complex:{
      dComplex c;
      if( m_data->GetValue_PreInx( c ) ){
        m_data->SetValue_PreInx(  dComplex( c.real()-1, c.imag() ) );
        BUG_MSG("Complex Value decremented");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_String:
    BUG_MSG("String Value");
    break;

  case DataDictionary::type_StructVariable:
    BUG_MSG("StructVariable");
    break;

  default:
    BUG_MSG("unknown Data Type");
    break;
  }
  m_data->SetItemInvalid_PreInx();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* setEditable --                                                              */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::setEditable( JobEngine *eng, bool yes ){
  BUG(BugJob,"JobDataReference::setEditable");

  JOB_CYCLE_SWITCH;

  if( !m_valid ){
    return op_Warning;
  }

  if( yes ){
    m_data->setDataItemAttr( DATAIsEditable, DATAIsReadOnly );
  }
  else{
    m_data->setDataItemAttr( DATAIsReadOnly, DATAIsEditable );
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* setLocked --                                                                */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::setLocked( JobEngine *eng, bool yes ){
  BUG(BugJob,"JobDataReference::setLocked");

  JOB_CYCLE_SWITCH;

  if( !m_valid ){
    return op_Warning;
  }

  if( yes ){
    m_data->setAttributes( DATAisLocked, 0 );
  }
  else{
    m_data->setAttributes( 0, DATAisLocked );
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* setColorBit                                                                 */
/* --------------------------------------------------------------------------- */

DATAAttributeMask JobDataReference::setColorBit( int color ){
  BUG_PARA(BugJob,"JobDataReference::setColorBit","color " << color );
  DATAAttributeMask set_mask = 0;

  bool isColorBit = static_cast<UserAttr*>(m_data->getUserAttr())->IsColorBit();
  if (!isColorBit) {
    if (color&1) set_mask = set_mask|DATAcolor1;
    if (color&2) set_mask = set_mask|DATAcolor2;
    if (color&4) set_mask = set_mask|DATAcolor3;
    if (color&8) set_mask = set_mask|DATAcolor4;
    if (color&16) set_mask = set_mask|DATAcolor5;
    if (color&32) set_mask = set_mask|DATAcolor6;
    if (color&64) set_mask = set_mask|DATAcolor7;
    if (color&128) set_mask = set_mask|DATAcolor8;
    BUG_MSG("NO_COLOR_BIT get color number '"<< color << "'");
    return set_mask;
  }
  switch( color ){
  case 1:
    set_mask = DATAcolor1;
    break;
  case 2:
    set_mask = DATAcolor2;
    break;
  case 3:
    set_mask = DATAcolor3;
    break;
  case 4:
    set_mask = DATAcolor4;
    break;
  case 5:
    set_mask = DATAcolor5;
    break;
  case 6:
    set_mask = DATAcolor6;
    break;
  case 7:
    set_mask = DATAcolor7;
    break;
  case 8:
    set_mask = DATAcolor8;
    break;
  default:
    break;
  }
  return set_mask;
}

/* --------------------------------------------------------------------------- */
/* setColor --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::setColor(  JobEngine *eng, int color ){
  BUG(BugJob,"JobDataReference::setColor");

  JOB_CYCLE_SWITCH;

  if( !m_valid ){
    return op_Warning;
  }

  DATAAttributeMask set_mask = setColorBit( color );
  DATAAttributeMask reset_mask = DATAcolorAll;

  reset_mask = ( reset_mask | set_mask ) ^ set_mask;
  m_data->setDataItemAttr( set_mask, reset_mask );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* setColorBit --                                                              */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::setColorBit(  JobEngine *eng, int color, bool unset ){
  BUG(BugJob,"JobDataReference::setColorBit");

  JOB_CYCLE_SWITCH;

  if( !m_valid ){
    return op_Warning;
  }

  DATAAttributeMask mask = setColorBit( std::min(color, 8) );
  if( unset ){
    m_data->setDataItemAttr( 0, mask );
  }
  else{
    m_data->setDataItemAttr( mask, 0 );
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* setStylesheet --                                                            */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::setStylesheet(JobEngine *eng, const std::string& stylesheet){
  BUG(BugJob,"JobDataReference::setStylesheet");

  JOB_CYCLE_SWITCH;

  if( !m_valid ){
    return op_Warning;
  }
  m_data->setStylesheet(stylesheet);
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* setDataTimestamp --                                                         */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::setDataTimestamp( JobEngine *eng, bool yes ){
  JOB_CYCLE_SWITCH;

  if( !m_valid ){
    return op_Warning;
  }
  if( m_data->setDataTimestamp( yes, DataGlobals::DataUpdated ) ){
    if( !yes ){
      GuiManager::Instance().setUpdateAlways();
    }
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* setValueTimestamp --                                                        */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::setValueTimestamp( JobEngine *eng, bool yes ){
  JOB_CYCLE_SWITCH;

  if( !m_valid ){
    return op_Warning;
  }
  if( m_data->setDataTimestamp( yes, DataGlobals::ValueUpdated ) ){
    if( !yes ){
      GuiManager::Instance().setUpdateAlways();
    }
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* setDbTimestamp --                                                           */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::setDbTimestamp( JobEngine *eng ){
  if( !m_valid ){
    return op_Warning;
  }
  DataAlterSetDbModified modified;
  m_data->alterData( modified );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void JobDataReference::setValue( int i, int inx ){
  JOB_CYCLE_SWITCH;

  if( m_valid ){
    m_data->SetValue( i, inx );
  }
}

/* --------------------------------------------------------------------------- */
/* setRealValue --                                                             */
/* --------------------------------------------------------------------------- */

void JobDataReference::setRealValue( double d, int inx ){
  JOB_CYCLE_SWITCH;

  if( m_valid ){
    m_data->SetValue( d, inx );
  }
}

/* --------------------------------------------------------------------------- */
/* setStringValue --                                                           */
/* --------------------------------------------------------------------------- */

void JobDataReference::setStringValue( const std::string& s, int inx ){
  JOB_CYCLE_SWITCH;

  if( m_valid ){
    m_data->SetValue( s, inx );
  }
}

/* --------------------------------------------------------------------------- */
/* clearValues --                                                              */
/* --------------------------------------------------------------------------- */

bool JobDataReference::clearValues(){
  JOB_CYCLE_SWITCH;

  if( m_valid ){
    // Wildcards auf allen Ebenen werden berücksichtigt. Es wird mit einem
    // DataAlter-Functor gearbeitet.
    m_data->clearDataItem();
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* clearAllValues --                                                           */
/* --------------------------------------------------------------------------- */

bool JobDataReference::clearAllValues(){
  JOB_CYCLE_SWITCH;

  if( m_valid ){
    if( m_data->hasWildcardsNotLastLevel() ){
      return false;
    }
    m_data->ClearDimensionIndizes();
    m_data->clearDataItem();
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* eraseValues --                                                              */
/* --------------------------------------------------------------------------- */

bool JobDataReference::eraseValues(){
  JOB_CYCLE_SWITCH;

  if( m_valid ){
    // Wildcards auf allen Ebenen werden berücksichtigt. Es wird mit einem
    // DataAlter-Functor gearbeitet.
    m_data->eraseAllElements( true );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* packValues --                                                               */
/* --------------------------------------------------------------------------- */

bool JobDataReference::packValues( bool packRow ){
  JOB_CYCLE_SWITCH;

  if( m_valid ){
    if( m_data->hasWildcardsNotLastLevel() ){
      return false;
    }
    m_data->packDataItem( packRow );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool JobDataReference::isValid(){
  JOB_CYCLE_SWITCH;

  if( m_valid ){
    return m_data->GetItemValid_PreInx();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* isInvalid --                                                                */
/* --------------------------------------------------------------------------- */

bool JobDataReference::isInvalid(){
  JOB_CYCLE_SWITCH;

  if( m_valid ){
    return !m_data->GetItemValid_PreInx();
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* newDataReferenceInNamespace --                                              */
/* --------------------------------------------------------------------------- */

bool JobDataReference::newDataReferenceInNamespace( const std::string &name, bool dataset ){
  m_isDataset = dataset;
  assert( m_data == 0 );
  m_data = DataPoolIntens::Instance().newDataReferenceInNamespace( name );
  return (m_data != 0);
}

/* --------------------------------------------------------------------------- */
/* newDataReference --                                                         */
/* --------------------------------------------------------------------------- */

bool JobDataReference::newDataReference( const std::string &name, bool dataset ){
  m_isDataset = dataset;
  assert( m_data == 0 );
  m_data = DataPoolIntens::getDataReference( name );
  return (m_data != 0);
}

bool JobDataReference::newDataReference( DataReference *dref ){
  assert( m_data == 0 );
  m_data = dref;
  return (m_data != 0);
}

/* --------------------------------------------------------------------------- */
/* extendDataReference --                                                      */
/* --------------------------------------------------------------------------- */

bool JobDataReference::extendDataReference( const std::string &name ){
  assert( m_data != 0 );
  DataReference *ref = DataPoolIntens::getDataReference( m_data, name );
  if( ref == 0 ){
    return false;
  }
  delete m_data;
  m_data = ref;
  return true;
}

/* --------------------------------------------------------------------------- */
/* addIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void JobDataReference::addIndex( int level ) {
  m_index_list.push_back( level );
}

/* --------------------------------------------------------------------------- */
/* addWildcard --                                                              */
/* --------------------------------------------------------------------------- */

void JobDataReference::addWildcard( int level ) {
  m_index_list.push_back( level );
  m_wildcards++;
}

/* --------------------------------------------------------------------------- */
/* addVariableName --                                                          */
/* --------------------------------------------------------------------------- */

void JobDataReference::addVariableName() {
  m_index_list.push_back( -1 );
}

/* --------------------------------------------------------------------------- */
/* updateDataReference --                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::updateDataReference( JobEngine *eng ){
  BUG(BugJob,"JobDataReference::updateDataReference");
  IndexList::iterator i;
  JobStackData *dat = 0;
  int index = 0, level = 0, value = 0;
  m_valid = true;
  IndexList  indices;

  for( int x = 0; x < m_index_list.size(); ++x ){
    JobStackDataPtr dat( eng->pop() );
    if( dat.isnt_valid() ) return op_FatalError;

    if( !dat->getIntegerValue( value ) ){
      m_valid = false; // ungültiger Wert
    }
    else{
      if( value < 0 && value != -1 ){
	m_valid = false; // negativer Wert
      }
      else{
	indices.push_back( value );
      }
    }
  }

  if( !m_valid ) return op_Ok;

  IndexList::reverse_iterator ri = indices.rbegin();
  for( i = m_index_list.begin(); i != m_index_list.end(); ++i ){
    if( level != (*i) ){
      level = (*i);
      index = 0;
    }
    BUG_MSG("SetDimensionIndexOfLevel( " << (*ri) << " , " << level << " , " << index << " )" );
    m_data->SetDimensionIndexOfLevel( (*ri), level, index );
    index++;
    ++ri;
  }

#if HAVE_LOG4CPLUS
  if (__logger__.isEnabledFor(log4cplus::DEBUG_LOG_LEVEL)) {
    std::string name;
    m_data->getFullName( name, true );
    if( m_data->GetItemValid_PreInx() )
      name += " is valid";
    else
      name += " is NOT valid";
    BUG_DEBUG( name );
  }
#endif
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* size --                                                                     */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::size( JobEngine *eng, JobStackDataPtr &data ){
  // Der Bereich für die Sizeliste (Resultat) wird immer zuerst geloescht.
  data->clearAllValues();

  JOB_CYCLE_SWITCH;

  if( !m_valid ){
    return op_Warning;
  }
  if( m_data->hasWildcardsNotLastLevel() ){
    return op_Warning;
  }

  DataContainer::SizeList dimsize;
  // Dies ist die neue Variante (4.10.2015/bh)
  int numDims = m_data->getAllDataDimensionSize_PreInx( dimsize );
  assert( numDims > 0 && numDims <= dimsize.size() );

  // #655 wegen Rückwärtskompatbilität zum alten intens-DatenPool (< changeset 22xx)
  // SIZE-Aufruf ohne wildcards werden wieder alle führenden "1" gelöscht.
  // (Wegen der vielen Bombardier-Anwendungen)
  int j=0;
  bool hide_trivial_dims = !m_data->hasWildcards();
  for( int i = 0; i < numDims; i++ ){
    if ( !(hide_trivial_dims && dimsize[i] == 1 && i < (numDims-1)) ) {
      hide_trivial_dims = false;
      data->setValue( dimsize[i], j++ );
    }
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* index --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::index( JobEngine *eng, JobStackDataPtr &data ){
  // Der Bereich für die Sizeliste (Resultat) wird immer zuerst geloescht.
  data->clearAllValues();

  JOB_CYCLE_SWITCH;

  int numIdx =  m_data->GetNumberOfDimensionIndizes();
  for (int i =0; i< numIdx; ++i) {
    data->setValue( m_data->GetDimensionIndexOfLevel( -1, i ), i);
  }

  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::compare( JobEngine *eng, JobDataReference *data ){
  BUG_PARA(BugJob,"JobDataReference::compare", "inx=" << m_data->fullName(true) );
  JOB_CYCLE_SWITCH;

  // cycle compare of one JobDataReference(s)
  if (!m_data || !data) {
    return op_Warning;
  }
  m_data->compareElements(  *(data->m_data), !data->m_data->hasWildcards() );

  // add cycle names as tags
  dynamic_cast<UserAttr*>(m_data->getUserAttr())->clearTags();
  DataPoolIntens& dpi = DataPoolIntens::Instance();
  for (int num=0; num < dpi.numCycles(); ++num) {
    std::string cn;
    dpi.getCycleName(num, cn);
    dynamic_cast<UserAttr*>(m_data->getUserAttr())->setTag(cn);
  }

  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::compare( JobEngine *eng
					      , JobDataReference *dataLeft
					      , JobDataReference *dataRight )
{
  BUG_PARA(BugJob,"JobDataReference::compare", "inx=" << m_data->fullName(true) );
  JOB_CYCLE_SWITCH;

  // compare between two JobDataReference(s)
  if (dataLeft && dataRight) {
    // left right compare
    m_data->compareElements( *(dataLeft->m_data), *(dataRight->m_data) );
  }
  else{
    return op_Warning;
  }

  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* setCycleNumber --                                                           */
/* --------------------------------------------------------------------------- */

void JobDataReference::setCycleNumber( int inx ){
  BUG_PARA(BugJob,"JobDataReference::setCycleNumber", "inx=" << inx );
  if( m_data == 0 ){
    BUG_EXIT("no DataReference available");
    return;
  }
  // Intern wird die Cyclenummer 0-relativ verwendet.
  inx--;

  if( inx < 0 ){
    m_valid = false;
    m_data->initCycleNumber();
    BUG_EXIT("set invalid");
  }
  else{
    m_data->setCycleNumber( inx );
    BUG_EXIT("set to " << inx );
  }
}

/* --------------------------------------------------------------------------- */
/* initCycleNumber --                                                          */
/* --------------------------------------------------------------------------- */

void JobDataReference::initCycleNumber(){
  if( m_data == 0 ){
    return;
  }
  m_data->initCycleNumber();
}

/* --------------------------------------------------------------------------- */
/* getUnit --                                                                  */
/* --------------------------------------------------------------------------- */

bool JobDataReference::getUnit( std::string & units ){
  if( m_data == 0 )
    return false;
  DataDictionary *dict = m_data -> GetDict();
  if( dict == 0 )
    return false;
  UserAttr *attr = static_cast<UserAttr*>( dict -> GetAttr() );
  if( attr == 0 )
    return false;

  units = attr -> Unit(false);
  return true;
}

/* --------------------------------------------------------------------------- */
/* getLabel --                                                                 */
/* --------------------------------------------------------------------------- */

bool JobDataReference::getLabel( std::string & label ){
  if( m_data == 0 )
    return false;
  DataDictionary *dict = m_data -> GetDict();
  if( dict == 0 )
    return false;
  UserAttr *attr = static_cast<UserAttr*>( dict -> GetAttr() );
  if( attr == 0 )
    return false;

  label = attr -> Label( m_data );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getClassname --                                                             */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::getClassname( JobEngine *eng ){
  if( !m_valid ) {
    eng->push( new JobStackDataString("") );
    return op_Warning;
  }

  if( m_data == 0 ) {
    eng->push( new JobStackDataString("") );
    return op_Warning;
  }
  DataDictionary *dict = m_data -> GetDict();
  if( dict == 0 ) {
    eng->push( new JobStackDataString("") );
    return op_Warning;
  }
  UserAttr *attr = static_cast<UserAttr*>( dict -> GetAttr() );
  if( attr == 0 ) {
    eng->push( new JobStackDataString("") );
    return op_Warning;
  }

  //  classname = attr -> Classname();
  eng->push( new JobStackDataString( attr -> Classname() ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* getNodename --                                                              */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobDataReference::getNodename( JobEngine *eng ){
  //  nodename
  if( m_data == 0 || !m_valid ){
    eng->push( new JobStackDataString("<invalid>") );
    return op_Warning;
  }
  else{
    eng->push( new JobStackDataString( m_data -> nodeName() ) );
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* printFullName --                                                            */
/* --------------------------------------------------------------------------- */

void JobDataReference::printFullName( std::ostream &ostr, bool withIndizes ){
  if( m_data == 0 ){
    ostr << "<invalid>";
  }
  else{
    m_data->printFullName( ostr, withIndizes );
  }
}

/* --------------------------------------------------------------------------- */
/* setStringType --                                                            */
/* --------------------------------------------------------------------------- */

void JobDataReference::setStringType(UserAttr::STRINGtype type) {
  m_stringType = type;
}

/* --------------------------------------------------------------------------- */
/* getRefCount --                                                              */
/* --------------------------------------------------------------------------- */

int JobDataReference::getRefCount(){
#if defined(_DEBUG)
  return s_ref_count;
#else
  return 0;
#endif
}
