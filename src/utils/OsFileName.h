
/*******************************************************************************/
/* Diese Klasse abstarahiert einen Filename. Unterschiedliche Betriebssystem   */
/* haben unterschiedliche Filenamens-Konventionen. D.h. wie ein Filename auf-  */
/* gebaut ist. Diese Klasse versucht, alle Unterschiede auszubuegeln und ein   */
/* einheitliches Interface zu den Namen zu lieferen.                           */
/*******************************************************************************/


#if !defined(AFX_OSFILENAME_H__INCLUDED_)
#define AFX_OSFILENAME_H__INCLUDED_

#ifndef _MSC_VER
#include <sys/param.h> // MAXPATHLEN is defined here
#else
#include <windows.h>
#define MAXPATHLEN MAX_PATH
#endif

#include <assert.h>

#include "utils/Root.h"
#include "utils/FileUtilities.h"


class OsFileName : public Root
{
public:
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
	OsFileName();
  OsFileName(const std::string & Fil);
	virtual ~OsFileName();
  OsFileName(const OsFileName &ref);
  OsFileName& operator=(const OsFileName &ref);

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
public:

  /**
   * NewFile
   * Parses a Filename and breaks it in pieces. Returns true, if successful
   */

  bool NewFile(const std::string & Fil);
  /**
   * IsValid
   * Ueberprueft, ob erfogreich geparset wurde
   */
  inline bool IsValid(void)               {return m_Valid;                          }

  /**
   * IsRelative
   * Checkt, ob ein relativer Filename vorliegt
   */
  inline bool IsRelative(void)            {return m_Relative;                       }

  /**
   * HasDriveLetter
   * Checkt, ob ein drive Letter im Filename ist.
   */
  inline bool HasDriveLetter(void)       {return Windows? m_DriveLetter : false;   }

  /**
   * IsUNC
   * Checkt, ob ein UNC-Filename vorliegt
   */
  inline bool IsUNC(void)                 {return Windows? m_UNC : false;           }


  /**
   * MakeAbsolutFilename
   * Kosntruiert einen Absoluten Filename
   */
  bool MakeAbsolutFilename(void);

  /**
   * GetName
   * Gibt den gesamten Namen als String zurueck
   */
  inline std::string GetName(void)         {assert(IsValid());   return std::string(m_FileName,m_Len);    }
  inline void   GetName(std::string & s)   {assert(IsValid());   s = std::string(m_FileName,m_Len);       }


/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/
protected:

  /**
   * InitFile
   * Initialisiert die lokalen Variablen so dass kein Filename und/oder
   * Directory spezifiziert ist.
   */
  void InitFile(void);

  /**
   * Initialisiert nur die Parser-Related variablen
   */
  void InitParse(void);

  /**
   * Parse
   * Analysiert die Variable in m_FileName
   */
  bool Parse(void);


/*******************************************************************************/
/* Member variables                                                            */
/*******************************************************************************/
protected:

  char m_FileName[MAXPATHLEN+10];               // Filename in einem Displayablen Format
  int  m_Len;                                   // Length of Filename
  int  m_DirIndex[25];                          // IndexArray. Index 0 definiert den Start des
                                                // Files nach dem Prefix der je nach OS ganz
                                                // unterschiedlich sein kann.
  int  m_NumDirs;                               // Anzahl Dirctories
  bool m_Relative;                              // Realtive Filename
  bool m_Quotes;                                // quoted Filename
  bool m_Valid;                                 // Valid Filename m_FileName
  bool m_UNC;                                   // File ist ein UNC-File auf einem Network-Host.
                                                // Der UNC-Name beginnt bei m_FileName[2] und erstreckt sich bis
                                                // zu m_DirIndex[0]-2;
  bool m_DriveLetter;                           // m_FileName[0] is Drive-Letter
};

#endif // !defined(AFX_OSFILENAME_H__INCLUDED_)
