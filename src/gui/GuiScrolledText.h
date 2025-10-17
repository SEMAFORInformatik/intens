
#if !defined(GUI_SCROLLEDTEXT_INCLUDED_H)
#define GUI_SCROLLEDTEXT_INCLUDED_H

#include "gui/GuiMenuButtonListener.h"
class HardCopyListener;

class GuiScrolledText
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiScrolledText(){
  }
  virtual ~GuiScrolledText(){
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  enum Format
  { format_None = 0
  , format_Fortran
  };

  virtual GuiElement *getElement() = 0;

  virtual void setOptionLength( int len ) = 0;
  virtual int getOptionLength() = 0;
  virtual void setOptionLines( int lines ) = 0;
  virtual int getOptionLines() = 0;
  virtual bool setOptionLabel( const std::string &label ) = 0;
  virtual void setOptionWordwrap( bool wrap ) = 0;
  virtual void setOptionFormat( Format fmt ) = 0;
  virtual void setOptionMaxLines( int maxlines ) = 0;
  virtual void setOptionPrintFilter( const std::string &filter ) = 0;

  virtual GuiMenuButtonListener *getSaveListener() = 0;
  /** write message to window */
  virtual void writeText( const std::string & ) = 0;
  /** write message to window (threadsafe) */
  virtual void writeTextFromSubthread( const std::string & ) = 0;
  virtual void clearText() = 0;
};

#endif
