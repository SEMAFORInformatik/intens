
#include <algorithm>
#include <cmath>

#include "gui/GuiImage.h"
#include "gui/DataProcessing.h"

// ********************************
// Average
// ********************************
void Average::setImage( const MFMImageMapper::ImageData *img ){

  int n=img->getSize(); // rows
  if ( n <= 0 ) {
    std::cerr << "no lines" << std::endl;
    return;
  }

  if ( m_img != img ) { // reset data
    m_img=img;
    m_setLines = 0;
    m_max_line = 0;
    if ( m_img->begin()->first > 0 ) { // scanning down
      m_max_line = m_img->begin()->first + n-1;
    }
    m_avg.clear();
  }

  if ( n > m_setLines ) {
    // first row to calculate
    MFMImageMapper::ImageData::Image::const_iterator row=m_img->begin();

    if ( m_max_line == 0 ) { // scanning up
      // set row to first line to be calculated
      for ( int i=0; i < m_setLines; ++i ) {
	row++;
      }
    }

    // number of new Lines
    int new_lines = n - m_setLines;

    for( int k=0; k < new_lines; ++k, ++row ){
      // calculate average value
      const MFMImageMapper::ImageData::Line &v= (*row).second;
      double p_avg = std::for_each( v.begin(), v.end(), AverageOp() ).getAverage();
      m_avg.push_back(p_avg);
      ++m_setLines;
    }
  }
}

void Average::setLine(int l){
  //    assert( m_img );

  if ( m_max_line == 0 ) { // scanning up
    m_line = l;
  } else {
    m_line = m_max_line - l;
  }
}

// ********************************
// LineFit
// ********************************
void LineFit::setImage( const MFMImageMapper::ImageData *img ){

  int n=img->getSize(); // rows
  if ( n <= 0 ) {
    std::cerr << "no lines" << std::endl;
    return;
  }

  if ( m_img != img ) { // reset data
    m_img=img;
    m_setLines = 0;
    m_max_line = 0;
    if ( m_img->begin()->first > 0 ) { // scanning down
      m_max_line = m_img->begin()->first + n-1;
    }
    m_a.clear();
    m_c.clear();
  }

  if ( n > m_setLines ) {
    // first row to calculate
    MFMImageMapper::ImageData::Image::const_iterator row=m_img->begin();

    if ( m_max_line == 0 ) { // scanning up
      // set row to first line to be calculated
      for ( int i=0; i < m_setLines; ++i ) {
	row++;
      }
    }

    // number of new Lines
    int new_lines = n - m_setLines;

    for( int k=0; k < new_lines; ++k, ++row ){
      // calc Line
      double p_a(0);
      double p_c(0);
      const MFMImageMapper::ImageData::Line &v= (*row).second;
			std::for_each( v.begin(), v.end(), LineFitOp() ).getParameters(p_a, p_c);
      m_a.push_back(p_a);
      m_c.push_back(p_c);
      ++m_setLines;
    }
  }
}

void LineFit::setLine(int l){
  //    assert( m_img );

  if ( m_max_line == 0 ) { // scanning up
    m_line = l;
  } else {
    m_line = m_max_line - l;
  }
  m_x = 0;
}

// ********************************
// PlaneFit
// ********************************
void PlaneFit::setImage( const MFMImageMapper::ImageData *img ){

  int n=img->getSize(); // rows
  if ( n <= 0 ) {
    std::cerr << "no lines" << std::endl;
    return;
  }
  int m=img->begin()->second.size(); // columns

  if( m_img != img ) { // reset data
    m_img = img;
		m_n = 0;
    m_sumX = m_sumY = 0;
    m_sumXX = m_sumYY = 0;
		m_sumXY = 0;
    m_sumXZ = m_sumYZ = 0;
    m_sumZ = 0;
    m_setLines = 0;
    m_max_line = 0;
    if ( m_img->begin()->first > 0 ) { // scanning down
      m_max_line = m_img->begin()->first + n-1;
    }
  }

  if ( n > m_setLines ) {
    // first row to calculate
    MFMImageMapper::ImageData::Image::const_iterator row=m_img->begin();
    int y=m_setLines;

    int stepY=1;
    if ( m_max_line > 0 ) { // scanning down
      stepY = -1;
    } else { // scanning up
      // set row to first line to be calculated
      for ( int i=0; i < m_setLines; ++i ) {
				row++;
      }
    }

    // number of new Lines
    int new_lines = n - m_setLines;

    for( int k=0; k < new_lines; ++k, ++row, y+=stepY ){
			// calc Line
      MFMImageMapper::ImageData::Line::const_iterator column;
      int x=0;
      for( column=(*row).second.begin();
					 column!=(*row).second.end(); ++column, ++x){
				if ( ! std::isnan((*column)) ) {
					++m_n;
					m_sumX  += x;
					m_sumY  += y;
					m_sumXX += x*x;
					m_sumYY += y*y;
					m_sumXY += x*y;
					m_sumXZ += x*(*column);
					m_sumYZ += y*(*column);
					m_sumZ  += (*column);
				}
      }
      ++m_setLines;
    }

    // setParameters
    m_a=m_b=m_c=0;

		// | m_sumXX  m_sumXY  m_sumX |   | m_a |   | m_sumXZ |
    // |                          |   |     |   |         |
    // | m_sumXY  m_sumYY  m_sumY | x | m_b | = | m_sumYZ |
    // |                          |   |     |   |         |
    // | m_sumX   m_sumY   m_n    |   | m_c |   | m_sumZ  |
    //
    //               A              x    X    =      B
    //
		// -> A'A X = A' B -> X = A' B
    // -> find inverse of A (A')

		double a11 = m_sumYY * m_n     - m_sumY  * m_sumY ;
		double a12 = m_sumX  * m_sumY  - m_sumXY * m_n    ;
		double a13 = m_sumXY * m_sumY  - m_sumX  * m_sumYY;
		double a22 = m_sumXX * m_n     - m_sumX  * m_sumX ;
		double a23 = m_sumX  * m_sumXY - m_sumXX * m_sumY ;
		double a33 = m_sumXX * m_sumYY - m_sumXY * m_sumXY;

		double det
			= m_sumXX * a11
			+ m_sumXY * a12
      + m_sumX  * a13;

    if( fabs(det) > 2*std::numeric_limits<double>::epsilon() ){
			m_a = (a11 * m_sumXZ + a12 * m_sumYZ + a13 * m_sumZ)/det;
			m_b = (a12 * m_sumXZ + a22 * m_sumYZ + a23 * m_sumZ)/det;
			m_c = (a13 * m_sumXZ + a23 * m_sumYZ + a33 * m_sumZ)/det;
		}
    else { // this is a line fit:
			det = a22;
			if( fabs(det) > 2*std::numeric_limits<double>::epsilon() ){
				m_a = ( m_n    * m_sumXZ - m_sumX  * m_sumZ)/det;
				m_c = (-m_sumX * m_sumXZ + m_sumXX * m_sumZ)/det;
			}
		}
  }
}

void PlaneFit::setLine(int l){
  if( l<0 )
    return;

  if ( m_max_line == 0 ) { // scanning up
    m_line = l;
  } else { // scanning down
    m_line =  m_max_line - l;
  }
  m_x = 0;
}
