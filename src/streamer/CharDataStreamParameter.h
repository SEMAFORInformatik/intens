
#if !defined(STREAMER_CHARDATA_STREAM_PARAMETER_H)
#define STREAMER_CHARDATA_STREAM_PARAMETER_H

#include <vector>
#include <string>

#include "streamer/DataStreamParameter.h"
#include "xfer/XferConverter.h"

class CharDataStreamParameter: public DataStreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  CharDataStreamParameter();
  CharDataStreamParameter( XferDataItem *ditem, int level, bool mand);
  virtual ~CharDataStreamParameter();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );

  virtual void putValues( StreamDestination *dest, bool transposed );
};

#endif
