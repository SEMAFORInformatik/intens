#ifndef FDOSTREAM_H
#define FDOSTREAM_H
/**********************************************************************************/
/* fdiostream.h                                                                   */
/* Dieses Klassen kapseln die Klassen istream und ostream um mit einem            */
/* beliebigen Datei Deskriptor einen Eingabestream bzw. Ausgabestream zu erzeugen.*/
/*                                                                                */
/* fdistream heisst die neue gekapselte Eingabestream Klasse                      */
/* Aufruf einfach mit einem Datei Descriptor                                      */
/* z.B.: fdistream in(0);  // entspricht cin                                      */
/*                                                                                */
/* fdostream heisst die neue gekapselte Ausgabestream Klasse                      */
/* Aufruf einfach mit einem Datei Descriptor                                      */
/* z.B.: fdostream in(0);  // entspricht cin                                      */
/**********************************************************************************/

#include <streambuf>
#include <iostream>
#include <string.h>

const int FD_BUFFERSIZE = 100;

/** This class handles only output streams and connects a file descriptor with the
    streambuf class. It is a auxiliary class and should be only used by the class fdostream.
    @author Semafor Informatik & Energie AG
    @version $id$
    @short This class is a derived class of the streambuf class, which connects a
           file descriptor with the streambuf class.
   @see fdistream
*/
class fdoutbuf : public std::streambuf {

 public:
  /** Constructor which needs a file descriptor
      @param fd file descriptor
      @param is_buffer should the output stream be buffered
   */
  fdoutbuf( int _fd, bool _is_buffer) : fd( _fd), is_buffer(_is_buffer)  {
    if ( is_buffer ) {
      setp( buffer, buffer+(FD_BUFFERSIZE-1) );
    }
  }
  /** Destructor */
  ~fdoutbuf() {
    if ( is_buffer ) sync();
  }

 protected:
  /** clear buffer function used when stream is buffered */
  int clearBuffer() {
    if ( !is_buffer ) return 0;
    int cnt = pptr()-pbase();
    if( write(1, buffer, cnt) != cnt )  return EOF;
    pbump( -cnt );
    return cnt;
  }

  /** function to write a single character into the stream
   @param c character in ASCII Code to write into stream
   @return the written character in ASCII Code (EOF if failed)
  */
  virtual int_type overflow( int_type c = EOF) {

    // stream with buffer
    if ( is_buffer ) {
      if( c != EOF ) {
	*pptr() = c;
	pbump(1);
      }
      if( clearBuffer() == EOF )  return EOF;
      return c;

    }  else {
      // stream without buffer
     if( c != EOF ) {
	char z = c;
	if(  write( fd, &z, 1) != 1 ) {
	  return EOF;
	}
      }
      return c;
    }
  }

  /** function to write multiple characters
      @param s character pointer
      @param anz number of characters
   */
  virtual std::streamsize xsputn( const char* s , std::streamsize anz ) {
    return  write( fd, s, anz );
  }

  /** synchroize data with data buffer is used when stream is buffered */
  virtual int sync() {
    if ( !is_buffer ) return 0;
    if( clearBuffer() == EOF )  return -1;
    return 0;
  }

 protected:
  /** file descriptor */
  int fd;
  /** data buffer */
  char buffer[ FD_BUFFERSIZE ];
  /** is stream buffered */
  bool is_buffer;
};


/** This class can be used in the same way as for examples the cout class with the
    exception that you can select the file descriptor you want to connect the output stream.
    @author Semafor Informatik & Energie AG
    @version $id$
    @short This class is a derived class of the ostream class, which connects a
           file descriptor with the ostream class.
    @see fdistream
*/

class fdostream : public std::ostream {
public:
  /** Constructor which needs a file descriptor
      @param fd file descriptor
      @param with_buffer Should the stream buffer be buffered. Default value is false
  */
  fdostream( int fd, bool with_buffer=false ) : buf(fd, with_buffer), std::ostream(& buf ) { rdbuf(&buf); }
protected:
  /** output stream buffer instance */
  fdoutbuf buf;
};

#include <zlib.h>

class gzofbuf : public std::streambuf {

 public:
  /** Constructor which needs a file descriptor
      @param fd file descriptor
      @param is_buffer should the output stream be buffered
   */
  gzofbuf( const std::string &fn , bool _is_buffer)
    : is_buffer(_is_buffer)  {
    m_fp = gzopen( fn.c_str(), "wb" );
    if ( is_buffer ) {
      setp( buffer, buffer+(FD_BUFFERSIZE-1) );
    }
  }

  gzofbuf( const char *fn , bool _is_buffer)
    : is_buffer(_is_buffer)  {
    m_fp = gzopen( fn, "wb" );
    if ( is_buffer ) {
      setp( buffer, buffer+(FD_BUFFERSIZE-1) );
    }
  }

  /** Destructor */
  ~gzofbuf() {
    if ( is_buffer ) sync();
    if( m_fp != 0 ) gzclose( m_fp );
  }

  gzFile fp(){ return m_fp; }
  void close(){
    if( m_fp != 0 ){
      gzclose( m_fp );
      m_fp = 0;
    }
  }
 protected:
  /** clear buffer function used when stream is buffered */
  int clearBuffer() {
    if ( !is_buffer ) return 0;
    int cnt = pptr()-pbase();
    if( write(1, buffer, cnt) != cnt )  return EOF;
    pbump( -cnt );
    return cnt;
  }

