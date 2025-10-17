
#ifndef GUIQWTSCALEDRAW_H
#define GUIQWTSCALEDRAW_H

#include <map>
#include <qwt_scale_draw.h>
#include <qwt_color_map.h>

#include "xfer/XferConverter.h"

//---------------------------------------------------
// Klasse GuiQwtScaleDraw
//---------------------------------------------------
class GuiQwtScaleDraw: public QwtScaleDraw {
public:
  enum Type
  { type_time = 0
    , type_date
    , type_datetime
    , type_real
    , type_colormap
    , type_none
  };
  GuiQwtScaleDraw(Type type, const char fmt=0, int fieldwidth=-1, int prec=-1);

  virtual QwtText label(double v) const;

  void setMinUserScale(double minVal);
  void setMaxUserScale(double maxVal);
  void resetUserScale();
  void setColorMap(const QwtLinearColorMap* colormap);

  //  void setDateType(bool bDate);
  void setType(Type type);
  void setAnnotationType(bool bAnno);

  bool isDateType() const;
  bool isAnnotationType() const;

  void clearAnnotationLabels();
  void setAnnotationLabel(double xval, std::string& lbl);
  std::vector<double>  getAnnotationLabelValues();

  private:
    virtual void drawTick( QPainter *, double val, double len ) const;
    virtual void drawLabel (QPainter *, double val) const;
    virtual void drawBackbone(QPainter *) const;
    virtual void draw( QPainter *, const QPalette & ) const;
private:
  GuiQwtScaleDraw(const GuiQwtScaleDraw&);
  char           m_format;
  int            m_fieldwidth;
  int            m_prec;
  Type           dType;
  RealConverter  m_scaleConverter;
  bool           m_bAnnotation;
  std::map<double, std::string>  m_annoMap;

  double         minUserScale;
  double         maxUserScale;
  const QwtLinearColorMap* colorMap;
 };

#endif
