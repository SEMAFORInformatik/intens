
#if !defined(STREAMER_GUIINDEX_STREAM_PARAMETER_H)
#define STREAMER_GUIINDEX_STREAM_PARAMETER_H

#include <vector>
#include <string>

#include "streamer/DataStreamParameter.h"
#include "gui/GuiIndex.h"

class DataSet;
class XferDataParameter;

class GuiIndexStreamParameter: public StreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiIndexStreamParameter( GuiIndex *index );
  virtual ~GuiIndexStreamParameter();

private:
  GuiIndexStreamParameter();
/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );
  virtual void fixupItemStreams( BasicStream &stream ) {}
  virtual size_t getDimensionSize(int ndim)const;
  virtual std::string getName()const{ return m_guiIndex->getElement()->getName(); }
  virtual bool isValid(bool validErrorMsg=false);

  virtual void putValues( StreamDestination *dest, bool transposed );
  virtual void getValues( StreamSource *src ){}

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  IntConverter         m_conv;
  GuiIndex            *m_guiIndex;
};

#endif
