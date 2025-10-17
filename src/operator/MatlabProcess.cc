
#include <typeinfo>
#include <string>
#include <complex>
#include <algorithm>
#include <numeric>
#include <sstream>

#ifdef HAVE_MATLAB
#define V5_COMPAT
#include <engine.h>
#else
typedef int mxComplexity;
typedef int mxClassID;
typedef int Engine;
typedef int mwSize;
typedef int mwIndex;
const int mxREAL=0;
const int mxCOMPLEX=0;
const int mxDOUBLE_CLASS=1;
#endif

#include "operator/Channel.h"
#include "operator/MatlabInitProcess.h"
#include "operator/MatlabProcess.h"
#include "operator/MatlabConnection.h"
#include "streamer/DataStreamParameter.h"
#include "streamer/StructStreamParameter.h"
#include "utils/utils.h"
#include "utils/Debugger.h"
#include "app/AppData.h"
#include "app/ModuleFactory.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiFactory.h"

char * MatlabProcess::s_msgBuffer=0;
int MatlabProcess::s_configured=0;

INIT_LOGGER();

/* initialize */
MatlabConnection *MatlabConnection::s_instance = 0;
/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */
MatlabConnection& MatlabConnection::Instance(){
  if( s_instance == 0 ){
    s_instance = new MatlabConnection();
  }
  return *s_instance;
}

//---------------------------------------------------------------------
// Auxiliary classes
//
class MatlabArray: public StreamSource {
public:
  MatlabArray( mxArray *m=0 ):m_pm(m), m_parent(0), m_failed(false){}
  virtual ~MatlabArray(){
#ifdef HAVE_MATLAB
//     if( m_parent == 0 )
//       driver.DestroyArray( m_pm );
#endif
  }
  mxArray *getArray(){ return m_pm; }
  void setParent( MatlabArray *p ){ m_parent=p; }

  virtual void getValues( DataStreamParameter &dpar );
  virtual void getValues( DataStreamParameter &dpar
			  , const std::vector<DataStreamParameter *>&fieldlist );
  std::string getMessages() const { return m_messages; }
  bool hasFailed() const { return m_failed; }

protected:
  bool getString( mxArray *mtx, std::vector<std::string> &svals )const;
  void rearrange( std::vector<double> &dvec, const double *re
		  , int ndims, const mwSize *dims
		  , int idim, int start, int step ) const;

  void rearrange( std::vector<std::string> &svec, mxArray *mtx
		  , int ndims, const mwSize *dims
		  , int idim, int start, int step ) const;

  void rearrange( DataStreamParameter &dparent, DataStreamParameter &dfield
		  , int f, mxArray *mtx
		  , int ndims, const mwSize *dims
		  , int idim, int start, int step );

  void rearrangeCell( mxArray *mtx, std::vector<double> revec, std::vector<double> imvec
		 , int ndims, const mwSize *dims
		 , int idim, int start, int step ) const;
  MatlabArray *m_parent;
  mxArray *m_pm;
  std::string   m_messages;
  bool     m_failed;
};

class MatlabRealArray: public MatlabArray{
public:
  MatlabRealArray( const std::vector<int>&dims, const std::vector<double>&vals );
};
class MatlabCellArray: public MatlabArray{
public:
  MatlabCellArray( const std::vector<int>&dims, const std::vector<double>&vals );
};
class MatlabComplexArray: public MatlabArray{
public:
  MatlabComplexArray( const std::vector<int>&dims, const std::vector<dComplex>&vals );
};

class MatlabStringArray: public MatlabArray{
public:
  MatlabStringArray( const std::vector<int>&dims, const std::vector<std::string>&vals );
  MatlabStringArray( const std::string& val );
};

class MatlabStructArray: public MatlabArray, public StreamDestination {
public:
  MatlabStructArray( StructStreamParameter &par
		     , const std::vector<DataStreamParameter *> fieldList
		     , const std::vector<int>&dims );
  void putValues ( StreamParameter &dpar, const std::vector<int>&dims
		   , const std::vector<dComplex>&vals
		   , const std::vector<int> &inds );
  void putValues ( StreamParameter &dpar, const std::vector<int>&dims
		   , const std::vector<double>&vals, bool isCell, const std::vector<int> &inds );
  void putValues ( StreamParameter &dpar, const std::vector<int>&dims
		   , const std::vector<std::string>&vals );
  void putValues ( DataStreamParameter &dpar
		   , const std::vector<DataStreamParameter *> &fieldList
		   , const std::vector<int>&dims );
  void putValue ( StreamParameter &dpar, const std::string &val );

private:
  void putFields( const std::vector<DataStreamParameter *> &fieldList
		  , int ndim, const std::vector<int> &idims
		  , int ipos, int start, int step );

  mxArray *m_tmpArray;
  StructStreamParameter &m_par;
  int _nfields;
};

/*=============================================================================*/
/* MatlabProcess                                                               */
/*=============================================================================*/

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

MatlabProcess::MatlabProcess( const std::string &name )
  : Process( name )
  , m_logstr( 0 )
  , m_terminated(true)
  , m_thread(this) {
  s_configured++;
}

MatlabProcess::MatlabProcess( const MatlabProcess &process )
  : Process( process )
  , m_evalString( process.m_evalString )
  , m_logstr( 0 )
  , m_terminated(true)
  , m_thread(this) {
  s_configured++;
}

