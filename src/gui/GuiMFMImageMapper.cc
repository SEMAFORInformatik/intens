
#include <algorithm>
#include <math.h>

#if defined HAVE_QT && !defined HAVE_HEADLESS
#include "gui/qt/GuiQtMinMaxColorMap.h"
#endif
#include "utils/Debugger.h"
INIT_LOGGER();
#include "gui/GuiMFMImageMapper.h"
#include "gui/GuiImage.h"
#include "xfer/XferDataItem.h"
#include "datapool/DataVector.h"
#include "datapool/DataRealValue.h"
#include "utils/base64.h"

#ifdef HAVE_QT
// thumbnail
#include <qrgb.h>
#include <qimage.h>
#include <qbuffer.h>
#include <qvector.h>
#include <qpainter.h>
#include <qstring.h>
#endif
#include "gui/DataProcessing.h"
#include "gui/LineStatistics.h"

std::vector<MFMImageMapper*> MFMImageMapper::s_mapperList;

//===============================================================
// MFMImageMapper
//===============================================================

//-------------------------------------------------------------
// Constructor / Destructor
//-------------------------------------------------------------

MFMImageMapper::MFMImageMapper( IntensServerSocket *server )
  : m_server( server )
  , m_scan_width(0)
  , m_scan_height(0){
  s_mapperList.push_back( this );
}

MFMImageMapper::~MFMImageMapper(){
  std::vector<IMAGE_MAP*>::iterator iiter;
  for( iiter = m_scan.begin(); iiter != m_scan.end(); ++iiter ){
    delete *iiter;
  }
  m_scan.clear();

  std::vector<MFMImageMapper*>::iterator iter;
  for( iter=s_mapperList.begin(); iter!=s_mapperList.end();++iter ){
    if( *iter == this ){
      s_mapperList.erase( iter );
      break;
    }
  }
}


//-------------------------------------------------------------
// addImage
//-------------------------------------------------------------

MFMImageMapper *MFMImageMapper::addImage( GuiImage *image,
					  IntensServerSocket* server ){
  std::vector<MFMImageMapper*>::iterator iter;
  for( iter=s_mapperList.begin(); iter!=s_mapperList.end();++iter ){
    std::map<GuiImage*,int>::iterator iiter;
    for( iiter=(*iter)->m_images.begin(); iiter != (*iter)->m_images.end(); ++iiter ){
      if( (*iiter).first==image )
	assert( false );
    }
  }
  for( iter=s_mapperList.begin(); iter!=s_mapperList.end();++iter ){
    if( (*iter)->m_server==server ){
      (*iter)->m_images[image]=image->getChannel();

      // image may be added after a scan was started
      // tell it the current or previous scan size
      int scan_width, scan_height;
      (*iter)->getScanSize(scan_width, scan_height);
      image->setImageScale( scan_width, scan_height );

      return *iter;
    }
  }
  MFMImageMapper *mapper = new MFMImageMapper( server );
  server->setRequestListener( mapper );
  s_mapperList.push_back( mapper );
  mapper->m_images[image]=image->getChannel();
  return mapper;
}

//-------------------------------------------------------------
// nextScan
//-------------------------------------------------------------

void MFMImageMapper::nextScan(){
  m_scan.push_back( new IMAGE_MAP() );
}

//-------------------------------------------------------------
// endOfScan
//-------------------------------------------------------------

void MFMImageMapper::endOfScan(){
  writeToDatapool();
}

//-------------------------------------------------------------
// writeToDatapool
//-------------------------------------------------------------

