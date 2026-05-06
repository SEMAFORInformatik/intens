// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#if !defined(GUI_QT_PROGRESSINDICATOR_H)
#define GUI_QT_PROGRESSINDICATOR_H

#include <QWidget>
#include <QColor>

/**
   \class GuiQtProgressIndicator
   \brief The GuiQtProgressIndicator class lets an application display a progress indicator to show that a lengthy task is under way.

   Progress indicators are indeterminate and do nothing more than spin to show that the application is busy.
*/
class GuiQtProgressIndicator : public QWidget
{
public:
  GuiQtProgressIndicator(QWidget* parent = NULL);
  virtual ~GuiQtProgressIndicator() {};

  /** Returns a boolean value indicating is currently animated.
      \return Animation state.
  */
    bool isAnimated () const;

  /** Returns the color
  */
    const QColor & color() const { return m_color; }

    virtual QSize sizeHint() const;
    int heightForWidth(int w) const;
public:
  /** Starts the animation.
  */
  void startTimer(int delay);

  /** Stops the animation.
  */
    void stopTimer();

  /** Sets the color
   */
  void setColor(const QColor & color);
protected:
  virtual void timerEvent(QTimerEvent * event);
  virtual void paintEvent(QPaintEvent * event);
private:
  int m_angle;
  int m_timerId;
  QColor m_color;
};

#endif // QPROGRESSINDICATOR_H
