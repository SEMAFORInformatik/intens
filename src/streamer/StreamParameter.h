
#ifndef STREAMER_STREAM_PARAMETER_H
#define STREAMER_STREAM_PARAMETER_H

#include <vector>
#include <string>
#ifdef __MINGW32__
#include <fstream>
#endif
#ifndef __MINGW32__
#include <qfile.h>
#include <qtextstream.h>
#endif

#include "datapool/DataReference.h"
#include "xml/Serializable.h"
#include "gui/GuiIndexListener.h"
#include "app/HardCopyListener.h"
#include "app/UserAttr.h"

class DataStreamParameter;
class StreamParameter;
class StructStreamParameter;
class JSONStreamParameter;
class BasicStream;
class XferDataItem;

/** StreamDestination
    The abstract interface class StreamDestination contains
    the required functions for a data transfer operation
    from the Intens datapool to an external destination
    @version $Id: StreamParameter.h,v 1.31 2007/05/22 11:55:36 amg Exp $
*/
class StreamDestination{
 public:
  /**@name methods */
  //@{
  /** transfers a double value
      @param val double value to be transferred
      @param dpar origin stream parameter
     */
  virtual void putValue( StreamParameter &dpar, double val ){
    putValues( dpar, std::vector<int>(1,1), std::vector<double>( 1, val ),false, std::vector<int>() );
  }
  /** transfers a complex value
      @param val complex value to be transferred
      @param dpar origin stream parameter
     */
  virtual void putValue( StreamParameter &dpar, const dComplex &val ){
    putValues( dpar, std::vector<int>(1,1), std::vector<dComplex>( 1, val ), std::vector<int>() );
  }
  /** transfers a string value
      @param val string value to be transferred
      @param dpar origin stream parameter
     */
  virtual void putValue( StreamParameter &dpar, const std::string &val ){
    putValues( dpar, std::vector<int>(1,1), std::vector<std::string>( 1, val ) );
  }
  /** transfers a multidimensional double array
      @param dpar origin stream parameter
      @param dims int vector including the dimension sizes
      @param vals double vector including the values
      @param isCell true if value is cell (array contains invalid values)
      @param inds  vector containing the indicator values (1 valid, -1 invalid)
     */
  virtual void putValues( StreamParameter &dpar
			  , const std::vector<int>&dims
			  , const std::vector<double>&vals
			  , bool isCell
			  , const std::vector<int>&inds )=0;
  /** transfers a multidimensional complex array
      @param dpar origin stream parameter
      @param dims int vector including the dimension sizes
      @param vals complex vector including the values
      @param inds  vector containing the indicator values (1 valid, -1 invalid)
     */
  virtual void putValues( StreamParameter &dpar
			  , const std::vector<int>&dims
			  , const std::vector<dComplex>&vals
			  , const std::vector<int>&inds )=0;
  /** transfers a multidimensional string array
      @param dpar origin stream parameter
      @param dims int vector including the dimension sizes
      @param vals double vector including the values
     */
  virtual void putValues( StreamParameter &dpar
			  , const std::vector<int>&dims
			  , const std::vector<std::string>&vals )=0;
  /** transfers a multidimensional struct array
      @param dpar origin stream parameter
      @param dims int vector including the dimension sizes
      @param fieldList vector containing the StreamParameter the fields
     */
  virtual void putValues( DataStreamParameter &dpar
			  , const std::vector<DataStreamParameter *>&fieldList
			  , const std::vector<int> &dims )=0;
  //@}
};
/** StreamSource
    The abstract interface class StreamSource contains
    the required functions for a data transfer operation
    from an external destination to the Intens datapool
    @version $Id: StreamParameter.h,v 1.31 2007/05/22 11:55:36 amg Exp $
*/
class StreamSource{
 public:
  /**@name methods */
  //@{
  /** get values from external external source
      @param dpar DataStreamParameter transfer destination
  */
  virtual void getValues( DataStreamParameter &dpar )=0;
  /** get a struct value from external external source
      @param dpar DataStreamParameter transfer destination
  */
  virtual void getValues( DataStreamParameter &dpar
			  , const std::vector<DataStreamParameter *>&fieldlist )=0;
  //@}
};


