/*
 * main.cpp
 *
 *  Created on: Jun 29, 2017
 *      Author: Matthew Gregory Krupa
 *   Copyright: Matthew Gregory Krupa
 */
/*============================================================================
 *  Name        : main.cpp
 *  Author      : Matthew Gregory Krupa
 *  Version     : 1.0
 *  Copyright   : Any thing that's neither public domain nor already
 *                 owned by someone (as of March 24, 2017), I hereby copywrite
 *                 to the maximum extent allowed by law. :)
 *  Description : The primary algorithm of interest here would be
 *                MergeWithOutBuffer()
 *  In general, the larger the size of the vector, the bigger the difference in
 *   speed between my algorithms and the corresponding std:: algorithms.
 *============================================================================
 */

#define MALLOC_CHECK_ 2

#include <algorithm>
#include <cassert>
#include <chrono>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <vector>

struct TestingOptions {
  typedef std::size_t SizeType;
  bool also_check_correctness_of_merge = true;
  static constexpr bool should_verify_merge_stability = true;
  static constexpr bool update_smallest_and_largest_ratio_for_each_vector = false;
  SizeType number_of_random_vec_sizes = 0;//(1u << 3);
  SizeType verify_merge_stability_num_tests_per_vec_size = (1u << 5);

  long double ratio_of_max_value_to_vec_size = 1.2l;
  //If the above is <= 0 then the random vector's
  // values are independent of the vector's size.
  //Pick the random vector values in the range
  // [0, ratio_of_max_value_to_vec_size * vec_size].
  //This is useful for controlling how many repeated
  // values the randomly generated vectors have.

  bool use_denormal_numbers = false; //This is only used with floating points.
  bool should_randomly_pick_start_right = true;

  //The following is used in GetLength_of_left_subvector_lambda().
  bool should_pick_new_random_length_for_each_new_vec = true;
  long double length_left_fraction_of_vec_size = 0.5l;
  bool should_return_initial_length_left_for_all_subsequence_calls = false;

  enum ContainerTypeEnum {
    vector_type = 0,
    deque_type,
    list_type
  } container_type_;

  static const std::vector<std::string> container_type_string_;


  //These values are used by the lambdas in mins_maxs_and_lambda.h
  long double vec_size_scale = 1.7l;

  SizeType vec_size_start = (1u << 15);
  SizeType vec_size_end   = (1u << 22);

  SizeType minimum_vec_size = static_cast<SizeType>(1u << 1);
  SizeType maximum_vec_size = static_cast<SizeType>(1u << 20);

  SizeType minimum_num_tests_per_vec_size  = static_cast<SizeType>(1u << 4);
  SizeType maximum_num_tests_per_vec_size  = static_cast<SizeType>(1u << 17);

  SizeType desired_num_repetitions         = static_cast<SizeType>(1u << 3);
  SizeType minimum_num_repititions_per_vec = static_cast<SizeType>(1u << 2);
  SizeType maximum_num_repititions_per_vec = static_cast<SizeType>(1u << 6);

  SizeType minimum_elements_to_process = vec_size_end
                                       * minimum_num_tests_per_vec_size
                                       * minimum_num_repititions_per_vec;
  SizeType maximum_elements_to_process = vec_size_end
                                       * minimum_num_tests_per_vec_size
                                       * maximum_num_repititions_per_vec;

  //Printing to file/std::cout options:
  static std::ostringstream ostrstrm;
  bool should_print_to_file = true;
  bool also_print_to_std_cout = true;
  bool also_print_grand_totals_of_each_container_to_separate_file = true;
  bool print_info_string_for_each_vector = false;
  bool print_average_time_for_each_vec_size = true;
  bool print_average_time_for_each_vec_size_divide_by_total_num_calls = false;
  bool print_average_time_for_each_vec_size_divide_by_num_repetitions = true;
  bool print_total_average_time_for_each_vec_size = false;
  bool verbose = false;

  //strings for constructing filepaths and strings containing basic info.
  // to be printed to these files.
  std::string intro_string;
  std::string out_file_path_base;
  std::string value_type_string_;
  //value_type_string_ but with each space replaced by a dash -
  std::string value_type_string_with_dashes;
  std::string comp_info_string_;
  std::string comp_string;
  std::string file_path_out;
  std::string ext_string = std::string(".txt");
  std::string sep = std::string("-");

