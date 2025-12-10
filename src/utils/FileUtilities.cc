
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <direct.h>
#endif
#include <stdlib.h>
#include <filesystem>

#ifdef HAVE_QT
#include <QFileInfo>
#include <QMimeType>
#include <QMimeDatabase>
#ifdef __MINGW32__
#include <qfile.h>
#include <qtextstream.h>
#endif
#endif

#include "utils/Debugger.h"
#include "utils/FileUtilities.h"
#include "utils/StringUtils.h"
#include "utils/OsFileName.h"

#define LOGGER "intens.utils"

INIT_LOGGER();


/*********************************************************************/
/* FileInfo                                                          */
/*********************************************************************/

FileUtilities::FileInfoType FileUtilities::FileInfo(const std::string & FName,const FileInfoAccessType RequestedAccess )
{
  BUG_PARA(BugUtilities,"FileUtilities::FileInfo","FName=[" << FName << "]");
  struct stat stat_buf;

  if (stat(FName.c_str(), &stat_buf)  != 0)
   {int err = errno; // Save it
    if (err == EACCES)
      {BUG_EXIT("rslt = NoAccess (stat)");
       return NoAccess;
      }
    else
      {BUG_EXIT("rslt = NoFile errno=" << err);
       return NoFile;
      }
   }

  assert((stat_buf.st_mode & S_IFMT) != 0);    // irgend etwas muss es sein

  if (RequestedAccess != DontCareAccess)
    {/*****************************************/
     /* wir machen zuerst den Access-Check    */
     /*****************************************/
      if ((stat_buf.st_mode & S_IFREG)  != 0 ||
          (stat_buf.st_mode & S_IFDIR)  != 0   )
        {/************************************/
         /* check read                       */
         /************************************/
         if (RequestedAccess & WantReadAccess && ! (stat_buf.st_mode & S_IREAD) )
           {BUG_EXIT("rslt = NoAccess (Read)");
            return NoAccess;
           }
         /************************************/
         /* check read                       */
         /************************************/
         if (RequestedAccess & WantWriteAccess && ! (stat_buf.st_mode & S_IWRITE) )
           {BUG_EXIT("rslt = NoAccess (Write)");
            return NoAccess;
           }
        }
    }

  if (stat_buf.st_mode & S_IFREG)
    {/*****************************************/
     /* normales File                         */
     /*****************************************/
     BUG_EXIT("rslt = IsPlainFile");
     return IsPlainFile;
    }

  if (stat_buf.st_mode & S_IFDIR)
    {/*****************************************/
     /* Directory                             */
     /*****************************************/
     BUG_EXIT("rslt=IsDirectory");
     return IsDirectory;
    }

  BUG_EXIT("rslt = NoFile (special or unknown type)");
  return NoFile;
}

/*********************************************************************/
/* ChopFilenameExtension                                             */
/*********************************************************************/

void FileUtilities::ChopFilenameExtension(std::string & FName, std::string & Ext){
//   BUG_PARA(BugUtilities,"FileUtilities::ChopFilenameExtension","FName=[" << FName << "]");
  BUG_DEBUG("FileUtilities::ChopFilenameExtension FName=[" << FName << "]");

  int Len = FName.length(); // Laenge des Filenamens
  int NameLen = 0;          // Laenge der Filenamens ohne Extension
  char delim = 0;

  const char * pt = FName.c_str() + Len-1;  // Letzter Charakter des Strings
  Ext = "";

  if (Len == 0){
    BUG_DEBUG("empty string, nothing to do");
    return;
  }

  if (*pt == '"' || *pt == '\''){
    // Am Schluss ist ein  Delimitter, den wir uns merken muessen
    delim = *pt;
  }

  NameLen = FName.rfind('.');
  if (NameLen == std::string::npos){
    BUG_DEBUG("no dot found");
    return;
  }

  Ext   = FName.substr( NameLen+1,std::string::npos );
  FName = FName.substr(0,NameLen);
  if (delim!=0){
    FName+=delim;
    Ext=Ext.substr(0,Ext.length()-1);
  }
  BUG_DEBUG( "Filename=[" << FName << "]  Ext=[" << Ext << "]");
}


/*********************************************************************/
/* isBinaryFormat                                                    */
/*********************************************************************/

