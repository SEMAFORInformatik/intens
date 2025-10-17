
#if !defined GUI_MFM_IMAGE_MAPPER_INCLUDED_H
#define GUI_MFM_IMAGE_MAPPER_INCLUDED_H

#include "operator/IntensServerSocket.h"

class GuiImage;

class MFMImageMapper : public IntensSocketListener{
  //-------------------------------------------------------
  // Constructor / Destructor
  //-------------------------------------------------------
private:
  MFMImageMapper( IntensServerSocket *server );
public:
  ~MFMImageMapper();

public:
  class ImageData {
  public:
      typedef float value_type;
      typedef std::vector<value_type> Line;
      typedef std::map<int,Line> Image;

      ImageData():m_height(0){}

      Image::const_iterator begin( )const{
	  return img.begin();
      }

      Image::const_iterator find( int l )const{
	  return img.find(l);
      }
      Image::const_iterator end( )const{
	  return img.end();
      }
      Image::size_type getSize()const{
	  return img.size();
      }
      Image::size_type getHeight()const{
	  return m_height;
      }
      // returns the Base64-encoded Thumbnail
    std::string &getThumbnail( int lines, int thumb_width, int thumb_height, int channel, int dir );
      // returns the Base64-encoded Image array
      std::string &getBuffer( int lines );
      // set the Base64-encoded Image array
      // @param width
      // @param height
      // @param img base64 encoded image array
      void setBuffer(Line::size_type width, Image::size_type height,
		     const std::string &img);

      Line &operator[](int i){
	  if( i+1 > m_height ) m_height = i+1; // i is line_nr, height is max line_nr + 1
	  return img[i];
      }
      bool empty()const{
	  return img.empty();
      }
  private:
      int m_height; // max line_nr + 1
      Image img;
      std::string imgbuf;
      std::string thumbbuf;
  };

  typedef std::map<int,ImageData *>            IMAGE_MAP;

  //-------------------------------------------------------
  // public functions
  //-------------------------------------------------------
public:
  static MFMImageMapper *addImage( GuiImage *image,
				   IntensServerSocket *server );
  static bool setChannel( GuiImage *, int );
  virtual bool read( std::istream & ){
    assert( false );
    return true;
  }
  virtual bool write( std::ostream & ){
    assert( false );
    return true;
  }
  void newLine( int, int , std::istream & );
  void nextScan();
  void endOfScan();
  void newScan( bool clearImages=true );
  void clearValues( );
  void setImageScale( int width, int height );
  void setScanningScheme( std::string setScanningScheme );
  static const ImageData *getImageData( GuiImage *image, int channel = -1 );
  void readFromDatapool( int width, int height );
  void setMaster();
  void enableContextMenu( bool enable ){}
  void getScanSize( int &w, int &h )const{
    w=m_scan_width;
    h=m_scan_height;
  }

  //-------------------------------------------------------
  // private functions
  //-------------------------------------------------------
private:
  void writeToDatapool();
  ImageData *getImage( int id );

  //-------------------------------------------------------
  // private members
  //-------------------------------------------------------
private:
  static std::vector<MFMImageMapper*>  s_mapperList;
  IntensServerSocket                  *m_server;
  std::map<GuiImage*,int >             m_images;
  std::vector<IMAGE_MAP*>              m_scan;
  std::vector<IMAGE_MAP*>              m_master;
  int                                  m_scan_width;
  int                                  m_scan_height;
  std::string                          m_scanningScheme;
};

#endif
