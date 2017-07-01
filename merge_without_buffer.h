/*
 * merge_without_buffer.h
 *
 *  Created on: Jun 29, 2017
 *      Author: Matthew Gregory Krupa
 *
 *  The most important function defined in this file is MergeWithOutBuffer().
 *  MergeWithOutBufferTrim1() contains the near minimum code needed to make the
 *   merge algorithm work.
 *  It is the fastest of MergeWithOutBuffer(), MergeWithOutBufferTrim 2(),
 *   and MergeWithOutBufferTrim1().
 */

#ifndef SRC_MERGE_WITHOUT_BUFFER_H_
#define SRC_MERGE_WITHOUT_BUFFER_H_

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
 * If after execution completes both subranges have size >= 3, (i.e.
 *  start_right - start_left >= 3 and end_left + 1 - start_right >= 3)
 *  then it is guaranteed that:
 *  1) *start_left > *(start_right + 2)
 *  2) *end_right  < *(end_left - 2)
 * If after execution completes both subranges have size >= 2, then it is
 *  guaranteed that:
 *  1) *start_left > *(start_right + 1)
 *  2) *end_right  < *(end_left - 1)
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
void TrimEnds(T &start_left_out,  T &end_left_out,
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

    //NOTE/REMINDER: Up to 10 - 20% of elements are sometimes emplaced by the
    // code in between this comment and definitions of length_left and
    // length_right below.
    //Also the first two main loops (i.e. the loops BEFORE
    // while(*(end_right - 1) >= *(end_left - 2)) {...}) combined do about
    // 2/3 of this total work.
    //The two main while() loops after this if statement require that both the
    // left and right vectors have at least elements 2 elements.
    //
    //Since start_right + 1 <= end_right, this is within bounds.
    if (*start_left <= *(start_right + 1)) {
      do {
        //Both vectors will remain non-decreasing after this swap.
        std::swap(*(start_left++), *start_right);
      } while (*start_left <= *(start_right + 1)) ;//start_left <= end_left
                           //since *end_left > *end_right >= *(start_right + 1).
      //At this point, *start_left > *start_right since
      // *start_left > *(start_right + 1) >= *start_right.
      if (start_left >= end_left || *start_left >= *end_right) {
        is_trivial = true;
        break;
      }
      //At this point, *start_left > *(start_right + 1) and
      // [start_left : end_left] has at least 2 elements.
    }

    //*(end_left - 1) is well-defined since start_left < end_left.
    if (*(end_left - 1) <= *end_right) {
      do {//Note end_right >= start_right + 1 since *end_left >= *(end_left - 1)
                        // >= *start_left > *(start_right + 1) >= *start_right.
        std::swap(*(end_right--), *end_left);
      } while (*(end_left - 1) <= *end_right) ;
      if (start_right >= end_right || *start_left >= *end_right) {
        is_trivial = true;
        break;
      }
      //Note end_right > start_right + 1 since *end_right > *start_left
      //  > *(start_right + 1) and [start_right : end_right] is non-decreasing.
    }
    //At this point, *(end_left - 1) > *end_right

    if (*(end_left - 2) <= *(end_right - 1)) {
      do {
        std::swap(*(end_left - 1), *(end_right - 1));
        std::swap(*end_left, *end_right);
        end_right -= 2;
      } while (*(end_left - 2) <= *(end_right - 1)) ;
      if (*(end_left - 1) <= *end_right)
        std::swap(*end_left, *(end_right--));
      if (*start_left >= *end_right) {
        is_trivial = true;
        break;
      }
    }

    if(*(end_left - 2) <= *end_right) {//This implies that *end_right >=
                                       // *(end_left - 2) > *(end_right - 1).
      auto temp       = *end_left;
      *end_left       = *(end_left - 1);
      *(end_left - 1) = *end_right;
      *end_right      = temp;
      --end_right;
      if (*start_left >= *end_right) {
        is_trivial = true;
        break;
      }
    }

    if (*(start_left + 1) <= *(start_right + 2)) {
      //In this case, our vector is something like:
      // 2 2 2 2 2 3 4 4 9 ...1000 0 1 4 6... 500, which we can efficiently
      // handle.
      do {
        std::swap(*(start_left + 1), *(start_right + 1));
        std::swap(*start_left, *start_right);
        start_left += 2;
        //At this point, *start_right <= *(start_right + 1) <= *start_left.
        //Note that *start_left==*start_right necessitates the below condition.
        if (*start_left == *(start_right + 1)) {
          //Note that initially, it's possible that *start_right < *start_left.
          std::swap(*(start_left++), *start_right);
          //At this point, *start_left >= *start_right == *(start_right + 1).
          //This loop make sure that *start_left > *start_right.
          while (*start_left == *start_right)
            ++start_left; //start_left will always be <= end_left.
        }
        //At this point, *start_left > *(start_right + 1) >= *start_right and
        // the right subvector's length is unchanged

        //Upon reaching this point the first time, our example
        // 2 2 2 2 2 3 4 4 9 ...1000 0 1 4 6... 500
        // has become: 3 4 4 9 ...1000 2 2 4 6... 500
      } while (*(start_left + 1) <= *(start_right + 2));
      //At this point our example 2 2 2 2 2 3 4 4 9 ...1000 0 1 4 6... 500
      // has become: 4 9 ...1000 3 4 4 6... 500
      if (*start_left == *(start_right + 1)) {
        std::swap(*(start_left++), *start_right);
      }
      if (*start_left >= *end_right) {
        is_trivial = true;
        break;
      }
    }

    if (*start_left <= *(start_right + 2)) {
      auto temp          = *start_left;
      *start_left        = *start_right;
      *start_right       = *(start_right + 1);
      *(start_right + 1)   = temp;
      ++start_left;
      if (*start_left >= *end_right) {
        is_trivial = true;
        break;
      }
    }
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
    MergeTrivialCases(start_left, end_left, start_right, end_right);
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
void MergeWithOutBuffer(RAI start_left,  RAI end_left,
                        RAI start_right, RAI end_right) {
  int length_left, length_right, length_smaller, d;
  TrimEnds(start_left, end_left, start_right, end_right);
  length_left  = std::distance(start_left, end_left + 1);
  length_right = std::distance(start_right, end_right + 1);
  length_smaller = length_left < length_right ? length_left : length_right;
  //Check for triviality.
  if (length_smaller <= 1) {
    MergeTrivialCases(start_left, end_left, start_right, end_right, length_left,
                      length_right);
    return ;
  }
  //At this point we're guaranteed to have start_left < start_right and
  // *start_left > *start_right.
  d = DisplacementFromMiddleIiteratorToPotentialMediansContiguous_KnownToExist(
                                                      end_left, length_smaller);
  auto start_2nd_quarter = end_left - (d - 1);
  std::swap_ranges(start_2nd_quarter, end_left + 1, start_right);
  auto start_4th_quarter = start_right + d;
  MergeWithOutBuffer(start_left, start_2nd_quarter - 1, start_2nd_quarter,
                     end_left);
  MergeWithOutBuffer(start_right, start_4th_quarter - 1, start_4th_quarter,
                     end_right);
  return ;
}

#endif /* SRC_MERGE_WITHOUT_BUFFER_H_ */
