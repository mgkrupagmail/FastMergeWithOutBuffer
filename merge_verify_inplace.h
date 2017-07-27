/*
 * merge_verify_inplace.h
 *
 *  Created on: Jul 27, 2017
 *      Author: diagoras
 */

#ifndef SRC_MERGE_VERIFY_INPLACE_H_
#define SRC_MERGE_VERIFY_INPLACE_H_

#include <cassert>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "misc_helpers.h"
#include "merge_without_buffer.h"
#include "merge_without_buffer_trim4.h"
#include "merge_without_buffer_trim3.h"
#include "merge_without_buffer_trim2.h"
#include "merge_without_buffer_trim1.h"


template<class T>
struct ObjectAndIndex {
  T ob;
  int index;

  bool operator<(ObjectAndIndex<T> rhs)  { return ob < rhs.ob; }
  bool operator<=(ObjectAndIndex<T> rhs) { return ob <= rhs.ob; }
  bool operator>(ObjectAndIndex<T> rhs)  { return ob > rhs.ob; }
  bool operator>=(ObjectAndIndex<T> rhs) { return ob >= rhs.ob; }
  bool operator==(ObjectAndIndex<T> rhs) { return ob == rhs.ob; }
  bool operator!=(ObjectAndIndex<T> rhs) { return ob != rhs.ob; }
};

/* Helper function for VerifyMergeIsInplace<T>(int, std::vector<T>, int, int,
 *                                             int, int, bool, T, T).
 * Checks if vec is non-decreasing, in which case it returns true.
 * Otherwise, it prints an error message and returns false.
 */
template<class T>
bool VerifyMergeIsInplace(std::vector<ObjectAndIndex<T>> &vec) {
  bool result = true;
  for (auto it = vec.begin(); result && it != vec.end(); it++) {
    while (it + 1 != vec.end() && *it == *(it + 1)) {
      if ((*it).index >= (*(it + 1)).index) {
        result = false;
        break;
      }
      it++;
    }
  }
  if (!result) {
    std::cout << "The merging was not inplace.\n";
    PrintLine("-");
  }
  return result;
}

/* Assumes that start_left <= start_right.
 * If the test fails then vec_that_it_failed_on will be set equal to the
 *  original vector that it failed on.
 */
/* Helper function for
 * VerifyMergeIsInplace<T>(int, unsigned int, bool, bool, T, T)
 */
template<class T>
bool VerifyMergeIsInplace(int vec_size,
            std::vector<T> &vec_that_it_failed_on,
            int start_left, int end_left,
            int start_right, int end_right = -1, bool verbose = false,
            T lower_bound = std::numeric_limits<T>::min(),
            T upper_bound = std::numeric_limits<T>::max()) {
  if (vec_size <= 1) {
    std::cout << "vec_size = " << vec_size << " <= 1." << std::endl;
    vec_that_it_failed_on = std::vector<T>(vec_size);
    return false;
  }
  if (end_right == -1)
    end_right = vec_size - 1;

  std::vector<T> vec_original_obs(vec_size);
  FillWithRandomNumbers(vec_original_obs.begin(), vec_original_obs.end(),
                        lower_bound, upper_bound);
  //Sort the left and right subvectors.
  std::sort(vec_original_obs.begin() + start_left,
            vec_original_obs.begin() + (end_left + 1));
  std::sort(vec_original_obs.begin() + start_right,
            vec_original_obs.begin() + (end_right + 1));

  if (verbose) {
    PrintLine("_");
    PrintTwoSortedVectorsInfo(vec_original_obs, start_left, end_left,
                              start_right, end_right);
  }

  //Transfer vec_original_obs values' to the following vector, which pairs each
  // object vec_original_obs[i] with the index i.
  std::vector<ObjectAndIndex<T>> vec(vec_size);
  for (int i = 0; i < static_cast<int>(vec_original_obs.size()); i++) {
    vec[i].ob = vec_original_obs[i];
    vec[i].index = i;
  }

  MergeWithOutBuffer(vec.begin() + start_left,  vec.begin() + end_left,
                     vec.begin() + start_right, vec.begin() + end_right);

  auto result = VerifyMergeIsInplace(vec);
  if (!result) {
    vec_that_it_failed_on = std::move(vec_original_obs);
    return false;
  }
  return true;
}

/* Helper function for
 * VerifyMergeIsInplace<T>(int, unsigned int, bool, bool, T, T)
 */
template<class T>
bool VerifyMergeIsInplace(int vec_size,
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
    std::uniform_int_distribution<int> dist(1, vec_size - 1);
    start_right = dist(generator);
  }
  auto start_left  = 0;
  auto end_left    = start_right - 1;
  auto end_right   = vec_size - 1;;
  return VerifyMergeIsInplace<T>(vec_size,
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
    VerifyMergeIsInplace(vec_size, num_tests_per_vec_size,
                   should_randomly_pick_start_right, verbose, 0, 100*vec_size);
 */
template<typename T>
bool VerifyMergeIsInplace(int vec_size,
                            unsigned int num_tests_per_vec_size = 1,
                            bool should_randomly_pick_start_right = true,
                            bool verbose = false,
                            T lower_bound = std::numeric_limits<T>::min(),
                            T upper_bound = std::numeric_limits<T>::max()) {
  for (unsigned int i = 0; i < num_tests_per_vec_size; i++) {
    std::vector<T> any_vec_object;
    auto result = VerifyMergeIsInplace<T>(vec_size,
                    any_vec_object, should_randomly_pick_start_right, verbose,
                    lower_bound, upper_bound);
    if (!result) {
      std::cout << "Failed to merge the following vectors:" << std::endl;
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

#endif /* SRC_MERGE_VERIFY_INPLACE_H_ */
