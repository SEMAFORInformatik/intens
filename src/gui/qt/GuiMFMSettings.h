
#if !defined(GUI_MFM_SETTINGS_H)
#define GUI_MFM_SETTINGS_H

#include <qobject.h>
#include <map>
#include <vector>
#include "xfer/XferConverter.h"

class GuiImage;
class QDialog;
class QLineEdit;
class QCheckBox;
class QGridLayout;
class QComboBox;
class QLabel;

class DataProcessing;
class ArrowKeyLineEdit;

class GuiMFMSettings : public QObject{
  Q_OBJECT
private:
  GuiMFMSettings();
public:
  ~GuiMFMSettings();
public:
  bool isLinked();
  void setActiveWindow();
  static GuiMFMSettings &getDialog();
  void openDialog( QWidget *parent );
  void setImage( GuiImage *, const std::string &name );
  void setMappingRange( double min, double max );
  void setAverage( double avg );
  void setWeight( double w );
  void setStdDev( double s );
  void setCurrentRange( double min, double max );
  void setScale( bool is_manual, double auto_x, double auto_y,
		 double manual_x, double manual_y );
  void setChannel( int dir, int channel );
  void setProcessing( int index );
  void setUnits( const std::string &units );
  void setFactor( double f ){ m_factor=f; };
  void setMapping( int m );
  int getMapping( );
  void rangeHasChanged();
  void weightHasChanged();
  void scaleHasChanged();
  void hide();
  void rebuildChannelBox();
  void createChannelBox( int row=0 );

  private slots:
  void slot_lostFocusRange();
  void slot_lostFocusScale();
  void slot_lostFocusWeight();
  void slot_scale_toggled( bool );
  void slot_link_toggled( bool );
  void slot_activateChannel( int );
  void slot_activateProcessing( int );
  void slot_activateMapping( int );

private:
  void createMappingBox( int row );
  void createDirectionBox( int row );
  void createProcessingBox( int row );
  void resizeImage( GuiImage*);
  void setFormattedValue( RealConverter &c, QLineEdit *w, double x );

  static GuiMFMSettings *s_instance;
  QDialog               *m_dialog;
  QGridLayout           *m_grid;
  QLabel                *m_titleLabel;
  GuiImage              *m_image;
  QLineEdit             *m_currentMin;
  QLineEdit             *m_currentMax;
  QLineEdit             *m_avg;
  ArrowKeyLineEdit             *m_weight;
  QLineEdit             *m_stddev;
  QLineEdit             *m_channel;
  QLineEdit             *m_direction;
  ArrowKeyLineEdit             *m_mappingMin;
  ArrowKeyLineEdit             *m_mappingMax;
  QLineEdit             *m_scale_x;
  QLineEdit             *m_scale_y;
  ArrowKeyLineEdit             *m_scale_x_manual;
  ArrowKeyLineEdit             *m_scale_y_manual;
  std::vector<QLabel*>  m_units;
  QCheckBox             *m_link_checkbox;
  QCheckBox             *m_scale_checkbox;
  QComboBox             *m_directionBox;
  QComboBox             *m_channelBox;
  QComboBox             *m_processingBox;
  QComboBox             *m_mappingBox;

  std::map<const int, int>       m_channelMap;
  std::vector<int>       m_directionVector;
  double m_factor;
  RealConverter          m_converter;
  RealConverter          m_scaleConverter;
};

#endif
