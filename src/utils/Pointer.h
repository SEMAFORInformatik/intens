
#include <assert.h>

#if !defined(_INTENS_UTILITIES_POINTER_H_)
#define _INTENS_UTILITIES_POINTER_H_

template <class T> class ConstPointer;

///
template <class T>
class Pointer
{
  friend class ConstPointer<T>;

// ================================================================ //
// Constructor / Destructor                                         //
// ================================================================ //
public:
  ///
  Pointer(): m_data( 0 ){
  }
  ///
  Pointer( T *t ): m_data( t ){
    if( is_valid() ) m_data->_private_plus_ptr();
  }
  ///
  Pointer( const Pointer<T> &p ): m_data( p.get_ptr( true ) ){
    if( is_valid() ) m_data->_private_plus_ptr();
  }
  ///
  Pointer( const Pointer<T> &p, bool override ): m_data( p.get_ptr( override ) ){
    if( is_valid() ) m_data->_private_plus_ptr();
  }
  ///
  virtual ~Pointer(){
    clear();
  }

// ================================================================ //
// operator                                                         //
// ================================================================ //
public:
  /** der == operator liefert nur ein Resultat 'true', wenn
      beide <class T>-Objekte vorhanden sind. Es werden die
      <class T>-Objekte verglichen.
   */
  bool operator==( const Pointer<T> &p ) const{
    if( isnt_valid() || p.isnt_valid() ) return false;
    return *m_data == *p;
  }
  /** der == operator liefert nur ein Resultat 'true', wenn
      beide <class T>-Objekte vorhanden sind. Es werden die
      <class T>-Objekte verglichen.
   */
  bool operator==( const ConstPointer<T> &p ) const{
    if( isnt_valid() || p.isnt_valid() ) return false;
    return *m_data == *p;
  }
  /** der != operator liefert nur ein Resultat 'false', wenn
      beide <class T>-Objekte vorhanden sind. Es werden die
      <class T>-Objekte verglichen.
   */
  bool operator!=( const Pointer<T> &p ) const{
    if( isnt_valid() || p.isnt_valid() ) return true;
    return *m_data != *p;
  }
  /** der != operator liefert nur ein Resultat 'false', wenn
      beide <class T>-Objekte vorhanden sind. Es werden die
      <class T>-Objekte verglichen.
   */
  bool operator!=( const ConstPointer<T> &p ) const{
    if( isnt_valid() || p.isnt_valid() ) return true;
    return *m_data != *p;
  }
  ///
  Pointer<T>& operator=( const Pointer<T> &p ){
    if( is_valid() ) if( m_data->_private_minus_ptr() <= 0 ) delete m_data;
    m_data = p.m_data;
    if( is_valid() ) m_data->_private_plus_ptr();
    return *this;
  }
  ///
  Pointer<T>& operator=( T *t ){
    if( is_valid() ) if( m_data->_private_minus_ptr() <= 0 ) delete m_data;
    m_data = t;
    if( is_valid() ) m_data->_private_plus_ptr();
    return *this;
  }
  ///
  virtual T *operator->() const{
    assert( is_valid() );
    return m_data;
  }
  ///
  T * get_ptr( bool override=false ) const{
    if( isnt_valid() ){
      assert( override );
    }
    return m_data;
  }
  ///
  virtual T & get_ref() const{
    assert( is_valid() );
    return *m_data;
  }
  ///
  virtual T & operator*() const{
    return get_ref();
  }
  ///
  bool is_valid() const{
    return m_data != 0;
  }
  ///
  bool isnt_valid() const{
    return m_data == 0;
  }

  void exchange( Pointer<T> &p ){
    T *ptr = p.m_data;
    p.m_data = m_data;
    m_data = ptr;
  }

  void clear(){
    if( is_valid() ){
      if( m_data->_private_minus_ptr() <= 0 ) delete m_data;
      m_data = 0;
    }
  }

// ================================================================ //
// protected data                                                   //
// ================================================================ //
protected:
  T *m_data;
};



