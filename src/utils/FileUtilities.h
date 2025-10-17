
#if !defined(AFX_FILEUTILITIES_H_INCLUDED_)
#define AFX_FILEUTILITIES_H_INCLUDED_

#include <string>
#include <vector>

class FileUtilities  {
public:
#if defined _WIN32
  #define DirectorySeparator     '\\'
  #define CurrentDir            "."
  #define Windows               true
#else
  #define DirectorySeparator     '/'
  #define CurrentDir            "./"
  #define Windows               false
#endif

enum FileInfoType
  {NoFile
  ,NoAccess
  ,IsPlainFile
  ,IsDirectory
  };
enum FileInfoAccessType
  {DontCareAccess     = 0
  ,WantReadAccess     = 1
  ,WantWriteAccess    = 2
  ,WantRWAccess       = (WantReadAccess | WantWriteAccess)
  };

public:
  FileUtilities()           { return; }
  virtual ~FileUtilities()  { return; }

public:

  /**
   * FileInfoType
   * Liefert Informationen ueber ein Files, ohne es zu oeffnen. Wenn ein bestimmer Access-
   * Type verlang wird, so wird dieser in einer zusaetzlichen weiteren Variablen angegeben.
   * Die Funktion prueft dann, ob dieser Accesstyp mit dem File Compatibel ist.
   */
  static FileInfoType FileInfo(const std::string & FName, const FileInfoAccessType RequestedAccess = DontCareAccess);

  /**
   * ChopFilenameExtension
   * Diese Funktion loescht die Extension von FName und plaziert sie
   * in Ext. Der Punkt in FName wird ebenfalls geloescht.
   * Folgende Einschraenkung gilt:
   *  - Es wird nur die letzte Extension extrahiert.
   *    z.B. "aaa.txt.cpp" wird aufgespalten in "aaa.txt" und "cpp"
   *  - Die funktion versucht, das quotieren von Filenamen korrekt zu honorieren.
   *    Die voraussetzung dazu ist aber, dass das Inputfile schon korrekt (nach den Regeln
   *    des jeweiligen OS) gequoted ist. Sollte die Extension quotes benoetigen, so wird dies
   *    zu einem Fehler fuehren.
   */
  static void ChopFilenameExtension(std::string & FName, std::string & Ext);

  /**
   * isBinaryFormat
   * With filename extension it will detect if it is binary format
   */
  static bool isBinaryFormat(const std::string &filename);

  /**
   * GetCurrentWorkDirectory
   * Diese Funktion holt das laufende Directory (pwd)
   */
  static std::string GetCurrentWorkDirectory(void);

  /**
   * MakeAbsolutPath
   * Diese Funktion errechnet einen absoluten Pfad aus einem relativen Pfad. Die Funktion
   * laeuft in Unix und NT.
   */
  static std::string MakeAbsolutPath(const std::string & Path);

  /**
   * ChangeFileExtension
   * Diese Funktion fuehrt ein Rename des Files durch mit einer neuen
   * Extension. Dies kann verwendet werden, um z.B. aa/bb.msg in aa/bb.bak zu
   * renamen.
   * Sollte das neue File schon existieren, so wird es ohne Rueckfrage einfach
   * geloescht.
   * Die Funktion gibt true zurueck, wenn alles ordnungsgemaess abgewickelt worden
   * ist.
   */
  static bool ChangeFileExtension(const std::string &OrigFile, const std::string & NewExtension);

  /**
   * ReadFile
   * Liest ein File aus (nur simple Files!!!).
   * Gibt einen Pointer auf ein istream zurück
   * im Fehlerfall wird NULL zurückgegeben
   */
  static std::istream* ReadFile(const std::string &filename);

  /**
   * WriteFile
   * Oeffnet ein File (nur simple Files!!!).
   * Gibt einen Pointer auf ein ostream zurück
   * im Fehlerfall wird NULL zurückgegeben
   */
  static std::ostream* WriteFile(const std::string &filename);

  /**
   * RemoveFile
   * Entfernt ein File (nur simple Files!!!).
   * Gibt true zurueck, wenn das File geloescht wurde. Wenn das File nicht existiert
   * wird ebenfalls true zurueck gegeben.
   */
  static bool RemoveFile(const std::string & FName);

  /**
   * GetNewestFile
   * Returns the name of the newest existing file in input files
   */
  static std::string GetNewestFile(const std::vector< std::string >& files);

  /**
   * VerifyPath
   * Diese Funktion ueberprueft, ob alle Directory-Elemente eines Files
   * vorhanden sind. Wenn nicht, wo werden sie erstellt. Wenn die Funktion
   * true zurueckgibt, so kann davon ausgegangen werden, dass das File
   * auch so erstellt werden kann.
   */
  static bool VerifyPath(const std::string & FileName, bool DoCreate = true);

  /**
   * IsFilenameQuoteChar
   * Ueberprueft, ob der char als ein quote eines Filenamens gebraucht werden kann.
   */
  inline static bool IsFilenameQuoteChar(const char  c)
    {return   Windows ? (c == '\"') : (c == '\"' || c == '\'');
    }

  /**
   * getSuffix
   * Gibt die Dateiendung zurück.
   */
  static std::string getSuffix(const std::string & FName);

  /**
   * getDataMimeType
   * Gibt die Data MimeType zurück.
   */
  static std::string getDataMimeType(const std::string& data);

  /**
   * getPreferredSuffixOfMimeType
   * Gibt die bevorzugte Erweiterung eines MimeTypes zurück.
   */
  static std::string getPreferredSuffixOfMimeType(const std::string& mime);

  /**
   * getDataFilename
   * Wenn die Daten mit einem Filenamen mit diesem Schema "data:filename/<filename>;" beginnen.
   * Gibt es den Filenamen zurück.
   * Und löscht diese Filename aus den Daten.
   */
  static std::string getDataFilename(std::string& data);

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/
protected:


/*******************************************************************************/
/* Member variables                                                            */
/*******************************************************************************/
protected:

};

#endif // !defined(AFX_FILEUTILITIES_H_INCLUDED_)
