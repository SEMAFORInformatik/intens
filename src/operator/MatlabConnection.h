
#ifndef MATLAB_CONNECTION_H
#define MATLAB_CONNECTION_H

//--------------------------------------------------------------------
// Utility class for Matlab interface
//
class MatlabConnection {
public:
  static MatlabConnection &Instance();
private:
  MatlabConnection():
    DestroyArray(0)
    ,OutputBuffer(0)
    ,EvalString(0)
    ,Open(0)
    ,CreateDoubleMatrix(0)
    ,DuplicateArray(0)
    ,CreateStructMatrix(0)
    ,CreateStructArray(0)
    ,CreateCellMatrix(0)
    ,CreateCellArray(0)
    ,CreateString(0)
    ,CreateNumericArray(0)
    ,GetPr(0)
    ,GetPi(0)
    ,SetCell(0)
    ,Close(0)
    ,Calloc(0)
    ,Free(0)
    ,SetClassName(0)
    ,GetClassName(0)
    ,SetFieldByNumber(0)
    ,GetFieldNumber(0)
    ,GetFieldByNumber(0)
    ,PutVariable(0)
    ,GetVariable(0)
    ,GetNumberOfDimensions(0)
    ,GetDimensions(0)
    ,IsChar(0)
    ,IsCell(0)
    ,IsDouble(0)
    ,IsComplex(0)
    ,IsStruct(0)
    ,GetNumberOfFields(0)
    ,GetNumberOfElements(0)
    ,GetN(0)
    ,GetM(0)
    ,GetString(0)
    ,GetCell(0)
    ,engine(0)
  {}

  // public methods
 public:
  Engine *engine;
  void      (* DestroyArray )( mxArray * );
  int       (* OutputBuffer )(Engine *,char *,int);
  int       (* EvalString )(Engine *, const char *);
  Engine *  (* Open )( const char * );
  mxArray * (* CreateDoubleMatrix )( mwSize, mwSize, mxComplexity );
  mxArray * (* DuplicateArray )(const mxArray *in);
  mxArray * (* CreateStructMatrix )( size_t, size_t, int, const char ** );
  mxArray * (* CreateStructArray )( mwSize, const mwSize *, int, const char ** );
  mxArray * (* CreateCellMatrix )( mwSize, mwSize );
  mxArray * (* CreateCellArray )( mwSize, const mwSize * );
  mxArray * (* CreateString )( const char * );
  mxArray * (* CreateNumericArray )( mwSize, const mwSize *, mxClassID, mxComplexity);
  double *  (* GetPr )( mxArray * );
  double *  (* GetPi )( mxArray * );
  void      (* SetCell )( mxArray *, mwIndex, mxArray *);
  int       (* Close )( Engine * );
  void *    (* Calloc )( size_t, size_t );
  void      (* Free )( void * );
  int       (* SetClassName )( mxArray *, const char *);
  const char * (* GetClassName )( mxArray * );
  void         (* SetFieldByNumber )( mxArray *, mwIndex, int, mxArray *);
  int          (* GetFieldNumber )(const mxArray *, const char *);
  mxArray *    (* GetFieldByNumber )(const mxArray *, mwIndex, int);
  int          (* PutVariable )( Engine *, const char *, const mxArray * );
  mxArray *    (* GetVariable )( Engine *, const char * );
  mwSize       (* GetNumberOfDimensions )( const mxArray * );
  const mwSize *  (* GetDimensions )( const mxArray * );
  bool         (* IsChar )( const mxArray * );
  bool         (* IsCell )( const mxArray * );
  bool         (* IsDouble )( const mxArray * );
  bool         (* IsComplex )( const mxArray *);
  bool         (* IsStruct )( const mxArray * );
  bool         (* IsClass )( const mxArray *, const char * );
  int          (* GetNumberOfFields )( const mxArray * );
  size_t       (* GetNumberOfElements )( const mxArray * );
  size_t       (* GetN )( const mxArray * );
  size_t       (* GetM )( const mxArray * );
  int          (* GetString )( const mxArray *,char *,mwSize );
  mxArray *    (* GetCell )( const mxArray *,mwIndex );
  char*        (* ArrayToString) (const mxArray *array_ptr);

 private:
  // Data
  static MatlabConnection  *s_instance;
};

#endif
