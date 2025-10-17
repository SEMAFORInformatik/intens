
#ifndef INTERFACES_H
#define INTERFACES_H

#include <QtPlugin>
#include <vector>
#include <string>

class Plugin;
class SmartIntensInterface;

/**
Smart Intens Interface plugin
*/
class SmartPluginInterface
{
public:
  virtual ~SmartPluginInterface() {}

  virtual std::string getDescription() = 0;
  virtual const std::string &getName() = 0;
  virtual void* createWindow( void* parent, int *width, int *height ) = 0;
  virtual bool insertSubWindow( void* widget, int xpos, int ypos, int xrange=-1, int yrange=-1 ) = 0;
  virtual bool messageQueueSubscribe(const std::string& header, const std::vector<std::string> &dataList) { return true; }
  virtual bool requestData(const std::string& header,
			   const std::vector<std::string> &inDataList,
			   std::vector<std::string> &outDataList) { return true; };
  virtual bool subscribeData(const std::string& header) { return true; }
  virtual bool initialize( SmartIntensInterface& iFace ) = 0;
  // virtual int update() = 0;
  /* virtual int execute( Plugin *p, const std::vector<std::string> &args ) = 0; */

};

#ifdef HAVE_QT
QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(SmartPluginInterface,
                    "ch.semafor.intens.SmartPluginInterface/1.0")
QT_END_NAMESPACE
#endif
#endif
