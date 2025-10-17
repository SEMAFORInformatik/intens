#ifndef FLEXER_H
#define FLEXER_H

#include<string>
#include<map>
#include<stack>
#include<vector>
#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif
#include "parser/HMap.h"

class Flexer : public yyFlexLexer{
  //----------------------------------------------------------------//
  // Constructor / Destructor                                       //
  //----------------------------------------------------------------//
 public:
  Flexer( std::istream *is, int startToken=0 );
 private:
  /** Der Default- und Copy-Konstruktor soll nicht aufgerufen werden koennen
   */
  Flexer();
  Flexer( const Flexer& );
  //----------------------------------------------------------------//
  // public member functions                                        //
  //----------------------------------------------------------------//
 public:
  /**
   * return token for parser (can be either 0, DESCRIPTION or DATAPOOL)
   */
  int startToken();

  /** return token for parser
   */
  int reserved_word_type();
  /** open include file and save current filename and line number
   */
  bool IncludeFile( const std::string &inclFileName
		    , const int lineNo
		    , int bufSize );
  /** return line number after processing include file
   */
  bool endOfFile( int &lineNo );
  /** set current filename (if possible with pathname)
   */
  void setCurrentFilename( const std::string &filename ){
    if( filename.find( "/usr/include" ) == std::string::npos ){
      m_currentFilename = filename;
    }
  }
  /** returns the complete filename (incl. path if possible) of the current
      file
  */
  const std::string &getCurrentFilename() const{ return m_currentFilename; }
  //----------------------------------------------------------------//
  // private member functions                                       //
  //----------------------------------------------------------------//
 private:
  void initRwTabMap();
  void initTokenMap();
  void initInclFileList();
  //----------------------------------------------------------------//
  // private definitions                                            //
  //----------------------------------------------------------------//
 private:
  class InclFile{
  public:
    InclFile( yy_buffer_state *include, const std::string &filename, int lineno  );

    yy_buffer_state *m_include;
    int              m_lineno;
    std::string      m_filename;
  };

  typedef HMap RwTabMap;
  typedef std::map< int, int > TokenMap;
  typedef TokenMap::value_type intPair;
  //----------------------------------------------------------------//
  // private data                                                   //
  //----------------------------------------------------------------//
 private:
  TokenMap m_lexIdTokenMap;
  RwTabMap m_rwTabMap;
  std::stack< InclFile *> m_inclFileStack;
  std::vector<std::string> m_inclPathList;
  std::string         m_currentFilename;
  int start_token;
};
#endif
