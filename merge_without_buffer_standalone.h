/*
 * merge_without_buffer_standalone.h
 *
 *  Created on: Dec 1, 2019
 *      Author: Matthew Gregory Krupa
 *      Copyright: Matthew Gregory Krupa
 *
 *  The main function defined in this file is
 *
 *   template<class RAI, class RAI2>
 *   void MergeWithOutBuffer(RAI  start1, RAI  end1,
 *                           RAI2 start2, RAI2 end2)
 *
 * which is defined at the very end of this file.
 * It is assumed that RAI and RAI2 are both Random Access Iterators.
 * Note that RAI and RAI2 need not be the same type but they must
 *  each access objects of the same type (i.e. *start1 and *start2
 *  must be objects of the same type).
 *
 * Assumes that start1 <= end1 and start2 <= end2 and that
 *  the values of the intervals of iterators [start1, end1] and
 *  [start2, end2] are each non-decreasing.
 *
 * This function will rearrange all elements so that
 *  *start1 <= *(start1 + 1) <= ... <= *end1 <= *start2 <= *(start2 + 1) <= ... <= *end2
 * (Note that start1 == end1 is allowed so that *(start1 + 1) need not
 *   actually be defined. Similarily start2 == end2 is allowed.)
 *
 * This function uses O(1) additional memory and has worst case O(N log N)
 *  time complexity, where N is the total number of elements in both sequences.
 * This algorithm is both inplace and stable.
 */

#ifndef SRC_MERGE_WITHOUT_BUFFER_STANDALONE_H_
#define SRC_MERGE_WITHOUT_BUFFER_STANDALONE_H_

#include <algorithm>

