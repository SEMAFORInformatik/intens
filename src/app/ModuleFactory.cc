#include <iostream>
#include <string>
#include <string.h>
#include <assert.h>
#include <ltdl.h>
#include <dirent.h>

#ifdef HAVE_MATLAB
#define V5_COMPAT
#include "utils/utils.h"
#include <engine.h>
#include "operator/MatlabConnection.h"
#endif


#include "app/ModuleFactory.h"
#include "app/AppData.h"
#include "app/Plugin.h"
#include "utils/Debugger.h"
#if defined HAVE_QT && !defined HAVE_HEADLESS
#include "gui/qt/GuiQtManager.h"
#include "app/SmartPluginInterface.h"
#endif

INIT_LOGGER();

ModuleFactory *ModuleFactory::s_connection_factory=0;

extern "C" {
  DBconnection * (*create_dbconn)();
}
std::vector<lt_dlhandle> s_ltDlHandle;

/*=============================================================================*/
/* Constructor                                                                 */
/*=============================================================================*/
ModuleFactory::ModuleFactory(){
  //#ifndef MODULE_PATH
  const std::string MODULE_PATH= std::string( AppData::Instance().IntensHome() + "lib" );
  //#endif
  lt_dlinit();
  if (MODULE_PATH.size() != 0){
    int errors = lt_dladdsearchdir (MODULE_PATH.c_str());
  }
}

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */
ModuleFactory& ModuleFactory::Instance(){
  if( s_connection_factory==0 ){
    s_connection_factory = new ModuleFactory();
  }
  return *s_connection_factory;
}

/* --------------------------------------------------------------------------- */
/* createDBconnection --                                                       */
/* --------------------------------------------------------------------------- */
DBconnection* ModuleFactory::createDBconnection( const char *so ){
  lt_dlhandle hModule;
  hModule = lt_dlopenext( so );
  if (!hModule) {
    hModule = lt_dlopenext( (std::string("lib")+so).c_str() );
  }

  if( hModule==0 ){
    std::cerr << "ModuleFactory::createDBconnection: " << so << " " << lt_dlerror() << std::endl;
    return 0;
  }
  lt_ptr pObj = lt_dlsym( hModule, "create_DBconnection" );
  if(pObj == 0){
    std::cerr << "Expected symbol create_DBconnection not found.\n";
    return 0;
  }
  s_ltDlHandle.push_back(hModule);
  create_dbconn=(DBconnection *(*)())pObj;
  if (create_dbconn)
    return create_dbconn();
  return 0;
}

