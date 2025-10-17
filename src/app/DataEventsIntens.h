
#if !defined(__DP_DATA_EVENTS_INTENS__)
#define __DP_DATA_EVENTS_INTENS__

#include <string>
#include "datapool/DataEvents.h"

/** Diese Klasse enthält Funktionen, welche vom DataPool unter gewissen Umständen
    aufgerufen werden, aber für seine Arbeiten nicht relevant sind. Die Implementation
    ist Sache der Kern-Applikation.
  */
class DataEventsIntens : public DataEvents
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

public:
  /// Konstruktor
  DataEventsIntens();
  /// Destruktor
  virtual ~DataEventsIntens();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion erhält als Parameter einen Text als Hinweis oder Warnung für einen Log-Eintrag.
      @param txt Log-Message
   */
  virtual void writeLogMessage( const std::string &txt );

  /** Bei langen Verarbeitungen wird hin und wieder diese Funktion aufgerufen, damit das
      das User-Interface die Möglichkeit hat, zwischendurch etwas anzuzeigen.
   */
  virtual void updateUserInterface();

  /** Bei langen Verarbeitungen wird hin und wieder diese Funktion aufgerufen, damit das
      das User-Interface die Möglichkeit hat, hängige Events abzuarbeiten.
   */
  virtual void dispatchPendingEvents();
};

#endif
