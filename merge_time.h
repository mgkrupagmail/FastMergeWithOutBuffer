/*
 * merge_time.h
 *
 *  Created on: Jun 28, 2017
 *      Author: Matthew Gregory Krupa
 *
 *  This header file defines the TimeMergesOnGivenVecSize() function, which
 *   generates random sorted ranges and times how long various merge functions
 *   take to merges the two oranges.
 *  The various merge functions will all merge the same exact two ordered
 *   ranges.
 *  The following lists the timed merge functions that do NOT use a buffer
 *  with the best performing functions first:
 *  1) MergeWithOutBuffer() (which is just MergeWithOutBufferTrim2())
 *  2) MergeWithOutBufferTrim1()
 *  3) MergeWithOutBufferTrim2()
 *  4) MergeWithOutBufferTrim3()
 *  5) MergeWithOutBufferTrim4()
 *  6) MergeWithOutBufferTrim5()
 *  7) gnu::gnu__merge_without_buffer()
 *
 * If one was to continue the pattern of the definitions of TrimEnds1(),
 *  TrimEnds2(), ... so as to construct TrimEnds6(), TrimEnds7(), ... and
 *  consequently also MergeWithOutBufferTrim6(), MergeWithOutBufferTrim7(), ...
 *  then the boost in speed becomes either miniscule or negative beginning with
 *  MergeWithOutBufferTrim6() and subsequent MergeWithOutBufferTrim#() tend to
 *  become increasingly slower.
 * This is why we end our pattern of definitions with the definitions of
 *  TrimEnds5() and MergeWithOutBuffer() (which could have been labeled
 *  MergeWithOutBufferTrim5() in order to fit the pattern).
 */

#ifndef SRC_MERGE_TIME_H_
#define SRC_MERGE_TIME_H_

// DESCRIPTION OF TIMING ALGORITHMS.
// A structure TotalTimes is defined, which keeps track of the execution times
//  of the various merge algorithms. We now describe details of TotalTimes:
//  - Each merge algorithm has a corresponding time member variable (of type
//    std::chrono::nanoseconds) associated with it (e.g. std::merge() has
//    TotalTimes::std_merge, MergeWithOutBuffer() has
//    TotalTimes::merge_without_buffer, etc.)
//    where each of these times is initialized to 0 nanoseconds.
//  - For each merge algorithm, there is a corresponding boolean member
//    variable whose name begins with should_time_ (e.g.
//    TotalTimes::should_time_std_merge, etc.), which if false results
//    in all things related to that merge algorithm being ignored.
//  - TotalTimes has static member variables to keep track of statistics.
//    For instance,
//    TotalTimes::largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer
//    keeps track of the largest ratio of
//    merge_without_buffer / gnu_merge_without_buffer.
//  - TotalTimes::GetStringOfStaticVariables() returns a string describing
//    the static member variables.
//  - TotalTimes::UpdateStaticVariables() updates (if necessary) the static
//    variables based on the values stored in the TotalTimes object's member
//    variables.
//  - TotalTimes::GetAveragesStr() Returns a string describing the TotalTimes
//    object's member variables.

// Description of TimeMergesOnGivenVecSize():
//
// Say that a vector vec of size vec_size is (or consists of)
//  ***two non-decreasing sequences*** if there is some integer start_right
//   such that
//   1) 0 < start_right < vec_size,
//   2) vec[0], ..., vec[start_right - 1] is non-decreasing (this is called
//      the ***left vector***
//   3) vec[start_right], ..., vec[vec_size - 1] is non-decreasing (this is
//      called the **right vector***
//  We will not be dealing with the degenerate case where start_right == 0 or
//  start_right == vec_size. The timing algorithm TimeMergesOnGivenVecSize()
//  is performed on the given value of vec_size so assume that vec_size
//  is fixed.
//
//  1) If start_right == -1 then start_right is replaced by vec_size / 2.
//  2) If start_right  < -1 then start_right is replaced by a random integer
//     between 1 and vec_size - 1 (inclusive).
//  3) A vector of size vec_size called vec_original is allocated. A copy
//     of vec_original called vec is allocated.
//  - Note that the memory locations of vec_original's data will not change
//    throughout the execution of TimeMergesOnGivenVecSize().
//    Ditto for vec's data.
//  4) A new TotalTimes object, called total_times, is created.
//  5) The following is done num_tests_per_vec_size times, where each iteration
//     is called a ***test***:
//    a) vec_original is filled with random values in such a way that it will
//       consist of two non-decreasing sequences.
//       In detail: (I) vec_original is first filled with random data,
//       (II) the first start_right elements of vec_original are sorted, and
//       (III) the last vec_size - start_right elements of vec_original
//             are sorted.
//       Note that vec_original's data will not be changed for the rest
//       of this test.
//    b) vec_original's data is copied into vec.
//    c) A call is made to TimeMergesOnGivenRanges(), which times the execution
//       of the various merge algorithms (more details below)
//       - This function returns a TotalTimes object called times, which is
//         then added to total_times.
//    d) If should_pick_new_random_length_for_each_new_vec == true then
//       start_right is replaced by a new random value.
//  6) Information from the total_times object is printed to std::out
//     and the total_times object is returned.

