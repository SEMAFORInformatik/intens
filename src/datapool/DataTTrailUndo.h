
#if !defined(_DATA_TTRAIL_UNDO_MANAGER_INCLUDED_)
#define _DATA_TTRAIL_UNDO_MANAGER_INCLUDED_

#include <list>

#include "datapool/DataTTrailElementList.h"
#include "datapool/DataDebugger.h"

class DataTTrailElement;
class DataPool;
class DataTTrailUndoListener;

/** DataTTrailUndo
    @author Copyright (C) 2009  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataTTrailUndo
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:

  /** Konstruktor
   */
  DataTTrailUndo();

  /** Destruktor
   */
  virtual ~DataTTrailUndo();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion aktiviert das Feature. Ohne Aktivierung ist Undo/Redo in einer
      Applikation nicht verfügbar. Ein Aufruf von enable() bleibt wirkungslos.
   */
  void activate();

  /** Beschreibung der Funktion isActivated
      \return
      \todo Die Beschreibung der Funktion isActivated fehlt
   */
  bool isActivated() const;

  /** Beschreibung der Funktion isntActivated
      \return
      \todo Die Beschreibung der Funktion isntActivated fehlt
   */
  bool isntActivated() const;

  /** Es wird festgelegt, wieviele Elemente höchstens für ein undo() verfügbar
      sein müssen.
      @param anzahl maximale Anzahl von Einträgen für undo()
   */
  void setMaxElements( int anzahl );

  /** Mit dieser Funktion wird der Undo-Listener angehängt. Er wird bei jeder
      Änderung des Undo-Status mit der Funktion undoStatus() aktiviert.
      \param lsnr
   */
  void attachUndoListener( DataTTrailUndoListener *lsnr );

  /** Mit dieser Funktion wird der Redo-Listener angehängt. Er wird bei jeder
      Änderung des Undo-Status mit der Funktion redoStatus() aktiviert.
      \param lsnr
   */
  void attachRedoListener( DataTTrailUndoListener *lsnr );

  /** Das Undo-Feature wird in Kraft gesetzt. Nach einem temporären Unterbruch
      bleiben die Listeners stumm.
      @param temporary true => die Listeners bleiben stumm.
   */
  void enable( bool temporary=false );

  /** Das Undo-Feature wird ausser Kraft gesetzt. Die eventuell vorhandenen Daten
      werden gelöscht falls der Unterbruch nicht temporär (kurzfristig) ist.
      Bei einem temporären Unterbruch bleiben die Listeners stumm.
      @param temporary true => keine Initialisierung
   */
  void disable( bool temporary=false );

  /** Die Funktion zeigt an, ob das Undo-Feature aktiviert ist.
   */
  bool isEnabled() const;

  /** Die Funktion zeigt an, ob das Undo-Feature deaktiviert ist.
   */
  bool isDisabled() const;

  /** Alle undo- und redo-Daten werden gelöscht. Der Status des Objekts
      bleibt unverändert.
   */
  void clear();

  /** Das gewünschte Element wird für ein mögliches undo() festgehalten. Ein
      allfälliges redo() ist anschliessend nicht mehr möglich.
      @param el Pointer auf das einzufügende Element.
   */
  void insertElement( DataTTrailElement *el );

  /** Falls Daten vorhanden sind wird ein undo durchgeführt.
      @param datapool Referenz auf den DataPool
   */
  void undo( DataPool &datapool );

  /** Falls Daten vorhanden sind wird ein redo durchgeführt.
      @param datapool Referenz auf den DataPool
   */
  void redo( DataPool &datapool );

  /** Die Funktion zeigt an, ob Daten für ein undo() verfügbar sind.
   */
  bool hasUndoData() const;

  /** Die Funktion zeigt an, ob Daten für ein redo() verfügbar sind.
   */
  bool hasRedoData() const;

  /** Beschreibung der Funktion getLastUndoElement
      \return
      \todo Die Beschreibung der Funktion getLastUndoElement fehlt
   */
  DataTTrailElement *getLastUndoElement();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

  /** Beschreibung der Funktion sendUndoStatus
      \param enabled
      \return
      \todo Die Beschreibung der Funktion sendUndoStatus fehlt
   */
  void sendUndoStatus( bool enabled );

  /** Beschreibung der Funktion sendToUndoListener
      \param enabled
      \return
      \todo Die Beschreibung der Funktion sendToUndoListener fehlt
   */
  void sendToUndoListener( bool enabled );

  /** Beschreibung der Funktion sendRedoStatus
      \param enabled
      \return
      \todo Die Beschreibung der Funktion sendRedoStatus fehlt
   */
  void sendRedoStatus( bool enabled );

  /** Beschreibung der Funktion sendToRedoListener
      \param enabled
      \return
      \todo Die Beschreibung der Funktion sendToRedoListener fehlt
   */
  void sendToRedoListener( bool enabled );

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  bool                    m_is_activated;
  bool                    m_is_enabled;

  DataTTrailElementList   m_undo_list;
  DataTTrailElementList   m_redo_list;

  DataTTrailUndoListener *m_undo_lsnr;
  DataTTrailUndoListener *m_redo_lsnr;
};

#endif
/** \file */
