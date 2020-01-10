/*
 * merge_without_buffer.h
 *
 *  Created on: Jun 29, 2017
 *      Author: Matthew Gregory Krupa
 *
 *  The most important function defined in this file is MergeWithOutBuffer().
 *  MergeWithOutBufferTrim1() contains the near minimum code needed to make the
 *   merge algorithm work.
 *  MergeWithOutBuffer() is the fastest of MergeWithOutBuffer(),
 *   MergeWithOutBufferTrim4(), MergeWithOutBufferTrim3(),
 *   MergeWithOutBufferTrim2(), and MergeWithOutBufferTrim1().
 */


#ifndef SRC_MERGE_WITHOUT_BUFFER_TRIM5_H_
#define SRC_MERGE_WITHOUT_BUFFER_TRIM5_H_


#include <algorithm>

/* Given two sorted ranges of values that are contiguous in memory as
 *  [start_left : end_right] this function will try function will try to
 *  increase start_left and decrease end_right as much as possible using only
 *  simple comparisons and switches near the ends of these two subintervals.
 * Assumes that both [start_left : end_left] and [start_right : end_right] are
 *  sorted in increasing order, and that
 *  start_left <= end_left == start_right - 1 < start_right <= end_right
 *  (IMPORTANT: Note the "end_left == start_right - 1")
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
 * (4) If after execution completes both subranges have length >= 4, then it is
 *  guaranteed that:
 *  a) *start_left > *(start_right + 3)
 *  b) *end_right  < *(end_left - 3)
 * (5) If after execution completes both subranges have length >= 5, then it is
 *  guaranteed that:
 *  a) *start_left > *(start_right + 4)
 *  b) *end_right  < *(end_left - 4)
 * If after execution completes, start_right > end_right or start_left >end_left
 *  then the two subranges have been completely merged;
 *  OTHERWISE both subranges have length >= 2 (i.e. start_left < end_left &&
 *   start_right < end_right).
 */