// Description of TimeMergesOnGivenRanges():
//
//  - Inputs: a const reference to vec_original, a reference to vec, and
//            the values start_right and num_repititions_per_vec.
//  - For each merge function that is to be timed, the following is
//    done num_repititions_per_vec times:
//    1) A variable std::chrono::nanoseconds total is initialized to 0.
//    2) At least one untimed call is made to the merge function. After
//       each call, vec's data is restored to its original state.
//      - This is done so as to load code and data into the cache.
//        Note that this does actually affect timings.
//    3) num_repititions timed calls to the merge function are executed.
//      - The time of each call is added to the variable total.
//      - After each call, vec is restored to its original state.
//    4) The value of total is returned and execution returns to
//       TimeMergesOnGivenRanges().
//      - The return value is added to the appropriate member variable
//        of total_times.
//    5) Once all merge functions have been timed, TimeMergesOnGivenRanges()
//       returns total_times and execution returns to
//       TimeMergesOnGivenVecSize().

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "gnu_merge_without_buffer.h"
#include "merge_without_buffer.h"
#include "merge_without_buffer1.h"
#include "merge_without_buffer2.h"

#include "merge_verify_stability.h"

static const std::chrono::nanoseconds zero_nano{0};

template<typename ForwardIterator1, typename ForwardIterator2>
inline void AssignRightVectorValuesToLeft(
                ForwardIterator1 lhs_start, ForwardIterator1 lhs_one_past_end,
                ForwardIterator2 rhs_start, ForwardIterator2 rhs_one_past_end) {
  auto lhs = lhs_start;
  auto rhs = rhs_start;
  while (lhs != lhs_one_past_end && rhs != rhs_one_past_end) {
    *lhs = *rhs;
    ++lhs;
    ++rhs;
  }
  return ;
}

template<typename ForwardIterator1, typename ForwardIterator2>
inline void MoveRightRangeValuesToLeftRange(
                ForwardIterator1 lhs_start, ForwardIterator1 lhs_one_past_end,
                ForwardIterator2 rhs_start, ForwardIterator2 rhs_one_past_end) {
  auto lhs = lhs_start;
  auto rhs = rhs_start;
  while (lhs != lhs_one_past_end && rhs != rhs_one_past_end) {
    *lhs = std::move(*rhs);
    ++lhs;
    ++rhs;
  }
  return ;
}

template<typename Iterator, typename Compare, typename Distance>
struct StdInplaceMerge {
  inline void operator()(Iterator start_left,
                         Iterator start_right,
                         Iterator one_past_end_right,
                         Distance length_left,
                         Distance length_right,
                         Compare comp) {
    std::inplace_merge<Iterator, Compare>(start_left, start_right,
                                          one_past_end_right, comp);
    return ;
  }
};

template<typename Iterator, typename Compare, typename Distance>
struct StdMerge {
  inline void operator()(Iterator start_left,
                         Iterator start_right,
                         Iterator one_past_end_right,
                         Distance length_left,
                         Distance length_right,
                         Compare comp) {
    typedef typename Iterator::value_type ValueType;
    std::vector<ValueType> temp_vec(length_left + length_right);
    std::merge(start_left, start_right, start_right, one_past_end_right,
               temp_vec.begin(), comp);
    //The following move is also timed so that in all merge function calls
    // we tinme the same thing: how long it takes to merge the ranges
    // [iter_left_start, iter_left_one_past_end) and
    // [iter_left_one_past_end, iter_right_one_past_end)
    //into the range
    // [iter_left_start, iter_right_one_past_end)
    MoveRightRangeValuesToLeftRange(start_left, one_past_end_right,
                                    temp_vec.begin(), temp_vec.end());
    return ;
  }
};

