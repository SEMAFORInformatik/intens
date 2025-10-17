
#include <limits>
#include <algorithm>

#include "utils/Debugger.h"
INIT_LOGGER();
#include "app/DataPoolIntens.h"
#include "datapool/DataReference.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferConverter.h"

#include "gui/GuiImage.h"
#include "gui/DataProcessing.h"
#include "gui/LineStatistics.h"

#if defined HAVE_QT && defined _WIN32
#include <qstring.h>
#endif

GuiImage *GuiImage::s_hasFocus = 0;
JobFunction *GuiImage::s_on_view_action = 0;
std::map<const int, std::string> GuiImage::s_channelList;
std::vector<GuiImage*> GuiImage::s_images;
bool GuiImage::s_eos=true;
bool GuiImage::s_general_scan( false );
bool GuiImage::s_image_scan( false );

// some utility functions
DataReference *getDataPoolSubRef( XferDataItem *x, const std::string &sub ){
    return DataPoolIntens::Instance().getDataReference(x->Data(), sub);
}

int getDataPool( const char* varname, std::vector<std::string> &svec ){
  DataReference *ref =
    DataPoolIntens::Instance().getDataReference(varname);
  if( ref != 0 ){
    XferDataItem *xfer = new XferDataItem( ref );
    XferDataItemIndex *index = xfer->newDataItemIndex();
    xfer->setDimensionIndizes();

    int n = 0;
    n = index->getDimensionSize( ref );

    for( int x = 0; x < n; ++ x ){
      index->setIndex( ref, x );
      std::string s;
      if( xfer->getValue( s ) ){
	svec.push_back( s );
      }
    }
    delete xfer;
    return n;
  }
  return 0;
}

int getDataPool( const char* varname, std::map<const int, std::string> &svec ){
  DataReference *ref =
    DataPoolIntens::Instance().getDataReference(varname);
  if( ref != 0 ){
    XferDataItem *xfer = new XferDataItem( ref );
    XferDataItemIndex *index = xfer->newDataItemIndex();
    xfer->setDimensionIndizes();

    int n = 0;
    n = index->getDimensionSize( ref );

    for( int x = 0; x < n; ++ x ){
      index->setIndex( ref, x );
      std::string s;
      if( xfer->getValue( s ) ){
	svec.insert( std::pair<const int, std::string>(x, s) );
      }
    }
    delete xfer;
    return n;
  }
  return 0;
}

int getDataPool( const char* varname, std::vector<double> &dvec ){
  DataReference *ref =
    DataPoolIntens::Instance().getDataReference(varname);
  if( ref != 0 ){
    XferDataItem *xfer = new XferDataItem( ref );
    XferDataItemIndex *index = xfer->newDataItemIndex();
    xfer->setDimensionIndizes();

    int n = 0;
    n = index->getDimensionSize( ref );

    for( int x = 0; x < n; ++ x ){
      index->setIndex( ref, x );
      double d;
      if( xfer->getValue( d ) ){
	dvec.push_back( d );
      }
    }
    delete xfer;
    return n;
  }
  return 0;
}

template <class T>
int getDataPool( const char* varname, int indexval, T& val ){
  int retval=0;
  DataReference *ref =
    DataPoolIntens::Instance().getDataReference(varname);
  if( ref != 0 ){
    XferDataItem *xfer = new XferDataItem( ref );
    XferDataItemIndex *index = xfer->newDataItemIndex();
    xfer->setDimensionIndizes();

    index->setIndex( ref, indexval );
    if( xfer->getValue( val ) ){
      retval=1;
    }
    delete xfer;
  }
  return retval;
}

  //-------------------------------------------------------------
  // Constructor / Destructor
  //-------------------------------------------------------------
