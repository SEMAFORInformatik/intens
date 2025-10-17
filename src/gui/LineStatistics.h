#ifndef LINE_STATISTICS_H
#define LINE_STATISTICS_H

#include <math.h>
class DataProcessing;

class LineStatistics {
public:
  LineStatistics( DataProcessing *d );

  void operator()( double v );

  double getSize()const{return m_count;}
  double getSum() const { return m_sum; }
  double getSqrSum() const { return m_sqrsum; }
  double getStdDev() const { return m_count>1?
      sqrt( (m_sqrsum - m_sum*m_sum/m_count )/(m_count-1) ):0; }
  double getAvg()const{ return m_count>0?m_sum/m_count:0; }
  double getMin()const{ return m_min; }
  double getMax()const{ return m_max; }

private:
  DataProcessing *m_d;
  double m_min, m_max, m_sum, m_count, m_sqrsum;
};
#endif