// NOTE: All of the following Time...() functions except for
//  TimeStdMergeOnGivenVec() have near identical code where the primary
//  difference is the merge function that is called and timed.
//
// The following is what is timed:
//  Given the following inputs (which are the same as the inputs to
//   gnu__merge_without_buffer):
//   (a) iterators iter_left_start and iter_left_one_past_end for the
//       left sorted vector, and
//   (b) iterators iter_right_start and iter_right_one_past_end for the
//       right sorted vector,
//   (c) the lengths of these left and right vectors
//  these functions will time how long its corresponding merge function takes
//  to rearrange the data so that
//   (1) [iter_left_start, iter_left_one_past_end] is increasing,
//   (2) [iter_right_start, iter_right_one_past_end] is increasing, and
//   (3) *(iter_left_one_past_end - 1) <= *iter_right_start.
//  The reasoning behind these requirements is that this is what
//   gnu__merge_without_buffer() does, (although it in the special case
//   where iter_left_one_past_end == iter_start_right).
//
// Note that TimeStdMergeOnGivenVec() additionally allocates a
//  std::vector<T> temp_vector of size vec_size since std::merge()
//  requires such a vector as input.
//  - Since the allocation of temp_vector is a necessary part of
//    std::merge()'s use to perform what is described above,
//    the time it takes to allocate temp_vector
//    is included in the timing of std::merge().
//  - For the same reasons, temp_vector's data is copied back into the
//    left and right vectors.

#ifndef NUMBER_OF_UNTIMED_CALLS_TO_MERGE
#define NUMBER_OF_UNTIMED_CALLS_TO_MERGE 2
#endif

template<template<class, class, class> class MergeFunction, typename ValueType,
         template<class, class> class ContainerType,
         typename Iterator, typename Compare>
std::chrono::nanoseconds TimeMergeOnGivenVec(Iterator iter_left_start,
                                     Iterator iter_right_start,
                                     Iterator iter_right_one_past_end,
                                     const std::vector<ValueType> &vec_original,
                                     std::size_t length_left,
                                     std::size_t length_right,
                                     std::size_t num_repititions,
                                     TestingOptions &to,
                                     Compare comp,
                                     std::string function_name_string) {
  typedef typename Iterator::difference_type Distance;
  typedef std::size_t SizeType;
  typedef MergeFunction<Iterator, Compare, Distance> MergeFunctionClass;
  std::chrono::nanoseconds total{0};

  if (to.should_verify_merge_stability) {
    bool result = VerifyMergeStabilityOnGivenVec<ValueType, ContainerType,
                          Compare, MergeFunction, SizeType>(vec_original.size(),
                          vec_original, length_left, length_right, to, comp, 0);
    if (!result) {
      std::ostringstream ostrm;
      ostrm << "line: " << __LINE__ << " \t"
            << function_name_string << " merge was NOT stable.\n";
      to.PrintString(ostrm, true);
      return total;
    }
  }
  {
    int number_of_untimed_calls = NUMBER_OF_UNTIMED_CALLS_TO_MERGE;
    if (number_of_untimed_calls == 0 && to.also_check_correctness_of_merge)
      number_of_untimed_calls = 1;
    for (int counter = 0; counter < number_of_untimed_calls; ++counter)
    { //Do at least one call to the merge function to load code in caches.
      try {
        MergeFunctionClass()(iter_left_start, iter_right_start,
                             iter_right_one_past_end, length_left, length_right,
                             comp);
      } catch (...) {
        std::ostringstream ostrm;
        ostrm << function_name_string << " threw an exception.\n";
        to.PrintString(ostrm, true);
        std::terminate();
      }
      if (to.also_check_correctness_of_merge && counter == 0) {
        if (!std::is_sorted(iter_left_start, iter_right_one_past_end, comp)) {
          std::ostringstream ostrm;
          ostrm << "std::is_sorted() return FALSE after call to "
                << function_name_string << '\n';
          to.PrintString(ostrm, true);
          std::terminate();
        }
      }
      AssignRightVectorValuesToLeft(iter_left_start, iter_right_one_past_end,
                                    vec_original.begin(), vec_original.end());
    }
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    MergeFunctionClass()(iter_left_start, iter_right_start,
                         iter_right_one_past_end, length_left, length_right,
                         comp);
    total += std::chrono::high_resolution_clock::now() - start_time;
    AssignRightVectorValuesToLeft(iter_left_start, iter_right_one_past_end,
                                  vec_original.begin(), vec_original.end());
  }
  //Make sure that the loop wasn't optimized away.
  assert(num_repititions == 0 || total != zero_nano);
  return total;
}

typedef std::chrono::nanoseconds time_units;


template<template<class, class, class> class MergeFunction,
         typename ValueType,
         template<class, class> class ContainerType,
         typename Iterator, typename Compare>
