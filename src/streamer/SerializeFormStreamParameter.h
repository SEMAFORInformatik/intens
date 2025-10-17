
#if !defined(XML_SERIALIZEFORM_STREAM_PARAMETER_H)
#define XML_SERIALIZEFORM_STREAM_PARAMETER_H

#include <string>

#include "streamer/StreamParameter.h"

class GuiForm;
class StreamSource;
class StreamDestination;
class HardCopyListener;

class SerializeFormStreamParameter : public StreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  SerializeFormStreamParameter( const std::string &formId, const std::string &xslFile );
  SerializeFormStreamParameter( GuiForm*form, const std::string &xslFile );
  SerializeFormStreamParameter( std::vector<std::string> &formIdList, const std::string &xslFile );
  virtual ~SerializeFormStreamParameter();
 private:
  SerializeFormStreamParameter(const SerializeFormStreamParameter &r);
  void operator=(const SerializeFormStreamParameter&);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool isValid(bool validErrorMsg=false) { return true; }
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );
  virtual void fixupItemStreams( BasicStream &stream ) {}
  virtual size_t getDimensionSize(int ndim)const;
  virtual std::string getName()const;
  virtual void putValues( StreamDestination *dest, bool transposed );
  virtual void getValues( StreamSource *src ){}

  bool serializeFormList( std::ostream &os );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* private member functions                                                     */
/*=============================================================================*/
private:
  bool pushForm( const std::string &form );
/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:

  typedef std::vector<GuiForm *> GuiFormList;

  GuiFormList  m_formlist;
  std::string  m_xslFileName;
  bool         m_useStylesheet;
  int          m_mainFormPos;
};

#endif
