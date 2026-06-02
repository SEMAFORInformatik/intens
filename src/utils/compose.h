// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#ifndef STRING_COMPOSE_H
#define STRING_COMPOSE_H

#include <format>
#include <string>

namespace SemaforString{
  /** replaces %<num> to {<num>-1} , e.g %1 => {0} */
  std::string convert2StdFormat(const std::string& format);

  /** compose with 1 arguments */
  template <typename T1>
  inline static std::string compose(const std::string &fmt, const T1 &o1){
    return std::vformat(convert2StdFormat(fmt), std::make_format_args(o1));
  }

  /** compose with 2 arguments */
  template <typename T1, typename T2>
  inline static std::string compose(const std::string &fmt,
                             const T1 &o1, const T2 &o2){
    return std::vformat(convert2StdFormat(fmt), std::make_format_args(o1, o2));
  }

  /** compose with 3 arguments */
  template <typename T1, typename T2, typename T3>
  inline static std::string compose(const std::string &fmt,
                             const T1 &o1, const T2 &o2, const T3 &o3)
  {
    return std::vformat(convert2StdFormat(fmt), std::make_format_args(o1, o2, o3));
  }

  /** compose with 4 arguments */
  template <typename T1, typename T2, typename T3, typename T4>
  inline static std::string compose(const std::string &fmt,
                             const T1 &o1, const T2 &o2, const T3 &o3,
                             const T4 &o4){
    return std::vformat(convert2StdFormat(fmt), std::make_format_args(o1, o2, o3, o4));
  }

  /** compose with 5 arguments */
  template <typename T1, typename T2, typename T3, typename T4, typename T5>
  inline static std::string compose(const std::string &fmt,
                             const T1 &o1, const T2 &o2, const T3 &o3,
                             const T4 &o4, const T5 &o5){
    return std::vformat(convert2StdFormat(fmt),
                        std::make_format_args(o1, o2, o3, o4, o5));
  }

  /** compose with 6 arguments */
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6>
  inline static std::string compose(const std::string &fmt,
                                    const T1 &o1, const T2 &o2, const T3 &o3,
                                    const T4 &o4, const T5 &o5, const T6 &o6){
    return std::vformat(std::string(fmt),
                        std::make_format_args(o1, o2, o3, o4, o5,
                                              o6));
  }

  /** compose with 7 arguments */
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7>
  inline static std::string compose(const std::string &fmt,
                                    const T1 &o1, const T2 &o2, const T3 &o3,
                                    const T4 &o4, const T5 &o5, const T6 &o6,
                                    const T7 &o7){
    return std::vformat(convert2StdFormat(fmt),
                        std::make_format_args(o1, o2, o3, o4, o5,
                                              o6, o7));
  }

  /** compose with 8 arguments */
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8>
  inline static std::string compose(const std::string &fmt,
                                    const T1 &o1, const T2 &o2, const T3 &o3,
                                    const T4 &o4, const T5 &o5, const T6 &o6,
                                    const T7 &o7, const T8 &o8){
    return std::vformat(convert2StdFormat(fmt),
                        std::make_format_args(o1, o2, o3, o4, o5,
                                              o6, o7, o8));
  }

  /** compose with 9 arguments */
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9>
  inline static std::string compose(const std::string &fmt,
                                    const T1 &o1, const T2 &o2, const T3 &o3,
                                    const T4 &o4, const T5 &o5, const T6 &o6,
                                    const T7 &o7, const T8 &o8, const T9 &o9){
    return std::vformat(convert2StdFormat(fmt),
                        std::make_format_args(o1, o2, o3, o4, o5,
                                              o6, o7, o8, o9));
  }

  /** compose with 10 arguments */
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10>
  inline static std::string compose(const std::string &fmt,
                                    const T1 &o1, const T2 &o2, const T3 &o3,
                                    const T4 &o4, const T5 &o5, const T6 &o6,
                                    const T7 &o7, const T8 &o8, const T9 &o9,
                                    const T10 &o10){
    return std::vformat(convert2StdFormat(fmt),
                        std::make_format_args(o1, o2, o3, o4, o5,
                                              o6, o7, o8, o9, o10));
  }

  /** compose with 11 arguments */
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11>
  inline static std::string compose(const std::string &fmt,
                                    const T1 &o1, const T2 &o2, const T3 &o3,
                                    const T4 &o4, const T5 &o5, const T6 &o6,
                                    const T7 &o7, const T8 &o8, const T9 &o9,
                                    const T10 &o10, const T11 &o11){
    return std::vformat(convert2StdFormat(fmt),
                        std::make_format_args(o1, o2, o3, o4, o5,
                                              o6, o7, o8, o9, o10,
                                              o11));
  }

  /** compose with 12 arguments */
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12>
  inline static std::string compose(const std::string &fmt,
                                    const T1 &o1, const T2 &o2, const T3 &o3,
                                    const T4 &o4, const T5 &o5, const T6 &o6,
                                    const T7 &o7, const T8 &o8, const T9 &o9,
                                    const T10 &o10, const T11 &o11, const T12 &o12){
    return std::vformat(convert2StdFormat(fmt),
                        std::make_format_args(o1, o2, o3, o4, o5,
                                              o6, o7, o8, o9, o10,
                                              o11, o12));
  }

  /** compose with 13 arguments */
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13>
  inline static std::string compose(const std::string &fmt,
                                    const T1 &o1, const T2 &o2, const T3 &o3,
                                    const T4 &o4, const T5 &o5, const T6 &o6,
                                    const T7 &o7, const T8 &o8, const T9 &o9,
                                    const T10 &o10, const T11 &o11, const T12 &o12,
                                    const T13 &o13){
    return std::vformat(convert2StdFormat(fmt),
                        std::make_format_args(o1, o2, o3, o4, o5,
                                              o6, o7, o8, o9, o10,
                                              o11, o12, o13));
  }

  /** compose with 14 arguments */
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14>
  inline static std::string compose(const std::string &fmt,
                                    const T1 &o1, const T2 &o2, const T3 &o3,
                                    const T4 &o4, const T5 &o5, const T6 &o6,
                                    const T7 &o7, const T8 &o8, const T9 &o9,
                                    const T10 &o10, const T11 &o11, const T12 &o12,
                                    const T13 &o13, const T14 &o14){
    return std::vformat(convert2StdFormat(fmt),
                        std::make_format_args(o1, o2, o3, o4, o5,
                                              o6, o7, o8, o9, o10,
                                              o11, o12, o13, o14));
  }

  /** compose with 15 arguments */
  template <typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15>
  inline static std::string compose(const std::string &fmt,
                                    const T1 &o1, const T2 &o2, const T3 &o3,
                                    const T4 &o4, const T5 &o5, const T6 &o6,
                                    const T7 &o7, const T8 &o8, const T9 &o9,
                                    const T10 &o10, const T11 &o11, const T12 &o12,
                                    const T13 &o13, const T14 &o14, const T15 &o15){
    return std::vformat(convert2StdFormat(fmt),
                        std::make_format_args(o1, o2, o3, o4, o5,
                                              o6, o7, o8, o9, o10,
                                              o11, o12, o13, o14, o15));
  }
}

#endif // STRING_COMPOSE_H
