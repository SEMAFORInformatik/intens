#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H
#include <string>
#include <vector>
#include "gui/GuiElement.h"
#include "HardCopyListener.h"

class Plugin;
class UIComponent;
class IntensInterface;
class PluginInterface {
public:
  virtual std::string getDescription() = 0;
  virtual const std::string &getName() = 0;
/*   virtual Widget createWindow( Widget parent, Dimension *width, Dimension *height ) = 0; */
  virtual void* createWindow( GuiElement* parent, int *width, int *height ) = 0;
  virtual int initialize( IntensInterface* iFace ) = 0;
  virtual int update(  GuiElement::UpdateReason reason = GuiElement::reason_Always ) = 0;
  virtual int execute( Plugin *p, const std::vector<std::string> &args ) = 0;
  virtual bool printDiagram() = 0;
  virtual HardCopyListener::FileFormat getFileFormat() = 0;
  virtual bool isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat ) = 0;
  virtual bool write( std::ostream &os ) = 0;
  virtual bool getDefaultSettings( HardCopyListener::PaperSize &size,
				   HardCopyListener::Orientation &orient,
				   int &quantity,
				   HardCopyListener::FileFormat &fileFormat,
				   double &lm, double &rm,
				   double &tm, double &bm ) = 0;
};

#endif