  //Options for printing tables of ratios (used with the Timings
  // class found in time_merge_algorithms_class.h):
  bool should_print_merge_function_categories = true;
  bool should_print_times = true;
  bool should_print_average_times = true;
  bool should_print_ratios_of_times = true;
  bool should_print_largest_ratios_of_times = false;
  bool should_print_smallest_ratios_of_times = false;
  bool should_include_percent_faster = true;
  bool should_include_size = true;
  bool should_print_diagonal_elements = false;
  static int default_line_length_;

  inline void SetContainerType(ContainerTypeEnum container_type) {
    container_type_ = container_type;
    return ;
  }

  inline void SetValueTypeString(std::string value_type_str) {
    value_type_string_ = value_type_str;
    std::string str = value_type_string_;
    //Replace each space with a dash - Similarly, replace characters
    // that might not be allowed in file names (of certain OSs)
    // with other characters,
    auto replace_characters = [](char c) {
        if (c == ':' || c == ',')
            return '_';
        else if (c == ' ')
            return '-';
        else
            return c;
    };
    std::transform(str.begin(), str.end(), str.begin(), replace_characters);
    value_type_string_with_dashes = str;
    return ;
  }

  //Returns a string such as std::vector (if prepend_std == true)
  inline std::string GetContainerTypeString(bool prepend_std = false) const {
    return GetContainerTypeString(container_type_, prepend_std);
  }

  static std::string GetContainerTypeString(unsigned int container_type,
                                            bool prepend_std = false) {
    assert(container_type < container_type_string_.size());
    if (prepend_std)
      return std::string("std_") + container_type_string_[container_type];
    else
      return container_type_string_[container_type];
  }

  //Returns a string such as std::vector<unsigned long> (if prepend_std == true)
  inline std::string GetFullContainerString(bool prepend_std = false) const {
    return GetFullContainerString(container_type_, value_type_string_,
                                  prepend_std);
  }

  inline std::string GetFullContainerString(unsigned int container_type,
                                            bool prepend_std = false) const {
    return GetFullContainerString(container_type, value_type_string_,
                                  prepend_std);
  }

  inline std::string GetFullContainerString(unsigned int container_type,
                                            std::string value_type_string,
                                            bool prepend_std = false) const {
    return GetContainerTypeString(container_type, prepend_std)
                      + std::string("<") + value_type_string + std::string(">");
  }

  inline std::string GetPathToFile() const {
    return GetPathToFile(value_type_string_with_dashes, comp_string);
  }

  std::string GetPathToFile(std::string valuetype_str) const {
    auto spaces_to_hyphens = [](char c) {
      return c == ' ' ? '-' : c;
    };
    std::transform(valuetype_str.begin(), valuetype_str.end(),
                   valuetype_str.begin(), spaces_to_hyphens);

    auto colon_to_underscore = [](char c) {
      return c == ':' ? '_' : c;
    };
    std::transform(valuetype_str.begin(), valuetype_str.end(),
                   valuetype_str.begin(), colon_to_underscore);
    return GetPathToFile(valuetype_str, comp_string);
  }

  std::string GetPathToFile(std::string valuetype_str,
                            std::string comp_str) const {
    std::string str = out_file_path_base;
    if (!valuetype_str.empty())
      str += sep + valuetype_str;
    if (!comp_str.empty())
      str += sep + comp_str;
    str += ext_string;
    return str;
  }

  void PrintString(const char * s) {
    PrintString(std::string(s));
  }

  void PrintString(std::stringstream &strstrm, bool should_flush = false) {
    PrintString(strstrm.str(), should_flush);
    return ;
  }

  void PrintString(std::ostringstream &strstrm, bool should_flush = false) {
    PrintString(strstrm.str(), should_flush);
    return ;
  }

  void PrintString(std::string &&str, bool should_flush = false) {
    ostrstrm << str;
    if (also_print_to_std_cout) {
      std::cout << str;
      std::cout.flush();
    }
    if (should_flush)
      Flush();
  }

  void PrintString(const std::string &str, bool should_flush = false) {
    ostrstrm << str;
    if (also_print_to_std_cout) {
      std::cout << str;
      std::cout.flush();
    }
    if (should_flush)
      Flush();
  }

