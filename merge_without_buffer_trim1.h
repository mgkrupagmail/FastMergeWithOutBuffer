/*
 * merge_without_buffer_trim1.h
 *
 *  Created on: Jun 30, 2017
 *      Author: Matthew Gregory Krupa
 *
 *  A greatly simplified version of MergeWithOutBuffer().
 *  MergeWithOutBufferTrim1() contains the near minimum code needed to make the
 *   merge algorithm work.
 *  It is also the slowest of MergeWithOutBuffer(), MergeWithOutBufferTrim4(),
 *   MergeWithOutBufferTrim3(), MergeWithOutBufferTrim2(), and
 *   MergeWithOutBufferTrim1().
 */

#ifndef SRC_MERGE_WITHOUT_BUFFER_TRIM1_H_
#define SRC_MERGE_WITHOUT_BUFFER_TRIM1_H_

#include <algorithm>

#include "merge_common.h"

/* This function does the same thing as TrimEnds5() EXCEPT that only condition
 *  (1) is guaranteed. See TrimEnds5()'s documentation for details since this
 *  is just TrimEnds5() with some code removed.
 * If *end_left <= *start_right then we return true. Otherwise, we return false.
 */
template<class RandomAccessIterator1, class RandomAccessIterator2>
bool TrimEnds1(RandomAccessIterator1 &start_left_out,
               RandomAccessIterator1 &end_left_out,
               RandomAccessIterator2 &start_right_out,
               RandomAccessIterator2 &end_right_out) {
  auto start_left  = start_left_out,  end_left  = end_left_out;
  auto start_right = start_right_out, end_right = end_right_out;
  bool is_trivial = false;
  while (true) {
    if (*end_left <= *start_right || end_left < start_left
                                  || end_right < start_right) {
      return true;
    }
    //If true, then this implies that start_left < end_left
    if (*start_left <= *start_right)
      start_left = SmallestIteratorWithValueGreaterThan_KnownToExist(
                                       start_left + 1, end_left, *start_right);
    if (*end_left <= *end_right)
      end_right = LargestIteratorWithValueLessThan_KnownToExist(
                                        start_right, end_right - 1, *end_left);
    if (*end_right <= *start_left || end_left <= start_left
        || end_right <= start_right) {
      is_trivial = true;
      break;
    }
    //Note that at this point,
    // 1) both length_left and length_right are >= 2, and
    // 2) *end_left > *end_right > *start_left > *start_right.

    auto length_left  = std::distance(start_left, end_left + 1);
    auto length_right = std::distance(start_right, end_right + 1);
    if (length_left <= length_right && *(start_right + length_left - 1) <=
                                                                 *start_left) {
      std::swap_ranges(start_left, end_left + 1, start_right);
      start_left   = start_right;
      start_right += length_left;
      end_left    += length_left;
      continue ;
    }
    if (length_left >= length_right && *end_right <=
                                            *(end_left - (length_right - 1))) {
      std::swap_ranges(start_right, end_right + 1, end_left - (length_right-1));
      end_left   -= length_right;
      start_right = end_left + 1;
      end_right   = start_right + (length_right - 1);
      continue ;
    }
    break ;
  }

  if (is_trivial) {
    MergeTrivialCases(start_left, end_left, start_right, end_right);
    return true;
  }
  start_left_out   = start_left;
  end_left_out     = end_left;
  start_right_out  = start_right;
  end_right_out    = end_right;
  return false;
}

//Assumes that start_left <= start_right
template<class RandomAccessIterator1, class RandomAccessIterator2>
void MergeWithOutBufferTrim1(RandomAccessIterator1 start_left,
                             RandomAccessIterator1 end_left,
                             RandomAccessIterator2 start_right,
                             RandomAccessIterator2 end_right,
                             std::size_t length_left,
                             std::size_t length_right) {
  if (TrimEnds1(start_left, end_left, start_right, end_right)) {
    return ;
  }
  length_left  = std::distance(start_left, end_left) + 1;
  length_right = std::distance(start_right, end_right) + 1;
  auto length_smaller = length_left < length_right ? length_left : length_right;
  //Check for triviality.
  if (length_smaller <= 1) {
    MergeTrivialCases(start_left, end_left, start_right, end_right, length_left,
                      length_right);
    return ;
  }
  auto d = DisplacementFromMiddleIteratorToPotentialMedians_KnownToExist(
                                         end_left, start_right, length_smaller);
  {
    auto start_2nd_quarter = end_left;
    std::advance(start_2nd_quarter, - static_cast<long>(d - 1));
    auto one_past_end_2nd_quarter = end_left;
    ++one_past_end_2nd_quarter;
    std::swap_ranges(start_2nd_quarter, one_past_end_2nd_quarter, start_right);
    std::size_t length_first_quarter = length_left - d;
//assert(static_cast<long long>(length_first_quarter) ==
//                                  std::distance(start_left, start_2nd_quarter));
//assert(d == std::distance(start_2nd_quarter, (end_left + 1)));
    MergeWithOutBufferTrim1<RandomAccessIterator1, RandomAccessIterator1>(start_left, start_2nd_quarter - 1,
        start_2nd_quarter, end_left, length_first_quarter, d);
  }

  auto start_4th_quarter = start_right;
  std::advance(start_4th_quarter, d);
  auto end_3rd_quarter = start_4th_quarter;
  --end_3rd_quarter;
  std::size_t length_4th_quarter = length_right - d;
//assert(static_cast<long>(d) == std::distance(start_right, start_4th_quarter));
//assert(static_cast<long>(length_4th_quarter) ==
//                             std::distance(start_4th_quarter, (end_right + 1)));
  MergeWithOutBufferTrim1<RandomAccessIterator2, RandomAccessIterator2>(start_right, end_3rd_quarter,
      start_4th_quarter, end_right, d, length_4th_quarter);
  return ;
}

template<class RandomAccessIterator1, class RandomAccessIterator2>
inline void MergeWithOutBufferTrim1(RandomAccessIterator1 start_left,
                                    RandomAccessIterator1 end_left,
                                    RandomAccessIterator2 start_right,
                                    RandomAccessIterator2 end_right) {
  auto length_left  = std::distance(start_left, end_left + 1);
  auto length_right = std::distance(start_right, end_right + 1);
  MergeWithOutBufferTrim1<RandomAccessIterator1, RandomAccessIterator2>(start_left, end_left,
                             start_right, end_right, length_left, length_right);
  return ;
}

template<class RandomAccessIterator>
inline void MergeWithOutBufferTrim1(RandomAccessIterator start_left,
                                    RandomAccessIterator start_right,
                                    RandomAccessIterator one_past_end) {
  auto length_left  = std::distance(start_left, start_right);
  auto length_right = std::distance(start_right, one_past_end);
  MergeWithOutBufferTrim1<RandomAccessIterator, RandomAccessIterator>(
                             start_left, start_right - 1,
                             start_right, one_past_end - 1,
                             length_left, length_right);
  return ;
}

#endif /* SRC_MERGE_WITHOUT_BUFFER_TRIM1_H_ */