bool FileUtilities::isBinaryFormat(const std::string & FName){
  std::string filename(lower(FName));
  std::string::size_type size = filename.size();

  if (filename.find(".pdf") == (size - 4) ||
      filename.find(".bin") == (size - 4) ||
      filename.find(".dat") == (size - 4) ||
      filename.find(".jpg") == (size - 4) ||
      filename.find(".jpeg") == (size - 5) ||
      filename.find(".png") == (size - 4) ||
      filename.find(".zip") == (size - 4) ||
      filename.find(".tar") == (size - 4) ||
      filename.find(".bz2") == (size - 4) ||
      filename.find(".gz") == (size - 3) ||
      filename.find(".war") == (size - 4) ||
      filename.find(".jar") == (size - 4) ||
      filename.find(".bzip") == (size - 5) ||
      filename.find(".xls") == (size - 4) ||
      filename.find(".xlsx") == (size - 5)) {
    return true;
  }
  return false;
}

/*********************************************************************/
/* GetCurrentWorkDirectory                                           */
/*********************************************************************/

std::string FileUtilities::GetCurrentWorkDirectory(void)
{
  char dir_tmp[MAXPATHLEN];

  // BUG(BugUtilities,"FileUtilities::GetCurrentWorkDirectory");


  if(getcwd( dir_tmp, MAXPATHLEN ) == NULL )
    {/**************************************/
     /* Darf wirklich nicht vorkommen.     */
     /**************************************/
     assert(false);
     perror( "_getcwd error" );
    }

  //BUG_EXIT("rslt=[" << std::string(dir_tmp) << "]");
  return std::string(dir_tmp);
}

/*********************************************************************/
/* MakeAbsolutPath                                           */
/*********************************************************************/

std::string FileUtilities::MakeAbsolutPath(const std::string & Path)

{

  char dir_tmp[MAXPATHLEN+1] = {0};

  BUG_PARA(BugUtilities,"FileUtilities::MakeAbsolutPath","Path=[" << Path << "]");

#if defined _MSC_VER || defined __MINGW32__
  assert(false);
  return Path;
#else
    if (realpath(Path.c_str(), dir_tmp) == 0)
      {int err = errno; // Save it
       BUG_EXIT("rslt=[" << Path << "] errno=" << errno << " Fehler in realpath");
       return Path;
      }
// #endif

  BUG_EXIT("rslt=[" << std::string(dir_tmp) << "]");
  return std::string(dir_tmp);
#endif
}

/*********************************************************************/
/* ChangeFileExtension                                               */
/*********************************************************************/

bool FileUtilities::ChangeFileExtension(const std::string &OrigFile, const std::string & NewExtension)
{


  BUG_PARA(BugUtilities,"FileUtilities::ChangeFileExtension"
          ,"Filename=[" << OrigFile << "] new extension=[" << NewExtension << "]"
          );

  assert(OrigFile.length() > 0);
  assert(NewExtension.length() > 0);

  std::string NewFile = OrigFile;
  std::string OldExt;

  ChopFilenameExtension(NewFile,OldExt);
  NewFile+="."+NewExtension;

  /**************************************************************/
  /*Altes File checken ob es auch wirklich hier ist.            */
  /**************************************************************/
  switch(FileInfo(OrigFile))
   {case IsPlainFile:
        {/******************************************************/
         /* File schon vorhanden. Fein.                        */
         /******************************************************/
        break;
       }
    case IsDirectory:
       {/*******************************************************/
        /* Es ist ein Directory. Damit kann ich nichts anfangen*/
        /*******************************************************/
        BUG_EXIT("rslt=false. original file [" << OrigFile << "] is a directory");
        return false;
       }
    case NoFile:
      {/*******************************************************/
       /* Kein vorhandenes File.                              */
       /*******************************************************/
       BUG_EXIT("rslt=true. original file [" << OrigFile << "] does not exist");
       return true;

      }
    case NoAccess:
      {/*******************************************************/
       /* Zugriff verweigert. Kann nichts damit machen.       */
       /*******************************************************/
       BUG_EXIT("rslt=false. no access to original file [" << NewFile << "]");
       return false;
      }
    default:
      {BUG_EXIT("rslt=false. unknown case");
       assert(false);
       return false;
      }
    }// switch


  /**************************************************************/
  /* Neuer Filename checken. Darf nicht hier sein oder wird ge- */
  /* loescht.                                                   */
  /**************************************************************/

  switch(FileInfo(NewFile))
   {case IsPlainFile:
        {/******************************************************/
         /* File schon vorhanden. Wir werden  das Biest jetzt  */
         /* loeschen.                                          */
         /******************************************************/
         if (!RemoveFile(NewFile))
           {BUG_EXIT("rslt=false. cannot remove new file [" << NewFile << "]");
            return false;
           }
        break;
       }
    case IsDirectory:
       {/*******************************************************/
        /* Es ist ein Directory. Damit kann ich nichts anfangen*/
        /*******************************************************/
        BUG_EXIT("rslt=false. new file [" << NewFile << "] is a directory");
        return false;
       }
    case NoFile:
      {/*******************************************************/
       /* Kein vorhandenes File.                              */
       /*******************************************************/
       break;
      }
    case NoAccess:
      {/*******************************************************/
       /* Zugriff verweigert. Kann nichts damit machen.       */
       /*******************************************************/
       BUG_EXIT("rslt=false. no access to existing new file [" << NewFile << "]");
       return false;
      }
    default:
      {BUG_EXIT("rslt=false. unknown case");
       assert(false);
       return false;
      }
    }// switch

  /**********************************************************/
  /* Jetzt umbenennen                                       */
  /**********************************************************/

  if (rename(OrigFile.c_str(), NewFile.c_str()) != 0)
    {BUG_EXIT("rslt=false. rename failed. errno=" << errno);
     return false;
    }

  BUG_EXIT("rslt true. Rename OK");
  return true;
}