void TimeGivenMergeOnGivenVec(
                    Iterator iter_left_start,
                    Iterator iter_right_start,
                    Iterator iter_right_one_past_end,
                    const std::vector<ValueType> &vec_original,
                    std::size_t length_left,
                    std::size_t length_right,
                    std::size_t num_repititions_per_vec,
                    Timings &total_times,
                    TestingOptions &to,
                    Compare comp,
                    TimingsIndex call_index,
                    std::string function_name_string) {
  if (total_times.GetIsMergeFunctionEnabled(call_index)) {
    try {
      total_times.times_[call_index] +=
          TimeMergeOnGivenVec<MergeFunction, ValueType, ContainerType,
                              Iterator, Compare>(
          iter_left_start, iter_right_start, iter_right_one_past_end,
          vec_original, length_left, length_right, num_repititions_per_vec,
          to, comp, function_name_string);
      total_times.number_of_times_merge_function_was_called_[call_index] +=
          num_repititions_per_vec;
    } catch (...) {
      std::ostringstream ostrm;
      ostrm << "line: " << __LINE__ << " \t"
            << function_name_string << " threw an exception.\n";
      to.PrintString(ostrm, true);
    }
  }
}

template<typename T, typename ValueType,
         template<class, class> class ContainerType,
         typename Iterator, typename Compare>
inline void TimeMergesOnGivenRanges(Iterator iter_left_start,
                                 Iterator iter_right_start,
                                 Iterator iter_right_one_past_end,
                                 const std::vector<ValueType> &vec_original,
                                 std::size_t length_left,
                                 std::size_t length_right,
                                 std::size_t num_repititions_per_vec,
                                 Timings &total_times,
                                 TestingOptions &to,
                                 Compare comp) {
assert(length_left == static_cast<std::size_t>(std::distance(iter_left_start, iter_right_start)));
  TimeGivenMergeOnGivenVec<StdMerge, ValueType, ContainerType, Iterator,
                           Compare>(
       iter_left_start, iter_right_start, iter_right_one_past_end,
       vec_original, length_left, length_right, num_repititions_per_vec,
       total_times, to, comp, TimingsIndex::StdMergeIndex,
       std::string("std::merge()"));
  TimeGivenMergeOnGivenVec<StdInplaceMerge, ValueType, ContainerType, Iterator,
                           Compare>(
       iter_left_start, iter_right_start, iter_right_one_past_end,
       vec_original, length_left, length_right, num_repititions_per_vec,
       total_times, to, comp, TimingsIndex::StdInplaceMergeIndex,
       std::string("std::inplace_merge()"));
  TimeGivenMergeOnGivenVec<GnuMergeWithoutBuffer, ValueType, ContainerType,
                           Iterator, Compare>(
       iter_left_start, iter_right_start, iter_right_one_past_end,
       vec_original, length_left, length_right, num_repititions_per_vec,
       total_times, to, comp, TimingsIndex::GnuMergeWithoutBufferIndex,
       std::string("gnu__merge_without_buffer()"));

  TimeGivenMergeOnGivenVec<MergeWOBuff, ValueType, ContainerType, Iterator,
                           Compare>(
       iter_left_start, iter_right_start, iter_right_one_past_end,
       vec_original, length_left, length_right, num_repititions_per_vec,
       total_times, to, comp, TimingsIndex::MergeWithOutBufferIndex,
       std::string("MergeWithOutBuffer()"));

  TimeGivenMergeOnGivenVec<MergeWOBuff1, ValueType, ContainerType, Iterator,
                           Compare>(
       iter_left_start, iter_right_start, iter_right_one_past_end,
       vec_original, length_left, length_right, num_repititions_per_vec,
       total_times, to, comp, TimingsIndex::MergeWithOutBuffer1Index,
       std::string("MergeWithOutBuffer1()"));
  return ;
}


std::string TimeMergesOnGivenVecSizeHelperGetFinalInfoString(
                                           std::size_t vec_size,
                                           Timings &times,
                                           const TestingOptions &to,
                                           std::size_t num_tests_per_vec_size,
                                           std::size_t num_repititions_per_vec,
                                           bool was_start_right_picked_randomly,
                                           std::size_t start_right) {
  std::ostringstream ostrm;
  std::string line_ = to.GetLineString('_');
  ostrm << line_;

  if (num_tests_per_vec_size > 1) {
    ostrm << "The sizes of the two component vectors ";
    if (to.should_pick_new_random_length_for_each_new_vec) {
      ostrm << "DID";
    } else {
      ostrm << "did NOT";
    }
    ostrm << " vary between different values of vec_original";
    if (!to.should_pick_new_random_length_for_each_new_vec) {
      ostrm << "; the value was: " << start_right;
    }
    ostrm << ".\n";
  }
  ostrm << "Each merge algorithm was called " << num_repititions_per_vec
        << " times for each value of vec_original, so ";
  auto total_num_calls = num_repititions_per_vec * num_tests_per_vec_size;
  ostrm << "each merge algorithm was called a total of "
        << total_num_calls << " times.\n\n";
  if (to.print_average_time_for_each_vec_size_divide_by_total_num_calls)
    ostrm << times.GetInfoString(total_num_calls, to);

  if (to.print_average_time_for_each_vec_size_divide_by_num_repetitions)
    ostrm << times.GetInfoString(num_repititions_per_vec, to);
  ostrm << line_;
  //ostrm << "\n";
  return ostrm.str();
}


