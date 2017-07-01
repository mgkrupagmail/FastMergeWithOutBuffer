/*
 * merge_test_correctness.h
 *
 *  Created on: Jun 28, 2017
 *      Author: Matthew Gregory Krupa
 *
 *  This header file defines the TestCorrectnessOfMerge() function, which
 *   generates random vectors and tests that some given merge function
 *   correctly merges two ordered sub-vectors.
 *  It tests MergeWithOutBuffer() by default.
 *  To test some other merge function replace MergeWithOutBuffer with your
 *   desired function in the body of MergeTwoSortedSubvectorsTestCorrectness().
 */

#ifndef SRC_MERGE_TEST_CORRECTNESS_H_
#define SRC_MERGE_TEST_CORRECTNESS_H_

#include <cassert>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "misc_helpers.h"
#include "merge_without_buffer.h"
#include "merge_without_buffer_trim2.h"
#include "merge_without_buffer_trim1.h"

/* Assumes that start_left <= start_right.
 * If the test fails then vec_that_it_failed_on will be set equal to the
 *  original vector that it failed on.
 */
/* Example call:
  auto vec_size    = 20;
  auto start_left  = 4;
  auto end_left    = 8;
  auto start_right = 12;
  auto end_right   = 16;
  std::vector<int> any_vec_object;
  auto result = MergeTwoSortedSubvectorsTestCorrectness(vec_size,
                  any_vec_object, start_left, end_left, start_right, end_right,
                  false, 0, 16 * vec_size);
  if (!result) {
    std::cout << __LINE__ << ", Failed to merge the following vectors:"
              << std::endl;
    auto iter = any_vec_object.begin();
    PrintNondecreasingSubsequences(iter + start_left,
                                   end_left  + 1 - start_left);
    PrintNondecreasingSubsequences(iter + start_right,
                                   end_right + 1 - start_right);
    return -1;
  }
 */
template<class T>
bool MergeTwoSortedSubvectorsTestCorrectness(int vec_size,
            std::vector<T> &vec_that_it_failed_on, int start_left, int end_left,
            int start_right, int end_right = -1, bool verbose = false,
            T lower_bound = std::numeric_limits<T>::min(),
            T upper_bound = std::numeric_limits<T>::max()) {
  if (vec_size <= 1) {
    std::cout << __LINE__ << ", vec_size = " << vec_size << " <= 1."
        << std::endl;
    vec_that_it_failed_on = std::vector<T>(vec_size);
    return false;
  }
  if (end_right == -1)
    end_right = vec_size - 1;
assert(start_left >= 0);
assert(start_right >= 0);
assert(start_left  <= end_left);
assert(start_right <= vec_size);
assert(start_right <= end_right);
assert(end_right   < vec_size);
assert(end_left    < vec_size);

//Check that [start_right, end_right] and [start_left,  end_left] are disjoint.
//Check that start_left is not in [start_right, end_right].
assert(!(start_left  <= end_right && start_left  >= start_right));
//Check that end_left   is not in [start_right, end_right].
assert(!(end_left    <= end_right && end_left    >= start_right));
//Check that start_right is not in [start_left,  end_left].
assert(!(start_right <= end_left  && start_right >= start_left));
//Check that end_right   is not in [start_left,  end_left].
assert(!(end_right   <= end_left  && end_right   >= start_left));

  std::vector<T> vec_original(vec_size);

  FillWithRandomNumbers(vec_original.begin(), vec_original.end(),
                        lower_bound, upper_bound);

  int length_left     = end_left  + 1 - start_left;
  int length_right    = end_right + 1 - start_right;

  //Sort the left and right subvectors.
  std::sort(vec_original.begin() + start_left,
            vec_original.begin() + (end_left + 1));
  std::sort(vec_original.begin() + start_right,
            vec_original.begin() + (end_right + 1));
  auto vec = vec_original;

  if (verbose) {
    PrintLine("_");
    std::cout << "start_left = "   << start_left          << "\tend_left =\t"
              << end_left          << "\tstart_right =\t" << start_right
              << "\tend_right =\t" << end_right           << std::endl;
    PrintNondecreasingSubsequences(vec.begin() + start_left,  length_left,
                                   false);
    PrintNondecreasingSubsequences(vec.begin() + start_right, length_right,
                                   true);
  }

  const int original_start_left   = start_left;
  const int original_start_right  = start_right;
  const int original_end_left     = end_left;
  const int original_end_right    = end_right;
  const int original_length_left  = length_left;
  const int original_length_right = length_right;

  if (verbose) {
    PrintLine("_");
    std::cout << "start_left = "   << start_left          << "\tend_left =\t"
              << end_left          << "\tstart_right =\t" << start_right
              << "\tend_right =\t" << end_right           << std::endl;
    PrintNondecreasingSubsequences(vec.begin() + start_left,  length_left,
                                   false);
    PrintNondecreasingSubsequences(vec.begin() + start_right, length_right,
                                   true);
    PrintLine("-");

  }

  MergeWithOutBuffer(vec.begin() + start_left, vec.begin() + end_left,
                     vec.begin() + start_right, vec.begin() + end_right);

  bool is_left_non_decreasing  = IsNonDecreasing(vec.begin()
                       + original_start_left,  vec.begin() + original_end_left);
  bool is_right_non_decreasing = IsNonDecreasing(vec.begin()
                      + original_start_right, vec.begin() + original_end_right);
  if (!(is_left_non_decreasing && is_right_non_decreasing && vec[end_left]
                                                         <= vec[start_right])) {
    PrintLine("-");
    std::cout << "Something went wrong when merging these vectors:" << '\n';
    PrintNondecreasingSubsequences(vec_original.begin() + original_start_left,
                                   original_length_left, false);
    PrintNondecreasingSubsequences(vec_original.begin() + original_start_right,
                                   original_length_right, true);
    std::cout << "These were the resulting vectors:" << std::endl;
    PrintNondecreasingSubsequences(vec.begin() + original_start_left,
                                   original_length_left, false);
    PrintNondecreasingSubsequences(vec.begin() + original_start_right,
                                   original_length_right, true);
    std::cout << "start_left = "    << start_left         << "\tend_left =\t"
              << end_left          << "\tstart_right =\t" << start_right
              << "\tend_right =\t" << end_right           << std::endl;
    PrintNondecreasingSubsequences(vec.begin() + start_left,  length_left,
                                   false);
    PrintNondecreasingSubsequences(vec.begin() + start_right, length_right,
                                   true);
    PrintLine("-");
    vec_that_it_failed_on = std::move(vec_original);
    return false;
  }
  if (verbose) {
    PrintLine("_");
    std::cout << "start_left = "   << start_left          << "\tend_left =\t"
              << end_left          << "\tstart_right =\t" << start_right
              << "\tend_right =\t" << end_right           << std::endl;
    PrintNondecreasingSubsequences(vec.begin() + start_left,  length_left,
                                   false);
    PrintNondecreasingSubsequences(vec.begin() + start_right, length_right,
                                   true);

  }
  return true;
}