GuiImage::GuiImage( int f )
  : m_channel( 0 )
  , m_settings_xfer( 0 )
  , m_processingIndex( 0 )
  , m_direction( 1 )
  , m_auto_range( true )
  , m_range_min_manual( 0 )
  , m_range_max_manual( 0 )
  , m_scale_x_manual( 1 )
  , m_scale_y_manual( 1 )
  , m_scale_x( 1 )
  , m_weight( 1 )
  , m_stddev( 0 )
  , m_avg( 0 )
  , m_scale_y( 1 )
  , m_imageWidth( 256 )
  , m_imageHeight( 256 )
  , m_scanWidth( 0 )
  , m_scanHeight( 0 )
  , m_manual_scale( false )
  , m_mapping( 0 ) {
  m_processingVector.push_back( new Raw(0) );
  m_processingVector.push_back( new Average(1) );
  m_processingVector.push_back( new LineFit(2) );
  m_processingVector.push_back( new PlaneFit(3) );
  m_width = m_imageWidth;
  m_height = m_imageHeight;
  resetMinMax();
}

//-------------------------------------------------------------
// setChannel
//-------------------------------------------------------------

void GuiImage::setChannel( int channel ){
  if( m_settings_xfer ){
    DataReference *d=getDataPoolSubRef( m_settings_xfer, "channel" );
    if( d ){
      d->SetValue( channel );
      BUG_DEBUG( "setChannel " << d->fullName( true ) << " " << channel );
    }
    delete d;
  }
  m_channel = channel;
  MFMImageMapper::setChannel( this, m_direction * 100 + m_channel );
  updateLabel();
}

void GuiImage::setSettings( XferDataItem * settings ){
  m_settings_xfer = settings;
  m_settings_xfer -> setDimensionIndizes();
}

void GuiImage::setChannel( int dir, int channel ){
  if( m_settings_xfer ){
    DataReference *d=getDataPoolSubRef( m_settings_xfer, "direction" );
    d->SetValue( dir );
    delete d;
  }
  m_direction = dir;
  setChannel( channel );
}

void GuiImage::setMapping(int m){
  if( m_settings_xfer ){
    DataReference *d=getDataPoolSubRef( m_settings_xfer, "mapping" );
    if( d ){
      d->SetValue( m );
    }
    delete d;
  }
  setAutoRange( m!=1 );
  m_mapping=m;
}

int GuiImage::getMapping() {
  if( m_settings_xfer ){
    DataReference *d=getDataPoolSubRef( m_settings_xfer, "mapping" );
    int m;
    if( d && d->GetValue( m ) ){
      setAutoRange( m!=1 );
      m_mapping = m;
    }
    delete d;
  }
  return m_mapping;
}

//-------------------------------------------------------------
// setImageScale
//-------------------------------------------------------------
void GuiImage::setImageScale( int scan_width, int scan_height ) {
  if ( scan_width == 0 && scan_height == 0 ) {
    return;
  }

  double xscale=1, yscale=1;

  setScanSize( scan_width, scan_height );

  xscale = (double)m_width/scan_width;
  yscale = (double)m_height/scan_height;
  double scale = xscale;
  if( xscale > yscale )
    scale = yscale;

  setScaleX( scale );
  setScaleY( scale );
  if( isManualScale() ){
    xscale = getManualScaleX();
    yscale = getManualScaleY();

    int w = static_cast<int>( scan_width * xscale );
    int h = static_cast<int>( scan_height * yscale );
    if( w > 4000 ){ w=4000; }
    if( h > 4000 ){ h=4000; }
    if( w < 10 ) { w=10; }
    if( h < 10 ) { h=10; }

    xscale = (double)w/scan_width;
    yscale = (double)h/scan_height;

    // round to two digits:
    double TOLERANZ(0.000001);
    xscale = floor(xscale * 100. + 0.5 + TOLERANZ) / 100.;
    yscale = floor(yscale * 100. + 0.5 + TOLERANZ) / 100.;

    w=static_cast<int>( scan_width * xscale );
    h=static_cast<int>( scan_height * yscale );
    resize( w, h );
    setManualScale( xscale, yscale );
  }
}