// Two vectors of size vec_size are created. The following is then done
// num_tests_per_vec_size times:
// 1) vec_original is filled with random data.
// 2) vec_original is split into 2 parts (a left part and a right part) based
//     on the value of start_right.
// 3) The left part of vec_original is sorted. The right part of vec_original
//     is sorted.
// 4) A copy of vec_original is made and stored in the variable vec.
// 5) TimeMergesOnGivenRanges() is called, which calls and times various merge
//     algorithms on vec.
//    The resulting times are added to total_times.
template<typename T, typename ValueType,
         template<class, class> class ContainerType, typename Container,
         typename LengthLeftLambdaType, typename Compare>
auto TimeMergesOnGivenVecSize(std::size_t vec_size,
                          std::size_t num_tests_per_vec_size,
                          std::size_t num_repititions_per_vec,
                          LengthLeftLambdaType length_of_left_subvector_lambda,
                          T value_lower_bound,
                          T value_upper_bound,
                          TestingOptions &to,
                          Compare comp,
                          Timings &total_times) {
  auto initial_p = length_of_left_subvector_lambda(vec_size, 0);
  auto start_right = initial_p.first;
  bool was_start_right_picked_randomly = initial_p.second;
assert(start_right < vec_size && start_right > 0);
  std::vector<ValueType> vec_original;
  Container cont;

  try {
    vec_original = std::vector<ValueType>(vec_size);
  } catch (...) {
    std::ostringstream ostrm;
    ostrm << "Unable to allocate vector vec_original of size "
          << vec_size << std::endl;
    to.PrintString(ostrm);
    return ;
  }

  try {
    cont = std::move(Container(vec_size));
  } catch (...) {
    std::ostringstream ostrm;
    ostrm << "Unable to create a copy of vector vec_original of size "
          << vec_size << std::endl;
    to.PrintString(ostrm);
    return ;
  }

  for (std::size_t i = 0; i < num_tests_per_vec_size; i++) {
assert(start_right > 0 && start_right < vec_size);
    std::size_t length_left  = start_right;
    std::size_t length_right = vec_size - start_right;

    auto iter_left_start = cont.begin();
    auto iter_left_one_past_end = cont.begin();
    std::advance(iter_left_one_past_end, start_right);
    auto iter_right_start = iter_left_one_past_end;
    auto iter_right_one_past_end = cont.end();

    FillWithRandomNumbers<ValueType>(vec_original.begin(), vec_original.end(),
                                     value_lower_bound, value_upper_bound);
    //Sort the left and right subvectors
    std::sort(vec_original.begin(), vec_original.begin() + start_right, comp);
    std::sort(vec_original.begin() + start_right, vec_original.end(), comp);

    AssignRightVectorValuesToLeft(cont.begin(), cont.end(),
                                  vec_original.begin(), vec_original.end());

    if (to.print_info_string_for_each_vector && to.verbose) {
      std::ostringstream ostrm;
      ostrm << "_______________________________________________________________";
      ostrm << "length_left = " << length_left
           << "\tlength_right = " << length_right
           << "\tstart_right = " << start_right
           << "\tend_right = "   << (vec_size - 1)  << '\n';
      to.PrintString(ostrm);
    }
    Timings times;
    TimeMergesOnGivenRanges<T, ValueType, ContainerType>(iter_left_start,
                            iter_right_start, iter_right_one_past_end,
                            vec_original, length_left, length_right,
                            num_repititions_per_vec, times, to, comp);
    if (to.update_smallest_and_largest_ratio_for_each_vector) {
      times.FillInRatioOfTimesFromCurrentTimings();
      times.UpdateLargestAndSmallestRatioOfTimesUsingCurrentRatios(vec_size);
      total_times.UpdateSmallestAndLargestRatiosOfTimesFromOtherTimings(times,
                                                                    vec_size);
    }
    total_times.AddTimesAndCountersFromOtherTimingsToThisObject(times);

    if (to.print_info_string_for_each_vector) {
      if (to.verbose)
        to.PrintString("Times for this particular vector: \n");
      times.FillInRatioOfTimesFromCurrentTimings();

      to.PrintString(times.GetInfoString(num_repititions_per_vec,
         to.should_print_times,
         false, //should_print_average_times
         to.should_print_ratios_of_times,
         false, //should_print_largest_ratios_of_times
         false, //should_print_smallest_ratios_of_times
         to.should_include_percent_faster,
         to.should_include_size,
         to.should_print_merge_function_categories));
      to.Flush();
    }
    auto p = length_of_left_subvector_lambda(vec_size, start_right);
    start_right = p.first;
    was_start_right_picked_randomly = p.second;
  }

  total_times.FillInRatioOfTimesFromCurrentTimings();
  total_times.UpdateLargestAndSmallestRatioOfTimesUsingCurrentRatios(vec_size);
  if (to.print_average_time_for_each_vec_size) {
    to.PrintString(TimeMergesOnGivenVecSizeHelperGetFinalInfoString(vec_size,
              total_times, to, num_tests_per_vec_size, num_repititions_per_vec,
              was_start_right_picked_randomly, start_right));
  }
  return ;
}