/** StreamParameter
    This abstract base class defines the interface common to
    all derived stream parameters
    @version $Id: StreamParameter.h,v 1.31 2007/05/22 11:55:36 amg Exp $
*/
class StreamParameter : public Serializable, public GuiIndexListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
 public:
  StreamParameter(char delimiter=' ')
    : m_continuousUpdate( false )
    , m_appendOption( false )
    , m_format( HardCopyListener::NONE )
    , m_delimiter(delimiter)
    , m_level(0)
    , m_serialize_flags( HIDEhidden )
  {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  typedef std::vector<StreamParameter *> ParameterList;
  typedef ParameterList::iterator iterator;
  typedef ParameterList::const_iterator const_iterator;

  /**@name methods */
  //@{

  ///
  virtual bool isJson() { return false; }
  ///
  virtual bool readJson( std::istream &, ParameterList &param_list ) { return false; }
  ///
  virtual JSONStreamParameter *getJSONStreamParameter() { return 0; }

  /** read values from istream
      @param istream source of input
  */
#ifdef __MINGW32__
  virtual bool read( const std::string &filename);
#endif
  virtual bool read( std::istream & )=0;
  /** write values to ostream
      @param ostream destination of output
  */
  virtual bool write( std::ostream & )=0;
  /** resets the continuous update
   */
  virtual void resetContinuousUpdate(){}
  /** sets the continuous update flag
   */
  virtual bool setContinuousUpdate( bool flag ){
    m_continuousUpdate = flag;
    return false; }
  /** has append option */
  bool hasAppendOption(){ return m_appendOption; }
  /** set append option */
  void setAppendOption( bool flag ){ m_appendOption = flag; }

  /** clears all referenced values
  */
  virtual void clearRange( bool incl_itemattr=false ){}

  /** Die Funktion setzt bei allen verfügbaren, nicht transienten Items den
      DbTimestamp, damit die Werte 'not modified' sind. Diese Aktion ist nur
      nach dem Laden von Komponenten aus der Datenbank sinnvoll.
  */
  virtual void setDbItemsNotModified(){}

  /** resolve any VarStreamParameters in their real Parameter
   */
  virtual void resolveVars(){}

  virtual void fixupItemStreams( BasicStream &stream ) = 0;

  /** returns the dimension size
      @param ndim dimension
  */
  virtual size_t getDimensionSize(int ndim)const{return 0;}
  /** returns the size of the framebox for LaTex-Report
   */
  virtual bool getFrameBoxSize( int &x, int &y ){ return false; }
  /** set the size of the framebox in a LaTex-Report
   */
  virtual void setFrameBoxSize( const int x, const int y ){}
  /** returns true if data is valid
   */
  virtual bool isValid(bool validErrorMsg=false) = 0;
  /** returns the data identifier
   */
  virtual std::string getName()const = 0;
  /** transfer function to send values to a destination
      @param dest data transfer destination
      @param transposed if true the values are transposed
   */
  virtual std::string getFullName(bool withIndexes) const { return getName(); };
  virtual const std::string getLabel() const { return getName(); };
  virtual void putValues( StreamDestination *dest, bool transposed )=0;
  /** transfer function to receive values from a source
      @param src data transfer source
  */
  virtual void getValues( StreamSource *src )=0;
  /// returns true if attribute is scalar
  virtual bool isScalar() const { return true; }
  /// returns a list of item names that are not valid
  virtual bool getInvalidItems( std::vector< std::string > &invaliditems ){ return false; }

  /// return number of indexes
  virtual int getNumIndexes() const { return 0; }
  /// return index name of index
  virtual std::string getIndexName( int i )const{ return ""; }
  //@}

  virtual void marshal( std::ostream &os )=0;
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList )=0;
  void setText( const std::string &text ) {}

  virtual void setTargetStream( BasicStream * ) {}
  void setFormat( HardCopyListener::FileFormat format ){ m_format=format; }

  // set stream delimiter
  void setDelimiter(char delim ) { m_delimiter = delim; };
  // get stream delimiter
  char getDelimiter() const { return m_delimiter; };

  // set level
  void setLevel( int level ) { m_level = level; }

  // get hideFlags
  SerializableMask getHideFlags() const { return m_serialize_flags; }
  // set hideFlags
  void setHideFlags( const SerializableMask flags ) { m_serialize_flags = flags; }

  /** has error messages
     */
  virtual bool hasErrorMessage() { return m_errorStream.str().size(); }
  /** get error message stream
     */
  virtual std::ostringstream& getErrorMessageStream() { return m_errorStream; }
  /** has warning messages
     */
  virtual bool hasWarningMessage() { return m_warningStream.str().size(); }
  /** get warning message stream
     */
  virtual std::ostringstream& getWarningMessageStream() { return m_warningStream; }

/*=============================================================================*/
/* protected method                                                            */
/*=============================================================================*/
protected:
  void clearErrorMessageStream() { m_errorStream.str(""); }
  void clearWarningMessageStream() { m_warningStream.str(""); }
/*=============================================================================*/
/* protected data                                                              */
/*=============================================================================*/
protected:
  bool                   m_continuousUpdate;
  bool                   m_appendOption;
  HardCopyListener::FileFormat m_format;
  char                   m_delimiter;
  int                    m_level; // for JSONStreamParameter
  std::ostringstream     m_errorStream;
  std::ostringstream     m_warningStream;

  // ignore hidden and/or dbTransient data items when writing (only used with JSON)
  SerializableMask       m_serialize_flags;
};

#endif
