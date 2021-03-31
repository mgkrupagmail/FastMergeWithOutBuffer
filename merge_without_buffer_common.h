/*
 * merge_without_buffer_common.h
 *
 *  Created on: Jan 2, 2020
 *      Author: Matthew Gregory Krupa
 *   Copyright: Metthew Gregory Krupa
 */

#ifndef SRC_MERGE_WITHOUT_BUFFER_COMMON_H_
#define SRC_MERGE_WITHOUT_BUFFER_COMMON_H_


#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <type_traits>

#ifndef IDENTITY_MACRO
//#define IDENTITY_MACRO(a) a
#define IDENTITY_MACRO(a)
#endif
#ifndef ASSERT
#define ASSERT(a) IDENTITY_MACRO(assert(a))
#endif


template<typename RandomAccessIterator>
inline void AdvanceBackward(RandomAccessIterator &it, std::size_t n,
                           std::random_access_iterator_tag) {
  it -= n;
}

template<typename BidirectionalIterator>
inline void AdvanceBackward(BidirectionalIterator &it, std::size_t n,
                           std::bidirectional_iterator_tag) {
  while (n > 0) {
    --n;
    --it;
  }
}

template<typename Iterator>
inline void AdvanceBackward(Iterator &it, std::size_t n) {
  AdvanceBackward(it, n,
                  typename std::iterator_traits<Iterator>::iterator_category());
}

template<typename RandomAccessIterator>
inline void AdvanceForward(RandomAccessIterator &it, std::size_t n,
                           std::random_access_iterator_tag) {
  it += n;
}

template<typename BidirectionalIterator>
inline void AdvanceForward(BidirectionalIterator &it, std::size_t n,
                           std::bidirectional_iterator_tag) {
  while (n > 0) {
    --n;
    ++it;
  }
}

template<typename Iterator>
inline void AdvanceForward(Iterator &it, std::size_t n) {
  AdvanceForward(it, n,
                 typename std::iterator_traits<Iterator>::iterator_category());
}