void MFMImageMapper::writeToDatapool(){
  XferDataItem *xfer = m_server->getXfer();
  assert( xfer != 0 );
  assert( xfer->getNumOfWildcards()>2 );

  XferDataItemIndex *imageIndex = xfer->getDataItemIndexWildcard(1);
  XferDataItemIndex *dirIndex = xfer->getDataItemIndexWildcard(2);
  XferDataItemIndex *channelIndex = xfer->getDataItemIndexWildcard(3);

  XferDataItem *thumb_xfer = m_server->getThumbXfer();
  XferDataItemIndex *thumb_imageIndex;
  XferDataItemIndex *thumb_dirIndex;
  XferDataItemIndex *thumb_channelIndex;
  int thumb_width, thumb_height;
  if ( thumb_xfer != 0 ) {
    assert( thumb_xfer->getNumOfWildcards()>2 );

    thumb_imageIndex = thumb_xfer->getDataItemIndexWildcard(1);
    thumb_dirIndex = thumb_xfer->getDataItemIndexWildcard(2);
    thumb_channelIndex = thumb_xfer->getDataItemIndexWildcard(3);
    thumb_width = m_server->getThumbWidth();
    thumb_height = m_server->getThumbHeight();
  }

  // lets first find the smallest image in terms of lines
  int minlines=std::numeric_limits<int>::max();
  std::vector<IMAGE_MAP*>::iterator scanIter;
  for( scanIter = m_scan.begin();
       scanIter != m_scan.end(); ++scanIter ){
    IMAGE_MAP::iterator imgIter;
    for( imgIter = (*scanIter)->begin();
	 imgIter != (*scanIter)->end(); ++imgIter ){
      int nl = (*imgIter).second->getHeight();
      if( minlines>nl ) minlines=nl;
    }
  }

  xfer->setDimensionIndizes();
  if ( thumb_xfer != 0 ) {
    thumb_xfer->setDimensionIndizes();
  }
  int img = 0;
  for( scanIter = m_scan.begin();
       scanIter != m_scan.end(); ++scanIter, ++img ){
    xfer->setIndex( imageIndex, img );
    if ( thumb_xfer != 0 ) {
      thumb_xfer->setIndex( thumb_imageIndex, img );
    }

    IMAGE_MAP::iterator imgIter;
    for( imgIter = (*scanIter)->begin();
	 imgIter != (*scanIter)->end(); ++imgIter ){
      int channel = (*imgIter).first % 100;
      int dir =  ( (*imgIter).first - channel ) / 100;
      xfer->setIndex( dirIndex, dir );
      xfer->setIndex( channelIndex, channel );
      xfer->setValue( (*imgIter).second->getBuffer(minlines) );
      if ( thumb_xfer != 0 ) {
	thumb_xfer->setIndex( thumb_dirIndex, dir );
	thumb_xfer->setIndex( thumb_channelIndex, channel );
	thumb_xfer->setValue( (*imgIter).second->getThumbnail(minlines, thumb_width, thumb_height, channel, dir) );
      }
    }
  }
}

//-------------------------------------------------------------
// readFromDatapool
//-------------------------------------------------------------

void MFMImageMapper::readFromDatapool( int width, int height ){
    BUG_DEBUG( "Read from Datapool " <<width << "/" << height );

  newScan( true );

  XferDataItem *xfer = m_server->getXfer();
  assert( xfer != 0 );
  int n = xfer->getNumOfWildcards();
  assert( n>2 );

  XferDataItemIndex *imageIndex = xfer->getDataItemIndexWildcard(1);
  XferDataItemIndex *dirIndex = xfer->getDataItemIndexWildcard(2);
  XferDataItemIndex *channelIndex = xfer->getDataItemIndexWildcard(3);

  xfer->setDimensionIndizes();
  xfer->setIndex( imageIndex, 0 );
  xfer->setIndex( dirIndex, 0 );
  xfer->setIndex( channelIndex, 0 );

  int nlines=height;
  int npoints=width;
  int nimg = imageIndex->getDimensionSize( xfer->Data() );
  for( int img = 0; img != nimg; ++img ){
    if( img != 0 ){
      m_scan.push_back( new IMAGE_MAP() );
    }
    imageIndex->setIndex( xfer->Data(), img );
    int ndir = dirIndex->getDimensionSize( xfer->Data() );
    for( int dir = 0; dir != ndir; ++dir ){
      dirIndex->setIndex( xfer->Data(), dir );
      int nchannel = channelIndex->getDimensionSize( xfer->Data() );
      std::string imgbuf;
      for( int channel = 0; channel != nchannel; ++channel ){
 	  imgbuf.clear();
	  channelIndex->setIndex( xfer->Data(), channel );
	  ImageData *_img =getImage(dir*100+channel);
	  xfer->getValue( imgbuf );
	  if( !imgbuf.empty() ){
	      _img->setBuffer( width, height, imgbuf );
	  }
      }
    }
  }

  setImageScale(npoints, nlines);

  IMAGE_MAP::iterator iter;
  for( iter = m_scan.back()->begin(); iter != m_scan.back()->end(); ++iter ){
    std::map<GuiImage*,int>::iterator iiter;
    for( iiter=m_images.begin(); iiter != m_images.end(); ++iiter ){
      (*iiter).first->setMapping( 0 );
      (*iiter).first->resetMinMax( );
      (*iiter).first->showImage( (*iter).second, (*iter).first );
    }
  }
 }

