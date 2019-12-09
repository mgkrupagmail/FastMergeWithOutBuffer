/*
 * main.cpp
 *
 *  Created on: Jun 29, 2017
 *      Author: Matthew Gregory Krupa
 */
#include "merge_without_buffer.h"
#include "merge_test_correctness.h"
#include "merge_time.h"

int main() {
  std::ios::sync_with_stdio(false);
  //Calling TestCorrectnessOfMerge()
  int vec_size_start = 2;
  int vec_size_end = (1 << 10);
  unsigned int num_tests_per_vec_size = (1 << 3);
  bool should_randomly_pick_start_right = true;
  for (auto vec_size = vec_size_start; vec_size <= vec_size_end; vec_size++)
    TestCorrectnessOfMerge(vec_size, num_tests_per_vec_size, should_randomly_pick_start_right, false, 0, 10*vec_size);


  std::ios::sync_with_stdio(false);

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
