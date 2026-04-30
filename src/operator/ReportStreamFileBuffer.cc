
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


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