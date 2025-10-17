#ifndef PLUGIN_H
#define PLUGIN_H

#include <iostream>
#include <string>

#include "job/JobAction.h"
#include "app/PluginInterface.h"
#include "app/HardCopyListener.h"
#include "operator/RequestInterface.h"
#include "operator/SubscribeInterface.h"
#include "app/SmartIntensInterface_Impl.h"
class HardCopyListener;
class SmartPluginInterface;

/// Class Plugin is an adapter to a plugin (sharedobject)
class Plugin : public GuiElement
             , public JobAction
             , public HardCopyListener
             , public RequestInterface
             , public SubscribeInterface
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
protected:
  Plugin(const std::string &name, void *hModule, void *pObj);
  Plugin(const std::string &name, SmartPluginInterface* plugin);
public:
  ~Plugin();
  virtual Plugin *getPlugin() { return this; }
  void test();

/*=============================================================================*/
/* public function of HardCopyListener                                         */
/*=============================================================================*/
 public:
  virtual FileFormat getFileFormat();
  virtual const std::string &getXSLFileName(){ return m_xslFileName.empty() ? m_pObj->getName() : m_xslFileName; }
  virtual const std::string &getName(){ return m_pObj->getName(); }
  virtual void setXSLFileName( const std::string &xslFileName ){ m_xslFileName = xslFileName; }
  virtual bool isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat );
  virtual const std::string &MenuLabel();
  virtual bool write( InputChannelEvent &event );
  virtual bool write( const std::string &fileName ){ return false; }
  virtual bool write( std::ostream &os );
  virtual bool getDefaultSettings( HardCopyListener::PaperSize &size,
                                   HardCopyListener::Orientation &orient,
                                   int &quantity,
                                   HardCopyListener::FileFormat &fileFormat,
                                   double &lm, double &rm,
                                   double &tm, double &bm );
  virtual bool saveFile( GuiElement * ) { return false; }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  static Plugin* open( const std::string &name );
  std::string getDescription();
  int initialize();
  void* getBaseWidget() { return m_pluginElement; }
  int update();

  /// set arguments
  void clearArguments() { m_args.clear(); }
  void setArguments( const std::vector<std::string> &args ) { m_args = args; }
  void setArgument( const std::string &arg ) { m_args.push_back( arg ); }

  int execute();

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Plugin; }
  virtual void create();
  virtual void manage();
  virtual bool destroy() { return false; }
  virtual void getSize( int &x, int &y, bool hint=true);
  virtual void update( UpdateReason );
  virtual bool isShown();

/*=============================================================================*/
/* member functions of JobAction                                               */
/*=============================================================================*/
public:
  virtual void printLogTitle( std::ostream &ostr ){}
  virtual void startJobAction();
  virtual void stopJobAction() {}
  void setExitCode( JobResult r );
protected:
  virtual void backFromJobController( JobResult rslt ) {}

private:
  //-------------------------------------------------
  // member functions of RequestInterface
  //-------------------------------------------------
  void startRequest();
  bool checkRequest();
  void terminateRequest() {}
  std::string getErrorMessage() const;

  //-------------------------------------------------
  // member functions of SubscribeInterface
  //-------------------------------------------------
  void doSubscribe();
  bool checkSubscribe();

 public:
  bool getSubscribeData(const std::string& header, std::vector<Stream*>& streams, JobFunction*& func);

  /*=============================================================================*/
  /* public function for SmartPluginInterface                                    */
  /*=============================================================================*/
 public:
  virtual bool messageQueueSubscribe(const std::string& header, const std::vector<std::string> &dataList);
  virtual void attach( GuiElement * );

 private:
  class SubscribeData {
  public:
  SubscribeData(const std::string& header, const std::vector<Stream*>& streams, JobFunction* func) :
    m_header(header)
      , m_out_streams(streams)
      , m_function(func)
    {}
    JobFunction    *m_function;
    std::vector<Stream*>  m_out_streams;
    std::string           m_header;

};
/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
  void*            m_hModule; // Module-Handle
  void*           m_pluginElement;
  PluginInterface          *m_pObj;    // Plugin-Object
  SmartPluginInterface     *m_pSmartObj;    // Plugin-Object
  SmartIntensInterface_Impl m_intensInterface;
  std::vector<SubscribeData*> m_subscriberData;
  GuiElementList            m_childElements;
  std::vector<std::string>  m_args;
  std::string               m_xslFileName;
  int                       m_width;
  int                       m_height;

  friend class ModuleFactory;
};

#endif
