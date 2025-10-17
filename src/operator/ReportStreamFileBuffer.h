
#ifndef REPORTSTREAMFILEBUFFER_H
#define REPORTSTREAMFILEBUFFER_H

#include <string>
#include "app/HardCopyListener.h"
#include "operator/InputChannelEvent.h"


class ReportStreamFileBuffer: public HardCopyListener {
public:
  ReportStreamFileBuffer( const std::string &b ):
    m_buffer(b)
    , m_name("ReportStreamFileBuffer")
    , m_label("ReportStreamFileBuffer"){}
  virtual ~ReportStreamFileBuffer(){}
  virtual bool write( InputChannelEvent &event );
  virtual bool write( const std::string &fileName ){ std::cerr<<"ReportStreamFileBuffer::write( const std::string)"<<std::endl; return false; }
  virtual bool write( std::ostream &os ){ std::cerr<<"ReportStreamFileBuffer::write( std::ostream)"<<std::endl; return false; }

  virtual FileFormat getFileFormat(){return HardCopyListener::XML;}
  virtual const std::string &getName(){return m_name;}
  virtual bool isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat ){return true;}
  virtual const std::string &MenuLabel(){return m_label;}
  virtual bool saveFile( GuiElement * ) { return false; }

 private:
  std::string  m_name;
  std::string  m_label;
  std::string  m_buffer;
};

#endif