///
template <class T>
class ConstPointer
{
  friend class Pointer<T>;

// ================================================================ //
// Constructor / Destructor                                         //
// ================================================================ //
public:
  ///
  ConstPointer(): m_data( 0 ){
  }
  ///
  ConstPointer( const T *t ): m_data( t ){
    if( is_valid() ) const_cast<T *>(m_data)->_private_plus_ptr();
  }
  ///
  ConstPointer( const ConstPointer<T> &p ): m_data( p.get_ptr( true ) ){
    if( is_valid() ) const_cast<T *>(m_data)->_private_plus_ptr();
  }
  ///
  ConstPointer( const ConstPointer<T> &p, bool override ): m_data( p.get_ptr( override ) ){
    if( is_valid() ) const_cast<T *>(m_data)->_private_plus_ptr();
  }
  ///
  ConstPointer( const Pointer<T> &p ): m_data( p.get_ptr( true ) ){
    if( is_valid() ) const_cast<T *>(m_data)->_private_plus_ptr();
  }
  ///
  ConstPointer( const Pointer<T> &p, bool override ): m_data( p.get_ptr( override ) ){
    if( is_valid() ) const_cast<T *>(m_data)->_private_plus_ptr();
  }
  ///
  virtual ~ConstPointer(){
    clear();
  }

// ================================================================ //
// operator                                                         //
// ================================================================ //
public:
  /** der == operator liefert nur ein Resultat 'true', wenn
      beide <class T>-Objekte vorhanden sind. Es werden die
      <class T>-Objekte verglichen.
   */
  bool operator==( const ConstPointer<T> &p ) const{
    if( isnt_valid() || p.isnt_valid() ) return false;
    return *m_data == *p;
  }
  /** der == operator liefert nur ein Resultat 'true', wenn
      beide <class T>-Objekte vorhanden sind. Es werden die
      <class T>-Objekte verglichen.
   */
  bool operator==( const Pointer<T> &p ) const{
    if( isnt_valid() || p.isnt_valid() ) return false;
    return *m_data == *p;
  }
  /** der != operator liefert nur ein Resultat 'false', wenn
      beide <class T>-Objekte vorhanden sind. Es werden die
      <class T>-Objekte verglichen.
   */
  bool operator!=( const ConstPointer<T> &p ) const{
    if( isnt_valid() || p.isnt_valid() ) return true;
    return *m_data != *p;
  }
  /** der != operator liefert nur ein Resultat 'false', wenn
      beide <class T>-Objekte vorhanden sind. Es werden die
      <class T>-Objekte verglichen.
   */
  bool operator!=( const Pointer<T> &p ) const{
    if( isnt_valid() || p.isnt_valid() ) return true;
    return *m_data != *p;
  }
  ///
  ConstPointer<T>& operator=( const ConstPointer<T> &p ){
    if( is_valid() ){
      T *t = const_cast<T *>(m_data);
      if( t->_private_minus_ptr() <= 0 ) delete t;
    }
    m_data = p.get_ptr();
    if( is_valid() ) const_cast<T *>(m_data)->_private_plus_ptr();
    return *this;
  }
  ///
  ConstPointer<T>& operator=( const Pointer<T> &p ){
    if( is_valid() ){
      T *t = const_cast<T *>(m_data);
      if( t->_private_minus_ptr() <= 0 ) delete t;
    }
    m_data = p.m_data;
    if( is_valid() ) const_cast<T *>(m_data)->_private_plus_ptr();
    return *this;
  }
  ///
  ConstPointer<T>& operator=( T *d ){
    if( is_valid() ){
      T *t = const_cast<T *>(m_data);
      if( t->_private_minus_ptr() <= 0 ) delete t;
    }
    m_data = d;
    if( is_valid() ) const_cast<T *>(m_data)->_private_plus_ptr();
    return *this;
  }
  ///
  ConstPointer<T>& operator=( const T *d ){
    if( is_valid() ){
      T *t = const_cast<T *>(m_data);
      if( t->_private_minus_ptr() <= 0 ) delete t;
    }
    m_data = d;
    if( is_valid() ) const_cast<T *>(m_data)->_private_plus_ptr();
    return *this;
  }
  ///
  const T *operator->() const{
    assert( is_valid() );
    return m_data;
  }
  ///
  const T * get_ptr( bool override=false ) const{
    if( isnt_valid() ){
      assert( override );
    }
    return m_data;
  }
  ///
  const T & get_ref() const{
    assert( is_valid() );
    return *m_data;
  }
  ///
  const T & operator*() const{
    return get_ref();
  }
  ///
  bool is_valid() const{
    return m_data != 0;
  }
  ///
  bool isnt_valid() const{
    return m_data == 0;
  }

  void clear(){
    if( is_valid() ){
      T *t = const_cast<T *>(m_data);
      if( t->_private_minus_ptr() <= 0 ) delete t;
      m_data = 0;
    }
  }

// ================================================================ //
// protected data                                                   //
// ================================================================ //
protected:
  const T *m_data;
};

#endif
