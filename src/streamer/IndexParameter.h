
#if !defined(STREAMER_INDEX_PARAMETER_H)
#define STREAMER_INDEX_PARAMETER_H

#include <vector>
#include <string>

#include "streamer/StreamParameter.h"
#include "xfer/XferConverter.h"

class XferDataItemIndex;
class DataStreamParameter;

/** IndexParameter
    Instances of this class can be used to control the index value
    of one or more client parameters
    @version $Id: IndexParameter.h,v 1.14 2005/02/01 14:46:42 tar Exp $
*/
class IndexParameter: public StreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /**@name methods */
  //@{
  /** construct a IndexParameter instance identified by name
      @param name index identifier
      @param width field width for formatted input/output
  */
  IndexParameter( const std::string &name, int width ):
    m_name(name)
    , m_conv(width, 0, false )
    , m_pos(0){}
  /** construct a IndexParameter instance identified by position
      @param name index identifier
      @param width field width for formatted input/output
  */
  IndexParameter( int pos, int width=0 ):
    m_pos(pos)
    , m_conv( width, 0, false ){}
  /// Destructor
  virtual ~IndexParameter(){}
  /** resets index value
   */

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void resetIndex(){ setValue(0); }
  /** increments the index value
   */
  virtual int inc();
  /** reads an integer value from istream
      @param istream
   */
  virtual bool read( std::istream & );
  /** writes the current index value to outstream
      @param ostream
   */
  virtual bool write( std::ostream & );
  /** puts a data parameter under index control
      @param d data stream parameter
   */
  void addIndexedParameter( DataStreamParameter *d );
  /** removes all data parameter under index control
      @param d data stream parameter
   */
  void removeIndexedParameter( DataStreamParameter *d );
  /** returns the greatest common size of all clients
   */
  virtual size_t getSize()const;
  virtual size_t getMaxIndex() const;
  /** sets an index identifier
      @param name identifier
   */
  void setName( const std::string &name ){ m_name=name; }
  /** sets a position
      @param pos new position
  */
  void setPosition( int pos ) { m_pos=pos; m_name=""; }
  virtual void fixupItemStreams( BasicStream &stream );
  virtual void setValue( const std::string &v );
  virtual bool getValue( std::string &v )const;
  virtual void setValue( int v );
  virtual bool getValue( int &v )const{ v=m_val;return true; }
  virtual std::string getName()const{ return m_name; }
  virtual void putValues( StreamDestination *dest, bool transposed );
  virtual void getValues( StreamSource *src );
  bool isEmpty(){ return m_params.empty();}
  virtual bool isValid(bool validErrorMsg=false);
  // void clearParameters();
  //@}

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  typedef std::vector<DataStreamParameter*> DataStreamParameterVector;

  int                       m_val;
  int                       m_pos;
  std::string               m_name;
  IntConverter              m_conv;
  DataStreamParameterVector m_params;
};

#endif