  std::string GetLineString(char c, int length = default_line_length_,
                            bool new_line_at_end = true) const {
    if (length <= 0 && !new_line_at_end)
      return std::string();
    std::ostringstream ostrm;
    for (int i = 0; i < length; ++i)
      ostrm << c;
    if (new_line_at_end)
      ostrm << '\n';
    return ostrm.str();
  }

  std::string GetLineString(std::string c, int length = default_line_length_,
                            bool new_line_at_end = true) const {
    if (length <= 0 && !new_line_at_end)
      return std::string();
    std::ostringstream ostrm;
    for (int i = 0; i < length; ++i)
      ostrm << c;
    if (new_line_at_end)
      ostrm << '\n';
    return ostrm.str();
  }

  void PrintLine(char c, int length = default_line_length_,
                 bool new_line_at_end = true) {
    auto line_str = GetLineString(c, length, new_line_at_end);
    PrintString(line_str);
  }

  void PrintLine(std::string c, int length = default_line_length_,
                 bool new_line_at_end = true) {
    auto line_str = GetLineString(c, length, new_line_at_end);
    PrintString(line_str);
  }

  void PrintNewLines(int number_of_new_lines = 1) {
    if (number_of_new_lines <= 0)
      return ;
    std::ostringstream ostrm;
    for (int i = 0; i < number_of_new_lines; ++i)
      ostrm << '\n';
    PrintString(ostrm.str());
  }

  void Flush() {
    if (should_print_to_file && ostrstrm.str().length() > 0) {
      if (!file_path_out.empty()) {
        std::ofstream f(file_path_out, std::ios::ate | std::ios::app);
        f << ostrstrm.str();
        f.flush();
      } else {
        std::ofstream f(GetPathToFile(), std::ios::ate | std::ios::app);
        f << ostrstrm.str();
        f.flush();
      }
      ostrstrm.str(std::string()); //Clear the ostringstream
    }
    std::cout.flush();
  }
};
//TestingOptions::ContainerTypeEnum TestingOptions::container_type_;
const std::vector<std::string> TestingOptions::container_type_string_ = {
    std::string("vector"),
    std::string("deque"),
    std::string("list")
};
std::ostringstream TestingOptions::ostrstrm;
int TestingOptions::default_line_length_ = 100;

#include "mins_maxs_and_lambda.h"

#include "time_merge_algorithms_class.h"

#include "misc_helpers.h"

#include "merge_common.h"
#include "merge_without_buffer.h"
#include "merge_test_correctness.h"
#include "merge_verify_stability.h"
#include "merge_time.h"

template<typename T, typename ValueType>
std::string GetFileIntro(const TestingOptions &to, T value_lower_bound,
                         T value_upper_bound) {
  std::stringstream strm;
  std::string type_T_string = GetTypeNameString<T>();
  strm << "ValueType:      "   << to.value_type_string_;
  strm << "\nContainer Type: " << to.GetContainerTypeString(true);
  strm << "\nContainer:      " << to.GetFullContainerString(true);
  strm << "\nT:              " << type_T_string << '\n';
  //strm << GetDescriptionOfRangeOfValues<T>(value_lower_bound, value_upper_bound);
  return strm.str();
}

//Helper for TimeAndTestMergeFunctionsWithGivenValuesAndContainer().
std::string GetMergeGrandTotalsString(Timings &total_times,
                                      TestingOptions &to) {
  std::ostringstream strm;
  strm << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
          "Grand Totals for merging ranges contained in containers of type: "
       << to.GetFullContainerString(true) << "\n";
  strm << total_times.GetInfoString(1,
                                true,  //should_print_times
                                true,  //should_print_average_times
                                true,  //should_print_ratios_of_times
                                true,  //should_print_largest_ratios_of_times
                                true,  //should_print_smallest_ratios_of_times
                                true,  //should_include_percent_faster
                                true,  //should_include_size
                                true   //should_print_merge_function_categories
                                );
  strm << "\nFinished Timing and Testing Mergings of ranges of type: "
       << to.GetFullContainerString(true) << '\n';
  return strm.str();
}

template<typename T, typename ValueType, typename Container,
         template<class, class> class ContainerType, typename Compare>
