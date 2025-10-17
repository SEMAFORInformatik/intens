
#include <sstream>
#include <limits>

#include "utils/utils.h"
#include "streamer/StreamManager.h"
#include "streamer/Stream.h"
#include "app/DataPoolIntens.h"
#include "app/AppData.h"
#include "gui/GuiElement.h"
#include "gui/GuiManager.h"
#include "gui/GuiPlotDataItem.h"
#include "plot/Gdev.h"
#include "gui/GuiOrientationContainer.h"
#include "gui/GuiFieldgroup.h"
#include "gui/GuiFieldgroupLine.h"
#include "gui/GuiDataField.h"
#include "plot/ListPlot.h"
#include "gui/GuiFieldgroup.h"
#include "gui/GuiLabel.h"
#include "plot/ListGraph.h"
#include "gui/GuiFactory.h"
#include "utils/gettext.h"


const int MAXVALUES = 8;
const int CROSS = 1;
const int TRIANGLE = 5;
/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* Konstruktor --                                                              */
/* --------------------------------------------------------------------------- */

ListGraph::ListGraph( const std::string &name, ListPlot *plot, const std::string &id )
  : m_name( name )
  , m_id( id )
  , m_title( "" )
  , m_titleStream( 0 )
  , m_xaxis( 0 )
  , m_gdev( 0 )
  , m_plot( plot )
  , m_listGraphStruct( 0 )
  , m_drefname( 0 )
  , m_drefmin( 0 )
  , m_drefmax( 0 )
  , m_drefstate( 0 )

  , m_numberOfMarks( 5 )
  , m_sameyrange( false )

  , m_isOriginXDef( false )
  , m_originX( 0.0 )
  , m_ndivx( 10 )
  , m_nmaxdivx( 10 )
  , m_logx( false )

  , m_isOriginYDef( false )
  , m_originY( 0.0 )
  , m_ndivy( 10 )
  , m_nmaxdivy( 10 )
  , m_logy( false )
  , m_lessThanOriginY(false)

  , m_options( MAXVALUES )
  , m_orgmaxvalues( MAXVALUES )
  , m_outmaxvalues( MAXVALUES )
  , m_orgminvalues( MAXVALUES )
  , m_outminvalues( MAXVALUES )
  , m_itemnames( MAXVALUES )
  , m_optionsxaxis( Invalid )
  , m_orgmaxxaxis( 0 )
  , m_outmaxxaxis( 0 )
  , m_orgminxaxis( 0 )
  , m_outminxaxis( 0 ){
  assert( plot != 0 );
  assert( id.size() > 0 );

  createDataReference();

  for( int i=0; i < MAXVALUES; i++ ) {
    m_options[i] = Invalid;
    m_orgmaxvalues[i] = 0;
    m_orgminvalues[i] = 0;
    m_outmaxvalues[i] = 0;
    m_outminvalues[i] = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* Destruktor --                                                               */
/* --------------------------------------------------------------------------- */

ListGraph::~ListGraph() {
  if( m_drefname != 0 ) delete m_drefname;
  if( m_drefmin != 0 ) delete m_drefmin;
  if( m_drefmax != 0 ) delete m_drefmax;
  if( m_drefstate != 0 ) delete m_drefstate;
}

/* --------------------------------------------------------------------------- */
/* setStringTitle --                                                           */
/* --------------------------------------------------------------------------- */

bool ListGraph::setStringTitle( const std::string &title ) {
  m_title = title;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setStreamTitle --                                                           */
/* --------------------------------------------------------------------------- */

bool ListGraph::setStreamTitle( const std::string &titlestream ) {
  Stream *s = StreamManager::Instance().getStream( titlestream );
  if( s != 0 ) {
    m_titleStream = s;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getTitle --                                                                 */
/* --------------------------------------------------------------------------- */

std::string ListGraph::getTitle() {
  return m_titleStream != 0 ? StreamToString(m_titleStream):m_title;
}

/* --------------------------------------------------------------------------- */
/* addGraphItem --                                                             */
/* --------------------------------------------------------------------------- */

void ListGraph::addGraphItem( GuiPlotDataItem *dataitem ) {
  if( m_yaxes.size() <= 8 ) {
    m_yaxes.push_back( dataitem );
  }
}

/* --------------------------------------------------------------------------- */
/* setXaxis --                                                                 */
/* --------------------------------------------------------------------------- */

void ListGraph::setXaxis( GuiPlotDataItem *dataitem ) {
  assert( dataitem != 0 );
  assert( dataitem->XferData() != 0 );
  m_xaxis = dataitem;
}

/* --------------------------------------------------------------------------- */
/* setLogX --                                                                  */
/* --------------------------------------------------------------------------- */

void ListGraph::setLogX() {
  m_logx = true;
}

/* --------------------------------------------------------------------------- */
/* checkGraph --                                                                  */
/* --------------------------------------------------------------------------- */

bool ListGraph::checkGraph() {
  return !( ( m_xaxis == 0 ) && m_logx );
}

/* --------------------------------------------------------------------------- */
/* setLogY --                                                                  */
/* --------------------------------------------------------------------------- */

void ListGraph::setLogY() {
  m_logy = true;
}

/* --------------------------------------------------------------------------- */
/* setAxesOriginX --                                                           */
/* --------------------------------------------------------------------------- */

void ListGraph::setAxesOriginX( double orig ) {
  m_isOriginXDef = true;
  m_originX = orig;
}

/* --------------------------------------------------------------------------- */
/* setAxesOriginY --                                                           */
/* --------------------------------------------------------------------------- */

void ListGraph::setAxesOriginY( double orig ) {
  m_isOriginYDef = true;
  m_originY = orig;
}

/* --------------------------------------------------------------------------- */
/* setSameYRange --                                                            */
/* --------------------------------------------------------------------------- */

void ListGraph::setSameYRange() {
  m_sameyrange = true;
}

/* --------------------------------------------------------------------------- */
/* StreamToString --                                                           */
/* --------------------------------------------------------------------------- */

std::string ListGraph::StreamToString( Stream *s ) {
  if ( s != 0 ) {
    std::ostringstream ostr;
    s->write( ostr );
    return ostr.str();
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* nyaxis --                                                                   */
/* --------------------------------------------------------------------------- */

int ListGraph::nyaxis() {
  return m_yaxes.size();
}

/* --------------------------------------------------------------------------- */
/* drawGraph --                                                                */
/* --------------------------------------------------------------------------- */

void ListGraph::drawGraph( Gdev *gdev, const Frame &plotarea, int nhelmnts ) {
  BUG(BugPlot,"ListGraph::drawGraph");
  if( gdev == 0 ) return;
  m_gdev = gdev;
  m_lessThanOriginY=false;    // reset

  int n, i, j;
  int nyitems = m_yaxes.size();

  /*
    Vorgabe der Achsenunterteilungen, wird bei der logarithmischen Darstellung
    waehrend der Abarbeitung gesetzt.
  */

  if( m_logx ){
    m_ndivx = 1;
    BUG_MSG("===============> Log X");
  }
  else{
    m_ndivx = 10;
  }
  if( m_logy ){
    m_ndivy = 1;
    BUG_MSG("===============> Log Y");
  }
  else{
    m_ndivy = 10;
  }

  /*
     Unterteilen der Zeichnungsflaeche in einzelne Abschnitte, die nach folgedem
     Schema unterteilt sind:

              | titlearea
      ---------------------
              |
       yaxis- | grapharea
       areas  |
              |
      ---------------------
              | xaxisarea
  */
  Frame titlearea;
  Frame grapharea;
  Frame xaxisarea;
  int nframes;

  // Ist ein gemeinsame y-Achsenunterteilung gewaehlt so wird der y-Achsenbereich
  // in zwei Bereich unterteilt, der rechte fuer die Achsenbeschriftungen und der
  // linke fuer die Achsenlabels.
  if( (nyitems > 1) && (m_sameyrange || m_plot->isSameYRange()) ){
    nframes = 2;
  }
  else{
  }
  nframes = m_plot->maxYRange();
  std::vector<Frame> yaxisareas( nframes );

  setSubdivisions( plotarea, titlearea, grapharea,
		   xaxisarea, yaxisareas, nhelmnts );

  /*
     Maximum- und Minimumwerte der einzelnen y-Achsen ermitteln, die ermittelten
     Werte werden in die zwei Vektoren 'm_maxvalues' und 'm_minvalues' gespeichert.
  */
  int maxvalues = 0;
  std::list<GuiPlotDataItem*>::iterator iaxes;

  if( m_plot->printAllCycles() ) {
    DataPool &datapool = DataPoolIntens::getDataPool();
    int currentcyc = datapool.GetCurrentCycle();
    int numcyc = datapool.NumCycles();
    bool first = true;

    for( int cyc=0; cyc < numcyc; cyc++ ) {
      datapool.SetCycle( cyc, false ); // do not clear undo stack
      maxvalues = calculateMinMax( first );
      first = false;
    }
    datapool.SetCycle( currentcyc, false ); // do not clear undo stack
  }
  else {
    maxvalues = calculateMinMax( true );
  }

  // DEBUG: Augabe der Minimal- und Maximalwerte
//   cerr << "ListGraph: Ausgabe der minimal und maximal Werte" << endl;
//   for( int i=0; i < nyitems; i++ ) {
//     if( m_options[i] != ListGraph::Invalid )
//       cerr << "  y-Achse Nr.: " << i << " Maximum: " << m_orgmaxvalues[i]
// 	   << " Minimum: " << m_orgminvalues[i] << endl;
//     else
//       cerr << "  y-Achse Nr.: " << i << " enthaelt keine gueltigen Werte" << endl;
//   }

  /*
    Maximal- und Minimalwert der y-Achesenskalierungen zur Ausgabe formatieren.
    Grundsaezlich sind drei Varianten moeglich:
    1. Logarithmische Darstellung
    2. Automatische Ermittlung einer "Schoenen" Achsenunterteilung, diese
       Unterteilung wird auf zwei weitere Arten formatiert:
       2.1 Unabhaeniger Achsenursprung (optimale Darstellung der Werte)
       2.2 Fest durch Intens-Skript vogegebener Achsenursprung
    3. Manuelle Einstellungen der Achseneinteilungen mit dem Scale-Dialog
    Die manuelle Einteilung kann bei der logarithmische Darstellung nicht
    erfolgen.
  */
  std::vector<double> stepvalues( 8 );

  if( m_logy ) {
    for( i=0; i < nyitems; i++ ){
      stepvalues[i] = 10;
    }
    if( calculateNewRangeLog(nyitems) == false ) {
//       ostringstream ostr;
//       ostr << "ListGraph '" << m_name << "' contains incorrect data" << endl;
//       GuiManager::Instance().getLogWindow()->writeText( ostr.str() );
      return;
    }
  }
  else {
    for( i=0; i < nyitems; i++ ) {
      if( m_options[i] == Dynamic )
	if( m_isOriginYDef ) {
	  double min = (m_orgminvalues[i] < m_originY) ? m_orgminvalues[i] : m_originY;
	  double max = (m_orgmaxvalues[i] > m_originY) ? m_orgmaxvalues[i] : m_originY;
	  stepvalues[i] = calculateNewRange( m_ndivy, min,  max); //, m_originY, m_orgmaxvalues[i] );
//	  stepvalues[i] = calculateNewRange( m_ndivy, m_orgminvalues[i],  m_orgmaxvalues[i]); //, m_originY, m_orgmaxvalues[i] );
	  if (m_lessThanOriginY) {
	    m_outminvalues[i] = m_originY - m_ndivy*stepvalues[i]/2.0; //m_originY;
	    m_outmaxvalues[i] = m_originY + m_ndivy*stepvalues[i]/2.0; //m_originY;
	  } else {
	    m_outminvalues[i] = m_originY;
	    m_outmaxvalues[i] = m_originY + m_ndivy*stepvalues[i];
	  }
	}
	else {
	  stepvalues[i] = calculateNewRange( m_ndivy, m_orgminvalues[i], m_orgmaxvalues[i] );
	  double val = 0.0;
	  if( stepvalues[i] != 0.0 )
	    val = static_cast<double>(static_cast<int>(m_orgminvalues[i]/stepvalues[i])*stepvalues[i]);
	  if( val > m_orgminvalues[i] )
	    val -= stepvalues[i];
	  m_outminvalues[i] = val;
	  m_outmaxvalues[i] = m_ndivy*stepvalues[i] + val;
	}
      else if( m_options[i] == Static ) {
	stepvalues[i] = (m_outmaxvalues[i] - m_outminvalues[i]) / m_ndivy;
      }
    }
  }
  // DEBUG: Augabe der neu kalkulierten Minimal- und Maximalwerte
//   std::cerr << "ListGraph: Ausgabe der neu kalkulierten Minimal- und Maximalwerte" << std::endl;
//   for( int i=0; i < nyitems; i++ ) {
//     if( m_options[i] != ListGraph::Invalid )
//       std::cerr << "  y-Achse Nr.: " << i << " Maximum: " << m_outmaxvalues[i]
// 		<< " Minimum: " << m_outminvalues[i] << " StepValus: " << stepvalues[i] <<std::endl;
//     else
//       std::cerr << "  y-Achse Nr.: " << i << " enthaelt keine gueltigen Werte" << std::endl;
//    }

  /*
    Gemeinsamer Maximal- und Minimalwert aller y-Achsen ermitteln, wird benoetigt
    fuer die Ausgabe bei gemeinsamer y-Achsenunterteilung
  */
  int nymin = -1;
  int nymax = -1;
  double ymin = 0;
  double ymax = 0;
  bool novalidvalue = true;
  for( i=0; i < nyitems; i++ ) {
    if( m_options[i] != Invalid ) {
      if( (ymin > m_outminvalues[i]) || novalidvalue ) {
	nymin = i;
	ymin = m_outminvalues[i];
      }
      if( (ymax < m_outmaxvalues[i]) || novalidvalue ) {
	nymax = i;
	ymax = m_outmaxvalues[i];
      }
      novalidvalue = false;
    }
  }

  /*
    Maximal- und Minimalwert der x-Achesenskalierung ermittlen, bei dieser
    Gelegenheit werden gleich x-Werte formatiert und zwischengelagert.
    Grundsaezlich sind drei Varianten moeglich:
    1. Logarithmische Darstellung
    2. Automatische Ermittlung einer "Schoenen" Achsenunterteilung, diese
       Unterteilung wird auf zwei weitere Arten formatiert:
       2.1 Unabhaeniger Achsenursprung (optimale Darstellung der Werte)
       2.2 Fest durch Intens-Skript vogegebener Achsenursprung
    3. Manuelle Einstellungen der Achseneinteilungen mit dem Scale-Dialog
    Die manuelle Einteilung kann bei der logarithmische Darstellung nicht
    erfolgen.
  */
  std::vector<double> xvalues;
  double stepxvalue;

  if( m_xaxis != 0 ) {
    XferDataItemIndex *index = m_xaxis->getDataItemIndexWildcard( 1 );
    int dimsize = index->getDimensionSize( m_xaxis->Data() );
    bool first = true;
    for( i=0; i < dimsize; i++ ) {
      index->setIndex( m_xaxis->Data(), i );
      if( m_xaxis->isValid() ) {
	double value;
	if( m_xaxis->getValue( value ) ) {
	  if( m_logx )
	    xvalues.push_back( log10(value*m_xaxis->getScaleFactor()) );
	  else
	    xvalues.push_back( value*m_xaxis->getScaleFactor() );
	  if( first ) {
	    m_orgmaxxaxis = 0;
	    m_orgminxaxis = 0;
	  }
	  if( ((value*m_xaxis->getScaleFactor()) > m_orgmaxxaxis) || first )
	    m_orgmaxxaxis = value * m_xaxis->getScaleFactor();
	  if( ((value*m_xaxis->getScaleFactor()) < m_orgminxaxis) || first )
	    m_orgminxaxis = value * m_xaxis->getScaleFactor();
	  first = false;
	  if( m_optionsxaxis == Invalid )
	    m_optionsxaxis = Dynamic;
	}
      }
    }
  }
  else {
    stepxvalue = 1.0;
    m_outminxaxis = 0.0;
    m_outmaxxaxis = static_cast<double>(m_ndivx);
      for( i=0; i < maxvalues; i++ )
	xvalues.push_back( m_outmaxxaxis*i/(maxvalues == 1 ? 1 : maxvalues-1) );
  }

  /*
    Werte der x-Achseunterteilung aufbereiten und ausgeben.
  */
  std::deque<double> xaxisvalues;

  if( m_logx && m_xaxis != 0 ) {
    if( calculateNewRangeXLog() == false ) {
//       ostringstream ostr;
//       ostr << "ListGraph '" << m_name << "' contains incorrect data" << endl;
//       GuiManager::Instance().getLogWindow()->writeText( ostr.str() );
      return;
    }
    stepxvalue = 10.0;
    double ii = 1.0;
    for( i=m_ndivx; i >= 0; i--, ii*=10 ) {
      xaxisvalues.push_front( m_outmaxxaxis/ii );
    }
  }
  else {
    if( m_optionsxaxis == Dynamic ) {
      if( m_isOriginXDef ) {
	stepxvalue = calculateNewRange( m_ndivx, m_originX, m_orgmaxxaxis, false );
	m_outminxaxis = m_originX;
	m_outmaxxaxis = m_ndivx*stepxvalue + m_originX;
      }
      else {
	stepxvalue = calculateNewRange( m_ndivx, m_orgminxaxis, m_orgmaxxaxis, false );
	do {
	  double val = static_cast<double>(static_cast<int>(m_orgminxaxis/stepxvalue)*stepxvalue);
	  if( val > m_orgminxaxis )
	    val -= stepxvalue;
	  m_outminxaxis = val;
	  m_outmaxxaxis = m_ndivx*stepxvalue + val;
	  if (m_outmaxxaxis < m_orgmaxxaxis) {
	    stepxvalue = calculateNewRange( m_ndivx, m_outminxaxis, m_orgmaxxaxis, false );

	  }
	} while (m_outmaxxaxis < m_orgmaxxaxis);
      }
    }
    else {
      stepxvalue = (m_outmaxxaxis - m_outminxaxis) / m_ndivx;
// muss hier nicht sein, bleibt von der dynamischen Achsenskalierung unveraendert
//       xvalues.clear();
//       for( i=0; i < maxvalues; i++ )
// 	xvalues.push_back( (m_outmaxxaxis - m_outminxaxis)*i/(maxvalues == 1 ? 1 : maxvalues-1) );
    }
    for( i=0; i < m_ndivx+1; i++ ) {
      xaxisvalues.push_back( m_outminxaxis + i*stepxvalue );
    }
   }

  drawXAxis( xaxisarea, xaxisvalues, m_xaxis );

  /*
    y-Achsen zeichnen
  */
  double outminval = 0.0;
  double outmaxval = 0.0;

  if( (nyitems > 1) && (m_sameyrange || m_plot->isSameYRange()) ) {
    if( m_logy ) {
// unbenutzt
//        double tmp = 1.0;
//       for( i=0; tmp < ymax/ymin; i++ ){
// 	tmp *= 10;
//       }
      m_ndivy = i < m_nmaxdivy ? i : m_nmaxdivy;
    }
    std::vector<double> yaxisvalues( m_ndivy+1 );
    // Werte zur Ausgabe einer Achse aufbereiten
    if( m_logy ) {
          double ii = 1.0;
      for( i=m_ndivy; i >= 0; i--, ii*=10 ) {
	yaxisvalues[i] = ymax/ii;
      }
      outminval = yaxisvalues[0];
      outmaxval = yaxisvalues[m_ndivy];
    }
    else {
      double stepval = calculateNewRange( m_ndivy, ymin, ymax );
      double val = static_cast<double>(static_cast<int>(ymin/stepval)*stepval);
      if( val > ymin )
	val -= stepval;
      outminval = val;
      outmaxval = m_ndivy*stepval + val;
      for( i=0; i < m_ndivy+1; i++ ) {
	yaxisvalues[i] = outminval + i*stepval;
      }
    }
    // Achse zeichen
    drawYAxis( yaxisareas[1], yaxisvalues, 1 );
    // Label zeichnen
    for( n=0; n < m_yaxes.size(); n++ ) {
      GuiPlotDataItem *yitem = m_yaxes[n];
      Frame tmparea;
      double delta = (yaxisareas[0].y1 - yaxisareas[0].y0) / MAXVALUES;
      tmparea.x0 = yaxisareas[0].x0;
      tmparea.y0 = yaxisareas[0].y0 + delta * (MAXVALUES-1-n);
      tmparea.x1 = yaxisareas[0].x1;
      tmparea.y1 = yaxisareas[0].y0 + delta * (MAXVALUES-n);
      drawYAxisLabel( tmparea, yitem, (n+1) );
    }
  }
  else {
    for( n=0; n < m_yaxes.size(); n++ ) {
      GuiPlotDataItem *yitem = m_yaxes[n];
      std::vector<double> yaxisvalues( m_ndivy+1 );
      // Werte zur Ausgabe einer Achse aufbereiten
      if( m_logy ) {
	double ii = 1.0;
	for( i=m_ndivy; i >= 0; i--, ii*=10 ) {
	  yaxisvalues[i] = m_outmaxvalues[n]/ii;
	}
      }
      else {
	for( i=0; i < m_ndivy+1; i++ ) {
	  yaxisvalues[i] = m_outminvalues[n] + i*stepvalues[n];
	}
      }
      // Achse zeichen
      drawYAxis( yaxisareas[nframes-(n+1)], yaxisvalues, (n+1) );
      // Label zeichnen
      Frame tmparea;
      tmparea.x0 = yaxisareas[nframes-(n+1)].x0;
      tmparea.y0 = titlearea.y0;
      tmparea.x1 = yaxisareas[nframes-(n+1)].x1;
      tmparea.y1 = titlearea.y1;
      drawYAxisLabel( tmparea, yitem, (n+1) );
    }
  }
  /*
    Raster zeichnen
  */
  drawGrid( grapharea );

  /*
    y-Werte zeichnen
  */
  if( m_plot->printAllCycles() ) {
    DataPool &datapool = DataPoolIntens::getDataPool();
    int currentcyc = datapool.GetCurrentCycle();
    int numcyc = datapool.NumCycles();

    for( int cyc=0; cyc < numcyc; cyc++ ) {
      datapool.SetCycle( cyc, false ); // do not clear undo stack
      for( n=0; n < m_yaxes.size(); n++ ) {
	GuiPlotDataItem *yitem = m_yaxes[n];
	if( (nyitems > 1) && (m_sameyrange || m_plot->isSameYRange()) ) {
	  Frame windowsize( m_outminxaxis, outminval, m_outmaxxaxis, outmaxval );
	  drawValues( grapharea, windowsize, yitem, xvalues, (n+1) );
	}
	else {
	  Frame windowsize( m_outminxaxis, m_outminvalues[n],
			    m_outmaxxaxis, m_outmaxvalues[n] );
	  drawValues( grapharea, windowsize, yitem, xvalues, (n+1) );
	}
      }
    }
    datapool.SetCycle( currentcyc, false ); // do not clear undo stack
  }
  else {
    for( n=0; n < m_yaxes.size(); n++ ) {
      GuiPlotDataItem *yitem = m_yaxes[n];
      if( (nyitems > 1) && (m_sameyrange || m_plot->isSameYRange()) ) {
	Frame windowsize( m_outminxaxis, outminval, m_outmaxxaxis, outmaxval );
	drawValues( grapharea, windowsize, yitem, xvalues, (n+1) );
      }
      else {
	Frame windowsize( m_outminxaxis, m_outminvalues[n], m_outmaxxaxis, m_outmaxvalues[n] );
	drawValues( grapharea, windowsize, yitem, xvalues, (n+1) );
      }
    }
  }

  /*
    Titel ausgeben
  */
  drawTitle( titlearea );
 }

/* --------------------------------------------------------------------------- */
/* setSubdivisions --                                                          */
/* --------------------------------------------------------------------------- */

void ListGraph::setSubdivisions( const Frame &plotarea,
				 Frame &titlearea,
				 Frame &grapharea,
				 Frame &xaxisarea,
				 std::vector<Frame> &yaxisareas,
				 int nhelmnts ) {
  double deltax = plotarea.x1 - plotarea.x0;
  double deltay = plotarea.y1 - plotarea.y0;

  // Skalierungsfaktoren in Prozent (x/100)
  double factor_yaxis = nhelmnts > 1 ? 0.09 : 0.045;
  double factor_legend = 0.0;
  double factor_title = 0.1;
  double factor_xaxis = 0.1;

  /* Hilfsachsen definieren
          |          |
     v2---------------
          |          |
          |          |
          |          |
     v1---------------
          |          |
          h1         h2
   */
  double h1 = factor_yaxis * deltax * yaxisareas.size() + plotarea.x0;
  double h2 = plotarea.x1 - factor_legend * deltax;
  double v1 = factor_xaxis * deltay + plotarea.y0;
  double v2 = plotarea.y1 - factor_title * deltay;

  // Unterteilgungen setzen
  titlearea.x0 = h1;
  titlearea.y0 = v2;
  titlearea.x1 = h2;
  titlearea.y1 = plotarea.y1;
  grapharea.x0 = h1;
  grapharea.y0 = v1;
  grapharea.x1 = h2;
  grapharea.y1 = v2;
  xaxisarea.x0 = h1;
  xaxisarea.y0 = plotarea.y0;
  xaxisarea.x1 = h2;
  xaxisarea.y1 = v1;
  double f = (h1 - plotarea.x0) / yaxisareas.size();
  for( int i=0; i < yaxisareas.size(); i++ ) {
    yaxisareas[i].x0 = i * f + plotarea.x0;
    yaxisareas[i].y0 = v1;
    yaxisareas[i].x1 = (i+1) * f + plotarea.x0;
    yaxisareas[i].y1 = v2;
  }
}

/* --------------------------------------------------------------------------- */
/* drawGrid --                                                                 */
/* --------------------------------------------------------------------------- */

void ListGraph::drawGrid( const Frame &viewport ) {
  m_gdev->setViewport( viewport.x0, viewport.y0, viewport.x1, viewport.y1 );
  m_gdev->setLinestyle( 1 );
  m_gdev->setLinewidth( 0 );

  if( m_logx ) {
    double offset = (viewport.x1 - viewport.x0) / m_ndivx;
    for( int i=0; i < m_ndivx; i++ ) {
      m_gdev->setLinestyle( 0 );
      m_gdev->setViewport( viewport.x0+i*offset,
			   viewport.y0,
			   viewport.x0+(i+1)*offset,
			   viewport.y1 );
      m_gdev->setWindow( 0,0,log10(10.),1 );
      m_gdev->moveTo( log10(10.), 0 );
      m_gdev->drawTo( log10(10.), 1 );
      m_gdev->setLinestyle( 1 );
      int d = m_plot->getNSubDivX() == 2 ? 10 : 4;
      double f = 10.0/d;
      for( int j=1; j < d; j++ ) {
	m_gdev->moveTo( log10((static_cast<double>(j))*f), 0 );
	m_gdev->drawTo( log10((static_cast<double>(j))*f), 1 );
      }
    }
  }
  else {
    m_gdev->setViewport( viewport.x0, viewport.y0, viewport.x1, viewport.y1 );
    m_gdev->setWindow( 0,0,1,1 );
    double f = 1.0 / (m_ndivx * m_plot->getNSubDivX());
    for( int i=0; i < (m_ndivx * m_plot->getNSubDivX()); i++ ) {
      m_gdev->moveTo( (i+1)*f, 0 );
      m_gdev->drawTo( (i+1)*f, 1 );
    }
  }

  if( m_logy ) {
    double offset = (viewport.y1 - viewport.y0) / m_ndivy;
    for( int i=0; i < m_ndivy; i++ ) {
      m_gdev->setLinestyle( 0 );
      m_gdev->setViewport( viewport.x0,
			   viewport.y0+i*offset,
			   viewport.x1,
			   viewport.y0+(i+1)*offset );
      m_gdev->setWindow( 0,0,1,log10(10.) );
      m_gdev->moveTo( 0, log10(10.) );
      m_gdev->drawTo( 1, log10(10.) );
      m_gdev->setLinestyle( 1 );
      int d = m_plot->getNSubDivY() == 2 ? 10 : 4;
      double f = 10.0/d;
      for( int j=1; j < d; j++ ) {
	m_gdev->moveTo( 0, log10((static_cast<double>(j))*f) );
	m_gdev->drawTo( 1, log10((static_cast<double>(j))*f) );
      }
    }
  }
  else {
    m_gdev->setViewport( viewport.x0, viewport.y0, viewport.x1, viewport.y1 );
    m_gdev->setWindow( 0,0,1,1 );
    double f = 1.0 / (m_ndivy * m_plot->getNSubDivY());
    for( int i=0; i < (m_ndivy * m_plot->getNSubDivY()); i++ ) {
      m_gdev->moveTo( 0, (i+1)*f );
      m_gdev->drawTo( 1, (i+1)*f );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* drawXAxis --                                                                */
/* --------------------------------------------------------------------------- */

void ListGraph::drawXAxis( const Frame &viewport,
			   const std::deque<double> &values,
			   GuiPlotDataItem *dataitem ) {
  BUG(BugPlot,"ListGraph::drawXAxis");
  if( values.size() < 2 ) return;
  BUG_MSG( "viewport.x0 " << viewport.x0 << " viewport.y0 " << viewport.y0 << " viewport.x1 " << viewport.x1 << " viewport.y1 " << viewport.y1 );
  m_gdev->setViewport( viewport.x0, viewport.y0, viewport.x1, viewport.y1 );
  m_gdev->setWindow( 0,0,1,1 );
  m_gdev->setLinestyle( 0 );
  m_gdev->setLinewidth( 0 );
  m_gdev->setTextAttributes( 0.0045, Gdev::HCENTER, Gdev::VTOP, 0, 0 );

  m_gdev->moveTo( 0, 1 );
  m_gdev->drawTo( 1, 1 );

  double f = 1.0/(values.size()-1);
  if( m_logx ) BUG_MSG("Logarythmisch");

  for( int i=0; i < values.size(); i++ ) {
    if( i > 0 ) {
      m_gdev->moveTo( i*f, 0.95 );
      m_gdev->drawTo( i*f, 1.05 );
    }
    if( i < (values.size()-1) ) {
      if( m_logx ) {
	m_gdev->moveTo( i*f+(f*log10(5.)), 0.95 );
	m_gdev->drawTo( i*f+(f*log10(5.)), 1.05 );
      }
      else {
	m_gdev->moveTo( i*f+(f*0.5), 0.95 );
	m_gdev->drawTo( i*f+(f*0.5), 1.05 );
      }
    }
  }

  for( int ii=0; ii < values.size(); ii++ ) {
    m_gdev->moveTo( ii*f, 0.80 );
    std::ostringstream ostr;
    ostr << values[ii];
    m_gdev->putText( ostr.str() );
    if( m_logx && ii < (values.size()-1) ) {
      m_gdev->moveTo( ii*f+(f*log10(5.)), 0.90 );
      std::ostringstream ostr;
      ostr << values[ii+1]/2;
      m_gdev->putText( ostr.str() );
    }
  }

  if( dataitem != 0 ) {
    std::ostringstream ostrunit;
    m_gdev->moveTo( (values.size()-1)*f, 0.5  );
    ostrunit << dataitem->getUnit();
    m_gdev->putText( ostrunit.str() );
    m_gdev->moveTo( (values.size()-1)*f, 0.7 );
    std::ostringstream ostrlabel;
    ostrlabel << dataitem->getLabel();
    m_gdev->putText( ostrlabel.str() );
  }
}

/* --------------------------------------------------------------------------- */
/* drawYAxis --                                                                */
/* --------------------------------------------------------------------------- */

void ListGraph::drawYAxis( const Frame &viewport,
			   const std::vector<double> &values,
			   int column ) {
  BUG(BugPlot,"ListGraph::drawYAxis");
  if( values.size() < 2 ) return;
  BUG_MSG( "viewport.x0 " << viewport.x0 << " viewport.y0 " << viewport.y0 << " viewport.x1 " << viewport.x1 << " viewport.y1 " << viewport.y1 );
  m_gdev->setViewport( viewport.x0, viewport.y0, viewport.x1, viewport.y1 );
  m_gdev->setWindow( 0,0,1,1 );
  m_gdev->setLinestyle( 0 );
  m_gdev->setLinewidth( 0 );
  m_gdev->setColor( 0 );  // column
  m_gdev->setTextAttributes( 0.0045, Gdev::HRIGHT, Gdev::VCENTER, 0, 0 );

  double f = 1.0/(values.size()-1);
  if( m_logy ) BUG_MSG("Logarythmisch");

  if( column == 1 ) {
    m_gdev->moveTo( 1, 0 );
    m_gdev->drawTo( 1, 1 );
    for( int i=0; i < values.size(); i++ ) {
      if( i > 0 ) {
	m_gdev->moveTo( 0.95, i*f );
	m_gdev->drawTo( 1.05, i*f );
      }
      if( i < (values.size()-1) ) {
	if( m_logy ) {
	  m_gdev->moveTo( 0.95, i*f+(f*log10(5.)) );
	  m_gdev->drawTo( 1.05, i*f+(f*log10(5.)) );
	}
	else {
	  m_gdev->moveTo( 0.95, i*f+(f*0.5) );
	  m_gdev->drawTo( 1.05, i*f+(f*0.5) );
	}
      }
    }
  }

  for( int i=0; i < values.size(); i++ ) {
    m_gdev->moveTo( 0.85, i*f );
    std::ostringstream ostr;
    if (fabs(values[i]) < std::numeric_limits<double>::epsilon() )
      ostr << 0; // 0 soll auch 0 sein!!!
    else
      ostr << values[i];
    m_gdev->putText( ostr.str() );
    if( m_logy && i < (values.size()-1) ) {
      m_gdev->moveTo( 0.85, i*f+(f*log10(5.)) );
      std::ostringstream ostr;
      ostr << values[i+1]/2;
      m_gdev->putText( ostr.str() );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* drawYAxis --                                                                */
/* --------------------------------------------------------------------------- */

void ListGraph::drawYAxisLabel( const Frame &viewport,
			       GuiPlotDataItem *dataitem,
			       int column ) {
  m_gdev->setViewport( viewport.x0, viewport.y0, viewport.x1, viewport.y1 );
  m_gdev->setWindow( 0,0,1,1 );
  m_gdev->setLinestyle( 0 );
  m_gdev->setLinewidth( 0 );
  m_gdev->setTextAttributes( 0.0045, Gdev::HCENTER, Gdev::VCENTER, 0, 0 );

  if( dataitem != 0 ) {
    std::ostringstream ostrunit;
    m_gdev->moveTo( 0.50, 0.35  );
    ostrunit << dataitem->getUnit();
    m_gdev->putText( ostrunit.str() );
    m_gdev->moveTo( 0.50, 0.55  );
    std::ostringstream ostrlabel;
    ostrlabel << dataitem->getLabel();
    m_gdev->putText( ostrlabel.str() );
    m_gdev->setColor( column );
    m_gdev->mark( 0.5, 0.75, column );
    m_gdev->setColor( 0 );
  }
}

/* --------------------------------------------------------------------------- */
/* calculateMinMax --                                                          */
/* --------------------------------------------------------------------------- */

int ListGraph::calculateMinMax( bool init ) {
  int maxvalues = 0;
  int n, i;
  std::list<GuiPlotDataItem*>::iterator iaxes;

  for( n=0; n < m_yaxes.size(); n++ ) {
    GuiPlotDataItem *yitem = m_yaxes[n];
    bool first = init;
    if( yitem->XferData() != 0 ) {
      XferDataItemIndex *index = yitem->getDataItemIndexWildcard( 1 );
      int dimsize = index->getDimensionSize( yitem->Data() );
      if( dimsize > maxvalues )
	maxvalues = dimsize;
      for( i=0; i < dimsize; i++ ) {
	index->setIndex( yitem->Data(), i );
	if( yitem->isValid() ) {
	  double value;
	  if( yitem->getValue( value ) ) {
	    if( !(m_logy && value*yitem->getScaleFactor() <= 0) ) {
	      if( ((value*yitem->getScaleFactor()) > m_orgmaxvalues[n]) || first )
		m_orgmaxvalues[n] = value * yitem->getScaleFactor();
	      if( ((value*yitem->getScaleFactor()) < m_orgminvalues[n]) || first )
		m_orgminvalues[n] = value * yitem->getScaleFactor();
	      // nur einmal setzen
	      m_lessThanOriginY = m_lessThanOriginY ? true : (m_orgminvalues[n] < m_originY);
	      first = false;
	      if( m_options[n] == Invalid )
		m_options[n] = Dynamic;
	    }
	  }
	}
      }
    }
  }
  return maxvalues;
}

/* --------------------------------------------------------------------------- */
/* calculateNewRange --                                                        */
/* --------------------------------------------------------------------------- */

double ListGraph::calculateNewRange( int steps, double min, double max, bool yAxis ) {
  if( steps < 1 )
    steps = 1;

//   if (yAxis) {
//      max = ( max > m_originY ) ? max : m_originY;
//      min = ( min < m_originY ) ? min : m_originY;
//    }

  // minimale Ausdehnung erzeugen
  if( fabs(max-min) < std::numeric_limits<double>::epsilon() ) {
    double tmp = 1.0;
    if( fabs(min) > std::numeric_limits<double>::epsilon() )
      tmp = fabs(min) / 10.0;
    min -= tmp;
    max += tmp;
  }

  // min/max Werte korrigieren:
  // gesetzes OriginY soll in der Mitte liegen
  // wenn darunter liegende Werte vorhanden sind
  if (yAxis &&
      m_lessThanOriginY && // Werte unterhalb OriginY vorhanden
      m_isOriginYDef       // OriginY definiert
      ) {
    double diffMax = fabs(m_originY - max);
    double diffMin = fabs(m_originY - min);
    if (diffMax> diffMin)
      min = m_originY - diffMax;
    else
      max = m_originY + diffMin;
  }

  // steps ermitteln
  double delta = 0;
  delta = (max - min) / steps;
  if (yAxis)
    delta += delta / steps / 2.0;
  //  delta = fabs( delta );

  double n = 1.0;
  while( (delta < 1.0) || (delta >= 10.0) ) {
    if( delta < 1.0 ) {
      delta *= 10.0;
      n /= 10.0;
    }
    else {
      delta /= 10.0;
      n *= 10.0;
    }
  }

//   double maxdeltau = 1.0;
//   double maxdeltao = 1.1;

//   vector<double> s( 3 );
//   s[0] = 1.0;
//   s[1] = 0.5;
//   s[2] = 0.25;

//   while( true ) {
//     for( int i=1; i <= 10; i++ ) {
//       double r = (s[0]*i) / delta;
//       if( (r > maxdeltau) && (r < maxdeltao) ) {
// 	return s[0] * i * n;
//       }
//     }
//     for( int i=1; i <= 20; i++ ) {
//       double r = (s[1]*i) / delta;
//       if( (r > maxdeltau) && (r < maxdeltao) ) {
// 	return s[1] * i * n;
//       }
//     }
//     for( int i=1; i <= 40; i++ ) {
//       double r = (s[2]*i) / delta;
//       if( (r > maxdeltau) && (r < maxdeltao) ) {
// 	return s[2] * i * n;
//       }
//     }
//     maxdeltao += 0.1;
//   }

  double maxdeltau = 1.0;
  double maxdeltao = yAxis ? 1.4 : 1.05;

  double s[][50] = {
    { 0.25, 0.5, 1.0, -1.0 }
  , { 0.1, 0.2, 0.75, -1.0 }
  , { 0.1, 0.2, 0.3, 0.4, 0.6, 0.7, 0.75, 0.8, 0.9, -1.0 }
  , { 0.15, 0.35, 0.45, 0.55, 0.65, 0.85, 0.95, 0.125, 0.175, 0.225, 0.275, 0.325, 0.375, 0.425
  , 0.475, 0.525, 0.575, 0.625, 0.675, 0.725, 0.775, 0.825, 0.875, 0.925, 0.975, -1.0 }
  , { -1.0 }
  };

  while (maxdeltao < 2.0) {
    for( int i=0; s[i][0] > 0; i++ ) {
      for( int j=0; s[i][j] > 0;  j++ ) {
	double r = s[i][j]*10 / delta;
	if( (r > maxdeltau) && (r < maxdeltao) ) {
	  return s[i][j] * 10 * n;
	}
      }
    }
    maxdeltao += 0.05;
  }

  assert( false );
  return -1;
}

/* --------------------------------------------------------------------------- */
/* calculateNewRangeLog --                                                     */
/* --------------------------------------------------------------------------- */

bool ListGraph::calculateNewRangeLog( int nyitems ) {
  bool ret = false;
  for( int i=0; i < nyitems; i++ ) {
    double min = m_orgminvalues[i];
    double max = m_orgmaxvalues[i];
    double nmin = 1.0;
    double nmax = 10.0;
    if (min != 0 || max != 0) ret= true;  // eine gueltiger plot reicht

    while( max && (max < 1.0) || (max >= 10.0) ) {
      if( max < 1.0 ) {
	max *= 10.0;
	nmax /= 10.0;
      }
      else {
	max /= 10.0;
	nmax *= 10.0;
      }
    }

    while( min && (min < 1.0) || (min >= 10.0) ) {
      if( min < 1.0 ) {
	min *= 10.0;
	nmin /= 10.0;
      }
      else {
	min /= 10.0;
	nmin *= 10.0;
      }
    }

    if( nmin >= nmax )
      nmin = nmax/10;

    double tmp = nmin;
    int count;
    for( count=0; tmp < nmax; count++ ) {
      tmp*=10;
    }

    m_outmaxvalues[i] = nmax;

    if( count > m_nmaxdivy )
      count = 10;
    if( count > m_ndivy )
      m_ndivy = count;
  }

  double f = 1.0;
  for( int ii=0; ii < m_ndivy; ii++ )
    f *= 10;

  for( int iii=0; iii < nyitems; iii++ )
    m_outminvalues[iii] = m_outmaxvalues[iii]/f;

  return ret;
}

/* --------------------------------------------------------------------------- */
/* calculateNewRangeXLog --                                                    */
/* --------------------------------------------------------------------------- */

bool ListGraph::calculateNewRangeXLog() {
  double min = m_orgminxaxis;
  //  if( min <= 0 ) return false;
  double max = m_orgmaxxaxis;
  //  if( max <= 0 ) return false;
  double nmin = 1.0;
  double nmax = 10.0;
  if (min != 0 && max != 0) return false;

  while( (max < 1.0) || (max >= 10.0) ) {
    if( max < 1.0 ) {
      max *= 10.0;
      nmax /= 10.0;
    }
    else {
      max /= 10.0;
      nmax *= 10.0;
    }
  }

  while( (min < 1.0) || (min >= 10.0) ) {
    if( min < 1.0 ) {
      min *= 10.0;
      nmin /= 10.0;
    }
    else {
      min /= 10.0;
      nmin *= 10.0;
    }
  }

  if( nmin >= nmax )
    nmin = nmax/10;

  double tmp = nmin;
  int count;
  for( count=0; tmp < nmax; count++ ) {
    tmp*=10;
  }

  if( count > m_nmaxdivx )
    count = 10;
  m_ndivx = count;

  double f = 1.0;
  for( int i=0; i < m_ndivx; i++ ) {
    f *= 10;
  }

  m_outmaxxaxis = nmax;
  m_outminxaxis = nmax/f;

  return true;
}

/* --------------------------------------------------------------------------- */
/* drawValues --                                                               */
/* --------------------------------------------------------------------------- */
void ListGraph::drawValues( const Frame &viewport,
			    Frame &windowsize,
			    GuiPlotDataItem *dataitem,
			    std::vector<double> &xvalues,
			    int column ) {
  m_gdev->setViewport( viewport.x0, viewport.y0, viewport.x1, viewport.y1 );
  if( m_logx ) {
    windowsize.x0 = log10(windowsize.x0);
    windowsize.x1 = log10(windowsize.x1);
  }
  if( m_logy ) {
    windowsize.y0 = log10(windowsize.y0);
    windowsize.y1 = log10(windowsize.y1);
  }
  m_gdev->setWindow( windowsize.x0, windowsize.y0, windowsize.x1, windowsize.y1 );
  m_gdev->setLinestyle( 0 );
  m_gdev->setLinewidth( 1 );
  m_gdev->setColor( column );
  bool setmark = true;
  double lastx = 0, lasty = 0;
  double value = 0.0;

  if( dataitem->XferData() != 0 ) {
    XferDataItemIndex *index = dataitem->getDataItemIndexWildcard( 1 );
    int dimsize = index->getDimensionSize( dataitem->Data() );
    int min = dimsize < xvalues.size() ? dimsize : xvalues.size();
    bool first = true;
    bool possible = false;
    for( int i=0; i < min; i++ ) {
      index->setIndex( dataitem->Data(), i );
      if( dataitem->isValid() ) {
	dataitem->getValue( value );
	if( m_logy ) {
	  if( value*dataitem->getScaleFactor() <= 0 ) {
// 	    ostringstream ostr;
// 	    ostr << "ListGraph '" << m_name << "' item '" << dataitem->getName()
// 		 << "' at dimension '" << i << "': can't draw value '"
// 		 << value*dataitem->getScaleFactor() << "' logarithmicaly!" << endl;
// 	    GuiManager::Instance().getLogWindow()->writeText( ostr.str() );
	    possible = false;
	  }
	  else {
	    value = log10(value*dataitem->getScaleFactor());
	    possible = true;
	  }
	}
	else {
	  value *= dataitem->getScaleFactor();
	  possible = true;
	}
  	if( value >= windowsize.y0 && value <= windowsize.y1 &&
  	    xvalues[i] >= windowsize.x0 && xvalues[i] <= windowsize.x1 && possible ) {
	  if( !first ) {
	    if( dataitem->getLineStyle() == GuiPlotDataItem::STEP ) {
	      m_gdev->drawTo( lastx, value );
	      m_gdev->moveTo( lastx, value );
	      m_gdev->drawTo( xvalues[i], value );
	    }
	    else if( dataitem->getLineStyle() != GuiPlotDataItem::DISCRETE ) {
	      m_gdev->drawTo( xvalues[i], value );
	    }
	  }
	  m_gdev->moveTo( xvalues[i], value );
	  lastx = xvalues[i];
	  lasty = value;

	  if( first ){
	    // erste markierung nicht mehr anderes?
	    m_gdev->mark( xvalues[i], value, column/*CROSS*/ );
	    first = false;
	  }
	  else {
	    if( dataitem->getLineStyle() != GuiPlotDataItem::DISCRETE ) {
	      int div =  0;
	      if( min >= m_numberOfMarks )
		div = i % static_cast<int>(min/m_numberOfMarks);
	      if( setmark && div==0 )
		m_gdev->mark( xvalues[i], value, column );
	    }
	    else {
	      m_gdev->mark( xvalues[i], value, column );
	    }
	  }
	}
	else {
	  first = true;
	}
      }
      else {
	first = true;
      }
    }
    // letzte markierung nicht mehr gewollt?
    ///    m_gdev->mark( lastx, lasty/*value*/, TRIANGLE );
  }
}

/* --------------------------------------------------------------------------- */
/* drawTitle --                                                                */
/* --------------------------------------------------------------------------- */

void ListGraph::drawTitle( const Frame &viewport ) {
  m_gdev->setViewport( viewport.x0, viewport.y0, viewport.x1, viewport.y1 );
  m_gdev->setWindow( 0,0,1,1 );
  m_gdev->setColor( 0 );
  m_gdev->setTextAttributes( 0.007, Gdev::HCENTER, Gdev::VCENTER, 0, 1 );

  m_gdev->moveTo( 0.5, 0.65 ); //0.5, 0.35
  m_gdev->putText( getTitle() );
}

/* --------------------------------------------------------------------------- */
/* buildConfigDialog --                                                        */
/* --------------------------------------------------------------------------- */

void ListGraph::buildConfigDialog( GuiElement *cont ) {
  BUG(BugPlot,"ListGraph::buildConfigDialog");

//   MultiLanguage &mls = MultiLanguage::Instance();

  GuiFieldgroup *group = GuiFactory::Instance() -> createFieldgroup( cont, "" );
  cont->attach( group -> getElement() );
  group->setTitle( getTitle() );
  group->setTitleAlignment( GuiElement::align_Center );

  GuiFieldgroupLine *line = dynamic_cast<GuiFieldgroupLine*>(group->addFieldgroupLine());
  assert( line != 0 );
  GuiLabel *label = GuiFactory::Instance() -> createLabel( line->getElement(), GuiElement::align_Default );
  label->setLabel( _("Item name") );
  label = GuiFactory::Instance() -> createLabel( line->getElement(), GuiElement::align_Default );
  label->setLabel( _("Unit") );
  label = GuiFactory::Instance() -> createLabel( line->getElement(), GuiElement::align_Default );
  label->setLabel( _("Min. Scale") );
  label = GuiFactory::Instance() -> createLabel( line->getElement(), GuiElement::align_Default );
  label->setLabel( _("Max. Scale") );
  label = GuiFactory::Instance() -> createLabel( line->getElement(), GuiElement::align_Default );
  label->setLabel( _("Scaling Type") );

  int offs = m_xaxis != 0 ? 0 : 1;
  std::vector<GuiPlotDataItem*> tmp;
  tmp.push_back( m_xaxis );
  for( int ii=0; ii < m_yaxes.size(); ii++ ) {
    tmp.push_back( m_yaxes[ii] );
  }

  DataReference     *dref = 0;
  XferDataItem      *dataitem = 0;
  XferDataItemIndex *index = 0;
  GuiDataField      *field = 0;

  for( int i=offs; i < tmp.size(); i++ ) {
    GuiPlotDataItem *item = tmp[i];
    line = dynamic_cast<GuiFieldgroupLine*>(group->addFieldgroupLine());
    assert( line != 0 );
    label = GuiFactory::Instance() -> createLabel( line->getElement()
						   , GuiElement::align_Default );
    std::string addLabel;
    if (item == m_xaxis)
      addLabel = _(" (X)");
    label->setLabel( tmp[i]->getLabel() + addLabel );
    label = GuiFactory::Instance() -> createLabel( line->getElement()
						   , GuiElement::align_Default );
    label->setLabel( tmp[i]->getUnit() );

    // min. data field
    dref = DataPoolIntens::getDataReference( m_listGraphStruct, "min" );
    assert( dref != 0 );
    dataitem = new XferDataItem();
    dataitem->setDataReference( dref );
    index = dataitem->newDataItemIndex( 1 );
    index->setLowerbound( i );
    field = dynamic_cast<GuiDataField*>(GuiFactory::Instance()->createDataField( line->getElement(), dataitem ));
    field->setLength( 10 );
    line->attach( field->getElement() );
    m_cDminDataFields.push_back( field );

    // max. data field
    dref = DataPoolIntens::getDataReference( m_listGraphStruct, "max" );
    assert( dref != 0 );
    dataitem = new XferDataItem();
    dataitem->setDataReference( dref );
    index = dataitem->newDataItemIndex( 1 );
    index->setLowerbound( i );
    field =  dynamic_cast<GuiDataField*>(GuiFactory::Instance()->createDataField( line->getElement(), dataitem ));
    field->setLength( 10 );
    line->attach( field->getElement() );
    m_cDmaxDataFields.push_back( field );

    // state data field
    dref = DataPoolIntens::getDataReference( m_listGraphStruct, "state" );
    assert( dref != 0 );
    dataitem = new XferDataItem();
    dataitem->setDataReference( dref );
    index = dataitem->newDataItemIndex( 1 );
    index->setLowerbound( i );
    field =  dynamic_cast<GuiDataField*>(GuiFactory::Instance()->createDataField( line->getElement(), dataitem ));
    field->setLength( 10 );
    line->attach( field->getElement() );
    m_cDstateDataFields.push_back( field );
  }
}

/* --------------------------------------------------------------------------- */
/* cDupdateDatafields --                                                       */
/* --------------------------------------------------------------------------- */

void ListGraph::cDupdateDatafields() {
  BUG(BugPlot,"ListGraph::cDupdateDatafields");
  for( int i=0; i < m_cDminDataFields.size(); i++ ) {
    DataReference *data = m_cDstateDataFields[i]->Data();
    if( data != 0 ) {
      int tmp=0;
      if (data->GetValue_PreInx( tmp ) ) {
	if( tmp == Dynamic ) {
	  m_cDminDataFields[i]->getElement()->disable();
	  m_cDmaxDataFields[i]->getElement()->disable();
	}
	else if( tmp == Static ) {
	  m_cDminDataFields[i]->getElement()->enable();
	  m_cDmaxDataFields[i]->getElement()->enable();
	}
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */

bool ListGraph::isDataItemUpdated( TransactionNumber trans ) {
  BUG(BugPlot,"ListGraph::isDataItemUpdated");
  BUG_MSG( "Graph '" << getTitle() << "'" );

  for( int n=0; n < m_yaxes.size(); n++ ) {
    GuiPlotDataItem *yitem = m_yaxes[n];
    if( yitem != 0 ) {
      if( yitem->isDataItemUpdated(trans) ) {
	BUG_MSG( "yitem '" << yitem->getName() <<"' is updated" );
	return true;
      }
    }
  }
  if( m_xaxis != 0 ) {
    if( m_xaxis->isDataItemUpdated(trans) ) {
      BUG_MSG( "xitem '" << m_xaxis->getName() <<"' is updated" );
      return true;
    }
  }
  if( m_listGraphStruct != 0 ) {
    if( m_listGraphStruct->isDataItemUpdated( DataReference::ValueUpdated, trans ) ) {
      BUG_MSG( "struct '" << m_listGraphStruct->fullName( true ) << "' is updated" );
      return true;
    }
  }
  BUG_MSG( "No item are updated" );
  return false;
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool ListGraph::acceptIndex( const std::string &name, int inx ){
  for( int n=0; n < m_yaxes.size(); n++ ) {
    GuiPlotDataItem *yitem = m_yaxes[n];
    if( yitem != 0 ) {
      if( !yitem->acceptIndex( name, inx ) ) {
	return false;
      }
    }
  }
  if( m_xaxis != 0 ) {
    if( !m_xaxis->acceptIndex( name, inx ) ){
      return false;
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void ListGraph::setIndex( const std::string &name, int inx ){
  for( int n=0; n < m_yaxes.size(); n++ ) {
    GuiPlotDataItem *yitem = m_yaxes[n];
    if( yitem != 0 ) {
      yitem->setIndex( name, inx );
    }
  }
  if( m_xaxis != 0 ) {
    m_xaxis->setIndex( name, inx );
  }
}

/* --------------------------------------------------------------------------- */
/* writeValues --                                                              */
/* --------------------------------------------------------------------------- */

void ListGraph::writeValues( bool reset ) {
  BUG(BugPlot,"ListGraph::writeValues");
  if( m_plot == 0 ) return;

  int maxvalues = 0;
  bool first = true;
  if( m_plot->printAllCycles() ) {
    DataPool &datapool = DataPoolIntens::getDataPool();
    int currentcyc = datapool.GetCurrentCycle();
    int numcyc = datapool.NumCycles();

    for( int cyc=0; cyc < numcyc; cyc++ ) {
      datapool.SetCycle( cyc, false ); // do not clear undo stack
      maxvalues = calculateMinMax( first );
    }
    datapool.SetCycle( currentcyc, false ); // do not clear undo stack
  }
  else {
    maxvalues = calculateMinMax( first );
  }

  for( int i=0; i < 9; i++ ) {
    if( i == 0 ) {
      m_drefname->SetValue( m_xaxisname, i );
      if( reset ) {
	if( m_optionsxaxis == Static )
	  m_optionsxaxis = Dynamic;
      }
      m_drefstate->SetValue( m_optionsxaxis, i );
      if( m_optionsxaxis == Static ) {
	m_drefmin->SetValue( m_outminxaxis, i );
	m_drefmax->SetValue( m_outmaxxaxis, i );
      }
      else {
	m_drefmin->SetValue( m_orgminxaxis, i );
	m_drefmax->SetValue( m_orgmaxxaxis, i );
      }
    }
    else {
      m_drefname->SetValue( m_itemnames[i-1], i );
      if( reset ) {
	if( m_options[i-1] == Static )
	  m_options[i-1] = Dynamic;
      }
      m_drefstate->SetValue( m_options[i-1], i );
      if( m_options[i-1] == Static ) {
	m_drefmin->SetValue( m_outminvalues[i-1], i );
	m_drefmax->SetValue( m_outmaxvalues[i-1], i );
      }
      else {
	m_drefmin->SetValue( m_orgminvalues[i-1], i );
	m_drefmax->SetValue( m_orgmaxvalues[i-1], i );
      }
    }
  }

  cDupdateDatafields();
  m_plot->updateConfigDialog( GuiElement::reason_FieldInput );
}

/* --------------------------------------------------------------------------- */
/* readValues --                                                               */
/* --------------------------------------------------------------------------- */

void ListGraph::readValues() {
  BUG(BugPlot,"ListGraph::readValues");
  if( m_logx ) BUG_MSG("===============> Log X");
  if( m_logy ) BUG_MSG("===============> Log Y");

  double dvalue=0;
  int    ivalue=0;

  BUG_MSG(m_drefmin->fullName());
  BUG_MSG(m_drefmax->fullName());
  BUG_MSG(m_drefstate->fullName());

  for( int i=0; i < 9; i++ ) {
    if( i == 0 ) {
      if( m_drefmin->GetValue( dvalue, i ) )
	m_outminxaxis = dvalue;
      if( m_drefmax->GetValue( dvalue, i ) )
	m_outmaxxaxis = dvalue;
      if( m_drefstate->GetValue( ivalue, i ) )
	m_optionsxaxis = ivalue;
    }
    else {
      if( m_drefmin->GetValue( dvalue, i ) )
	m_outminvalues[i-1] = dvalue;
      if( m_drefmax->GetValue( dvalue, i ) )
	m_outmaxvalues[i-1] = dvalue;
      if( m_drefstate->GetValue( ivalue, i ) )
	m_options[i-1] = ivalue;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* createDataReference --                                                      */
/* --------------------------------------------------------------------------- */

void ListGraph::createDataReference() {
  BUG(BugPlot,"ListGraph::initDataItem");
  assert( m_plot != 0 );

  DataPool &datapool = DataPoolIntens::getDataPool();
  std::string varname;
  varname = "@ListGraphVar" + m_id;

  DataDictionary *dict_struct;
  dict_struct = datapool.AddToDictionary( ""
					, varname
					, DataDictionary::type_StructVariable
					, INTERNAL_LISTGRAPH_STRUCT );

  assert( dict_struct != 0 );

  // Wir schützen die Variable vor dem Cycle-Clear. Sonst gehen immer
  // alle Einstellungen verloren.
  dict_struct->setItemProtected();
  static_cast<UserAttr*>(dict_struct->GetAttr())->SetEditable();

  m_listGraphStruct=DataPoolIntens::getDataReference( varname );
  assert( m_listGraphStruct != 0 );
  static_cast<UserAttr*>(m_listGraphStruct->getUserAttr())->SetEditable();

  m_drefname = DataPoolIntens::getDataReference( m_listGraphStruct, "name" );
  assert( m_drefname != 0 );

  m_drefmin = DataPoolIntens::getDataReference( m_listGraphStruct, "min" );
  assert( m_drefmin != 0 );

  m_drefmax = DataPoolIntens::getDataReference( m_listGraphStruct, "max" );
  assert( m_drefmax != 0 );

  m_drefstate = DataPoolIntens::getDataReference( m_listGraphStruct, "state" );
  assert( m_drefstate != 0 );

  // Namen der dataitems zwischenlagern
  for( int n=0; n < m_yaxes.size(); n++ ) {
    if( m_yaxes[n] != 0 )
      m_itemnames[n] = m_yaxes[n]->getName();
  }
  if( m_xaxis != 0 )
    m_xaxisname = m_xaxis->getName();
}