// Helper function for TimeMergeFunctionsOnRandomVecSizes() and
//  TimeMergeFunctions().
// Finds the values of appropriate values of range_lower_bound and
//  range_upper_bound.
// Note that if ratio_of_max_value_to_vec_size > 0.0l then
//  range_lower_bound will be set to 0 and range_upper_bound will be
//  set to ratio_of_max_value_to_vec_size// vec_size.
//  This is done to control how many values in vec_original will be repeated.
//  e.g. If ratio_of_max_value_to_vec_size = 0.5l then there will be many
//   values that appear twice in vec_original.
//   If ratio_of_max_value_to_vec_size = 0.0l then each value of
//   vec_original[i] is chosen at random from the interval
//   [std::numeric_limits<T>::min(), std::numeric_limits<T>::max()]
//   so that if for instance T is an int or a float then it is unlikely
//   that very many (if any) values will occur twice in vec_original.
//   Thus ratio_of_max_value_to_vec_size is included to prevent us from
//   ONLY timing merge functions on vectors whose values are all distinct.
//
// Example call:
//  FillInRangeBounds(vec_size, range_lower_bound, range_upper_bound,
//                    ratio_of_max_value_to_vec_size, value_lower_bound,
//                    value_upper_bound);
template<typename T, typename ValueType,
std::enable_if_t<std::is_integral<ValueType>::value, int> = 0>
void FillInRangeBounds(std::size_t vec_size,
                T &range_lower_bound, T &range_upper_bound, TestingOptions &to,
                T value_lower_bound, T value_upper_bound) {
  auto ratio_of_max_value_to_vec_size = to.ratio_of_max_value_to_vec_size;
  range_lower_bound = value_lower_bound;
  range_upper_bound = value_upper_bound;
  if (ratio_of_max_value_to_vec_size > 0.0l) { //Then replace range_upper_bound
                                // by ratio_of_max_value_to_vec_size * vec_size
                                // and replace range_lower_bound by 0.
    range_lower_bound = 0;
    //Check if ratio_of_max_value_to_vec_size * vec_size will overflow
    if (!(static_cast<long double>(std::numeric_limits<T>::max())
         / static_cast<long double>(vec_size) < ratio_of_max_value_to_vec_size)) {
      long double new_range_upper_bound_ld = ratio_of_max_value_to_vec_size
                            * static_cast<long double>(vec_size); //No overflow.
      if (new_range_upper_bound_ld <=
                        static_cast<long double>(std::numeric_limits<T>::max()))
        range_upper_bound = static_cast<T>(new_range_upper_bound_ld);
    }
  }
  if (range_upper_bound > std::numeric_limits<T>::max())
    range_upper_bound = std::numeric_limits<T>::max();
  return ;
}

template<typename T, typename ValueType,
std::enable_if_t<std::is_same<ValueType, typename std::pair<T, T>>::value, int> = 0,
std::enable_if_t<std::is_integral<T>::value, int> = 0>
void FillInRangeBounds(std::size_t vec_size,
                T &range_lower_bound, T &range_upper_bound, TestingOptions &to,
                T value_lower_bound, T value_upper_bound) {
  FillInRangeBounds<T, T>(vec_size, range_lower_bound, range_upper_bound, to,
      value_lower_bound, value_upper_bound);
  return ;
}

template<typename T, typename ValueType,
std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
void FillInRangeBounds(std::size_t vec_size,
                T &range_lower_bound, T &range_upper_bound, TestingOptions &to,
                T value_lower_bound, T value_upper_bound) {
  return ;
}

