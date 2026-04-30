/*
 * SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#if !defined(__DP_DATA_TTRAIL_UNDO_LISTENER__)
#define __DP_DATA_TTRAIL_UNDO_LISTENER__

class DataTTrailUndoListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:

  /** Konstruktor
   */
  DataTTrailUndoListener() {}

  /** Destruktor
   */
  virtual ~DataTTrailUndoListener() {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:

  /** Beschreibung der Funktion undoStatus
      \param enabled
      \return
      \todo Die Beschreibung der Funktion undoStatus fehlt
   */
  virtual void undoStatus( bool enabled ) = 0;

  /** Beschreibung der Funktion redoStatus
      \param enabled
      \return
      \todo Die Beschreibung der Funktion redoStatus fehlt
   */
  virtual void redoStatus( bool enabled ) = 0;

};

#endif
/** \file */
