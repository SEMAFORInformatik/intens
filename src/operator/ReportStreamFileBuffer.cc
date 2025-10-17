
#include <sstream>
#include "operator/ReportStreamFileBuffer.h"

/* --------------------------------------------------------------------------- */
/* getDefaultSettings --                                                       */
/* --------------------------------------------------------------------------- */
bool ReportStreamFileBuffer::write( InputChannelEvent &event ) {
#if defined HAVE_QT
    std::ostringstream ostr;
#else
    std::ostream &ostr = event.getOstream();
#endif
    ostr << m_buffer.c_str();
#if defined HAVE_QT
    return event.write( ostr );
#else
    return true;
#endif
}
