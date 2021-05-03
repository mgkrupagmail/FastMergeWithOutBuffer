/*
 * mins_maxs_and_lambda.h
 *
 *  Created on: Jan 17, 2020
 *      Author: Matthew Gregory Krupa
 *   Copyright: Matthew Gregory Krupa
 */

#ifndef SRC_MINS_MAXS_AND_LAMBDA_H_
#define SRC_MINS_MAXS_AND_LAMBDA_H_

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

//The lambda returns 0 if count >= max_num_times_lambda_should_be_called or if
// cur_size >= to.vec_size_end, where count is the number of times
// that this lambda has been called.
//Otherwise, the lambda returns:  cur_size * to.vec_size_scale.
auto GetExponential_next_vec_size_lambda(const TestingOptions &to,
    const std::size_t max_num_times_lambda_should_be_called
                                    = std::numeric_limits<std::size_t>::max()) {
  //Given a value cur_size, this lambda will:
  //count should be the number of times that this lambda has been called.
  // If cur_size < vec_size_size, return vec_size_start, else
  // if cur_size >= vec_size_end, return 0 to indicate that there are
  //  no more values in the sequence of vector sizes, else
  // return a value > cur_size and <= vec_size_end
  //If vec_size_scale <= 1.0l then it will return cur_size + 1.
  return [&to, max_num_times_lambda_should_be_called]
                (std::size_t cur_size, std::size_t &count) -> std::size_t {
    if  (count >= max_num_times_lambda_should_be_called)
      return 0; //Indicate that we're done.
    ++count;
    if (cur_size < to.vec_size_start)
      return to.vec_size_start;
    if (cur_size >= to.vec_size_end)
      return 0; //Indicate that we're done.
    long double v_ld = static_cast<long double>(cur_size) * to.vec_size_scale;
    if (v_ld >= static_cast<long double>(to.vec_size_end))
      return to.vec_size_end;
    std::size_t v = v_ld;
    if (v <= cur_size)
      return cur_size + 1;
    return v;
  };
}

//The lambda returns 0 if count >= to.number_of_random_vec_sizes,
// where count is the number of times that this lambda has been called.
//Otherwise, the lambda returns a random integer in the range
// [to.vec_size_start, to.vec_size_end].
auto GetRandom_next_vec_size_lambda(const TestingOptions &to) {
  //If count >= max_num_times_lambda_should_be_called then this lambda will
  // return 0 to indicate that no more iterations should be performed.
  //Otherwise, it will return a random value in
  // [vec_size_lower_bound, vec_size_upper_bound]
return [&to](std::size_t cur_size, std::size_t &count) -> std::size_t {
    if  (count >= to.number_of_random_vec_sizes)
      return 0; //Indicate that we're done.
    ++count;
    std::random_device rnd_device;
    std::mt19937 generator(rnd_device());
    std::uniform_int_distribution<int> vec_size_dist(to.vec_size_start,
        to.vec_size_end);
    return vec_size_dist(generator);
  };
}

