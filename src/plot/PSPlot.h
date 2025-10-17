
#ifndef PSPLOT_H
#define PSPLOT_H

#include "app/HardCopyListener.h"
#include "streamer/BasicStream.h"

class PSPlot : public HardCopyListener{

  /*=============================================================================*/
  /* public definitions                                                          */
  /*=============================================================================*/
 public:
  class PSStream : public BasicStream{
    /*=============================================================================*/
    /* Constructor / Destructor                                                    */
    /*=============================================================================*/
  public:
    PSStream( const std::string &name )
      : BasicStream( name ){}
    ~PSStream(){}
  private:
    PSStream( const PSStream & );
    const PSStream &operator=( const PSStream & );

    /*=============================================================================*/
    /* public function of BasicStream                                              */
    /*=============================================================================*/
  public:
    virtual int getParameters( std::ostream &os ) const;
    virtual void getValues( StreamSource *src );

    /*=============================================================================*/
    /* public function of ChannelListener                                          */
    /*=============================================================================*/
  public:
    virtual void resetContinuousUpdate(){}
    virtual bool setContinuousUpdate( bool flag ){ return false; }
    virtual bool read( OutputChannelEvent & );
    virtual bool write( InputChannelEvent& );

    /*=============================================================================*/
    /* public function of IntensSocketListener                                     */
    /*=============================================================================*/
  public:
    virtual bool read( std::istream & ){ assert( false ); return false; }

    /*=============================================================================*/
    /* public memeber function                                                     */
    /*=============================================================================*/
  public:
    bool write( const std::string &fileName );
  };

  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
 public:
  PSPlot( PSStream *stream );
  virtual ~PSPlot();
 private:
  PSPlot( const PSPlot & );
  const PSPlot &operator=( const PSPlot & );

/*=============================================================================*/
/* public function of HardCopyListener                                         */
/*=============================================================================*/
public:
  virtual FileFormat getFileFormat();
  virtual const std::string &getName();
  virtual bool isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat );
  virtual const std::string &MenuLabel();
  virtual bool write( InputChannelEvent &event );
  virtual bool write( const std::string &fileName );
  virtual bool getDefaultSettings( HardCopyListener::PaperSize &size,
				   HardCopyListener::Orientation &orient,
				   int &quantity,
				   HardCopyListener::FileFormat &fileFormat,
				   double &lm, double &rm,
				   double &tm, double &bm );

/*=============================================================================*/
/* public members                                                              */
/*=============================================================================*/
public:
  BasicStream *getStream();
  void setTitle( const std::string &title );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
 private:
  std::string                   m_title;
  PSStream                     *m_stream;
  HardCopyListener::FileFormats m_supportedFileFormats;
};

#endif