//-------------------------------------------------------------
// initChannel
//-------------------------------------------------------------
void GuiImage::initChannel(){
  if( m_settings_xfer ){
      DataReference *d=getDataPoolSubRef( m_settings_xfer, "direction" );
      if( d && d->GetValue( m_direction ) ){
	  setDirection( m_direction );
      }
      delete d;
      d=getDataPoolSubRef( m_settings_xfer, "channel" );
      if( d && d->GetValue( m_channel ) ){
	  setChannel( m_channel );
      }
      delete d;
  }
}

//-------------------------------------------------------------
// redraw
//-------------------------------------------------------------

void GuiImage::redraw( bool clearFlag ){
//    BUG_DEBUG( "GuiImage::redraw( " << clearFlag << ")" );
  const MFMImageMapper::ImageData *image_data =
    MFMImageMapper::getImageData( this );
  resetMinMax();
// always ignored:  clear( clearFlag );
  if( image_data ){
    showImage( image_data, getId() );
    return;
  }
}

//-------------------------------------------------------------
// setDirection
//-------------------------------------------------------------

void GuiImage::setDirection( int d ){
  m_direction = d;
  MFMImageMapper::setChannel( this, d * 100 + m_channel );
}

//-------------------------------------------------------------
// getProcessing
//-------------------------------------------------------------
DataProcessing *GuiImage::getProcessing( const MFMImageMapper::ImageData *img ){
    getProcessingIndex();
    if( m_processingVector[m_processingIndex]!=0 ){
        m_processingVector[m_processingIndex] -> setImage( img );
    }
    return m_processingVector[m_processingIndex];
}

//-------------------------------------------------------------
// getProcessingIndex
//-------------------------------------------------------------
int GuiImage::getProcessingIndex( ){
    if( m_settings_xfer ){
	DataReference *d=getDataPoolSubRef( m_settings_xfer, "processing" );
	int p;
	if( d && d->GetValue( p ) ){
	  m_processingIndex = p;
	}
	delete d;
    }

  return m_processingIndex;
}

//-------------------------------------------------------------
// setProcessing
//-------------------------------------------------------------

void GuiImage::setProcessing( int index ){
  m_processingIndex=index;
  if( m_settings_xfer ){
      DataReference *s=getDataPoolSubRef( m_settings_xfer, "processing" );
      if( s ){
	  s->SetValue( index );
      }
      delete s;
  }
}

//-------------------------------------------------------------
// setFocus
//-------------------------------------------------------------

void GuiImage::setFocus(){
  GuiImage *prev=s_hasFocus;
  s_hasFocus=this;
  if( prev ) prev->setFocus( false );
  setFocus( true );
}

//-------------------------------------------------------------
// getChannelList
//-------------------------------------------------------------

int GuiImage::getChannelList( std::map<const int, std::string>&chl ){
  s_channelList.clear();
  getDataPool( "MFM_channel", s_channelList );
  chl.clear();
  chl = s_channelList;
  return s_channelList.size();
}

//-------------------------------------------------------------
// getDirectionList
//-------------------------------------------------------------

int GuiImage::getDirectionList( std::vector<std::string>&dl ){
  return getDataPool( "MFM_direction", dl );
}

//-------------------------------------------------------------
// getChannelName
//-------------------------------------------------------------

const std::string &GuiImage::getChannelName() {
  if( getDataPool( "MFM_channel", m_channel, m_channelName )==0 ){
    std::ostringstream o;
    o<< m_channel;
    m_channelName =o.str();
  }
  return m_channelName;
}

//-------------------------------------------------------------
// getChannelUnit
//-------------------------------------------------------------

const std::string &GuiImage::getChannelUnit(){
  getDataPool( "MFM_channel_unit", getChannel(), m_channelUnit );
  return m_channelUnit;
}

//-------------------------------------------------------------
// getChannelUnitFactor
//-------------------------------------------------------------

double GuiImage::getChannelUnitFactor(){
  double unitfactor=1;
  getDataPool( "MFM_channel_unit_factor", m_channel, unitfactor );
  return unitfactor;
}

//-------------------------------------------------------------
// update
//-------------------------------------------------------------

