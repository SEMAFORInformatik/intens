
#if !defined(STREAMER_DATA_STREAM_PARAMETER_H)
#define STREAMER_DATA_STREAM_PARAMETER_H

#include <vector>
#include <string>

#include "streamer/StreamParameter.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferConverter.h"
#include "datapool/DataContainer.h"

class StructStreamParameter;
class StreamParameterGroup;
class DataReference;

class DataStreamParameter: public StreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  DataStreamParameter( XferDataItem *ditem,
                       int level,
                       StructStreamParameter *p,
                       bool isMandatory=false,
                       char delimiter=' ',
                       bool locale=false );

  DataStreamParameter( const DataStreamParameter &d );

  virtual ~DataStreamParameter();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element,
			   const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

  virtual void setTargetStream( BasicStream * );

  virtual void clearRange( bool incl_itemattr=false );
  virtual void fixupItemStreams( BasicStream &stream );

  /// returns dimension size
  virtual size_t getDimensionSize(int ndim) const;

  virtual void setValue( const std::string &v );
  virtual bool getValue( std::string &v )const;

  virtual void setValue( double v );
  virtual bool getValue( double &v )const;

  virtual void setValue( const dComplex &v );
  virtual bool getValue( dComplex &v )const;

  virtual void setValue( int v );
  virtual bool getValue( int &v )const;

  virtual bool setValue( const std::vector<int> &dims
			 , const std::vector<int>& vals, int level=-1 );
  virtual bool setValue( const std::vector<int> &dims
			 , const std::vector<double>& revals
			 , const std::vector<double>& imvals
			 , const std::vector<int> &inds, int level =-1 );
  virtual bool setValue( const std::vector<int> &dims
			 , const std::vector<dComplex>& cvals, int level =-1 );
  virtual bool setValue( const std::vector<int> &dims
			 , const std::vector<std::string>& svals, int level=-1 );

  bool newDataItemIndex( int level, const std::string &name, int value );
  virtual void setIndex( const std::string &name, int index );
  virtual void setIndex( int pos, int v );
  void setDimensionIndizes(int level);
  const DataReference &data();
  virtual bool isValid(bool validErrorMsg=false);
  virtual std::string getName() const;
  virtual std::string getFullName( bool withIndexes ) const;

  virtual void getValues( StreamSource *src );

  /** Die Funktion setzt bei allen verfügbaren, nicht transienten Items den
      DbTimestamp, damit die Werte 'not modified' sind. Diese Aktion ist nur
      nach dem Laden von Komponenten aus der Datenbank sinnvoll.
  */
  virtual void setDbItemsNotModified();

  /** Diese Funktion setzt den Datenbank-Timestamp eines Items. Mit diesem
      Timestamp wird festgehalten, dass die Werte des Items von der Datenbank
      eingelesen wurden.
  */
  void setDbItemTimestamp( TransactionNumber trans );

  /** Mit dieser Funktion wird geprüft, ob Werte des Items seit dem Einlesen
      von der Datenbank verändert wurden. Ist kein Timestamp gesetzt worden
      (siehe setDbItemTimestamp() ), so wird als Resultat false zurückgegeben.
  */
  bool isDbItemUpdated() const;

  /** Mit dieser Funktion wird geprüft, ob ein Wert des Items seit dem Einlesen
      von der Datenbank verändert wurden. Ist kein Timestamp gesetzt worden
      (siehe setDbItemTimestamp() ), so wird als Resultat false zurückgegeben.
  */
  bool isDbValueUpdated() const;

  virtual const std::string getLabel() const;
  XferDataItem *DataItem() const;
  int getLevel() const;

  /// returns true if attribute is scalar
  bool isScalar() const;
  /// returns true if attribute is cell
  bool isCell() const;
  /// returns true if attribute is hidden (means do not exchange data with Matlab)
  bool isHidden() const;
  /// creates indexes and sets upper and lower limits for all dims[i] > -1
  void createIndexes( const std::vector<int> &dims );
  /// erases indexes
  void eraseIndexes();

  void setScalar( bool is_scalar );
  void setCell( bool is_cell );
  /// returns true if parameter must be valid
  bool isMandatory() const;
  void setMandatory();
  bool isLocale() const;
  void setLocale(bool locale);
  virtual bool getInvalidItems( std::vector< std::string > &invaliditems );
  int getFirstIndexNum() const;
  /// return number of indexes
  virtual int getNumIndexes() const;
  /// return index name of index
  virtual std::string getIndexName( int i ) const;

  /** unlink data stream parameter with index
      @return true if parameter has variable index
  */
  void unlinkIndex( StreamParameterGroup *group );

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  int createDataItemIndexes();
  void removeDataItemIndexes();
  void setDataItemIndexes();
  void setLevel( int level );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* protected data                                                              */
/*=============================================================================*/
 protected:
  XferDataItem          *m_dataitem;
  int                    m_indexNum;
  StructStreamParameter *m_parent;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  int                    m_level;
  bool                   m_isScalar;
  bool                   m_isCell;
  bool                   m_isMandatory;
  bool                   m_locale;
};







#endif
