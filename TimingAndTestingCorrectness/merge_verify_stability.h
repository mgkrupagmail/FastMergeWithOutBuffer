/*
 * merge_verify_stability.h
 *
 *  Created on: Jul 27, 2017
 *      Author: Matthew Gregory Krupa
 */

#ifndef SRC_MERGE_VERIFY_STABILITY_H_
#define SRC_MERGE_VERIFY_STABILITY_H_

#include <cassert>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <functional>
#include <string>
#include <vector>

#include "misc_helpers.h"

/*
template<typename T, typename ValueType>
struct ObjectAndIndex {
  ValueType ob;
  int index; // = -1

  inline bool operator==(const ObjectAndIndex<T, ValueType> &rhs) const {
    return ob == rhs.ob;
  }
  inline bool operator!=(const ObjectAndIndex<T, ValueType> &rhs) const {
    return !(this->operator==(rhs));
  }
  inline bool operator<(const ObjectAndIndex<T, ValueType> &rhs)  const {
    return ob < rhs.ob;
  }
  inline bool operator<=(const ObjectAndIndex<T, ValueType> &rhs) const {
    return !(rhs.operator<(*this));
  }
  inline bool operator>(const ObjectAndIndex<T, ValueType> &rhs)  const {
    return rhs.operator<(*this);
  }
  inline bool operator>=(const ObjectAndIndex<T, ValueType> &rhs) const {
    return rhs.operator<=(*this);
  }
};*/

template<typename ValueType, typename Compare>
struct ObjectAndIndex {
  static std::function<bool(const ValueType&, const ValueType&)> comp_;
  typedef ObjectAndIndex<ValueType, Compare> ThisObjectType;

  ValueType ob;
  int index; // = -1

  inline bool operator==(const ThisObjectType &rhs) const {
    return !comp_(*this, rhs) && !comp_(rhs, *this);//ob == rhs.ob;
  }
  inline bool operator!=(const ThisObjectType &rhs) const {
    return comp_(*this, rhs) || comp_(rhs, *this); //!(this->operator==(rhs));
  }
  inline bool operator<(const ThisObjectType &rhs)  const {
    return comp_(ob, rhs.ob);
  }
  inline bool operator<=(const ThisObjectType &rhs) const {
    return !comp_(rhs.ob, ob);//!(rhs.operator<(*this));
  }
  inline bool operator>(const ThisObjectType &rhs)  const {
    return comp_(rhs.ob, ob); //rhs.operator<(*this);
  }
  inline bool operator>=(const ThisObjectType &rhs) const {
    return !comp_(ob, rhs.ob); //rhs.operator<=(*this);
  }
};

template<typename ValueType, typename Compare>
std::function<bool(const ValueType&, const ValueType&)>
                                   ObjectAndIndex<ValueType, Compare>::comp_;


/* Helper function for VerifyMergeStability<T>(int, std::vector<T>, int, int,
 *                                             int, int, bool, T, T).
 * Checks if vec is non-decreasing, in which case it returns true.
 * Otherwise, it prints an error message and returns false.
 */
template<typename ValueType, typename Compare,
         typename Iterator>
bool VerifyThatTheMergeWasStable(Iterator start_it, Iterator one_past_end_it,
                                 TestingOptions &to, Compare comp) {
  auto equality = [comp](const ValueType &lhs, const ValueType &rhs) {
    return !comp(lhs, rhs) && !comp(rhs, lhs);
  };
  bool result = true;
  for (auto it = start_it; it != one_past_end_it; it++) {
    auto next_it = it;
    ++next_it;
    if (next_it == one_past_end_it)
      break ;
    assert((*next_it).index != (*it).index);
    if (equality((*next_it).ob, (*it).ob)) {
      if ((*next_it).index <= (*it).index) {
        result = false;
        break ;
      }
    }
  }
  if (!result) {
    to.PrintString("The merge was NOT stable.\n");
    to.Flush();
  }
  return result;
}

template<typename ValueType,
         template<class, class> class ContainerType, typename Compare,
         template<class, class, class> class MergeFunction,
         typename SizeType>
bool VerifyMergeStabilityOnGivenVec(SizeType vec_size,
            const std::vector<ValueType> &vec_original_obs,
            SizeType length_left, SizeType length_right,
            TestingOptions &to,
            Compare comp,
            SizeType start_left) {
  typedef ObjectAndIndex<ValueType, Compare> ObAndInType;
  typedef ContainerType<ObAndInType, std::allocator<ObAndInType>> Container;
  typedef typename Container::iterator Iterator;
  typedef typename Iterator::difference_type Distance;
  auto object_and_index_comp = [comp](const ObAndInType &lhs,
                                      const ObAndInType &rhs) -> bool {
    return comp(lhs.ob, rhs.ob);
  };
  //Transfer vec_original_obs values' to the following vector, which pairs each
  // object vec_original_obs[i] with the index i.
  Container v(vec_size);
  for (auto i = 0, it = v.begin();
                    i < static_cast<int>(vec_original_obs.size()); ++i, ++it) {
    it->ob = vec_original_obs[i];
    it->index = i;
  }

  auto start_left_it = v.begin();
  std::advance(start_left_it, start_left);
  auto start_right_it = start_left_it;
  std::advance(start_right_it, length_left);
  auto one_past_end_right_it = start_right_it;
  std::advance(one_past_end_right_it, length_right);
  typedef MergeFunction<Iterator, decltype(object_and_index_comp), Distance>
                                                            MergeFunctionClass;

  MergeFunctionClass()(start_left_it, start_right_it, one_past_end_right_it,
                       length_left, length_right, object_and_index_comp);

  assert(std::is_sorted(start_left_it, one_past_end_right_it,
                        object_and_index_comp));
  return VerifyThatTheMergeWasStable<ValueType, Compare>(
                                    start_left_it, one_past_end_right_it, to,
                                    comp);
}

