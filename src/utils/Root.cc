// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

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
