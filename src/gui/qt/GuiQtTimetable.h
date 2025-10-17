
#ifndef GUIQTTIMETABLE_H
#define GUIQTTIMETABLE_H

#include "gui/GuiTimeTable.h"
#include "gui/qt/GuiQtElement.h"
#include "gui/qt/timetable/qardatainterface.h"

class QStackedWidget;
class QMonthCalendar;
class QWeekCalendar;
class QDayCalendar;
class QCalendarBase;

class GuiQtTimeTable : public GuiQtElement, public GuiTimeTable  {
  Q_OBJECT
public:
  GuiQtTimeTable(GuiElement *parent, const std::string &name);
  virtual ~GuiQtTimeTable() {}

  virtual void setSize( int x, int y );
  virtual bool setFromStringDateTime( XferDataItem *dataitem );
  virtual bool setToStringDateTime( XferDataItem *dataitem );
  virtual bool setToStringTime( XferDataItem *dataitem );
  virtual bool setAppointment( XferDataItem *dataitem );


/*=============================================================================*/
/* public member functions of GuiQtElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_List; }
  virtual void create();
  virtual void manage() {}
  virtual bool destroy();
  virtual void update( UpdateReason );
  virtual QWidget* myWidget();

/*   virtual bool cloneable() { return true; } */
/*   virtual GuiElement *clone() { return new GuiQtTimeTable( *this ); } */
  virtual GuiElement *getElement() { return this; }
  virtual void serialize( std::ostream &os );

private slots:
  void onMonthViewClicked(unsigned int);
  void onAppointmentDoubleClicked(unsigned int);
  void toDayView(const QDateTime&);
  void toWeekView();
  void toMonthView();
  void toLastMainView();

 private:
  QStackedWidget *m_widgetStack;
  QMonthCalendar *m_calMonthWidget;
  QWeekCalendar  *m_calWeekWidget;
  QDayCalendar   *m_calDayWidget;
  QCalendarBase  *m_lastMainViewWidget;

  int m_width, m_height;
  XferDataItem* m_xferFromDate;
  XferDataItem* m_xferToDate;
  bool          m_bToTime;
  XferDataItem* m_xferString;
};



class IntensARInterface : public QARDataInterface
{
 public:
    IntensARInterface();
    virtual ~IntensARInterface();

    virtual unsigned int insertAppointment(QAppointment *newAppointment, bool autoDelete = false);
    virtual bool updateAppointment(QAppointment *changeAppointment, bool autoDelete = false);
    virtual bool removeAppointment(QAppointment *oldAppointment, bool autoDelete = false);
    virtual bool removeAppointment(unsigned int oldId);

    bool clearAllAppointments() {m_pAppList->clear();  }

    /**
     * Returns a list of appointments for the given time frame.
     *
     * Be sure to delete the list after using it!
     */
    QAppointmentPList* getAppointments(const QDateTime& startDate,
				       const QDateTime& endDate);

    /** Returns the appointment with the respective id */
    QAppointment* getAppointment(unsigned int appId);

    /** Returns the appointment with the respective id */
    QAppointment* createAppointment(std::string &title, QDateTime startDate, QDateTime endDate);

 private:
  /** List of appointments */
  QAppointmentPList* m_pAppList;
  static unsigned int s_id;
};

#endif