/* Assumes that start_left <= start_right.
 * If the test fails then vec_that_it_failed_on will be set equal to the
 *  original vector that it failed on.
 */
/* Helper function for
 * VerifyMergeStability<T>(int, unsigned int, bool, bool, T, T)
 */
template<typename T, typename ValueType,
         template<class, class> class ContainerType, typename Compare,
         template<class, class, class> class MergeFunction,
         typename SizeType>
bool VerifyMergeStability(int vec_size,
            TestingOptions &to,
            Compare comp,
            std::vector<ValueType> &vec_that_it_failed_on,
            SizeType start_left,
            SizeType start_right, SizeType one_past_end_right,
            T lower_bound, T upper_bound) {
  typedef ObjectAndIndex<ValueType, Compare> ObAndInType;
  typedef ContainerType<ObAndInType, std::allocator<ObAndInType>> Container;
  typedef typename Container::iterator Iterator;
  typedef typename Iterator::difference_type Distance;
  Distance length_left  = start_right - start_left;
  Distance length_right = one_past_end_right - start_right;
  if (vec_size <= 1) {
    std::cout << "vec_size = " << vec_size << " <= 1." << std::endl;
    vec_that_it_failed_on = std::vector<ValueType>(vec_size);
    return false;
  }
  if (one_past_end_right == static_cast<SizeType>(-1))
    one_past_end_right = vec_size;

  std::vector<ValueType> vec_original_obs(vec_size);
  FillWithRandomNumbers<ValueType>(vec_original_obs.begin(),
                                   vec_original_obs.end(),
                                   lower_bound, upper_bound);
  //Sort the left and right sub-vectors.
  std::sort(vec_original_obs.begin() + start_left,
            vec_original_obs.begin() + start_right, comp);
  std::sort(vec_original_obs.begin() + start_right,
            vec_original_obs.begin() + one_past_end_right, comp);
  bool result = VerifyMergeStabilityOnGivenVec<ValueType, ContainerType,
                                    Compare, MergeFunction, SizeType>(vec_size,
                                   vec_original_obs, length_left, length_right,
                                   to, comp, start_left);
  if (!result) {
    vec_that_it_failed_on = std::move(vec_original_obs);
    return false;
  }
  return true;
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
    VerifyMergeStability(vec_size, num_tests_per_vec_size,
                   should_randomly_pick_start_right, verbose, 0, 100*vec_size);
 */
template<typename T, typename ValueType,
         template<class, class> class ContainerType, typename Compare,
         template<class, class, class> class MergeFunction,
         typename SizeType>
bool VerifyMergeStability(SizeType vec_size,
                          TestingOptions &to,
                          Compare comp,
                          T lower_bound = std::numeric_limits<T>::min(),
                          T upper_bound = std::numeric_limits<T>::max()) {
  ObjectAndIndex<ValueType, Compare>::comp_ = comp;
  assert(vec_size >= 0);
  auto comp_le = [comp](const auto &lhs, const auto &rhs) -> bool {
    return !comp(rhs, lhs);
  };
  std::random_device rnd_device;
  std::mt19937 generator(rnd_device());
  std::uniform_int_distribution<int> dist(1, vec_size - 1);
  for (SizeType i = 0; i < to.verify_merge_stability_num_tests_per_vec_size; i++) {
    std::vector<ValueType> fail_result;
    SizeType start_right;
    if (!to.should_randomly_pick_start_right) {
      start_right = vec_size / 2;
    } else {
      start_right = dist(generator);
    }
    SizeType start_left  = 0;
    SizeType one_past_end_right = vec_size;
    bool result = VerifyMergeStability<T, ValueType, ContainerType, Compare,
                                       MergeFunction, SizeType>(
        vec_size, to, comp, fail_result, start_left,
        start_right, one_past_end_right, lower_bound, upper_bound);
    if (!result) {
      to.PrintString("Failed to stabily merge the following ranges:\n");
      SizeType right = 1;
      //Find start_right.
      while (right < vec_size && comp_le(fail_result[right - 1],
                                                          fail_result[right]))
        right++;
      auto iter = fail_result.begin();
      PrintNondecreasingSubsequences(iter, right, comp, true, to.ostrstrm);
      auto iter_plus_right = iter;
      std::advance(iter_plus_right, right);
      PrintNondecreasingSubsequences(iter_plus_right, vec_size - right, comp,
          true, to.ostrstrm);
      to.Flush();
      return false;
    }
  }
  return true;
}

#endif /* SRC_MERGE_VERIFY_STABILITY_H_ */