//-------------------------------------------------------------
// clearValues
//-------------------------------------------------------------

void MFMImageMapper::clearValues(){
  XferDataItem *xfer = m_server->getXfer();
  int level;
  assert( xfer != 0 );
  XferDataItemIndex *imageIndex = xfer->getDataItemIndexWildcard(1);
  level = imageIndex -> Level();
  //  std::cerr << level << std::endl;
  xfer->clearRange(level);
}

//-------------------------------------------------------------
// newScan
//-------------------------------------------------------------

void MFMImageMapper::newScan( bool clearImages ){
  std::map<GuiImage*,int>::iterator iter;
  for( iter=m_images.begin(); iter != m_images.end(); ++iter ){
    (*iter).first->clear( clearImages );
    (*iter).first->resetMinMax();
    (*iter).first->clearView();
  }
  std::vector<IMAGE_MAP*>::iterator iiter;
  for( iiter = m_scan.begin(); iiter != m_scan.end(); ++iiter ){
      IMAGE_MAP::iterator miter;
      for( miter = (*iiter)->begin(); miter != (*iiter)->end(); ++miter ){
	  delete (*miter).second;
      }
      delete *iiter;
  }
  m_scan.clear();
  assert(m_scan.size()==0);
  m_scan.push_back( new IMAGE_MAP() );
}


//-------------------------------------------------------------
// setChannel
//-------------------------------------------------------------

bool MFMImageMapper::setChannel( GuiImage *image, int channel ){
  std::vector<MFMImageMapper*>::iterator iter;
  for( iter=s_mapperList.begin(); iter!=s_mapperList.end();++iter ){
    std::map<GuiImage*,int>::iterator iiter;
    for( iiter=(*iter)->m_images.begin();
	 iiter != (*iter)->m_images.end(); ++iiter ){
      if( (*iiter).first==image ){
	(*iiter).second=channel;
	return true;
      }
    }
  }
  return false;
}

//-------------------------------------------------------------
// getImageData
//-------------------------------------------------------------

const MFMImageMapper::ImageData *
MFMImageMapper::getImageData( GuiImage* image, int channel ){
  std::vector<MFMImageMapper*>::iterator iter;
  for( iter=s_mapperList.begin(); iter!=s_mapperList.end();++iter ){
    if( !(*iter)->m_scan.empty() ){
      std::map<GuiImage*,int>::iterator iiter;
      for( iiter=(*iter)->m_images.begin();
	   iiter != (*iter)->m_images.end(); ++iiter ){
	if( (*iiter).first==image ){
	  IMAGE_MAP *img_map = (*iter)->m_scan.back();
	  if( channel == -1 ){
	    channel = (*iiter).second;
	  }
	  if( img_map->find( channel ) != img_map->end() ){
	    return ((*img_map)[channel]);
	  }
	}
      }
    }
  }
  return 0;
}

//-------------------------------------------------------------
// setImageScale
//-------------------------------------------------------------

void MFMImageMapper::setImageScale( int scan_width, int scan_height ){
  m_scan_width = scan_width;
  m_scan_height = scan_height;

  std::map<GuiImage*,int>::iterator iter;
  for( iter=m_images.begin(); iter != m_images.end(); ++iter ){
    GuiImage *img = (*iter).first;
    img -> setImageScale( scan_width, scan_height );
  }
}

//-------------------------------------------------------------
// setScanningScheme
//-------------------------------------------------------------

void MFMImageMapper::setScanningScheme( std::string scanningScheme ) {
  m_scanningScheme = scanningScheme;
}

MFMImageMapper::ImageData *MFMImageMapper::getImage( int id ){
    ImageData *img = (*m_scan.back())[id];
    if( img == 0 ){
	  if( img == 0 ){
	      img =  new ImageData();
	      (*m_scan.back())[id] = img;
	  }
    }
    return img;
}

//-------------------------------------------------------------
// newLine
//-------------------------------------------------------------

