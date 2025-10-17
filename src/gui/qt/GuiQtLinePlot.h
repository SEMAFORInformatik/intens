
#if !defined(GUI_QT_LINEPLOT_H)
#define GUI_QT_LINEPLOT_H

#include <qwidget.h>

#include "gui/qt/GuiQtElement.h"
#include "gui/GuiMenuButtonListener.h"
#include "gui/GuiImage.h"
#include "gui/qt/GuiMFMSettings.h"

class GuiQtPopupMenu;
class QwtPlot;
class QwtPlotCurve;
class GuiMFMSettings;

class GuiQtLinePlot : public GuiQtElement , public GuiImage
 {
   Q_OBJECT
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
   GuiQtLinePlot( GuiElement *parent, const std::string &name );
  virtual ~GuiQtLinePlot();

 private:
   GuiQtLinePlot( const GuiQtLinePlot &i );
   GuiQtLinePlot &operator=( const GuiQtLinePlot &i );

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
   virtual GuiElement::ElementType Type() { return GuiElement::type_Image; }
   virtual void create();
   virtual void manage();
   virtual void unmanage();
   virtual bool destroy();
   virtual QWidget* myWidget();

   virtual GuiElement * getElement() { return this; }

   virtual void getSize( int &x, int &y );
   virtual void update( UpdateReason );
   /** change indicator */
   virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);
   virtual void serializeXML(std::ostream &os, bool recursive = false);

   void popupMenu(const QContextMenuEvent* event);
   void focusInEvent( QFocusEvent *event );
/*=============================================================================*/
/* public slots                                                                */
/*=============================================================================*/
   public slots:
     void selected (const QPolygon &pos);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
 public:
   virtual void setDialogImage(){
     GuiMFMSettings::getDialog().setImage( this, getTitle() );
   }
   virtual void rebuildDialogChannelBox(){
     GuiMFMSettings::getDialog().rebuildChannelBox();
   }
   virtual void updateDialogUnit(){
     GuiMFMSettings::getDialog().setUnits( getChannelUnit() );
   }
   virtual std::string getTitle();
   virtual void updateLabel();
   virtual void updateUnit();
   virtual void setFocus( bool );
   virtual void setRange( bool, double, double );
   virtual void clear( bool clearImages );
   virtual void setLine( int id, int line,
			const MFMImageMapper::ImageData::Line &data,
			const MFMImageMapper::ImageData *img );

   virtual void paint(){}
   virtual void showImage( const MFMImageMapper::ImageData *img, int id );
   virtual void redraw( bool clear = true );
/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
 private:
   void createLinePlotAndPainter();
   bool updateImageStatistics( const MFMImageMapper::ImageData *img, int direction );
   void createPopupMenu();
   void createSettingsDialog();

  class SettingsListener : public GuiMenuButtonListener
  {
  public:
    SettingsListener( GuiQtLinePlot *plot ): m_plot( plot ) {}
    virtual void ButtonPressed();
    JobAction* getAction(){ return 0; }
  private:
    GuiQtLinePlot  *m_plot;
  };

 private:
   QColor            m_defaultTitleBackgroundColor;
   QColor            m_defaultTitleForegroundColor;
   QwtPlot	     *m_plot;	    // the line plot
   QwtPlotCurve*      m_curve[2];      // the curve

   int                m_bottom;
   int                m_current_line;
   int   m_size;
   double *m_xArray[2];
   double *m_yArray[2];

   GuiQtPopupMenu* m_popupMenu;
   bool                       m_bPopup;

   SettingsListener              m_settingsListener;
};

#endif
