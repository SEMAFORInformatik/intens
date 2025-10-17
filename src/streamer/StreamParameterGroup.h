
#if !defined(STREAMER_STREAM_PARAMETER_GROUP_H)
#define STREAMER_STREAM_PARAMETER_GROUP_H

#include <vector>
#include <string>

#include "streamer/StreamParameter.h"

class StreamSource;
class StreamDestination;
class IndexParameter;

class bad_call : public std::exception{
};

class StreamParameterGroup: public StreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// constructs an unnamed StreamParameterGroup object
  StreamParameterGroup( StreamParameterGroup *parent=0, char delimiter=' ' );

  /// Destructor
  virtual ~StreamParameterGroup();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  bool isPending(){ return m_pending; }
  void setPending( bool state ){ m_pending = state; }
#ifdef __MINGW32__
  virtual bool read( const std::string &filename )
    { return StreamParameter::read(filename);  }
#endif
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );
  virtual void resetContinuousUpdate();
  virtual bool setContinuousUpdate( bool flag );
  virtual bool setAppendOption( bool flag );
  /// returns the number of parameters included
  virtual size_t   size() const {return m_parvec.size();}

  /// adds a column parameter
  virtual void addParameter( StreamParameter *p );

  /// returns the parent stream group if any or 0 otherwise
  StreamParameterGroup *getParent() const { return m_parent; }

  /// adds a index column and returns the corresponding parameter
  virtual IndexParameter * addIndex( const std::string &ident, int length=0);

  /// returns true if this group has an index
  virtual bool hasIndex() const;

  /** return StreamParameterGroup that has named index
      @param indexname name of index
  */
  virtual StreamParameterGroup *findIndex( const std::string &indexname );
  virtual void addIndexedParameter( const std::string &ident, DataStreamParameter *d ){};
  virtual void removeIndexedParameter( DataStreamParameter *d ){};

  /// do any pre-transfer actions
  virtual void clearRange( bool incl_itemattr=false );

  /** Die Funktion setzt bei allen verfügbaren, nicht transienten Items den
      DbTimestamp, damit die Werte 'not modified' sind. Diese Aktion ist nur
      nach dem Laden von Komponenten aus der Datenbank sinnvoll.
  */
  virtual void setDbItemsNotModified();
  virtual bool isValid(bool validErrorMsg=false);

   /** resolve any VarStreamParameters in their real Parameter
  */
  virtual void resolveVars();
  void setFormat( HardCopyListener::FileFormat format ){ m_format = format; }

  virtual void fixupItemStreams( BasicStream &stream );

  /// returns an identifier list of the included parameters
  int getParameters( std::ostream &os ) const;
  ParameterList *getParameterList(){ return &m_parvec; }

  virtual std::string getName()const{ return "<StreamParameterGroup>"; }
  virtual const std::string getLabel() const;

  virtual void putValues( StreamDestination *dest, bool transposed );
  virtual void getValues( StreamSource *src );
  virtual bool getInvalidItems( std::vector< std::string >&invaliditems );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

  virtual bool linkWithIndex( StreamParameter * ){ return false; }
  virtual void setTargetStream( BasicStream * );

  // set stream delimiter
  void setDelimiter(char delim );

  /// ignore hidden and/or dbTransient data items when writing (only used with JSON)
  SerializableMask getHideFlags() const;
  ///
  void setHideFlags( const SerializableMask flags );

  virtual bool hasErrorMessage();
  virtual std::ostringstream& getErrorMessageStream();

  virtual bool hasWarningMessage();
  virtual std::ostringstream& getWarningMessageStream();

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  iterator begin(){ return m_parvec.begin(); }
  iterator end(){ return m_parvec.end(); }
  const_iterator begin() const{ return m_parvec.begin(); }
  const_iterator end() const { return m_parvec.end(); }

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
 protected:
  bool                   m_pending;
  ParameterList          m_parvec;
  iterator               m_currentParameter;
private:
  StreamParameterGroup  *m_parent;
  bool                   m_end;
};

#endif
