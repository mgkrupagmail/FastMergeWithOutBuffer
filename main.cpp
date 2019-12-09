/*
 * main.cpp
 *
 *  Created on: Jun 29, 2017
 *      Author: Matthew Gregory Krupa
 */
/*============================================================================
 *  Name        : main.cpp
 *  Author      : Matthew Gregory Krupa
 *  Version     : 1.0
 *  Copyright   : Any thing that's neither public domain nor already
 *                 owned by someone (as of March 24, 2017), I hereby copywrite
 *                 to the maximum extent allowed by law. :)
 *  Description : The primary algorithm of interest here would be QuickMidSelectSubdivideSort()
 *                (1) A selection algorithm QuickMidSelect() that for large vectors
 *                    of primitive types, is faster than the g++ libstdc++'s
 *                     std::nth_element() about half the time.
 *                (2) A sorting algorithm QuickMidSelectSubdivideSort() based on QuickMidSelect() that
 *                    for large vectors of primitive types, is usually around 10-20%
 *                    faster than g++ libstdc++'s  std::sort() (i.e. introsort).
 *                (3) An algorithm similar to QuickMidSelect(), called QuickMidMedian(),
 *                    that for large vectors of primitive types, finds the
 *                    median faster than the 1 or 2 calls to std::nth_element()
 *                    that would otherwise be required to compute it.
 *  I implemented QuickMidMedian() before I implemented QuickMidSelect() so QuickMidMedian()
 *   has most of the comments explaining how these algorithms work, why they're
 *   generally so fast, why the above description emphasized primitive types,
 *   and when these algorithms can be applied to non-primitive types.
 *  To test these algorithms, scroll down to main() and un-comment out which ever
 *   of the tests (i.e. SortingComparison(), SelectComparison (), MedianComparison())
 *   you want to run.
 *  In general, the larger the size of the vector, the bigger the difference in
 *   speed between my algorithms and the corresponding std:: algorithms. The same
 *   is true for vectors that take on a larger range of values. However, sometimes
 *   an algorithm (referring to all algorithms here, not just my own) gets
 *   lucky by say, selecting the right pivot or having a distribution of data
 *   that is somehow amiable for that algorithm. So there can be some variability
 *   if differences in speed.
 * ============================================================================
 */

#include <cassert>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <vector>


#include "random_helpers.h"

#include "merge_common.h"
#include "merge_without_buffer.h"
#include "merge_test_correctness.h"
#include "merge_time.h"
#include "merge_verify_stability.h"



int main() {
  std::ios::sync_with_stdio(false);
  //Calling TestCorrectnessOfMerge()
  int vec_size_start = (1u << 5);
  int vec_size_end = (1 << 10);
  unsigned int num_tests_per_vec_size = (1 << 3);
  bool should_randomly_pick_start_right = true;
  for (auto vec_size = vec_size_start; vec_size <= vec_size_end; vec_size++)
    TestCorrectnessOfMerge(vec_size, num_tests_per_vec_size, should_randomly_pick_start_right, false, 0, 10*vec_size);

  typedef double ValueType;
  std::size_t vec_size_lower_bound = vec_size_start;
  std::size_t vec_size_upper_bound = (1u << 22);
  std::size_t number_of_random_vec_sizes = (1u << 12);

  int start_right = -2; //For the first vector, pick the lengths of the left and right vectors randomly.
  bool pick_new_random_start_right_for_each_new_vec = true;
  auto next_vec_size_lambda           = default_next_vec_size_lambda;
  auto num_tests_per_vec_size_lambda  = default_num_tests_per_vec_size_lambda;
  auto num_repititions_per_vec_lambda = default_num_repititions_per_vec_lambda;

  bool print_vec_averages = false;
  bool print_total_averages = true;
  bool verbose = false;
  bool print_vec_original = false;
  //Pick the random vector values in the range [0, const_to_scale_vec_size_by * vec_size].
  //This is useful for controlling how many repeated values the randomly generated vectors have.
  long double const_to_scale_vec_size_by = 1.2l; // If <= 0 then the random vector values independently of the vector's size.
  ValueType value_lower_bound = 0;

  TotalTimes grand_total_times;
  auto total_times_exp = TimeMergeFunctions<ValueType>(vec_size_start, start_right, pick_new_random_start_right_for_each_new_vec,
                            next_vec_size_lambda, num_tests_per_vec_size_lambda, num_repititions_per_vec_lambda,
                            print_vec_averages, print_total_averages, verbose, print_vec_original,
                            const_to_scale_vec_size_by, value_lower_bound);

  auto total_times_random = TimeMergeFunctionsOnRandomVecSizes<ValueType>(vec_size_lower_bound, vec_size_upper_bound,
                              number_of_random_vec_sizes,
                              start_right, pick_new_random_start_right_for_each_new_vec,
                              num_tests_per_vec_size_lambda, num_repititions_per_vec_lambda,
                              print_vec_averages, print_total_averages, verbose, print_vec_original,
                              const_to_scale_vec_size_by, value_lower_bound);
  grand_total_times += total_times_exp;
  grand_total_times += total_times_random;
  PrintLine("*");
  PrintLine("*");
  std::cout << "grand_total_times.total_number_of_times_each_merge_function_was_called = ";
  std::cout << grand_total_times.total_number_of_times_each_merge_function_was_called << '\n';
  std::cout << grand_total_times.GetAveragesStr(grand_total_times.total_number_of_times_each_merge_function_was_called) << '\n';
  std::cout << TotalTimes::GetStringOfStaticVariables() << '\n';
  std::cout.flush();
  return 0;
}
/**/
