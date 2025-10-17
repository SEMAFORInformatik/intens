
#include "plot/Gdev.h"
#include "plot/Frame.h"

/* --------------------------------------------------------------------------- */
/* drawFrame --                                                                */
/* --------------------------------------------------------------------------- */

void Frame::drawFrame( Gdev *gdev ) {
  if( gdev == 0 )
    return;
  gdev->setViewport( x0,y0,x1,y1 );
  gdev->setWindow( 0,0,1,1 );
  gdev->setLinestyle( 0 );
  gdev->setLinewidth( 0 );
  gdev->setColor( 0 );
  gdev->moveTo( 0,0 ); gdev->drawTo( 0,1 );
  gdev->moveTo( 0,1 ); gdev->drawTo( 1,1 );
  gdev->moveTo( 1,1 ); gdev->drawTo( 1,0 );
  gdev->moveTo( 1,0 ); gdev->drawTo( 0,0 );
}