/* --------------------------------------------------------------------------- */
/* createMatlabConnection --                                                   */
/* --------------------------------------------------------------------------- */
bool ModuleFactory::createMatlabConnection() {
#ifdef HAVE_MATLAB
  BUG_DEBUG("ModuleFactory::createMatlabConnection");
  MatlabConnection& matlabConn =  MatlabConnection::Instance();

  // we do not try to load the matlab libraries
  // if matlab flag is not set
  if (getenv("LD_LIBRARY_PATH_PREPEND")) {
    mysetenv( "LD_LIBRARY_PATH", compose("%1:/%2", getenv("LD_LIBRARY_PATH_PREPEND"), getenv("LD_LIBRARY_PATH")).c_str());
    std::cerr << "Extend LD_LIBRARY_PATH: " << getenv("LD_LIBRARY_PATH") << std::endl;
  }
  BUG_DEBUG("try to open libeng");

  lt_dlhandle hModule=0;
  if((hModule = lt_dlopenext( "libeng" )) == 0) {
    std::cerr << "MatlabConnection::load: libeng " << lt_dlerror() << std::endl;
    BUG_DEBUG("open libeng failed");
    return false;
  }
  std::string funcname;
  lt_ptr pFunc;
  funcname = "engOpen";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load :" << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }

  matlabConn.Open = (Engine * (*)(const char *))pFunc;

  funcname = "engOutputBuffer";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.OutputBuffer = (int  (*)(Engine *,char *,int))pFunc;

  funcname = "engEvalString";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.EvalString = ( int       (*)(Engine *, const char *))pFunc;

  funcname = "engClose";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.Close =(int       (*)( Engine * ))pFunc;

  funcname = "engPutVariable";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.PutVariable =(int          (*)( Engine *, const char * , const mxArray * ))pFunc;

  funcname = "engGetVariable";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetVariable =(mxArray *    (*)( Engine *, const char * ))pFunc;

  if((hModule = lt_dlopenext( "libmx" )) == 0) {
    std::cerr << "MatlabConnection::load: libmx " << lt_dlerror() << std::endl;
    return false;
  }
  funcname = "mxDestroyArray";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load :" << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.DestroyArray = (void      (*)(mxArray *))pFunc;

  funcname = "mxCreateDoubleMatrix";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.CreateDoubleMatrix = (mxArray * (*)( mwSize, mwSize, mxComplexity ))pFunc;

  funcname = "mxCreateStructMatrix";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.CreateStructMatrix = (mxArray * (*)( size_t, size_t, int, const char ** ))pFunc;

  funcname = "mxCreateStructArray";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.CreateStructArray =(mxArray * (*)( mwSize, const mwSize *, int, const char ** ))pFunc;

  funcname = "mxDuplicateArray";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.DuplicateArray =(mxArray * (*)( const mxArray * ))pFunc;

  funcname = "mxCreateCellMatrix";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.CreateCellMatrix =(mxArray * (*)( mwSize, mwSize ))pFunc;

  funcname = "mxCreateCellArray";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.CreateCellArray =(mxArray * (*)( mwSize, const mwSize * ))pFunc;

  funcname = "mxCreateString";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.CreateString =(mxArray * (*)( const char * ))pFunc;

  funcname = "mxCreateNumericArray";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.CreateNumericArray =(mxArray * (*)( mwSize, const mwSize *, mxClassID, mxComplexity))pFunc;

  funcname = "mxGetPr";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetPr =(double *  (*)( mxArray * ))pFunc;

  funcname = "mxGetPi";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetPi =(double *  (*)( mxArray * ))pFunc;

  funcname = "mxSetCell";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.SetCell =(void      (*)( mxArray *, mwIndex, mxArray *))pFunc;

  funcname = "mxCalloc";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.Calloc =(void *    (*)( size_t, size_t ))pFunc;

  funcname = "mxFree";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.Free =(void      (*)( void * ))pFunc;

  funcname = "mxSetClassName";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.SetClassName =(int       (*)( mxArray *, const char *))pFunc;

  funcname = "mxGetClassName";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetClassName =(const char * (*)( mxArray * ))pFunc;

  funcname = "mxSetFieldByNumber";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.SetFieldByNumber =(void         (*)( mxArray *, mwIndex, int, mxArray *))pFunc;

  funcname = "mxGetFieldNumber";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetFieldNumber =(int          (*)(const mxArray *, const char *))pFunc;

  funcname = "mxGetFieldByNumber";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetFieldByNumber =(mxArray *    (*)(const mxArray *, mwIndex, int))pFunc;

  funcname = "mxGetNumberOfDimensions";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetNumberOfDimensions =(mwSize          (*)( const mxArray * ))pFunc;

  funcname = "mxGetDimensions";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetDimensions =(const mwSize *  (*)( const mxArray * ))pFunc;

  funcname = "mxIsChar";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.IsChar =(bool         (*)( const mxArray * ))pFunc;

  funcname = "mxIsCell";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.IsCell =(bool         (*)( const mxArray * ))pFunc;

  funcname = "mxIsDouble";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.IsDouble =(bool         (*)( const mxArray * ))pFunc;

  funcname = "mxIsComplex";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.IsComplex =(bool         (*)( const mxArray *))pFunc;

  funcname = "mxIsStruct";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.IsStruct =(bool         (*)( const mxArray * ))pFunc;

  funcname = "mxIsClass";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.IsClass =(bool         (*)( const mxArray *,const char * ))pFunc;

  funcname = "mxGetNumberOfFields";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetNumberOfFields =(int          (*)( const mxArray * ))pFunc;

  funcname = "mxGetNumberOfElements";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetNumberOfElements =(size_t          (*)( const mxArray * ))pFunc;

  funcname = "mxGetN";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetN =(size_t          (*)( const mxArray * ))pFunc;

  funcname = "mxGetM";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetM =(size_t          (*)( const mxArray * ))pFunc;

  funcname = "mxGetString";
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetString =(int          (*)( const mxArray *,char *,mwSize ))pFunc;

  funcname = "mxGetCell";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.GetCell =(mxArray *    (*)( const mxArray *,mwIndex ))pFunc;

  funcname = "mxArrayToString";
  BUG_DEBUG("try to find function: " << funcname);
  if((pFunc =  lt_dlsym( hModule, funcname.c_str())) == 0) {
    std::cerr << "Cannot load : " << funcname << std::endl;
    lt_dlclose( hModule );
    return false;
  }
  matlabConn.ArrayToString =(char*    (*)( const mxArray * ))pFunc;
  s_ltDlHandle.push_back(hModule);

#endif
  return true;
}

