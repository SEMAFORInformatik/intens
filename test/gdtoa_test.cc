
#include <format>
#include <numbers> // std::numbers
#include <cmath> // log10, isnan, isinf

#include "xfer/gdtoa/gdtoa.h"
#include <gtest/gtest.h>

char *xdtoa(double d, int mode, int ndigits, int *decpt, int *sign, const char**rve){
 /*	returns string representing the number d. trailing zeros are suppressed.
    If not null, *rve is set to point to the end of the return value.
    If d is +-Infinity or NaN, then *decpt is set to 9999.

	mode:
		0 ==> shortest string that yields d when read in
			and rounded to nearest.
		3 ==> through ndigits past the decimal point.
        ndigits can be negative.

    NOTE: this function is a replacement of dtoa from
         https://github.com/jwiegley/gdtoa
 */
  std::string s;
  if (std::isnan(d) || std::isinf(d)){
    *decpt = 9999;
    if(std::isinf(d)){
      s = "Infinity";
    }
    else {
      s = "NaN";
    }
  }
  else {
    if(!d){
      s = "0";
    }
    else {
      int decptoffset = int(log10(abs(d)));
      if(mode == 3){
        ndigits -= decptoffset;
      }
      else {
        ndigits = 15 - decptoffset;
      }
      if(d<1){
        ndigits += 2;
      }
      s = std::format("{:.{}f}", d, ndigits);
      *sign = 0;
      if(d<0){
        *sign = 1;
        s.erase(0, 1);
      }
      *decpt = s.find(".");
      if( *decpt != s.npos){
        s.erase(*decpt, 1);
      }
      else {
        *decpt = s.size();
      }
      // remove trailing 0
      s.erase(s.find_last_not_of("0") + 1);
      // remove leading 0
      int origsize = s.size();
      s.erase(0, s.find_first_not_of("0"));
      *decpt -= origsize - s.size();
    }
  }
  char *ret = strdup(s.c_str());
  *rve = ret + s.size();
  return ret;
}

