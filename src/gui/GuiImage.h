
#if !defined GUI_IMAGE_INCLUDED_H
#define GUI_IMAGE_INCLUDED_H

#include<limits>
#include "app/HardCopyListener.h"
#include "gui/GuiMFMImageMapper.h"

class GuiPlotDataItem;
class XferDataItem;
class GuiImage;
class DataProcess;
class DataProcessing;

class GuiImage {
  //-------------------------------------------------------
  // Constructor / Destructor
  //-------------------------------------------------------
public:
  /** Constructor
   * @param f size of frame in pixels
   */
  GuiImage( int f );

  virtual ~GuiImage(){}

  enum Type{
    e_Plot
    , e_Image
  };

  //-------------------------------------------------------
  // public functions
  //-------------------------------------------------------
 public:
  virtual std::string getTitle()=0;
  Type getType(){ return m_type; };
  void setAutoRange(bool flag){ m_auto_range=flag; }
  void setManualScale(bool flag );
  bool isManualScale();
  virtual void setRange( bool, double, double )=0;
  virtual void clear( bool )=0;
  void setChannel( int type, int channel );
  void setChannel( int channel );
  int getChannel()const{ return m_channel; }
  int getId(){ return m_direction * 100 + m_channel; }
  void setDirection( int d );
  int getDirection()const{ return m_direction; }
  void setSettings( XferDataItem * settings );
  void setProcessing( int index );
  DataProcessing *getProcessing( const MFMImageMapper::ImageData *img=0 );
  int getProcessingIndex( );
  void setMapping( int m );
  void setFocus();
  virtual void setFocus( bool )=0;
  bool hasFocus()const{ return this == s_hasFocus || s_hasFocus==0; }
  int getMapping();
  virtual GuiElement* getElement() = 0;
  virtual void setLine( int id, int line,
			const MFMImageMapper::ImageData::Line &data,
			const MFMImageMapper::ImageData *img ) = 0;
  virtual void resize( int w, int h ){}
  virtual void resize(){}
  void setSize ( const int width, const int height ){
    m_width = width;
    m_height = height;
    m_imageWidth = width;
    m_imageHeight = height;
  }
  void getSize( int &w, int &h )const{
    w=m_width;
    h=m_height;
  }

  void setImageScale( int scan_width, int scan_height );

  void setImageSize( int w, int h ){
    m_imageWidth = w;
    m_imageHeight = h;
  }

  void setScanSize( int w, int h ){
    m_scanWidth = w;
    m_scanHeight = h;
  }
  void getImageSize( int &w, int &h ){
    w = m_imageWidth;
    h = m_imageHeight;
  }
  void getScanSize( int &w, int &h ){
    w = m_scanWidth;
    h = m_scanHeight;
  }

  int getImageWidth( )const{ return m_imageWidth; }
  int getImageHeight( )const{ return m_imageHeight; }

  double getMinValue()const{ return m_min_value; }
  double getMaxValue()const{ return m_max_value; }
  double getManualRangeMin();
  double getManualRangeMax();
  void setManualRange(double min, double max);
  bool getAutoRange()const{ return m_auto_range; }
  int getScanWidth()const{ return m_scanWidth; }
  int getScanHeight()const{ return m_scanHeight; }

  double getRangeMax(){
      switch ( m_mapping ) {
	  case 0:
	      return getMaxValue();
	  case 1:
	      return m_range_max_manual;
	  case 2:
	      return getAverage() + getWeight()*getStdDev();
      }
      return  1;
  }
  double getRangeMin(){
      switch ( m_mapping ){
	  case 0:
	      return getMinValue();
	  case 1:
	      return m_range_min_manual;
	  case 2:
	      return getAverage()-getWeight()*getStdDev();
      }
      return  0;
  }
  void setMinValue( double value ){
    m_min_value = value; }
  void setAverage( double value ){ m_avg = value;}
  double getAverage(  )const{ return m_avg; }
  void setStdDev( double value ){ m_stddev = value;}
  double getStdDev(  )const{ return m_stddev; }
  void setMaxValue( double value ){
    m_max_value = value; }
  void resetMinMax(){
    setMinValue( std::numeric_limits<double>::max() );
    setMaxValue( -std::numeric_limits<double>::max() );
  }
  void setScaleX( double s ){ m_scale_x = s; }
  void setScaleY( double s ){ m_scale_y = s; }
  void setManualScale( double x, double y );
  double getScaleX( ){
    return (isManualScale()? getManualScaleX() : m_scale_x);
  }
  double getScaleY( ){
    return (isManualScale()? getManualScaleY() : m_scale_y);
  }
  double getAutoScaleX( )const{ return m_scale_x;}
  double getAutoScaleY( )const{ return m_scale_y; }
  double getManualScaleX();
  double getManualScaleY();
  void setManualScaleX( double );
  void setManualScaleY( double );
  double getWeight();
  void setWeight( double w );
  const std::string &getChannelName();
  const std::string &getChannelUnit();
  double getChannelUnitFactor();
  static int getChannelList( std::map<const int, std::string> &chl );
  static int getDirectionList( std::vector<std::string> &tpl );

  virtual void redraw( bool clear = true );
  virtual void showImage( const MFMImageMapper::ImageData *img, int id )=0;
  static void setOnViewAction( JobFunction *func ){
    s_on_view_action = func;
  }
  static JobFunction *getOnViewAction(){
    return s_on_view_action;
  }
  virtual void clearView(){}
  static GuiImage* currentImage(){
    return s_hasFocus;
  }
  virtual void updateLabel() = 0;
  virtual void updateUnit() = 0;
  static void setEos( bool e ){ s_eos = e; }
  static void setGeneralScan(){ s_general_scan = true; s_image_scan = false; }
  static void setImageScan(){ s_image_scan = true; s_general_scan = false; }
  static bool imageScan(){ return s_image_scan; }
  static bool eos(){ return s_eos; }
  static std::vector<GuiImage*> &getImages(){ return s_images; }
  virtual void setContentsPos( double x, double y ){}
  bool update( TransactionNumber trans );
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);
  void initChannel();
  virtual void updateDialogUnit()=0;
  virtual void setDialogImage()=0;
  virtual void rebuildDialogChannelBox()=0;

 protected:
  virtual bool updateImageStatistics( const MFMImageMapper::ImageData *img, int direction=0 );
  //-------------------------------------------------------
  // public members
  //-------------------------------------------------------
private:
  int m_width;
  int m_height;
  int m_imageWidth;
  int m_imageHeight;
  int m_scanWidth;
  int m_scanHeight;
  int m_channel;
  XferDataItem *m_settings_xfer;
  std::string m_channelName;
  std::string m_channelUnit;
  int m_direction;
  bool m_auto_range;
  bool m_manual_scale;
  int m_mapping;

  double m_min_value;
  double m_max_value;
  double m_avg;
  double m_stddev;
  double m_weight;

  double m_scale_x;
  double m_scale_y;

  double m_range_min_manual;
  double m_range_max_manual;
  double m_scale_x_manual;
  double m_scale_y_manual;
  std::vector<DataProcessing *>m_processingVector;
  int m_processingIndex;

  static GuiImage *s_hasFocus;
  static std::map<const int, std::string> s_channelList;
  static JobFunction *s_on_view_action;

  static bool s_general_scan;
  static bool s_image_scan;
  static bool s_eos;
protected:
  Type m_type;
  static std::vector<GuiImage*> s_images;
};


#endif
