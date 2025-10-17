
#include <sstream>
#include <limits>

#include "utils/utils.h"
#include "utils/Separator.h"
#include "utils/compose.h"
#include "utils/Debugger.h"
#include "datapool/DataDimension.h"
#include "datapool/DataRealElement.h"
#include "datapool/DataIntegerElement.h"
#include "datapool/DataStringElement.h"
#include "datapool/DataElement.h"
#include "datapool/DataItem.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataPool.h"
#include "datapool/DataEvents.h"
#include "datapool/DataReference.h"
#include "datapool/DataVector.h"
#include "datapool/DataValue.h"
#include "datapool/DataAlterFunctor.h"
#include "datapool/DataInx.h"
#include "datapool/DataElementMap.h"
#if defined(HAVE_PYTHON)
#include "python/PythonObject.h"
#endif

using namespace SemaforString;

INIT_LOGGER();

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataDimension);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataDimension::DataDimension( DataItem &item, DataContainer *data )
  : m_NumDimensions( data->getNumberOfDimensions() + 1 )
  , m_number_of_resize( 0 )
  , m_is_compare_dimension( false ){
  BUG_INCR_COUNT;
  BUG( BugRef, "Constructor" );

  int default_size = item.dictionary().GetDefaultDimension( m_NumDimensions );
  setSizeOfContainer( maximum( 1, default_size ) );
}


DataDimension::DataDimension( int nDim, int size )
  : m_NumDimensions( nDim )
  , m_number_of_resize( 0 )
  , m_is_compare_dimension( false ){
  BUG_INCR_COUNT;
  BUG_PARA( BugRef, "Constructor", "numDim=" << nDim << " size=" << size );

  assert(getNumberOfDimensions() > 0);
  setSizeOfContainer( size );
}


DataDimension::DataDimension( const DataDimension &ref ) // Copy
  : DataContainer( ref )
  , m_NumDimensions( ref.m_NumDimensions )
  , m_number_of_resize( 0 )
  , m_is_compare_dimension( ref.m_is_compare_dimension ){
  BUG_INCR_COUNT;
  BUG( BugRef, "Copy-Constructor" );

  assert( getNumberOfDimensions() > 0 );

  // Bei einer Kopie wollen wir keine Reserven anlegen. Ein leerer Container
  // mit der exakten Grösse wird angelegt.
  setSizeOfContainer( ref.m_Containers.size() );

  // Der Container enthaelt Pointers auf DataContainer's. Diese Container
  // werden nun geklont.
  ContainerList::const_iterator it_src = ref.m_Containers.begin();
  ContainerList::iterator it_dest;
  for( it_dest = m_Containers.begin(); it_dest != m_Containers.end(); it_dest++ ){
    if( *it_src != 0 ){
      *it_dest = (*it_src)->clone();
    }
    it_src++;
  }
}