template<typename T, typename ValueType,
std::enable_if_t<std::is_same<ValueType, typename std::string>::value, int> = 0>
void FillInRangeBounds(std::size_t vec_size,
                T &range_lower_bound, T &range_upper_bound, TestingOptions &to,
                T value_lower_bound, T value_upper_bound) {
  return ;
}

// Helper function for the GetVecSizeTimingIntroductionText() functions.
std::string GetVecSizeTimingIntroductionText(std::size_t vec_size,
    std::size_t num_tests_per_vec_size, std::size_t num_repititions_per_vec) {
  std::ostringstream ostrm;
  ostrm << "vec_size = " << vec_size << '\n'
        << "num_tests_per_vec_size = " << num_tests_per_vec_size
        << " \tnum_repititions_per_vec = " << num_repititions_per_vec
        << " \t";
  return ostrm.str();
}

// Helper function for TimeMergeFunctions().
template<typename T, typename ValueType,
std::enable_if_t<std::is_integral<T>::value, int> = 0,
std::enable_if_t<std::is_integral<ValueType>::value
         || std::is_same<ValueType, typename std::pair<T, T>>::value, int> = 0>
std::string GetVecSizeTimingIntroductionText(std::size_t vec_size,
    std::size_t num_tests_per_vec_size, std::size_t num_repititions_per_vec,
    T range_lower_bound, T range_upper_bound) {
  std::ostringstream ostrm;
  //Don't use ostrm << range_lower_bound since it don't output a number if
  // T has type char.
  std::string range_lower_bound_str = std::to_string(range_lower_bound);
  std::string range_upper_bound_str = std::to_string(range_upper_bound);
  ostrm << GetVecSizeTimingIntroductionText(vec_size, num_tests_per_vec_size,
                                            num_repititions_per_vec)
        << "Values picked randomly from ["
        << range_lower_bound_str << ", " << range_upper_bound_str << "].\n";
  return ostrm.str();
}

template<typename T, typename ValueType,
std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
std::string GetVecSizeTimingIntroductionText(std::size_t vec_size,
    std::size_t num_tests_per_vec_size, std::size_t num_repititions_per_vec,
    T range_lower_bound, T range_upper_bound) {
  std::ostringstream ostrm;
  ostrm << GetVecSizeTimingIntroductionText(vec_size, num_tests_per_vec_size,
                                            num_repititions_per_vec)
        << "Values picked randomly from ("
        << range_lower_bound << ", " << range_upper_bound << ").\n";
  return ostrm.str();
}

template<typename T, typename ValueType,
std::enable_if_t<std::is_same<ValueType, typename std::string>::value, int> = 0>
std::string GetVecSizeTimingIntroductionText(std::size_t vec_size,
    std::size_t num_tests_per_vec_size, std::size_t num_repititions_per_vec,
    T range_lower_bound, T range_upper_bound) {
  std::ostringstream ostrm;
  ostrm << GetVecSizeTimingIntroductionText(vec_size, num_tests_per_vec_size,
                                           num_repititions_per_vec)
        << "Strings picked randomly with lengths between "
        << range_lower_bound << " and " << range_upper_bound << ".\n";
  return ostrm.str();
}

