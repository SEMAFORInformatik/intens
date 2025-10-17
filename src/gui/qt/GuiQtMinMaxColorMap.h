
#include <vector>

#include <QRgb>

// ******************************************************************
// Class ColorScale
// ******************************************************************
class ColorScale{
public:
  static ColorScale* Instance();
protected:
  // ------------------------
  // Constructor / Destructor
  // ------------------------
  ColorScale();
  ColorScale(const ColorScale&);
  ColorScale& operator=(const ColorScale&);
  virtual ~ColorScale(){ }

  // ------------------------
  // public functions
  // ------------------------
public:
  const std::vector<QRgb> & getColors(int channel);
  // -------------------------
  // private members
  // -------------------------
private:
  static ColorScale* pinstance;
  std::vector<std::vector<QRgb> > m_colors;
};


// Functor for color calculation
//
class ColorMap {
public:
  virtual unsigned int operator()( double value )const = 0;
};

// ******************************************************************
// Class MinMaxColorMap
// ******************************************************************
// Functor for color calculation based in min/max
class MinMaxColorMap: public ColorMap {
public:
  MinMaxColorMap( double min, double max, int channel );

  virtual unsigned int operator()( double value )const;
private:
  double _min,_max,_d;
  const std::vector<QRgb> & _color;
};