DataDimension::~DataDimension(){
  BUG_DECR_COUNT;

  ContainerList::iterator pIt;
  for(pIt=m_Containers.begin(); pIt!=m_Containers.end(); pIt++){
    if( (*pIt) != 0 ){
      delete (*pIt);
      *pIt = 0;
    }
  }
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

DataContainer *DataDimension::clone() const{
  return new DataDimension( *this );
}

/* --------------------------------------------------------------------------- */
/* setContainer --                                                             */
/* --------------------------------------------------------------------------- */

void DataDimension::setContainer( DataContainer *data, int inx ){
  assert( getNumberOfElements() > inx );
  assert( m_Containers[inx] == 0 );
  m_Containers[inx] = data;
}

/* --------------------------------------------------------------------------- */
/* setContainer --                                                             */
/* --------------------------------------------------------------------------- */

void DataDimension::moveContainer( int inx_from, int inx_to, DataContainer *data ){
  assert( getNumberOfElements() > inx_from );
  assert( inx_from != inx_to );

  if( inx_to >= getNumberOfElements() ){
    int cap = m_Containers.capacity();
    m_Containers.resize( inx_to + 1, 0 );
  }
  if( m_Containers[inx_to] != 0 ){
    delete m_Containers[inx_to];
  }

  m_Containers[inx_to]   = m_Containers[inx_from];
  m_Containers[inx_from] = data;
}

/* --------------------------------------------------------------------------- */
/* getNumberOfElements --                                                      */
/* --------------------------------------------------------------------------- */

int DataDimension::getNumberOfElements() const{
  return (int)m_Containers.size();
}

/* --------------------------------------------------------------------------- */
/* getNumberOfDimensions --                                                    */
/* --------------------------------------------------------------------------- */

int DataDimension::getNumberOfDimensions() const{
  return m_NumDimensions;
}

/* --------------------------------------------------------------------------- */
/* setSizeOfContainer --                                                       */
/* --------------------------------------------------------------------------- */

void DataDimension::setSizeOfContainer( int sz ){
  if( getNumberOfElements() >= sz ){
    return;
  }
  m_Containers.reserve( sz );
  m_Containers.resize( sz, 0 );
}

/* --------------------------------------------------------------------------- */
/* setReserve --                                                               */
/* --------------------------------------------------------------------------- */

void DataDimension::setReserve( int reserve ){
  if( (int)m_Containers.size() < reserve ){
    m_Containers.reserve( reserve );
  }
}

/* --------------------------------------------------------------------------- */
/* resizeContainer --                                                          */
/* --------------------------------------------------------------------------- */

void DataDimension::resizeContainer( int NewSize ){
  BUG_PARA( BugRef, "ResizeContainer"
            , "new size=" << NewSize << ", old size=" << (int)m_Containers.size() );

  assert( NewSize >= (int)m_Containers.size() );

  if( (int)m_Containers.capacity() < NewSize ){
    // Je nach Grösse des Containers gehen wir unterschiedlich vor. Erst ab einer
    // gewissen Grösse versuchen wir mit reserve() den Speicherplatz einigermassen
    // zu kontrollieren.
    int oversize = 0;
    if( NewSize > 10000 ){
      oversize = minimum( 2000, 100 * m_number_of_resize );
    }
    else
    if( NewSize > 1000 ){
      oversize = minimum( 1000, 50 * m_number_of_resize );
    }
    else
    if( NewSize > 20 ){
      oversize = minimum( 100, 5 * m_number_of_resize );
    }
    m_number_of_resize++;

    if( oversize != 0 ){
      m_Containers.reserve( NewSize + oversize );
    }
  }
  m_Containers.resize( NewSize, 0 );
}

/* --------------------------------------------------------------------------- */
/* isBigEnough --                                                              */
/* --------------------------------------------------------------------------- */

bool DataDimension::isBigEnough( int sz ) const{
  return sz <= getNumberOfElements();
}

/* --------------------------------------------------------------------------- */
/* installNewContainer --                                                      */
/* --------------------------------------------------------------------------- */

void DataDimension::installNewContainer( int index, DataItem &item ){
  BUG_PARA( BugRef, "installNewContainer", "Index=" << index );
  assert( index >= 0 );

  if( !isBigEnough( index + 1 ) ){
    resizeContainer( index + 1 );
  }
  assert( m_Containers[index] == 0 );
  m_Containers[index] = item.newContainer();

  while( getNumberOfDimensions()-1 > m_Containers[index]->getNumberOfDimensions() ){
    BUG_MSG( "Adding Dimension" );
    m_Containers[index] = m_Containers[index]->addNewDimension( item );
  }
  return;
}

/* --------------------------------------------------------------------------- */
/* container --                                                                */
/* --------------------------------------------------------------------------- */

DataContainer *DataDimension::container( int inx ){
  if( inx < getNumberOfElements() ){
    return m_Containers[inx];
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* container --                                                                */
/* --------------------------------------------------------------------------- */

const DataContainer *DataDimension::container( int inx ) const{
  if( inx < getNumberOfElements() ){
    return m_Containers[inx];
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getDataDimensionSizeOf --                                                   */
/* --------------------------------------------------------------------------- */

int DataDimension::getDataDimensionSizeOf( int dim_num ) const{
  int elements = getNumberOfElements();
  if( dim_num > getNumberOfDimensions() ){
    // Jede Dimension, ob physisch vorhanden oder nicht, hat ein Element.
    return 1;
  }

  if( dim_num < 1 ){
    return elements;
  }

  int dim_size = 0;
  for( int i=0; i < elements; i++ ){
    if( m_Containers[i] != 0 ){
      dim_size = maximum( dim_size, m_Containers[i]->getDataDimensionSizeOf( dim_num-1 ) );
    }
  }
  return dim_size;
}

/* --------------------------------------------------------------------------- */
/* getAllDataDimensionSize --                                                  */
/* --------------------------------------------------------------------------- */

int DataDimension::getAllDataDimensionSize( DataContainer::SizeList &dimsize,
					    int dim,
					    bool incl_attr ) const
{
  BUG_DEBUG( "getAllDataDimensionSize: Dimension=" << dim );

  int elems = getNumberOfElements();
  int maxdims = 0;

  if( getNumberOfDimensions() == 1 ){
    // Scalare Werte
    assert( dim == dimsize.size()-1 );
    for( int i = elems-1; i >= 0; i-- ){
      if( m_Containers[i] != 0 ){
        if( m_Containers[i]->getAllDataDimensionSize( dimsize, dim + 1, incl_attr ) > 0 ){
          // Der Wert ist gültig
          maxdims = i + 1;
          i = -1; // break
        }
      }
    }
  }
  else{
    assert(dim < (int)dimsize.size() );
    for( int i = elems-1; i >= 0; i-- ){
      if( m_Containers[i] != 0 ){
        int tmp = m_Containers[i]->getAllDataDimensionSize( dimsize, dim + 1, incl_attr );
        if( tmp > 0 && maxdims == 0 ){
          maxdims = i + 1;
        }
      }
    }
  }

  dimsize[dim] = maxdims > dimsize[dim] ? maxdims : dimsize[dim];
  return maxdims;
}

/* --------------------------------------------------------------------------- */
/* getAllDataDimensionSize --                                                  */
/* --------------------------------------------------------------------------- */

int DataDimension::getAllDataDimensionSize( DataContainer::SizeList &dimsize
                                          , int dim
                                          , DataInx &index ) const
{
  BUG_DEBUG( "getAllDataDimensionSize: Dimension=" << dim );
  assert( dim < (int)dimsize.size() );

  int elems       = getNumberOfElements();
  bool last_level = getNumberOfDimensions() == 1;
  int dims        = 0;
  int max_dims    = 0;
  int inx         = 0;

  if( last_level ){
    assert( dim == dimsize.size()-1 );

    if( index.isWildcard() ){
      dims = getValidDimensionSize();
      max_dims = dims;
    }
    else{
      int inx = index.getIndex();
      if( elems > inx ){
        if( m_Containers[inx] != 0 ){
          dims = m_Containers[inx]->getValidDimensionSize();
          max_dims = dims > 0 ? 1 : 0;
        }
      }
    }
    dimsize[dim] = maximum( dims, dimsize[dim] );
    return max_dims; // end of last level
  }

  if( index.isWildcard() ){
    for( inx = elems-1; inx >= 0; inx-- ){
      DataInx tmp_index( index );
      tmp_index.nextIndex();

      if( m_Containers[inx] != 0 ){
        dims = m_Containers[inx]->getAllDataDimensionSize( dimsize, dim + 1, tmp_index );
        if( dims > 0 ){
          max_dims = maximum( max_dims, inx+1 );
        }
      }
    }
  }
  else{
    inx = index.getIndex();
    if( elems > inx ){
      if( m_Containers[inx] != 0 ){
        index.nextIndex();
        dims = m_Containers[inx]->getAllDataDimensionSize( dimsize, dim + 1, index );
        max_dims = dims > 0 ? 1 : 0;
      }
    }
  }

  dimsize[dim] = max_dims > dimsize[dim] ? max_dims : dimsize[dim];
  return max_dims;
}

/* --------------------------------------------------------------------------- */
/* getValidDimensionSize --                                                    */
/* --------------------------------------------------------------------------- */

 int DataDimension::getValidDimensionSize( bool incl_itemattr ) const {
  int elems = getNumberOfElements();

  for( int i = elems-1; i >= 0; i-- ){
    if( m_Containers[i] != 0 ){
      if( m_Containers[i]->getValidDimensionSize( incl_itemattr ) ){
        // Der Wert ist gültig. Die Grösse ist gefunden.
        return i + 1;
      }
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* DataDictionaryModified --                                                   */
/* --------------------------------------------------------------------------- */

void DataDimension::DataDictionaryModified( DataPool &datapool
                                          , DataDictionary *pDict
                                          , bool AddOrDelete
                                          , bool GlobalFlag
                                          , bool FirstCycle
                                          , DataItem **pFirstCycleItem )
{
  BUG_DEBUG("DataDictionaryModified");

  ContainerList::iterator it;
  for( it = m_Containers.begin(); it != m_Containers.end(); ++it ){
    if( (*it) ){
      (*it)->DataDictionaryModified( datapool
                                   , pDict
                                   , AddOrDelete
                                   , GlobalFlag
                                   , FirstCycle
                                   , pFirstCycleItem);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* copyContainer --                                                            */
/* --------------------------------------------------------------------------- */

DataContainer *DataDimension::copyContainer( DataPool &datapool ) const{
  BUG_DEBUG("copyContainer");

  // ----------------------------------------------------------------------
  // Bei einer Kopieraktion gehen wir exakt vor. Die Anzahl der Dimensionen
  // bleibt bestehen. Die physische Grösse der Dimension kann jedoch kleiner
  // werden.
  // ----------------------------------------------------------------------
  int elems = getNumberOfElements();
  DataContainer::SizeList dimsize;
  int maxdims = 0;

  for(int i = elems-1; i >= 0 && maxdims == 0; i-- ){
    if( m_Containers[i] != 0 ){
      dimsize.clear();
      int numDim = m_Containers[i]->getNumberOfDimensions();
      if( numDim > 0 ){
        // DataContainer mit 0 Dimensionen geben uns mit GetAllDataDimensionSize()
        // nur 0 oder 1 zurück und interessieren sich nicht für den ersten
        // Parameter dimsize.
        dimsize.clear();
        for( int i=0; i < numDim; i++ ){
          dimsize.push_back( 0 );
        }
      }
      int tmp = m_Containers[i]->getAllDataDimensionSize( dimsize, 0, false );
      if( tmp > 0 ){
        maxdims = i + 1;
      }
    }
  }

  if( maxdims == 0 ){
    // Es gibt keine gueltigen Werte in dieser Matrix. Wir setzen maxdims
    // einfach auf 1. Somit wird ein ungueltiger Container kopiert.
    maxdims = 1;
  }

  DataDimension *dim = new DataDimension( m_NumDimensions, maxdims );
  assert( getNumberOfDimensions() > 0 );

  // ----------------------------------------------------------------
  // Nun werden die einzelnen DataContainer im Container kopiert. Die
  // Grösse des Containers im neu erstellten Objekt (maxdims) muss
  // nun korrekts sein.
  // ----------------------------------------------------------------
  ContainerList::iterator iDest;
  ContainerList::const_iterator iSrc = m_Containers.begin();
  for( iDest = dim->m_Containers.begin(); iDest != dim->m_Containers.end(); ++iDest ){
    assert( *iDest == 0 );
    if( *iSrc != 0 ){
      *iDest = (*iSrc)->copyContainer( datapool );
    }
    ++iSrc;
  }

  dim->markDataContainerValueUpdated();
  return dim;
}

/* --------------------------------------------------------------------------- */
/* clearContainer --                                                           */
/* --------------------------------------------------------------------------- */

DataDimension::UpdateStatus DataDimension::clearContainer( bool incl_itemattr ){
  BUG( BugRef, "clearContainer" );

  UpdateStatus status = NoUpdate;
  ContainerList::iterator it = m_Containers.begin();
  for( it = m_Containers.begin(); it != m_Containers.end(); ++it ){
    if( *it != 0 ){
      status = maximum( status, (*it)->clearContainer( incl_itemattr ) );
    }
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* eraseContainer --                                                           */
/* --------------------------------------------------------------------------- */

DataDimension::UpdateStatus DataDimension::eraseContainer(){
  // Dieser Aufruf ist nicht möglich
  assert( false );
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* setContainer --                                                             */
/* --------------------------------------------------------------------------- */

void DataDimension::setContainer( int index, DataContainer *cont ){
  setSizeOfContainer( index + 1 );
  if( m_Containers[index] != 0 ){
    delete m_Containers[index];
  }
  if( cont->isDataElement() ){
    assert( m_NumDimensions == 1 );
  }
  else{
    assert( m_NumDimensions > 1 );
  }
  m_Containers[index] = cont;
}

/* --------------------------------------------------------------------------- */
/* appendContainer --                                                          */
/* --------------------------------------------------------------------------- */

void DataDimension::appendContainer( DataContainer *cont ){
  m_Containers.push_back( cont );
}

/* --------------------------------------------------------------------------- */
/* writeXML --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataDimension::writeXML( std::ostream &ostr
                            , std::vector<std::string> &attrs
                            , bool is_structitem
                            , int level
                            , bool debug
                            , bool isScalar )
{
  return writeXML( ostr, attrs, m_Containers, is_structitem, level, debug );
}


/* --------------------------------------------------------------------------- */
/* writeXML --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataDimension::writeXML( std::ostream &ostr
                            , std::vector<std::string> &attrs
                            , ContainerList &containers
                            , bool is_structitem
                            , int level
                            , bool debug )
{
  int i = debug ? level : 0;

  if( containers.empty() ){
    indent( i, ostr ) << "</vector>\n";
    return true;
  }

  ContainerList::reverse_iterator rit;
  for( rit = containers.rbegin(); rit != containers.rend(); ++rit ){
    if( (*rit) != 0 ){
      if( (*rit)->hasValidElements( false ) ){
        break; // letzter gültiger Eintrag
      }
    }
  }
  if( rit == containers.rend() ){
    // Keine gültigen Daten vorhanden
    indent( i, ostr ) << "<vector/>\n";
    return true;
  }

  indent( i, ostr ) << "<vector>\n";

  ContainerList::iterator it;
  int n = 0;
  for( it = containers.begin(); it != containers.end(); ++it, ++n ){
    if( n == 10000 ){
      DataPool::getDataEvents().dispatchPendingEvents();
      n = 0;
    }
    if( (*it) != 0 ){
      (*it)->writeXML( ostr, attrs, is_structitem, 2 + level, debug, false );
      if( (*it) == (*rit) ){
        break; // Dies war der letzte gültige Eintrag
      }
    }
    else{
      if( is_structitem ){
        indent( i+2, ostr ) << "<contents/>\n";
      }
      else{
        if( m_NumDimensions > 1 ){
          indent( i+2, ostr ) << "<vector/>\n";
        }
        else{
          indent( i+2, ostr ) << "<v/>\n";
        }
      }
    }
  }

  indent( i, ostr ) << "</vector>\n";
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeJSON --                                                                */
/* --------------------------------------------------------------------------- */

bool DataDimension::writeJSON( std::ostream &ostr,
                               bool is_structitem,
                               int level,
                               int indentation,
                               bool isScalar,
                               double scale,
                               const SerializableMask flags )
{
  BUG_DEBUG("Begin of DataDimension::writeJSON");

  if( m_Containers.empty() ){
    ostr << ( isScalar ? "null" : "[]");
    BUG_DEBUG("End of DataDimension::writeJSON: Container is empty");
    return true;
  }

  // find the last valid item
  ContainerList::reverse_iterator rit;
  for( rit = m_Containers.rbegin(); rit != m_Containers.rend(); ++rit ){
    if( (*rit) != 0 ){
      if( (*rit)->hasValidElements( false ) ){
        break; // last valid item found
      }
    }
  }
  if( rit == m_Containers.rend() ){
    // no valid items found
    ostr << ( isScalar ? "null" : "[]") ;
    BUG_DEBUG("End of DataDimension::writeJSON: No valid items");
    return true;
  }

  if( !isScalar ){
    ostr << "[";
  }

  int i = (level + 1) * indentation;
  Separator sep( 0, ',', ']' );

  ContainerList::iterator it;
  int n = 0;
  for( it = m_Containers.begin(); it != m_Containers.end(); ++it, ++n ){
    if( n == 10000 ){
      DataPool::getDataEvents().dispatchPendingEvents();
      n = 0;
    }

    if( !isScalar ){
      BUG_DEBUG("newline");
      ostr << sep << std::endl;
      indent( i, ostr );
    }

    if( (*it) != 0 ){
      BUG_DEBUG("Element available");
      (*it)->writeJSON( ostr,
                        is_structitem,
                        level + 1,
                        indentation,
                        isScalar,
                        scale,
                        flags );
    }
    else{
      BUG_DEBUG("No Element");
      if( getNumberOfDimensions() > 1 && !isScalar ){
        ostr << "[]";
      }
      else{
        ostr << "null";
      }
    }
    if( (*it) == (*rit) || // Dies war der letzte gültige Eintrag
        isScalar // Nur 1. Eintrag schreiben
        ){
      break;
    }
  }
  if( !isScalar ){
    ostr << std::endl;
    indent(level * indentation, ostr) << "]";
  }
  BUG_DEBUG("End of DataDimension::writeJSON");
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeDP --                                                                  */
/* --------------------------------------------------------------------------- */

void DataDimension::writeDP( std::ostream &ostr, int i, int index ) const{
  indent( i, ostr ) << "<DIM dims=\"" << m_NumDimensions << "\"";
  if( index >= 0 ){
    ostr << " index=\"" << index << "\"";
  }
  if( m_is_compare_dimension ){
    ostr << " compare=\"1\"";
  }
  ostr << ">" << std::endl;
  int inx = 0;
  for( ContainerList::const_iterator it = m_Containers.begin(); it != m_Containers.end(); ++it ){
    if( (*it) != 0 ){
      (*it)->writeDP( ostr, i + 2, inx );
    }
    else{
      if( m_NumDimensions > 1 ){
        indent( i + 2, ostr ) << "<DIM";
        if( inx >= 0 ){
          ostr << " index=\"" << inx << "\"";
        }
        ostr << " />" << std::endl;
      }
      else{
        indent( i + 2, ostr ) << "<EL";
        if( inx >= 0 ){
          ostr << " index=\"" << inx << "\"";
        }
        ostr << "/>" << std::endl;
      }
    }
    inx++;
  }
  indent( i, ostr ) << "</DIM>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* hasValidElements --                                                         */
/* --------------------------------------------------------------------------- */

bool DataDimension::hasValidElements( bool incl_itemattr ) const{
  ContainerList::const_iterator it;
  for( it = m_Containers.begin(); it != m_Containers.end(); ++it ){
    if( *it != 0 ){
      if( (*it)->hasValidElements( incl_itemattr ) ){
        return true;
      }
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* hasSerializableValidElements --                                             */
/* --------------------------------------------------------------------------- */

bool DataDimension::hasSerializableValidElements( const SerializableMask flags ) const{
  ContainerList::const_iterator it;
  for( it = m_Containers.begin(); it != m_Containers.end(); ++it ){
    if( *it != 0 ){
      if( (*it)->hasSerializableValidElements( flags ) ){
        return true;
      }
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataDimension::isValid( int x ) const{
  if( x >= (int)m_Containers.size() ){
    return false;
  }
  if( m_Containers[x] == 0 ){
    return false;
  }
  return m_Containers[x]->hasValidElements( false );
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataDimension::UpdateStatus DataDimension::alterData( DataItem &item
                                                    , DataAlterFunctor &func
                                                    , DataInx &index )
{
  BUG_PARA( BugRef, "alterData"
          , "verbleibende Levels=" << index.remainingLevels() <<
            ", Indizes=" << index.printRemainingIndices() );

  UpdateStatus status = NoUpdate;

  if( index.isIndexListValid() ){
    BUG_MSG( "Index is valid: remaining indices=" << index.remainingIndicesInIndexList() <<
             ", dimensions=" << getNumberOfDimensions() );

    if( index.remainingIndicesInIndexList() < getNumberOfDimensions() ){
      // Es sind mehr Dimensionen als Indizes vorhanden. Wir folgen dem Container
      // mit Index 0 bis wir auf ein DataElement stossen. Die Funktion
      // getNumberOfDimensions() gibt immer eine Zahl > 0 zurück. Es gibt keine
      // Probleme ohne Indizes.
      BUG_MSG( "kein Index -> Container mit Index 0 folgen" );
      assert( m_Containers[0] != 0 );
      if( func.startDimension( *m_Containers[0], index ) ){
        status = m_Containers[0]->alterData( item, func, index );
      }
      if( func.updateElements() ){
        markDataContainerUpdated( status );
      }
      BUG_EXIT( "Status=" << status );
      return status;
    }
  }
  else{
    if( !item.isScalar() ){
      // Bei skalaren Items wird nicht mit Wildcards gearbeitet.
      if( func.setWildcardWithoutIndices() ){
        // Der Funktor zeigt uns an, dass in diesem Fall alle vorhandenen
        // Elemente über alle vorhandenen Dimensionen bearbeitet werden.
        BUG_MSG( "Indexlist ist leer und der Functor will alle Elemente => set wildcard" );
        index.setWildcard();
      }
      else
      if( index.isLastLevelFrozen() ){
        // Wir arbeiten uns über den adressierten Datenbereich hinaus. Dort
        // wird immer über alle vorhandenen Dimensionen nach Elementen gesucht.
        index.setWildcard();
      }
    }
  }

  if( index.isWildcard() ){
    // Bei einer Wildcard iterieren wir über den ganzen Container.
    BUG_MSG( "Iteration wegen Wildcard" );

    if( !isBigEnough( index.getLowerbound() + 1 ) ){
      // Bei Wildcards können nur vorhandene Container bearbeitet werden.
      // Ein Vergrösserung des Containers macht in der Regel keinen Sinn.
      if( !func.createWildcardElements( index.wildcardNumber() ) ){
        return NoUpdate; // kein update
      }
      // Damit in der anschliessenden for-Schlaufe mindestens ein
      // Durchlauf erfolgt (siehe upper).
      resizeContainer( index.getLowerbound() + 1 );
    }

    int lower = index.getLowerbound();
    int upper = maximum( lower, getValidDimensionSize( func.inclusiveValidItemAttributes() ) - 1 );

    int inx_relativ = 0;
    if( index.getUpperbound() > lower ){
      upper = minimum( upper, index.getUpperbound() );
    }

    int wildcard_number = index.wildcardNumber();
    func.startWithWildcard( wildcard_number, upper - lower + 1 );

    while( lower <= upper ){
      if( m_Containers[lower] == 0 ){
        if( func.createWildcardElements( index.wildcardNumber() ) ){
          installNewContainer( lower, item );
        }
      }
      index.setWildcardIndex( inx_relativ );

      DataInx tmp_index( index );
      tmp_index.nextIndex();

      if( m_Containers[lower] == 0 ){
        func.nullElement( tmp_index, getNumberOfDimensions() );
      }
      else{
        if( func.startDimension( *m_Containers[lower], tmp_index ) ){
          status = maximum( status, m_Containers[lower]->alterData( item, func, tmp_index ) );
        }
      }
      lower++;
      inx_relativ++;

      if( func.createWildcardElements( index.wildcardNumber() ) ){
        // Ein weiterer Durchgang wird gewünscht
        if( lower > upper ){
          // Der lowerbound aus dem Index wird in diesem Fall ignoriert.
          upper = lower;
          if( !isBigEnough( lower + 1 ) ){
            resizeContainer( lower + 1 );
          }
        }
      }
    }

    func.endWithWildcard( wildcard_number );

    if( func.updateElements() ){
      markDataContainerUpdated( status );
    }
    BUG_EXIT( "mit Wildcard" );
    return status;
  }

  int inx = index.getIndex();
  if( !isBigEnough( inx + 1 ) ){
    if( !func.createElements() ){
      // Es gibt kein Element zum Bearbeiten.
      BUG_EXIT( "Kein Element zum Bearbeiten vorhanden" );
      return NoUpdate;
    }
    resizeContainer( inx + 1 );
  }

  if( m_Containers[ inx ] == 0 ){
    // Kein Element oder Dimension vorhanden.
    if( !func.createElements() ){
      // Es gibt kein Element zum Bearbeiten.
      BUG_EXIT( "Kein Element zum Bearbeiten vorhanden" );
      return NoUpdate;
    }
    installNewContainer( inx, item );
  }

  assert( m_Containers[inx] != 0 );

  index.nextIndex();
  func.startOneElement();
  BUG_MSG( "call Element[" << inx << "]" );
  if( func.startDimension( *m_Containers[inx], index ) ){
    status = m_Containers[inx]->alterData( item, func, index );
  }
  BUG_MSG( "back from Element[" << inx << "]" );
  func.endOneElement();

  if( func.updateElements() ){
    markDataContainerUpdated( status );
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* getDataElement --                                                           */
/* --------------------------------------------------------------------------- */

DataElement &DataDimension::getDataElement( DataItem &item,
                                            DataIndexList &inxlist,
                                            int index ){
  BUG_DEBUG( "getDataElement (" << index << ")" );
  int remainingIndices = inxlist.getDimensions() - index;
  if ( remainingIndices < 0 ){
    remainingIndices = 0;
  }

  BUG_DEBUG( " -- dimensions=" << getNumberOfDimensions() <<
             ", remaining=" << remainingIndices );

  if( remainingIndices < getNumberOfDimensions() ){
    // Es sind mehr Dimensionen als Indizes vorhanden. Wir folgen dem Container
    // mit Index 0 bis wir auf ein DataElement stossen. Die Funktion
    // getNumberOfDimensions() gibt immer eine Zahl > 0 zurück. Es gibt keine
    // Probleme ohne Indizes.
    assert( m_Containers[0] != 0 );
    return m_Containers[0]->getDataElement( item, inxlist, index );
  }

  int inx = inxlist.getDimensions() == 0 ? 0 : inxlist.index( index ).getIndex();
  if( !isBigEnough( inx + 1 ) ){
    resizeContainer( inx + 1 );
  }

  if( m_Containers[ inx ] == 0 ){
    // Kein Element oder Dimension vorhanden.
    installNewContainer( inx, item );
  }

  assert( m_Containers[inx] != 0 );
  return m_Containers[inx]->getDataElement( item, inxlist, ++index );
}

/* --------------------------------------------------------------------------- */
/* getDataElement --                                                           */
/* --------------------------------------------------------------------------- */

DataElement &DataDimension::getDataElement( DataItem &item,
                                            int inx ){
  BUG_DEBUG( "getDataElement [" << inx << "]" );
  assert( isBigEnough( inx + 1 ) );

  if( m_Containers[ inx ] == 0 ){
    // Kein Element oder Dimension vorhanden.
    installNewContainer( inx, item );
  }

  DataElement *el = m_Containers[inx]->getDataElement();
  assert( el );
  return *el;
}

/* --------------------------------------------------------------------------- */
/* getDataContainer --                                                         */
/* --------------------------------------------------------------------------- */

DataContainer &DataDimension::getDataContainer( DataItem &item,
                                                DataIndexList &inxlist,
                                                int index,
                                                int sz )
{
  BUG_DEBUG( "DataDimension::getDataContainer (" << index << ")" );
  BUG_DEBUG( " -- Indices: " << inxlist.indices() );

  int remainingIndices = inxlist.getDimensions() - index;
  BUG_DEBUG( " -- dimensions=" << getNumberOfDimensions() <<
             ", remaining=" << remainingIndices );
  assert( remainingIndices > 0 );
  assert( inxlist.getDimensions() > 0 );

  if( remainingIndices < getNumberOfDimensions() ){
    assert( m_Containers[0] != 0 );
    return m_Containers[0]->getDataContainer( item, inxlist, index, sz );
  }

  assert( remainingIndices == getNumberOfDimensions() );
  if( getNumberOfDimensions() == 1 ){
    // Wir sind bei der letzten Dimension angelangt.
    if( !isBigEnough( sz ) ){
      resizeContainer( sz );
    }
    return *this;
  }

  int inx = inxlist.index( index ).getIndex();
  if( !isBigEnough( inx + 1 ) ){
    resizeContainer( inx + 1 );
  }

  if( m_Containers[ inx ] == 0 ){
    // Kein Element oder Dimension vorhanden.
    installNewContainer( inx, item );
  }
  assert( m_Containers[inx] != 0 );
  return m_Containers[inx]->getDataContainer( item, inxlist, ++index, sz );
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

DataCompareResult DataDimension::compare( const DataContainer &cont
                                        , int fall
                                        , int max_faelle )
{
  DataDimension *dim = this;
  if( dim->isCompareDimension() ){
    // Die Compare-Dimension muss als erstes übersprungen werden. Ob sie an dieser
    // Stelle vorhanden sein darf, wird nicht mehr geprüft. Es darf sich nicht um
    // ein DataItem mit Struktur-Elementen handeln.
    dim = dim->m_Containers[0]->getDataDimension();
  }

  while( dim != 0 && dim->getNumberOfDimensions() > cont.getNumberOfDimensions() ){
    ContainerList::iterator it = dim->m_Containers.begin();
    for( ++it; it != dim->m_Containers.end(); it++ ){
      if( *it != 0 ){
        (*it)->setCompareAttributesOfAllElements( DATAisDifferent, fall, max_faelle );
      }
    }
    if( dim->m_Containers[0]->getContainerType() == ContainerIsDimension ){
      dim = dim->m_Containers[0]->getDataDimension();
    }
    else{
      assert( dim->getNumberOfDimensions() == 1 );
      assert( cont.getNumberOfDimensions() == 0 );
      DataCompareResult rslt( dim->m_Containers[0]->compare( cont, fall, max_faelle ) );
      return rslt;
    }
  }

  // Nun sind wir auf derselben Ebene
  assert( dim->getNumberOfDimensions() == cont.getNumberOfDimensions() );
  if( dim->getNumberOfElements() < cont.getNumberOfElements() ){
    dim->resizeContainer( cont.getNumberOfElements() );
  }

  return compareDimensions( cont, fall, max_faelle );
}

/* --------------------------------------------------------------------------- */
/* compareDimensions --                                                        */
/* --------------------------------------------------------------------------- */

DataCompareResult DataDimension::compareDimensions( const DataContainer &cont
                                                  , int fall
                                                  , int max_faelle )
{
  const DataDimension *dim = cont.getDataDimension();
  assert( dim != 0 );

  DataCompareResult rslt;
  for( int inx = 0; inx < dim->getNumberOfElements(); inx++ ){
    if( container( inx ) == 0 ){
      if( dim->container( inx ) != 0 ){
        setContainer( dim->container( inx )->clone(), inx );
        container( inx )->clearContainer( false );
        container( inx )->setCompareAttributesOfAllElements( DATAisDifferent, fall, max_faelle );
        rslt.setResult( DATAisDifferent );
      }
      else{
        // Beide Container sind nicht vorhanden.
        rslt.setResult( DATAisEqual );
      }
    }
    else{
      if( dim->container( inx ) != 0 ){
        rslt.setResult( container( inx )->compare( *dim->container( inx ), fall, max_faelle ) );
      }
      else{
        container( inx )->setCompareAttributesOfAllElements( DATAisDifferent, fall, max_faelle );
        rslt.setResult( DATAisDifferent );
      }
    }
  } // end for

  // Wenn die this-DataDimension mehr Einträge hat als die Parameter-DataDimension,
  // wird der Rest einfach als 'ungleich' markiert.
  for( int inx = dim->getNumberOfElements(); inx < getNumberOfElements(); inx++ ){
    if( container( inx ) != 0 ){
      if( container( inx )->hasValidElements( false ) ){
        container( inx )->setCompareAttributesOfAllElements( DATAisLeftOnly, 0, max_faelle );
        rslt.setResult( DATAisLeftOnly );
      }
    }
  } // end for

  return rslt;
}

/* --------------------------------------------------------------------------- */
/* compareSorted --                                                            */
/* --------------------------------------------------------------------------- */

DataCompareResult DataDimension::compareSorted( const DataDimension &dim
                                              , int fall
                                              , int max_faelle )
{
  assert( getNumberOfDimensions() == 1 );
  assert( dim.getNumberOfDimensions() == 1 );

  DataCompareResult rslt;
  DataElementMap diff_map;
  for( ContainerList::iterator it = m_Containers.begin(); it != m_Containers.end(); it++ ){
    if( *it != 0 ){
      if( !diff_map.insert( (*it)->getDataElement() ) ){
        // Doppelte Einträge werden als 'ungleich' markiert.
        (*it)->setCompareAttributesOfAllElements( DATAisDifferent, fall, max_faelle );
        rslt.setResult( DATAisDifferent );
      }
    }
  }

  DataElementMap const_map;
  for( ContainerList::const_iterator it = dim.m_Containers.begin(); it != dim.m_Containers.end(); it++ ){
    if( *it != 0 ){
      if( !const_map.insert( (*it)->getDataElement() ) ){
        // Doppelte Einträge werden als 'ungleich' gemeldet, aber nicht weiter vermerkt.
        rslt.setResult( DATAisDifferent );
      }
    }
  }

  rslt.setResult( diff_map.compare( const_map, fall, max_faelle ) );

  if( const_map.append( *this, fall, max_faelle ) ){
    rslt.setResult( DATAisRightOnly );
  }

  return rslt;
}

/* --------------------------------------------------------------------------- */
/* setCompareAttributesOfAllElements --                                        */
/* --------------------------------------------------------------------------- */

bool DataDimension::setCompareAttributesOfAllElements( DATAAttributeMask mask
                                                     , int fall
                                                     , int max_faelle )
{
  bool rslt = false;
  for( ContainerList::iterator it = m_Containers.begin(); it != m_Containers.end(); it++ ){
    if( *it != 0 ){
      if( (*it)->setCompareAttributesOfAllElements( mask, fall, max_faelle ) ){
        rslt = true;
      }
    }
  }
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* isCompareDimension --                                                       */
/* --------------------------------------------------------------------------- */

void DataDimension::setCompareDimension(){
  m_is_compare_dimension = true;
}

/* --------------------------------------------------------------------------- */
/* isCompareDimension --                                                       */
/* --------------------------------------------------------------------------- */

bool DataDimension::isCompareDimension() const{
  return m_is_compare_dimension;
}

/* --------------------------------------------------------------------------- */
/* getPython --                                                                */
/* --------------------------------------------------------------------------- */

void *DataDimension::getPython(){
#if defined(HAVE_PYTHON)
  PyObject *list = PyList_New( 0 );
  PyObject *obj = 0;
  int sz = getValidDimensionSize();
  bool has_valid_elements = false;
  for( int i = 0; i < sz; i++ ){
    if( m_Containers[i] != 0 ){
      obj = (PyObject*)m_Containers[i]->getPython();
      if( obj != Py_None ){
        has_valid_elements = true;
      }
      PyList_Append( list, obj );
    }
    else{
      PyList_Append( list, PythonObject::getEmptyObject() );
    }
  }
  if( has_valid_elements ){
    return list;
  }

  // Die leere oder ungültige Liste wird wieder verworfen.
  Py_DECREF( list );
  return PythonObject::getEmptyObject();
#else
  return 0;
#endif
}

/* --------------------------------------------------------------------------- */
/* setPython --                                                                */
/* --------------------------------------------------------------------------- */

DataDimension::UpdateStatus DataDimension::setPython( const PythonObject &obj ){
  assert( false );
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* setPython --                                                                */
/* --------------------------------------------------------------------------- */

DataDimension::UpdateStatus DataDimension::setPython( const PythonObject &obj
                                                    , DataItem &item
                                                    , int dims )
{
#if defined(HAVE_PYTHON)
  assert( dims > 0 );
  BUG_DEBUG( "DataDimension::setPython: dims=" << dims );

  if( !obj.hasPyObject() ){
    // bei leeren Objekten machen wir nichts.
    return NoUpdate;
  }

  UpdateStatus status = NoUpdate;

  if( PyList_Check( obj ) ){
    Py_ssize_t sz = PyList_Size( obj );
    resizeContainer( (int)sz );
    for( Py_ssize_t x = 0; x < sz; x++ ){
      PythonObject sub_obj( PyList_GetItem( obj, x ) );
      DataContainer *cont = container( x );
      if( cont == 0 ){
        installNewContainer( x, item );
        cont = container( x );
      }
      status = maximum( cont->setPython( sub_obj, item, dims-1 ), status );
    }
  }
  else
  if( PyTuple_Check( obj ) ){
    Py_ssize_t sz = PyTuple_Size( obj );
    resizeContainer( (int)sz );
    for( Py_ssize_t x = 0; x < sz; x++ ){
      PythonObject sub_obj( PyTuple_GetItem( obj, x ) );
      DataContainer *cont = container( x );
      if( cont == 0 ){
        installNewContainer( x, item );
        cont = container( x );
      }
      status = maximum( cont->setPython( sub_obj, item, dims-1 ), status );
    }
  }
  else{
    // Im DataDimension-Objekt können nur Listen verarbeitet werden. Nicht passende
    // Python-Objekte werden ignoriert.
    status = NoUpdate;
  }
  return status;
#else
  return NoUpdate;
#endif
}