namespace MergeWithoutBufferNamespace {

/* Finds the SMALLEST integer  0 <= d < length IF it exists such that
 *  (*) *(end_left - d) <= *(start_right + d), and
 *  otherwise, if such a d does NOT exist then it returns length - 1.
 * Assumes that [start_left : start_index + length - 1] is non-decreasing,
 *  that all these elements exist, and that BOTH subvectors have size >= length
 *  and that length > 0, where start_left = end_left - (length - 1).
 * WARNING: POTENTIAL FALSE POSITIVE: There are two ways that this function may
 *  return length - 1:
 *  1) Such a d does NOT exist. i.e. *start_index > *end_right.
 *  2) Such a d exists and happens to equal length - 1. i.e.
 *     *start_left <= *end_right and *(start_left + 1) > *(end_right - 1]
 * NOTES:
 *  (1) If *end_left > *(end_left + 1) and such a d exists then d is
 *       necessarily > 0.
 */
template<class RAI>
int DisplacementFromMiddleIteratorToPotentialMedians_KnownToExist(
                         const RAI end_left,const RAI start_right, int length) {
  (void)length--;  //We will now use length_left as if it were d_upper.
  int d_lower = 0; //So that end_left - d_lower = end_left
  do {
    auto d = d_lower + ((length - d_lower)/2);
    if (*(end_left - d) <= *(start_right + d)) {
      length       = d;
    } else {
      d_lower       = d + 1;
    }
  } while (d_lower < length) ;
  return d_lower;
}

/* Assumes that [start, end] is non-decreasing, that the iterator
 *  ele_to_shift points to an element not in the interval, and that the
 *  singleton interval [ele_to_shift, ele_to_shift] is imagined to
 *  "lie to the LEFT" of [start, end]. i.e. that we have the following
 *  situation: ... [ele_to_shift] ... [start, end] ....
 *
 * If it exists then it will find the smallest d > 0 such that
 *  *(start + d) >= *ele_to_shift and then shifts/rotates all elements in
 *  [start, start + d) left-wards (i.e. towards end_left) by 1 and swaps end
 *  with ele_to_shift resulting in [ele_to_shift, start, ..., end] being
 *  non-decreasing.
 * If such a d does not exist then it does nothing.
 * This is a helper function for MergeTrivialCases().
 */
template<class RAI>
inline void RotateLeftByExactlyOneElement(RAI start, RAI end, RAI ele_to_shift) {
  const auto value = *ele_to_shift;
  if (*start >= value)
    return ;
  else
    std::iter_swap(start, ele_to_shift);
  for (auto it = start; it < end && *(it + 1) < value; (void)it++)
    std::iter_swap(it, it + 1);
}

/* Assumes that [start, end] is non-decreasing, that the iterator
 *  ele_to_shift points to an element not in the interval, and that the
 *  singleton interval [ele_to_shift, ele_to_shift] is imagined to
 *  "lie to the RIGHT" of [start, end]. i.e. that we have the following
 *  situation: ... [start, end] ... [ele_to_shift] ....
 *
 * If it exists then it will find the smallest d > 0 such that
 *  *(end - d) <= *ele_to_shift and then shifts/rotates all elements in
 *  (end - d, end] right-wards (i.e. towards end) by 1 and swaps end with
 *  ele_to_shift resulting in [start, ..., end, ele_to_shift] being
 *  non-decreasing.
 * If such a d does not exist then it does nothing.
 * This is a helper function for MergeTrivialCases().
 */
template<class RAI>
inline void RotateRightByExactlyOneElement(RAI start, RAI end, RAI ele_to_shift) {
  const auto value = *ele_to_shift;
  if (*end <= value)
    return ;
  else
    std::iter_swap(end, ele_to_shift);
  for (auto it = end - 1; it > start && *it > value; (void)it--)
    std::iter_swap(it, it + 1);
}

/* Given two ranges [start_left, end_left] and [start_right, end_right], this
 *  function moves all elements in such a way that in the range
 *  [start_left, ... , end_left, start_right, ..., end_right] all elements
 *  will be shifted right by std::distance(start_right, end_right + 1),
 *  resulting in the range:
 *  [start_right, ... , end_right, start_left, ..., end_left].
 * This is a helper function for MergeTrivialCases().
 */
template<class RAI>
inline void ShiftRightSideToTheRightByItsLength(RAI start_left, RAI end_left,
                                         RAI start_right, RAI end_right) {
  auto end_left_plus1  = end_left + 1;
  auto end_right_plus1 = end_right + 1;
  auto length_left  = std::distance(start_left, end_left_plus1);
  auto length_right = std::distance(start_right, end_right_plus1);

  while (length_left > 0 && length_right > 0) {
    RAI start_swap1, end_swap1, start_swap2;
    if (length_left < length_right) {
      start_swap1   = start_left;
      end_swap1     = end_left_plus1;
      start_swap2   = end_right_plus1 - length_left;
      length_right -= length_left;
      end_right    -= length_left;
      end_right_plus1 = end_right + 1;
    } else {
      start_swap1  = start_right;
      end_swap1    = end_right_plus1;
      start_swap2  = start_left;
      length_left -= length_right;
      start_left  += length_right;
    }
    std::swap_ranges(start_swap1, end_swap1, start_swap2);
  }
  return ;
}

/* This is a helper function that merges two ranges when the merge is trivial,
 *  by which it is meant that length_left <= 1 or length_right <= 1.
 */
template<class T>
void MergeTrivialCases(T start_left,  T end_left, T start_right, T end_right,
                       long length_left, long length_right) {
  if (length_left <= 0 || length_right <= 0 || *end_left <= *start_right)
    return ;
  else if (*start_left >= *end_right) {
    ///Note that this function has the same effect as
    // std::rotate(vec.begin() + start_left, vec.begin() + start_right,
    //             vec.begin() + (end_right + 1));
    // except that it works for ranges iterated by distinct objects.
    MergeWithoutBufferNamespace::ShiftRightSideToTheRightByItsLength(start_left, end_left, start_right,
                                                            end_right);
  } else if (end_left == start_left) {
    MergeWithoutBufferNamespace::RotateLeftByExactlyOneElement(start_right, end_right, end_left);
  } else {// if (start_right == end_right) {
    MergeWithoutBufferNamespace::RotateRightByExactlyOneElement(start_left, end_left, start_right);
  }
  return ;
}

/* Overload of MergeTrivialCases().
 */
template<class T>
inline void MergeTrivialCases(T start_left,  T end_left, T start_right, T end_right) {
  auto length_left  = std::distance(start_left,  end_left + 1);
  auto length_right = std::distance(start_right, end_right + 1);
  MergeWithoutBufferNamespace::MergeTrivialCases(start_left, end_left, start_right, end_right, length_left,
                                        length_right);
  return ;
}

/* Does the equivalent of: while(*end_right >= *end_left) end_right--;
 * NOTE: This means that it assumes that such an index exists exists within
 *  [start_it : end_it] so IT DOES NO ITERATOR BOUNDS CHECKING.
 * Does the same thing as LargestIndexWithValueLessThan(), except that its
 *  inputs are different and also, in addition to performing a binary search,
 *  it simultaneously performs a linear search starting from the end_it.
 * Assumes that start_it <= end_it.
 * This algorithm find d by performing <= 3 * min {dist(start_it, d) + 1,
 *  dist(d, end_it) + 1, ceil(log_2(dist(start_it, end_it + 1)))} comparisons.
 */
template<class RAI, class T>
inline RAI LargestIteratorWithValueLessThan_KnownToExist(RAI start_it,
                                                   RAI end_it, const T value) {
  while (true) {//Use: while(start_it < end_it){ if the d is not known to exist.
    //The below three lines are optional. They are performed at most
    // std::distance(d, end_it) + 1 times.
    if (*end_it < value) {
      return end_it;
    }
    (void)end_it--;

    //The below three lines are optional. They are performed at most
    // std::distance(start_it, d) + 1 times.
    if (*(start_it + 1) >= value) {
      return start_it;
    }
    (void)start_it++;

    auto d = start_it + std::distance(start_it, end_it) / 2;
    if (*d < value)
      start_it = d;
    else
      end_it   = d - 1;
  }
}

/* Does the equivalent of:
 *     while(start_it < end_it && *start_it <= *end_it) start_it++;
 *  except that it finds the resulting start_it via a binary search.
 * In addition to performing a binary search, it simultaneously performs a
 *  linear search starting from the end_it.
 */
template<class ForwardIterator, class T>
ForwardIterator SmallestIteratorWithValueGreaterThan_KnownToExist(
             ForwardIterator start_it, ForwardIterator end_it, const T value) {
  while (true) {
    //The below lines are optional. They are performed at most
    // std::distance(start_it, d) + 1 times.
    if (*start_it > value)
      return start_it;
    (void)start_it++;

    //The below lines are optional. They are performed at most
    // std::distance(d, end_it) + 1 times.
    auto cur_length_minus_one = std::distance(start_it, end_it);
    if (*(start_it + cur_length_minus_one - 1) <= value)
      return end_it;
    (void)end_it--;

    auto d = start_it + std::distance(start_it, end_it) / 2;
    if (*d <= value)
      start_it = d + 1; //Note that start_it will be <= end_it since the
                        //desired iterator is known to exist.
    else
      end_it   = d;
  }
  //At this point, start_it == end_it OR start_it == end_it + 1.
  //return start_it + (vec[start_it] > value ? 0 : 1);
}

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
 *  set start_left_out = end_left_out + 1; start_right_out = end_right_out + 1;
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
template<class RAI, class RAI2>
void TrimEnds5(RAI &start_left_out,   RAI &end_left_out,
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
      start_left = MergeWithoutBufferNamespace::SmallestIteratorWithValueGreaterThan_KnownToExist(
                                       start_left + 1, end_left, *start_right);
    if (*end_right >= *end_left)
      end_right = MergeWithoutBufferNamespace::LargestIteratorWithValueLessThan_KnownToExist(start_right,
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
      if (start_right >= end_right || *start_left >= *end_right) {
        is_trivial = true;
        break;
      }
      continue ;
    }

    //At this point, start_right + 3 <= end_right and start_left + 2 <= end_left
    if (*(start_right + 3) >= *start_left) {
      if (*(start_right + 3) >= *(start_left + 2)) {
        std::iter_swap(start_left, start_right);
        std::iter_swap(start_left + 1, start_right + 1);
        std::iter_swap(start_left + 2, start_right + 2);
        start_left = start_left + 3;
      }
      //At this point *(start_right + 3) < *(start_left + 2).
      else if (*(start_right + 3) >= *(start_left + 1)) {
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
      if (start_left >= end_left || *start_left >= *end_right) {
        is_trivial = true;
        break;
      }
      continue ;
    }

    //At this point, end_left - 4 >= start_left and end_right - 3 >= start_right
    if (*(end_left - 4) <= *end_right) {
      if (end_right - 3 >= start_right && *(end_left - 4) <= *(end_right - 3)) {
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
        auto temp        = *end_right;
        *end_right       = *end_left;
        *end_left        = *(end_left - 3);
        *(end_left - 3)  = *(end_right - 2);
        *(end_right - 2) = *(end_left - 2);
        *(end_left - 2)  = *(end_right - 1);
        *(end_right - 1) = *(end_left - 1);
        *(end_left - 1)  = temp;
        end_right        = end_right - 3;
      }
      //At this point, *(end_left - 4) > *(end_right - 2).
      else if (*(end_left - 4) <= *(end_right - 1)) {
        //Rotate end_left - 3, end_left - 2, end_left - 1, end_left,
        // end_right - 1, end_right to the right by 2.
        auto temp        = *end_left;
        *end_left        = *(end_left - 2);
        *(end_left - 2)  = *end_right;
        *end_right       = temp;
        auto temp2       = *(end_left - 1);
        *(end_left - 1)  = *(end_left - 3);
        *(end_left - 3)  = *(end_right - 1);
        *(end_right - 1) = temp2;
        end_right        = end_right - 2;

      }
      //At this point, *(end_left - 4) > *(end_right - 1) and
      // *(end_left - 4) <= *end_right.
      else {
        //Rotate end_left - 3, end_left - 2, end_left - 1, end_left, end_right
        // to the right by 1.
        auto temp       = *end_right;
        *end_right      = *end_left;
        *end_left       = *(end_left - 1);
        *(end_left - 1) = *(end_left - 2);
        *(end_left - 2) = *(end_left - 3);
        *(end_left - 3) = temp;
        end_right       = end_right - 1;
      }
      if (start_right >= end_right || *start_left >= *end_right) {
        is_trivial = true;
        break;
      }
      continue ;
    }

    //At this point, start_right + 4 <= end_right and start_left + 3 <= end_left
    if (*(start_right + 4) >= *start_left) {
      if (*(start_right + 4) >= *(start_left + 3)) {
        std::iter_swap(start_left, start_right);
        std::iter_swap(start_left + 1, start_right + 1);
        std::iter_swap(start_left + 2, start_right + 2);
        std::iter_swap(start_left + 3, start_right + 3);
        start_left = start_left + 4;
      }
      //At this point *(start_right + 4) < *(start_left + 3).
      else if (*(start_right + 4) >= *(start_left + 2)) {
        //Rotate start_left, start_left + 1, start_left + 2, start_right,
        // start_right + 1, start_right + 2, start_right + 3 to the left by 3.
        auto temp          = *start_right;
        *start_right       = *(start_right + 3);
        *(start_right + 3) = *(start_left + 2);
        *(start_left + 2)  = *(start_right + 2);
        *(start_right + 2) = *(start_left + 1);
        *(start_left + 1)  = *(start_right + 1);
        *(start_right + 1) = * start_left;
        *start_left        = temp;
        start_left         = start_left + 3;
      }
      //At this point *(start_right + 4) < *(start_left + 2).
      else if (*(start_right + 4) >= *(start_left + 1)) {
        //Rotate start_left, start_left + 1, start_right, start_right + 1,
        // start_right + 2, start_right + 3 to the left by 2.
        auto temp          = *start_right;
        *start_right       = *(start_right + 2);
        *(start_right + 2) = *start_left;
        *start_left        = temp;
        auto temp2         = *(start_right + 1);
        *(start_right + 1) = *(start_right + 3);
        *(start_right + 3) = *(start_left + 1);
        *(start_left + 1)  = temp2;
        start_left         = start_left + 2;
      }
      //At this point *(start_right + 3) < *(start_left + 1)) and
      // *(start_right + 4) >= *start_left.
      else {
        //Rotate start_left, start_right, start_right + 1, start_right + 2,
        // start_right + 3 to the left by 1.
        auto temp          = *start_left;
        *start_left        = *start_right;
        *start_right       = *(start_right + 1);
        *(start_right + 1) = *(start_right + 2);
        *(start_right + 2) = *(start_right + 3);
        *(start_right + 3) = temp;
        start_left       = start_left + 1;
      }
      if (start_left >= end_left || *start_left >= *end_right) {
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
    MergeWithoutBufferNamespace::MergeTrivialCases(start_left, end_left, start_right, end_right);
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
template<class RAI, class RAI2>
void MergeWithOutBuffer(RAI start_left,   RAI end_left,
                        RAI2 start_right, RAI2 end_right) {
  int length_left, length_right, length_smaller, d;
  MergeWithoutBufferNamespace::TrimEnds5(start_left, end_left, start_right, end_right);
  length_left  = std::distance(start_left, end_left + 1);
  length_right = std::distance(start_right, end_right + 1);
  length_smaller = length_left < length_right ? length_left : length_right;
  //Check for triviality.
  if (length_smaller <= 1) {
    MergeWithoutBufferNamespace::MergeTrivialCases(start_left, end_left, start_right, end_right, length_left,
                                          length_right);
    return ;
  }
  //At this point we're guaranteed to have start_left < start_right and
  // *start_left > *start_right.
  d = MergeWithoutBufferNamespace::DisplacementFromMiddleIteratorToPotentialMedians_KnownToExist(end_left,
                                                   start_right, length_smaller);
  auto start_2nd_quarter = end_left - (d - 1);
  std::swap_ranges(start_2nd_quarter, end_left + 1, start_right);
  auto start_4th_quarter = start_right + d;
  MergeWithoutBufferNamespace::MergeWithOutBuffer(start_left, start_2nd_quarter - 1, start_2nd_quarter,
                                         end_left);
  MergeWithoutBufferNamespace::MergeWithOutBuffer(start_right, start_4th_quarter - 1, start_4th_quarter,
                                         end_right);
  return ;
}

} //End namespace MergeWithoutBufferNamespace

/* Assumes that RAI and RAI2 are both Random Access Iterators.
 * Note that RAI and RAI2 need not be the same type but they must
 *  both access to objects of the same type (i.e. *start1 and *start2
 *  must be objects of the same type).
 *
 * Assumes that the two sequences of elements:
 * *start1, *(start1 + 1), ..., *end1
 * *start2, *(start2 + 1), ..., *end2
 * are each individually sorted and that the first sequence
 * is meant to appear to the LEFT of the second sequence.
 *
 * This function will rearrange all elements so that
 *  *start1 <= *(start1 + 1) <= ... <= *end1 <= *start2 <= *(start2 + 1) <= ... <= *end2
 * (Note that start1 == end1 is allowed so that *(start1 + 1) need not
 *   actually be defined. Similarily start2 == end2 is allowed.)
 *
 * This function uses O(1) additional memory and has worst case
 *   O(N log N) complexity, where N is the number of elements in both sequences.
 * This algorithm is both inplace and stable.
 */
template<class RAI, class RAI2>
inline void MergeWithOutBuffer(RAI  start1, RAI  end1,
                               RAI2 start2, RAI2 end2) {
  MergeWithoutBufferNamespace::MergeWithOutBuffer(start1, end1, start2, end2);
}

#endif /* SRC_MERGE_WITHOUT_BUFFER_STANDALONE_H_ */
