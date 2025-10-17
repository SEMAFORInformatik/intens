
#if !defined(FILE_SELECTION_LISTENER_H)
#define FILE_SELECTION_LISTENER_H

#include "app/HardCopyListener.h"

class FileSelectListener
{
/*=============================================================================*/
/* Interface                                                                   */
/*=============================================================================*/
public:
  virtual void FileSelected( const std::string &filename
			     , const HardCopyListener::FileFormat format
			     , const std::string dir ) = 0;
  virtual void canceled() {}
  /** write file stream data into stream
      used by WebInterface to download file content
      @param stream results stream
  */
  virtual void writeStreamData(std::ostream& stream, HardCopyListener::FileFormat& format) {}
  /** read file stream data from uploaded data
      used by WebInterface with uploaded file content
      @param stream output stream
      @param format output stream format
  */
  virtual void readStreamData(std::istream& stream) {}
};

#endif