bool GuiImage::update( TransactionNumber trans ){
  bool updated( false );
  static TransactionNumber static_trans[2]={0,0};
  DataReference *ref = DataPoolIntens::Instance().getDataReference("MFM_channel");
  bool newlabel( false );

  if( ref != 0 ){
    if( ref->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
      if( trans != static_trans[0] ){
	static_trans[0] = trans;
	rebuildDialogChannelBox();
      }
      newlabel = true;
    }
  }

  delete ref;
  ref = NULL;
  ref = DataPoolIntens::Instance().getDataReference("MFM_channel_unit");
  if( ref != 0 ){
    if( ref->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
      if( trans != static_trans[1] ){
	static_trans[1] = trans;
	updateDialogUnit();
      }
      updateUnit();
    }
  }
  delete ref;

  if( m_settings_xfer != 0 ){
    if( m_settings_xfer->isUpdated( trans ) ){
	// should we check it individually?
      int channel;
      DataReference *d = getDataPoolSubRef( m_settings_xfer, "channel" );
      if( d && d->GetValue( channel ) ){
	  if( channel >= 0 && channel <= 99 ){
	      m_channel = channel;
	      updated = true;
	  }
      }
      delete d;
      d = getDataPoolSubRef( m_settings_xfer, "direction" );
      int dir;
      if( d && d->GetValue( dir ) ){
	  if( dir == 0 || dir == 1 ){
	      m_direction = dir;
	      updated = true;
	  }
      }
    }
  }

  if( updated ){
    newlabel = true;
    setDirection( m_direction );
//     setDialogImage();
    newlabel = true;
    redraw( false );
  }
  if( newlabel ){
    updateLabel();
  }
  return updated;
}

