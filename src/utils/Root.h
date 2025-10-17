//
// Root.h: interface for the Root class.
//

#if !defined(ROOT_H__INCLUDED_)
#define ROOT_H__INCLUDED_

//@{
class Root
{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  inline Root()            { NumAllocs++;  }
  inline virtual ~Root()   {NumDestroys++; }

protected:
  Root(const Root &ref);
  Root& operator=(const Root &ref);

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
public:
  /**
   * ShowCounters
   *    Diese Funktion zeigt die Anzahl Allocs und Destroys dieses Objekts.
   *    Sie ist mehr als Kuriosum fuer neugierige gedacht als dass sie einen
   *    praktischen Zweck hat.
   */
   void ShowCounters(void);

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/
protected:

/*******************************************************************************/
/* Member variables                                                            */
/*******************************************************************************/
protected:

  static long NumAllocs;            // Anzahl Konstruktor-Aufrufe
  static long NumDestroys;          // Anzahl Destruktor-Aufrufe
};
//@}

#endif // !defined(ROOT_H__INCLUDED_)
