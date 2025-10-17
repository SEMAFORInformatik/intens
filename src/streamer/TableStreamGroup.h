
#if !defined(STREAMER_TABLE_STREAM_GROUP_H)
#define STREAMER_TABLE_STREAM_GROUP_H

#include <string>

#include "streamer/StreamParameterGroup.h"
#include "streamer/IndexParameter.h"

class StreamSource;
class StreamDestination;

class TableStreamGroup: public StreamParameterGroup
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// constructs a TableStreamGroup object
  TableStreamGroup( char delimiter, StreamParameterGroup *parent=0 );

  /// Destructor
  virtual ~TableStreamGroup();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// adds an index column and returns the corresponding parameter
  virtual IndexParameter * addIndex( const std::string &ident, int length=0 );
  virtual StreamParameterGroup * findIndex( const std::string &indexname );
  virtual void addIndexedParameter( const std::string &indexname, DataStreamParameter *d );
  virtual void removeIndexedParameter( DataStreamParameter *d );
  virtual bool hasIndex() const { return m_index!=0; }
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );
  virtual void resetContinuousUpdate();
  virtual bool isValid(bool validErrorMsg=false);

  virtual std::string getName()const{ return "<TableStreamGroup>"; }

  virtual void putValues( StreamDestination *dest, bool transposed );
  virtual void getValues( StreamSource *src );
  virtual bool getInvalidItems( std::vector< std::string > &invaliditems );

  /** link data stream parameter with index
      @return true if parameter has variable index
  */
  bool linkWithIndex( StreamParameter *p );

  /** link all stream parameters of this table with their index
      @return true if at least one parameter has variable index
  */
  bool linkWithIndex();

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  class ResetIndex {
  public:
    void operator()( IndexParameter *ip ){
      if( ip != 0 ) ip -> resetIndex();
    }
  };

  IndexParameter     *m_index;
  bool                m_hasExplicitIndex;
  //  char                m_delimiter;
};

#endif
