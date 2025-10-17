
#include <limits>
#include <QDate>
#include <QTime>
#include <QLocale>
#include <qwt_painter.h>
#include <qwt_scale_map.h>
#include <qwt_text.h>
#include <qwt_interval.h>
#include <QPainter>

#include "GuiQwtScaleDraw.h"

GuiQwtScaleDraw::GuiQwtScaleDraw(Type type, const char fmt, int fieldwidth, int prec)
  : m_format( fmt )
  , m_fieldwidth( fieldwidth )
  , m_prec( prec )
  , dType(type)
  , m_scaleConverter(fieldwidth,prec, 0, true, true)
  , m_bAnnotation(false)
  , colorMap(0) {
  resetUserScale();
}

void GuiQwtScaleDraw::setType(Type type) {
  if ( dType != type ) {
    dType = type;
    invalidateCache();  // do redraw
  }
}

bool GuiQwtScaleDraw::isDateType() const {
  if (dType == type_time ||
      dType == type_date ||
      dType == type_datetime)
    return true;

  return false;
}

void GuiQwtScaleDraw::setAnnotationType(bool bAnno) {
  m_bAnnotation = bAnno;
}

bool GuiQwtScaleDraw::isAnnotationType() const {
  return m_bAnnotation;
}

void GuiQwtScaleDraw::clearAnnotationLabels() {
  m_annoMap.clear();
}

void GuiQwtScaleDraw::setAnnotationLabel(double xval, std::string& lbl) {
  m_annoMap[xval] = lbl;
  invalidateCache();
}

std::vector<double> GuiQwtScaleDraw::getAnnotationLabelValues() {
  std::vector<double> ret;
  std::map<double, std::string>::const_iterator it = m_annoMap.begin();
  for (;it !=  m_annoMap.end(); ++it)
    ret.push_back( (*it).first );
  return ret;
}

QwtText GuiQwtScaleDraw::label(double v) const {
   std::ostringstream output;
   // annotation
   if (m_bAnnotation) {
     std::map<double, std::string>::const_iterator it = m_annoMap.find(v);
     if (it != m_annoMap.end()) {
       return QwtText( QString::fromStdString((*it).second) );
     }
   }

   // others
   switch (dType) {
    case type_time:
      {
        QTime t = QTime(0, 0, 0, 0).addMSecs(v);
        return QwtText(QLocale::system().toString(t, QLocale::ShortFormat));
      }
    case type_date:
      {
        QDate d = QDate::fromJulianDay(v);
        return QwtText(QLocale::system().toString(d, QLocale::ShortFormat));
      }
    case type_datetime:
      {
        QDate d = QDate::fromJulianDay(v);
	int value = d.toJulianDay();
 	double dMSecs = (v - value)* 86400.0 * 1000;
	QTime t = QTime(0, 0, 0, 0).addMSecs(qRound(dMSecs));
	//QDateTime dt(d, t, Qt::UTC);
	if (dMSecs == 0) {
    return QwtText(QLocale::system().toString(d, QLocale::ShortFormat));
	} else
	  return QwtText(QLocale::system().toString(d, QLocale::ShortFormat) + "\n" +
	                 QLocale::system().toString(t, QLocale::ShortFormat));
      }
    case type_real:
      // intens fromatiert
      if (m_fieldwidth) {
	m_scaleConverter.write( v, output);
	return QwtText( QString::fromStdString(output.str()));
      // default
      }
      else return  QwtScaleDraw::label(v);
#if QWT_VERSION >= 0x060000
    case type_colormap: {
      scaleMap().invTransform( v ); // tval
      QwtInterval interval = scaleDiv().interval();
      double dval = interval.minValue() + v*interval.width();
      if (fabs(dval) < 1e-10) dval=0.;
      QwtText t = QwtScaleDraw::label( dval );
      return t;
    }
#endif
    default:
      assert(false);
    }
   return QwtText();
  }
  void GuiQwtScaleDraw::setMinUserScale(double minVal) {
    minUserScale = minVal;
  }

  void GuiQwtScaleDraw::setMaxUserScale(double maxVal) {
    maxUserScale = maxVal;
  }

  void GuiQwtScaleDraw::resetUserScale() {
    minUserScale = std::numeric_limits<double>::max();
    maxUserScale = std::numeric_limits<double>::min();
  }

  void GuiQwtScaleDraw::setColorMap(const QwtLinearColorMap* colormap) {
    colorMap = colormap;
  }

  void GuiQwtScaleDraw::drawTick( QPainter *p, double val, double len ) const {

    if (!colorMap)
      QwtScaleDraw::drawTick(p, val, len);
  }

  void GuiQwtScaleDraw::drawLabel (QPainter *p, double val) const {

    if (dType != type_colormap)
      QwtScaleDraw::drawLabel(p, val);
  }

  void GuiQwtScaleDraw::drawBackbone (QPainter *p)  const {
    QwtScaleDraw::drawBackbone(p);
  }

  void GuiQwtScaleDraw::draw( QPainter *p, const QPalette &pal ) const {
    QwtScaleDraw::draw(p, pal);

#if QWT_VERSION >= 0x060000
    // if colormap type
    if (dType == type_colormap && colorMap) {
      QVector<double> colors = colorMap->colorStops();
      for (int i=0; i< colors.size(); ++i) {

        const bool roundingAlignment = QwtPainter::roundingAlignment( p );
        double val = colors.at(i);
        double len = tickLength (QwtScaleDiv::MajorTick);
        double tval = scaleMap().transform( val );
        if ( roundingAlignment )
          tval = qRound( tval );

#if QWT_VERSION < 0x060200
        const int pw = penWidth();
#else
        const int pw = penWidthF();
#endif
        switch ( alignment() )
          {
          case RightScale:
            {
              double x1 = pos().x();
              double x2 = pos().x() + pw + len;
              double x3 = pos().x() + 0.5*(pw + len);
              if ( roundingAlignment )
                {
                  x1 = qRound( x1 );
                  x2 = qRound( x2 );
                  x3 = qRound( x3 );
                }

              QwtPainter::drawLine( p, x1, tval, x2, tval );
              QPolygonF pa;
              pa << QPoint(x1, tval) << QPoint(x3, tval-0.5*len) << QPoint(x2, tval) << QPoint(x3, tval+0.5*len);
              if (colorMap) {
                QColor c = colorMap->color(QwtInterval(0,1), val);
                p->setBrush(c);
                QwtPainter::drawPolygon( p, pa );
		p->save();
		if ((val == colors.front() || val == colors.back()) &&
		    (minUserScale != std::numeric_limits<double>::max() ||
		     maxUserScale != std::numeric_limits<double>::min())  )
		  p->setPen(QColor(Qt::red));
		QwtScaleDraw::drawLabel(p, val);
		p->restore();
              }
              break;
            }
          default:
            break;
          }
      }
    }
#endif
  }
