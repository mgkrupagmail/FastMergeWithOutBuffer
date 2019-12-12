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
 *  1) MergeWithOutBuffer() (which could have been called MergeWithOutBufferTrim5())
 *  2) MergeWithOutBufferTrim4()
 *  3) MergeWithOutBufferTrim3()
 *  4) MergeWithOutBufferTrim2()
 *  5) MergeWithOutBufferTrim1()
 *  6) gnu::gnu__merge_without_buffer()
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

/* DESCRIPTION OF TIMING ALGORITHMS.
 * A structure TotalTimes is defined, which keeps track of the execution times
 *  of the various merge algorithms. We now describe details of TotalTimes:
 *  - Each merge algorithm has a corresponding time member variable (of type
 *    std::chrono::nanoseconds) associated with it (e.g. std::merge() has
 *    TotalTimes::std_merge, MergeWithOutBuffer() has
 *    TotalTimes::merge_without_buffer, etc.)
 *    where each of these times is initialized to 0 nanoseconds.
 *  - For each merge algorithm, there is a corresponding boolean member
 *    variable whose name begins with should_time_ (e.g.
 *    TotalTimes::should_time_std_merge, etc.), which if false results
 *    in all things related to that merge algorithm being ignored.
 *  - TotalTimes has static member variables to keep track of statistics.
 *    For instance,
 *    TotalTimes::largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer
 *    keeps track of the largest ratio of
 *    merge_without_buffer / gnu_merge_without_buffer.
 *  - TotalTimes::GetStringOfStaticVariables() returns a string describing
 *    the static member variables.
 *  - TotalTimes::UpdateStaticVariables() updates (if necessary) the static
 *    variables based on the values stored in the TotalTimes object's member
 *    variables.
 *  - TotalTimes::GetAveragesStr() Returns a string describing the TotalTimes
 *    object's member variables.
 */
/* Description of TimeMergesOnGivenVecSize():
 *
 * Say that a vector vec of size vec_size is (or consists of)
 *  ***two non-decreasing sequences*** if there is some integer start_right
 *   such that
 *   1) 0 < start_right < vec_size,
 *   2) vec[0], ..., vec[start_right - 1] is non-decreasing (this is called
 *      the ***left vector***
 *   3) vec[start_right], ..., vec[vec_size - 1] is non-decreasing (this is
 *      called the **right vector***
 *  We will not be dealing with the degenerate case where start_right == 0 or
 *  start_right == vec_size. The timing algorithm TimeMergesOnGivenVecSize()
 *  is performed on the given value of vec_size so assume that vec_size
 *  is fixed.
 *
 *  1) If start_right == -1 then start_right is replaced by vec_size / 2.
 *  2) If start_right  < -1 then start_right is replaced by a random integer
 *     between 1 and vec_size - 1 (inclusive).
 *  3) A vector of size vec_size called vec_original is allocated. A copy
 *     of vec_original called vec is allocated.
 *  - Note that the memory locations of vec_original's data will not change
 *    throughout the execution of TimeMergesOnGivenVecSize().
 *    Ditto for vec's data.
 *  4) A new TotalTimes object, called total_times, is created.
 *  5) The following is done num_tests_per_vec_size times, where each iteration
 *     is called a ***test***:
 *    a) vec_original is filled with random values in such a way that it will
 *       consist of two non-decreasing sequences.
 *       In detail: (I) vec_original is first filled with random data,
 *       (II) the first start_right elements of vec_original are sorted, and
 *       (III) the last vec_size - start_right elements of vec_original
 *             are sorted.
 *       Note that vec_original's data will not be changed for the rest
 *       of this test.
 *    b) vec_original's data is copied into vec.
 *    c) A call is made to TimeMergesOnGivenVec(), which times the execution
 *       of the various merge algorithms (more details below)
 *       - This function returns a TotalTimes object called times, which is
 *         then added to total_times.
 *    d) If pick_new_random_start_right_for_each_new_vec == true then
 *       start_right is replaced by a new random value.
 *  6) Information from the total_times object is printed to std::out
 *     and the total_times object is returned.
 */
/* Description of TimeMergesOnGivenVec():
 *
 *  - Inputs: a const reference to vec_original, a reference to vec, and
 *            the values start_right and num_repititions_per_vec.
 *  - For each merge function that is to be timed, the following is
 *    done num_repititions_per_vec times:
 *    1) A variable std::chrono::nanoseconds total is initialized to 0.
 *    2) At least one untimed call is made to the merge function. After
 *       each call, vec's data is restored to its original state.
 *      - This is done so as to load code and data into the cache.
 *        Note that this does actually affect timings.
 *    3) num_repititions timed calls to the merge function are executed.
 *      - The time of each call is added to the variable total.
 *      - After each call, vec is restored to its original state.
 *    4) The value of total is returned and execution returns to
 *       TimeMergesOnGivenVec().
 *      - The return value is added to the appropriate member variable
 *        of total_times.
 *    5) Once all merge functions have been timed, TimeMergesOnGivenVec()
 *       returns total_times and execution returns to
 *       TimeMergesOnGivenVecSize().
 */

//#include "merge_time_dev.h"
//#define SRC_MERGE_TIME_H_

#ifndef SRC_MERGE_TIME_H_
#define SRC_MERGE_TIME_H_

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "gnu_merge_without_buffer.h"
#include "merge_without_buffer.h"
#include "merge_without_buffer_trim4.h"
#include "merge_without_buffer_trim3.h"
#include "merge_without_buffer_trim2.h"
#include "merge_without_buffer_trim1.h"

#define NUMBER_OF_UNTIMED_CALLS_TO_MERGE 2

static const std::chrono::nanoseconds zero_nano{0};

