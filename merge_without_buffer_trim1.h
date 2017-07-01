/*
 * merge_without_buffer_trim1.h
 *
 *  Created on: Jun 30, 2017
 *      Author: Matthew Gregory Krupa
 *
 *  A greatly simplified version of MergeWithOutBuffer().
 *  MergeWithOutBufferTrim1() contains the near minimum code needed to make the
 *   merge algorithm work.
 *  It is also the slowest of MergeWithOutBuffer(), MergeWithOutBufferTrim 2(),
 *   and MergeWithOutBufferTrim1().
 */

#ifndef SRC_MERGE_WITHOUT_BUFFER_TRIM1_H_
#define SRC_MERGE_WITHOUT_BUFFER_TRIM1_H_

#include <algorithm>

#include "merge_common.h"

/* Given two sorted ranges of values that are contiguous in memory as
 *  [start_left : end_right] this function will try function will try to
 *  increase start_left and decrease end_right as much as possible using only
 *  simple comparisons and switches near the ends of these two subintervals.
 * Assumes that both [start_left : end_left] and [start_right : end_right] are
 *  sorted in increasing order, and that
 *  start_left <= end_left == start_right - 1 < start_right <= end_right
 *  (IMPORTANT: Note the "end_left == start_right - 1")
 *
 * If *end_left <= *start_right then we make the intervals invalid. i.e.
 *  start_left_out = start_right; end_right_out = end_left;
 * After execution completes [initial_start_left : end_left] and
 *  [start_right : initial_end_right] will both still be increasing
 *  (where initial_start_left and initial_end_right refer to the values of
 *   start_left and end_right when this function is first entered).
 *
 * If after execution completes both subranges have size >= 1, then it is
 *  guaranteed that:
 *  1) *start_left > *start_right
 *  2) *end_right  < *end_left
 *  3) *start_left < *end_right
 * If after execution completes, start_right > end_right or start_left >end_left
 *  then the two subranges have been completely merged;
 *  OTHERWISE both subranges have length >= 2 (i.e. start_left < end_left &&
 *   start_right < end_right).
 */
template<class T>
void TrimEnds1(T &start_left_out,  T &end_left_out,
               T &start_right_out, T &end_right_out) {
  auto start_left  = start_left_out,  end_left  = end_left_out;
  auto start_right = start_right_out, end_right = end_right_out;
  bool is_trivial = false;
  while (true) {
    if (*end_left <= *start_right || start_left >= start_right) {
      start_left_out   = end_left + 1;
      end_left_out     = end_left;
      start_right_out  = end_right + 1;
      end_right_out    = end_right;
      return ;
    }
    //If true, then this implies that start_left < end_left
    if (*start_left <= *start_right)
      start_left = SmallestIteratorWithValueGreaterThan_KnownToExist(
                                       start_left + 1, end_left, *start_right);
    if (*end_right >= *end_left)
      end_right = LargestIteratorWithValueLessThan_KnownToExist(start_right,
                                                     end_right - 1, *end_left);
    if (*start_left >= *end_right || start_left >= end_left
        || start_right >= end_right) {
      is_trivial = true;
      break;
    }
    //Note that at this point,
    // 1) both length_left and length_right are >= 2, and
    // 2) *end_left > *end_right > *start_left > *start_right.

    auto length_left  = std::distance(start_left, end_left + 1);
    auto length_right = std::distance(start_right, end_right + 1);
    if (length_left <= length_right && *start_left >=
                                            *(start_right + length_left - 1)) {
      std::swap_ranges(start_left, end_left + 1, start_right);
      start_left   = start_right;
      start_right += length_left;
      end_left    += length_left;
      continue ;
    }
    if (length_left >= length_right && *(end_left - (length_right - 1)) >=
                                                                  *end_right) {
      std::swap_ranges(start_right, end_right + 1, end_left - (length_right-1));
      end_left   -= length_right;
      start_right = end_left + 1;
      end_right   = start_right + (length_right - 1);
      continue ;
    }
    break ;
  }

  if (is_trivial) {
    MergeTrivialCases(start_left, end_left, start_right, end_right,
                      &start_left_out, &end_right_out);
    start_left_out   = end_left + 1;
    end_left_out     = end_left;
    start_right_out  = end_right + 1;
    end_right_out    = end_right;
    return ;
  }
  start_left_out   = start_left;
  end_left_out     = end_left;
  start_right_out  = start_right;
  end_right_out    = end_right;
  return ;
}

//Assumes that start_left <= start_right
template<class RAI>
void MergeWithOutBufferTrim1(RAI start_left,  RAI end_left,
                             RAI start_right, RAI end_right) {
  int length_left, length_right, length_smaller, d;
  TrimEnds1(start_left, end_left, start_right, end_right);
  length_left  = std::distance(start_left, end_left + 1);
  length_right = std::distance(start_right, end_right + 1);
  length_smaller = length_left < length_right ? length_left : length_right;
  //Check for triviality.
  if (length_smaller <= 1) {
    MergeTrivialCases(start_left, end_left, start_right, end_right, length_left,
                      length_right);
    return ;
  }
  d = DisplacementFromMiddleIiteratorToPotentialMediansContiguous_KnownToExist(
                                                      end_left, length_smaller);
  auto start_2nd_quarter = end_left - (d - 1);
  std::swap_ranges(start_2nd_quarter, end_left + 1, start_right);
  auto start_4th_quarter = start_right + d;
  MergeWithOutBufferTrim1(start_left, start_2nd_quarter - 1, start_2nd_quarter,
                          end_left);
  MergeWithOutBufferTrim1(start_right, start_4th_quarter - 1, start_4th_quarter,
                          end_right);
  return ;
}

#endif /* SRC_MERGE_WITHOUT_BUFFER_TRIM1_H_ */
