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
  //Calling TestCorrectnessOfMerge()
  int vec_size_start = 2;
  int vec_size_end = (1 << 6);
  unsigned int num_tests_per_vec_size = (1 << 3);
  bool should_randomly_pick_start_right = true;
  for (auto vec_size = vec_size_start; vec_size <= vec_size_end; vec_size++)
    TestCorrectnessOfMerge(vec_size, num_tests_per_vec_size, should_randomly_pick_start_right, false, 0, 10*vec_size);

  //Calling TimeMergesOnGivenVecSize()
  std::size_t vec_size, num_repititions_per_vec;
  auto start_right = -1;
  auto pick_new_start_right_for_each_new_vec = true;
  auto print_vec_averages = false;
  auto print_total_averages = true;
  vec_size = (1u << 24);
  num_tests_per_vec_size = (1u << 0);
  num_repititions_per_vec = (1u << 0);
  TimeMergesOnGivenVecSize<int>(vec_size, num_tests_per_vec_size, num_repititions_per_vec,
                    start_right, pick_new_start_right_for_each_new_vec,
                    print_vec_averages, print_total_averages, false,
                    0, 100 * vec_size);
  return 0;
}


