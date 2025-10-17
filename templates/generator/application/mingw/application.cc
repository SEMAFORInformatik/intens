#include <windows.h>

#include <string>
#include <direct.h>
#include <iostream>
#include <sstream>

//#define BUFSIZ 1024

std::string getInstallDir( const std::string path ){
  unsigned int pos = path.rfind('\\');
  if( pos == std::string::npos ){
    return std::string();
  }
  return path.substr(0,pos);
}


int APIENTRY WinMain(HINSTANCE hInstance,
		     HINSTANCE hPrevInstance,
		     LPSTR lpCmdLine,
		     int nCmdShow){

  // get current working directoy
  char cwd[BUFSIZ+1];
  if (_getcwd( cwd, BUFSIZ ) == NULL) {
    strcpy(cwd, ".");
  }
  std::string instDir = getInstallDir(cwd);
  std::string intensPath = instDir + "\\intens\\"+INTENS_VERSION;
  // set path variable
  // set path variable
  #ifdef MSYS2
  std::string path("PATH="+ //std::string(cwd)+";" +
                   intensPath+"\\bin;" +
                   intensPath+"\\msys2\\usr\\bin;" + // msys2
		   std::string(getenv("PATH")));
  #else
  std::string path("PATH="+ //std::string(cwd)+";" +
                   intensPath+"\\bin;" +
                   intensPath+"\\msys\\bin;" +
		   std::string(getenv("PATH")));
  #endif
  putenv( path.c_str());

  // start intens Application
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );

 //
  std::string cmd = "cmd /K start /B /min .\\bin\\${application}.bat";

  // append command line
  int createFlag = DETACHED_PROCESS;
  if (strlen(lpCmdLine) > 0) {
    cmd += " " + std::string(lpCmdLine)+"\"";
    createFlag = CREATE_NEW_PROCESS_GROUP;
  }

  BOOL ret =  CreateProcess( NULL,   // No module name (use command line).
			     (char*)cmd.c_str(), // Command line.
			     NULL,             // Process handle not inheritable.
			     NULL,             // Thread handle not inheritable.
			     FALSE,            // Set handle inheritance to FALSE.
			     createFlag,       // No creation flags.
			     NULL,             // Use parent's environment block.
			     NULL,             // Use parent's starting directory.
			     &si,              // Pointer to STARTUPINFO structure.
			     &pi );             // Pointer to PROCESS_INFORMATION structure.  exit(0);

  if (ret == 0) {
    char * errText;
    DWORD errCode = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &errText,
        0, NULL );

    // Display the error message and exit the process
    std::ostringstream msg;
    msg << "CreateProcess failed with error: "
	<< errCode << std::endl << errText << std::endl
	<< "PATH: " << getenv("PATH") << std::endl << std::endl
	<< "CMD : " << cmd <<std::endl;
    MessageBox(NULL, msg.str().c_str(), TEXT("Error"), MB_OK);

    ExitProcess(errCode);
  }
  // maybe we should check the process exit code?
  DWORD exitCode=0;
  GetExitCodeProcess(pi.hProcess, &exitCode);
  if (exitCode != STILL_ACTIVE) {
    std::stringstream os;
    os << exitCode;
    os << std::endl << " PATH: " << getenv("PATH") << std::endl << std::endl;
    MessageBox(NULL, os.str().c_str(), TEXT("Exit Code:"), MB_OK);
  }
  return 0;
}
