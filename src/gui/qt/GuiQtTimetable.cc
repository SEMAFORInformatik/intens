
#include <QStackedWidget>

#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/GuiQtTimetable.h"
#include "gui/qt/timetable/qmonthcalendar.h"
#include "gui/qt/timetable/qweekcalendar.h"
#include "gui/qt/timetable/qdaycalendar.h"
#include "gui/qt/timetable/qappointment.h"
#include "gui/qt/timetable/qapprepository.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferDataItemIndex.h"
#include "utils/gettext.h"
#include "app/UserAttr.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtTimeTable::GuiQtTimeTable(GuiElement *parent, const std::string &name)
  : GuiQtElement( parent, name )
  , m_widgetStack(0), m_calMonthWidget(0), m_calWeekWidget(0), m_calDayWidget(0)
  , m_lastMainViewWidget(0)
  , m_width(300), m_height(200)
  , m_xferFromDate(0), m_xferToDate(0), m_bToTime(true), m_xferString(0) {
}

/* --------------------------------------------------------------------------- */
/* setSize                                                                     */
/* --------------------------------------------------------------------------- */
void GuiQtTimeTable::setSize( int w, int h ) {
  if (w>300) m_width  = w;
  if (h>200) m_height = h;
}

/* --------------------------------------------------------------------------- */
/* setFromStringDateTime                                                       */
/* --------------------------------------------------------------------------- */
bool GuiQtTimeTable::setFromStringDateTime( XferDataItem *dataitem ) {
  m_xferFromDate  = dataitem;
  if (m_xferFromDate->getDataType() == DataDictionary::type_String &&
      m_xferFromDate->getUserAttr()->DateType() == UserAttr::date_kind_datetime)
    return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* setToStringDateTime                                                         */
/* --------------------------------------------------------------------------- */
bool GuiQtTimeTable::setToStringDateTime( XferDataItem *dataitem ) {
  if (m_xferToDate) return false;
  m_xferToDate  = dataitem;
  m_bToTime = false;
  if (m_xferToDate->getDataType() == DataDictionary::type_String &&
      m_xferToDate->getUserAttr()->DateType() == UserAttr::date_kind_datetime)
    return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* setToStringTime                                                             */
/* --------------------------------------------------------------------------- */
bool GuiQtTimeTable::setToStringTime( XferDataItem *dataitem ) {
  if (m_xferToDate) return false;
  m_xferToDate  = dataitem;
  m_bToTime = true;
  if (m_xferToDate->getDataType() == DataDictionary::type_String &&
      m_xferToDate->getUserAttr()->DateType() == UserAttr::date_kind_time)
    return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* setAppointment                                                              */
/* --------------------------------------------------------------------------- */
bool GuiQtTimeTable::setAppointment( XferDataItem *dataitem ) {
  m_xferString = dataitem;
  if (m_xferString->getDataType() == DataDictionary::type_String &&
      m_xferString->getUserAttr()->DateType() == UserAttr::date_kind_none)
    return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* create                                                                      */
/* --------------------------------------------------------------------------- */
void GuiQtTimeTable::create() {

#ifdef __USE_GUITIMETABLE__
  assert( getParent() );
  m_widgetStack = new QStackedWidget(getParent()->getQtElement()->myWidget());
  m_calMonthWidget = new QMonthCalendar( m_widgetStack );
  QObject::connect(m_calMonthWidget, SIGNAL(appointmentClicked(unsigned int)),
		   this, SLOT(onMonthViewClicked(unsigned int)));
  QObject::connect(m_calMonthWidget, SIGNAL(appointmentDoubleClicked(unsigned int)),
		   this, SLOT(onAppointmentDoubleClicked(unsigned int)));
  QObject::connect(m_calMonthWidget, SIGNAL(calendarDoubleClicked(const QDateTime&)),
		   this, SLOT(toDayView(const QDateTime&)));
  QObject::connect(m_calMonthWidget, SIGNAL(leftButtonClicked()),
		   this, SLOT(toWeekView()));

  m_calWeekWidget = new QWeekCalendar( m_widgetStack );
  QObject::connect(m_calWeekWidget, SIGNAL(calendarDoubleClicked(const QDateTime&)),
		   this, SLOT(toDayView(const QDateTime&)));
  QObject::connect(m_calWeekWidget, SIGNAL(appointmentDoubleClicked(unsigned int)),
		   this, SLOT(onAppointmentDoubleClicked(unsigned int)));
  QObject::connect(m_calWeekWidget, SIGNAL(leftButtonClicked()),
		   this, SLOT(toMonthView()));

  m_calDayWidget = new QDayCalendar( m_widgetStack );
  QObject::connect(m_calDayWidget, SIGNAL(appointmentDoubleClicked(unsigned int)),
		   this, SLOT(onAppointmentDoubleClicked(unsigned int)));
  QObject::connect(m_calDayWidget, SIGNAL(leftButtonClicked()),
		   this, SLOT(toLastMainView()));

  QFont font =  m_calMonthWidget->font();
  font.setPointSize( 8 );
  m_widgetStack->setFont( font );
  m_widgetStack->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  m_widgetStack->setMinimumSize(m_width, m_height);
#endif
}

/* --------------------------------------------------------------------------- */
/* destroy                                                                     */
/* --------------------------------------------------------------------------- */
bool GuiQtTimeTable::destroy() {
  m_calMonthWidget->deleteLater();
  m_calMonthWidget = 0;
  m_calWeekWidget->deleteLater();
  m_calWeekWidget = 0;
  m_calDayWidget->deleteLater();
  m_calDayWidget = 0;
}

/* --------------------------------------------------------------------------- */
/* update                                                                      */
/* --------------------------------------------------------------------------- */
void GuiQtTimeTable::update( UpdateReason ) {

#ifdef __USE_GUITIMETABLE__
  static IntensARInterface *arint=0;
  if (arint) {
    arint->clearAllAppointments();
  } else {
    arint = new IntensARInterface();
    QAppRepository::instance()->addBackend(arint);
  }
  int idx=0;
  std::string valueDate, valueToDate, valueString;
  m_xferFromDate->setDimensionIndizes();
  m_xferToDate->setDimensionIndizes();
  m_xferString->setDimensionIndizes();

  XferDataItemIndex *indexDate(NULL);
  XferDataItemIndex *indexString(NULL);
  // get last wildcard or create a index
  if (m_xferFromDate->getNumOfWildcards()) {
    indexDate = m_xferFromDate->getDataItemIndexWildcard( m_xferFromDate->getNumOfWildcards());
  } else {
    indexDate = m_xferFromDate->getDataItemIndex( "tt_indexDate" );
    if (!indexDate ) {
      indexDate = m_xferFromDate->newDataItemIndex();
      indexDate->setIndexName( "tt_indexDate", "" );
    }
  }
  if (m_xferToDate->getNumOfWildcards()) {
    indexDate = m_xferToDate->getDataItemIndexWildcard( m_xferToDate->getNumOfWildcards());
  } else {
    indexDate = m_xferToDate->getDataItemIndex( "tt_indexDate" );
    if (!indexDate ) {
      indexDate = m_xferToDate->newDataItemIndex();
      indexDate->setIndexName( "tt_indexDate", "" );
    }
  }
  if (m_xferString->getNumOfWildcards()) {
    indexString = m_xferString->getDataItemIndexWildcard( m_xferString->getNumOfWildcards());
  } else {
    indexString = m_xferString->getDataItemIndex( "tt_indexString" );
    if (!indexString ) {
      indexString = m_xferString->newDataItemIndex();
      indexString->setIndexName( "tt_indexString", "" );
    }
  }

  DataContainer::SizeList dimslist;
  m_xferFromDate->getAllDataDimensionSize( dimslist );
  for (int i=0; i<dimslist.back(); ++i) {
    m_xferFromDate->setIndex( indexDate, i );
    m_xferToDate->setIndex( indexDate, i );
    m_xferString->setIndex( indexString, i );
    if(m_xferFromDate->getValue(valueDate) && m_xferString->getValue(valueString) /*&&
										    m_xferToDate->getValue(valueToDate)*/  ) {
      QDateTime d = QDateTime::fromString( QString::fromStdString(valueDate), Qt::ISODate );
      QDateTime tod(d);
      if (m_xferToDate->getValue(valueToDate)) {
	if (m_bToTime) {
	  QTime tmp = QTime::fromString( QString::fromStdString(valueToDate), Qt::ISODate );
	  tod = tod.addSecs( tmp.hour()*3600+tmp.minute()*60+tmp.second() );
	} else
	  tod = QDateTime::fromString( QString::fromStdString(valueToDate), Qt::ISODate );
      } else
	tod = d.addSecs(1200);
      QAppointment*  pItem = arint->createAppointment( valueString, d, tod );
      QAppRepository::instance()->insertAppointment(pItem);
    }
  }
  m_widgetStack->repaint();
#endif
}

/* --------------------------------------------------------------------------- */
/* myWidget                                                                    */
/* --------------------------------------------------------------------------- */
QWidget* GuiQtTimeTable::myWidget() {
  return m_widgetStack;
}

/* --------------------------------------------------------------------------- */
/* serialize                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtTimeTable::serialize( std::ostream &os ) {
  os << "<sorry serialize of timetable not implemented yet/>";
}

/* --------------------------------------------------------------------------- */
/* onMonthViewClicked                                                          */
/* --------------------------------------------------------------------------- */
void GuiQtTimeTable::onMonthViewClicked(unsigned int i) {
#ifdef __USE_GUITIMETABLE__
  QAppointment *ap = QAppRepository::instance()->getAppointment(i);
  printMessage( compose(_("Appointment text: %1"), ap->m_szTitle.toStdString()), msg_Information, 10);
  delete ap;
#endif
}

/* --------------------------------------------------------------------------- */
/* onAppointmentDoubleClicked                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtTimeTable::onAppointmentDoubleClicked(unsigned int i) {
#ifdef __USE_GUITIMETABLE__
  QAppointment *ap = QAppRepository::instance()->getAppointment(i);
  std::string msg = std::string(ap->m_szTitle.toStdString()) + "\n\n" +
    compose(_("Start Date [%1]\n  End Date [%2]"),
	    ap->m_startDate.toString(Qt::LocalDate).toStdString(),
	    ap->m_endDate.toString(Qt::LocalDate).toStdString());
  GuiQtFactory::Instance()->showDialogInformation(this, _("Appointment Information"), msg, NULL);
  delete ap;
#endif
}

/* --------------------------------------------------------------------------- */
/* toDayView                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtTimeTable::toDayView(const QDateTime& d) {
#ifdef __USE_GUITIMETABLE__
  m_lastMainViewWidget = dynamic_cast<QCalendarBase*>(m_widgetStack->currentWidget());
  m_widgetStack->setCurrentWidget(m_calDayWidget);
  m_calDayWidget->setDate(d.date());
#endif
}

/* --------------------------------------------------------------------------- */
/* toWeekView                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtTimeTable::toWeekView() {
  m_widgetStack->setCurrentWidget(m_calWeekWidget);
  m_calWeekWidget->setDate( m_calMonthWidget->getDate() );
}

/* --------------------------------------------------------------------------- */
/* toMonthView                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtTimeTable::toMonthView() {
  m_widgetStack->setCurrentWidget(m_calMonthWidget);
  m_calMonthWidget->setDate( m_calWeekWidget->getDate() );
}

/* --------------------------------------------------------------------------- */
/* toLastMainView                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtTimeTable::toLastMainView() {
  if (m_lastMainViewWidget) {
    m_widgetStack->setCurrentWidget( m_lastMainViewWidget );
    m_lastMainViewWidget->setDate( m_calDayWidget->getDate() );
  }
}


/*************************************************************************
 *
 * IntensARInterface Implementation
 *
 *************************************************************************/

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
#ifdef __USE_GUITIMETABLE__
#include "timetable/qappointment.h"

IntensARInterface::IntensARInterface()
    : QARDataInterface()
{
  setReadOnly( false );
  m_pAppList = new QAppointmentPList();
//   m_pAppList->setAutoDelete(true);

  QDateTime curDate = QDateTime::currentDateTime();

  // MEIN TEST2 appointment
  std::string title("Oscar Hungeranfall");
  QAppointment*  pItem = createAppointment( title
					    , curDate.addSecs(60  * 60  * 1)
					    , curDate.addSecs(60  * 60  * 3)
					    );
  m_pAppList->append(pItem);
} // !IntensARInterface()

IntensARInterface::~IntensARInterface()
{
    if (m_pAppList)
    {
      QAppointmentPListIterator it = m_pAppList->begin();
      for(; it != m_pAppList->end();++it)
	  delete (*it);
      delete m_pAppList;
	m_pAppList = 0;
    }
} // !~IntensARInterface()

/* --------------------------------------------------------------------------- */
/* insertAppointment                                                           */
/* --------------------------------------------------------------------------- */
unsigned int IntensARInterface::insertAppointment(QAppointment* newAppointment, bool autoDelete)
{
  if (!newAppointment)
    return 0;
  m_pAppList->append(new QAppointment(*newAppointment));
  return newAppointment->m_nId;
} // !insertAppointment()

/* --------------------------------------------------------------------------- */
/* updateAppointment                                                           */
/* --------------------------------------------------------------------------- */
bool IntensARInterface::updateAppointment(QAppointment* changeAppointment, bool autoDelete)
{
    return true;
} // !updateAppointment()

/* --------------------------------------------------------------------------- */
/* removeAppointment                                                           */
/* --------------------------------------------------------------------------- */
bool IntensARInterface::removeAppointment(QAppointment* oldAppointment, bool autoDelete)
{
    return true;
} // !removeAppointment()

/* --------------------------------------------------------------------------- */
/* removeAppointment                                                           */
/* --------------------------------------------------------------------------- */
bool IntensARInterface::removeAppointment(unsigned int oldId)
{
  QAppointmentPListIterator it = m_pAppList->begin();
  while (it != m_pAppList->end())
    {
      QAppointment* pApp = (*it);
      if (pApp->m_nId == oldId)
	{
	  // remove object
	  delete (*it);
	  m_pAppList->erase( it );
	  return true;
	}
      it++;
    } // !while(it)
  return false;
} // !removeAppointment()

/* --------------------------------------------------------------------------- */
/* getAppointments                                                             */
/* --------------------------------------------------------------------------- */
QAppointmentPList* IntensARInterface::getAppointments(const QDateTime& startDate, const QDateTime& endDate)
{
  QAppointmentPList* retval    = new QAppointmentPList;
  QAppointmentPListIterator it = m_pAppList->begin();

//   retval->setAutoDelete(true);

  while (it != m_pAppList->end())
    {
     QAppointment* pApp = (*it);

     /*
      * There are various options for the event being in the
      * desired timeframe:
      * 1. start_event < start_timeframe && end_event > start_timeframe
      * 2. start_event > start_timeframe && start_event < end_timeframe
      */

     if ( ((pApp->m_startDate <= startDate) && (pApp->m_endDate >= startDate)) ||
	  ((pApp->m_startDate >= startDate) && (pApp->m_startDate <= endDate)) )
       {
	 // yes!
	   QAppointment* pNewApp = new QAppointment(*pApp); // deep copy because of auto-deletion
	   retval->append(pNewApp);
       }

     it++;
    } // !while(it)

  return retval;
} // !getAppointments()

/* --------------------------------------------------------------------------- */
/* getAppointment                                                              */
/* --------------------------------------------------------------------------- */
QAppointment* IntensARInterface::getAppointment(unsigned int appId)
{
    QAppointmentPListIterator it = m_pAppList->begin();
    while (it != m_pAppList->end())
    {
	QAppointment* pApp = (*it);
	if (pApp->m_nId == appId)
	{
	    // create return object
	    QAppointment* pRetval = new QAppointment(*pApp);
	    return pRetval;
	}
	it++;
    } // !while(it)

    return 0;
} // !getAppointment()

/*************************************************************************
 *
 * IntensAppointment Implementation
 *
 *************************************************************************/
unsigned int IntensARInterface::s_id = 0;

/* --------------------------------------------------------------------------- */
/* createAppointment                                                           */
/* --------------------------------------------------------------------------- */
QAppointment* IntensARInterface::createAppointment(std::string &title, QDateTime startDate, QDateTime endDate)
 {
   QAppointment *pItem              = new QAppointment();
   pItem->m_startDate = startDate;
   pItem->m_endDate = endDate;
   pItem->m_szTitle = QString::fromStdString(title);
   pItem->m_nId = ++s_id;
   pItem->setColor(QColor(180, 180, 250));
   return pItem;
}
#endif
