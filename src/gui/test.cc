#include <cstdio>
#include <locale.h>

int main( int argc, char** argv) {
  double value(1 * 1.0);
  // std::string lc_num = setlocale(LC_NUMERIC, NULL);
  setlocale(LC_NUMERIC, argv[1]);
  fprintf(stdout, "%#.16g y(%s)\n", value, argv[1]);
  char buf[100];
  sprintf(buf, "%#.16g", value);
  fprintf(stdout, "(%s)\n", buf);
  setlocale(LC_NUMERIC, "C");
  fprintf(stdout, "%#.16g\n", value);
  // setlocale(LC_NUMERIC, lc_num.c_str());
}
