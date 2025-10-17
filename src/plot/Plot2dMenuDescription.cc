
#include <iostream>
#include <algorithm>

#include "plot/Plot2dMenuDescription.h"
#include "utils/gettext.h"

Plot2dMenuDescription *Plot2dMenuDescription::s_instance = 0;

Plot2dMenuDescription::Plot2dMenuDescription()
  : m_clearEntries( true ){
  m_entryMap.insert( entryPair( eREDRAW, "Redraw" ) );
  m_entryMap.insert( entryPair( eANNOTATION, "Annotation" ) );
  m_entryMap.insert( entryPair( eENDPOINTS, "EndPoints" ) );
  m_entryMap.insert( entryPair( eRESET, "Reset" ) );
  m_entryMap.insert( entryPair( ePRINT, "Print" ) );
  m_entryMap.insert( entryPair( eCOPY, "Copy" ) );
  m_entryMap.insert( entryPair( eFULLSCREEN, "Fullscreen" ) );
  m_entryMap.insert( entryPair( eDELETE_TEXT, "DeleteText" ) );
  m_entryMap.insert( entryPair( eSHOW_TEXT, "ShowText" ) );
  m_entryMap.insert( entryPair( eCONFIG, "Config" ) );
  m_entryMap.insert( entryPair( eCYCLES, "Cases" ) );
  m_entryMap.insert( entryPair( eSCALE, "Scale" ) );
  m_entryMap.insert( entryPair( eLOGARITHMIC, "Logarithmic" ) );
  m_entryMap.insert( entryPair( eSTYLE, "Style" ) );
  m_entryMap.insert( entryPair( eY_IN_FRONT, "yInFront" ) );
  m_entryMap.insert( entryPair( e2ND_PRINTSTYLE, "2ndPrintstyle" ) );
#ifdef HAVE_QT
  m_entryMap.insert( entryPair( ePROPERTY, "Property" ) );
  m_entryMap.insert( entryPair( eZOOM, "Zoom" ) );
#endif
  EntryMap::iterator iter;
  for( iter = m_entryMap.begin(); iter != m_entryMap.end(); ++iter )
    m_entries.push_back( (*iter).first );
  // new entries ( not in standard menu )
  m_entryMap.insert( entryPair( eZOOM, "Zoom" ) );
}

bool Plot2dMenuDescription::addEntry( const std::string &entry ){
  if( m_clearEntries ){
    m_entries.clear();
    m_clearEntries = false;
  }
  eEntry e;
  if( ( e = findEntry( entry ) ) != eNONE  ){
    if( std::find( m_entries.begin(), m_entries.end(), e ) == m_entries.end() ){
      m_entries.push_back( e );
    }
    return true;
  }
  return false;
}

Plot2dMenuDescription::eEntry Plot2dMenuDescription::findEntry( const std::string &entry ){
  std::map<eEntry, std::string>::iterator iter;
  for( iter = m_entryMap.begin(); iter != m_entryMap.end(); ++iter ){
    if( (*iter).second == entry )
      return (*iter).first;
  }
  return eNONE;
}