void MFMImageMapper::newLine( int id, int line_nr, std::istream &is ){
  if( m_scan.empty() ){
    newScan();
  }
  bool forward = (id <= 99);
  int imageId = id;
  if ( m_scanningScheme == "meander" && !forward ) {
    imageId -= 100; // write backward line to forward images
  }

  ImageData *img=getImage(imageId);
  ImageData::Line &line = (*img)[line_nr];
  if( line.empty() ){
    line.reserve( m_scan_width );
  }
// bas : commented defined _WIN32 because stream method doesn't read nan as double
#if defined HAVE_QT //&& defined _WIN32
  std::string s;
  if( forward ){
    while( is >> s ){
      line.push_back( QString( s.c_str() ).toFloat() );
    }
  }
  else{
    ImageData::Line _line;
    while( is >> s ){
      _line.push_back( QString( s.c_str() ).toFloat() );
    }
    line.insert( line.begin(), _line.rbegin(), _line.rend());
  }
#else
  ImageData::value_type point;
  if( forward ){
    while( is >> point ){
      line.push_back( point );
    }
  }
  else{
    ImageData::Line _line;
    while( is >> point ){
      _line.push_back( point );
    }
    line.insert( line.begin(), _line.rbegin(), _line.rend());
  }
#endif

  if( line.size() == m_scan_width ){
    std::map<GuiImage*,int>::iterator iter;
    for( iter=m_images.begin(); iter != m_images.end(); ++iter ){
      (*iter).first->setLine( imageId,line_nr, line, img );
    }
  }
}

//-------------------------------------------------------------
// setMaster
//-------------------------------------------------------------

void MFMImageMapper::setMaster(){
  m_master = m_scan;
}

//-------------------------------------------------------------
// MFMImageMapper::ImageData
//-------------------------------------------------------------
std::string & MFMImageMapper::ImageData::getThumbnail(int lines, int thumb_width, int thumb_height, int channel, int dir){
  assert(thumb_width > 0 && thumb_width <= 1000);
  assert(thumb_height > 0 && thumb_height <= 1000);
  thumbbuf="";
  if( empty() ){
    return thumbbuf;
  }
  #ifdef HAVE_QT
  // first line defines width: (should we check this?)
  Line::size_type width = (*img.begin()).second.size();
  Line::size_type height = lines < getHeight() ? lines : getHeight();

  DataProcessing *dproc = new PlaneFit(3);
  dproc->setImage(this);

  // get max, min value (using stddev)
  ImageData::value_type min= std::numeric_limits<double>::infinity();
  ImageData::value_type max=-std::numeric_limits<double>::infinity();

  //stddev
  double sum=0;
  double sqrsum=0;
  double count=0;

  for ( int l=0; l<height; ++l ){
    dproc->setLine(l);
    ImageData::Line line = img[l];
    if( line.empty() ) continue;

    LineStatistics m=std::for_each( line.begin(),
                                    line.end(),
                                    LineStatistics(dproc) );
    sum += m.getSum();
    sqrsum += m.getSqrSum();
    count += m.getSize();
  }
  if( count > 0 ){
    double avg = sum/count;
    double stddev = 0;
    if( count > 1 ){
      if ( sqrsum-sum*avg < 0. ) {
        stddev = 0.;
      } else {
        stddev = sqrt((sqrsum-sum*avg)/(count-1) );
      }
    }
		min = avg - 1.5 * stddev;
		max = avg + 1.5 * stddev;
  }

  // build image
  QImage *thumbnail = 0;
  if ( height == 1 ) { // line
    // factor d
    double d(max-min);
    if( fabs(d) < 2*std::numeric_limits<double>::epsilon() ){
      d = 2*std::numeric_limits<double>::epsilon();
    }
    d = thumb_height * 0.999999 / d; // 0.999999 to avoid i=thumb_height for value=max !!

    // thumbnail
    thumbnail = new QImage( thumb_width, thumb_height, QImage::Format_RGB32 );
    thumbnail->fill(qRgb( 255, 255, 255 ));
    QPainter painter(thumbnail);
		painter.setPen( dir == 0 ? "red" : "blue" );

    int x = 0;
    int nx = 0;
    double dx = (double)width/thumb_width;

    double sum = 0;
    int y = 0;

    int x0 = -1, y0 = -1;

    dproc->setLine(0);
    for( int k=0; k<width; ){
      sum+=(*dproc)(img[0][k]);
      ++nx;
      ++k;

      if ( k > x*dx ) {
				// if ( sum/nx > min-std::numeric_limits<double>::epsilon() &&
				// 		 sum/nx < max+std::numeric_limits<double>::epsilon()
				// 		 ) {
					y = (int)( (sum/nx - min) * d );
				// } else {
				// 	y = -1;
				// }
        if ( x > 0 ) {
          painter.drawLine(x0,y0,x,y);
        }
        x0 = x;
        y0 = y;
        x = (int)ceil(k/dx);

        sum=0;
        nx=0;
      }
    }
  } else { // image
#if defined HAVE_QT && !defined HAVE_HEADLESS
    thumbnail = new QImage( width, height, QImage::Format_RGB32 );
    thumbnail->fill(qRgb( 255, 255, 255 ));
    MinMaxColorMap cmap = MinMaxColorMap(min,max,channel);
    for ( int l=0; l<height; ++l ){
      dproc->setLine(l);
      if( !img[l].empty() ){
        for( int k=0; k<width; ++k ){
          thumbnail->setPixel( k, height-l-1, cmap( (*dproc)(img[l][k]) ) );
        }
      }
    }
#else
    std::cerr << "not implemented [MFMImageMapper::ImageData::getThumbnail()]\n";
#endif
  }

  // convert image to base64 string
  QByteArray *ba = new QByteArray;
  QBuffer *buffer = new QBuffer(ba);
  buffer->open(QIODevice::WriteOnly);
  if ( height == 1 ) { // line
#if QT_VERSION >= 0x060900
    thumbnail->flipped().save(buffer, "PNG"); // writes image into ba in PNG format
#else
    thumbnail->mirrored().save(buffer, "PNG"); // writes image into ba in PNG format
#endif
  } else {
    thumbnail->scaled(thumb_width,thumb_height,Qt::KeepAspectRatio).save(buffer, "PNG"); // writes image into ba in PNG format
  }
  delete thumbnail;
  delete buffer;
  base64encode(reinterpret_cast< unsigned char *>(ba->data()),
               ba->size(), thumbbuf, true );
  delete ba;
  return thumbbuf;
  #else
  thumbbuf = "";
  return thumbbuf; // TODO
  #endif
}