MatlabProcess::~MatlabProcess(){
  if( s_configured == 1){
    MatlabProcess::close();
  }
  s_configured--;
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

Process *MatlabProcess::clone(){
  return new MatlabProcess( *this );
}

//---------------------------------------------------------------------
// MatlabProcess start a new process
//

bool MatlabProcess::start(){
  BUG_DEBUG("MatlabProcess::start()" );
  MatlabConnection& driver = MatlabConnection::Instance();

  if( driver.Open == 0 ){
    std::ostringstream ostr;
    ostr << "Cannot start Matlab Func " << getExecCmd() << std::endl;
    ostr << "No matlab connection"<<std::endl;
    setErrorMessage( ostr.str() );
    BUG_DEBUG( getErrorMessage() );
    return false;
  }
  m_terminated=false;
  setErrorMessage("");
  std::ostringstream eval;
  // remove all allocated arrays
  std::for_each( m_mxArrayVec.begin(), m_mxArrayVec.end(), driver.DestroyArray );
  m_mxArrayVec.clear( );

  eval << "[";
  getOutputParameters( eval );
  eval << "]=" << getExecCmd();
  std::ostringstream inpars;
  getInputParameters( inpars );
  if ( !inpars.str().empty() ){
    eval << "(" << inpars.str() << ");";
  }
  m_evalString=eval.str();

  //  std::cerr << m_evalString << std::endl;

  BUG_DEBUG( m_evalString );
  putStreamValues(true);  // Transfer the values to the Matlab workspace

  m_is_running = true;
  setExitStatus(0);
#if defined( linux ) || defined( __hpux ) ||  defined( __WIN32__ )
  execute();
#else
  m_thread.start();
#endif
  return true;
}
/*------------------------------------------------------------
  startThread --
  ------------------------------------------------------------*/
void MatlabProcess::startThread() {
  execute( );
}

void MatlabProcess::execute(){
  int rslt;
  BUG_DEBUG("MatlabProcess::execute evalString: " << m_evalString);
  MatlabConnection& driver = MatlabConnection::Instance();

  if( driver.engine != 0 ){
    //    std::string eval( "evalin('base'," );
    //     eval += m_evalString + ")";
    if( (rslt=driver.EvalString(driver.engine, m_evalString.c_str() )) != 0 ){
      GuiFactory::Instance()->getLogWindow()->writeText( compose( _("%1: Cannot evaluate matlab command '%2' [%3]\n"), DateAndTime(),  m_evalString, getMessages() ) );
      //     std::cerr << getMessages() << std::endl;
      //     throw MatlabException( msg, __FILE__, __LINE__ );
    }
  }
  else {
    BUG_DEBUG( "no engine" );
  }
  m_thread.lock();
  m_terminated=true;
  m_thread.unlock();
}
//-----------------------------------------------------------------
// Print MATLAB messages
//
void MatlabProcess::printMessages(std::ostream &ostr) const {
  if( (s_msgBuffer == 0) && getErrorMessage().empty() )
    return;
  if( !getErrorMessage().empty() )
    ostr << getErrorMessage() << std::endl;
  if( strlen( s_msgBuffer ) ){
    ostr << s_msgBuffer << std::endl;
  }
}
std::string MatlabProcess::getMessages() const {
  if( s_msgBuffer == 0 )
    return getErrorMessage();
  if( strlen( s_msgBuffer ) ){
    return getErrorMessage() + s_msgBuffer;
  }
  return "";
}
std::string MatlabProcess::getErrorMessage() const {
  if( s_msgBuffer == 0 ){
    return "";
  }
  std::string errmsg;

  if( strlen( s_msgBuffer )>0 ){
    std::string msg( s_msgBuffer );
    std::string::size_type pos=0;
    while ((pos = msg.find("??? ", pos ))!= std::string::npos) {
      std::string::size_type posend=msg.find("\n\n", pos );
      pos +=4;
      if( posend != std::string::npos ){
	errmsg += msg.substr( pos, posend-pos );
      }
      else {
	errmsg += msg.substr( pos );
      }
    }
  }
  return errmsg;
}
//-----------------------------------------------------------------
//  count errors
//
int MatlabProcess::countErrors() const {
  int errors=getErrorMessage().find("Error")!=std::string::npos ? 1 : 0;

  if( s_msgBuffer == 0 ){
    return errors;
  }

  if( strlen( s_msgBuffer )>0 ){
    std::string msg( s_msgBuffer );
    std::string::size_type pos=0;
    while ((pos = msg.find("???", pos ))!= std::string::npos) {
      ++pos;
      ++errors;
    }
  }
  return errors;
}
//-----------------------------------------------------------------
//  stop process
//
bool MatlabProcess::stop(){
  m_thread.lock();
  if( !m_terminated )
    m_thread.terminate();
  m_terminated = true;
  m_thread.unlock();
  return true;
}

bool MatlabProcess::hasTerminated(){
  BUG_DEBUG("MatlabProcess::hasTerminated()" );
  bool terminated;
  m_thread.lock();
  terminated=m_terminated;
  m_thread.unlock();

  if( !terminated ){
    BUG_DEBUG( "MatlabProcess Not yet Terminated" );
    return false;
  }
  // is it necessary ? pthread_join( m_run_thread, 0 );
  BUG_DEBUG( getMessages() );
  GuiFactory::Instance()->getLogWindow()->writeText( getMessages() );

  setExitStatus( countErrors() );
  if( getExitStatus()==0 ){
    getStreamValues();
    GuiFactory::Instance()->getLogWindow()->writeText(DateAndTime() +
						     ": " +
						     getExecCmd() +
						     " terminated\n" );
  }
  m_is_running = false;
  return true;
}

//-----------------------------------------------------------------
// Startup MATLAB engine either locally or remote
// and register the output buffer for messages
//
bool MatlabProcess::open( const std::string &cmd ){
  BUG_DEBUG("MatlabProcess::open() cmd: " << cmd);
  MatlabConnection& driver = MatlabConnection::Instance();
  if( driver.engine!=0 ){
    std::cerr << _("WARNING: MatlabProcess::open() cannot open multiple Matlab Connections!") << std::endl;
    return false;
  }

  bool ret = ModuleFactory::Instance().createMatlabConnection();
  if( !ret || driver.Open == 0 ){
    std::cerr << _("MatlabProcess::open() cannot start engine\n");
    std::cerr << "Cmd: " << cmd <<"\n";
    GuiFactory::Instance()->getLogWindow()->writeText( _("MatlabProcess::open() cannot start engine\n") );
    return false;
  }
  driver.engine = driver.Open( cmd.c_str() );

  if( driver.engine != 0 ){
    if( s_msgBuffer == 0 )
      s_msgBuffer = new char [BUFSIZ+1];
    s_msgBuffer[0]='\0';
    if( driver.OutputBuffer( driver.engine, s_msgBuffer, BUFSIZ ) != 0 ){
      // cannot start matlab (license problem?)
      GuiFactory::Instance()->getLogWindow()->writeText( "cannot start Matlab" );
      if( !cmd.empty() )
	GuiFactory::Instance()->getLogWindow()->writeText( cmd );
      GuiFactory::Instance()->getLogWindow()->writeText( "\n" );

      driver.Close( driver.engine );
      driver.engine = 0;
      return false;
    }
    GuiFactory::Instance()->getLogWindow()->writeText( compose( _("%1: Matlab %2 started\n"), DateAndTime(), cmd ) );
    return true;
  }
  return false;
}
//-----------------------------------------------------------------
// final Shutdown of MATLAB engine
//
void MatlabProcess::close(){
  MatlabConnection& driver = MatlabConnection::Instance();
  if( driver.engine != 0 ){
    driver.Close( driver.engine );
    driver.engine=0;
  }
  if( s_msgBuffer != 0 ){
    delete [] s_msgBuffer;
    s_msgBuffer=0;
  }
}

//----------------------------------------------------------------------
// creates a numeric array
MatlabRealArray::MatlabRealArray ( const std::vector<int>&dims
				   , const std::vector<double>&vals ){
  mxComplexity complex_flag = mxREAL;
  mwSize ndim=dims.size();
  MatlabConnection& driver = MatlabConnection::Instance();

  BUG_DEBUG("MatlabRealArray::MatlabRealArray( const std::vector<int> ..) " );
  if( ndim < 1 ){
//        cerr << "empty matrix" << std::endl;
    BUG_DEBUG("Call matlab func CreateDoubleMatrix" );
    m_pm=driver.CreateDoubleMatrix( 1, 0, complex_flag );
    BUG_DEBUG("Call matlab func CreateDoubleMatrix Done" );
    return;
  }
  if( ndim == 1 )
    ndim=2;

  mwSize *idims=new mwSize[ ndim ];
  double *dval=new double [ vals.size() ];
  std::copy( dims.begin(), dims.end(), idims );
  std::copy( vals.begin(), vals.end(), dval );
  int len=vals.size();
  if( dims.size() == 1 ){
    idims[0]=1; // we always make two dimensions!
    idims[1]=dims[0];
  }
  else {
    len=std::accumulate( dims.begin(), dims.end(), 1, std::multiplies<int>() );
  }
  if( len == vals.size() ) {

    m_pm = driver.CreateNumericArray(ndim, idims, mxDOUBLE_CLASS, complex_flag);
    BUG_DEBUG("Call matlab func GetPr, ptr: " << driver.GetPr(m_pm));
    if (driver.GetPr(m_pm))
      memcpy((void *)driver.GetPr(m_pm), (void *)dval, len*sizeof(double));
  }
  else { // any better idea? this should really not happen. maybe it is a cell?
    BUG_DEBUG("Call matlab func CreateDoubleMatrix" );
    m_pm=driver.CreateDoubleMatrix( 1, 0, complex_flag );
  }
//       cerr << " pm=" << m_pm << " len=" << len << " dval=" << dval << " pr=" << driver.GetPr(m_pm) << std::endl;
  delete [] idims;
  delete [] dval;
  BUG_DEBUG("MatlabRealArray::MatlabRealArray Done.) " );
}
//----------------------------------------------------------------------
// creates a numeric cell array
//
// The dims are interpreted as follows:
//     dims[0] -> number of dimensions ndims
//     dims[1..ndims] -> dimension sizes
//     dims[ndims+1 .. dims.size()] sizes of the individual cells.
//
// Examples:
//  1.  dims={2 3 1 0 2 3} vals={1 2 3 4 5}
//        gives: { {}, {1,2}, {3,4,5} }
//
//  2.  dims={2 2 1}   vals={30 70}
//        gives: {{30,70}}
//
MatlabCellArray::MatlabCellArray ( const std::vector<int>&dims
				   , const std::vector<double>&vals ){
  mxComplexity complex_flag = mxREAL;
  mwSize ndim=dims.size();
  MatlabConnection& driver = MatlabConnection::Instance();

//   cerr << "MatlabCellArray::MatlabCellArray( const std::vector<int> ..) ";
//   copy( dims.begin(), dims.end(), std::ostream_iterator<int>( cerr, " " ) );
//   cerr << std::endl;
//   cerr << "vals:\n";
//   copy( vals.begin(), vals.end(), std::ostream_iterator<double>( cerr, " " ) );
//   cerr << std::endl;

  BUG_DEBUG("MatlabCellArray::MatlabCellArray( const std::vector<int> ..) " );
  if( ndim < 1 || vals.size()==0 || dims.size()==0 ){
    m_pm=driver.CreateCellMatrix( 1, 0 );
    return;
  }
  std::vector<int>::const_iterator di=dims.begin();
  ndim=*di++;
  int startIndex=ndim+1;
  std::vector<int> tempDims(di, di+ndim);
  if( ndim < 2 ){ // we always want 2 dimensions at least
    ndim=2;
    tempDims.push_back(1);
  }

  mwSize *idims=new mwSize [ ndim ];
  double *dval=new double [ vals.size() ];

  std::copy( tempDims.begin(), tempDims.end(), idims );
  std::copy( vals.begin(), vals.end(), dval );

//   cerr << "ndim = " << ndim << " tempDims: ";
//   std::copy( tempDims.begin(), tempDims.end(), std::ostream_iterator<int>( cerr, " " ) );
//   cerr << std::endl;

  int numCells=1;

  for( int i=0; i<ndim; ++i ){
    numCells *= idims[i];
  }

  m_pm = driver.CreateCellArray( ndim, idims );

  int j=0;
  for( int c=0; c<numCells; c++ ){
    int k=c+startIndex;
    mxArray *a=0;
//     cerr << "k= " << k << " dims.size() " << dims.size() << std::endl;
    if( k < dims.size() && dims[k]>0 ){
      a=driver.CreateDoubleMatrix( 1,dims[k], complex_flag );
      memcpy((void *)driver.GetPr(a), (void *) &dval[j], dims[k]*sizeof(double) );
      j += dims[k];
    }
    else {
      a = driver.CreateDoubleMatrix( 1,0, complex_flag );
    }
    driver.SetCell(m_pm, c, a );
  }

  delete [] idims;
  delete [] dval;
}
//----------------------------------------------------------------------
// creates a numeric array
MatlabComplexArray::MatlabComplexArray ( const std::vector<int>&dims
				      , const std::vector<dComplex>&vals ){
  BUG_DEBUG("MatlabComplexArray::MatlabComplexArray" );
  mxComplexity complex_flag = mxCOMPLEX;
  mwSize ndim=dims.size();
  int nlen = vals.size();
  MatlabConnection& driver = MatlabConnection::Instance();
  BUG_DEBUG( "ndim=" << ndim << " nlen=" << nlen );
  if( ndim < 1 ){
    m_pm=driver.CreateDoubleMatrix( 1, 0, complex_flag );
    return;
  }
  if( ndim == 1 )
    ndim=2;

  mwSize *idims=new mwSize [ ndim ];
  double *reval=new double [ nlen ];
  double *imval=new double [ nlen ];
  std::copy( dims.begin(), dims.end(), idims );
  int n;
  for( n=0; n<nlen; ++n ){
    reval[n] = vals[n].real();
    imval[n] = vals[n].imag();
  }
  if( dims.size()==1 ){
    idims[0] = 1;  // we always make 2 dimensions if there is only 1
    idims[1] = dims[0];
  }

  m_pm = driver.CreateNumericArray(ndim, idims, mxDOUBLE_CLASS, complex_flag);

  int len=idims[0];
  for( int nn=1; nn<ndim; nn++ ){
    len *= idims[nn];
  }
  memcpy((void *)driver.GetPr(m_pm), (void *)reval, len*sizeof(double));
  if (driver.GetPi(m_pm))
    memcpy((void *)driver.GetPi(m_pm), (void *)imval, len*sizeof(double));
  BUG_DEBUG( "copied " << len << " values" );
  delete [] idims;
  delete [] reval;
  delete [] imval;
}
//----------------------------------------------------------------------
// creates a string array
MatlabStringArray::MatlabStringArray ( const std::vector<int>&dims
				       , const std::vector<std::string>&vals ){
  BUG_DEBUG("MatlabStringArray::MatlabStringArray( vector )" );
  MatlabConnection& driver = MatlabConnection::Instance();
  int ndim=dims.size();
  if( ndim<1 ){
    BUG_DEBUG( "empty matrix" );
    m_pm=driver.CreateCellMatrix( 1, 0 );
    return;
  }
  BUG_DEBUG( "Dimensions: " << ndim );
  if( ndim == 1)
    ndim=2;

  mwSize *idims=new mwSize [ ndim ];
  std::copy( dims.begin(), dims.end(), idims );
//   copy( dims.begin(), dims.end(), std::ostream_iterator<int>( cerr , " " ) );
//           cerr << " (CellArray) ";
//           for( int i=0; i<ndim; ++i )
//             cerr << i << " " << idims[i] << " ";
//           cerr << std::endl;
  if( dims.size() == 1 ){
    idims[0]=1;
    idims[1]=dims[0];
  }
  m_pm = driver.CreateCellArray( ndim, idims );
  for( int c=0; c<vals.size(); c++ ){
//      cerr << "'" << vals[c] << "' ";
    driver.SetCell(m_pm, c, driver.CreateString(vals[c].c_str()) );
  }
//    cerr << std::endl;
  delete[] idims;

  //   }
//    else {
//      int maxlength=0;
//      int l;
//      for( int i=0; i<vals.size(); ++i ){
//        l=vals[i].size();
//        if( l > maxlength )
// 	 maxlength=l;
//      }
//      int length=maxlength * vals.size();
//      char *buf = new char [ length + 1 ];
//      memset( buf, ' ', length );
//      buf[length]='\0';
//      int rows = length /dims[0];
  //      for( int i=0; i<vals.size(); ++i ){
  //        for( int k=0; k<vals[i].size(); ++k ){
  // 	 buf[k+i*maxlength]=vals[i][k];
  //        }
  //      }
  //      cerr << "'" << buf << "' " << " columns=" << dims[0] << " rows=" << rows << std::endl;
  //      m_pm=driver.CreateString( buf );
  //      driver.SetN( m_pm, dims[0] );
  //      driver.SetM( m_pm, rows );
  //      delete buf;
  //    }

  //   mxArray *pm = driver.CreateCharArray(ndim, dims);
  //   memcpy((void *)driver.GetData(pm), (void *)_val, sizeof(char)*strlen(_val));
  //   cerr << "putStringArray( " << getName() << "[" << _rows << "," << _cols << "] )" << std::endl;
}
//----------------------------------------------------------------------
// creates a string array
MatlabStringArray::MatlabStringArray ( const std::string& val ){
  BUG_DEBUG("MatlabStringArray::MatlabStringArray( scalar ) "<< "'" << val << "'" );
  m_pm = MatlabConnection::Instance().CreateString(val.c_str());
}
//   }
//----------------------------------------------------------------------
// creates a struct array
MatlabStructArray::MatlabStructArray ( StructStreamParameter &par
				       , const std::vector<DataStreamParameter *> fieldList
				       , const std::vector<int>&dims ): m_par(par), _nfields(0){
  BUG_DEBUG("MatlabStructArray::MatlabStructArray( StructStreamParameter & .. ) "
            << par.getName() );
  MatlabConnection& driver = MatlabConnection::Instance();
  mwSize ndim = dims.size();
  if( ndim < 1 ){
//        cerr << " empty matrix" << std::endl;
    m_pm=driver.CreateCellMatrix( 1, 0 );
    return;
  }

  // we need to count the non-hidden fields:
  for( int ii=0; ii<fieldList.size(); ++ii ){
    if( ! fieldList[ii] -> isHidden() )
      ++_nfields;
  }
  const char **fnames = (const char **)driver.Calloc( _nfields, sizeof( char * ) );
  for( int i = 0; i<_nfields; ++i ){
    fnames[i]=0;
//     cerr << i << std::endl;
  }
  for( int f = 0, n=0; f<fieldList.size(); ++f ){
    if( !fieldList[f]->isHidden() ){
      fnames[n] = strdup(fieldList[f]->getName().c_str());
      ++n;
//             cerr << f << ": " << fieldList[f]->getName() << std::endl;
    }
  }
  if( ndim == 1 ){
    BUG_DEBUG( " create Struct matrix array " << dims[0] );
    m_pm = driver.CreateStructMatrix( 1, dims[0], _nfields, fnames );
    if( m_pm == 0 ){
      BUG_DEBUG( "Cannot create array" );
      return;
    }
  }
  else {
    mwSize *idims=new mwSize [ ndim ];
    std::copy( dims.begin(), dims.end(), idims );
    m_pm = driver.CreateStructArray( ndim, idims, _nfields, fnames );

    delete[] idims;
  }
  driver.Free( fnames );
  par.setDimensionSize( ndim );

  putFields( fieldList, ndim, dims, 0, 0, 1 );

  //   cerr << "Checking classname of " << par.getName() << std::endl;
  if( !par.getClassname().empty() ){
    driver.SetClassName( m_pm, par.getClassname().c_str() );
    BUG_DEBUG( "obsolete driver.GetName( m_pm )" << " " << driver.GetClassName( m_pm )
	     << "(expected: " << par.getClassname() << ")");
  }
}
//----------------------------------------------------------------------
//
void MatlabStructArray::putFields ( const std::vector<DataStreamParameter *>&fieldList
				    , int ndims, const std::vector<int>&idims
				    , int ipos, int start, int step ){
  int i;
  int r;
  MatlabConnection& driver = MatlabConnection::Instance();
  BUG_DEBUG("MatlabStructArray::putFields" );
//    cerr << "MatlabStructArray::putField " << ipos << " ndims=" << ndims << std::endl;
//     std::copy( idims.begin(), idims.end(), std::ostream_iterator<int>( cerr, " " ) );
//     cerr << std::endl;
  if( ipos < ndims-1 ){
    int dims = idims[ipos];
    for( i = 0; i < dims; i++ ){
      m_par.setItemIndex( ipos, i );

      putFields( fieldList
		 , ndims   // Dimensions
                 , idims
		 , ipos + 1      // current Index in dim
		 , start + i * step    // Start-Index in re und im
		 , step * idims[ipos]    // Schrittweite
		  );
    }
  }
  else{
    int dims = idims[ ipos ];
    if( dims==0 ) dims=1;
    for( i = 0; i < dims; i++ ){
      int inx = start + i * step;
      m_par.setItemIndex( ipos, i );

      if( m_par.isValid() ){
	for( int f=0, fi=0; f<fieldList.size(); ++f ){
	  if( !fieldList[f] -> isHidden() ){
	    BUG_DEBUG( "Field " << fi << " name " << fieldList[f]->getName() );
	    fieldList[f]->putValues( this, true );
	    if(dynamic_cast<StructStreamParameter*>(fieldList[f])){
	      //  a CLASS object must be wrapped in a cell!!
	      if( !(dynamic_cast< StructStreamParameter * >( fieldList[f] ) -> getClassname()).empty() ){
//		std::cerr << "Wrapping " << fieldList[f]->getName() << std::endl;
		mxArray *mcell=driver.CreateCellMatrix( 1, 1 );
		//  		std::cerr << driver.GetClassName( m_tmpArray ) << std::endl;
		driver.SetCell( mcell, 0, m_tmpArray );
		m_tmpArray=mcell;
	      }
	    }
	    //	    std::cerr << "Setting field  for "<< m_pm << " inx=" << inx << " f="
	    // 		 << fi << " tmpArray="<< m_tmpArray << " nfields=" << _nfields << std::endl;
	    driver.SetFieldByNumber( m_pm, inx, fi, m_tmpArray );
	    ++fi;
	  }
	}
      }
//         else {
// 	  std::cerr << m_par.getFullName(true) << "  not valid: skipping " << inx <<std::endl;
//         }
      ++inx;
    }
  }
}
//----------------------------------------------------------------------
//
void MatlabStructArray::putValue ( StreamParameter &dpar, const std::string &val ){
  MatlabStringArray s( val );
  s.setParent( this );
  m_tmpArray=s.getArray();
}
//----------------------------------------------------------------------
//
void MatlabStructArray::putValues ( StreamParameter &dpar
				    , const std::vector<int>&dims
				    , const std::vector<double>&vals, bool isCell
				    , const std::vector<int> &inds ){
   MatlabArray *r=0;
   if(isCell)
     r=new MatlabCellArray( dims, vals );
   else
     r=new MatlabRealArray( dims, vals );

  r -> setParent( this );
  m_tmpArray=r -> getArray();
}
//----------------------------------------------------------------------
//
void MatlabStructArray::putValues ( StreamParameter &dpar, const std::vector<int>&dims
				    , const std::vector<dComplex>&vals
				    , const std::vector<int> &inds ){
  //  std::cerr << "MatlabStructArray::putValues( complex )" << std::endl;
  MatlabComplexArray r( dims,vals );
  r.setParent( this );
  m_tmpArray=r.getArray();
}
//----------------------------------------------------------------------
//
void MatlabStructArray::putValues ( StreamParameter &dpar, const std::vector<int>&dims
				, const std::vector<std::string>&vals ){
//   std::cerr << "MatlabStructArray::putValues( std::string )" << std::endl;
//   std::copy( dims.begin(), dims.end(), std::ostream_iterator<int>(cerr, " ") );
//   std::cerr << std::endl;
  MatlabStringArray s( dims,vals );
  s.setParent( this );
  m_tmpArray = s.getArray();
}
//----------------------------------------------------------------------
//
void MatlabStructArray::putValues ( DataStreamParameter &dpar
				    , const std::vector<DataStreamParameter *>&fieldList
				    , const std::vector<int>&dims ){
  //  std::cerr << "MatlabStructArray::putValues( fieldlist )" << std::endl;
  MatlabStructArray s( static_cast<StructStreamParameter &>(dpar), fieldList, dims );
  s.setParent( this );
  m_tmpArray = s.getArray();
}
//----------------------------------------------------------------------
//
void MatlabProcess::putValue ( StreamParameter &dpar, const std::string &val ){
  MatlabConnection& driver = MatlabConnection::Instance();
  if( driver.engine == 0 ){
    return;
  }
  MatlabStringArray s( val );
  //  std::cerr << "MatlabProcess::putValue " << dpar.getName() << " '" << val << "'" << std::endl;

  driver.PutVariable(driver.engine, dpar.getName().c_str(), s.getArray());
  m_mxArrayVec.push_back( s.getArray() );
}
//----------------------------------------------------------------------
//
void MatlabProcess::putValues ( StreamParameter &dpar, const std::vector<int>&dims
				, const std::vector<double>&vals, bool isCell
				, const std::vector<int> &inds ){

  MatlabConnection& driver = MatlabConnection::Instance();
  if( driver.engine == 0 ){
    return;
  }
  //  std::cerr << "MatlabProcess::putValues " << dpar.getName() << " size:" << vals.size() << std::endl;
   MatlabArray *r=0;
   if(isCell)
     r=new MatlabCellArray( dims,vals );
   else
     r=new MatlabRealArray( dims,vals );
   //  MatlabRealArray r(dims,vals);

  driver.PutVariable(driver.engine, dpar.getName().c_str(), r->getArray());
  m_mxArrayVec.push_back( r -> getArray() );
  //  std::cerr << "done" << std::endl;
}
//----------------------------------------------------------------------
//
void MatlabProcess::putValues ( StreamParameter &dpar, const std::vector<int>&dims
				, const std::vector<dComplex>&vals
				, const std::vector<int> &inds){
  BUG_DEBUG("MatlabProcess::putValues" );
  MatlabConnection& driver = MatlabConnection::Instance();
  if( driver.engine == 0 ){
    return;
  }
  MatlabComplexArray r( dims,vals );
  driver.PutVariable(driver.engine, dpar.getName().c_str(), r.getArray());
  m_mxArrayVec.push_back( r.getArray() );
}
//----------------------------------------------------------------------
//
void MatlabProcess::putValues ( StreamParameter &dpar, const std::vector<int>&dims
				, const std::vector<std::string>&vals ){
  MatlabConnection& driver = MatlabConnection::Instance();
  if( driver.engine == 0 ){
    return;
  }
  //  std::cerr << "MatlabProcess::putValues " << dpar.getName() << " dims " << dims.size() << std::endl;
  MatlabStringArray s( dims,vals );
  driver.PutVariable(driver.engine, dpar.getName().c_str(), s.getArray());
  m_mxArrayVec.push_back( s.getArray() );
}
//----------------------------------------------------------------------
//
void MatlabProcess::putValues ( DataStreamParameter &dpar
				, const std::vector<DataStreamParameter *>&fieldList
				, const std::vector<int>&dims ){
  //  std::cerr << "MatlabProcess::putValues " << dpar.getName() << " dims " << dims.size() << std::endl;

  MatlabConnection& driver = MatlabConnection::Instance();
  if( driver.engine == 0 ){
    return;
  }
  MatlabStructArray s( static_cast< StructStreamParameter &>(dpar)
		       , fieldList, dims );
  driver.PutVariable(driver.engine, dpar.getName().c_str(), s.getArray());
  m_mxArrayVec.push_back( s.getArray() );
}

void MatlabArray::getValues( DataStreamParameter &dpar ){
  std::vector<int> dims;
  MatlabConnection& driver = MatlabConnection::Instance();
  BUG_DEBUG("MatlabArray::getValues " << dpar.getName() );
  //  cerr << "MatlabArray::getValues " << dpar.getName() << std::endl;
  int numdims = driver.GetNumberOfDimensions( m_pm );
  BUG_DEBUG( " NumberOfDimensions = " << numdims );
  const mwSize *idims = driver.GetDimensions( m_pm );
  int       i;
//   cerr << "MatlabArray::getValues " << numdims <<" Dimensions\n";
  for( i=0; i<numdims; i++ ){
    BUG_DEBUG( i << " " << idims[i] );
    dims.push_back(idims[i]);
  }
//   std::copy( dims.begin(), dims.end(), std::ostream_iterator<int>( cerr, " " ) );
//   cerr << std::endl;
  if( driver.IsChar( m_pm ) ){
//     cerr << " is char " << std::endl;
    std::vector<std::string> svals;
    if( getString( m_pm, svals ) )
      dpar.setValue( std::vector<int>(1,1), svals );
    else {
      std::ostringstream ostr;
      ostr << "MatlabArray::getValues Error: '" << dpar.getName() << "' is not char" << std::endl;
      m_messages = ostr.str();
      m_failed = true;
    }
  }
  else {
    mxArray   *mtx=m_pm;

    if( driver.IsCell( m_pm ) ){
      BUG_DEBUG( "is cell" );
      //       cerr << "According to its header, array " << driver.GetName(m_pm) << " has "
      //	   << driver.GetNumberOfDimensions(m_pm) << " dimensions\n";

      if( dpar.DataItem()->getDataType() == DataDictionary::type_String ){
	std::vector<std::string> svals;
// 	       cerr << dpar.getName() << std::endl;
// 	        std::copy( svals.begin(), svals.end(), std::ostream_iterator<std::string>( cerr, " " ) );
// 	        cerr << std::endl;
	rearrange( svals, m_pm, numdims, idims, 0, 0, 1 );
	dpar.setValue( dims, svals );
	return;
      }
      if( dpar.DataItem()->getDataType() != DataDictionary::type_StructVariable ){
	std::vector<double> rdvals, idvals;
	rearrangeCell( mtx, rdvals, idvals, numdims, idims, 0,0,1 );
	// not yet implemented !!! dpar.setValue( dims, rdvals, idvals );
// 	cerr << "MatlabArray::getValues WARNING cannot transfer cell data! (Variable: "<< dpar.getName() <<")"<< std::endl;
	return;
      }
//       cerr << "What the hell is this\n";
    }

    if( driver.IsDouble( mtx ) ){
      BUG_DEBUG( "is numeric" );
      double *re = driver.GetPr( mtx );

      std::vector<double> rdvals, idvals;
// 	       cerr << dpar.getName() << std::endl;
// 	        std::copy( rdvals.begin(), rdvals.end(), ostream_iterator<double>( cerr, " " ) );
// 	        cerr << std::endl;
      rearrange( rdvals, re, numdims, idims, 0, 0, 1 );

      if( driver.IsComplex( mtx ) ){
	BUG_DEBUG("and complex");

	double *im = driver.GetPi( mtx );

	rearrange( idvals, im, numdims, idims, 0, 0, 1 );
      }
//       cerr << " MatlabArray::getValues ndims =" << dims.size() << std::endl;
      std::vector<int> inds(rdvals.size(),1);
      dpar.setValue( dims, rdvals, idvals, inds );
      return;
    }

    if( driver.IsStruct( mtx ) ) {
      std::ostringstream ostr;
      BUG_DEBUG( "is structure (help me!)" );
      ostr << "MatlabArray::getValues: Error: cannot assign struct variable '"
	   << dpar.getName() << std::endl;
      m_messages = ostr.str();
      m_failed = true;
    }
  }
}

void MatlabArray::getValues( DataStreamParameter &dpar
			  ,const std::vector<DataStreamParameter *> &fieldList ){
  BUG_DEBUG("Matlab::getValues " << dpar.getName() );
  int f, nfields, e, nelmnts;
  MatlabConnection& driver = MatlabConnection::Instance();
  nfields=driver.GetNumberOfFields( m_pm );
  nelmnts=driver.GetNumberOfElements( m_pm );
  int numdims = driver.GetNumberOfDimensions( m_pm );
  const mwSize *idims = driver.GetDimensions( m_pm );

  BUG_DEBUG( "Dimensions: " << numdims << " Elements: " << nelmnts << " Fields: " << nfields );

//   cerr << "Dimensions: " << numdims << " Elements: " << nelmnts << " Fields: " << nfields << std::endl;
  static_cast<StructStreamParameter &>(dpar).setDimensionSize( numdims );
  // iterate over all fields and elements
  std::vector<DataStreamParameter *>::const_iterator I;
  for( I=fieldList.begin(); I != fieldList.end(); ++I ){
    if( !(*I) -> isHidden() ){
      int f = driver.GetFieldNumber( m_pm, (*I)->getName().c_str() );
      if( f > -1 ){
// 	cerr << "Rearrange " << (*I)->getFullName(false) << std::endl;
	rearrange( dpar, *(*I), f, m_pm, numdims, idims, 0, 0, 1 );
      }
      else {
	std::ostringstream ostr;
	ostr << "Matlab::getValues: Error: missing field '"
	     << (*I)->getName()  << "' in struct " << dpar.getName() << std::endl;
	m_messages += ostr.str();
	m_failed = true;
      }
    }
  }
  dpar.eraseIndexes();
}


void MatlabProcess::getValues( DataStreamParameter &dpar ){
  mxArray   *mtx;
  MatlabConnection& driver = MatlabConnection::Instance();
  BUG_DEBUG("MatlabProcess::getValues" << dpar.getName() );
//   cerr << "MatlabProcess::getValues "<< dpar.getName() << std::endl;
  if( driver.engine == 0 ){
    return;
  }
  if( (mtx=driver.GetVariable( driver.engine, dpar.getName().c_str() )) != 0 ){
    MatlabArray m( mtx );
    m.getValues( dpar );
    if( m.hasFailed() )
      setErrorMessage( getErrorMessage() + m.getMessages() );
    m_mxArrayVec.push_back( mtx );
  }
  else {
    setErrorMessage( getErrorMessage() +
		     std::string("Matlab::getValues: Warning: missing values for ") +
		     dpar.getName() + "\n");
  }
}

bool MatlabArray::getString( mxArray *mtx
			     , std::vector<std::string> &svals )const{
  bool ok = true;
  MatlabConnection& driver = MatlabConnection::Instance();
  BUG_DEBUG("MatlabArray::getString" );
  if( mtx == 0 )
    return false;

  int rows = driver.GetN( mtx );
  int cols = driver.GetM( mtx );
  // Find out how long the input string array is.
  size_t buflen = (rows * cols) + 1;
  BUG_DEBUG( "rows = " << rows << " cols = " << cols );
  // Allocate enough memory to hold the converted string.
  char *buf = new char [ buflen ];
  std::ostringstream ostr;
  if( driver.IsChar( mtx ) ){
    int status = driver.GetString( mtx, buf, buflen );
    if( status != 0 ){
      BUG_DEBUG( "std::string truncated" );
    }
    for( int c=0; c<cols; c++ ){
      int inx;
      ostr.str("");
      for( int r=0; r<rows; r++ ){
	inx = c+r*cols;
	if( inx < buflen )
	  ostr << buf[inx];
      }
      BUG_DEBUG( ostr.str() );
      svals.push_back( ostr.str() );
    }
    if( cols <1 )
      svals.push_back( "" );

  }
  else {
    ok = false;
    if( driver.IsCell( mtx ) ){
      BUG_DEBUG( "matrix is cell" );
    }
    else {
      if( driver.IsDouble( mtx ) ){
	BUG_DEBUG( "matrix is numeric" );
      }
      else {
	if( driver.IsStruct( mtx ) ){
	  BUG_DEBUG( "matrix is struct" );
	}
      }
    }
    svals.push_back( "" );
  }
  delete [] buf;
  return ok;
}

void MatlabProcess::getValues( DataStreamParameter &dpar
			       , const std::vector<DataStreamParameter *> &fieldList ){
  mxArray   *mtx;
  BUG_DEBUG("MatlabProcess::getValues (fieldlist)" );
  BUG_DEBUG( dpar.getName() );
  MatlabConnection& driver = MatlabConnection::Instance();
  if( driver.engine == 0 ){
    return;
  }
//   cerr << "MatlabProcess::getValues"<< dpar.getName() << std::endl;
  if( (mtx=driver.GetVariable( driver.engine, dpar.getName().c_str() )) != 0 ){
    if( driver.IsStruct( mtx ) ) {
      MatlabArray m( mtx );
      m.getValues( dpar, fieldList );
      if( m.hasFailed() )
	setErrorMessage( getErrorMessage() + m.getMessages() );
    }
    else {
      setErrorMessage( getErrorMessage() + std::string("MatlabProcess: Warning: ") +
		       dpar.getName() + " is not struct\n" );
    }
    m_mxArrayVec.push_back( mtx );
  }
  else {
    setErrorMessage( getErrorMessage() +std::string("MatlabProcess: Warning: ") +
		     dpar.getName() + " not found\n" );
  }

}
//-----------------------------------------------------------------------
//
void MatlabArray::rearrange( std::vector<double> &dvec, const double *re
			       , int ndims, const mwSize *dims
			       , int idim, int start, int step ) const{
  int i;

  if( idim < ndims-1 ){
    for( i = 0; i < dims[idim]; i++ ){
      rearrange( dvec
		 , re          // Werte
		 , ndims, dims   // Dimensionen
		 , idim + 1      // aktueller Index in dim
		 , start + i * step    // Start-Index in re und im
		 , step * dims[idim]    // Schrittweite
		 );
    }
  }
  else{
    for( i = 0; i < dims[idim]; i++ ){
      int inx = start + i * step;
      if(re != 0){
	dvec.push_back(re[inx]);
      }
    }
  }
}
//-----------------------------------------------------------------------
//
void MatlabArray::rearrange( std::vector<std::string> &svec, mxArray *mtx
			       , int ndims, const mwSize *dims
			       , int idim, int start, int step ) const{
  int i;

  if( idim < ndims-1 ){
    for( i = 0; i < dims[idim]; i++ ){
      rearrange( svec
		 , mtx          // Werte
		 , ndims, dims   // Dimensionen
		 , idim + 1      // aktueller Index in dim
		 , start + i * step    // Start-Index in re und im
		 , step * dims[idim]    // Schrittweite
		 );
    }
  }
  else{
    for( i = 0; i < dims[idim]; i++ ){
      int inx = start + i * step;
      getString( MatlabConnection::Instance().GetCell( mtx, inx ), svec );
    }
  }
}
//-----------------------------------------------------------------------
//
void MatlabArray::rearrangeCell( mxArray *mtx, std::vector<double> revec, std::vector<double> imvec
				  , int ndims, const mwSize *dims
				  , int idim, int start, int step ) const{
  int i;
  BUG_DEBUG("MatlabArray::rearrangeCell" );
  if( idim < ndims-1 ){
    for( i = 0; i < dims[idim]; i++ ){
      rearrangeCell( mtx          // Werte
		     , revec, imvec
		     , ndims, dims   // Dimensionen
		     , idim + 1      // aktueller Index in dim
		     , start + i * step    // Start-Index in re und im
		     , step * dims[idim]    // Schrittweite
		     );
    }
  }
  else{
    for( i = 0; i < dims[idim]; i++ ){
      int inx = start + i * step;
      MatlabConnection& driver = MatlabConnection::Instance();
      mxArray *m=driver.GetCell( mtx, inx );
      int numdims = driver.GetNumberOfDimensions( m );
      BUG_DEBUG( " NumberOfDimensions = " << numdims );
      const mwSize *idims = driver.GetDimensions( m );
      std::vector<int> dimvec( idims, &idims[numdims] );
      if( driver.IsDouble( m ) ){
	rearrange( revec, driver.GetPr( m ), numdims, idims, 0, 0, 1 );
	if( driver.IsComplex( m ) ){
	  BUG_DEBUG("and complex");
	  rearrange( imvec, driver.GetPi( m ), numdims, idims, 0, 0, 1 );
	}
      }
    }
  }
}

void MatlabArray::rearrange( DataStreamParameter &dparent, DataStreamParameter &dfield
			     , int f, mxArray *mtx
			       , int ndims, const mwSize *dims
			       , int idim, int start, int step ){
  BUG_DEBUG("MatlabArray::rearrange" );
  if( idim < ndims-1 ){
    for( int i = 0; i < dims[idim]; i++ ){
      dparent.setIndex( idim, i );
      rearrange( dparent, dfield
		 , f, mtx
		 , ndims, dims   // Dimensionen
		 , idim + 1      // aktueller Index in dim
		 , start + i * step    // Start-Index in re und im
		 , step * dims[idim]    // Schrittweite
		 );
    }
  }
  else{
    for( int i = 0; i < dims[idim]; i++ ){
      int inx = start + i * step;
      mxArray *tmp;
      tmp = MatlabConnection::Instance().GetFieldByNumber( mtx, inx, f );
      BUG_DEBUG(  " field: " << f << " element: " << inx
		<< " is (" << tmp << ")" );
      if( tmp ){
	MatlabArray m( tmp );
	m.setParent(this);
	dparent.setIndex(idim,i);
	dfield.getValues( &m );
      }
    }
  }
}
