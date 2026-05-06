 #include <limits>
#include "xfer/Scale.h"
#include "xfer/XferConverter.h"
#include "app/AppData.h"
#include "gui/qt/IntegerValidator.h"

 //  Implementation of IntegerValidator
 //
IntegerValidator::IntegerValidator ( int bottom, int top, QObject * parent, Scale* scale=0)
  : QIntValidator(bottom, top, parent)
  , m_scale(scale)
{}

IntegerValidator::~IntegerValidator () {}

void IntegerValidator::setRange ( int bottom, int top ){
  if (m_scale){
    bool    ok;
    double fac = m_scale->getValue();
    bottom *= fac;
    top *= fac;
  }
  QIntValidator::setRange(bottom, top);
}





