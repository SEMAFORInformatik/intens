#include "gui/qt/GuiQtProgressIndicator.h"

#include <QPainter>

GuiQtProgressIndicator::GuiQtProgressIndicator(QWidget* parent)
  : QWidget(parent)
  , m_timerId(-1)
  , m_color(Qt::black)
{
}

bool GuiQtProgressIndicator::isAnimated () const{
    return (m_timerId != -1);
}

void GuiQtProgressIndicator::startTimer(int delay){
    m_angle = 0;

    if (m_timerId == -1)
      m_timerId = QWidget::startTimer(delay);
}

void GuiQtProgressIndicator::stopTimer(){
    if (m_timerId != -1)
        killTimer(m_timerId);

    m_timerId = -1;

    update();
}

void GuiQtProgressIndicator::setColor(const QColor & color){
  if (color.isValid())
    m_color = color;

  update();
}

QSize GuiQtProgressIndicator::sizeHint() const{
  return QWidget::sizeHint();
}

int GuiQtProgressIndicator::heightForWidth(int w) const{
    return w;
}

void GuiQtProgressIndicator::timerEvent(QTimerEvent *event){
    m_angle = (m_angle+10)%360;

    update();
}

void GuiQtProgressIndicator::paintEvent(QPaintEvent *event){
    if (!isAnimated())
        return;

    int width = qMin(this->width(), this->height());
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    int outerRadius = (width-1)*0.5;
    int innerRadius = (width-1)*0.19;

    int capsuleHeight = outerRadius - innerRadius;
    int capsuleWidth  = (width > 32 ) ? capsuleHeight *.23 : capsuleHeight *.35;
    int capsuleRadius = capsuleWidth/2;

    for (int i=0; i<12; i++)
    {
        QColor color = m_color;
        color.setAlphaF(1.0f - (i/12.0f));
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.save();
        p.translate(rect().center());
        p.rotate(m_angle - i*30.0f);
        p.drawRoundedRect(-capsuleWidth*0.5, -(innerRadius+capsuleHeight),
                          capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
        p.restore();
    }
}
