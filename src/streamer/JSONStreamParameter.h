
#if !defined(JSON_STREAM_PARAMETER_H)
#define JSON_STREAM_PARAMETER_H

#include <string>

#include <jsoncpp/json/json.h>
#include "streamer/StreamParameter.h"

class StreamSource;
class StreamDestination;
class HardCopyListener;
class DataValue;

class JSONStreamParameter : public StreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// constructs a plotgroup stream parameter
  JSONStreamParameter( XferDataItem *xfer, int i=0 );
  virtual ~JSONStreamParameter();
 private:
  JSONStreamParameter(const JSONStreamParameter &r);
  void operator=(const JSONStreamParameter&);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  ///
  virtual bool isJson() { return true; }
  ///
  virtual bool readJson( std::istream &, ParameterList &param_list );
  ///
  virtual JSONStreamParameter *getJSONStreamParameter() { return this; }

  ///
  virtual void clearRange( bool incl_itemattr=false );

  /** Die Funktion setzt bei allen verfügbaren, nicht transienten Items den
      DbTimestamp, damit die Werte 'not modified' sind. Diese Aktion ist nur
      nach dem Laden von Komponenten aus der Datenbank sinnvoll.
  */
  virtual void setDbItemsNotModified();
  virtual bool isValid(bool validErrorMsg=false);

  virtual bool read( std::istream &is );
#ifdef __MINGW32__
  virtual bool read( const std::string &filename );
#endif
  bool readString(const std::string& s);
  virtual bool write( std::ostream &os );
  virtual void fixupItemStreams( BasicStream &stream ) {}
  virtual std::string getName()const;
  virtual std::string getFullName(bool withIndexes) const;
  virtual const std::string getLabel() const;
  std::string getDbAttr() const;
  std::string getItemLabel(const std::string& subvarname) const;
  std::string getDataSetValue(const std::string& subvarname, const std::string& value) const;
  virtual void putValues( StreamDestination *dest, bool transposed ){}
  virtual void getValues( StreamSource *src ){}
  void withAllCycles(){ m_allCycles = true; }

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  //  void setText( const std::string &text );

  /** Diese Funktion ist die Implementation der GuiIndexListener-Funktion.
   */
  virtual void setIndex( const std::string &name, int index );

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  XferDataItem *getXferDataItem() { return m_xfer; }
  bool isCycleStream();
  bool isDatapoolStream();
  bool isElementStream();

/*=============================================================================*/
/* private methods                                                             */
/*=============================================================================*/
private:
  Json::ValueType getChildType( Json::Value &value );
  bool parse_json_stream( std::istream &is );
  bool read_datapool();
  bool read_cycle();
  bool write_element( std::ostream &os );
  void write_a_cycle( std::ostream &os, int level, int num );
  bool write_datapool( std::ostream &os );
  bool write_cycle( std::ostream &os );
  DataReference *getDataReference_Sub(const std::string& subvarname) const;
  std::string getDbAttrItemLabel(DataReference* ref, const std::string& subvarname) const;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  static Json::Value       s_root;

  XferDataItem            *m_xfer;
  int                      m_indentation;
  bool                     m_allCycles;
};

#endif
