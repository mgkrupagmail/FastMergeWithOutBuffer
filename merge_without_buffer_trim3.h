/*
 * merge_without_buffer_trim3.h
 *
 *  Created on: Jul 15, 2017
 *      Author: Matthew Gregory Krupa
 */

#ifndef SRC_MERGE_WITHOUT_BUFFER_TRIM3_H_
#define SRC_MERGE_WITHOUT_BUFFER_TRIM3_H_


#include <algorithm>

#include "merge_common.h"

/* This function does the same thing as TrimEnds4() EXCEPT that only conditions
 *  (1), (2), and (3) are guaranteed. See TrimEnds4()'s documentation for
 *  details since this is just TrimEnds4() with some code removed.
 * Its purpose is to help experimentally verify that the additional code in
 *  TrimEnds4() does not decrease the performance of the resulting merge
 *  function.
 */
template<class RAI, class RAI2>
void TrimEnds3(RAI &start_left_out,   RAI &end_left_out,
              RAI2 &start_right_out, RAI2 &end_right_out) {
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

    if (*(end_right - 2) >= *(end_left - 3)) {
      std::iter_swap(end_left - 2, end_right - 2);
      std::iter_swap(end_left - 1, end_right - 1);
      std::iter_swap(end_left, end_right);
      end_right = end_right - 3;
      if (start_right >= end_right) {
        is_trivial = true;
        break;
      }
      continue ;
    }

    if (*(start_left + 2) <= *(start_right + 3)) {
      std::iter_swap(start_left, start_right);
      std::iter_swap(start_left + 1, start_right + 1);
      std::iter_swap(start_left + 2, start_right + 2);
      start_left = start_left + 3;
      if (start_left >= end_left) {
        is_trivial = true;
        break;
      }
      continue ;
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
template<class RAI, class RAI2>
void MergeWithOutBufferTrim3(RAI start_left,   RAI end_left,
                        RAI2 start_right, RAI2 end_right) {
  int length_left, length_right, length_smaller, d;
  TrimEnds3(start_left, end_left, start_right, end_right);
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
  d = DisplacementFromMiddleIteratorToPotentialMedians_KnownToExist(end_left,
                                                   start_right, length_smaller);
  auto start_2nd_quarter = end_left - (d - 1);
  std::swap_ranges(start_2nd_quarter, end_left + 1, start_right);
  auto start_4th_quarter = start_right + d;
  MergeWithOutBufferTrim3(start_left, start_2nd_quarter - 1, start_2nd_quarter,
                     end_left);
  MergeWithOutBufferTrim3(start_right, start_4th_quarter - 1, start_4th_quarter,
                     end_right);
  return ;
}


#endif /* SRC_MERGE_WITHOUT_BUFFER_TRIM3_H_ */