template<class RandomAccessIterator1, class RandomAccessIterator2>
bool TrimEnds5(RandomAccessIterator1 &start_left_out,
               RandomAccessIterator1 &end_left_out,
               RandomAccessIterator2 &start_right_out,
               RandomAccessIterator2 &end_right_out) {
  typedef decltype(*start_left_out) ValueType;
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
      start_left = SmallestIteratorWithValueGreaterThan_KnownToExist<RandomAccessIterator1,
                            ValueType>(start_left + 1, end_left, *start_right);
    if (*end_left <= *end_right)
      end_right = LargestIteratorWithValueLessThan_KnownToExist<RandomAccessIterator2,ValueType>(
                                         start_right, end_right - 1, *end_left);
    if (*end_right <= *start_left || end_left <= start_left
        || end_right <= start_right) {
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
      //At this point, *(start_right + 1) < *start_left and
      // [start_left : end_left] has at least 2 elements.
    }

    //*(end_left - 1) is well-defined since start_left < end_left.
    if (*(end_left - 1) <= *end_right) {
      do {//Note end_right >= start_right + 1 since *end_left >= *(end_left -1)
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
    //At this point, *(end_left - 1) > *end_right.
    //Note that since also *end_right > *start_left, [start_left, end_left] must
    // contain at least 3 elements. In particular, end_left - 2 is defined.

    if (*(end_left - 2) <= *(end_right - 1)) {
      do {
        std::iter_swap(end_left - 1, end_right - 1);
        std::iter_swap(end_left, end_right);
        end_right -= 2;
      } while (*(end_left - 2) <= *(end_right - 1)) ;
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
      auto temp       = std::move(*end_left);
      *end_left       = std::move(*(end_left - 1));
      *(end_left - 1) = std::move(*end_right);
      *end_right      = std::move(temp);
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
      auto temp          = std::move(*start_left);
      *start_left        = std::move(*start_right);
      *start_right       = std::move(*(start_right + 1));
      *(start_right + 1) = std::move(temp);
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
        auto temp        = std::move(*end_left);
        *end_left        = std::move(*(end_left - 2));
        *(end_left - 2)  = std::move(*(end_right - 1));
        *(end_right - 1) = std::move(*(end_left - 1));
        *(end_left - 1)  = std::move(*end_right);
        *end_right       = std::move(temp);
        end_right        = end_right - 2;
      }
      //At this point, *(end_left - 3) > *(end_right - 1) and
      // *(end_left - 3) <= *end_right.
      else {
        //Rotate end_left - 2, end_left - 1, end_left, end_right
        // to the right by 1.
        auto temp       = std::move(*end_right);
        *end_right      = std::move(*end_left);
        *end_left       = std::move(*(end_left - 1));
        *(end_left - 1) = std::move(*(end_left - 2));
        *(end_left - 2) = std::move(temp);
        end_right       = end_right - 1;
      }
      if (end_right <= start_right || *end_right <= *start_left) {
        is_trivial = true;
        break;
      }
      continue ;
    }

    //At this point, start_right + 3 <= end_right and start_left + 2 <= end_left
    if (*start_left <= *(start_right + 3)) {
      if (*(start_left + 2) <= *(start_right + 3)) {
        std::iter_swap(start_left, start_right);
        std::iter_swap(start_left + 1, start_right + 1);
        std::iter_swap(start_left + 2, start_right + 2);
        std::advance(start_left, 3);
      }
      //At this point *(start_right + 3) < *(start_left + 2).
      else if (*(start_left + 1) <= *(start_right + 3)) {
        //Rotate start_left, start_left + 1, start_right, start_right + 1,
        // start_right + 2 to the left by 2.
        auto temp          = std::move(*start_right);
        *start_right       = std::move(*(start_right + 2));
        *(start_right + 2) = std::move(*(start_left + 1));
        *(start_left + 1)  = std::move(*(start_right + 1));
        *(start_right + 1) = std::move(*start_left);
        *start_left        = std::move(temp);
        std::advance(start_left, 2);
      }
      //At this point *(start_right + 3) < *(start_left + 1)) and
      // *start_left <= *(start_right + 3).
      else {
        //Rotate start_left, start_right, start_right + 1, start_right + 2
        // to the left by 1.
        auto temp          = std::move(*start_left);
        *start_left        = std::move(*start_right);
        *start_right       = std::move(*(start_right + 1));
        *(start_right + 1) = std::move(*(start_right + 2));
        *(start_right + 2) = std::move(temp);
        std::advance(start_left, 1);
      }
      if (end_left <= start_left || *end_right <= *start_left) {
        is_trivial = true;
        break;
      }
      continue ;
    }

    //At this point, end_left - 4 >= start_left and start_right <= end_right - 3
    if (*(end_left - 4) <= *end_right) {
      if (start_right <= end_right - 3 && *(end_left - 4) <= *(end_right - 3)) {
        std::iter_swap(end_left - 3, end_right - 3);
        std::iter_swap(end_left - 2, end_right - 2);
        std::iter_swap(end_left - 1, end_right - 1);
        std::iter_swap(end_left, end_right);
        end_right = end_right - 4;
      }
      //At this point, *(end_left - 4) > *(end_right - 3).
      else if (*(end_left - 4) <= *(end_right - 2)) {
        //Rotate end_left - 3, end_left - 2, end_left - 1, end_left,
        // end_right - 1, end_right to the right by 3.
        auto temp        = std::move(*end_right);
        *end_right       = std::move(*end_left);
        *end_left        = std::move(*(end_left - 3));
        *(end_left - 3)  = std::move(*(end_right - 2));
        *(end_right - 2) = std::move(*(end_left - 2));
        *(end_left - 2)  = std::move(*(end_right - 1));
        *(end_right - 1) = std::move(*(end_left - 1));
        *(end_left - 1)  = std::move(temp);
        end_right        = end_right - 3;
      }
      //At this point, *(end_left - 4) > *(end_right - 2).
      else if (*(end_left - 4) <= *(end_right - 1)) {
        //Rotate end_left - 3, end_left - 2, end_left - 1, end_left,
        // end_right - 1, end_right to the right by 2.
        auto temp        = std::move(*end_left);
        *end_left        = std::move(*(end_left - 2));
        *(end_left - 2)  = std::move(*end_right);
        *end_right       = std::move(temp);
        auto temp2       = std::move(*(end_left - 1));
        *(end_left - 1)  = std::move(*(end_left - 3));
        *(end_left - 3)  = std::move(*(end_right - 1));
        *(end_right - 1) = std::move(temp2);
        end_right        = end_right - 2;

      }
      //At this point, *(end_left - 4) > *(end_right - 1) and
      // *(end_left - 4) <= *end_right.
      else {
        //Rotate end_left - 3, end_left - 2, end_left - 1, end_left, end_right
        // to the right by 1.
        auto temp       = std::move(*end_right);
        *end_right      = std::move(*end_left);
        *end_left       = std::move(*(end_left - 1));
        *(end_left - 1) = std::move(*(end_left - 2));
        *(end_left - 2) = std::move(*(end_left - 3));
        *(end_left - 3) = std::move(temp);
        end_right       = end_right - 1;
      }
      if (end_right <= start_right || *end_right <= *start_left) {
        is_trivial = true;
        break;
      }
      continue ;
    }

    //At this point, start_right + 4 <= end_right and start_left + 3 <= end_left
    if (*start_left <= *(start_right + 4)) {
      if (*(start_left + 3) <= *(start_right + 4)) {
        std::iter_swap(start_left, start_right);
        std::iter_swap(start_left + 1, start_right + 1);
        std::iter_swap(start_left + 2, start_right + 2);
        std::iter_swap(start_left + 3, start_right + 3);
        std::advance(start_left, 4);
      }
      //At this point *(start_right + 4) < *(start_left + 3).
      else if (*(start_left + 2) <= *(start_right + 4)) {
        //Rotate start_left, start_left + 1, start_left + 2, start_right,
        // start_right + 1, start_right + 2, start_right + 3 to the left by 3.
        auto temp          = std::move(*start_right);
        *start_right       = std::move(*(start_right + 3));
        *(start_right + 3) = std::move(*(start_left + 2));
        *(start_left + 2)  = std::move(*(start_right + 2));
        *(start_right + 2) = std::move(*(start_left + 1));
        *(start_left + 1)  = std::move(*(start_right + 1));
        *(start_right + 1) = std::move(* start_left);
        *start_left        = std::move(temp);
        std::advance(start_left, 3);
      }
      //At this point *(start_right + 4) < *(start_left + 2).
      else if (*(start_left + 1) <= *(start_right + 4)) {
        //Rotate start_left, start_left + 1, start_right, start_right + 1,
        // start_right + 2, start_right + 3 to the left by 2.
        auto temp          = std::move(*start_right);
        *start_right       = std::move(*(start_right + 2));
        *(start_right + 2) = std::move(*start_left);
        *start_left        = std::move(temp);
        auto temp2         = std::move(*(start_right + 1));
        *(start_right + 1) = std::move(*(start_right + 3));
        *(start_right + 3) = std::move(*(start_left + 1));
        *(start_left + 1)  = std::move(temp2);
        std::advance(start_left, 2);
      }
      //At this point *(start_right + 3) < *(start_left + 1)) and
      // *start_left <= *(start_right + 4).
      else {
        //Rotate start_left, start_right, start_right + 1, start_right + 2,
        // start_right + 3 to the left by 1.
        auto temp          = std::move(*start_left);
        *start_left        = std::move(*start_right);
        *start_right       = std::move(*(start_right + 1));
        *(start_right + 1) = std::move(*(start_right + 2));
        *(start_right + 2) = std::move(*(start_right + 3));
        *(start_right + 3) = std::move(temp);
        ++start_left;
      }
      if (end_left <= start_left || *end_right <= *start_left) {
        is_trivial = true;
        break;
      }
      continue ;
    }

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
                                            *(end_left - (length_right - 1)) ) {
      std::swap_ranges(start_right, end_right + 1, end_left - (length_right-1));
      end_left   -= length_right;
      start_right = end_left + 1;
      end_right   = start_right + (length_right - 1);
      continue ;
    }
    break ;
  }

  if (is_trivial) {
    MergeTrivialCases<RandomAccessIterator1, RandomAccessIterator2>(start_left, end_left, start_right, end_right);
    return true;
  }
  start_left_out   = start_left;
  end_left_out     = end_left;
  start_right_out  = start_right;
  end_right_out    = end_right;
  return false;
}

