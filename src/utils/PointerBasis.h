
#if !defined(_INTENS_UTILITIES_POINTER_BASIS_H_)
#define _INTENS_UTILITIES_POINTER_BASIS_H_

#include "Pointer.h"

/** Die Pointer Basis-Klasse
 */
template <class T>
class PointerBasis
{
//  friend Pointer<T>;
//  friend ConstPointer<T>;

// ================================================================ //
// Constructor / Destructor                                         //
// ================================================================ //
public:
  PointerBasis() : m_anzahl_ptr( 0 ){}
  virtual ~PointerBasis(){}

// ================================================================ //
// public Functions only for Pointer                                //
// ================================================================ //
public:
  void _private_plus_ptr() { m_anzahl_ptr++; }
  int _private_minus_ptr() { return --m_anzahl_ptr; }
  int _private_anzahl_ptr() { return m_anzahl_ptr; }

// ================================================================ //
// private Data                                                     //
// ================================================================ //
private:
  int   m_anzahl_ptr;

};
#endif
