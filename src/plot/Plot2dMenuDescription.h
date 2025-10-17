
#include <map>
#include <vector>
#include <string>

#ifndef PLOT2D_MENU_DESCRIPTION_H
#define PLOT2D_MENU_DESCRIPTION_H

class Plot2dMenuDescription{
  /*=============================================================================*/
  /* public definitions                                                          */
  /*=============================================================================*/
 public:
  enum eEntry{
    eREDRAW
    , eZOOM
    , eANNOTATION
    , eENDPOINTS
    , eRESET
    , ePRINT
    , eDELETE_TEXT
    , eSHOW_TEXT
    , eCONFIG
    , eCYCLES
    , eSCALE
    , eLOGARITHMIC
    , eSTYLE
    , eY_IN_FRONT
    , e2ND_PRINTSTYLE
    , eCOPY
    , eFULLSCREEN
    , ePROPERTY
    , eNONE
  };

  /*=============================================================================*/
  /* private definitions                                                         */
  /*=============================================================================*/
 private:
  typedef std::map<eEntry, std::string> EntryMap;
  typedef EntryMap::value_type entryPair;

  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
 public:
  static Plot2dMenuDescription &Instance(){
    if( s_instance == 0 )
      s_instance = new Plot2dMenuDescription();
    return *s_instance;
  }
  virtual ~Plot2dMenuDescription() {}

  Plot2dMenuDescription();

 private:
  Plot2dMenuDescription(Plot2dMenuDescription&);
  Plot2dMenuDescription& operator=(const Plot2dMenuDescription&);

  /*=============================================================================*/
  /* public functions                                                            */
  /*=============================================================================*/
 public:
  bool addEntry( const std::string &entry );
  const std::vector<eEntry> &getDescription(){ return m_entries; }

  /*=============================================================================*/
  /* private functions                                                           */
  /*=============================================================================*/
  eEntry findEntry( const std::string &entry );

  /*=============================================================================*/
  /* private members                                                             */
  /*=============================================================================*/
  static Plot2dMenuDescription *s_instance;
  bool                          m_clearEntries;
  EntryMap                      m_entryMap;
  std::vector<eEntry>           m_entries;
};

#endif