int TimeAndTestMergeFunctionsWithGivenValuesAndContainer(T value_lower_bound,
                                                         T value_upper_bound,
                                                         TestingOptions &to,
                                                         Compare comp,
                                                         Timings &times_out) {
  to.PrintString("\nMerging ranges of type: ");
  to.PrintString(to.GetFullContainerString(true));
  to.PrintNewLines(2);

  auto exponential_next_vec_size_lambda = GetExponential_next_vec_size_lambda(to);
  //The above lambda generates the sequence:
  // vec_size_lower_bound,
  // static_cast(vec_size_scale * vec_size_lower_bound(,
  // static_cast(vec_size_scale * static_cast<std::size_t>(vec_size_scale * vec_size_lower_bound)),
  // ...,
  // vec_size_upper_bound

  auto num_tests_and_num_repititions_lambda
        = GetNum_tests_and_num_repititions_lambda(to);
  auto length_of_left_subvector_lambda = GetLength_of_left_subvector_lambda(to);

  to.PrintString(GetFileIntro<T, ValueType>(to, value_lower_bound,
                                            value_upper_bound));
  to.PrintNewLines(1);
  to.Flush();

  Timings total_times_exp;
  TimeMergeFunctions<T, ValueType, ContainerType, Container>(total_times_exp,
                                   value_lower_bound, value_upper_bound,
                                   exponential_next_vec_size_lambda,
                                   num_tests_and_num_repititions_lambda,
                                   length_of_left_subvector_lambda, to, comp);

  //Now randomly pick vec_size and time the merge functions.
  auto random_next_vec_size_lambda = GetRandom_next_vec_size_lambda(to);
  //The above lambda random picks a value of vec_size in the range
  // [vec_size_lower_bound, vec_size_upper_bound]
  //It does this number_of_random_vec_sizes times before returning 0,
  // at which point the TimeMergeFunctions() terminates.
  auto total_times = total_times_exp;
  total_times.FillInRatioOfTimesFromCurrentTimings();

  if (to.number_of_random_vec_sizes > 0) {
    Timings total_times_random;
    TimeMergeFunctions<T, ValueType, ContainerType, Container>(
                                     total_times_random, value_lower_bound,
                                     value_upper_bound,
                                     random_next_vec_size_lambda,
                                     num_tests_and_num_repititions_lambda,
                                     length_of_left_subvector_lambda, to, comp);
    total_times.UpdateSmallestAndLargestRatiosOfTimesFromOtherTimingsMinMax(
                                                            total_times_random);
    total_times.AddTimesAndCountersFromOtherTimingsToThisObject(
                                                            total_times_random);
    total_times.FillInRatioOfTimesFromCurrentTimings();
  }
  for (int k = 0; k < 2; ++k)
    to.PrintLine('!');
  to.PrintNewLines(1);
  to.PrintString(GetMergeGrandTotalsString(total_times, to));
  times_out = total_times;
  return 0;
}

template<typename T, typename ValueType>
std::string TimeAndTestMergeFunctionsGetInfo(const TestingOptions &to,
                                             bool print_denormal_info = false) {
  auto type_string = GetTypeNameString<ValueType>();
  std::ostringstream osstrm;
  osstrm << to.intro_string;
  osstrm << "ValueType         = " << type_string << '\n';
  osstrm << "sizeof(ValueType) = " << sizeof(ValueType) << "\n";
  if (to.comp_info_string_.empty())
    osstrm << "Using default operator<().";
  else
    osstrm << to.comp_info_string_;
  if (print_denormal_info) {
    osstrm << "Merging " << type_string << "s with denormal numbers ";
    if (to.use_denormal_numbers)
      osstrm << "ALLOWED\n";
    else
      osstrm << "NOT allowed\n";
    if (!to.use_denormal_numbers)
      osstrm << " - NO denormal numbers were used in the ranges.\n";
  }
  osstrm << "\n";
  return osstrm.str();
}

template<typename T, typename ValueType,
         template<class, class> class ContainerType, typename Compare,