/* --------------------------------------------------------------------------- */
/* createPlugin --                                                             */
/* --------------------------------------------------------------------------- */
Plugin* ModuleFactory::createPlugin( const std::string &name ) {
  DIR *plugin_dir;
  std::string shlibfile;
  std::string dirname;
  struct dirent *this_entry;

  lt_dlhandle hModule;
  void *pFunction;
  void *pObj;

  BUG( BugPlugin, "Plugin::open" );
#ifdef  HAVE_DLOPEN_WITH_UNRESOLVED_REFERENCES
  dirname = AppData::Instance().IntensHome()+ "plugins";

  plugin_dir = opendir( dirname.c_str() );

  if( plugin_dir == NULL ){
    // perror( dirname.c_str() );
    //    return 0;
  } else {
    while( (this_entry = readdir( plugin_dir ) )!=NULL ){
      void * symbol;
      std::string fn(this_entry->d_name);
      if( strstr(this_entry->d_name, LTDL_SHLIB_EXT) != NULL ){
	shlibfile = dirname + "/" + this_entry->d_name;
	if((hModule = lt_dlopen( shlibfile.c_str() )) == 0) {
	  std::cerr << lt_dlerror() << std::endl;
	}
	else {
	  if((pObj =  lt_dlsym( hModule, name.c_str())) == 0) {
	    BUG_MSG( "Expected object " << name << " not found in " << shlibfile );
	    lt_dlclose( hModule );
	  }
	  else {
	    s_ltDlHandle.push_back(hModule);
	    Plugin* plugin = new Plugin(name, hModule, pObj);
	    if( plugin->getDescription()==name ){
	      BUG_MSG( name << " loaded" );
	      return plugin;
	    }
	    BUG_MSG( plugin->getDescription() << " is not " << name );
	    delete plugin;
	    lt_dlclose( hModule );
	  }
	}
      }
    }
  }
#endif
#if defined HAVE_QT && !defined HAVE_HEADLESS
  // try to find plugin through QPluginLoader
  SmartPluginInterface* plugin = GuiQtManager::Instance().loadPlugin(name);
  return plugin ? new Plugin(name, plugin) : 0;
#else
  return 0;
#endif
}

void ModuleFactory::closeAll() {
  std::vector<lt_dlhandle>::iterator it = s_ltDlHandle.begin();
  for (; it != s_ltDlHandle.end(); ++it) {
    lt_dlclose( *it );
  }
}

void ModuleFactory::closeAllStatic() {
  if( s_connection_factory==0 )
    return;
  s_connection_factory->closeAll();
}
