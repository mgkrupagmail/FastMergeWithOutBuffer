/*
 * merge_without_buffer_trim4.h
 *
 *  Created on: Jul 19, 2017
 *      Author: Matthew Gregory Krupa
 *
 *  A simplified version of MergeWithOutBuffer() that is faster but more
 *   complicated than MergeWithOutBufferTrim3() and slower than
 *   MergeWithOutBuffer().
 */

#ifndef SRC_MERGE_WITHOUT_BUFFER_TRIM4_H_
#define SRC_MERGE_WITHOUT_BUFFER_TRIM4_H_


#include <algorithm>

#include "merge_common.h"

/* Given two sorted ranges of values that are contiguous in memory as
 *  [start_left : end_right] this function will try to increase start_left
 *  and decrease end_right as much as possible using only simple comparisons
 *  and swaps near the ends of these two subintervals.
 * Assumes that both [start_left : end_left] and [start_right : end_right] are
 *  sorted in non-decreasing order, and that
 *  start_left <= end_left and start_right - 1 < start_right <= end_right.
 *
 * If *end_left <= *start_right then we return true. Otherwise, we return false.
 * After execution completes [initial_start_left : initial_end_left] and
 *  [initial_start_right : initial_end_right] will both be non-decreasing, where
 *  initial_start_left, initial_end_right, etc. refer to the values of
 *   start_left and end_right when this function is first entered.
 *
 * (1) If after execution completes both subranges have length >= 1, then it is
 *  guaranteed that:
 *  a) *start_left > *start_right
 *  b) *end_right  < *end_left
 *  c) *start_left < *end_right
 * (2) If after execution completes both subranges have length >= 2, then it is
 *  guaranteed that:
 *  a) *start_left > *(start_right + 1)
 *  b) *end_right  < *(end_left - 1)
 * (3) If after execution completes both subranges have length >= 3, then it is
 *  guaranteed that:
 *  a) *start_left > *(start_right + 2)
 *  b) *end_right  < *(end_left - 2)
 * (4) If after execution completes both subranges have length >= 4, (i.e.
 *  end_left + 1 - start_left >= 4 and end_left + 1 - start_right >= 4)
 *  then it is guaranteed that:
 *  a) *start_left > *(start_right + 3)
 *  b) *end_right  < *(end_left - 3)
 * OTHERWISE both subranges have length >= 2 (i.e. start_left < end_left &&
 *   start_right < end_right).
 */
