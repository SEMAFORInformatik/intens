#ifndef GUIQT3DBASEPLOT_H_
#define GUIQT3DBASEPLOT_H_

#include "gui/GuiElement.h"
#include "gui/Gui3dPlot.h"

class QAbstract3DAxis;
class GuiQt3dData;

class GuiQt3dBasePlot{
  public:
  struct ConfigData{
    ConfigData():
      rotationX(0),
      rotationY(0),
      zoom(100.)
    {}
    ConfigData(double rotationX, double rotationY, double zoom):
      rotationX(rotationX),
      rotationY(rotationY),
      zoom(zoom)
    {}
    void init(Gui3dPlot::Style plotStyle);
    bool update(const GuiQt3dData& data);
    std::string plotType;
    double rangeMinX, rangeMaxX, rangeMinY, rangeMaxY;
    bool rangeX, rangeY;
    double rotationX, rotationY, zoom;
    bool orthoProjection;
    int selectionMode;
    bool showGrid;
    bool showMesh;
    bool showSmooth;
  };
  /*=============================================================================*/
  /* Interface                                                                   */
  /*=============================================================================*/
  /** update data
      @param data plot data
  */
  virtual void update(GuiQt3dData& data) = 0;

  /** returns axis
      @param atype axis tye
      @return pointer to axis
  */
  virtual QAbstract3DAxis *axisWidget(GuiElement::GuiAxisType atype) const = 0;

  /**  print info to stdout
   */
  virtual void printLog() = 0;
  virtual void getConfigData(ConfigData& configData) = 0;
  virtual void updateConfigData(ConfigData& configData) = 0;
};
#endif