std::enable_if_t<!std::is_same<ValueType, typename std::string>::value, int> = 0>
int TimeAndTestMergeFunctions(std::vector<Timings> &timings,
                              TestingOptions &to,
                              Timings &sum_total,
                              Compare comp) {
  to.SetValueTypeString(GetTypeNameString<ValueType>());
  typedef ContainerType<ValueType, std::allocator<ValueType>> Container;
  T value_lower_bound = 0;
  T value_upper_bound = std::numeric_limits<T>::max();
  std::string outro_info = TimeAndTestMergeFunctionsGetInfo<T, ValueType>(to);
  bool is_file_path_out_initially_empty = to.file_path_out.empty();
  if (to.should_print_to_file && is_file_path_out_initially_empty)
    to.file_path_out = to.GetPathToFile();
  to.PrintString(outro_info);
  Timings times;
  int return_value = 0;
  return_value = TimeAndTestMergeFunctionsWithGivenValuesAndContainer<T,
                                ValueType, Container, ContainerType, Compare>(
            value_lower_bound, value_upper_bound, to, comp, times);
  timings.push_back(times);
  sum_total.AddTimesAndCountersFromOtherTimingsToThisObject(times);
  sum_total.UpdateSmallestAndLargestRatiosOfTimesFromOtherTimingsMinMax(times);
  to.PrintString(outro_info);
  std::cout << "\n\n\n\n\n";
  to.Flush();
  if (to.should_print_to_file && is_file_path_out_initially_empty)
    to.file_path_out.clear();
  return return_value;
}

template<typename T, typename ValueType,
         template<class, class> class ContainerType, typename Compare,
std::enable_if_t<std::is_same<ValueType, typename std::string>::value, int> = 0>
int TimeAndTestMergeFunctions(std::vector<Timings> &timings,
                              TestingOptions &to,
                              Timings &sum_total,
                              Compare comp) {
  to.SetValueTypeString(GetTypeNameString<ValueType>());
  typedef ContainerType<ValueType, std::allocator<ValueType>> Container;
  T value_lower_bound = 0;
  T value_upper_bound = 10;
  std::string outro_info = TimeAndTestMergeFunctionsGetInfo<T, ValueType>(to);
  std::ostringstream osstrm;
  osstrm << "merging std::strings with lengths between "
         << value_lower_bound << " and " << value_upper_bound << "\n";
  outro_info += osstrm.str();
  bool is_file_path_out_initially_empty = to.file_path_out.empty();
  if (to.should_print_to_file && is_file_path_out_initially_empty)
    to.file_path_out = to.GetPathToFile();
  to.PrintString(outro_info);
  Timings times;
  int return_value = 0;
  return_value = TimeAndTestMergeFunctionsWithGivenValuesAndContainer<T,
                                ValueType, Container, ContainerType, Compare>(
            value_lower_bound, value_upper_bound, to, comp, times);
  timings.push_back(times);
  sum_total.AddTimesAndCountersFromOtherTimingsToThisObject(times);
  sum_total.UpdateSmallestAndLargestRatiosOfTimesFromOtherTimingsMinMax(times);
  to.PrintString(outro_info);
  to.Flush();
  if (to.should_print_to_file && is_file_path_out_initially_empty)
    to.file_path_out.clear();
  return return_value;
}

template<typename T, typename ValueType,
         template<class, class> class ContainerType, typename Compare>
int TimeAndTestMergeFunctions_floatingpoint(std::vector<Timings> &timings,
                                            TestingOptions &to,
                                            Timings &sum_total,
                                            Compare comp) {
  to.SetValueTypeString(GetTypeNameString<ValueType>());
  typedef ContainerType<ValueType, std::allocator<ValueType>> Container;
  std::string valuetype_str = GetTypeNameString<ValueType>('-');
  T value_lower_bound = std::numeric_limits<T>::min();
  T value_upper_bound = std::numeric_limits<T>::max();
  if (to.use_denormal_numbers) {
    //Use denormal numbers
    value_lower_bound = std::numeric_limits<T>::lowest();
  } else {
    valuetype_str += std::string("-NO-denormals");
  }
  std::string outro_info = TimeAndTestMergeFunctionsGetInfo<T, ValueType>(to, true);
  bool is_file_path_out_initially_empty = to.file_path_out.empty();
  if (to.should_print_to_file && is_file_path_out_initially_empty)
    to.file_path_out = to.GetPathToFile(valuetype_str);
  to.PrintString(outro_info);
  Timings times;
  int return_value = 0;
  return_value = TimeAndTestMergeFunctionsWithGivenValuesAndContainer<T,
                                ValueType, Container, ContainerType, Compare>(
            value_lower_bound, value_upper_bound, to, comp, times);
  timings.push_back(times);
  sum_total.AddTimesAndCountersFromOtherTimingsToThisObject(times);
  sum_total.UpdateSmallestAndLargestRatiosOfTimesFromOtherTimingsMinMax(times);
  to.PrintString(outro_info);
  to.Flush();
  if (to.should_print_to_file && is_file_path_out_initially_empty)
    to.file_path_out.clear();
  std::cout<<__LINE__<<std::endl;
  return return_value;
}

