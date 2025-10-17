
#if !defined(__DP_DATA_VALUE_VECTOR__)
#define __DP_DATA_VALUE_VECTOR__

#include <vector>
#include <iostream>

#include "datapool/DataValue.h"
#include "datapool/DataDebugger.h"

/** Das DataVector-Objekt ist eine Liste von DataValue-Objekten.
    Sie werden mit dem DataAlterGetVector-Funktor mit einer Wildcard erstellt.
    Mit dem DataAlterSetVector-Funktor und einer Wildcard wird ein Vector in
    den Datenbaum zurück geladen.
 */
class DataVector : public std::vector<DataValue::Ptr>
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataVector();

  /** Destruktor
   */
  virtual ~DataVector();

protected:
  /** Der Copy-Konstruktor ist nicht implementiert
      \param v
   */
  DataVector(const DataVector &v );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// Debug-Funktion
  BUG_DECL_SHOW_COUNT("DataVector");

  /** Ein DataValue-Objekt wird der Liste angehängt. Als Parameter wird
      eine Referenz auf das entsprechende Pointer-Objekt geliefert.
      @param val Pointer-Objekt eines DataValue-Objekts.
   */
  void appendValue( DataValue::Ptr &val );

  /** Die Funktion zeigt an, ob sich an der Position pos ein gültiges DataValue-Objekt
      befindet.
      @param pos zu untersuchende Position im Vektor
      @return true -> Ein gültiges DataValue-Objekt ist vorhanden
   */
  bool isValid( int pos ) const;

  /** Die Funktion liefert ein Pointer-Objekt mit dem entsprechenden DataValue-Objekt
      @param pos Position des gewünschten DataValue-Objekt im Vektor. Ist kein
             Objekt vorhanden, ist das Pointer-Objekt leer.
      @return Pointer-Objekt
   */
  DataValue::Ptr getValue( int pos ) const;

  /** Ein DataValue-Objekt wird in der Liste an der Position pos eingefügt. Als Parameter
      wird eine Referenz auf das entsprechende Pointer-Objekt geliefert.
      @param pos Position im Vektor
      @param val Pointer-Objekt eines DataValue-Objekts.
      @param resize true -> Der Vector wird bei Bedarf vergrössert.
   */
  void setValue( int pos, DataValue::Ptr &val, bool resize=false );

  /** Das DataValue-Objekt an der Position pos wird eliminiert. Das entsprechende
      Pointer-Objekt wird ungültig gemacht. Unter Umständen wird somit auch das
      DataValue-Objekt gelöscht.
      @param pos Position des zu löschenden DataValue-Objekts im Vektor.
   */
  void deleteValue( int pos );

  /** Der Vektor wird auf die gewünschte Grösse erweitert. Der Vektor wird jedoch nie kleiner.
      @param sz Neue Grösse (size) des Vektors.
   */
  void resizeVector( int sz );

  /** Die leeren DataValue-Objekte am Ende des Vektors werden eliminiert.
      @return logische Grösse des Vektors
   */
  int crunchVector();

  /** Der Vektor mit allen DataValue-Objekten wird gelöscht.
   */
  void clearVector();

  /** Im Vektor werden die leeren DataValue-Objekte eliminiert und die verbleibenden Objekte
      zusammen geschoben.
      @return true => Es wurden Element verschoben
   */
  bool packVector();

  /** Der Vektor wird zum Testen in den Outputstream ausgegeben.
      @param ostr Outputstream
   */
  void print( std::ostream &ostr ) const;

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

  /** Beschreibung der Funktion firstInvalidValue
      \param it
      \return
      \todo Die Beschreibung der Funktion firstInvalidValue fehlt
   */
  bool firstInvalidValue( iterator &it );

  /** Beschreibung der Funktion deleteInvalidValues
      \param it
      \return
      \todo Die Beschreibung der Funktion deleteInvalidValues fehlt
   */
  bool deleteInvalidValues( iterator &it );

  /** Beschreibung der Funktion moveValidValues
      \param itsrc
      \param itdest
      \return
      \todo Die Beschreibung der Funktion moveValidValues fehlt
   */
  void moveValidValues( iterator &itsrc, iterator &itdest );

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  BUG_DECLARE_COUNT;
};

#endif
/** \file */
