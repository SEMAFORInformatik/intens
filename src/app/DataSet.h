
#if !defined(DATASET_INCLUDED_H)
#define DATASET_INCLUDED_H

typedef void (*DataSetCallbackProc)( void * );

#include "app/DataSetItem.h"
#include "app/LSPItem.h"
#include "datapool/DataPool.h"
#include "datapool/DataDictionary.h"

#include "xml/Serializable.h"

class DataReference;
class DataIndexList;

typedef std::vector<DataSetItem *> DataSetItemList;

class DataSet: public Serializable, public LSPItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  DataSet();
  virtual ~DataSet();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void resetInvalidEntry();
  DataReference *strings(){return m_strings;}
  bool setItemValues( const std::string &, double );
  bool setItemValues( const std::string &, const std::string& );
  void addCallback( DataSetCallbackProc );
  DataDictionary::DataType DataType();
  DataSetItemList &getDataSetItemList();
  bool getDataSetItemName( std::string &, double );
  bool getDataSetItemName( std::string &, const std::string & );
  bool DataSetItemNameAvailable( const std::string & );
  bool CallbackAvailable();
  DataSetCallbackProc Callback();

  void create( const std::string &name );
  bool isUpdated( TransactionNumber timestamp );
  bool getInputValue( std::string &val, int inx );
  bool getInputValue( std::string &val, const DataReference &ref );
  bool getOutputValue( std::string &val, int inx );
  int getInputPosition( const std::string &value );
  int getOutputPosition( const std::string &value );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  DataSet(const DataSet&);
  bool searchEntry( const std::string & );
  bool getValue( std::string &val, int inx, DataReference *ref );
  int getPosition( const std::string &value, DataReference *ref );
  void updateOutput();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  DataDictionary::DataType   m_type;
  DataSetItemList            m_list;
  DataSetCallbackProc        m_func;

  DataReference             *m_dataset;
  DataReference             *m_input;
  DataReference             *m_output;
  DataReference             *m_strings;

  bool                       m_invalid_entry;
};

#endif