std::string & MFMImageMapper::ImageData::getBuffer(int lines){
    imgbuf="";
    if( empty() ){
	return imgbuf;
    }
    // first line defines width: (should we check this?)
    Line::size_type width = (*img.begin()).second.size();
    Line::size_type height = lines < getHeight() ? lines : getHeight();
    Line::size_type size = height * width;
    ImageData::value_type *array = new ImageData::value_type [size];
    int i=0;
    // now copy array
    for ( int l=0; l<height; ++l ){
	ImageData::Line line = img[l];
	if( !line.empty() ){
	    for( int k=0; k<width; ++k ){
		array[i++] = img[l][k];
	    }
	}
    }
    size=i;
    BUG_DEBUG(  "MFMImageMapper::ImageData::getBuffer" <<
		" lines: " << lines <<
		" width: " << width <<
		" height: " << height <<
		" size: " << size );
    base64encode(reinterpret_cast< unsigned char *>( array ),
	       size*sizeof(ImageData::value_type), imgbuf, true );
    delete [] array;
    return imgbuf;
}

void MFMImageMapper::ImageData::setBuffer( Line::size_type width,
					   Image::size_type height,
					   const std::string &pimg){
    this->imgbuf=pimg;
//    BUG_DEBUG( "ImageData::setBuffer " << pimg );
    size_t outsize = width*height;
    ImageData::value_type *output = new ImageData::value_type [ outsize ];
    outsize *= sizeof(ImageData::value_type);
    if( !base64decode(this->imgbuf,
		      reinterpret_cast< unsigned char *>( output ),outsize ) ){
      BUG_WARN( "ImageData::setBuffer Conversion Error "
                << pimg.size() << " != " << outsize );
      return;
    }
    int i=0;
    int outlen = outsize/sizeof( ImageData::value_type );
    img.clear();
    for ( int l=0; l<height; ++l ){
	img[l].clear();
	for( int k=0; k<width; ++k ){
//	    BUG_DEBUG( "ImageData::setBuffer "
//			   << l << " " << k << " " << output[i] );
	    if( i < outlen ){
		img[l].push_back( output[i++] );
	    }
	}
    }
		m_height = outlen / width;
    BUG_DEBUG(  "MFMImageMapper::ImageData::setBuffer" <<
		" width: " << width <<
		" height: " << height <<
		" outsize: " << outlen );

    delete [] output;
}
