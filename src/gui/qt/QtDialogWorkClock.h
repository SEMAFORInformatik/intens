
#if !defined(QT_DIALOG_WORKCLOCK_H)
#define QT_DIALOG_WORKCLOCK_H

#include <qobject.h>
#include <string>

#include "gui/DialogWorkClock.h"
#include "utils/gettext.h"


class GuiElement;
class WorkerListener;
class QProgressDialog;
class QMessageBox;
class QtDialogWorkClock_myDialog;
class QtDialogWorkClock_QObject;

class QtDialogWorkClock : public DialogWorkClock
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  QtDialogWorkClock( DialogWorkClockListener *listener );
  virtual ~QtDialogWorkClock();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void setMessage( const std::string &msg );
  void slotCancelButtonPressed();
  static void forceUnmap();

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual void manage();
  virtual void unmanage();
  virtual void ClockTimeOver(){}

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  static QtDialogWorkClock_QObject  *s_object;
  static QtDialogWorkClock  *s_this;
public:
  static std::string  ObjectName;
};


class QtDialogWorkClock_QObject : public QObject
{
  Q_OBJECT

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  QtDialogWorkClock_QObject( QtDialogWorkClock *workclock );
  virtual ~QtDialogWorkClock_QObject();

/*=============================================================================*/
/* public member functions                                                  */
/*=============================================================================*/
public:
  void setWorkClock( QtDialogWorkClock *workclock );
  void setMessage( const std::string &msg ){ m_msg = msg; }
  void manage();
  void unmanage();

/*=============================================================================*/
/* myTimer                                                                     */
/*=============================================================================*/
protected:					// event handlers
  void timerEvent( QTimerEvent *e );
public slots:
  void slotCancelButtonPressed(bool);

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  int           m_timerID;
  std::string   m_msg;

  QtDialogWorkClock          *m_workclock;
  QtDialogWorkClock_myDialog *m_myDialog;
};

#endif
