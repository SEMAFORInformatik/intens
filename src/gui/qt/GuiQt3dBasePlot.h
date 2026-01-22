#ifndef GUIQT3DBASEPLOT_H_
#define GUIQT3DBASEPLOT_H_

class QAbstract3DAxis;
class GuiQt3dData;

class GuiQt3dBasePlot{
  /*=============================================================================*/
  /* Interface                                                                   */
  /*=============================================================================*/
 public:
  /** update data
      @param data plot data
  */
  virtual void update(GuiQt3dData& data) = 0;

  /** returns axis
      @param atype axis type
      @return pointer to axis
  */
  virtual QAbstract3DAxis *axisWidget(GuiElement::GuiAxisType atype) const = 0;

  virtual void printLog() = 0;
  virtual QWidget* myWidget() = 0;
};
#endif