//The lambda returns a pair (a, b) of type std::pair<std::size_t, std::size_t>
//We try to find (a, b) so that:
// (1) if possible, b == to.desired_num_repetitions
// (2) to.minimum_elements_to_process <= cur_size * a * b
//                                          <= to.maximum_elements_to_process
//     with cur_size * a * b being as close to to.maximum_elements_to_process
//     as possible.
// (3) to.minimum_num_tests_per_vec_size <= a and
//                                        a <= to.maximum_num_tests_per_vec_size
// (4) to.minimum_num_repititions_per_vec <= b and
//                                       b <= to.maximum_num_repititions_per_vec
auto GetNum_tests_and_num_repititions_lambda(const TestingOptions &to) {
  return [&to](std::size_t cur_size)-> std::pair<std::size_t, std::size_t> {
    std::size_t max_max_product = cur_size
                                  * to.maximum_num_tests_per_vec_size
                                  * to.maximum_num_repititions_per_vec;
    if (max_max_product <= to.maximum_elements_to_process
     || max_max_product <= to.minimum_elements_to_process) {
     std::size_t num_tests = to.maximum_num_tests_per_vec_size;
     std::size_t num_repetitions = to.maximum_num_repititions_per_vec;
     return std::make_pair(num_tests, num_repetitions);
    }

    std::size_t min_min_product = cur_size
                                  * to.minimum_num_tests_per_vec_size
                                  * to.minimum_num_repititions_per_vec;
    if (min_min_product >= to.maximum_elements_to_process) {
     std::size_t num_tests = to.minimum_num_tests_per_vec_size;
     std::size_t num_repetitions = to.minimum_num_repititions_per_vec;
     return std::make_pair(num_tests, num_repetitions);
    }

    std::size_t min_desired_product = cur_size
                                      * to.minimum_num_tests_per_vec_size
                                      * to.desired_num_repetitions;
    if (min_desired_product <= to.maximum_elements_to_process) {
     std::size_t num_repetitions = to.desired_num_repetitions;
     std::size_t num_tests = to.maximum_elements_to_process
                                             / (cur_size * num_repetitions);
     if (num_tests > to.maximum_num_tests_per_vec_size) {
       num_tests = to.maximum_num_tests_per_vec_size;
       num_repetitions = to.maximum_elements_to_process / (cur_size * num_tests);
       if (num_repetitions > to.maximum_num_repititions_per_vec)
         num_repetitions = to.maximum_num_repititions_per_vec;
     }
     return std::make_pair(num_tests, num_repetitions);
    }

    std::size_t max_min_product = cur_size
                                  * to.maximum_num_tests_per_vec_size
                                  * to.minimum_num_repititions_per_vec;
    if (max_min_product >= to.maximum_elements_to_process) {
     std::size_t num_repetitions = to.minimum_num_repititions_per_vec;
     std::size_t num_tests = to.maximum_elements_to_process
                                             / (cur_size * num_repetitions);
assert(num_tests <= to.maximum_num_tests_per_vec_size);
     if (num_tests < to.minimum_num_tests_per_vec_size)
       num_tests = to.minimum_num_tests_per_vec_size;
     return std::make_pair(num_tests, num_repetitions);
    }

    std::size_t num_tests = to.maximum_num_tests_per_vec_size;
    std::size_t num_repetitions = to.maximum_elements_to_process
                                                  / (cur_size * num_tests);
    if (num_repetitions < to.minimum_num_repititions_per_vec)
      num_repetitions = to.minimum_num_repititions_per_vec;
    else if (num_repetitions > to.maximum_num_repititions_per_vec)
      num_repetitions = to.maximum_num_repititions_per_vec;
assert(cur_size * num_tests * num_repetitions <= to.maximum_elements_to_process);
    return std::make_pair(num_tests, num_repetitions);
  };
}

//This lambda returns the length of the left range based on the options that
// are enabled/disabled in the TestingOptions object "to".
auto GetLength_of_left_subvector_lambda(const TestingOptions &to) {
assert(!(to.should_return_initial_length_left_for_all_subsequence_calls
         && to.should_pick_new_random_length_for_each_new_vec));
  //Given vec_size and previous_length_left, this lambda will return an
  // std::pair<std::size_t, bool> p where p.first is the length_left to use
  // for the left subector (a value between 1 and cur_size - 1) and
  // p.second is true if and only if the value was chosen randomly.
  return [&to](std::size_t cur_size, std::size_t previous_length_left)
                                              -> std::pair<std::size_t, bool> {
    auto fraction_of_vec_size = to.length_left_fraction_of_vec_size;
    //Note that previous_length_left == 0 if and only if this lambda
    // has not been called before for this current vec_size.
    if (cur_size <= 2) {
      return std::make_pair(1, false);
    } else if ((previous_length_left > 0 &&
                to.should_pick_new_random_length_for_each_new_vec)
   || (previous_length_left <= 0 && to.should_randomly_pick_start_right)) {
      std::random_device rnd_device;
      std::mt19937 generator(rnd_device());
      std::uniform_int_distribution<int> vec_size_dist(1, cur_size - 1);
      return std::make_pair(vec_size_dist(generator), true);
    } else if (previous_length_left > 0 &&
              to.should_return_initial_length_left_for_all_subsequence_calls) {
      return std::make_pair(previous_length_left, false);
    } else if (fraction_of_vec_size <= 0.0l) {
      return std::make_pair(1, false);
    } else if (fraction_of_vec_size >= 1.0l) {
      return std::make_pair(cur_size - 1, false);
    } else if (fraction_of_vec_size == 0.5l) {
      return std::make_pair(cur_size / 2, false);
    } else {
      auto next_length_left = static_cast<std::size_t>(fraction_of_vec_size
                                                       * cur_size);
      if (next_length_left <= 0)
        return std::make_pair(1, false);
      else if (next_length_left >= cur_size)
        return std::make_pair(cur_size - 1, false);
      else
        return std::make_pair(next_length_left, false);
    }
  };
}


#endif /* SRC_MINS_MAXS_AND_LAMBDA_H_ */
