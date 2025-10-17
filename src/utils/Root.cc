// Root.cpp: implementation of the Root class.
//

#include "utils/Root.h"
#include <iostream>

long Root::NumAllocs   = 0;
long Root::NumDestroys = 0;

/*********************************************************************/
/* Constructor                                                       */
/*********************************************************************/

/*
Root::Root()
{

}
*/
/*********************************************************************/
/* Destructor                                                        */
/*********************************************************************/
/*
Root::~Root()
{

}
*/
/*********************************************************************/
/* ShowCounters                                                      */
/*********************************************************************/

void Root::ShowCounters(void)
{
  std::cout << "Number of Object Allocs:      " << NumAllocs << std::endl;
  std::cout << "Number of Object Destructions:  " << NumDestroys << std::endl;
}
