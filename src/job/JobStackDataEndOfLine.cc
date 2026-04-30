
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#include "job/JobIncludes.h"

#include "job/JobStackDataEndOfLine.h"

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

JobStackDataEndOfLine *JobStackDataEndOfLine::clone(){
  return new JobStackDataEndOfLine();
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void JobStackDataEndOfLine::print( std::ostream &ostr ){
  ostr << std::endl;
}