/*********************************************************************/
/* ReadFile                                                          */
/*********************************************************************/
std::istream* FileUtilities::ReadFile(const std::string &filename) {
#ifdef __MINGW32__
  return new std::ifstream(QFile::encodeName(filename.c_str()).data(), std::ios::in);
#else
  return new std::ifstream( filename.c_str(), std::ios::in );
#endif
}

/*********************************************************************/
/* WriteFile                                                          */
/*********************************************************************/
std::ostream* FileUtilities::WriteFile(const std::string &filename) {
#ifdef __MINGW32__
  return new std::ofstream(QFile::encodeName(filename.c_str()).data(), std::ios::out);
#else
  return new std::ofstream( filename.c_str(), std::ios::out );
#endif
}

/*********************************************************************/
/* RemoveFile                                                        */
/*********************************************************************/

bool FileUtilities::RemoveFile(const std::string & OrigFile)
{

  BUG_PARA(BugUtilities,"FileUtilities::RemoveFile"
          ,"Filename=[" << OrigFile << "]"         );

  /**************************************************************/
  /* Neuer Filename checken. Darf nicht hier sein oder wird ge- */
  /* loescht.                                                   */
  /**************************************************************/

  switch(FileInfo(OrigFile))
   {case IsPlainFile:
        {/******************************************************/
         /* Alles schoen. Schreiten wir zur Tat.               */
         /******************************************************/
         if (remove(OrigFile.c_str()) != 0)
           {BUG_EXIT("cannot remove file [" << OrigFile << " errno=" << errno);
            return false;
           }
        break;
       }
    case NoFile:
      {/*******************************************************/
       /* Kein vorhandenes File. Sehr schoen. All done!!!     */
       /*******************************************************/
       BUG_EXIT("rslt=true. No file");
       return true;
      }
     case IsDirectory:
     case NoAccess:
       {/*******************************************************/
        /* Es ist ein Directory. Damit kann ich nichts anfangen*/
        /*******************************************************/
        BUG_EXIT("rslt=false. Cannot Remove. [" << OrigFile << "] is a directory or access is impossible");
        return false;
       }
    default:
      {BUG_EXIT("rslt=false. unknown case");
       assert(false);
       return false;
      }
    }// switch
  BUG_EXIT("rslt=true. All fine");
  return true;
}