template<class T>
inline void AssignRightVectorValuesToLeft(std::vector<T> &lhs,
                                          const std::vector<T> &rhs) {
  assert(lhs.size() >= rhs.size());
  for (auto it_lhs = lhs.begin(), it_rhs = rhs.begin();
            it_rhs != rhs.end(); ++it_lhs, ++it_rhs) {
    *it_lhs = *it_rhs;
  }
  return ;
}

/* NOTE: All of the following Time...() functions except for
 *  TimeStdMergeOnGivenVec()have near identical code where the only
 *  difference is the merge function that is called.
 * Note that TimeStdMergeOnGivenVec() additionally allocates a
 *  std::vector<T> temp_vector of size vec_size since std::merge()
 *  requires such a vector as input.
 *  - Since the allocation of temp_vector is a necessary part of
 *    std::merge()'s use, the time it takes to allocate temp_vector
 *    is included in the timing of std::merge().
 */

template<class T>
std::chrono::nanoseconds TimeStdInplaceMergeWithOutBufferOnGivenVec(
                    std::vector<T> &vec, const std::vector<T> &vec_original,
                    std::size_t start_right, std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  for (int counter = 0; counter < NUMBER_OF_UNTIMED_CALLS_TO_MERGE; ++counter)
  { //Do at least one call to the merge function to load code in caches.
    try {
        std::inplace_merge(vec.begin(), vec.begin() + start_right, vec.end());
    } catch (...) {
      std::cout << "std::inplace_merge() failed." << std::endl;
      std::terminate();
    }
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    try {
        std::inplace_merge(vec.begin(), vec.begin() + start_right, vec.end());
    } catch (...) {
      std::cout << "std::inplace_merge() failed." << std::endl;
      std::terminate();
    }
    total += std::chrono::high_resolution_clock::now() - start_time;
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  //Make sure that the loop wasn't optimized away.
  assert(num_repititions == 0 || total != zero_nano);
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeStdMergeOnGivenVec(std::vector<T> &vec,
                   const std::vector<T> &vec_original, std::size_t start_right,
                   std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  {
    for (int counter = 0; counter < NUMBER_OF_UNTIMED_CALLS_TO_MERGE; ++counter)
    { //Do at least one call to the merge function to load code in caches.
      std::vector<T> temp_vec(vec.size(), 0);
      std::merge(vec.begin(), vec.begin() + start_right,
                 vec.begin() + start_right, vec.end(), temp_vec.begin());
      AssignRightVectorValuesToLeft(vec, temp_vec);
      AssignRightVectorValuesToLeft(vec, vec_original);
    }
    for (std::size_t i = 0; i < num_repititions; i++) {
      auto start_time = std::chrono::high_resolution_clock::now();
      std::vector<T> temp_vec(vec.size());
      std::merge(vec.begin(), vec.begin() + start_right,
                 vec.begin() + start_right, vec.end(), temp_vec.begin());
      AssignRightVectorValuesToLeft(vec, temp_vec);
      total += std::chrono::high_resolution_clock::now() - start_time;
      AssignRightVectorValuesToLeft(vec, vec_original);
    }
    //Make sure that the loop wasn't optimized away.
    assert(num_repititions == 0 || total != zero_nano);
  }
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeMergeWithOutBufferOnGivenVec(std::vector<T> &vec,
                   const std::vector<T> &vec_original, std::size_t start_right,
                   std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  for (int counter = 0; counter < NUMBER_OF_UNTIMED_CALLS_TO_MERGE; ++counter)
  { //Do at least one call to the merge function to load code in caches.
    MergeWithOutBuffer<typename std::vector<T>::iterator,
                       typename std::vector<T>::iterator>(vec.begin(),
                       vec.begin() + (start_right - 1),
                       vec.begin() + start_right, vec.end() - 1);
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    MergeWithOutBuffer<typename std::vector<T>::iterator,
                       typename std::vector<T>::iterator>(vec.begin(),
                       vec.begin() + (start_right - 1),
                       vec.begin() + start_right, vec.end() - 1);
    total += std::chrono::high_resolution_clock::now() - start_time;
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  //Make sure that the loop wasn't optimized away.
  assert(num_repititions == 0 || total != zero_nano);
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeStdMergeWithOutBufferOnGivenVec(
                    std::vector<T> &vec, const std::vector<T> &vec_original,
                    std::size_t start_right, std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  auto comp = [](const T &lhs, const T &rhs) -> bool{return lhs < rhs;};
  for (int counter = 0; counter < NUMBER_OF_UNTIMED_CALLS_TO_MERGE; ++counter)
  { //Do at least one call to the merge function to load code in caches.
    gnu::gnu__merge_without_buffer(vec.begin(), vec.begin() + start_right,
                        vec.end(), start_right, vec.size() - start_right, comp);
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    gnu::gnu__merge_without_buffer(vec.begin(), vec.begin() + start_right,
                      vec.end(), start_right, vec.size() - start_right, comp);
    total += std::chrono::high_resolution_clock::now() - start_time;
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  //Make sure that the loop wasn't optimized away.
  assert(num_repititions == 0 || total != zero_nano);
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeMergeWithOutBuffer4OnGivenVec(std::vector<T> &vec,
                   const std::vector<T> &vec_original, std::size_t start_right,
                   std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  for (int counter = 0; counter < NUMBER_OF_UNTIMED_CALLS_TO_MERGE; ++counter)
  { //Do at least one call to the merge function to load code in caches.
    MergeWithOutBufferTrim4(vec.begin(), vec.begin() + (start_right - 1),
                            vec.begin() + start_right, vec.end() - 1);
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    MergeWithOutBufferTrim4(vec.begin(), vec.begin() + (start_right - 1),
                            vec.begin() + start_right, vec.end() - 1);
    total += std::chrono::high_resolution_clock::now() - start_time;
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  //Make sure that the loop wasn't optimized away.
  assert(num_repititions == 0 || total != zero_nano);
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeMergeWithOutBuffer3OnGivenVec(std::vector<T> &vec,
                   const std::vector<T> &vec_original, std::size_t start_right,
                   std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  for (int counter = 0; counter < NUMBER_OF_UNTIMED_CALLS_TO_MERGE; ++counter)
  { //Do at least one call to the merge function to load code in caches.
    MergeWithOutBufferTrim3(vec.begin(), vec.begin() + (start_right - 1),
                            vec.begin() + start_right, vec.end() - 1);
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    MergeWithOutBufferTrim3(vec.begin(), vec.begin() + (start_right - 1),
                            vec.begin() + start_right, vec.end() - 1);
    total += std::chrono::high_resolution_clock::now() - start_time;
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  //Make sure that the loop wasn't optimized away.
  assert(num_repititions == 0 || total != zero_nano);
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeMergeWithOutBuffer2OnGivenVec(std::vector<T> &vec,
                   const std::vector<T> &vec_original, std::size_t start_right,
                   std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  for (int counter = 0; counter < NUMBER_OF_UNTIMED_CALLS_TO_MERGE; ++counter)
  { //Do at least one call to the merge function to load code in caches.
    MergeWithOutBufferTrim2(vec.begin(), vec.begin() + (start_right - 1),
                            vec.begin() + start_right, vec.end() - 1);
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    MergeWithOutBufferTrim2(vec.begin(), vec.begin() + (start_right - 1),
                            vec.begin() + start_right, vec.end() - 1);
    total += std::chrono::high_resolution_clock::now() - start_time;
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  //Make sure that the loop wasn't optimized away.
  assert(num_repititions == 0 || total != zero_nano);
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeMergeWithOutBuffer1OnGivenVec(std::vector<T> &vec,
                  const std::vector<T> &vec_original,  std::size_t start_right,
                  std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  for (int counter = 0; counter < NUMBER_OF_UNTIMED_CALLS_TO_MERGE; ++counter)
  { //Do at least one call to the merge function to load code in caches.
    MergeWithOutBufferTrim1(vec.begin(), vec.begin() + (start_right - 1),
                            vec.begin() + start_right, vec.end() - 1);
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    MergeWithOutBufferTrim1(vec.begin(), vec.begin() + (start_right - 1),
                            vec.begin() + start_right, vec.end() - 1);
    total += std::chrono::high_resolution_clock::now() - start_time;
    AssignRightVectorValuesToLeft(vec, vec_original);
  }
  //Make sure that the loop wasn't optimized away.
  assert(num_repititions == 0 || total != zero_nano);
  return static_cast<std::chrono::nanoseconds>(total);
}

struct TotalTimes {
  typedef std::chrono::nanoseconds time_units;
  time_units std_merge{0};
  time_units std_inplace_merge{0};
  time_units merge_without_buffer{0};
  time_units merge_without_buffer4{0};
  time_units merge_without_buffer3{0};
  time_units merge_without_buffer2{0};
  time_units merge_without_buffer1{0};
  time_units gnu_merge_without_buffer{0};

  std::size_t total_number_of_times_each_merge_function_was_called = 0;

  static long double smallest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer;
  static std::size_t vec_size_of_smallest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer;
  static long double largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer;
  static std::size_t vec_size_of_largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer;

  bool should_time_merge_without_buffer  = true;
  bool should_time_merge_without_buffer1 = true;
  bool should_time_merge_without_buffer2 = true;
  bool should_time_merge_without_buffer3 = true;
  bool should_time_merge_without_buffer4 = true;
  bool should_time_std_inplace_merge = true;
  bool should_time_std_merge = true;
  bool should_time_gnu_merge_without_buffer = true;

  std::size_t max_nano_str_width = 1;

  TotalTimes &operator+=(const TotalTimes &rhs) {
    std_merge                 += rhs.std_merge;
    std_inplace_merge         += rhs.std_inplace_merge;
    merge_without_buffer      += rhs.merge_without_buffer;
    merge_without_buffer4     += rhs.merge_without_buffer4;
    merge_without_buffer3     += rhs.merge_without_buffer3;
    merge_without_buffer2     += rhs.merge_without_buffer2;
    merge_without_buffer1     += rhs.merge_without_buffer1;
    gnu_merge_without_buffer  += rhs.gnu_merge_without_buffer;
    total_number_of_times_each_merge_function_was_called +=
          rhs.total_number_of_times_each_merge_function_was_called;
    return *this;
  }
  template<class T> static std::size_t GetStringWidth(T value) {
    return std::to_string(value).length();
  }
  //Given a tume nano_time (in nanoseconds), it divides the time by divisor
  // and returns a string describing the resulting quotient
  // in nanoseconds, microseconds, milliseconds, and seconds.
  std::string GetTimeStr(time_units nano_time, std::size_t divisor) {
    std::stringstream strm;
    if (divisor == 0)
      divisor = 1;
    std::size_t nano = static_cast<std::size_t>(nano_time.count() / divisor);
    auto nano_str_length = GetStringWidth(nano);
    if (nano_str_length > max_nano_str_width)
      max_nano_str_width = nano_str_length;
    strm << std::setw(max_nano_str_width) << nano << " ns = ";
    strm << std::setw(max_nano_str_width > 3 ? max_nano_str_width - 3 : 1)
         << (nano/1000) << " mus = ";
    strm << std::setw(max_nano_str_width > 6 ? max_nano_str_width - 6 : 1)
         << (nano/1000000) << " ms = ";
    strm << std::setw(max_nano_str_width > 9 ? max_nano_str_width - 9 : 1)
         << (nano/1000000000) << " s";
    return strm.str();
  }
  //Generate a string listing the average time to perform each merge function.
  // divisor is the total number of times that each individual merge function
  // was called.
  std::string GetAveragesStr(std::size_t divisor) {
    if (divisor == 0)
      divisor = total_number_of_times_each_merge_function_was_called;
    if (divisor == 0)
      divisor = 1;
    std::stringstream strm;
    if (should_time_std_merge) {
      strm << "Merge algorithms that use a buffer:\n";
      strm << "std::merge ave               = "
           << GetTimeStr(std_merge, divisor) << '\n';
    }

    if (should_time_std_inplace_merge) {
      strm << "Merge algorithms that use a buffer if available:\n";
      strm << "std::inplace_merge ave       = "
           << GetTimeStr(std_inplace_merge, divisor) << '\n';
    }

    strm << "Merge algorithms that do not use a buffer:\n";
    if (should_time_merge_without_buffer) {
      strm << "merge_without_buffer ave     = "
           << GetTimeStr(merge_without_buffer, divisor) << '\n';
    }
    if (should_time_merge_without_buffer4) {
      strm << "merge_without_buffer4 ave    = "
           << GetTimeStr(merge_without_buffer4, divisor) << '\n';
    }
    if (should_time_merge_without_buffer3) {
      strm << "merge_without_buffer3 ave    = "
           << GetTimeStr(merge_without_buffer3, divisor) << '\n';
    }
    if (should_time_merge_without_buffer2) {
      strm << "merge_without_buffer2 ave    = "
           << GetTimeStr(merge_without_buffer2, divisor) << '\n';
    }
    if (should_time_merge_without_buffer1) {
      strm << "merge_without_buffer1 ave    = "
           << GetTimeStr(merge_without_buffer1, divisor) << '\n';
    }
    if (should_time_gnu_merge_without_buffer) {
      strm << "gnu_merge_without_buffer ave = "
           << GetTimeStr(gnu_merge_without_buffer, divisor) << '\n';
    }
    return strm.str();
  }

  void UpdateStaticVariables(std::size_t vec_size, std::size_t divisor = 1, bool verbose = true) {
    if (gnu_merge_without_buffer !=  zero_nano) {
      //Don't need to include division by divisor since it would just cancel out.
      auto merge_without_buffer_count = merge_without_buffer.count();
      auto gnu_merge_without_buffer_count = gnu_merge_without_buffer.count();
      long double gnu_merge_without_buffer_long_double
                 = static_cast<long double>(gnu_merge_without_buffer_count);
      if (gnu_merge_without_buffer_long_double > 0.0l) {
        long double ratio = merge_without_buffer_count
                            / gnu_merge_without_buffer_long_double;
        if (ratio < smallest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer) {
          if (verbose) {
            std::cout << "New smallest ratio of merge_without_buffer / gnu_merge_without_buffer = "
                      << ratio
                      << " occurred when vec_size = " << vec_size << "\n";
          }
          smallest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer = ratio;
          vec_size_of_smallest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer = vec_size;
        }
        if (ratio > largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer) {
          if (verbose) {
            std::cout << "New largest ratio of merge_without_buffer / gnu_merge_without_buffer = "
                      << ratio
                      << " occurred when vec_size = " << vec_size << "\n";
          }
          largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer = ratio;
          vec_size_of_largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer = vec_size;
        }
      }
    }
  }

  void ResetStaticVariables() {
    smallest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer
      = std::numeric_limits<long double>::max();
    vec_size_of_smallest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer = 0;
    largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer = -1.0l;
    vec_size_of_largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer = 0;
  }

  //Construct a string describing the static variables.
  static std::string GetStringOfStaticVariables() {
    std::stringstream strm;
    strm << "Smallest ratio of merge_without_buffer / gnu_merge_without_buffer = \t"
         << smallest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer
         << " \toccurred when vec_size = \t"
         << vec_size_of_smallest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer
         << "\n";
    strm << "Largest  ratio of merge_without_buffer / gnu_merge_without_buffer = \t"
         << largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer
         << " \toccurred when vec_size = \t"
         << vec_size_of_largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer
         << "\n";
    return strm.str();
  }
};

template<class T> inline TotalTimes TimeMergesOnGivenVec(std::vector<T> &vec,
                                      const std::vector<T> &vec_original,
                                      std::size_t start_right,
                                      std::size_t num_repititions_per_vec = 1) {
  TotalTimes total_times;
  total_times.total_number_of_times_each_merge_function_was_called += num_repititions_per_vec;
  if (total_times.should_time_std_merge) {
    try {
      total_times.std_merge += TimeStdMergeOnGivenVec<T>(vec, vec_original,
                                           start_right, num_repititions_per_vec);
    } catch (...) {
      std::cout << "TimeStdMergeOnGivenVec() Failed." << std::endl;
    }
  }
  if (total_times.should_time_std_inplace_merge) {
    try {
      total_times.std_inplace_merge += TimeStdInplaceMergeWithOutBufferOnGivenVec<T>(
                        vec, vec_original, start_right, num_repititions_per_vec);
    } catch (...) {
      std::cout << "TimeStdInplaceMergeWithOutBufferOnGivenVec() Failed."
                << std::endl;
    }
  }
  if (total_times.should_time_merge_without_buffer) {
    try {
      total_times.merge_without_buffer += TimeMergeWithOutBufferOnGivenVec<T>(vec,
                             vec_original, start_right, num_repititions_per_vec);
    } catch (...) {
      std::cout << "TimeMergeWithOutBufferOnGivenVec() Failed." << std::endl;
    }
  }
  if (total_times.should_time_merge_without_buffer4) {
    try {
      total_times.merge_without_buffer4 += TimeMergeWithOutBuffer4OnGivenVec<T>(vec,
                             vec_original, start_right, num_repititions_per_vec);
    } catch (...) {
      std::cout << "TimeMergeWithOutBuffer4OnGivenVec() Failed." << std::endl;
    }
  }
  if (total_times.should_time_merge_without_buffer3) {
    try {
      total_times.merge_without_buffer3 += TimeMergeWithOutBuffer3OnGivenVec<T>(vec,
                             vec_original, start_right, num_repititions_per_vec);
    } catch (...) {
      std::cout << "TimeMergeWithOutBuffer3OnGivenVec() Failed." << std::endl;
    }
  }
  if (total_times.should_time_merge_without_buffer2) {
    try {
      total_times.merge_without_buffer2 += TimeMergeWithOutBuffer2OnGivenVec<T>(vec,
                             vec_original, start_right, num_repititions_per_vec);
    } catch (...) {
      std::cout << "TimeMergeWithOutBuffer2OnGivenVec() Failed." << std::endl;
    }
  }
  if (total_times.should_time_merge_without_buffer1) {
    try {
      total_times.merge_without_buffer1 += TimeMergeWithOutBuffer1OnGivenVec<T>(vec,
                             vec_original, start_right, num_repititions_per_vec);
    } catch (...) {
      std::cout << "TimeMergeWithOutBuffer1OnGivenVec() Failed." << std::endl;
    }
  }
  if (total_times.should_time_gnu_merge_without_buffer) {
    try {
      total_times.gnu_merge_without_buffer += TimeStdMergeWithOutBufferOnGivenVec<T>(
                        vec, vec_original, start_right, num_repititions_per_vec);
    } catch (...) {
      std::cout << "TimeStdMergeWithOutBufferOnGivenVec() Failed." << std::endl;
    }
  }
  return total_times;
}

long double TotalTimes::smallest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer
= std::numeric_limits<long double>::max();
std::size_t TotalTimes::vec_size_of_smallest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer = 0;

long double TotalTimes::largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer
= -1.0l;
std::size_t TotalTimes::vec_size_of_largest_ratio_of_merge_without_buffer_over_gnu_merge_without_buffer = 0;



/* Two vectors of size vec_size are created. The following is then done num_tests_per_vec_size times:
   1) vec_original is filled with random data.
   2) vec_original is split into 2 parts (a left part and a right part) based on the value of start_right.
   3) The left part of vec_original is sorted. The right part of vec_original is sorted.
   4) A copy of vec_original is made and stored in the variable vec.
   5) TimeMergesOnGivenVec() is called, which calls and times various merge algorithms on vec.
      The resulting times are added to total_times.

 Example call
  std::size_t vec_size, num_tests_per_vec_size, num_repititions_per_vec;
  auto start_right = -2;
  auto pick_new_random_start_right_for_each_new_vec = true;
  auto print_vec_averages = false;
  auto print_total_averages = true;
  vec_size = (1u << 6);
  num_tests_per_vec_size = (1u << 10);
  num_tests_per_vec_size = (1u << 8);
  TimeMergesOnGivenVecSize<int>(vec_size, num_tests_per_vec_size, num_repititions_per_vec,
                    start_right, pick_new_random_start_right_for_each_new_vec,
                    print_vec_averages, print_total_averages, false,
                    0, 100 * vec_size);
 */
template<class T> TotalTimes TimeMergesOnGivenVecSize(std::size_t vec_size,
                    std::size_t num_tests_per_vec_size = 1,
                    std::size_t num_repititions_per_vec = 1,
                    int start_right = -1, //set to -1 for the midpoint, and set
                                          //it to be < -1 to pick it randomly.
                    bool pick_new_random_start_right_for_each_new_vec = false,
                    bool print_vec_averages = false,
                    bool print_total_averages = true,
                    bool verbose = false,
                    bool print_vec_original = false,
                    T value_lower_bound = std::numeric_limits<T>::min(),
                    T value_upper_bound = std::numeric_limits<T>::max()) {
  if (start_right == -1) {
    start_right = vec_size / 2;
  } else if (start_right < -1) {
    std::random_device rnd_device;
    std::mt19937 generator(rnd_device());
    std::uniform_int_distribution<int> dist(1, vec_size - 1);
    start_right = dist(generator);
  }
  if (start_right == 0) //Then there'd be no left vector.
    start_right = 1; //Make the left vector have size 1.
  assert(start_right < static_cast<int>(vec_size));
  std::vector<T> vec_original;
  std::vector<T> vec;
  TotalTimes total_times;

  try {
    vec_original = std::vector<T>(vec_size);
  } catch (...) {
    std::cout << "Unable to allocate vector vec_original of size "
              << vec_size << std::endl;
    return total_times;
  }

  try {
    vec = vec_original;
  } catch (...) {
    std::cout << "Unable to create a copy of vector vec_original of size "
              << vec_size << std::endl;
    return total_times;
  }

  //const T * const vec_original_data_pointer_saved = vec_original.data();
  //const T * const vec_data_pointer_saved = vec.data();

  for (std::size_t i = 0; i < num_tests_per_vec_size; i++) {
    FillWithRandomNumbers(vec_original.begin(), vec_original.end(), value_lower_bound,
                          value_upper_bound);
    auto end_left    = start_right - 1;
    int length_left  = start_right;
    int length_right = vec_size - start_right;
    //Sort the left and right subvectors
    std::sort(vec_original.begin(), vec_original.begin() + start_right);
    std::sort(vec_original.begin() + start_right, vec_original.end());
    AssignRightVectorValuesToLeft(vec, vec_original);
    if (verbose) {
      PrintLine("_");
      std::cout << "start_left = 0 \tend_left =\t" << end_left
                << "\tstart_right =\t" << start_right
                << "\tend_right =\t"   << (vec_size - 1)  << std::endl;
      if (print_vec_original) {
        PrintNondecreasingSubsequences(vec.begin(), length_left, false);
        PrintNondecreasingSubsequences(vec.begin() + start_right, length_right,
                                       true);
      }
    }
    auto times = TimeMergesOnGivenVec<T>(vec, vec_original, start_right,
                                      num_repititions_per_vec);
    total_times += times;

    if (print_vec_averages) {
      if (verbose) {
        std::cout << "Times for this particular vector: \n";
      }
      std::cout << times.GetAveragesStr(num_repititions_per_vec);
    }
    if (pick_new_random_start_right_for_each_new_vec) {
      std::random_device rnd_device;
      std::mt19937 generator(rnd_device());
      std::uniform_int_distribution<int> dist(1, vec_size - 1);
      start_right =  dist(generator);
    }
  }
  //Make sure all of the above code that was executed used the same memory locations.
  //assert(vec_original_data_pointer_saved == vec_original.data());
  //assert(vec_data_pointer_saved == vec.data());

  total_times.UpdateStaticVariables(vec_size);
  if (print_total_averages) {
    PrintLine("_");
    std::cout << "Times for merging " << num_tests_per_vec_size
              << " vectors of combined size " << vec_size << ". ";
    std::cout << "The sizes of the two component vectors ";
    if (pick_new_random_start_right_for_each_new_vec && num_tests_per_vec_size > 1) {
      std::cout << "DID";
    } else {
      std::cout << "did NOT";
    }
    std::cout << " vary between different values of vec_original.\n";
    std::cout << "Each merge algorithm was called " << num_repititions_per_vec
              << " times for each value of vec_original, so \n";
    std::cout << "each merge algorithm was called a total "
              << (num_repititions_per_vec * num_tests_per_vec_size)
              << " times.\n";
    std::cout << total_times.GetAveragesStr(num_repititions_per_vec
                                            * num_tests_per_vec_size);
    PrintLine("_");
    PrintLine("_");
    std::cout << TotalTimes::GetStringOfStaticVariables();
    std::cout << "\n";
  }
  std::cout.flush();
  return total_times;
}


template<class T>
inline T ForceValueToBeWithinBounds(T value, T minimum_value, T maximum_value) {
  if (value <= minimum_value)
    return minimum_value;
  else if (value >= maximum_value)
    return maximum_value;
  else
    return value;
}

#define MINIMUM_vec_size static_cast<std::size_t>(1u << 1)
#define MAXIMUM_vec_size static_cast<std::size_t>(1u << 23)
auto default_next_vec_size_lambda = [](std::size_t cur_vec_size,
           const long double vec_size_scale = 1.4l) -> std::size_t {
  long double v_ld = static_cast<long double>(cur_vec_size) * vec_size_scale;
  if (v_ld >= static_cast<long double>(MAXIMUM_vec_size))
    return MAXIMUM_vec_size;
  std::size_t v = v_ld;
  v = ForceValueToBeWithinBounds<std::size_t>(v, MINIMUM_vec_size, MAXIMUM_vec_size);
  return v;
};

#define MINIMUM_num_tests_per_vec_size static_cast<std::size_t>(1u << 10)
#define MAXIMUM_num_tests_per_vec_size static_cast<std::size_t>(1u << 17)
auto default_num_tests_per_vec_size_lambda = [](std::size_t cur_vec_size) -> std::size_t {
  if (cur_vec_size + 1 >= MAXIMUM_num_tests_per_vec_size)
    return 1;
  std::size_t v = (MAXIMUM_num_tests_per_vec_size / (cur_vec_size + 1)) + 1;
  v = ForceValueToBeWithinBounds<std::size_t>(v,
                 MINIMUM_num_tests_per_vec_size, MAXIMUM_num_tests_per_vec_size);
  return v;
};

#define MINIMUM_num_repititions_per_vec static_cast<std::size_t>(1u << 4)
#define MAXIMUM_num_repititions_per_vec static_cast<std::size_t>(1u << 8)
auto default_num_repititions_per_vec_lambda = [](std::size_t cur_vec_size) -> std::size_t {
  std::size_t v = (MAXIMUM_vec_size / (cur_vec_size + 1)) + 1;
  v = ForceValueToBeWithinBounds<std::size_t>(v,
                   MINIMUM_num_repititions_per_vec, MAXIMUM_num_repititions_per_vec);
  return v;
};

#define MINIMUM_value_upper_bound 0
#define MAXIMUM_value_upper_bound std::numeric_limits<T>::max()
#define SCALE_value_upper_bound 1.2l

/*Example call:

  typedef double ValueType;
  std::size_t vec_size_start = 2;

  int start_right = -2;
  bool pick_new_random_start_right_for_each_new_vec = true;
  auto next_vec_size_lambda = default_next_vec_size_lambda;
  auto num_tests_per_vec_size_lambda = default_num_tests_per_vec_size_lambda;
  auto num_repititions_per_vec_lambda = default_num_repititions_per_vec_lambda;

  bool print_vec_averages = false;
  bool print_total_averages = true;
  bool verbose = false;
  bool print_vec_original = false;
  long double const_to_scale_vec_size_by = 1.2l;
  ValueType value_lower_bound = 0;
  auto total_times = TimeMergeFunctions<ValueType>(vec_size_start,
                    start_right, pick_new_random_start_right_for_each_new_vec,
                    next_vec_size_lambda, num_tests_per_vec_size_lambda,
                    num_repititions_per_vec_lambda,
                    print_vec_averages, print_total_averages, verbose,
                    print_vec_original,
                    const_to_scale_vec_size_by, value_lower_bound);
*/
/* This function will do the following until the values that vec_size
 *  takes on cease to strictly increase.
 *  1) It will initialize vec_size to vec_size_start.
 *  2) It will use num_tests_per_vec_size_lambda  to generate a
 *     value num_tests_per_vec_size.
 *  3) It will use num_repititions_per_vec_lambda to generate a
 *     value num_repititions_per_vec.
 *  4) It will eventually call TimeMergesOnGivenVecSize() where
 *     the random values of the vectors will be taken from the range
 *     [range_lower_bound, range_upper_bound], which TimeMergeFunctions()
 *     defines.
 *     - In particular, if const_to_scale_vec_size_by > 0.0l then
 *       range_lower_bound will be set to 0 and range_upper_bound will be
 *       set to const_to_scale_vec_size_by * vec_size.
 *  5) It will call TimeMergesOnGivenVecSize() with the obvious
 *     input and add the TotalTimes object it returns to total_times.
 *  6) It will call next_vec_size_lambda() to get the value of vec_size
 *     for the next iteration of the loop (or it may leave the loop).
 * It will terminate by returning total_times.
 */
template<class T>
TotalTimes TimeMergeFunctions(
      std::size_t vec_size_start = 2,
      int start_right = -1, //set to -1 for the midpoint, and set
                            //it to be < -1 to pick the first length of the left vector randomly.
      bool pick_new_random_start_right_for_each_new_vec = false,
      decltype(default_next_vec_size_lambda) next_vec_size_lambda
                                           = default_next_vec_size_lambda,
      decltype(default_num_tests_per_vec_size_lambda) num_tests_per_vec_size_lambda
                                           = default_num_tests_per_vec_size_lambda,
      decltype(default_num_repititions_per_vec_lambda) num_repititions_per_vec_lambda
                                           = default_num_repititions_per_vec_lambda,
      bool print_vec_averages = false,
      bool print_total_averages = true,
      bool verbose = false,
      bool print_vec_original = false,
      long double const_to_scale_vec_size_by = 1.2l, //If <= 0 then
                      // value_upper_bound is not replaced by
                      // const_to_scale_vec_size_by * vec_size
      T value_lower_bound = std::numeric_limits<T>::min(),
      T value_upper_bound = std::numeric_limits<T>::max()
      ) {
  TotalTimes total_times;
  std::size_t vec_size = vec_size_start;
  vec_size = ForceValueToBeWithinBounds<std::size_t>(vec_size,
                                 MINIMUM_vec_size, MAXIMUM_vec_size);
  while (vec_size <= MAXIMUM_vec_size) {
    std::size_t num_tests_per_vec_size = num_tests_per_vec_size_lambda(vec_size);
    std::size_t num_repititions_per_vec = num_repititions_per_vec_lambda(vec_size);
    std::cout << "vec_size = " << vec_size << '\n';
    std::cout << "num_tests_per_vec_size = " << num_tests_per_vec_size;
    std::cout << " \tnum_repititions_per_vec = \t"
              << num_repititions_per_vec << '\n';
    std::cout.flush();
    T range_lower_bound = value_lower_bound;
    T range_upper_bound = value_upper_bound;
    if (const_to_scale_vec_size_by > 0.0l) { //Then replace range_upper_bound
                                 // by const_to_scale_vec_size_by * vec_size and
                                 // replace range_lower_bound by 0.
      range_lower_bound = 0;
      //Check if const_to_scale_vec_size_by * vec_size will overflow
      if (!(static_cast<long double>(std::numeric_limits<T>::max())
                      / static_cast<long double>(vec_size)
                      < const_to_scale_vec_size_by)) {
        long double new_range_upper_bound_ld =
                 const_to_scale_vec_size_by
                 * static_cast<long double>(vec_size); //No overflow.
        if (new_range_upper_bound_ld <=
             static_cast<long double>(std::numeric_limits<T>::max()))
          range_upper_bound = static_cast<T>(new_range_upper_bound_ld);
      }
    }
    if (range_upper_bound > MAXIMUM_value_upper_bound)
      range_upper_bound = MAXIMUM_value_upper_bound;
    TotalTimes times = TimeMergesOnGivenVecSize<T>(vec_size,
                   num_tests_per_vec_size, num_repititions_per_vec,
                   start_right, pick_new_random_start_right_for_each_new_vec,
                   print_vec_averages, print_total_averages,
                   verbose, print_vec_original,
                   range_lower_bound, range_upper_bound);
    std::cout << '\n';
    std::cout.flush();
    total_times += times;
    std::size_t next_vec_size = next_vec_size_lambda(vec_size);
    if (next_vec_size <= vec_size || vec_size >= MAXIMUM_vec_size)
      break ;
    vec_size = next_vec_size;
  }
  return total_times;
}


/*Example call:

  typedef double ValueType;
  std::size_t vec_size_lower_bound = 2;
  std::size_t vec_size_upper_bound = (1u << 23);
  std::size_t number_of_random_vec_sizes = (1u << 20);

  int start_right = -2; //For the first vector, pick the lengths
                        // of the left and right vectors randomly.
  bool pick_new_random_start_right_for_each_new_vec = true;
  auto num_tests_per_vec_size_lambda = default_num_tests_per_vec_size_lambda;
  auto num_repititions_per_vec_lambda = default_num_repititions_per_vec_lambda;

  bool print_vec_averages = false;
  bool print_total_averages = true;
  bool verbose = false;
  bool print_vec_original = false;
  long double const_to_scale_vec_size_by = 1.2l;
  ValueType value_lower_bound = 0;
  auto total_times = TimeMergeFunctionsOnRandomVecSizes<ValueType>(
          vec_size_lower_bound, vec_size_upper_bound,
          number_of_random_vec_sizes,
          start_right, pick_new_random_start_right_for_each_new_vec,
          num_tests_per_vec_size_lambda, num_repititions_per_vec_lambda,
          print_vec_averages, print_total_averages, verbose, print_vec_original,
          const_to_scale_vec_size_by, value_lower_bound);
*/
/* This function will do the following number_of_random_vec_sizes times:
 *  1) It will pick a random value of vec_size in
 *     [vec_size_lower_bound, vec_size_upper_bound]
 *  2) It will use num_tests_per_vec_size_lambda  to generate a value
 *     num_tests_per_vec_size.
 *  3) It will use num_repititions_per_vec_lambda to generate a value
 *     num_repititions_per_vec.
 *  4) It will call TimeMergesOnGivenVecSize() with the obvious input
 *      and add the TotalTimes object it returns to total_times.
 * It will terminate by returning total_times.
 */
template<class T>
TotalTimes TimeMergeFunctionsOnRandomVecSizes(
                  std::size_t vec_size_lower_bound = MINIMUM_vec_size,
                  std::size_t vec_size_upper_bound = MAXIMUM_vec_size,
                  std::size_t number_of_random_vec_sizes = (1u << 16),
                  const int start_right = -1, //set to -1 for the midpoint, and set
                                              //it to be < -1 to pick the first
                                              // length of the left vector randomly.
                  bool pick_new_random_start_right_for_each_new_vec = true,
                  decltype(default_num_tests_per_vec_size_lambda) num_tests_per_vec_size_lambda
                                           = default_num_tests_per_vec_size_lambda,
                  decltype(default_num_repititions_per_vec_lambda) num_repititions_per_vec_lambda
                                           = default_num_repititions_per_vec_lambda,
                  bool print_vec_averages = false,
                  bool print_total_averages = true,
                  bool verbose = false,
                  bool print_vec_original = false,
                  long double const_to_scale_vec_size_by = 1.2l, //If <= 0 then
                                 // value_upper_bound is not replaced
                                 // by const_to_scale_vec_size_by * vec_size
                  T value_lower_bound = std::numeric_limits<T>::min(),
                  T value_upper_bound = std::numeric_limits<T>::max()
                  ) {
  TotalTimes total_times;
  if (vec_size_upper_bound > MAXIMUM_vec_size) {
    std::cout << "Reduced vec_size_upper_bound from "
              << vec_size_upper_bound << " to " << MAXIMUM_vec_size
              << std::endl;
    vec_size_upper_bound = MAXIMUM_vec_size;
  }
  if (vec_size_lower_bound < MINIMUM_vec_size) {
    std::cout << "Increased vec_size_lower_bound from "
              << vec_size_lower_bound << " to " << MINIMUM_vec_size
              << std::endl;
    vec_size_lower_bound = MINIMUM_vec_size;
  }
  std::size_t vec_size;
  std::random_device rnd_device;
  std::mt19937 generator(rnd_device());
  std::uniform_int_distribution<int> vec_size_dist(vec_size_lower_bound,
                                                   vec_size_upper_bound);
  for (std::size_t counter = 0; counter < number_of_random_vec_sizes; ++counter) {
    vec_size = vec_size_dist(generator);
    vec_size = ForceValueToBeWithinBounds<std::size_t>(vec_size,
                                    MINIMUM_vec_size, MAXIMUM_vec_size);
    auto new_start_right = start_right;
    if (start_right == -1 || start_right >= vec_size) {
      new_start_right = vec_size / 2;
    } else if (start_right < -1) {
      std::uniform_int_distribution<int> start_right_dist(1, vec_size - 1);
      new_start_right = start_right_dist(generator);
    }
    T range_lower_bound = value_lower_bound;
    T range_upper_bound = value_upper_bound;
    if (const_to_scale_vec_size_by > 0.0l) { //Then replace range_upper_bound
                                          // by const_to_scale_vec_size_by * vec_size
                                          // and replace range_lower_bound by 0.
      range_lower_bound = 0;
      //Check if const_to_scale_vec_size_by * vec_size will overflow
      if (!(static_cast<long double>(std::numeric_limits<T>::max())
           / static_cast<long double>(vec_size) < const_to_scale_vec_size_by)) {
        long double new_range_upper_bound_ld = const_to_scale_vec_size_by
                                     * static_cast<long double>(vec_size); //No overflow.
        if (new_range_upper_bound_ld <= static_cast<long double>(std::numeric_limits<T>::max()))
          range_upper_bound = static_cast<T>(new_range_upper_bound_ld);
      }
    }
    if (range_upper_bound > MAXIMUM_value_upper_bound)
      range_upper_bound = MAXIMUM_value_upper_bound;
    std::size_t num_tests_per_vec_size = num_tests_per_vec_size_lambda(vec_size);
    std::size_t num_repititions_per_vec = num_repititions_per_vec_lambda(vec_size);
    std::cout << "vec_size = " << vec_size << '\n';
    std::cout << "num_tests_per_vec_size = " << num_tests_per_vec_size;
    std::cout << " \tnum_repititions_per_vec = \t" << num_repititions_per_vec << '\n';
    std::cout.flush();
    TotalTimes times = TimeMergesOnGivenVecSize<T>(vec_size, num_tests_per_vec_size,
        num_repititions_per_vec, new_start_right,
        pick_new_random_start_right_for_each_new_vec,
        print_vec_averages, print_total_averages,
        verbose, print_vec_original,
        range_lower_bound, range_upper_bound);
    std::cout << '\n';
    std::cout.flush();
    total_times += times;
  }
  return total_times;
}

#endif /* SRC_MERGE_TIME_H_ */
