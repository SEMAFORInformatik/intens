// OsFileName.cpp: implementation of the OsFileName class.
//

#include <assert.h>
#include <string.h>

#include "utils/Debugger.h"
#include "utils/OsFileName.h"
#include "utils/FileUtilities.h"

/*********************************************************************/
/* Constructor                                                       */
/*********************************************************************/

OsFileName::OsFileName()
{
  BUG(BugUtilities,"OsFileName::OsFileName");
  InitFile();
  InitParse();
}

OsFileName::OsFileName(const std::string & Fil)
{
  BUG_PARA(BugUtilities,"OsFileName::OsFileName","File=[" << Fil << "]");
  InitFile();
  InitParse();
  NewFile(Fil);
}

/*********************************************************************/
/* Destructor                                                        */
/*********************************************************************/

OsFileName::~OsFileName()
{

  BUG(BugUtilities,"OsFileName::~OsFileName");

}

/*********************************************************************/
/* Copy Constructor                                                  */
/*********************************************************************/

OsFileName::OsFileName(const OsFileName &ref)
  : m_Len(ref.m_Len)
  , m_NumDirs(ref.m_NumDirs)
  , m_Relative(ref.m_Relative)
  , m_Quotes(ref.m_Quotes)
  , m_Valid(ref.m_Valid)
  , m_UNC(ref.m_UNC)
  , m_DriveLetter(ref.m_DriveLetter)
{
  BUG(BugUtilities,"OsFileName::OsFileName (copyconstructor)");
  memcpy(m_FileName, ref.m_FileName, sizeof(m_FileName));
  memcpy(m_DirIndex, ref.m_DirIndex, sizeof(m_DirIndex));
}

/*********************************************************************/
/* Assignment Operator                                               */
/*********************************************************************/

OsFileName & OsFileName::operator=(const OsFileName &ref)
{

  BUG(BugUtilities,"OsFileName::OsFileName (operator=)");
  m_Len         = ref.m_Len;
  m_NumDirs     = ref.m_NumDirs;
  m_Relative    = ref.m_Relative;
  m_Quotes      = ref.m_Quotes;
  m_Valid       = ref.m_Valid;
  m_UNC         = ref.m_UNC;
  m_DriveLetter = ref.m_DriveLetter;
  memcpy(m_FileName, ref.m_FileName, sizeof(m_FileName));
  memcpy(m_DirIndex, ref.m_DirIndex, sizeof(m_DirIndex));
  return *this;
}


/*********************************************************************/
/* InitFile                                                          */
/*********************************************************************/

void OsFileName::InitFile(void)
{
  BUG(BugUtilities,"OsFileName::InitVars");
  m_Len     = 0;
  InitParse();

}

/*********************************************************************/
/* InitFile                                                          */
/*********************************************************************/

void OsFileName::InitParse(void)
{
  BUG(BugUtilities,"OsFileName::InitParse");
  m_NumDirs = 0;
  m_Relative= false;
  m_Quotes  = false;
  m_Valid   = false;
  m_UNC     = false;

}

/*********************************************************************/
/* NewFile                                                           */
/*********************************************************************/

bool OsFileName::NewFile(const std::string & Fil)
{
  BUG_PARA(BugUtilities,"OsFileName::NewFile","Fil=[" << Fil << "]");


  InitFile();

  if (Fil.empty())
    {BUG_EXIT("rslt=false. Empty File");
     return false;
    }


  const char *pin = Fil.c_str();
  char       *px  = m_FileName;
  int         i;

  /*******************************************************************/
  /* Uebertrag ins neue Domizil. Dabei wird der Name von zusaetzlich-*/
  /* em Ballast wie leading und trailing white Space gereinigt.      */
  /*******************************************************************/

  m_Len = Fil.length();
  while (m_Len> 0 && isspace(*pin))
    {++pin; --m_Len;  }
  i = m_Len;
  while(i >= 0)
    {*(px++) = *(pin++);  --i; }
  while(m_Len>0 && isspace(m_FileName[m_Len-1]))
    {m_FileName[m_Len-1] = 0; --m_Len; }

  if (m_Len == 0)
    {BUG_EXIT("rslt=false. Nur white space");
     return false;
    }

  /*******************************************************************/
  /* Und jetzt wird geparsed.                                        */
  /*******************************************************************/

  bool brslt = Parse();
  BUG_EXIT("rslt=" << brslt);
  return brslt;

}

/*********************************************************************/
/* Parse                                                             */
/*********************************************************************/