/* Example call
  auto vec_size = 20;
  std::vector<int> any_vec_object;
  auto result = MergeTwoSortedSubvectorsTestCorrectness(vec_size,
                                          any_vec_object, true, false, 0, 100);
  if (!result) {
    std::cout << __LINE__ << ", Failed to merge the following vectors:"
              << std::endl;
    auto start_right = 1;
    while (start_right < vec_size && any_vec_object[start_right - 1] <=
                                                   any_vec_object[start_right])
      start_right++;
    auto iter = any_vec_object.begin();
    PrintNondecreasingSubsequences(iter, start_right);
    PrintNondecreasingSubsequences(iter + start_right, vec_size - start_right);
    return -1;
  }
 */
template<class T>
bool MergeTwoSortedSubvectorsTestCorrectness(int vec_size,
                   std::vector<T> &vec_that_it_failed_on,
                   bool should_randomly_pick_start_right, bool verbose = false,
                   T lower_bound = std::numeric_limits<T>::min(),
                   T upper_bound = std::numeric_limits<T>::max()) {
  assert(vec_size >= 0);
  int start_right;
  if (!should_randomly_pick_start_right) {
    start_right = vec_size / 2;
  } else {
    std::random_device rnd_device;
    std::mt19937 generator(rnd_device());
    std::uniform_int_distribution<T> dist(1, vec_size - 1);
    start_right =  dist(generator);
  }
  auto start_left  = 0;
  auto end_left    = start_right - 1;
  auto end_right   = vec_size - 1;;
  return MergeTwoSortedSubvectorsTestCorrectness(vec_size,
            vec_that_it_failed_on, start_left, end_left, start_right, end_right,
            verbose,lower_bound, upper_bound);
}

/* Returns true if and only if MergeTwoSortedSubvectorsTest() succeeded.
 */
/* Example call:
  int vec_size_start = 2;
  int vec_size_end = (1 << 10);
  unsigned int num_tests_per_vec_size = (1 << 3);
  bool should_randomly_pick_start_right = true;
  bool verbose = false;
  for (auto vec_size = vec_size_start; vec_size <= vec_size_end; vec_size++)
    TestCorrectnessOfMerge(vec_size, num_tests_per_vec_size,
                   should_randomly_pick_start_right, verbose, 0, 100*vec_size);
 */
template<typename T>
bool TestCorrectnessOfMerge(int vec_size,
                    unsigned int num_tests_per_vec_size = 1,
                    bool should_randomly_pick_start_right = true,
                    bool verbose = false,
                    T lower_bound = std::numeric_limits<T>::min(),
                    T upper_bound = std::numeric_limits<T>::max()) {
  for (unsigned int i = 0; i < num_tests_per_vec_size; i++) {
    std::vector<int> any_vec_object;
    auto result = MergeTwoSortedSubvectorsTestCorrectness(vec_size,
                    any_vec_object, should_randomly_pick_start_right, verbose,
                    lower_bound, upper_bound);

    if (!result) {
      std::cout << __LINE__ << ", Failed to merge the following vectors:"
                << std::endl;
      auto start_right = 1;
      //Find start_right.
      while (start_right < vec_size && any_vec_object[start_right - 1] <=
                                                    any_vec_object[start_right])
        start_right++;
      auto iter = any_vec_object.begin();
      PrintNondecreasingSubsequences(iter, start_right);
      PrintNondecreasingSubsequences(iter + start_right, vec_size -start_right);
      return false;
    }
  }

  return true;
}

#endif /* SRC_MERGE_TEST_CORRECTNESS_H_ */