template<class RandomAccessIterator1, class RandomAccessIterator2>
bool TrimEnds4(RandomAccessIterator1 &start_left_out,
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
      end_right = LargestIteratorWithValueLessThan_KnownToExist(start_right,
                                                     end_right - 1, *end_left);
    if (*end_right <= *start_left || start_left >= end_left
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
        std::iter_swap(start_left++, start_right);
      } while (*start_left <= *(start_right + 1)) ;//start_left <= end_left
                           //since *end_left > *end_right >= *(start_right + 1).
      //At this point, *start_left > *start_right since
      // *start_left > *(start_right + 1) >= *start_right.
      if (end_left <= start_left || *end_right <= *start_left) {
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
        std::iter_swap(end_right--, end_left);
      } while (*(end_left - 1) <= *end_right) ;
      if (end_right <= start_right || *end_right <= *start_left) {
        is_trivial = true;
        break;
      }
      //Note end_right > start_right + 1 since *end_right > *start_left
      //  > *(start_right + 1) and [start_right : end_right] is non-decreasing.
    }
    //At this point, *(end_left - 1) > *end_right

    if (*(end_left - 2) <= *(end_right - 1)) {
      do {
        std::iter_swap(end_left - 1, end_right - 1);
        std::iter_swap(*end_left, *end_right);
        end_right -= 2;
      } while (end_left - 2 <= end_right - 1) ;
      if (*(end_left - 1) <= *end_right) {
        std::iter_swap(end_left - 1, end_right); //Needed for the merge to be stable.
        std::iter_swap(end_left, end_right--);
      }
      if (*end_right <= *start_left) {
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
      if (*end_right <= *start_left) {
        is_trivial = true;
        break;
      }
    }

    if (*(start_left + 1) <= *(start_right + 2)) {
      //In this case, our vector is something like:
      // 2 2 2 2 2 3 4 4 9 ...1000 0 1 4 6... 500, which we can efficiently
      // handle.
      do {
        std::iter_swap(start_left + 1, start_right + 1);
        std::iter_swap(start_left, start_right);
        start_left += 2;
      } while (*(start_left + 1) <= *(start_right + 2));
      //At this point our example 2 2 2 2 2 3 4 4 9 ...1000 0 1 4 6... 500
      // has become: 4 9 ...1000 3 4 4 6... 500
      if (*start_left <= *(start_right + 1)) { //iff *start_left == *(start_right + 1)
        std::iter_swap(start_left, start_right + 1);  //Needed for the merge to be stable.
        std::iter_swap(start_left++, start_right);
      }
      if (*end_right <= *start_left) {
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
      if (*end_right <= *start_left) {
        is_trivial = true;
        break;
      }
    }

    if (*(end_left - 3) <= *end_right) {
      if (*(end_left - 3) <= *(end_right - 2)) {
        std::iter_swap(end_left - 2, end_right - 2);
        std::iter_swap(end_left - 1, end_right - 1);
        std::iter_swap(end_left, end_right);
        end_right = end_right - 3;
      }
      //At this point, *(end_left - 3) > *(end_right - 2).
      else if (*(end_left - 3) <= *(end_right - 1)) {
        //Rotate end_left - 2, end_left - 1, end_left, end_right - 1, end_right
        // to the right by 2.
        auto temp        = *end_left;
        *end_left        = *(end_left - 2);
        *(end_left - 2)  = *(end_right - 1);
        *(end_right - 1) = *(end_left - 1);
        *(end_left - 1)  = *end_right;
        *end_right       = temp;
        end_right        = end_right - 2;
      }
      //At this point, *(end_left - 3) > *(end_right - 1) and
      // *(end_left - 3) <= *end_right.
      else {
        //Rotate end_left - 2, end_left - 1, end_left, end_right
        // to the right by 1.
        auto temp       = *end_right;
        *end_right      = *end_left;
        *end_left       = *(end_left - 1);
        *(end_left - 1) = *(end_left - 2);
        *(end_left - 2) = temp;
        end_right       = end_right - 1;
      }
      if (start_right >= end_right || *end_right <= *start_left) {
        is_trivial = true;
        break;
      }
      continue ;
    }

    if (*start_left <= *(start_right + 3)) {
      if (*(start_left + 2) <= *(start_right + 3)) {
        std::iter_swap(start_left, start_right);
        std::iter_swap(start_left + 1, start_right + 1);
        std::iter_swap(start_left + 2, start_right + 2);
        start_left = start_left + 3;
      }
      //At this point *(start_right + 3) < *(start_left + 2).
      else if (*(start_left + 1) <= *(start_right + 3)) {
        //Rotate start_left, start_left + 1, start_right, start_right + 1,
        // start_right + 2 to the left by 2.
        auto temp          = *start_right;
        *start_right       = *(start_right + 2);
        *(start_right + 2) = *(start_left + 1);
        *(start_left + 1)  = *(start_right + 1);
        *(start_right + 1) = *start_left;
        *start_left        = temp;
        start_left         = start_left + 2;
      }
      //At this point *(start_right + 3) < *(start_left + 1)) and
      // *(start_right + 3) >= *start_left.
      else {
        //Rotate start_left, start_right, start_right + 1, start_right + 2
        // to the left by 1.
        auto temp          = *start_left;
        *start_left        = *start_right;
        *start_right       = *(start_right + 1);
        *(start_right + 1) = *(start_right + 2);
        *(start_right + 2) = temp;
        start_left       = start_left + 1;
      }
      if (start_left >= end_left || *end_right <= *start_left) {
        is_trivial = true;
        break;
      }
      continue ;
    }

    auto length_left  = std::distance(start_left, end_left + 1);
    auto length_right = std::distance(start_right, end_right + 1);
    if (length_left <= length_right && *(start_right + length_left - 1) <= *start_left) {
      std::swap_ranges(start_left, end_left + 1, start_right);
      start_left   = start_right;
      start_right += length_left;
      end_left    += length_left;
      continue ;
    }
    if (length_left >= length_right && *end_right <= *(end_left - (length_right - 1))) {
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
void MergeWithOutBufferTrim4(RandomAccessIterator1 start_left,
                             RandomAccessIterator1 end_left,
                             RandomAccessIterator2 start_right,
                             RandomAccessIterator2 end_right,
                             std::size_t length_left,
                             std::size_t length_right) {
  if (TrimEnds4(start_left, end_left, start_right, end_right)) {
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
  //At this point we're guaranteed to have start_left < start_right and
  // *start_left > *start_right.
  auto d = DisplacementFromMiddleIteratorToPotentialMedians_KnownToExist(
                                         end_left, start_right, length_smaller);
  {
    auto start_2nd_quarter = end_left;
    std::advance(start_2nd_quarter, - static_cast<long>(d - 1));
    auto one_past_end_2nd_quarter = end_left;
    ++one_past_end_2nd_quarter;
    std::swap_ranges(start_2nd_quarter, one_past_end_2nd_quarter, start_right);
    std::size_t length_first_quarter = length_left - d;
assert(static_cast<long long>(length_first_quarter) ==
                                  std::distance(start_left, start_2nd_quarter));
assert(d == std::distance(start_2nd_quarter, (end_left + 1)));
    MergeWithOutBufferTrim4<RandomAccessIterator1, RandomAccessIterator1>(start_left, start_2nd_quarter - 1,
        start_2nd_quarter, end_left, length_first_quarter, d);
  }

  auto start_4th_quarter = start_right;
  std::advance(start_4th_quarter, d);
  auto end_3rd_quarter = start_4th_quarter;
  --end_3rd_quarter;
  std::size_t length_4th_quarter = length_right - d;
assert(static_cast<long>(d) == std::distance(start_right, start_4th_quarter));
assert(static_cast<long>(length_4th_quarter) ==
                             std::distance(start_4th_quarter, (end_right + 1)));
  MergeWithOutBufferTrim4<RandomAccessIterator2, RandomAccessIterator2>(start_right, end_3rd_quarter,
      start_4th_quarter, end_right, d, length_4th_quarter);
  return ;
}

template<class RandomAccessIterator1, class RandomAccessIterator2>
inline void MergeWithOutBufferTrim4(RandomAccessIterator1 start_left,
                                    RandomAccessIterator1 end_left,
                                    RandomAccessIterator2 start_right,
                                    RandomAccessIterator2 end_right) {
  auto length_left  = std::distance(start_left, end_left + 1);
  auto length_right = std::distance(start_right, end_right + 1);
  MergeWithOutBufferTrim4<RandomAccessIterator1, RandomAccessIterator2>(start_left, end_left,
                             start_right, end_right, length_left, length_right);
  return ;
}


#endif /* SRC_MERGE_WITHOUT_BUFFER_TRIM4_H_ */
