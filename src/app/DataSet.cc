
#include "app/DataSet.h"
#include "app/DataPoolIntens.h"
#include "app/UserAttr.h"
#include "datapool/DataValue.h"
#include "datapool/DataVector.h"
#include "datapool/DataReference.h"
#include "datapool/DataStringValue.h"
#include "datapool/DataRealValue.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataSet::DataSet()
  : m_type( DataDictionary::type_None )
  , m_func( 0 )
  , m_dataset( 0 )
  , m_input( 0 )
  , m_output( 0 )
  , m_strings( 0 )
  , m_invalid_entry( true ){
}

DataSet::~DataSet(){
  delete m_dataset;
  delete m_input;
  delete m_output;
  delete m_strings;
}

/*=============================================================================*/
/* Member Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* resetInvalidEntry --                                                        */
/* --------------------------------------------------------------------------- */

void DataSet::resetInvalidEntry(){
  m_invalid_entry = false;
}

/* --------------------------------------------------------------------------- */
/* searchEntry --                                                              */
/* --------------------------------------------------------------------------- */

bool DataSet::searchEntry( const std::string &name ){
  DataSetItemList::iterator si;
  for( si = m_list.begin(); si != m_list.end(); si++ ){
    if( (*si)->Name() == name ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setItemValues --                                                            */
/* --------------------------------------------------------------------------- */

bool DataSet::setItemValues(  const std::string &name, double value ){
  if( m_type == DataDictionary::type_None ){
    m_type = DataDictionary::type_Real;
  }
  else
  if( m_type != DataDictionary::type_Real ){
    return false;
  }
  if( searchEntry( name ) ){
    return false;
  }
  DataSetItem *dsi = new DataSetItem;
  dsi->setValue( name, value );
  m_list.push_back( dsi );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setItemValues --                                                            */
/* --------------------------------------------------------------------------- */

bool DataSet::setItemValues(  const std::string &name, const std::string &value ){
  if( m_type == DataDictionary::type_None ){
    m_type = DataDictionary::type_String;
  }
  else
  if( m_type != DataDictionary::type_String ){
    return false;
  }
  if( searchEntry( name ) ){
    return false;
  }
  DataSetItem *dsi = new DataSetItem;
  dsi->setValue( name, value );
  m_list.push_back( dsi );
  return true;
}

/* --------------------------------------------------------------------------- */
/* addCallback --                                                              */
/* --------------------------------------------------------------------------- */

void  DataSet::addCallback( DataSetCallbackProc func ){
  m_func = func;
}

/* --------------------------------------------------------------------------- */
/* DataType --                                                                 */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType DataSet::DataType(){
  return m_type;
}

/* --------------------------------------------------------------------------- */
/* getDataSetItemList --                                                       */
/* --------------------------------------------------------------------------- */

DataSetItemList &DataSet::getDataSetItemList(){
  return m_list;
}

/* --------------------------------------------------------------------------- */
/* getDataSetItemName --                                                       */
/* --------------------------------------------------------------------------- */

bool DataSet::getDataSetItemName( std::string &name, double d ){
  DataSetItemList::iterator si;
  for( si = m_list.begin(); si != m_list.end(); si++ ){
    if( (*si)->isEqual( d ) ){
      name = (*si)->Name();
      return true;
    }
      }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getDataSetItemName --                                                       */
/* --------------------------------------------------------------------------- */

bool DataSet::getDataSetItemName( std::string &name, const std::string &s ){
  DataSetItemList::iterator si;
  for( si = m_list.begin(); si != m_list.end(); si++ ){
    if( (*si)->isEqual( s ) ){
      name = (*si)->Name();
      return true;
    }
      }
  return false;
}
/* --------------------------------------------------------------------------- */
/* DataSetItemNameAvailable --                                                 */
/* --------------------------------------------------------------------------- */

bool DataSet::DataSetItemNameAvailable( const std::string &name ){
  return searchEntry( name );
}

/* --------------------------------------------------------------------------- */
/* CallbackAvailable --                                                        */
/* --------------------------------------------------------------------------- */

bool DataSet::CallbackAvailable(){
  return m_func != 0;
}

/* --------------------------------------------------------------------------- */
/* Callback --                                                                 */
/* --------------------------------------------------------------------------- */

DataSetCallbackProc DataSet::Callback(){
  return m_func;
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void DataSet::create( const std::string &name ){
  if (m_dataset && m_dataset->dictionary().ItemIsGlobal()) return;  // speed up
  m_dataset = DataPoolIntens::getDataReference( name );
  assert( m_dataset != 0 );

  m_input = DataPoolIntens::getDataReference( m_dataset, "Input" );
  assert( m_input != 0 );
  m_output = DataPoolIntens::getDataReference( m_dataset, "Output" );
  assert( m_output != 0 );
  m_strings = DataPoolIntens::getDataReference( m_dataset, "Strings" );
  assert( m_strings != 0 );

  if( !m_invalid_entry ){
    UserAttr *attr = static_cast<UserAttr*>( m_dataset->getUserAttr() );
    attr->unsetInvalidEntry();
  }
  DataVector dVecOut;
  DataVector dVecIn;
  int id[1] = {-1};
  DataSetItemList::iterator di;
  int n = 0;
  for( di = m_list.begin(); di != m_list.end(); ++di ){
    DataValue::Ptr ptr = new DataStringValue((*di)->Name());
    dVecIn.appendValue(ptr);
    if( DataType() == DataDictionary::type_Real ){
      DataValue::Ptr ptr = new DataRealValue((*di)->getReal());
      dVecOut.appendValue(ptr);
    }
    else{
      DataValue::Ptr ptr = new DataStringValue((*di)->getString());
      dVecOut.appendValue(ptr);
    }
    n++;
  }
  if( m_invalid_entry ){
    DataValue::Ptr ptr = new DataStringValue(" ");
    dVecIn.appendValue(ptr);
  }
  // setDataVector
  m_input->setDataVector(dVecIn, 1, id);
  m_output->setDataVector(dVecOut, 1, id);

  if( DataType() == DataDictionary::type_String ){
    m_strings->SetValue( 1 );
  }
}

/* --------------------------------------------------------------------------- */
/* isUpdated --                                                                */
/* --------------------------------------------------------------------------- */

bool DataSet::isUpdated( TransactionNumber timestamp ){
  if( m_dataset == 0 ){
    return false;
  }
  return m_dataset->isDataItemUpdated( DataReference::ValueUpdated, timestamp );
}

/* --------------------------------------------------------------------------- */
/* getInputValue --                                                            */
/* --------------------------------------------------------------------------- */

bool DataSet::getInputValue( std::string &val, int inx ){
  return getValue( val, inx, m_input );
}

/* --------------------------------------------------------------------------- */
/* getInputValue --                                                            */
/* --------------------------------------------------------------------------- */

bool DataSet::getInputValue( std::string &val, const DataReference &ref ){
  if( static_cast<UserAttr*>(ref.getUserAttr())->DataSetIndexed() ){
    const DataIndexList &inxlist = ref.getDimensionIndizes(-1);
    m_output->SetDimensionIndizes( 0, inxlist );
    m_input->SetDimensionIndizes( 0, inxlist );
  }
  int inx = getPosition( val, m_output );
  if( inx < 0 ){
    val = "";
    return false;
  }
  if( !getValue( val, inx, m_input ) ){
    val = "";
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getOutputValue --                                                           */
/* --------------------------------------------------------------------------- */

bool DataSet::getOutputValue( std::string &val, int inx ){
  updateOutput();
  return getValue( val, inx, m_output );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataSet::getValue( std::string &val, int inx, DataReference *ref ){
  if( ref->GetValue( val, inx ) ){
    return true;
  }
  val = " ";
  return false;
}

/* --------------------------------------------------------------------------- */
/* getInputPosition --                                                         */
/* --------------------------------------------------------------------------- */

int DataSet::getInputPosition( const std::string &value ){
  return getPosition( value, m_input );
}

/* --------------------------------------------------------------------------- */
/* getOutputPosition --                                                        */
/* --------------------------------------------------------------------------- */

int DataSet::getOutputPosition( const std::string &value ){
  updateOutput();
  return getPosition( value, m_output );
}

/* --------------------------------------------------------------------------- */
/* getPosition --                                                              */
/* --------------------------------------------------------------------------- */

int DataSet::getPosition( const std::string &value, DataReference *ref ){
  if( value.length() == 0 ){
    return -1;
  }
  int n = 0;
  std::string set_value;
  std::string::size_type idx = value.find_first_not_of( ' ' );
  if( idx == std::string::npos )
    return -1;
  std::string::size_type anz = value.find_last_not_of( ' ' ) - idx + 1;
  while( ref->GetValue( set_value, n ) ){
#if defined( __GNUC__ ) && ( __GNUC__ < 3 )
    if( value.compare( set_value, idx, anz ) == 0 ){
#else
    if( value.compare( idx, anz, set_value ) == 0 ){
#endif
      return n;
    }
    n++;
  }
  return -1;
}

/* --------------------------------------------------------------------------- */
/* updateOutput --                                                             */
/* --------------------------------------------------------------------------- */

void DataSet::updateOutput(){
  BUG( BugUndo, "updateOutput" );

  // --------------------------------------------------------------------------
  // Wir kontrollieren, ob der Output in Ordnung ist. Wenn der Output-Array
  // leer oder älter als der Input ist, wird er neu generiert.
  // --------------------------------------------------------------------------
  TransactionNumber transout = m_output->getDataItemValueUpdated();
  TransactionNumber transin  = m_input->getDataItemValueUpdated();

  if( transin > transout || !m_output->GetItemValid( 0 ) ){
    BUG_MSG("transin=" << transin << ", transout=" << transout );
    int n = 0;
    while( m_input->GetItemValid( n ) ){
      m_output->SetValue( n, n );
      n++;
    }
    m_output->SetItemInvalid( n );
    BUG_EXIT("Dataset recreated");
  }
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void DataSet::marshal( std::ostream &os ){
  std::cout<<"DataSet::marshal()"<<std::endl;
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable * DataSet::unmarshal( const std::string &element
				 , const XMLAttributeMap &attributeList )
{
  std::cout<<"DataSet::unmarshal()"<<std::endl;
  return 0;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void DataSet::setText( const std::string &text ){
}