/*Assumes that start_left <= start_right and start_right <= end_right and that
 * the values of the intervals of iterators [start_left, end_left] and
 * [start_right, end_right] are non-decreasing.
 *
 * The reason why this particular function is distinguished with the name
 *  MergeWithOutBuffer() rather than MergeWithOutBufferTrim5(), which would have
 *  continued the patter of names MergeWithOutBufferTrim1(), ...,
 *  MergeWithOutBufferTrim4() is explained in the comment at the top of
 *  merge_time.h.
 */
template<class RandomAccessIterator1, class RandomAccessIterator2>
void MergeWithOutBufferTrim5(RandomAccessIterator1 start_left,
                             RandomAccessIterator1 end_left,
                             RandomAccessIterator2 start_right,
                             RandomAccessIterator2 end_right,
                             std::size_t length_left,
                             std::size_t length_right) {
  if (TrimEnds5(start_left, end_left, start_right, end_right)) {
    return ;
  }
  length_left  = std::distance(start_left, end_left) + 1;
  length_right = std::distance(start_right, end_right) + 1;
  auto length_smaller = length_left < length_right ? length_left : length_right;
  //Check for triviality.
  if (length_smaller <= 5) {
    MergeTrivialCases<RandomAccessIterator1, RandomAccessIterator2>(start_left, end_left, start_right, end_right,
                                 length_left, length_right);
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
    MergeWithOutBufferTrim5<RandomAccessIterator1, RandomAccessIterator1>(start_left, start_2nd_quarter - 1,
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
  MergeWithOutBufferTrim5<RandomAccessIterator2, RandomAccessIterator2>(start_right, end_3rd_quarter,
      start_4th_quarter, end_right, d, length_4th_quarter);
  return ;
}

template<class RandomAccessIterator1, class RandomAccessIterator2>
inline void MergeWithOutBufferTrim5(RandomAccessIterator1 start_left,
                                    RandomAccessIterator1 end_left,
                                    RandomAccessIterator2 start_right,
                                    RandomAccessIterator2 end_right) {
  auto length_left  = std::distance(start_left, end_left + 1);
  auto length_right = std::distance(start_right, end_right + 1);
  MergeWithOutBufferTrim5<RandomAccessIterator1, RandomAccessIterator2>(start_left, end_left,
                             start_right, end_right, length_left, length_right);
  return ;
}

#endif /* SRC_MERGE_WITHOUT_BUFFER_TRIM5_H_ */
