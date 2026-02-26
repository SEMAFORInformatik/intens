#include "gui/qt/GuiQt3dData.h"
#include "gui/qt/GuiQt3dBasePlot.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* ConfigData::init --                                                         */
/* --------------------------------------------------------------------------- */
void GuiQt3dBasePlot::ConfigData::init(Gui3dPlot::Style plotStyle) {
  orthoProjection = false;
  selectionMode = 1;  // select item
  showGrid = true;
  showMesh = true;
  showSmooth = true;
  // rangeMinX = 0.;
  // rangeMaxX = 100.;
  rangeX = false;
  // rangeMinY = 0.;
  // rangeMaxY = 100.;
  rangeY = false;
  switch(plotStyle){
  case Gui3dPlot::BAR:
    plotType = "BAR";
    zoom = 80.0;
    rotationX = 45.0;
    rotationY = 45.0;
    break;
  case Gui3dPlot::SURFACE:
    plotType = "SURFACE";
    zoom = 80.0;
    rotationX = 45.0;
    rotationY = 45.0;
    break;
  case Gui3dPlot::CONTOUR:
    plotType = "CONTOUR";
    zoom = 80.0;
    rotationX = 45.0;
    rotationY = 45.0;
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* ConfigData::init --                                                         */
/* --------------------------------------------------------------------------- */
bool GuiQt3dBasePlot::ConfigData::update(const GuiQt3dData& data){
#if HAVE_QGRAPHS
  if (!rangeX){
    auto interval = data.getXInterval();
    rangeMinX = interval.min;
    rangeMaxX = interval.max;
  }
  if (!rangeY){
    auto interval = data.getYInterval();
    rangeMinY = interval.min;
    rangeMaxY = interval.max;
  }
  BUG_DEBUG("update range: " << rangeMinX << ", " << rangeMaxX);
#endif
  return (!rangeX || !rangeY);
}