/*********************************************************************/
/* VerifyPath                                                        */
/*********************************************************************/
std::string FileUtilities::GetNewestFile(const std::vector<std::string>& files) {
  std::string path = "./";  // Directory to search
  std::filesystem::file_time_type newest_time;
  std::filesystem::path newest_file;

  for (const auto& f : files) {
    std::filesystem::path entry(f);
    if (std::filesystem::exists(entry) && std::filesystem::is_regular_file(entry)) {
      auto ftime = std::filesystem::last_write_time(entry);
      if (newest_file.empty() || ftime > newest_time) {
        newest_time = ftime;
        newest_file = entry;
      }
    }
  }

  if (!newest_file.empty()) {
    return std::filesystem::absolute(newest_file).string();
  }
  return "";
}

/*********************************************************************/
/* VerifyPath                                                        */
/*********************************************************************/
bool FileUtilities::VerifyPath(const std::string & FileName, bool DoCreate)
{
  BUG_PARA(BugUtilities,"FileUtilities::VerifyPath"
          ,"FileName=[" << FileName << "]");


  OsFileName os(FileName);

  if (!os.IsValid())
    {BUG_EXIT("rslt=false Invalid Filename");
     return false;
    }


  if (!os.MakeAbsolutFilename())
    {BUG_EXIT("rslt=false Bad Invalid Filename");
     return false;
    }


  /***************************************
   * ===> hier muss nun der Check rein
   ****************************************/

  BUG_EXIT("rslt=true");
  return true;
}

/*********************************************************************/
/* getSuffix                                                         */
/*********************************************************************/
std::string FileUtilities::getSuffix(const std::string & fName) {
#ifdef HAVE_QT
  QFileInfo finfo( QString::fromStdString(fName) );
  return  finfo.suffix().toLower().toStdString();
#else
  std::string::size_type  pos = fName.find_last_of(".");
  if (pos != std::string::npos ) {
    return  lower(fName.substr(pos+1));
  }
  return std::string();
#endif
}

/* --------------------------------------------------------------------------- */
/* getDataMimeType --                                                          */
/* --------------------------------------------------------------------------- */

std::string FileUtilities::getDataMimeType(const std::string& data){
  if (data.substr(0, 5) == "data:"){
    size_t pos = data.find_first_of(',');
    size_t pos2 = data.find_first_of(';');
    if (pos2 < 50 && pos < 100){
      return data.substr(5, pos2-5);
    }
  }
  if (data.find("<svg") >= 0 && data.find("svg>") != std::string::npos)
    return "image/svg+xml";
  if (data.find("<html ") != std::string::npos && data.find("html>") != std::string::npos)
    return "text/html";
  if (data.find("\211PNG\r\n\032\n") == 0)
    return "image/png";
  if (data.find("%PDF-") == 0)
    return "application/pdf";
  if (data.find("\x1F\x8B\x08") == 0)
    return "application/gzip";
  if (data.find("PK\x03\x04") == 0 ||
      data.find("PK\x05\x06") == 0 ||
      data.find("PK\x07\x08") == 0)
    return "application/zip";
  if (data.find("\x04\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00(\x00\x00\x00    *** File with magnetic curve ***") == 0)
    return "application/octet-stream";
  return "text/plain";
}

/* --------------------------------------------------------------------------- */
/* getPreferredSuffixOfMimeType --                                           */
/* --------------------------------------------------------------------------- */
std::string FileUtilities::getPreferredSuffixOfMimeType(const std::string& mime){
#ifdef HAVE_QT
  QMimeDatabase db;
  QMimeType qmt = db.mimeTypeForName(QString::fromStdString(mime));
  return qmt.preferredSuffix().toStdString();
#else
  int pos = mime.rfind('/');
  if(pos == std::string::npos) {
    return "";
  }
  std::string subType = mime.substr(pos + 1, std::string::npos);
  switch(subType) {
  case "svg+xml":
    return "svg";
  case "gzip":
    return "gz";
  case "octet-stream":
    return "txt";
  default:
    return subType;
#endif
}

/* --------------------------------------------------------------------------- */
/* getDataFilename --                                                          */
/* --------------------------------------------------------------------------- */
std::string FileUtilities::getDataFilename(std::string& data){
  std::string file_token("data:filename/");
  if (data.find(file_token) == 0) {
    std::string endToken(",");
    std::string::size_type posE = data.find(endToken);
    std::string fn = data.substr(file_token.size(), posE-file_token.size());
    BUG_INFO("Read Filename: " << fn);
    data.erase(0, posE+endToken.size());
    return fn;
  }
  return "";
}