bool OsFileName::Parse(void)
{
  BUG_PARA(BugUtilities,"OsFileName::Parse","FileName=[" << m_FileName << "]");
  InitParse();

  char *px  = m_FileName;
  int         plen= m_Len;

  /*******************************************************************/
  /* Check for quotes                                                */
  /*******************************************************************/
  px = m_FileName;
  if (FileUtilities::IsFilenameQuoteChar(*px))
    {if (!FileUtilities::IsFilenameQuoteChar(m_FileName[plen-1]))
      {BUG_EXIT("rslt=false. Missing Endquote");
       return false;
      }
     px++;
     m_Quotes = true;
     plen-=2;
    }

  /*******************************************************************/
  /* Jetzt kommt der Prefix dran.                                    */
  /*******************************************************************/

  if (Windows)
    {/****************************************************************/
     /* Der Prefix von einem Window-File kann ziemlich komplex sein  */
     /* mit oder ohne dirve, unc etc. Die einzelnen faelle sind im   */
     /* Code dokumentiert. Der code ist so strukturiert, dass alles  */
     /* diese Faelle in einer Schlaufe sind, die genau einmal durch- */
     /* laufen wird. Immer wenn ein Fall abgeschlossen ist, wird mit */
     /* einem break ans Ende der Schlaufe verzweigt.                 */
     /****************************************************************/
     assert(plen > 0);
     do { /*************************************************************/
          /* Check UNC-Name                                            */
          /*************************************************************/
          if (plen > 2 && *px == DirectorySeparator && *(px+1) == DirectorySeparator)
            {px+=2;plen-=2;
             while(plen> 0 &&  *px != DirectorySeparator)
               {plen--; ++px;  }
             while(plen > 0 && *px == DirectorySeparator)
               {plen--; ++px;  }
             m_UNC     = true;
             m_DirIndex[0] = m_Len-plen;
             if (plen > 0)
               m_NumDirs=1;
             break;
           }

          /*************************************************************/
          /* Check drive absolut   c:\fff                              */
          /*************************************************************/
          if (plen > 3 && isalpha(*px) && *(px+1) == ':' &&  *(px+2) == DirectorySeparator)
            {px+=3;plen-=3;
             while(plen > 0 && *px == DirectorySeparator)
               {plen--; ++px;  }
             m_DriveLetter     = true;
             m_DirIndex[0]     =  m_Len-plen;
             if (plen > 0)
               m_NumDirs=1;
             break;
            }

          /*************************************************************/
          /* Check drive relative   c:aaa                              */
          /*************************************************************/
          if (plen > 3 && isalpha(*px) && *(px+1) == ':' &&  *(px+2) != DirectorySeparator)
            {px+=2;plen-=2;
             while(plen > 0 && *px == DirectorySeparator)
               {plen--; ++px;  }
             m_DriveLetter     = true;
             m_Relative        = true;
             m_DirIndex[0]     = m_Len-plen;
             if (plen > 0)
               m_NumDirs=1;
             break;
            }

          /*************************************************************/
          /* Check  absolut   \aaa on current drive                    */
          /*************************************************************/
          if (plen > 2 && *px == DirectorySeparator && *(px+1) != DirectorySeparator)
            {px+=1;plen-=1;
             while(plen > 0 && *px == DirectorySeparator)
               {plen--; ++px;  }
             m_DirIndex[0]     = m_Len-plen;
             if (plen > 0)
               m_NumDirs=1;
             break;
            }

          /*************************************************************/
          /* must be drive relative   fff                              */
          /*************************************************************/
          m_Relative        = true;
          m_DirIndex[0]     = 0;
          if (plen > 0)
             m_NumDirs=1;
          break;

       }
    while(0);
    }

  if (!Windows)
    {/****************************************************************/
     /* Verglichen mit dem Windows-Gedoens ist der Unix-Filename     */
     /* sehr einfach strukturiert. Speziall der Prefix ist praktisch */
     /* nicht existent.                                              */
     /****************************************************************/
     assert(plen > 0);
     do{ /*************************************************************/
         /* Relativer Name mit Punkt am Anfang.                       */
         /*************************************************************/
         if (*px == '.')
           {while(plen> 0 && (*px == '.' || *px ==DirectorySeparator))
              {plen--; ++px;  }
            m_Relative=true;
            m_DirIndex[0] = m_Len-plen;
            if (plen > 0)
              m_NumDirs=1;
            break;
           }

         /*************************************************************/
         /* Absoluter Pafadname mit "/" am Anfang                     */
         /*************************************************************/
         if (*px == DirectorySeparator)
           {while(plen> 0 && (*px == '.' || *px ==DirectorySeparator))
              {plen--; ++px;  }
            m_DirIndex[0] = m_Len-plen;
            if (plen > 0)
              m_NumDirs=1;
            break;
           }

         /*************************************************************/
         /* Relativer Filename beginnend mit einem Buchstabe          */
         /*************************************************************/
          m_NumDirs=1;
          m_DirIndex[0] = m_Len-plen;
          break;
       }
    while(0);
    }

  /********************************************************************/
  /* An dieser Stelle haben wir nun den Prefix geparsed. Px zeigt zu  */
  /* ersten Char des ersten Directories. m_DirIndex enthaelt den      */
  /* Index auf das erst Directory-Element.                            */
  /********************************************************************/

  while(plen > 0)
    {assert(*px != DirectorySeparator);
     while(plen > 0 && *px != DirectorySeparator)
       {--plen ; ++px;  }
     while(plen > 0 && *px == DirectorySeparator)
       {--plen ; ++px;  }
     m_DirIndex[m_NumDirs] = m_Len-plen;
     if (plen > 0)
        m_NumDirs+=1;
    }

  m_Valid = true;
  BUG_EXIT("rslt=true");
  return true;
}

/*********************************************************************/
/* MakeAbsolutFilename                                               */
/*********************************************************************/

bool OsFileName::MakeAbsolutFilename(void)
{
  BUG_PARA(BugUtilities,"OsFileName::MakeAbsolutFilename","m_FileName=[" << m_FileName << "]");
  assert(IsValid());

  if (!IsRelative() || (Windows && !HasDriveLetter()))
    {BUG_EXIT("rslt=true. Already absolute Filename");
     return true;
    }

  std::string wd = FileUtilities::GetCurrentWorkDirectory();

  if (Windows && !IsRelative() && !HasDriveLetter())
    {assert(isalpha(wd[0]));
     wd = wd.substr(0,1)+
          std::string(":")   +
          std::string(& m_FileName[m_DirIndex[0]], m_Len- m_DirIndex[0]);
    }
  else
    wd+=std::string(& m_FileName[m_DirIndex[0]], m_Len- m_DirIndex[0]);

  bool brslt = NewFile(wd);

  BUG_EXIT("rslt=" << brslt);
  return brslt;
}