//  This function will use next_vec_size_lambda to generate an increasing
//   sequence of values for vec_size.
//  For each value of vec_size it will:
//   (1) Use num_tests_per_vec_size_lambda to generate a value
//           num_tests_per_vec_size.
//   (2) Use num_repititions_per_vec_lambda to generate a value
//           num_repititions_per_vec.
//   where these two values are by default chosen so that the total computation
//   time is approximately the same for each value of vec_size.
//   (4) Allocate two vectors of size vec_size called vec_original and vec.
//      - The location in memory of vec's internal data array will not be
//         not be changed until vec_size is changed.
//      - The same is true of vec_original's internally allocated data array.
//
//  For each value of vec_size it will do the following num_tests_per_vec_size
//   times:
//   (1) Fill a vector vec_original with random values.
//   (2) Pick a random value start_right such that 1 <= start_right < vec_size.
//   (3) Sort vec_original[0, start_right), which forms "the left vector".
//   (4) Sort vec_original[start_right, vec_size] which forms "the right vector"
//   (5) Copy vec_original's data into vec.
//   (6) The following will be done num_repititions_per_vec times for each
//        merge function:
//      (a) The merge function will be applied to merge "the left vector"
//           and "the right vector" and its execution will be timed.
//      (b) This time will be added to a running total time for the merge
//           function being timed.
//      (c) After the merge function is done being times, vec's data will
//           be restored to its previous "pre-merge" state (i.e.
//           vec_original's data will be copied over to vec).
//   (7) Information about the timings will be printed.
//
// This function will do the following until the values that vec_size
//  takes on cease to strictly increase.
//  1) It will initialize vec_size to vec_size_start.
//  2) It will use num_tests_per_vec_size_lambda  to generate a
//     value num_tests_per_vec_size.
//  3) It will use num_repititions_per_vec_lambda to generate a
//     value num_repititions_per_vec.
//  4) It will eventually call TimeMergesOnGivenVecSize() where
//     the random values of the vectors will be taken from the range
//     [range_lower_bound, range_upper_bound], which TimeMergeFunctions()
//     defines.
//     - In particular, if ratio_of_max_value_to_vec_size > 0.0l then
//       range_lower_bound will be set to 0 and range_upper_bound will be
//       set to ratio_of_max_value_to_vec_size * vec_size.
//       This is done to control how many values in vec_original will be
//        repeated.
//       e.g. If ratio_of_max_value_to_vec_size = 0.5l then there will be many
//        values that appear twice in vec_original.
//        If ratio_of_max_value_to_vec_size = 0.0l then each value of
//        vec_original[i] is chosen at random from the interval
//        [std::numeric_limits<T>::min(), std::numeric_limits<T>::max()]
//        so that if for instance T is an int or a float then it is unlikely
//        that very many (if any) values will occur twice in vec_original.
//        Thus ratio_of_max_value_to_vec_size is included to prevent us from
//        ONLY timing merge functions on vectors whose values are all distinct.
//  5) It will call TimeMergesOnGivenVecSize() with the obvious
//     input and add the TotalTimes object it returns to total_times.
//  6) It will call next_vec_size_lambda() to get the value of vec_size
//     for the next iteration of the loop (or it may leave the loop).
// It will terminate by returning total_times.
template<typename T, typename ValueType,
         template<class, class> class ContainerType, typename Container,
         typename VecSizeLambdaType, typename NumTestAndRepititionsLambdaType,
         typename LengthLeftLambdaType, typename Compare>
auto TimeMergeFunctions(
          Timings &total_times,
          T value_lower_bound,
          T value_upper_bound,
          VecSizeLambdaType next_vec_size_lambda,
          NumTestAndRepititionsLambdaType num_tests_and_num_repititions_lambda,
          LengthLeftLambdaType length_of_left_subvector_lambda,
          TestingOptions &to,
          Compare comp) {
  std::size_t vec_size_count = 0;
  std::size_t vec_size = next_vec_size_lambda(0, vec_size_count);
  do {
    auto p = num_tests_and_num_repititions_lambda(vec_size);
    std::size_t num_tests_per_vec_size = p.first;
    std::size_t num_repititions_per_vec = p.second;
    T range_lower_bound = value_lower_bound,
      range_upper_bound = value_upper_bound;
    FillInRangeBounds<T, ValueType>(vec_size, range_lower_bound,
                   range_upper_bound, to, value_lower_bound, value_upper_bound);
    to.PrintString(GetVecSizeTimingIntroductionText<T, ValueType>(vec_size,
                              num_tests_per_vec_size, num_repititions_per_vec,
                              range_lower_bound, range_upper_bound));
    std::cout.flush();
    Timings times;
    TimeMergesOnGivenVecSize<T, ValueType, ContainerType, Container,
                             LengthLeftLambdaType>(
                             vec_size, num_tests_per_vec_size,
                             num_repititions_per_vec,
                             length_of_left_subvector_lambda,
                             range_lower_bound, range_upper_bound,
                             to, comp, times);
    times.FillInRatioOfTimesFromCurrentTimings();
    total_times.UpdateSmallestAndLargestRatiosOfTimesFromOtherTimings(times,
                                                                      vec_size);
    total_times.AddTimesAndCountersFromOtherTimingsToThisObject(times);
    if (to.print_total_average_time_for_each_vec_size) {
      total_times.FillInRatioOfTimesFromCurrentTimings();
      to.PrintString(total_times.GetInfoString(num_repititions_per_vec, to));
    }
    to.Flush();
    std::size_t next_vec_size = next_vec_size_lambda(vec_size, vec_size_count);
    vec_size = next_vec_size;
  } while (vec_size != 0);
  total_times.FillInRatioOfTimesFromCurrentTimings();

  to.PrintLine('*');
  to.PrintNewLines(10);
  std::ostringstream ostrm;
  ostrm << "Total Times:\n"
        << total_times.GetInfoString(1, to);
  to.PrintString(ostrm);
  to.PrintNewLines(10);
  to.Flush();
  return ;
}

#ifdef NUMBER_OF_UNTIMED_CALLS_TO_MERGE
#undef NUMBER_OF_UNTIMED_CALLS_TO_MERGE
#endif

#endif /* SRC_MERGE_TIME_H_ */
