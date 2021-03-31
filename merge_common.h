/*
 * merge_common.h
 *
 *  Created on: Jun 29, 2017
 *      Author: Matthew Gregory Krupa
 *
 *  This file contains helper functions for the TrimEnds() functions.
 */

#ifndef SRC_MERGE_COMMON_H_
#define SRC_MERGE_COMMON_H_

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
template<class RAI, class RAI2>
int DisplacementFromMiddleIteratorToPotentialMedians_KnownToExist(
                        const RAI end_left,const RAI2 start_right, int length) {
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

/* Does the equivalent of:
 *     while(start_it < end_it && *start_it <= *end_it) start_it++;
 *  except that it finds the resulting start_it via a binary search.
 * In addition to performing a binary search, it simultaneously performs a
 *  linear search starting from the end_it.
 */
template<class RandomAccessIterator, class T>
RandomAccessIterator SmallestIteratorWithValueGreaterThan_KnownToExist(
    RandomAccessIterator start_it, RandomAccessIterator end_it, const T &value,
    std::random_access_iterator_tag) {
  while (true) {
    //The below lines are optional. They are performed at most
    // std::distance(start_it, d) + 1 times.
    if (*start_it > value)
      return start_it;
    (void)++start_it;

    //The below lines are optional. They are performed at most
    // std::distance(d, end_it) + 1 times.
    auto cur_length_minus_one = std::distance(start_it, end_it);
    if (*(start_it + cur_length_minus_one - 1) <= value)
      return end_it;
    (void)--end_it;

    auto d = start_it + std::distance(start_it, end_it) / 2;
    if (*d <= value)
      start_it = d + 1; //Note that start_it will be <= end_it since the
                        //desired iterator is known to exist.
    else
      end_it   = d;
  }
  //At this point, start_it == end_it OR start_it == end_it + 1.
  //return start_it + (vec[start_it] > value ? 0 : 1);
  //This return value should not execute.
  return start_it;
}

template<class BiDirectionalIterator, class T>
BiDirectionalIterator SmallestIteratorWithValueGreaterThan_KnownToExist(
    BiDirectionalIterator start_it, BiDirectionalIterator end_it, const T &value,
             std::bidirectional_iterator_tag) {
  while (true) {
    //The below lines are optional. They are performed at most
    // std::distance(start_it, d) + 1 times.
    if (*start_it > value)
      return start_it;
    (void)++start_it;

    //The below lines are optional. They are performed at most
    // std::distance(d, end_it) + 1 times.
    auto one_before_end_it = end_it;
    (void)--one_before_end_it;
    if (*one_before_end_it <= value)
      return end_it;
    (void)--end_it;

    auto d = start_it;
    std::advance(start_it, std::distance(start_it, end_it) / 2);
    if (*d <= value) {
      start_it = d; //Note that start_it will be <= end_it since the
                    //desired iterator is known to exist.
      (void)++d;
    }else
      end_it   = d;
  }
  //At this point, start_it == end_it OR start_it == end_it + 1.
  //return start_it + (vec[start_it] > value ? 0 : 1);
  //This return value should not execute.
  return start_it;
}

template<class Iterator, class T>
inline Iterator SmallestIteratorWithValueGreaterThan_KnownToExist(
                        Iterator start_it, Iterator end_it, const T &value) {
  return SmallestIteratorWithValueGreaterThan_KnownToExist(start_it, end_it,
      value, typename std::iterator_traits<Iterator>::iterator_category());
}

/* Does the equivalent of: while(*end_right >= *end_left) end_right--;
 * NOTE: This means that it assumes that such an index exists exists within
 *  [start_it : end_it] so IT DOES NO ITERATOR BOUNDS CHECKING.
 * Does the same thing as LargestIndexWithValueLessThan(), except that its
 *  inputs are different and also, in addition to performing a binary search,
 *  it simultaneously performs a linear search starting from the end_it.
 * Assumes that start_it <= end_it.
 * SE means that the inputs are start_index and end_index.
 * This algorithm find d by performing <= 3 * min {dist(start_it, d) + 1,
 *  dist(d, end_it) + 1, ceil(log_2(dist(start_it, end_it + 1)))} comparisons.
 */
template<class RandomAccessIterator, class T>
inline RandomAccessIterator LargestIteratorWithValueLessThan_KnownToExist(
                                    RandomAccessIterator start_it,
                                    RandomAccessIterator end_it, const T &value,
                                    std::random_access_iterator_tag) {
  while (true) {//Use:while(start_it < end_it){ if the d is not known to exist.
    //The below three lines are optional. They are performed at most
    // std::distance(d, end_it) + 1 times.
    if (*end_it < value) {
      return end_it;
    }
    (void)--end_it;

    //The below three lines are optional. They are performed at most
    // std::distance(start_it, d) + 1 times.
    if (*(start_it + 1) >= value)
      return start_it;
    (void)++start_it;

    auto d = start_it + std::distance(start_it, end_it) / 2;
    if (*d < value)
      start_it = d;
    else
      end_it   = d - 1;
  }
  //This return value should not execute.
  return end_it;
}

template<class BiDirectionalIterator, class T>
inline BiDirectionalIterator LargestIteratorWithValueLessThan_KnownToExist(
                                 BiDirectionalIterator start_it,
                                 BiDirectionalIterator end_it, const T &value,
                                 std::bidirectional_iterator_tag) {
  while (true) {//Use:while(start_it < end_it){ if the d is not known to exist.
    //The below three lines are optional. They are performed at most
    // std::distance(d, end_it) + 1 times.
    if (*end_it < value) {
      return end_it;
    }
    (void)--end_it;

    //The below three lines are optional. They are performed at most
    // std::distance(start_it, d) + 1 times.
    auto start_it_plus1 = start_it;
    ++start_it_plus1;
    if (*start_it_plus1 >= value)
      return start_it;
    start_it = start_it_plus1;

    auto d = start_it;
    std::advance(d, std::distance(start_it, end_it) / 2);
    if (*d < value)
      start_it = d;
    else {
      end_it   = d;
      (void)--end_it;
    }
  }
  //This return value should not execute.
  return end_it;
}

template<class Iterator, class T>
inline Iterator LargestIteratorWithValueLessThan_KnownToExist(
                        Iterator start_it, Iterator end_it, const T &value) {
  return LargestIteratorWithValueLessThan_KnownToExist(start_it, end_it,
      value, typename std::iterator_traits<Iterator>::iterator_category());
}

template<class BiDirectionalIterator>
inline void SwapRangesStable(BiDirectionalIterator start_left,
                             BiDirectionalIterator end_left,
                             BiDirectionalIterator start_right,
                             BiDirectionalIterator end_right) {
  bool not_equal = *end_right < *end_left;
  auto one_past_end = end_right;
  (void)++one_past_end;
  std::swap_ranges(start_left, start_right, one_past_end);
  if (not_equal)
    return ;
  const auto &value = *start_right;
  BiDirectionalIterator first_equal;
  if (*start_left < value) {
    auto end = end_left;
    (void)--end;
    first_equal = LargestIteratorWithValueLessThan_KnownToExist(start_left,
                                                                end, value);
  } else {
    first_equal = start_left;
  }
  BiDirectionalIterator one_past_last_equal;
  if (value < *end_right) {
    auto start = start_right;
    (void)++start;
    one_past_last_equal = SmallestIteratorWithValueGreaterThan_KnownToExist(
                                                       start, end_right, value);
  } else {
    one_past_last_equal = one_past_end;
  }
  std::rotate(first_equal, start_right, one_past_last_equal);
  return ;
}




/*
 * Assumes that [start_right + 1, end_right] is non-decreasing.
 * It is possible that *start_right > *(start_right + 1).
 * This function will rearrange values until [start_right, end_right] is
 *  non-decreasing.
 */
template<class RAI>
inline void MergeTrivialCasesLeftLength1(RAI start_right, RAI end_right) {
  if (start_right == end_right)
    return ;
  auto start_right_plus1 = start_right;
  ++start_right_plus1;
  const auto &start_right_value = *start_right;
  if (start_right_value <= *start_right_plus1)
    return ;
  for (auto it = start_right_plus1; it != end_right
                                   && *(it + 1) < start_right_value; (void)it++)
    std::iter_swap(it, it + 1);
  return ;
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
template<class RAI, class RAI2>
inline void MergeTrivialCasesLeftLength1(RAI start_right, RAI end_right,
                                         RAI2 ele_to_shift) {
  const auto &value = *ele_to_shift;
  if (value <= *start_right)
    return ;
  else
    std::iter_swap(start_right, ele_to_shift);
  for (auto it = start_right; it != end_right && *(it + 1) < value; (void)it++)
    std::iter_swap(it, it + 1);
  return ;
}


/*
 * Assumes that [start_left, end_left-1] is non-decreasing.
 * It is possible that *end_left < *(end_left - 1).
 * This function will rearrange values until [start_left, end_left] is
 *  non-decreasing.
 */
template<class RAI>
inline void MergeTrivialCasesRightLength1(RAI start_left, RAI end_left) {
  if (start_left == end_left)
    return ;
  auto end_left_minus1 = end_left;
  --end_left_minus1;
  const auto &end_left_value = *end_left;
  if (*end_left_minus1 <= end_left_value)
    return ;
  for (auto it = end_left_minus1; it != start_left && end_left_value < *it; (void)it--)
    std::iter_swap(it, it + 1);
  return ;
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


template<class RAI, class RAI2>
inline void MergeTrivialCasesRightLength1(RAI start_left, RAI end_left,
                                           RAI2 ele_to_shift) {
  const auto &value = *ele_to_shift;
  if (*end_left <= value)
    return ;
  else
    std::iter_swap(end_left, ele_to_shift);
  for (auto it = end_left - 1; it != start_left && value < *it; (void)it--)
    std::iter_swap(it, it + 1);
  return ;
}

/*
 * Assumes that *start_right < *start_left_out < *end_right
 * Assumes that *start_left_out  < *end_right  < *end_left
 *  - This implies that *end_left > *start_right_out
 *    and that [start_left_out, end_left] contains at least 2 elements.
 *
 * This function will swap *start_left_out with *start_right and then
 *  rearrange elements so that [start_right, end_right] is
 *  non-decreasing.
 * After this function finishes, length_left_out will have been decreased
 *  by at least 1, call this difference D, and start_left_out would have
 *  been advanced forwards D places.
 */
template<class BidirectionalIterator, class BidirectionalIterator2>
inline void MergeTrivialCasesHelperEmplaceStartLeftAndStartRight(
         BidirectionalIterator &start_left_out, BidirectionalIterator end_left,
         BidirectionalIterator2 start_right, BidirectionalIterator2 end_right,
         long &length_left_out) {
  auto start_left  = start_left_out;
  auto length_left = length_left_out;
assert(*start_right     < *start_left_out);
assert(*start_left_out  < *end_right);
assert(*end_right       < *end_left);

  std::swap(*start_left, *start_right);
  ++start_left;
  --length_left;
  MergeTrivialCasesLeftLength1<BidirectionalIterator2>(start_right, end_right);
  const auto &value_start_right = *start_right;
  if (*start_left <= value_start_right) {
    auto one_past_end_left = end_left;
    ++one_past_end_left;
    do {
      ++start_left;
      --length_left;
assert(start_left != one_past_end_left); assert(length_left > 0);
    } while (*start_left <= value_start_right);
    //Formerly: } while (start_left != one_past_end_left &&
    //                  *start_left <= value_start_right);
assert(start_left != one_past_end_left); assert(length_left > 0);
  }
  start_left_out = start_left;
assert(length_left < length_left_out);
  length_left_out = length_left;
  return ;
}

/*
 * Assumes that *start_right < *start_left < *end_right_out
 * Assumes that *start_left  < *end_right_out  < *end_left
 *  - This implies that *end_left > *start_right_out
 *    and that [start_left, end_left] contains at least 2 elements.
 *
 * This function will swap *end_left *end_right_out and then
 *  rearrange elements so that [start_right, end_right_out] is
 *  non-decreasing.
 * After this function finishes, length_right_out will have been decreased
 *  by at least 1, call this difference D, and end_right_out would have
 *  been advanced backwards (i.e. left) D places.
 */
template<class BidirectionalIterator, class BidirectionalIterator2>
inline void MergeTrivialCasesHelperEmplaceEndLeftAndEndRight(
     BidirectionalIterator start_left, BidirectionalIterator  end_left,
     BidirectionalIterator2 start_right, BidirectionalIterator2 &end_right_out,
     long &length_right_out) {
  auto end_right = end_right_out;
  auto length_right = length_right_out;
assert(*start_right   < *start_left);
assert(*start_left    < *end_right_out);
assert(*end_right_out < *end_left);

  std::swap(*end_left, *end_right);
  --end_right;
  --length_right;
  MergeTrivialCasesRightLength1<BidirectionalIterator>(start_left, end_left);
  const auto &value_end_left = *end_left;
  if (value_end_left <= *end_right) {
    do {
assert(end_right != start_right); assert(length_right > 0);
      --end_right;
      --length_right;
    } while (value_end_left <= *end_right);
    //Formerly: } while (length_right > 0 && value_end_left <= *end_right);
assert(end_right != start_right); assert(length_right > 0);
  }
  end_right_out = end_right;
assert(length_right < length_right_out);
  length_right_out = length_right;
  return ;
}

/*
 * Assumes that *start_right < *start_left < *end_right_out
 * Assumes that *start_left  < *end_right_out  < *end_left
 *  - This implies that *end_left > *start_right_out
 *    and that [start_left, end_left] contains at least 2 elements.
 *
 * This function should only be used when length_left_out is small
 *  (e.g. <= 5)
 */
template<class BidirectionalIterator, class BidirectionalIterator2>
inline void MergeTrivialCasesHelperEmplaceInsertionMergeLeftIntoRight(
    BidirectionalIterator &start_left_out, BidirectionalIterator end_left,
    BidirectionalIterator2 start_right, BidirectionalIterator2 end_right,
    long &length_left_out) {
  while (length_left_out > 1) {
    MergeTrivialCasesHelperEmplaceStartLeftAndStartRight<BidirectionalIterator,
           BidirectionalIterator2>(start_left_out, end_left,
                                   start_right, end_right, length_left_out);
  }
assert(length_left_out == 1);//?????????
  if (length_left_out == 1) {
assert(start_left_out == end_right);
    MergeTrivialCasesLeftLength1<BidirectionalIterator, BidirectionalIterator2>(
        start_right, end_right, start_left_out);
    --length_left_out;
  }
assert(length_left_out == 0);
  return ;
}


/*
 * Assumes that *start_right < *start_left < *end_right_out
 * Assumes that *start_left  < *end_right_out  < *end_left
 *  - This implies that *end_left > *start_right_out
 *    and that [start_left, end_left] contains at least 2 elements.
 *
 * This function should only be used when length_right_out is small
 *  (e.g. <= 5)
 */
template<class BidirectionalIterator, class BidirectionalIterator2>
inline void MergeTrivialCasesHelperEmplaceInsertionMergeRightIntoLeft(
    BidirectionalIterator start_left, BidirectionalIterator end_left,
    BidirectionalIterator2 start_right, BidirectionalIterator2 &end_right_out,
    long &length_right_out) {
  while (length_right_out > 1) {
    MergeTrivialCasesHelperEmplaceEndLeftAndEndRight<BidirectionalIterator,
           BidirectionalIterator2>(start_left, end_left,
                                   start_right, end_right_out, length_right_out);
  }
assert(length_right_out == 1);//?????????
  if (length_right_out == 1) {
assert(start_right == end_right_out);
    MergeTrivialCasesRightLength1<BidirectionalIterator, BidirectionalIterator2>(
        start_left, end_left, start_right);
    --length_right_out;
  }
assert(length_right_out == 0);
  return ;
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
  auto end_left_plus1  = end_left;
  ++end_left_plus1;
  auto end_right_plus1 = end_right;
  ++end_right_plus1;
  if (end_left_plus1 == start_right) {
    std::rotate(start_left, start_right, end_right_plus1);
    return ;
  }
  if (end_left_plus1 == start_right) {
    std::rotate(start_left, start_right, end_right_plus1);
  } else {
    auto length_left  = std::distance(start_left, end_left_plus1);
    auto length_right = std::distance(start_right, end_right_plus1);
    if (length_right < length_left) {
      auto new_start_right = end_left_plus1;
      std::advance(new_start_right, -length_right);
      std::swap_ranges(start_right, end_right_plus1, new_start_right);
      std::rotate(start_left, new_start_right, end_left_plus1);
    } else { //Else length_left <= length_right
      std::swap_ranges(start_left, end_left_plus1, start_right);
      if (length_left == length_right)
        return ;
      auto new_start_right = start_right;
      std::advance(new_start_right, length_left);
      std::rotate(start_right, new_start_right, end_right_plus1);
    }
  }
  return ;
}

/* Given two ranges [start_left, end_left] and [start_right, end_right], this
 *  function moves all elements in such a way that in the range
 *  [start_left, ... , end_left, start_right, ..., end_right] all elements
 *  will be shifted right by std::distance(start_right, end_right + 1),
 *  resulting in the range:
 *  [start_right, ... , end_right, start_left, ..., end_left].
 * This is a helper function for MergeTrivialCases().
 */
template<class RAI, class RAI2>
inline void ShiftRightSideToTheRightByItsLength(RAI start_left, RAI end_left,
                                             RAI2 start_right, RAI2 end_right) {
  auto end_left_plus1  = end_left;
  ++end_left_plus1;
  auto end_right_plus1 = end_right;
  ++end_right_plus1;
  auto length_left  = std::distance(start_left, end_left_plus1);
  auto length_right = std::distance(start_right, end_right_plus1);
  if (length_right < length_left) {
    auto new_start_right = end_left_plus1;
    std::advance(new_start_right, -length_right);
    std::swap_ranges(start_right, end_right_plus1, new_start_right);
    std::rotate(start_left, new_start_right, end_left_plus1);
  } else { //Else length_left <= length_right
    std::swap_ranges(start_left, end_left_plus1, start_right);
    if (length_left == length_right)
      return ;
    auto new_start_right = start_right;
    std::advance(new_start_right, length_left);
    std::rotate(start_right, new_start_right, end_right_plus1);
  }
  return ;
}


constexpr long MergeTrivialCases_small_length = 5;
/*
 * If one of length_left or length_right is small (e.g. <= 5) then this
 *  function will do an "insertion merge" of the smaller sequence
 *  into the larger sequence.
 */
template<class BidirectionalIterator, class BidirectionalIterator2>
inline void MergeTrivialCasesWhenOneLengthIsSmall(
    BidirectionalIterator &start_left_out, BidirectionalIterator end_left,
    BidirectionalIterator2 start_right, BidirectionalIterator2 &end_right_out,
                                      long &length_left, long &length_right) {
assert(length_left <= MergeTrivialCases_small_length
      || length_right <= MergeTrivialCases_small_length);
  if (length_left < length_right) {
    MergeTrivialCasesHelperEmplaceInsertionMergeLeftIntoRight<
                                 BidirectionalIterator, BidirectionalIterator2>(
        start_left_out, end_left, start_right, end_right_out, length_left);
assert(length_left == 0);
  } else {
    MergeTrivialCasesHelperEmplaceInsertionMergeRightIntoLeft<
                                 BidirectionalIterator, BidirectionalIterator2>(
        start_left_out, end_left, start_right, end_right_out, length_right);
assert(length_right == 0);
  }
  return ;
}

/* This is a helper function that merges two ranges when the merge is trivial,
 *  by which it is meant that length_left <= 1 or length_right <= 1.
 */
template<class BidirectionalIterator, class BidirectionalIterator2>
void MergeTrivialCases(
          BidirectionalIterator start_left,  BidirectionalIterator end_left,
          BidirectionalIterator2 start_right, BidirectionalIterator2 end_right,
          long length_left, long length_right) {
  if (length_left <= 0 || length_right <= 0 || *end_left <= *start_right)
    return ;
  else if (*end_right <= *start_left) {
    ///Note that this function has the same effect as
    // std::rotate(vec.begin() + start_left, vec.begin() + start_right,
    //             vec.begin() + (end_right + 1));
    // except that it works for ranges iterated by distinct objects.
    ShiftRightSideToTheRightByItsLength(start_left, end_left, start_right,
                                        end_right);
  }  else if (end_left == start_left) {
    MergeTrivialCasesLeftLength1<BidirectionalIterator,
                                      BidirectionalIterator2>(start_right,
                                                           end_right, end_left);
  } else if (start_right == end_right) {
    MergeTrivialCasesRightLength1<BidirectionalIterator,
                                      BidirectionalIterator2>(start_left,
                                                         end_left, start_right);
  } else {
assert(length_left <= MergeTrivialCases_small_length
      || length_right <= MergeTrivialCases_small_length);
    MergeTrivialCasesWhenOneLengthIsSmall<BidirectionalIterator,
               BidirectionalIterator2>(start_left, end_left, start_right,
                                       end_right, length_left, length_right);
  }
  return ;
}

/* Overload of MergeTrivialCases().
 */
template<class BidirectionalIterator, class BidirectionalIterator2>
inline void MergeTrivialCases(BidirectionalIterator  start_left,
    BidirectionalIterator end_left,
    BidirectionalIterator2 start_right, BidirectionalIterator2 end_right) {
  auto length_left  = std::distance(start_left,  end_left + 1);
  auto length_right = std::distance(start_right, end_right + 1);
  MergeTrivialCases<BidirectionalIterator, BidirectionalIterator2>(start_left,
                    end_left, start_right, end_right,
                    length_left, length_right);
  return ;
}

#endif /* SRC_MERGE_COMMON_H_ */
