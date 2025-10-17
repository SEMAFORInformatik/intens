
#if !defined(MODULE_FACTORY_INCLUDED_H)
#define MODULE_FACTORY_INCLUDED_H

class DBconnection;
class MatlabConnection;
class CorbaServer;
class Plugin;

/** Der ModuleFactory ist ein Singleton-Object. Er stellt die gesammte
    Schnittstelle zwischen Intens und einer dynamisch dazugelinkten Library.

    @author Copyright (C) 2004  SEMAFOR Informatik & Energie AG, Basel, Switzerland
*/
class ModuleFactory {
public:
/*=============================================================================*/
/* public static member functions                                              */
/*=============================================================================*/
  static ModuleFactory& Instance();
  DBconnection  *createDBconnection( const char *so );
  bool           createMatlabConnection();
  CorbaServer*   createCorbaServer( const char *so, int argc, char *argv[]);
  void           shutdownCorbaServer();
  void           closeAll();
  static void    closeAllStatic();
  Plugin*        createPlugin( const std::string &so );

private:
/*=============================================================================*/
/* Constructor                                                                 */
/*=============================================================================*/
  ModuleFactory();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
  static ModuleFactory *s_connection_factory;
};

#endif