//Helper for TimeAndTestMergeFunctionsOnGivenContainerType().
std::string TimeAndTestMergeFunctionsOnGivenContainerTypeIntroString(
                                                    const TestingOptions &to) {
  std::ostringstream osstrm;
  osstrm << "Container type = " << to.GetContainerTypeString(true)
         << "\nvec_size_scale = " << to.vec_size_scale
         << "\nratio_of_max_value_to_vec_size = "
         << to.ratio_of_max_value_to_vec_size
         << "\nshould_verify_merge_stability = " << std::boolalpha
         << to.should_verify_merge_stability << '\n';
  return osstrm.str();
}

template<template<class, class> class ContainerType>
int TimeAndTestMergeFunctionsOnGivenContainerType(TestingOptions &to,
                                                std::vector<Timings> &timings) {
  to.intro_string = TimeAndTestMergeFunctionsOnGivenContainerTypeIntroString(to);
  Timings sum_total;
  {
    typedef int T;
    typedef std::pair<T, T> ValueType;
    auto comp = [](const ValueType &lhs, const ValueType &rhs) -> bool {
      return (lhs.first + lhs.second) < (rhs.first + rhs.second);
    };
    to.Flush();
    auto to2 = to;
    to2.comp_info_string_ = std::string("Using comparator that is a strict weak"
        " ordering but NOT a total ordering: \n"
        "\tcomp(const std::pair<int, int> &lhs, const std::pair<int, int> &rhs)"
        " { return (lhs.first + lhs.second) < (rhs.first + rhs.second); }\n\n");
    to2.comp_string = std::string("comp=(a+b)<(c+d)");
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to2, sum_total, comp);
    to2.Flush();
    if (return_value != 0)
      return return_value;
  }
  {
    typedef int T;
    typedef std::pair<T, T> ValueType;
    auto comp = [](const ValueType &lhs, const ValueType &rhs) -> bool {
      return (lhs.first * lhs.first + lhs.second * lhs.second)
           < (rhs.first * rhs.first + rhs.second * rhs.second);
    };
    to.Flush();
    auto to2 = to;
    to2.comp_info_string_ = std::string("Using comparator that is a strict weak"
        " ordering but NOT a total ordering: \n"
        "\tcomp(const std::pair<int, int> &lhs, const std::pair<int, int> &rhs)"
        " { return (lhs.first^2 + lhs.second^2) < (rhs.first^2 + rhs.second^2); }\n\n");
    to2.comp_string = std::string("comp=(a^2+b^2)<(c^2+d^2)");
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to2, sum_total, comp);
    to2.Flush();
    if (return_value != 0)
      return return_value;
  }
  {
    typedef int T;
    typedef std::pair<T, T> ValueType;
    auto comp = [](const ValueType &lhs, const ValueType &rhs) -> bool {
      return lhs.first < rhs.first;
    };
    to.Flush();
    auto to2 = to;
    to2.comp_info_string_ = std::string("Using comparator that is a strict weak"
        " ordering but NOT a total ordering: \n"
        "\tcomp(const std::pair<int, int> &lhs, const std::pair<int, int> &rhs)"
        " { return lhs.first < rhs.first; }\n\n");
    to2.comp_string = std::string("comp=lhs.first<rhs.first");
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to2, sum_total, comp);
    to2.Flush();
    if (return_value != 0)
      return return_value;
  }
  {
    typedef int T;
    typedef std::pair<T, T> ValueType;
    auto comp = [](const ValueType &lhs, const ValueType &rhs) -> bool {
      return std::min(lhs.first, lhs.second) < std::min(rhs.first, rhs.second);
    };
    to.Flush();
    auto to2 = to;
    to2.comp_info_string_ = std::string("Using comparator that is a strict weak"
        " ordering but NOT a total ordering: \n"
        "\tcomp(const std::pair<int, int> &lhs, const std::pair<int, int> &rhs)"
        " { return min(lhs.first, lhs.second) < min(rhs.first, rhs.second);"
        " }\n\n");
    to2.comp_string = std::string("comp=min(ab)<min(c,d)");
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to2, sum_total, comp);
    to2.Flush();
    if (return_value != 0)
      return return_value;
  }
  {
    typedef int T;
    typedef std::pair<T, T> ValueType;
    auto comp = std::less<ValueType>();
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  {
    typedef std::string ValueType;
    typedef std::size_t T;
    auto comp = std::less<ValueType>();
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  {
    typedef int ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  {
    typedef unsigned int ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  {
    typedef char ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  {
    typedef unsigned char ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  if (sizeof(short) != sizeof(int))
  {
    typedef short ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  if (sizeof(unsigned short) != sizeof(unsigned int))
  {
    typedef unsigned short ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  if (sizeof(long) != sizeof(int))
  {
    typedef long ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  if (sizeof(unsigned long) != sizeof(unsigned int))
  {
    typedef unsigned long ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  if (sizeof(long long) != sizeof(long))
  {
    typedef long long ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  if (sizeof(unsigned long long) != sizeof(unsigned long))
  {
    typedef unsigned long long ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    int return_value = TimeAndTestMergeFunctions<T, ValueType, ContainerType,
                                decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  const auto use_denormal_numbers_saved = to.use_denormal_numbers;
  {
    typedef float ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    to.use_denormal_numbers = true;
    int return_value = TimeAndTestMergeFunctions_floatingpoint<T, ValueType,
                ContainerType, decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  {
    typedef float ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    to.use_denormal_numbers = false;
    int return_value = TimeAndTestMergeFunctions_floatingpoint<T, ValueType,
                ContainerType, decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  {
    typedef double ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    auto to2 = to;
    to.use_denormal_numbers = true;
    int return_value = TimeAndTestMergeFunctions_floatingpoint<T, ValueType,
                ContainerType, decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  {
    typedef double ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    to.use_denormal_numbers = false;
    int return_value = TimeAndTestMergeFunctions_floatingpoint<T, ValueType,
                ContainerType, decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  {
    typedef long double ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    to.use_denormal_numbers = true;
    int return_value = TimeAndTestMergeFunctions_floatingpoint<T, ValueType,
                ContainerType, decltype(comp)>(timings, to, sum_total, comp);
    if (return_value != 0)
      return return_value;
  }
  {
    typedef long double ValueType;
    typedef ValueType T;
    auto comp = std::less<ValueType>();
    to.use_denormal_numbers = false;
    int return_value = TimeAndTestMergeFunctions_floatingpoint<T, ValueType,
                ContainerType, decltype(comp)>(timings, to, sum_total, comp);std::cout<<__LINE__<<std::endl;
    if (return_value != 0)
      return return_value;
  }
  to.use_denormal_numbers = use_denormal_numbers_saved;
  sum_total.FillInRatioOfTimesFromCurrentTimings();
  timings.push_back(sum_total);

  std::ostringstream strm;
  strm << "\n\n";
  for (int k = 0; k < 2; ++k)
    strm << "!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!="
            "!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=\n";
  strm << "\n";
  strm << "Grand Total Times for container: ";
  strm << to.GetContainerTypeString() << "\n\n";
  strm << to.intro_string << "\n\n";
  strm << sum_total.GetInfoString(1,
                                true,  //should_print_times
                                true,  //should_print_average_times
                                true,  //should_print_ratios_of_times
                                true,  //should_print_largest_ratios_of_times
                                true,  //should_print_smallest_ratios_of_times
                                true,  //should_include_percent_faster
                                true,  //should_include_size
                                true   //should_print_merge_function_categories
                                );
  strm << '\n';
  if (to.also_print_to_std_cout) {
    std::cout << strm.str();
    std::cout << "\n\n\n\n\n\n\n\n\n\n";
  }
  std::cout.flush();
  if (to.also_print_grand_totals_of_each_container_to_separate_file) {
    std::string file_path_out = to.out_file_path_base + to.sep
        + std::string("GRAND-TOTAL") + to.ext_string;
    std::ofstream f(file_path_out, std::ios::out | std::ios::ate);
    f << strm.str();
    f.flush();
  }
  return 0;
}

int main() {
  std::ios::sync_with_stdio(false); //Makes output with std::cout faster.

  TestingOptions to;
  //out_file_path_base will be the prefix used for all output files' names
  std::string out_file_path_base = std::string("Timings_")
                      + std::string("CompileDateTime_")
                      + std::string(__DATE__) + std::string("_")
                      + std::string(__TIME__) + std::string("__")
                      + std::string("ExecDateTime_")
                      + GetCurrentTimeString() + std::string("__");
  //Replace each space with a dash - Similarly, replace characters
  // that might not be allowed in file names (of certain OSs)
  // with other characters,
  auto replace_characters = [](char c) {
      if (c == ':' || c == ',')
          return '_';
      else if (c == ' ')
          return '-';
      else
          return c;
  };
  std::transform(out_file_path_base.begin(), out_file_path_base.end(),
                 out_file_path_base.begin(), replace_characters);
  int return_value;
  Timings supreme_grand_total;

  std::vector<Timings> timings_vector;
  to.SetContainerType(TestingOptions::ContainerTypeEnum::vector_type);
  to.out_file_path_base = out_file_path_base + to.GetContainerTypeString();
  return_value = TimeAndTestMergeFunctionsOnGivenContainerType<std::vector>(
      to, timings_vector);
  if (return_value != 0)
    return return_value;
  supreme_grand_total.AddTimesAndCountersFromOtherTimingsToThisObject(timings_vector.back());
  supreme_grand_total.UpdateSmallestAndLargestRatiosOfTimesFromOtherTimingsMinMax(timings_vector.back());

  std::vector<Timings> timings_deque;
  to.SetContainerType(TestingOptions::ContainerTypeEnum::deque_type);
  to.out_file_path_base = out_file_path_base + to.GetContainerTypeString();
  return_value = TimeAndTestMergeFunctionsOnGivenContainerType<std::deque>(
      to, timings_deque);
  if (return_value != 0)
    return return_value;
  supreme_grand_total.AddTimesAndCountersFromOtherTimingsToThisObject(timings_deque.back());
  supreme_grand_total.UpdateSmallestAndLargestRatiosOfTimesFromOtherTimingsMinMax(timings_deque.back());

  std::vector<Timings> timings_list;
  to.SetContainerType(TestingOptions::ContainerTypeEnum::list_type);
  to.out_file_path_base = out_file_path_base + to.GetContainerTypeString();
  return_value = TimeAndTestMergeFunctionsOnGivenContainerType<std::list>(
      to, timings_list);
  if (return_value != 0)
    return return_value;
  supreme_grand_total.AddTimesAndCountersFromOtherTimingsToThisObject(timings_list.back());
  supreme_grand_total.UpdateSmallestAndLargestRatiosOfTimesFromOtherTimingsMinMax(timings_list.back());
  supreme_grand_total.FillInRatioOfTimesFromCurrentTimings();

  to.out_file_path_base = out_file_path_base;
  std::ostringstream strm;
  strm << "Grand Total Times for ALL timings: \n\n";
  strm << supreme_grand_total.GetInfoString(1,
                                true,  //should_print_times
                                true,  //should_print_average_times
                                true,  //should_print_ratios_of_times
                                true,  //should_print_largest_ratios_of_times
                                true,  //should_print_smallest_ratios_of_times
                                true,  //should_include_percent_faster
                                true,  //should_include_size
                                true   //should_print_merge_function_categories
                                );
  if (to.also_print_to_std_cout) {
    std::cout << strm.str();
  }
  std::cout.flush();
  if (to.should_print_to_file) {
    std::string file_path_out = out_file_path_base + to.sep
        + std::string("GRAND-TOTAL") + to.ext_string;
    std::ofstream f(file_path_out, std::ios::out | std::ios::ate);
    f << strm.str();
    f.flush();
  }
  return 0;
  return return_value;
}

