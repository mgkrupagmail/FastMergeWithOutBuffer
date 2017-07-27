/*
 * misc_helpers.h
 *
 *  Created on: Jul 1, 2017
 *      Author: diagoras
 */

#ifndef SRC_MISC_HELPERS_H_
#define SRC_MISC_HELPERS_H_

#include <iostream>
#include <iomanip>
#include <random>
#include <string>

//This general case is designed to work with any type T for which
// std::is_integral<A>::value == true.
template<class Iterator, typename T>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, T a, T b) {
  std::random_device rnd_device;
  std::mt19937 generator(rnd_device());
  std::uniform_int_distribution<T> dist(a, b);
  for (auto it = start; it != one_past_end; it++)
    *it = dist(generator);
  return ;
}

template<class Iterator>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, float a, float b) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<float> dist(a, b);
  for (auto it = start; it != one_past_end; it++)
    *it = dist(generator);
  return ;
}

template<class Iterator>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, double a, double b) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<double> dist(a, b);
  for (auto it = start; it != one_past_end; it++)
    *it = dist(generator);
  return ;
}

template<class Iterator>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, long double a, long double b) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<long double>  dist(a, b);
  for (auto it = start; it != one_past_end; it++)
    *it = dist(generator);
  return ;
}


template<class ForwardIterator>
bool IsNonDecreasing(ForwardIterator start_it, ForwardIterator one_past_last) {
  auto one_ahead = start_it;
  if(start_it == one_past_last || ++one_ahead == one_past_last)
    return true; //Vacuously non-decreasing.
  while (one_ahead != one_past_last) {
    if (*start_it > *one_ahead)
      return false;
    start_it++;
    one_ahead++;
  }
  return true;
}

inline void PrintLine(const std::string str, const int num_repetitions = 80) {
  for (auto i = 0; i < num_repetitions; i++)
    std::cout << str;
  std::cout << std::endl;
  return ;
}

template<class RAI>
std::string GetNondecreasingSubsequences(RAI start, std::size_t length) {
  std::size_t count_width = 1;
  std::size_t num = 0;
  auto it = start;
  while (num + 1 < length) {
    std::size_t count = 1;
    if (*it > *(it + 1)) {
      it++;
      num++;
      continue;
    }
    //Count how many elements are in the current monotone subsequence.
    while (num + 1 < length && *it <= *(it + 1)) {
      it++;
      num++;
      count++;
    }
    auto cur_count_length = std::to_string(count).length();
    if (count_width < cur_count_length)
      count_width = cur_count_length;
  }

  num = 0;
  it = start;
  std::stringstream strm;
  while (num < length) {
    auto cur_it       = it;
    auto cur_num      = num;
    std::size_t count = 1;
    //Count how many elements are in the current monotone subsequence.
    while (cur_num + 1 < length && *cur_it <= *(cur_it + 1)) {
      cur_it++;
      cur_num++;
      count++;
    }
    strm << std::left << std::setw(count_width) << count << "|";
    strm << ' ' << *it;
    num++;
    while (num < length && *it <= *(it + 1)) {
      it++;
      strm << ' ' << *it;
      num++;
    }
    it++;
    strm << '\n';
  }
  return strm.str();
}

template<class RAI>
void PrintNondecreasingSubsequences(RAI start, std::size_t length,
                                    bool print_new_line_at_end = true,
                                    std::ostream &ostrm = std::cout) {
  ostrm << GetNondecreasingSubsequences(start, length);
  if (print_new_line_at_end)
    ostrm << '\n';
  ostrm.flush();
}

/* Helper function for VerifyMergeIsInplace() andVerifyMergeIsInplace().
 */
template<class T>
void PrintTwoSortedVectorsInfo(const std::vector<T> &vec, int start_left,
                               int end_left, int start_right, int end_right) {
    std::cout << "start_left = "   << start_left          << "\tend_left =\t"
              << end_left          << "\tstart_right =\t" << start_right
              << "\tend_right =\t" << end_right           << std::endl;
    auto length_left  = end_left  + 1 - start_left;
    auto length_right = end_right + 1 - start_right;
    PrintNondecreasingSubsequences(vec.begin() + start_left,  length_left,
                                   false);
    PrintNondecreasingSubsequences(vec.begin() + start_right, length_right,
                                   true);
    return ;
}

#endif /* SRC_MISC_HELPERS_H_ */