  /** function to write a single character into the stream
   @param c character in ASCII Code to write into stream
   @return the written character in ASCII Code (EOF if failed)
  */
  virtual int_type overflow( int_type c = EOF) {

    // stream with buffer
    if ( is_buffer ) {
      if( c != EOF ) {
	*pptr() = c;
	pbump(1);
      }
      if( clearBuffer() == EOF )  return EOF;
      return c;

    }  else {
      // stream without buffer
     if( c != EOF ) {
	char z = c;
	if(  gzwrite( m_fp, &z, 1) != 1 ) {
	  return EOF;
	}
     }
     return c;
    }
  }

  /** function to write multiple characters
      @param s character pointer
      @param anz number of characters
  */
  virtual std::streamsize xsputn( /*const*/ char* s , std::streamsize anz ) {
    return gzwrite(
		   m_fp
		   , s
		   , anz );
  }

  /** synchroize data with data buffer is used when stream is buffered */
  virtual int sync() {
    if ( !is_buffer ) return 0;
    if( clearBuffer() == EOF )  return -1;
    return 0;
  }

 protected:
  /** file descriptor */
  gzFile m_fp;
  /** data buffer */
  char buffer[ FD_BUFFERSIZE ];
  /** is stream buffered */
  bool is_buffer;
};


/** This class can be used in the same way as for examples the cout class with the
    exception that you can select the file descriptor you want to connect the output stream.
    @author Semafor Informatik & Energie AG
    @version $id$
    @short This class is a derived class of the ostream class, which connects a
           file descriptor with the ostream class.
    @see fdistream
*/

class gzofstream : public std::ostream {
public:
  /** Constructor which needs a file descriptor
      @param fd file descriptor
      @param with_buffer Should the stream buffer be buffered. Default value is false
  */
  gzofstream( const std::string &fn, bool with_buffer=false )
    : buf(fn, with_buffer)
    , std::ostream(& buf ){
    rdbuf(&buf);
    if( buf.fp() == NULL ){
      setstate( std::ios::failbit );
    }
    else{
      clear();
    }
  }

  gzofstream( const char *fn, bool with_buffer=false )
    : buf(fn, with_buffer)
    , std::ostream(& buf ){
    rdbuf(&buf);
    if( buf.fp() == NULL ){
      setstate( std::ios::failbit );
    }
    else{
      clear();
    }
  }
  void close(){
    buf.close();
  }
protected:
  /** output stream buffer instance */
  gzofbuf buf;
};

class gzifbuf : public std::streambuf {
protected:
  gzFile m_fp;    // file descriptor
protected:
  /* data buffer:
   * - at most, pbSize characters in putback area plus
   * - at most, bufSize characters in ordinary read buffer
   */
  static const int pbSize = 4;        // size of putback area
  static const int bufSize = 1024;    // size of the data buffer
  char buffer[bufSize+pbSize];        // data buffer

public:
  /* constructor
   * - initialize file descriptor
   * - initialize empty data buffer
   * - no putback area
   * => force underflow()
   */
  gzifbuf (const std::string &fn){
    if( fn.find(".gz", fn.size()-3) == std::string::npos ){
      m_fp=NULL;
    }
    else{
      m_fp = gzopen( fn.c_str(), "rb" );
    }
    setg (buffer+pbSize,     // beginning of putback area
	  buffer+pbSize,     // read position
	  buffer+pbSize);    // end position
  }
  virtual ~gzifbuf(){
    gzclose( m_fp );
  }
  gzFile fp(){ return m_fp; }
protected:
  // insert new characters into the buffer
  virtual int_type underflow () {
#if __GNUC__ < 4 //#ifndef _MSC_VER
    using std::memmove;
#endif

    // is read position before end of buffer?
    if (gptr() < egptr()) {
      return traits_type::to_int_type(*gptr());
    }

    /* process size of putback area
     * - use number of characters read
     * - but at most size of putback area
     */
    int numPutback;
    numPutback = gptr() - eback();
    if (numPutback > pbSize) {
      numPutback = pbSize;
    }

    /* copy up to pbSize characters previously read into
     * the putback area
     */
    memmove (buffer+(pbSize-numPutback), gptr()-numPutback,
	     numPutback);

    // read at most bufSize new characters
    int num = 0;
    if( m_fp != 0 ){
      num = gzread (m_fp, buffer+pbSize, bufSize);
    }
    if (num <= 0) {
      // ERROR or EOF
      return EOF;
    }

    // reset buffer pointers
    setg (buffer+(pbSize-numPutback),   // beginning of putback area
	  buffer+pbSize,                // read position
	  buffer+pbSize+num);           // end of buffer

    // return next character
    return traits_type::to_int_type(*gptr());
  }
};

class gzifstream : public std::istream {
protected:
  gzifbuf buf;
public:
  gzifstream ( const char *filename )
    : std::istream(0)
    , buf(std::string(filename)) {
    rdbuf(&buf);
    if( buf.fp() == NULL ){
      setstate( std::ios::failbit );
    }
  }

  gzifstream( const std::string & filename )
    : std::istream(0)
    , buf(filename) {
    rdbuf(&buf);
    if( buf.fp() == NULL ){
      setstate( std::ios::failbit );
    }
  }
};

#endif
