#ifndef UTILS_STRING_COMPOSE_PRIVATE_H
#define UTILS_STRING_COMPOSE_PRIVATE_H

#include <map>
#include <list>
#include <string>
#include <sstream>

namespace ComposePrivate
{
  // This class composition provides string::compose functionality
  class ComposeString{
  public:
    /**
       Constructor with format string on the form "text %1 text %2 etc."
       @param fmt format string like "textA %1 textB %2 textC."
    */
    explicit ComposeString(std::string fmt);

    /**
       argument function to replace argument in format string starting with %1
       @param obj argument
     */
    template <typename T>
    ComposeString &arg(const T &obj);

    /** returns the result string
     */
    std::string str() const;

  private:
    std::ostringstream os;
    int arg_no;

    typedef std::list<std::string> output_list;
    output_list output;

    typedef std::multimap<int, output_list::iterator> specification_map;
    specification_map specs;
  };

  inline int char_to_int(char c){
    switch (c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    default: return -1000;
    }
  }

  inline bool is_number(int n){
    switch (n) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return true;
    default:
      return false;
    }
  }

  // class ComposeString
  template <typename T>
  inline ComposeString &ComposeString::arg(const T &obj){
    os << obj;

    std::string rep = os.str();
    if (!rep.empty()) {	// manipulators do not produce output
      for (specification_map::const_iterator i = specs.lower_bound(arg_no),
             end = specs.upper_bound(arg_no); i != end; ++i) {
        output_list::iterator pos = i->second;
        ++pos;
        output.insert(pos, rep);
      }
      os.str(std::string());
      //os.clear();
      ++arg_no;
    }
    return *this;
  }

  inline ComposeString::ComposeString(std::string fmt)
    : arg_no(1){
    std::string::size_type b = 0, i = 0;

    // fill in output with the strings between the %1 %2 %3 etc. and
    // fill in specs with the positions
    while (i < fmt.length()) {
      if (fmt[i] == '%' && i + 1 < fmt.length()) {
        if (fmt[i + 1] == '%') {	// catch %%
          fmt.replace(i, 2, "%");
          ++i;
        }
        else if (is_number(fmt[i + 1])) { // aha! a spec!
          // save string
          output.push_back(fmt.substr(b, i - b));
          int n = 1;	// number of digits
          int spec_no = 0;

          do {
            spec_no += char_to_int(fmt[i + n]);
            spec_no *= 10;
            ++n;
          } while (i + n < fmt.length() && is_number(fmt[i + n]));

          spec_no /= 10;
          output_list::iterator pos = output.end();
          --pos;		// safe since we have just inserted a string>
          specs.insert(specification_map::value_type(spec_no, pos));

          // jump over spec string
          i += n;
          b = i;
        }
        else
          ++i;
      }
      else
        ++i;
    }
    if (i - b > 0)	// append rest of the string
      output.push_back(fmt.substr(b, i - b));
  }

  inline std::string ComposeString::str() const {
    // collect string
    std::string str;
    for (output_list::const_iterator i = output.begin(), end = output.end();
         i != end; ++i)
      str += *i;
    return str;
  }
}

#endif
