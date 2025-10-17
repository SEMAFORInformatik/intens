
#if !defined(UTSNAME__INCLUDED_) && defined __MINGW32__
#define UTSNAME_H__INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

struct utsname {
  char machine[16];
  char nodename[32];
  char release[16];
  char sysname[16];
  char version[24];
};

#include <windows.h>

int	uname(struct utsname *name) {
  char computerName[128];
  SYSTEM_INFO sysinfo;
  OSVERSIONINFO versionInfo;
  DWORD computerNameLength = sizeof(name->nodename);

  // nodename
  GetComputerName( (LPTSTR)name->nodename, &computerNameLength );

  // CPU type
  GetSystemInfo( &sysinfo);
  switch (sysinfo.wProcessorArchitecture)  {
    case PROCESSOR_ARCHITECTURE_INTEL:
      {
      unsigned int ptype;
      if (sysinfo.dwProcessorType < 3) /* Shouldn't happen. */
	ptype = 3;
      else if (sysinfo.dwProcessorType > 9) /* P4 */
	ptype = 6;
      else
	ptype = sysinfo.dwProcessorType;

      sprintf (name->machine, "i%d86", ptype);
      break;
      }
    case PROCESSOR_ARCHITECTURE_ALPHA:
      strcpy (name->machine, "alpha");
      break;
    case PROCESSOR_ARCHITECTURE_MIPS:
      strcpy (name->machine, "mips");
      break;
    default:
      strcpy (name->machine, "unknown");
      break;
  }

  // Versionsinfo
  versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx( &versionInfo );
  sprintf (name->version, "%d", versionInfo.dwBuildNumber );

  // sysname
  switch ( versionInfo.dwPlatformId ) {
    case VER_PLATFORM_WIN32s:
      strcpy (name->sysname, "WIN32S");
      break;
    case VER_PLATFORM_WIN32_WINDOWS:
      strcpy (name->sysname, "WIN95");
      break;
    case VER_PLATFORM_WIN32_NT:
      strcpy (name->sysname, "WINNT");
      break;
    default:
      strcpy (name->sysname, "UNKNOWN_OS");
      break;
  }

  // release
  sprintf (name->release, "%d.%d", versionInfo.dwMajorVersion, versionInfo.dwMinorVersion );

  return 0;
}

#ifdef __cplusplus
}
#endif

#endif
