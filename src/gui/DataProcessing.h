/* $Id: DataProcessing.h,v 1.12 2007/05/02 12:38:59 bas Exp $
 * Simple Utility classes for Scanning Probe Data Analysis
 */
#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H
#include <numeric>
#include <string>
#include <cmath>
#include "utils/gettext.h"
//#include <cassert>

/** Abstract Base class for Data Analysis Processing
 */
class DataProcessing {
 public:
  /** Constructor
   * @param name name of data processing (should be unique)
   */
  DataProcessing(const char *name, int index):m_name(name), m_index(index){}

  /** get unique name of this object
   * @returns non-empty name
   */
  const std::string &getName(){return m_name;}

	// not used and strange!!
  // /** get index of this object
  //  * @returns index
  //  */
	// int &getIndex(){return m_index;}

  /** set image data and current line with recalculated processing
   * parameters
   * @param l line index within image
   * @param img image data of scan
   */
  virtual void setImage( const MFMImageMapper::ImageData *img ){};

  /** set current line. The processing parameters are recalculated
   * only if necessary
   * @param l line index within image
   */
  virtual void setLine( int l ){};

  /** pure virtual function operator for processed data
   * @returns processed data
   */
  virtual double operator()(double x)=0;

private:
  std::string m_name;
  int m_index;
};

/** class for doing no processing
 */
class Raw: public DataProcessing {
public:
  Raw(int i ):DataProcessing(_("Raw"),i){}
  virtual double operator()(double x){return x;}
};

/** utility class as a function operator for
 * Average
 */
class AverageOp {
public:
  AverageOp():m_n(0), m_sumZ(0){}
	double getAverage()const { return m_n>0 ? (m_sumZ/m_n) : 0.0; }
	void  operator()( double z ){
		if ( ! std::isnan(z) ) {
			++m_n;
			m_sumZ += z;
		}
	}
private:
  int m_n; // number of valid data (not NaN)
  double m_sumZ;
};

/** For each scan line the average value is subtracted from
 *  the measured data.
 */
class Average: public DataProcessing {
public:
  Average(int i ): DataProcessing(_("Average"),i )
                 , m_avg(0), m_img(0)
                 , m_max_line(0), m_line(0), m_setLines(0)
  {}
  virtual void setImage( const MFMImageMapper::ImageData *img );
  virtual void setLine(int l);

  virtual double operator()(double z){
    //    std::cerr << getName() << ": " << x << "-" << m_avg << std::endl;
    return (z-m_avg[m_line]);
  }
private:
  const MFMImageMapper::ImageData *m_img;
  std::vector<double> m_avg;
  int m_max_line;
  int m_line;
  int m_setLines;
};

/** utility class as a function operator for
 * LineFit
 */
class LineFitOp {
 public:
  LineFitOp():m_x(0), m_n(0), m_sumX(0),m_sumZ(0), m_sumXX(0),m_sumXZ(0){}
	bool getParameters(double &a, double &c)const {
		a=c=0;
		// | m_sumXX  m_sumX |   | m_a |   | m_sumXZ |
    // |                 |   |     |   |         |
    // | m_sumX   m_n    |   | m_c |   | m_sumZ  |
    //
    //           A         x    X    =      B
    //
		// -> A'A X = A' B -> X = A' B
    // -> find inverse of A (A')

		double det = m_sumXX * m_n - m_sumX * m_sumX;
    if( fabs(det) > 2*std::numeric_limits<double>::epsilon() ){
			a = ( m_n    * m_sumXZ - m_sumX  * m_sumZ)/det;
			c = (-m_sumX * m_sumXZ + m_sumXX * m_sumZ)/det;
			return true;
		} else { // not enough values, division by 0
			a = c = 0.0;
			return false;
		}
	}

	void  operator()( double z ){
		if ( ! std::isnan(z) ) {
			++m_n;
			m_sumX  += m_x;
			m_sumZ  +=   z;
			m_sumXX += m_x*m_x;
			m_sumXZ += m_x*  z;
		}
		m_x++;
	}
private:
  int m_x;
  int m_n;
  double m_sumX;
  double m_sumZ;
  double m_sumXX;
  double m_sumXZ;
};

/** Fits a straight line through the data points of the current
 *  line and calculates the distance to the measured point
 */
class LineFit: public DataProcessing {
public:
  LineFit(int i ):DataProcessing(_("Line Fit"),i )
                 , m_a(0), m_c(0), m_img(0)
                 , m_x(0)
                 , m_max_line(0), m_line(0), m_setLines(0)
  {}
  virtual void setImage( const MFMImageMapper::ImageData *img );
  virtual void setLine(int l);

  virtual double operator()(double z){
    return ( z-m_c[m_line] - m_a[m_line]*m_x++);
  }
private:
  const MFMImageMapper::ImageData *m_img;
  std::vector<double> m_a, m_c;
  int m_x;
  int m_max_line;
  int m_line;
  int m_setLines;
};

/** Fits a plane to the measured data and calculates the distance
 * of each data point to the plane.
 */
class PlaneFit: public DataProcessing {
public:
  PlaneFit(int i ):DataProcessing(_("Plane Fit"),i )
									, m_img(0)
									, m_a(0), m_b(0), m_c(0)
									, m_x(0), m_max_line(0), m_line(0)
									, m_n(0)
									, m_sumX(0), m_sumY(0)
									, m_sumXX(0), m_sumYY(0)
									, m_sumXY(0)
									, m_sumXZ(0), m_sumYZ(0)
									, m_sumZ(0)
			, m_setLines(0){
	}
  virtual void setImage( const MFMImageMapper::ImageData *img );
  virtual void setLine(int l);

  virtual double operator()(double z){
    return ( z - m_c - m_b*m_line - m_a*m_x++);
  }

private:
  const MFMImageMapper::ImageData *m_img;
  double m_a, m_b, m_c;
  int m_x;
  int m_max_line;
  int m_line;
	int m_n;
  double m_sumX;
  double m_sumY;
  double m_sumXX;
  double m_sumYY;
  double m_sumXY;
  double m_sumXZ;
  double m_sumYZ;
  double m_sumZ;
  int m_setLines;
};

#endif