/* --------------------------------------------------------------------------- */
/* isUpdated --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiImage::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  BUG(BugGui, "GuiImage::hasChanged");
  DataReference *ref =
    DataPoolIntens::Instance().getDataReference("MFM_channel");
  if( ref != 0 )
    if( ref->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
      BUG_EXIT("item '"<<ref->fullName(true) <<"' has Changed ");
      delete ref;
      return true;
    }
  delete ref;
  ref =DataPoolIntens::Instance().getDataReference("MFM_channel_unit");
  if( ref != 0 )
    if( ref->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
      BUG_EXIT("item '"<<ref->fullName(true) <<"' has Changed ");
      delete ref;
      return true;
    }
  delete ref;

  if( m_settings_xfer != 0 )
    if( m_settings_xfer->isUpdated( trans ) ) {
      BUG_EXIT("settings have Changed ");
      return true;
    }

  return false;
}

//-------------------------------------------------------------
// setManualScale
//-------------------------------------------------------------

void GuiImage::setManualScale(bool flag ){
  if( m_settings_xfer ){
      DataReference *s=getDataPoolSubRef( m_settings_xfer, "scaling" );
      if( s ){
	  s->SetValue( (int)flag );
      }
      delete s;
  }
  m_manual_scale=flag;
}

bool GuiImage::isManualScale(){
    if( m_settings_xfer ){
	DataReference *d=getDataPoolSubRef( m_settings_xfer, "scaling" );
	int flag;
	if( d && d->GetValue( flag ) ){
	    m_manual_scale = (flag!=0);
	}
	delete d;
    }
    return m_manual_scale;
}
//-------------------------------------------------------------
// setManualScale
//-------------------------------------------------------------

void GuiImage::setManualScale( double x, double y ){
  setManualScaleX( x );
  setManualScaleY( y );
}

//-------------------------------------------------------------
// set/getWeight
//-------------------------------------------------------------

void GuiImage::setWeight( double w ){
  if( m_settings_xfer ){
      DataReference *s=getDataPoolSubRef( m_settings_xfer, "weight" );
      if( s ){
	  s->SetValue( w );
      }
      delete s;
  }
  m_weight=w;
}
double GuiImage::getWeight(){
    if( m_settings_xfer ){
	DataReference *d=getDataPoolSubRef( m_settings_xfer, "weight" );
	double w;
	if( d && d->GetValue( w ) ){
	    m_weight = w;
	}
	delete d;
    }
    return m_weight;
}

void GuiImage::setManualScaleX( double x ){
  if( m_settings_xfer ){
      DataReference *s=getDataPoolSubRef( m_settings_xfer, "x" );
      if( s ){
	  s->SetValue( x );
      }
      delete s;
  }
  m_scale_x_manual=x;
}
double GuiImage::getManualScaleX(){
    if( m_settings_xfer ){
	DataReference *d=getDataPoolSubRef( m_settings_xfer, "x" );
	double x;
	if( d && d->GetValue( x ) ){
	    m_scale_x_manual = x;
	}
	delete d;
    }
    return m_scale_x_manual;
}

void GuiImage::setManualScaleY( double y ){
  if( m_settings_xfer ){
      DataReference *s=getDataPoolSubRef( m_settings_xfer, "y" );
      if( s ){
	  s->SetValue( y );
      }
      delete s;
  }
  m_scale_y_manual=y;
}
double GuiImage::getManualScaleY(){
    if( m_settings_xfer ){
	DataReference *d=getDataPoolSubRef( m_settings_xfer, "y" );
	double y;
	if( d && d->GetValue( y ) ){
	    m_scale_y_manual = y;
	}
	delete d;
    }
    return m_scale_y_manual;
}

void GuiImage::setManualRange( double min, double max ){
  if( m_settings_xfer ){
      DataReference *s=getDataPoolSubRef( m_settings_xfer, "min" );
      if( s ){
	  s->SetValue( min );
      }
      delete s;
      s=getDataPoolSubRef( m_settings_xfer, "max" );
      if( s ){
	  s->SetValue( max );
      }
  }
  m_range_min_manual = min;
  m_range_max_manual = max;
}

double GuiImage::getManualRangeMin(){
    if( m_settings_xfer ){
	DataReference *d=getDataPoolSubRef( m_settings_xfer, "min" );
	double min;
	if( d && d->GetValue( min ) ){
	    m_range_min_manual = min;
	}
	delete d;
    }
    return m_range_min_manual;
}

double GuiImage::getManualRangeMax(){
    if( m_settings_xfer ){
	DataReference *d=getDataPoolSubRef( m_settings_xfer, "max" );
	double max;
	if( d && d->GetValue( max ) ){
	    m_range_max_manual = max;
	}
	delete d;
    }
    return m_range_max_manual;
}

/** calculate statistics values (min/max/avg/stddev)
 * @return true if range has changed
 */
bool GuiImage::updateImageStatistics( const MFMImageMapper::ImageData *img, int direction ){
  DataProcessing *dproc=getProcessing(img);

  double imgmin=getMinValue();
  double imgmax=getMaxValue();
  MFMImageMapper::ImageData::Image::const_iterator iter;

  double sum=0;
  double sqrsum=0;
  double count=0;

  for( iter = img->begin(); iter != img->end(); ++iter ){
      dproc->setLine((*iter).first);
      if( (*iter).second.empty() ) continue;
      LineStatistics m=std::for_each( (*iter).second.begin(),
				      (*iter).second.end(),
				      LineStatistics(dproc) );
      double max=m.getMax();
      double min=m.getMin();
      sum += m.getSum();
      sqrsum += m.getSqrSum();
      count += m.getSize();
      if( max > imgmax ) imgmax=max;
      if( min < imgmin ) imgmin=min;
  }
  bool new_range =
      (getMaxValue() < imgmax ) ||
      (getMinValue() > imgmin );

//  std::cerr << "updateImageStatistics count="
//   << count << " sum=" << sum << std::endl;

  setMinValue( imgmin );
  setMaxValue( imgmax );

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
      if( direction < 1 ){ // this is the standard case
	  setAverage( avg );
	  if( count > 1 ){
	      setStdDev( stddev );
	  }
      }
      else { // only relevant for line plots
	  setAverage( ( getAverage() + avg ) / 2. );
	  setStdDev( sqrt( pow(getStdDev(),2.)+ pow(stddev,2.) ) );
      }
  }
  return new_range;

}
