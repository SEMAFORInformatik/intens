
#ifndef FRAME_H
#define FRAME_H

class Gdev;

class Frame {
 public:
  Frame()
    : x0( 0.0 )
    , y0( 0.0 )
    , x1( 1.0 )
    , y1( 1.0 ) {
  }
  Frame( double tx0,
	 double ty0,
	 double tx1,
	 double ty1 )
    : x0( tx0 )
    , y0( ty0 )
    , x1( tx1 )
    , y1( ty1 ) {
  }
  Frame( const Frame &frame )
    : x0( frame.x0 )
    , y0( frame.y0 )
    , x1( frame.x1 )
    , y1( frame.y1 ) {
  }
  void drawFrame( Gdev *gdev );

  double x0,y0,x1,y1;
};

#endif
