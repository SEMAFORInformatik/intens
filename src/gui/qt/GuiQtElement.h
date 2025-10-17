
#if !defined(GUI_QT_ELEMENT_INCLUDED_H)
#define GUI_QT_ELEMENT_INCLUDED_H

#include <assert.h>

#include <qobject.h>
#include <qsizepolicy.h>
#include "gui/GuiElement.h"
#include "gui/FileSelectListener.h"
#include "gui/ConfirmationListener.h"
#include "job/JobAction.h"

class QWidget;
class QAction;
class QEnterEvent;
class QIODevice;
class GuiQtButtonbar;
class GuiQtForm;
class GuiQtDialog;
class GuiQtScrolledText;
class GuiQtIndex;


/** class for all qt gui objects

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiQtElement.h,v 1.22 2006/08/03 11:59:47 amg Exp $
 */

class GuiQtElement : public QObject, public GuiElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtElement( GuiElement *parent, const std::string &name="" );
  virtual ~GuiQtElement(){}

protected:
  GuiQtElement( const GuiQtElement &el );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Diese Funktion zerstört den Widget-Tree ab dem entsprechenden GuiElement und
      initialisiert das GuiElement neu.Achtung: Diese Funktion ist nicht bei allen Elementen
      implementiert.
  */
  virtual bool destroy();

  /** Mit der Funktion setAttachment() wird ein Objekt inner eines GuiContainers
      positioniert (XmForm-Widget). Der Container ruft diese Funktion auf. Er
      arbeitet mit FractionBase (siehe Motif XmForm).
      param x1 Attachment rechts ( <0 => rechter Rand)
      param x2 Attachment links ( <0 => linker Rand)
      param y1 Attachment oben ( <0 => oberer Rand)
      param y2 Attachment unten ( <0 => unterer Rand)
  */
  virtual void setAttachment( int x1, int x2, int y1, int y2 );

  virtual void setPosition( int x, int y );
  virtual void getPosition( int &x, int &y );

  /** Fragt nach der ExpandPolicy des QtElements.
   */
  virtual Qt::Orientations getExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Container.
   */
  virtual GuiElement::Orientation getContainerExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  virtual GuiElement::Orientation getDialogExpandPolicy();

  virtual void manage();
  virtual void unmanage();
  virtual void map();
  virtual void unmap();
  virtual bool isShown();
  virtual QWidget* myWidget() = 0;
  virtual QWidget* myCurrentWidget();
  virtual void updateWidgetProperty();

  virtual GuiDialog         *getDialog();
  virtual GuiQtDialog       *getQtDialog();
  virtual GuiQtElement      *getQtElement() { return this; }
  virtual GuiQtButtonbar    *getQtButtonbar() { return 0; }
  virtual GuiQtForm         *getQtForm() { return 0; }
  virtual GuiQtScrolledText *getQtScrolledText() { return 0; }
  virtual GuiQtIndex        *getQtIndex() { return 0; }

  void enterEvent ( QEnterEvent * );
  virtual void grabShortcutAction( std::string& accelerator_key, QAction * );
  virtual void grabFocus();
  virtual void getSize( int &, int &, bool hint=true);
  virtual void printSizeInfo(std::ostream& os, int intent, bool onlyMaxChilds=true);

  virtual void setStylesheet(const std::string& stylesheet);

  /// File save functions
  virtual JobAction *getSaveAction( XferDataItem *xfer );
  /// File save functions with file name
  virtual JobAction *getSaveAction( const std::string& filename );

  /// dump Widget to a graphic file (eps, png, ...)
  virtual void writeFile(QIODevice* ioDevice, const std::string &filename, bool bPrinter );

  /**
     Ist das Element streamable?
   */
  virtual BasicStream* streamableObject();

/*============================================================================*/
/* private Definitions for Save-Classes & Functions                           */
/*============================================================================*/

private:
  class FileJobAction : public JobAction
  {

  public:
    FileJobAction(GuiQtElement *e, bool open );
    virtual ~FileJobAction(){}
    virtual void startJobAction();
    virtual void stopJobAction(){ assert( false );}

    virtual void printLogTitle( std::ostream &ostr );
    void endFileStream( JobResult rslt );

    virtual void serializeXML(std::ostream &os, bool recursive = false);
    void setFilename( const std::string& filename );
    const std::string& getFilename() { return m_filename; }
	void setXfer( XferDataItem *xfer );
    XferDataItem * getXfer(){ return m_xfer; }
  protected:
    virtual void backFromJobController( JobResult rslt ){ assert( false ); }
  private:
    GuiQtElement *m_elem;
    XferDataItem *m_xfer;
	std::string   m_filename;
    bool m_open;
  };

  class SaveListener : public FileSelectListener, public ConfirmationListener
  {
  public:
    SaveListener( GuiQtElement *s ): m_stream( s ) {}
    virtual void FileSelected( const std::string &filename
			       , const HardCopyListener::FileFormat ff
			       , const std::string dir );
    virtual void canceled();
    virtual void confirmYesButtonPressed();
    virtual void confirmNoButtonPressed();
  private:
    std::string m_filename;
    GuiQtElement *m_stream;
  };

  /// defines the directory in the fileselection box.
  void setDirname( const std::string &dirname ) { m_dir = dirname; }

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual bool generateFileWithSvgGenerator(QIODevice* ioDevice, std::string &filename, bool bPrinter);
  bool processConvert( const std::string &inFilename , std::string& outFilename);
  void endFileStream( JobAction::JobResult rslt );
  void setDebugTooltip();
  /// File save functions
  void saveFile( FileJobAction *action = 0 );

public:
  Qt::AlignmentFlag getQtAlignment();
  Qt::AlignmentFlag getQtAlignment( Alignment align );
protected:
  GuiElement::Orientation getGuiOrientation(Qt::Orientations o);

  Qt::ScrollBarPolicy getScrollbarPolicy( GuiElement::ScrollbarType sb ) const;
  static const std::string getMenuName(GuiElement* parent, const std::string &name);

/*============================================================================*/
/* private data                                                               */
/*============================================================================*/
private:
   // FileJobAction varibles
   FileJobAction *m_action;
   SaveListener   m_save_listener;
   std::string    m_dir;
   std::string    m_filter;
};

#endif
