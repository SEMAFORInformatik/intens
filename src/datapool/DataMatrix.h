
#if !defined(__DP_DATA_VALUE_MATRIX__)
#define __DP_DATA_VALUE_MATRIX__

#include "datapool/DataDebugger.h"
#include "DataVector.h"

/** Das DataMatrix-Objekt ist eine Liste von DataVector-Objekten.
    Sie werden mit dem DataAlterGetMatrix-Funktor mit zwei Wildcards erstellt.
    Mit dem DataAlterSetMatrix-Funktor und zwei Wildcards wird eine Matrix in
    den Datenbaum zurück geladen.
 */
class DataMatrix : public std::vector<DataVector*>
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataMatrix();

  /** Destruktor
   */
  virtual ~DataMatrix();

protected:
  /** Copy-Konstruktor
      \param m
      \note Der Copy-Konstruktor steht nicht zur Verfügung
   */
  DataMatrix(const DataMatrix &m );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// Debug-Funktion
  BUG_DECL_SHOW_COUNT("DataMatrix");

  /** Ein neues DataVector-Objekt wird der Liste angehängt.
      @return Pointer auf den neu erstellten DataVector.
   */
  DataVector *appendVector();

  /** Die Liste mit allen DataVector-Objekten wird gelöscht.
   */
  void clearMatrix();

  /** Bei allen Kolonnen (Columns) werden die leeren Elemente eliminiert und
      die verbleibenden Elemente zusammen geschoben.
      @return true -> Es wurden Veränderungen vorgenommen.
   */
  bool packMatrixColumns();

  /** Bei allen Reihen (Rows) werden die leeren Elemente eliminiert und
      die verbleibenden Elemente zusammen geschoben.
      @return true -> Es wurden Veränderungen vorgenommen.
   */
   bool packMatrixRows();

  /** Die Matrix wird zum Testen in den Outputstream ausgegeben.
      @param ostr Outputstream
   */
  void print( std::ostream &ostr );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

  /** Beschreibung der Funktion packOneRow
      \param row
      \return
      \todo Die Beschreibung der Funktion packOneRow fehlt
   */
  bool packOneRow( int row );

  /** Beschreibung der Funktion firstInvalidValue
      \param it
      \param row
      \return
      \todo Die Beschreibung der Funktion firstInvalidValue fehlt
   */
  bool firstInvalidValue( iterator &it, int row );

  /** Beschreibung der Funktion deleteInvalidValues
      \param it
      \param row
      \return
      \todo Die Beschreibung der Funktion deleteInvalidValues fehlt
   */
  bool deleteInvalidValues( iterator &it, int row );

  /** Beschreibung der Funktion moveValidValues
      \param itsrc
      \param itdest
      \param row
      \return
      \todo Die Beschreibung der Funktion moveValidValues fehlt
   */
  void moveValidValues( iterator &itsrc, iterator &itdest, int row );

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  BUG_DECLARE_COUNT;
};
#endif
/** \file */