TEST(GdtoaTest, double2stringtest)
{
  int nprec = 9;
  char *se;
  const char *sec;
  char *actual;
  int decpt, sign;

  double pi = std::numbers::pi;

  actual = dtoa(pi, 3, nprec, &decpt, &sign, &se);
  ASSERT_EQ("3141592654", std::string(actual));
  ASSERT_EQ(1, decpt);
  ASSERT_EQ(0, sign);

  actual = xdtoa(pi, 3, nprec, &decpt, &sign, &sec);
  ASSERT_EQ("3141592654", std::string(actual));
  ASSERT_EQ(1, decpt);
  ASSERT_EQ(0, sign);
  free(actual);

  actual = dtoa(pi, 0, 0, &decpt, &sign, &se);
  ASSERT_EQ("3141592653589793", std::string(actual));
  ASSERT_EQ(1, decpt);
  ASSERT_EQ(0, sign);

  actual = dtoa(pi*1e3, 0, nprec, &decpt, &sign, &se);
  ASSERT_EQ("3141592653589793", std::string(actual));
  ASSERT_EQ(4, decpt);
  ASSERT_EQ(0, sign);

  actual = xdtoa(pi*1e3, 0, nprec, &decpt, &sign, &sec);
  ASSERT_EQ("3141592653589793", std::string(actual));
  ASSERT_EQ(4, decpt);
  ASSERT_EQ(0, sign);
  free(actual);

  actual = dtoa(pi*1e-3, 0, nprec, &decpt, &sign, &se);
  ASSERT_EQ("31415926535897933", std::string(actual));
  ASSERT_EQ(-2, decpt);
  ASSERT_EQ(0, sign);

  actual = xdtoa(pi*1e-3, 0, nprec, &decpt, &sign, &sec);
  ASSERT_EQ("31415926535897933", std::string(actual));
  ASSERT_EQ(-2, decpt);
  ASSERT_EQ(0, sign);
  free(actual);

  pi = 3.14;
  char *cp = dtoa(pi, 3, nprec, &decpt, &sign, &se);
  ASSERT_EQ("314", std::string(cp));
  ASSERT_EQ(1, decpt);
  ASSERT_EQ(0, sign);
  ASSERT_EQ(3, se-cp);

  cp = xdtoa(pi, 3, nprec, &decpt, &sign, &sec);
  ASSERT_EQ("314", std::string(cp));
  ASSERT_EQ(1, decpt);
  ASSERT_EQ(0, sign);
  ASSERT_EQ(3, sec-cp);
  free(cp);

  cp = dtoa(-pi, 3, nprec, &decpt, &sign, &se);
  ASSERT_EQ("314", std::string(cp));
  ASSERT_EQ(1, decpt);
  ASSERT_EQ(1, sign);
  ASSERT_EQ(3, se-cp);

  cp = xdtoa(-pi, 3, nprec, &decpt, &sign, &sec);
  ASSERT_EQ("314", std::string(cp));
  ASSERT_EQ(1, decpt);
  ASSERT_EQ(1, sign);
  ASSERT_EQ(3, sec-cp);
  free(cp);

  double x = 0.123456789;
  cp = dtoa(x, 3, nprec, &decpt, &sign, &se);
  ASSERT_EQ("123456789", std::string(cp));
  ASSERT_EQ(0, decpt);
  ASSERT_EQ(0, sign);
  ASSERT_EQ(9, se-cp);

  cp = xdtoa(x, 3, nprec, &decpt, &sign, &sec);
  ASSERT_EQ("123456789", std::string(cp));
  ASSERT_EQ(0, decpt);
  ASSERT_EQ(0, sign);
  ASSERT_EQ(9, sec-cp);
  free(cp);

  x = 1234500;
  cp = dtoa(x, 3, nprec, &decpt, &sign, &se);
  ASSERT_EQ("12345", std::string(cp));
  ASSERT_EQ(7, decpt);
  ASSERT_EQ(0, sign);
  ASSERT_EQ(5, se-cp);

  cp = xdtoa(x, 3, nprec, &decpt, &sign, &sec);
  ASSERT_EQ("12345", std::string(cp));
  ASSERT_EQ(7, decpt);
  ASSERT_EQ(0, sign);
  ASSERT_EQ(5, sec-cp);
  free(cp);

  x = 0.0;
  cp = dtoa(x, 3, nprec, &decpt, &sign, &se);
  ASSERT_EQ("0", std::string(cp));
  ASSERT_EQ(1, decpt);
  ASSERT_EQ(0, sign);
  ASSERT_EQ(1, se-cp);

  cp = xdtoa(x, 3, nprec, &decpt, &sign, &sec);
  ASSERT_EQ("0", std::string(cp));
  ASSERT_EQ(1, decpt);
  ASSERT_EQ(0, sign);
  ASSERT_EQ(1, sec-cp);
  free(cp);
}

TEST(GdtoaTest, naninf2stringtest)
{
  int nprec = 9;
  char *se;
  const char *sec;
  char *actual;
  int decpt, sign;

  double inf = std::numeric_limits<double>::infinity();
  double nan = std::numeric_limits<double>::quiet_NaN();

  actual = dtoa(inf, 3, nprec, &decpt, &sign, &se);
  ASSERT_EQ("Infinity", std::string(actual));
  ASSERT_EQ(9999, decpt);
  ASSERT_EQ(0, sign);

  actual = xdtoa(inf, 3, nprec, &decpt, &sign, &sec);
  ASSERT_EQ("Infinity", std::string(actual));
  ASSERT_EQ(9999, decpt);
  ASSERT_EQ(0, sign);
  free(actual);

  actual = dtoa(nan, 3, nprec, &decpt, &sign, &se);
  ASSERT_EQ("NaN", std::string(actual));
  ASSERT_EQ(9999, decpt);
  ASSERT_EQ(0, sign);

  actual = xdtoa(nan, 3, nprec, &decpt, &sign, &sec);
  ASSERT_EQ("NaN", std::string(actual));
  ASSERT_EQ(9999, decpt);
  ASSERT_EQ(0, sign);
  free(actual);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
