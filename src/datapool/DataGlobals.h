
/** \file
    Diese Headerdatei enthält die Definition der Klasse DataGlobals mit allen
    globalen Definitionen, welche überall im DataPool verwendet werden.
*/

#if !defined(__DP_DATA_DEFINES__)
#define __DP_DATA_DEFINES__

//#define __OMIT_DP_TIMESTAMPS__

#include <complex>

/// Datentyp für complexe Zahlen im Datapool
typedef std::complex<double>  dComplex;

/// Datentyp der Transaktionsnummer (Zeitstempel) einer Änderung (Transaction)
typedef long    TransactionNumber;

/// Datentyp der binären Attribute von Elementen
typedef unsigned long DATAAttributeMask;

/// Datentyp für die Attribute für die Funktionen isSerializable()
typedef unsigned long SerializableMask;

// Data Element Attributes
#define DATAisValid        1L
#define DATAisLocked       (1L<<1)
#define DATAisGuiUpdated   (1L<<2)
#define DATAisEqual        (1L<<3)
#define DATAisDifferent    (1L<<4)
#define DATAisLeftOnly     (1L<<4)
#define DATAisRightOnly    (1L<<4)

#define DATAelAttrNext     7

/** Diese Klasse beinhaltet alle Definitionen, welche überall im Datapool
    verwendet werden.
    @author Copyright (C) 2015 SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataGlobals
{
/*=============================================================================*/
/* Daten-Definitionen                                                          */
/*=============================================================================*/
public:
  /** \enum ContainerType
      Es wird zwischen verschiedenen Typen von Datencontainern unterschieden.
   */
  enum ContainerType
  { ContainerIsDimension /// Der Container enthält einen Vektor von Containern
  , ContainerIsElement   /// Der Container enthält Datenelemente mit Werten (siehe DataType)
  };

  /** \enum DataType
      Es gibt verschiedene Typen von Daten- und Dictionary-Elementen. Jedes DataElement- und
      jedes DataDictionary-Objekt ist von einem entsprechenden Typ.
   */
  enum DataType
  { type_None               /** Das Objekt ist noch nicht initialisiert oder bereits gelöscht */ = 0
  , type_Integer            /// Das Objekt ist vom Typ Integer (int)
  , type_Real               /// Das Objekt ist vom Typ Real (double)
  , type_String             /// Das Objekt ist vom Typ String (std::string)
  , type_Complex            /// Das Objekt ist vom Typ Complex (complex)
  , type_CharData           /// Das Objekt ist vom Typ Character (deimplementiert)
  , type_StructDefinition   /// Das Objekt ist vom Typ Struktur-Definition
  , type_StructVariable     /// Das Objekt ist vom Typ Struktur-Variable
  , type_Bad                /// abgestuerztes Element. Status unknown
  };


  /** \enum UpdateStatus
      Bei einem Zugriff mit DataFunctor-Objekten wird immer der Update-Status der Operation
      zurück gegeben.
  */
  enum UpdateStatus
  { NoUpdate
    /** Keine Veränderung an DataElement-Objekten
     */ = 0
  , DataUpdated
    /** Es wurden Daten oder Attribute mutiert
     */
  , ValueUpdated
    /** Es wurden Daten (Werte) mutiert
     */
  , GuiUpdated
    /** Es wurden Daten verändert, welche für das GUI relevant sind.
	Ein Gui-Update muss immer mit diesem Status arbeiten, da zusätzlich
	zu DataUpdated auch der Rollback-Timestamp berücksichtigt wird.
     */
  };

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:

  /** Konstruktor
   */
  DataGlobals();

  /** Destruktor
   */
  virtual ~DataGlobals();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:

/*=============================================================================*/
/* member variables                                                            */
/*=============================================================================*/
private:

};

#endif
/** \file */
