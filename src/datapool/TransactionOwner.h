
#if !defined(DATA_TRANSACTION_OWNER_INCLUDED)
#define DATA_TRANSACTION_OWNER_INCLUDED

#include <string>
#include <iostream>

class DataTTrail;

/** Das TransactionOwner-Objekt dient der Identifikation des 'Besitzers' einer
    Datapool-Transaction.
 */
class TransactionOwner
{
  friend class DataTTrail;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  TransactionOwner() : m_final_transaction( false ){}

  /** Destruktor
   */
  virtual ~TransactionOwner(){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Identifikation des Besitzers resp. Auslösers der Transaktion.
      @return Eindeutige ID des Auslösers der Transaction.
   */
  virtual std::string ownerId() const = 0;

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Mit dieser Funktion wird dem Besitzer der Transaction mitgeteilt, ob es
      sich um die äusserste Transaction handelt. Die Funktion wird nur vom
      DataTTrail-Objekt aufgerufen !
      @param final true -> äusserste Transaction
   */
  void setFinalTransaction( bool final ) { m_final_transaction = final; }

  /** Mit dieser Funktion wird dem DataTTrail-Objekt mitgeteilt, ob es sich um
      die äusserste Transaction handelt. Die Funktion wird nur vom
      DataTTrail-Objekt aufgerufen !
      @return true -> äusserste Transaction
   */
  bool isFinalTransaction() const { return m_final_transaction; }

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  bool    m_final_transaction;
};

/** Beschreibung der Funktion operator<<
    \param ostr
    \param owner
    \return
    \todo Die Beschreibung der Funktion operator<< fehlt
 */
std::ostream &operator << ( std::ostream &ostr, const TransactionOwner &owner );

#endif
/** \file */
