// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
//
// SPDX-License-Identifier: Apache-2.0

#include <cstdlib>
#include <csignal>

#if HAVE_LOG4CPLUS
#include <log4cplus/log4cplus.h>
#endif

#include <iostream>
#include "app/QuitApplication.h"

#include "app/App.h"

void signalHandler ( int signo )
{
  //    std::cout << "----signalHandler---- signum["<<signo<<"]\n";
  switch (signo) {
  case SIGINT:
    //QuitApplication::Instance()->ButtonPressed();
    QuitApplication::Instance()->exitApplication();
    break;
  case SIGSEGV:
    std::cout << signo<< " SIGSEGV(11): Segmentation violation!\n";
    break;
  default:
    std::cout << "default Signalhandler: SigNo: " << signo << " ==> do nothing\n";
    break;
  }
  //  signal (signo, signalHandler);
}

void setSignals ( void )
{
  // default signal settings
 signal (SIGINT, signalHandler);

 // override settings
 int index;
 for (index = 1; index < NSIG; index++) {
#ifdef _WIN32
   signal (index, signalHandler);
#else
   if (index != SIGINT && index != SIGKILL && index != SIGSTOP && index != SIGTSTP && index != SIGCHLD)
     signal (index, SIG_IGN);
#endif

 }
}

const char *program_name;

int main( int argc, char **argv )
{
  program_name=argv[0];
  setSignals();
#if HAVE_LOG4CPLUS
  // configure basic
  log4cplus::Initializer initializer;
  log4cplus::BasicConfigurator config;
  config.configure();
  log4cplus::Logger::getRoot().setLogLevel(log4cplus::WARN_LOG_LEVEL);
#endif
  App &app = App::Instance( argc, argv );
  if( app.parse( argc, argv ) ){
    return app.run();
  }
  exit( 1 );
}
