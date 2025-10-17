
#include <limits>
#include <QSettings>
#include <cmath>

#include "gui/qt/GuiQtMinMaxColorMap.h"
#include "gui/qt/GuiQtManager.h"

// ******************************************************************
// Class ColorScale
// ******************************************************************
ColorScale* ColorScale::pinstance = 0;

ColorScale* ColorScale::Instance(){
  if ( pinstance == 0){
    pinstance = new ColorScale;
  }
  return pinstance;
};
ColorScale::ColorScale() : m_colors(22){
  std::string abbreviation;
  for ( int i=0; i < m_colors.size(); ++i ) {
    switch( i ) {
    case 0:  abbreviation = "nd"; break;
    case 1:  abbreviation = "ld"; break;
    case 2:  abbreviation = "di"; break;
    case 3:  abbreviation = "i3"; break;
    case 4:  abbreviation = "i4"; break;
    case 5:  abbreviation = "i5"; break;
    case 6:  abbreviation = "i6"; break;
    case 7:  abbreviation = "i7"; break;
    case 8:  abbreviation = "ea"; break;
    case 9:  abbreviation = "sx"; break;
    case 10: abbreviation = "sy"; break;
    case 11: abbreviation = "sa"; break;
    case 12: abbreviation = "sp"; break;
    case 13: abbreviation = "df"; break;
    case 14: abbreviation = "tz"; break;
    case 15: abbreviation = "default"; break; // 15 is the empty channel -> used as default
    case 16: abbreviation = "pd"; break;
    case 17: abbreviation = "ps"; break;
    case 18: abbreviation = "ts"; break;
    case 19: abbreviation = "hx"; break;
    case 20: abbreviation = "hy"; break;
    case 21: abbreviation = "hz"; break;
    }

    std::string defaultColors = "0 255 255 0";
    std::vector< std::vector<int> > component(3, std::vector<int>(256,0));
    int ix;
    std::string color[] = {"red", "green", "blue"};
    for ( int comp=0; comp < 3; ++comp ) {
      std::string setting ("Colors/imageColorScale."+abbreviation+"."+color[comp]) ;
      std::string generalSetting ("Colors/imageColorScale."+color[comp]) ;
      QString s;
      s =  GuiQtManager::Settings()->value
        ( setting.c_str(),
          GuiQtManager::Settings()->value
          ( generalSetting.c_str(),
            defaultColors.c_str()
            ).toString()
          ).toString();

      std::istringstream is( s.toStdString() );
      int index0;
      double value0;
      is >> index0 >> value0; // first
      assert ( index0 >= 0 && index0 <= 255 );
      assert ( value0 >= 0 && value0 <= 255 );

      ix=0;
      while ( ix < index0 ) {
        component[comp][ix] = 255 - (int)value0;
        ix++;
      }

      int index;
      double value;
      double factor;
      while ( is >> index >> value ) {
        assert ( index >= 0 && index <= 255 );
        assert ( value >= 0 && value <= 255 );

        factor = 255.9999 / 255 / (index-index0);
        while ( ix < index ) {
          component[comp][ix] = 255 - (int)floor( ((index-ix)*value0 + (ix-index0)*value) * factor);
          ++ix;
        }
        index0 = index;
        value0 = value;
      }
      while ( ix < 256 ) {
        component[comp][ix] = 255 - (int)value;
        ix++;
      }
    }

    for ( ix=0; ix < 256; ++ix ) {
      m_colors[i].push_back( QColor( component[0][ix], component[1][ix], component[2][ix] ).rgb() );
    }
  }
};

const std::vector<QRgb> & ColorScale::getColors(int channel) {
  if ( channel >= m_colors.size() ) {
    channel = 15; // 15 is the empty channel -> used as default
  }
  assert ( channel >= 0 && channel < m_colors.size() );
  return m_colors[channel];
}

// ******************************************************************
// Class MinMaxColorMap
// ******************************************************************
MinMaxColorMap::MinMaxColorMap( double min, double max, int channel ):
  _min(min)
  ,_max(max)
  ,_color(ColorScale::Instance()->getColors(channel)){
  _d=max-min;
  if( fabs(_d) < 2*std::numeric_limits<double>::epsilon() ){
    _d = 2*std::numeric_limits<double>::epsilon();
  }
  _d = 255.9999 / _d; // not 256 to avoid i=256 for value=max !!
}

unsigned int MinMaxColorMap::operator()( double value )const{
  if( std::isnan(value) ) {
    return GuiQtManager::imageHighlightColor().rgb();
  }
  else if( value - _min < -1e-15 ){
    return GuiQtManager::imageLowerAlarmColor().rgb();
  }
  else if( value - _max > 1e-15 ){
    return GuiQtManager::imageUpperAlarmColor().rgb();
  }
  else {
    int i = (int)( ( value - _min ) * _d );
    return _color[i];
  }
}