namespace mwob_namespace {

// Helper Function for Trim().
//Assumes that std::distance(start, one_past_end) >= 2.
// Rotates [start, one_past_end) leftwards by 1.
// i.e. this range is rotated so that start becomes the last value in the range
//      while start+1 becomes the first value in the range.
//
template<typename ForwardIterator, typename Distance>
inline void RotateLeftBy1(ForwardIterator start,
                          ForwardIterator one_past_end) {
  auto it_one_ahead = start;
  (void)++it_one_ahead;
  auto start_value = std::move(*start);
  while (it_one_ahead != one_past_end) {
    *start = std::move(*it_one_ahead);
    start = it_one_ahead++;
  }
  *start = std::move(start_value);
  return ;
}

//
// Helper Function for Trim().
// Rotates [start, end] rightwards by 1.
// i.e. if end := one_past_end - 1 then this range is rotated
//  so that end becomes the first value in the range.
//
// Assumes that:
// (1) std::distance(start, end) >= 1
//
template<typename BidirectionalIterator, typename Distance>
inline void RotateRightBy1(BidirectionalIterator start,
                           BidirectionalIterator end) {
  auto it_one_behind = end;
  (void)--it_one_behind;
  auto end_value = std::move(*end);
  while (true) {
    *end = std::move(*it_one_behind);
    if (it_one_behind == start)
      break ;
    end = it_one_behind--;
  }
  *start = std::move(end_value);
  return ;
}

// Assumes that:
// (1) std::distance(start, end) >= 1
// (2) length_minus1 == std::distance(start, end)
template<typename BidirectionalIterator, typename Distance>
inline void RotateRightBy1(BidirectionalIterator start,
                           BidirectionalIterator end,
                           Distance length_minus1) {
ASSERT(start != end);
ASSERT(length_minus1 == std::distance(start, end));
ASSERT(length_minus1 >= 1);
  auto it_one_behind = end;
  (void)--it_one_behind;
  auto end_value = std::move(*end);
  while (--length_minus1 > 0) {
ASSERT(it_one_behind != start);
    *(end--) = std::move(*(it_one_behind--));
  }
ASSERT(it_one_behind == start);
  *end   = std::move(*start);
  *start = std::move(end_value);
  return ;
}

// Does the equivalent of: while(*end_right >= *end_left) end_right--;
// NOTE: This means that it assumes that such an index exists exists within
//  [start_it : end_it] so IT DOES NO ITERATOR BOUNDS CHECKING.
// Does the same thing as LargestIndexWithValueLessThan(), except that its
//  inputs are different and also, in addition to performing a binary search,
//  it simultaneously performs a linear search starting from the end_it.
// Assumes that start_it <= end_it.
// SE means that the inputs are start_index and end_index.
// This algorithm find d by performing <= 3 * min {dist(start_it, d) + 1,
//  dist(d, end_it) + 1, ceil(log_2(dist(start_it, end_it + 1)))} comparisons.
//
template<typename RandomAccessIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline RandomAccessIterator LargestIteratorLessThan_KnownToExist_RAI(
                        RandomAccessIterator start_it,
                        RandomAccessIterator end_it,
                        RandomAccessIterator one_past_end,
                        ValueType &value,
                        Compare comp,
                        CompareLessOrEqual comp_le,
                        const bool check_end_iterator_each_iteration = false,
                        const bool check_one_after_start_iterator_each_iteration = false) {
  //The below return statement makes execution slightly slower.
  //return --std::lower_bound(start_it, one_past_end, value);
ASSERT(comp(*start_it, value));
  while (true) {//Use:while(start_it < end_it){ if the d is not known to exist.
ASSERT(comp(*start_it, value));
    if (check_end_iterator_each_iteration) {
      //The below three lines are optional. They are performed at most
      // std::distance(d, end_it) + 1 times.
      if (comp(*end_it, value))
        break ;
ASSERT(end_it != start_it);
      (void)--end_it;
    }

    auto dist = std::distance(start_it, end_it);
    if (dist <= 1) {
      if (dist == 1 && !comp(*end_it, value))
      //Formerly: if (!(dist == 1 && comp(*end_it, value)))
        end_it = start_it;
      break ;
    }

    if (check_one_after_start_iterator_each_iteration) {
ASSERT(end_it != start_it);
      auto start_plus1 = start_it + 1;
      if (comp_le(value, *start_plus1)) {
        end_it = start_it;
        break ;
      }
      start_it = start_plus1;
      --dist;
    }

    auto d = start_it + (dist / 2);
    if (comp(*d, value)) {
      start_it = d;
    } else {
      end_it   = d - 1;
    }
  }
  return end_it;
}


//LargestIteratorLessThan_KnownToExist for bidirection iterators.
//Suppose that this function's return value is iter.
//Then distance_from_end_it_out will have the value
// distance_from_end_it_out == std::distance(iter, end_right).
template<typename BidirectionalIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline BidirectionalIterator LargestIteratorLessThan_KnownToExist_bi(
                        BidirectionalIterator start_it,
                        BidirectionalIterator end_right,
                        BidirectionalIterator one_past_end,
                        Distance length,
                        ValueType &value,
                        Distance &distance_from_end_it_out,
                        Compare comp,
                        CompareLessOrEqual comp_le,
                        const bool check_end_iterator_each_iteration = false,
                        const bool check_one_after_start_iterator_each_iteration = false
                        ) {
ASSERT(length == std::distance(start_it, one_past_end));
//const auto original_end_right = end_right; //variable used in a later assertion.
  Distance distance_from_end = 0;
  //Find the largest iterator it >= start_right such that comp(*it, value).
  //Such an iterator exists because comp(*start_right, *end_left).
  while (true) {
ASSERT(length > 0); ASSERT(length == std::distance(start_it, end_right) + 1);
//ASSERT(distance_from_end == std::distance(end_right, original_end_right));
    if (check_end_iterator_each_iteration) {
      if (comp(*end_right, value)) {
        break ;
      } else {
ASSERT(end_right != start_it); ASSERT(length > 1);
        (void)--end_right;
        --length;
        ++distance_from_end;
      }
    }
ASSERT(length > 0);
    if (length <= 2) {
      if (length == 2 && comp_le(value, *end_right)) {
        end_right = start_it;
        ++distance_from_end;
      }
ASSERT(comp(*end_right, value));
      break ;
    }

    if (check_one_after_start_iterator_each_iteration) {
ASSERT(end_right != start_it);
      auto start_plus1 = start_it;
      if (comp_le(value, *(++start_plus1))) {
        end_right = start_it;
        distance_from_end += (length - 1);
        break ;
      }
      start_it = start_plus1;
      --length;
    }

ASSERT(length >= 2);
    auto displacement = length / 2;
ASSERT(displacement >= 1); ASSERT(start_it != end_right);
    auto d = start_it;
    std::advance(d, displacement);
ASSERT(d != start_it);ASSERT(length > displacement);
    if (comp(*d, value)) {
      start_it = d;
      length  -= displacement;
    } else { //so comp_le(value, *d)
ASSERT(distance(start_it, d) == displacement);
      end_right = --d;
      distance_from_end += (length - displacement);
      length = displacement;
ASSERT(std::distance(start_it, d) + 1 == length);
//ASSERT(distance_from_end == std::distance(end_right, original_end_right));
    }
//ASSERT(distance_from_end == std::distance(end_right, original_end_right));
ASSERT(length == std::distance(start_it, end_right) + 1);
  }
  distance_from_end_it_out = distance_from_end;
  return end_right;
}

/*
template<typename BidirectionalIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline BidirectionalIterator LargestIteratorLessThan_KnownToExist_bi(
                  BidirectionalIterator start_it,
                  BidirectionalIterator end_right,
                  BidirectionalIterator one_past_end,
                  Distance length,
                  ValueType &value,
                  Compare comp,
                  CompareLessOrEqual comp_le,
                  const bool check_end_iterator_each_iteration = false,
                  const bool check_initial_end_iterator = false) {
ASSERT(length == std::distance(start_it, one_past_end));
//const auto original_end_right = end_right; //variable used in a later assertion.
  if (check_initial_end_iterator) {
    if (comp(*end_right, value)) {
      return end_right;
    }
ASSERT(end_right != start_it); ASSERT(length > 1);
    (void)--end_right;
    --length;
  }// else { ASSERT(!comp(*end_right, value)); }
  //Find the largest iterator it >= start_right such that comp(*it, value).
  //Such an iterator exists because comp(*start_right, *end_left).
  while (true) {
ASSERT(length > 0); ASSERT(length == std::distance(start_it, end_right) + 1);
    if (check_end_iterator_each_iteration) {
      if (comp(*end_right, value)) {
        return end_right;
      } else {
ASSERT(end_right != start_it); ASSERT(length > 1);
        (void)--end_right;
        --length;
      }
    }
ASSERT(length > 0);
    if (length == 1) {
ASSERT(start_it == end_right); ASSERT(comp(*end_right, value));
      return end_right;
    }
ASSERT(length >= 2);
    auto displacement = length / 2;
ASSERT(displacement >= 1); ASSERT(start_it != end_right);
    auto d = start_it;
    std::advance(d, displacement);
ASSERT(d != start_it);ASSERT(length > displacement);
    if (comp(*d, value)) {
      start_it = d;
      length  -= displacement;
    } else { //so comp_le(value, *d)
ASSERT(distance(start_it, d) == displacement);
      (void)--d;
      length = displacement;
ASSERT(std::distance(start_it, d) + 1 == length);
      end_right = d;
    }
ASSERT(length == std::distance(start_it, end_right) + 1);
  }
}
*/





// This function first finds the smallest n such that start_it + n is defined
//  and comp(value, *(start_it + n)) == true.
//Such an n is assumed to exist.
//
// Does the equivalent of:
//     while(start_it < end_it && *start_it <= *end_it) start_it++;
//  except that it finds the resulting start_it via a binary search.
// In addition to performing a binary search, it may simultaneously performs a
//  linear search starting from the end_it.
template<typename RandomAccessIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline RandomAccessIterator SmallestIteratorGreaterThan_KnownToExist_RAI(
            RandomAccessIterator start_it,
            RandomAccessIterator end_it,
            RandomAccessIterator one_past_end,
            ValueType &value,
            Compare comp,
            CompareLessOrEqual comp_le,
            const bool check_start_iterator_each_iteration = false,
            //const bool check_if_start_equals_end_before_loop = false,
            const bool check_one_before_end_iterator_each_iteration = false
            ) {
  //The below return statement makes execution slightly slower.
  //return std::upper_bound(start_it, one_past_end, value);
  while (true) {
    if (check_start_iterator_each_iteration) {
      //The below lines are optional. They are performed at most
      // std::distance(start_it, end_it) + 1 times.
      if (comp(value, *start_it))
        break ;
      (void)++start_it;
//ASSERT(start_it != one_past_end);
    }

    auto cur_length_minus_one = std::distance(start_it, end_it);
    if (cur_length_minus_one <= 1) {
      if (cur_length_minus_one == 1 && !comp(value, *start_it))
        start_it = end_it;
      break ;
    }

    if (check_one_before_end_iterator_each_iteration) {
      //The below lines are optional. They are performed at most
      // std::distance(d, end_it) + 1 times.
//ASSERT(end_it != start_it);
      auto one_before_end_it = end_it - 1;
//ASSERT(one_before_end_it == (start_it + (std::distance(start_it, end_it) - 1)));
      if (comp_le(*one_before_end_it, value)) {
        start_it = end_it;
        break ;
      }
      end_it = one_before_end_it;
      --cur_length_minus_one;
    }
    auto d = start_it + (cur_length_minus_one / 2);
    if (comp_le(*d, value))
      start_it = d + 1; //Note that start_it will be <= end_it since the
                        //desired iterator is known to exist.
    else
      end_it   = d;
  }
  //At this point, start_it == end_it OR start_it == end_it + 1.
  //return start_it + (vec[start_it] > value ? 0 : 1);
  return start_it;
}



//Suppose that this function's return value is iter.
//Then distance_from_start_out will have the value:
// distance_from_start_out == std::distance(start_it, iter)..
template<typename BidirectionalIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline BidirectionalIterator SmallestIteratorGreaterThan_KnownToExist_bi(
                  BidirectionalIterator start_it,
                  BidirectionalIterator end_it,
                  BidirectionalIterator one_past_end,
                  Distance length,
                  ValueType &value,
                  Distance &distance_from_start_out,
                  Compare comp,
                  CompareLessOrEqual comp_le,
                  const bool check_start_iterator_each_iteration = false,
                  const bool check_one_before_end_iterator_each_iteration = false
                  //const bool check_if_start_equals_end_before_loop = false
                  ) {
  //return std::upper_bound(start_it, one_past_end, value, comp);
ASSERT(comp(value, *end_it));
ASSERT(length == std::distance(start_it, one_past_end)); ASSERT(length > 0);
  Distance distance_from_start = 0;
  auto cur_length_minus_one = length - 1;
  /*if (check_if_start_equals_end_before_loop) {
    if (start_it == end_it) {
      distance_from_start_out = distance_from_start;
      return start_it;
    }
ASSERT(length > 1);
  }*/
  while (true) {
    if (check_start_iterator_each_iteration) {
      //The below lines are optional. They are performed at most
      // std::distance(start_it, end_it) + 1 times.
      if (comp(value, *start_it))
        break ;
      (void)++start_it;
      --cur_length_minus_one;
      ++distance_from_start;
ASSERT(start_it != one_past_end); ASSERT(cur_length_minus_one >= 0);
    }

    if (cur_length_minus_one <= 1) {
      if (cur_length_minus_one == 1 && !comp(value, *start_it))  {
        start_it = end_it;
        ++distance_from_start;
      }
      break ;
    }
    if (check_one_before_end_iterator_each_iteration) {
      //The below lines are optional. They are performed at most
      // std::distance(d, end_it) + 1 times.
//ASSERT(end_it != start_it);
      auto one_before_end_it = end_it;
      (void)--one_before_end_it;
//ASSERT(one_before_end_it == (start_it + (std::distance(start_it, end_it) - 1)));
      if (comp_le(*one_before_end_it, value)) {
        start_it = end_it;
        distance_from_start += cur_length_minus_one;
        break ;
      }
      end_it = one_before_end_it;
      --cur_length_minus_one;
      if (cur_length_minus_one <= 1) {
        if (cur_length_minus_one == 1 && !comp(value, *start_it))  {
          start_it = end_it;
          ++distance_from_start;
        }
        break ;
      }
    } else {
ASSERT(cur_length_minus_one >= 2);
    }
ASSERT(cur_length_minus_one >= 1);
    Distance displacement = (cur_length_minus_one / 2);
ASSERT(displacement >= 1);
    auto d = start_it;
    std::advance(d, displacement);
ASSERT(displacement <= cur_length_minus_one);
ASSERT(std::distance(start_it, end_it) == cur_length_minus_one);
    if (comp_le(*d, value)) {
      start_it = d; //Note that start_it will be <= end_it since the
                        //desired iterator is known to exist.
      (void)++start_it;
      ++displacement;
      distance_from_start += displacement;
ASSERT(displacement < length);
      cur_length_minus_one -= displacement;
ASSERT(std::distance(start_it, end_it) == cur_length_minus_one);
    } else { // comp(value, *d)
ASSERT(std::distance(start_it, d) == displacement);
      end_it = d;
      cur_length_minus_one = displacement;
ASSERT(std::distance(start_it, end_it) == cur_length_minus_one);
    }
  }
  distance_from_start_out = distance_from_start;
  return start_it;

/*
  auto return_value = std::upper_bound(start_it, one_past_end, value, comp);
  distance_from_start_out = std::distance(start_it, return_value);
  return return_value;
  */
}





// Helper Function.
//
// Assumes that:
// (1) length_left_out > 1 where
//     length_left_out == std::distance(start_left_out, end_left) + 1
// (2) comp(start_right_value, *end_left)
// (3) comp_le(*start_left_out, start_right_value)
//
// This function will find the smallest iterator iter such that
//  start_left_out <= iter <= end_left and comp(start_right_value, *iter).
// It will then replace start_left_out with iter, replace length_left_out with
//  std::distance(iter, end_left) + 1
template<typename RandomAccessIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline void TrimLeft1_RAI(RandomAccessIterator &start_left_out,
                  RandomAccessIterator end_left,
                  ValueType &start_right_value,
                  Distance &length_left_out,
                  Compare comp,
                  CompareLessOrEqual comp_le,
                  const bool check_start_iterator_each_iteration = false,
                  const bool check_one_before_end_iterator_each_iteration = false) {
  auto start_left = start_left_out;
  //i.e. if *start_left <= *start_right ASSERT(comp_le(*start_left, *start_right));
ASSERT(start_left != end_left);
  //Find the smallest iterator start_left_out <= it < start_right such that
  // *end_right < *it, which exists because *end_right < *end_left.
  //start_left is replaced by this iterator.
  auto new_start_left = SmallestIteratorGreaterThan_KnownToExist_RAI<
    RandomAccessIterator, Compare, Distance, CompareLessOrEqual, ValueType>(
        start_left + 1, end_left, end_left + 1, start_right_value, comp, comp_le,
        check_start_iterator_each_iteration,
        check_one_before_end_iterator_each_iteration
        );
  start_left_out = new_start_left;
  length_left_out = std::distance(new_start_left, end_left) + 1;
ASSERT(length_left_out >= 1);
//ASSERT(comp(*start_right, *new_start_left));
  return ;
}


template<typename BidirectionalIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline void TrimLeft1_bi(BidirectionalIterator &start_left_out,
                         BidirectionalIterator end_left,
                         BidirectionalIterator start_right,
                         Distance &length_left_out,
                         Compare comp,
                         CompareLessOrEqual comp_le,
                         const bool check_start_iterator_each_iteration = false,
                         const bool check_one_before_end_iterator_each_iteration = false) {
  auto start_left = start_left_out;
  //i.e. if *start_left <= *start_right
ASSERT(comp_le(*start_left, *start_right));
ASSERT(start_left != end_left && start_left != start_right);
  auto start_left_plus1 = start_left;
  (void)++start_left_plus1;
  Distance distance_from_start_it;
  auto length_left = length_left_out - 1;
  //Find the smallest iterator start_left_out <= it < start_right such that
  // *end_right < *it, which exists because *end_right < *end_left.
  //start_left is replaced by this iterator.
  auto new_start_left = SmallestIteratorGreaterThan_KnownToExist_bi<
      BidirectionalIterator, Compare, Distance, CompareLessOrEqual, ValueType>(
          start_left_plus1, end_left, start_right, length_left, *start_right,
          distance_from_start_it,
          comp, comp_le,
          check_start_iterator_each_iteration,
          check_one_before_end_iterator_each_iteration
        );
ASSERT(distance_from_start_it == std::distance(start_left_plus1, new_start_left));
ASSERT(distance_from_start_it < length_left);
ASSERT(comp(*start_right, *new_start_left));
  start_left_out = new_start_left;
  length_left_out = length_left - distance_from_start_it;
ASSERT(length_left_out == std::distance(new_start_left, start_right));
ASSERT(length_left_out >= 1);
  return ;
}

//
// Helper Function.
//
// Assumes that:
// (1) length_right_out > 1 where
//     length_right_out == std::distance(start_right, one_past_end_out)
// (2) end_left_value == *(start_right - 1)
// (3) comp(*start_right, end_left_value)
// (4) comp_le(end_left_value, *end_right)
// (5) end_right_out + 1 == one_past_end_out
//
// This function will find the largest iterator iter such that
//  start_right <= iter <= end_right_out and comp(*iter, end_left_value).
// It will then replace end_right_out with iter, replace one_past_end_out
//  with iter + 1, and replace length_right_out with
//  std::distance(start_right, iter + 1)
template<typename RandomAccessIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline void TrimRight1_RAI(ValueType &end_left_value,
                           RandomAccessIterator start_right,
                           RandomAccessIterator &end_right_out,
                           RandomAccessIterator &one_past_end_out,
                           Distance &length_right_out,
                           Compare comp,
                           CompareLessOrEqual comp_le,
                           const bool check_end_iterator_each_iteration = false,
                           const bool check_one_after_start_iterator_each_iteration = false) {
  auto end_right = end_right_out;
IDENTITY_MACRO(auto end_left = start_right - 1;)
ASSERT(comp(*start_right, end_left_value));
ASSERT(comp_le(end_left_value, *end_right)); //i.e. *end_left <= *end_right
ASSERT(end_right != start_right);
  auto one_past_end = end_right--;
  //Find the largest iterator it >= start_right such that
  // comp(*it, *end_left) and replace end_right with it.
  //Such an iterator exists because comp(*start_right, *end_left).
  auto new_end_right = LargestIteratorLessThan_KnownToExist_RAI<
    RandomAccessIterator, Compare, Distance, CompareLessOrEqual, ValueType>(
          start_right, end_right, one_past_end, end_left_value,
          comp, comp_le, check_end_iterator_each_iteration,
          check_one_after_start_iterator_each_iteration);
ASSERT(new_end_right != end_right_out);
  end_right_out = new_end_right++;
  one_past_end_out = new_end_right;
  length_right_out = std::distance(start_right, new_end_right);
ASSERT(length_right_out >= 1);
ASSERT(comp(*end_right_out, *end_left));
  return ;
}

template<typename BidirectionalIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline void TrimRight1_bi(ValueType &end_left_value,
                          BidirectionalIterator start_right,
                          BidirectionalIterator &end_right_out,
                          BidirectionalIterator &one_past_end_out,
                          Distance &length_right_out,
                          Compare comp,
                          CompareLessOrEqual comp_le,
                          const bool check_end_iterator_each_iteration = false,
                          const bool check_one_after_start_iterator_each_iteration = false) {
  auto end_right = end_right_out;
IDENTITY_MACRO(auto end_left = IteratorMinus1<BidirectionalIterator>(start_right);)
ASSERT(comp(*start_right, *end_left));
ASSERT(comp_le(*end_left, *end_right)); //i.e. *end_left <= *end_right
ASSERT(end_right != start_right);
  Distance length_right = length_right_out - 1;
  Distance distance_from_end_it;
  auto one_past_end = end_right--;
  //Find the largest iterator it >= start_right such that
  // comp(*it, *end_left) and replace end_right with it.
  //Such an iterator exists because comp(*start_right, *end_left).
  auto new_end_right = LargestIteratorLessThan_KnownToExist_bi<
     BidirectionalIterator, Compare, Distance, CompareLessOrEqual, ValueType>(
          start_right, end_right, one_past_end, length_right, end_left_value,
          distance_from_end_it, comp, comp_le,
          check_end_iterator_each_iteration,
          check_one_after_start_iterator_each_iteration);
ASSERT(new_end_right != end_right_out);
ASSERT(distance_from_end_it == std::distance(new_end_right, end_right));
ASSERT(distance_from_end_it < length_right);
ASSERT(comp(*new_end_right, *end_left));
  end_right_out = new_end_right++;
  one_past_end_out = new_end_right;
  length_right_out = length_right - distance_from_end_it;
ASSERT(length_right_out == std::distance(start_right, new_end_right));
ASSERT(length_right_out >= 1);
ASSERT(length_right_out >= 1);
ASSERT(comp(*end_right_out, *end_left));
  return ;
}



/* Finds the SMALLEST integer  0 <= d < length (assumed to exist) such that
 *  (*) *(end_left - d) <= *(start_right + d)
 * Assumes that [start_left : start_index + length - 1] is non-decreasing,
 *  that all these elements exist, that BOTH ranges have size >= length,
 *  and that length > 0.
 * WARNING: POTENTIAL FALSE POSITIVE: There are two ways that this function may
 *  return length - 1:
 *  1) Such a d does NOT exist. i.e. *start_index > *end_right.
 *  2) Such a d exists and happens to equal length - 1. i.e.
 *     *start_left <= *end_right and *(start_left + 1) > *(end_right - 1]
 * NOTES:
 *  (1) If *end_left > *(end_left + 1) and such a d exists then d is
 *       necessarily > 0.
 */
template<typename RandomAccessIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline Distance DisplacementToPotentialMedians_KnownToExist_RAI(
                                            RandomAccessIterator end_left,
                                            RandomAccessIterator start_right,
                                            Distance length,
                                            Compare comp,
                                            CompareLessOrEqual comp_le) {
ASSERT(start_right == end_left + 1);
  (void)--length;       //We will now use length as if it were d_upper.
  Distance d_lower = 0; //So that end_left - d_lower = end_left
  do {
    auto d = d_lower + ((length - d_lower) / 2); //No overflow formula
    if (comp_le(*(end_left - d), *(start_right + d))) {
      length  = d;
    } else {
      d_lower = d + 1;
    }
  } while (d_lower < length) ;
  return d_lower;
}


} //END namespace: merge_without_buffer_common_namespace

#ifdef ASSERT
#undef ASSERT
#endif
#ifdef IDENTITY_MACRO
#undef IDENTITY_MACRO
#endif


#endif /* SRC_MERGE_WITHOUT_BUFFER_COMMON_H_ */
