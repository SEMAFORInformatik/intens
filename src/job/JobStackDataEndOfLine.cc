
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
