
#include <limits>
#include <cmath>

#include "gui/LineStatistics.h"
#include "gui/GuiMFMImageMapper.h"
#include "gui/DataProcessing.h"

LineStatistics::LineStatistics( DataProcessing *d ):
	m_d(d),
	m_min(std::numeric_limits<MFMImageMapper::ImageData::value_type>::max()),
	m_max(-std::numeric_limits<MFMImageMapper::ImageData::value_type>::max()),
	m_sum(0), m_count(0),m_sqrsum(0){ }

void LineStatistics::operator()( double v ){
	if ( !std::isnan(v) ) {
		double x = (*m_d)(v);
		m_sum += x;
		m_sqrsum += x*x;
		m_count++;
		if( x > m_max ){ m_max=x; }
		if( x < m_min ){ m_min=x; }
	}
}
