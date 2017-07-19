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
 *  1) MergeWithOutBuffer() (which could have been called
 *                                                    MergeWithOutBufferTrim5())
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
#include "misc_helpers.h"

template<class T>
std::chrono::nanoseconds TimeStdInplaceMergeWithOutBufferOnGivenVec(
                    std::vector<T> &vec, const std::vector<T> &vec_original,
                    std::size_t start_second, std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  {
  std::inplace_merge(vec.begin(), vec.begin() + start_second, vec.end());
  vec = vec_original;
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::inplace_merge(vec.begin(), vec.begin() + start_second, vec.end());
    total += std::chrono::high_resolution_clock::now() - start_time;
    vec = vec_original;
  }
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeStdMergeOnGivenVec(std::vector<T> &vec,
                   const std::vector<T> &vec_original, std::size_t start_second,
                   std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  {
  std::vector<T> temp_vec(vec.size());
  std::merge(vec.begin(), vec.begin() + start_second,
             vec.begin() + start_second, vec.end(), temp_vec.begin());
  vec = vec_original;
  }
  //volatile T vol_time = 0;
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<T> temp_vec(vec.size());
    std::merge(vec.begin(), vec.begin() + start_second,
               vec.begin() + start_second, vec.end(), temp_vec.begin());
    vec = temp_vec;
    total += std::chrono::high_resolution_clock::now() - start_time;
    vec = vec_original;
  }
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeMergeWithOutBufferOnGivenVec(std::vector<T> &vec,
                   const std::vector<T> &vec_original, std::size_t start_second,
                   std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  {
  MergeWithOutBuffer(vec.begin(), vec.begin() + (start_second - 1),
                     vec.begin() + start_second, vec.end());
  vec = vec_original;
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    MergeWithOutBuffer(vec.begin(), vec.begin() + (start_second - 1),
                       vec.begin() + start_second, vec.end());
    total += std::chrono::high_resolution_clock::now() - start_time;
    vec = vec_original;
  }
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeStdMergeWithOutBufferOnGivenVec(
                    std::vector<T> &vec, const std::vector<T> &vec_original,
                    std::size_t start_second, std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  auto comp = [](const T &lhs, const T &rhs) -> bool{return lhs < rhs;};
  {
  gnu::gnu__merge_without_buffer(vec.begin(), vec.begin() + start_second,
                      vec.end(), start_second, vec.size() - start_second, comp);
  vec = vec_original;
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    gnu::gnu__merge_without_buffer(vec.begin(), vec.begin() + start_second,
                      vec.end(), start_second, vec.size() - start_second, comp);
    total += std::chrono::high_resolution_clock::now() - start_time;
    vec = vec_original;
  }
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeMergeWithOutBuffer4OnGivenVec(std::vector<T> &vec,
                   const std::vector<T> &vec_original, std::size_t start_second,
                   std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  {
  MergeWithOutBufferTrim4(vec.begin(), vec.begin() + (start_second - 1),
                          vec.begin() + start_second, vec.end());
  vec = vec_original;
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    MergeWithOutBufferTrim4(vec.begin(), vec.begin() + (start_second - 1),
                            vec.begin() + start_second, vec.end());
    total += std::chrono::high_resolution_clock::now() - start_time;
    vec = vec_original;
  }
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeMergeWithOutBuffer3OnGivenVec(std::vector<T> &vec,
                   const std::vector<T> &vec_original, std::size_t start_second,
                   std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  {
  MergeWithOutBufferTrim3(vec.begin(), vec.begin() + (start_second - 1),
                          vec.begin() + start_second, vec.end());
  vec = vec_original;
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    MergeWithOutBufferTrim3(vec.begin(), vec.begin() + (start_second - 1),
                            vec.begin() + start_second, vec.end());
    total += std::chrono::high_resolution_clock::now() - start_time;
    vec = vec_original;
  }
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeMergeWithOutBuffer2OnGivenVec(std::vector<T> &vec,
                   const std::vector<T> &vec_original, std::size_t start_second,
                   std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  {
  MergeWithOutBufferTrim2(vec.begin(), vec.begin() + (start_second - 1),
                          vec.begin() + start_second, vec.end());
  vec = vec_original;
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    MergeWithOutBufferTrim2(vec.begin(), vec.begin() + (start_second - 1),
                            vec.begin() + start_second, vec.end());
    total += std::chrono::high_resolution_clock::now() - start_time;
    vec = vec_original;
  }
  return static_cast<std::chrono::nanoseconds>(total);
}

template<class T>
std::chrono::nanoseconds TimeMergeWithOutBuffer1OnGivenVec(std::vector<T> &vec,
                  const std::vector<T> &vec_original,  std::size_t start_second,
                  std::size_t num_repititions = 1) {
  std::chrono::nanoseconds total{0};
  {
  MergeWithOutBufferTrim1(vec.begin(), vec.begin() + (start_second - 1),
                                         vec.begin() + start_second, vec.end());
  vec = vec_original;
  }
  for (std::size_t i = 0; i < num_repititions; i++) {
    auto start_time = std::chrono::high_resolution_clock::now();
    MergeWithOutBufferTrim1(vec.begin(), vec.begin() + (start_second - 1),
                            vec.begin() + start_second, vec.end());
    total += std::chrono::high_resolution_clock::now() - start_time;
    vec = vec_original;
  }
  return static_cast<std::chrono::nanoseconds>(total);
}

struct TotalTimes {
  std::chrono::nanoseconds std_merge{0};
  std::chrono::nanoseconds std_inplace_merge{0};
  std::chrono::nanoseconds merge_without_buffer{0};
  std::chrono::nanoseconds merge_without_buffer4{0};
  std::chrono::nanoseconds merge_without_buffer3{0};
  std::chrono::nanoseconds merge_without_buffer2{0};
  std::chrono::nanoseconds merge_without_buffer1{0};
  std::chrono::nanoseconds gnu_merge_without_buffer{0};

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
    return *this;
  }
  template<class T> static std::size_t GetStringWidth(T value) {
    return std::to_string(value).length();
  }
  std::string GetTimeStr(std::chrono::nanoseconds nano_time, std::size_t divisor) {
    std::stringstream strm;
    if (divisor == 0.0l)
      divisor = 1.0l;
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
  std::string GetAveragesStr(std::size_t divisor) {
    std::stringstream strm;
    strm << "Merge algorithms that use a buffer:\n";
    strm << "std::merge ave               = "
         << GetTimeStr(std_merge, divisor) << '\n';
    strm << "Merge algorithms that use a buffer if available:\n";
    strm << "std::inplace_merge ave       = "
         << GetTimeStr(std_inplace_merge, divisor) << '\n';
    strm << "Merge algorithms that do not use a buffer:\n";
    strm << "merge_without_buffer ave     = "
         << GetTimeStr(merge_without_buffer, divisor) << '\n';
    strm << "merge_without_buffer4 ave    = "
         << GetTimeStr(merge_without_buffer4, divisor) << '\n';
    strm << "merge_without_buffer3 ave    = "
         << GetTimeStr(merge_without_buffer3, divisor) << '\n';
    strm << "merge_without_buffer2 ave    = "
         << GetTimeStr(merge_without_buffer2, divisor) << '\n';
    strm << "merge_without_buffer1 ave    = "
         << GetTimeStr(merge_without_buffer1, divisor) << '\n';
    strm << "gnu_merge_without_buffer ave = "
         << GetTimeStr(gnu_merge_without_buffer, divisor) << '\n';
    return strm.str();
  }
};

template<class T> inline TotalTimes TimeMergesOnGivenVec(std::vector<T> &vec,
                                      const std::vector<T> &vec_original,
                                      std::size_t start_second,
                                      std::size_t num_repititions_per_vec = 1) {
  TotalTimes total_times;
  try {
    total_times.std_merge += TimeStdMergeOnGivenVec(vec, vec_original,
                                         start_second, num_repititions_per_vec);
  } catch (...) {
    std::cout << "TimeStdMergeOnGivenVec() Failed." << std::endl;
  }
  try {
    total_times.std_inplace_merge += TimeStdInplaceMergeWithOutBufferOnGivenVec(
                      vec, vec_original, start_second, num_repititions_per_vec);
  } catch (...) {
    std::cout << "TimeStdInplaceMergeWithOutBufferOnGivenVec() Failed."
              << std::endl;
  }
  try {
    total_times.merge_without_buffer += TimeMergeWithOutBufferOnGivenVec(vec,
                           vec_original, start_second, num_repititions_per_vec);
  } catch (...) {
    std::cout << "TimeMergeWithOutBufferOnGivenVec() Failed." << std::endl;
  }
  try {
    total_times.merge_without_buffer4 += TimeMergeWithOutBuffer4OnGivenVec(vec,
                           vec_original, start_second, num_repititions_per_vec);
  } catch (...) {
    std::cout << "TimeMergeWithOutBuffer4OnGivenVec() Failed." << std::endl;
  }
  try {
    total_times.merge_without_buffer3 += TimeMergeWithOutBuffer3OnGivenVec(vec,
                           vec_original, start_second, num_repititions_per_vec);
  } catch (...) {
    std::cout << "TimeMergeWithOutBuffer3OnGivenVec() Failed." << std::endl;
  }
  try {
    total_times.merge_without_buffer2 += TimeMergeWithOutBuffer2OnGivenVec(vec,
                           vec_original, start_second, num_repititions_per_vec);
  } catch (...) {
    std::cout << "TimeMergeWithOutBuffer2OnGivenVec() Failed." << std::endl;
  }
  try {
    total_times.merge_without_buffer1 += TimeMergeWithOutBuffer1OnGivenVec(vec,
                           vec_original, start_second, num_repititions_per_vec);
  } catch (...) {
    std::cout << "TimeMergeWithOutBuffer1OnGivenVec() Failed." << std::endl;
  }
  try {
    total_times.gnu_merge_without_buffer += TimeStdMergeWithOutBufferOnGivenVec(
                      vec, vec_original, start_second, num_repititions_per_vec);
  } catch (...) {
    std::cout << "TimeStdMergeWithOutBufferOnGivenVec() Failed." << std::endl;
  }
  return total_times;
}

/* Example call
  std::size_t vec_size, num_tests_per_vec_size, num_repititions_per_vec;
  auto start_right = -2;
  auto pick_new_start_right_for_each_new_vec = true;
  auto print_vec_averages = false;
  auto print_total_averages = true;
  vec_size = (1u << 6);
  num_tests_per_vec_size = (1u << 10);
  num_tests_per_vec_size = (1u << 8);
  TimeMergesOnGivenVecSize<int>(vec_size, num_tests_per_vec_size, num_repititions_per_vec,
                    start_right, pick_new_start_right_for_each_new_vec,
                    print_vec_averages, print_total_averages, false,
                    0, 100 * vec_size);
 */
template<class T> TotalTimes TimeMergesOnGivenVecSize(std::size_t vec_size,
                    std::size_t num_tests_per_vec_size = 1,
                    std::size_t num_repititions_per_vec = 1,
                    int start_right = -1, //set to -1 for the midpoint, and set
                                          //it to be < -1 to pick it randomly.
                    bool pick_new_start_right_for_each_new_vec = false,
                    bool print_vec_averages = false,
                    bool print_total_averages = true,
                    bool verbose = false,
                    T lower_bound = std::numeric_limits<T>::min(),
                    T upper_bound = std::numeric_limits<T>::max()) {
  if (start_right == -1) {
    start_right = vec_size / 2;
  } else if (start_right < -1) {
    std::random_device rnd_device;
    std::mt19937 generator(rnd_device());
    std::uniform_int_distribution<int> dist(1, vec_size - 1);
    start_right = dist(generator);
  }
  std::vector<T> vec_original(vec_size);
  auto vec = vec_original;

  TotalTimes total_times;
  for (std::size_t i = 0; i < num_tests_per_vec_size; i++) {
    FillWithRandomNumbers(vec_original.begin(), vec_original.end(), lower_bound,
                          upper_bound);
    auto end_left    = start_right - 1;
    int length_left  = start_right;
    int length_right = vec_size - start_right;
    //Sort the left and right subvectors
    std::sort(vec_original.begin(), vec_original.begin() + start_right);
    std::sort(vec_original.begin() + start_right, vec_original.end());
    vec = vec_original;
    if (verbose) {
      PrintLine("_");
      std::cout << "start_left = 0 \tend_left =\t" << end_left
                << "\tstart_right =\t" << start_right
                << "\tend_right =\t"   << (vec_size - 1)  << std::endl;
      PrintNondecreasingSubsequences(vec.begin(), length_left, false);
      PrintNondecreasingSubsequences(vec.begin() + start_right, length_right,
                                     true);
    }
    auto times = TimeMergesOnGivenVec(vec, vec_original, start_right,
                                      num_repititions_per_vec);
    total_times += times;

    if (print_vec_averages)
      std::cout << times.GetAveragesStr(num_repititions_per_vec);
    if (pick_new_start_right_for_each_new_vec) {
      std::random_device rnd_device;
      std::mt19937 generator(rnd_device());
      std::uniform_int_distribution<int> dist(1, vec_size - 1);
      start_right =  dist(generator);
    }
  }
  if (print_total_averages)
    std::cout << total_times.GetAveragesStr(num_repititions_per_vec);
  return total_times;
}

#endif /* SRC_MERGE_TIME_H_ */
