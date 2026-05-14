#ifndef UTILS_STRING_COMPOSE_H
#define UTILS_STRING_COMPOSE_H

#include "utils/compose_p.h"

namespace SemaforString
{
  // list of functions which accept a format string on the form "textA %1
  // textB %2 textC %3" and a number of templated parameters and spits out the
  // composited string
  template <typename T1>
  inline std::string compose(const std::string &fmt, const T1 &o1)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1);
    return c.str();
  }

  template <typename T1, typename T2>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2);
    return c.str();
  }

  template <typename T1, typename T2, typename T3>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3);
    return c.str();
  }

  template <typename T1, typename T2, typename T3, typename T4>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3,
			     const T4 &o4)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3).arg(o4);
    return c.str();
  }

  template <typename T1, typename T2, typename T3, typename T4, typename T5>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3,
			     const T4 &o4, const T5 &o5)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3).arg(o4).arg(o5);
    return c.str();
  }

  template <typename T1, typename T2, typename T3, typename T4, typename T5,
	    typename T6>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3,
			     const T4 &o4, const T5 &o5, const T6 &o6)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3).arg(o4).arg(o5).arg(o6);
    return c.str();
  }

  template <typename T1, typename T2, typename T3, typename T4, typename T5,
	    typename T6, typename T7>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3,
			     const T4 &o4, const T5 &o5, const T6 &o6,
			     const T7 &o7)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3).arg(o4).arg(o5).arg(o6).arg(o7);
    return c.str();
  }

  template <typename T1, typename T2, typename T3, typename T4, typename T5,
	    typename T6, typename T7, typename T8>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3,
			     const T4 &o4, const T5 &o5, const T6 &o6,
			     const T7 &o7, const T8 &o8)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3).arg(o4).arg(o5).arg(o6).arg(o7).arg(o8);
    return c.str();
  }

  template <typename T1, typename T2, typename T3, typename T4, typename T5,
	    typename T6, typename T7, typename T8, typename T9>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3,
			     const T4 &o4, const T5 &o5, const T6 &o6,
			     const T7 &o7, const T8 &o8, const T9 &o9)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3).arg(o4).arg(o5).arg(o6).arg(o7).arg(o8).arg(o9);
    return c.str();
  }

  template <typename T1, typename T2, typename T3, typename T4, typename T5,
	    typename T6, typename T7, typename T8, typename T9, typename T10>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3,
			     const T4 &o4, const T5 &o5, const T6 &o6,
			     const T7 &o7, const T8 &o8, const T9 &o9,
			     const T10 &o10)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3).arg(o4).arg(o5).arg(o6).arg(o7).arg(o8).arg(o9)
      .arg(o10);
    return c.str();
  }

  template <typename T1, typename T2, typename T3, typename T4, typename T5,
	    typename T6, typename T7, typename T8, typename T9, typename T10,
	    typename T11>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3,
			     const T4 &o4, const T5 &o5, const T6 &o6,
			     const T7 &o7, const T8 &o8, const T9 &o9,
			     const T10 &o10, const T11 &o11)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3).arg(o4).arg(o5).arg(o6).arg(o7).arg(o8).arg(o9)
      .arg(o10).arg(o11);
    return c.str();
  }

  template <typename T1, typename T2, typename T3, typename T4, typename T5,
	    typename T6, typename T7, typename T8, typename T9, typename T10,
	    typename T11, typename T12>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3,
			     const T4 &o4, const T5 &o5, const T6 &o6,
			     const T7 &o7, const T8 &o8, const T9 &o9,
			     const T10 &o10, const T11 &o11, const T12 &o12)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3).arg(o4).arg(o5).arg(o6).arg(o7).arg(o8).arg(o9)
      .arg(o10).arg(o11).arg(o12);
    return c.str();
  }

  template <typename T1, typename T2, typename T3, typename T4, typename T5,
	    typename T6, typename T7, typename T8, typename T9, typename T10,
	    typename T11, typename T12, typename T13>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3,
			     const T4 &o4, const T5 &o5, const T6 &o6,
			     const T7 &o7, const T8 &o8, const T9 &o9,
			     const T10 &o10, const T11 &o11, const T12 &o12,
			     const T13 &o13)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3).arg(o4).arg(o5).arg(o6).arg(o7).arg(o8).arg(o9)
      .arg(o10).arg(o11).arg(o12).arg(o13);
    return c.str();
  }

  template <typename T1, typename T2, typename T3, typename T4, typename T5,
	    typename T6, typename T7, typename T8, typename T9, typename T10,
	    typename T11, typename T12, typename T13, typename T14>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3,
			     const T4 &o4, const T5 &o5, const T6 &o6,
			     const T7 &o7, const T8 &o8, const T9 &o9,
			     const T10 &o10, const T11 &o11, const T12 &o12,
			     const T13 &o13, const T14 &o14)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3).arg(o4).arg(o5).arg(o6).arg(o7).arg(o8).arg(o9)
      .arg(o10).arg(o11).arg(o12).arg(o13).arg(o14);
    return c.str();
  }

  template <typename T1, typename T2, typename T3, typename T4, typename T5,
	    typename T6, typename T7, typename T8, typename T9, typename T10,
	    typename T11, typename T12, typename T13, typename T14,
	    typename T15>
  inline std::string compose(const std::string &fmt,
			     const T1 &o1, const T2 &o2, const T3 &o3,
			     const T4 &o4, const T5 &o5, const T6 &o6,
			     const T7 &o7, const T8 &o8, const T9 &o9,
			     const T10 &o10, const T11 &o11, const T12 &o12,
			     const T13 &o13, const T14 &o14, const T15 &o15)
  {
    ComposePrivate::ComposeString c(fmt);
    c.arg(o1).arg(o2).arg(o3).arg(o4).arg(o5).arg(o6).arg(o7).arg(o8).arg(o9)
      .arg(o10).arg(o11).arg(o12).arg(o13).arg(o14).arg(o15);
    return c.str();
  }
}
#endif // UTILS_STRING_COMPOSE_H
