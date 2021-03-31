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
template<typename RandomAccessIterator1, typename RandomAccessIterator2>
bool TrimEnds1_2iters_RAI(RandomAccessIterator1 &start_left_out,
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
      SwapRangesStable(start_left, end_left, start_right, end_right);
      //std::swap_ranges(start_left, end_left + 1, start_right);
      start_left   = start_right;
      start_right += length_left;
      end_left    += length_left;
      continue ;
    }
    if (length_left >= length_right && *end_right <=
                                            *(end_left - (length_right - 1))) {
      SwapRangesStable(end_left - (length_right-1), end_left, start_right, end_right);
      //std::swap_ranges(start_right, end_right + 1, end_left - (length_right-1));
      end_right   = end_left;
      end_left   -= length_right;
      start_right = end_left + 1;
      end_right   = start_right + (length_right - 1);
      assert(end_right == start_right + (length_right - 1));
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

template<typename BiDirectionalIterator1, typename BiDirectionalIterator2>
bool TrimEnds1_2iters_bi(BiDirectionalIterator1 &start_left_out,
                         BiDirectionalIterator1 &end_left_out,
                         BiDirectionalIterator2 &start_right_out,
                         BiDirectionalIterator2 &end_right_out) {
  auto start_left  = start_left_out,  end_left  = end_left_out;
  auto start_right = start_right_out, end_right = end_right_out;
  bool is_trivial = false;
  while (true) {
    if (*end_left <= *start_right || end_left < start_left
                                  || end_right < start_right) {
      return true;
    }
    //If true, then this implies that start_left < end_left
    if (*start_left <= *start_right) {
      auto start_left_plus1 = start_left;
      (void)++start_left_plus1;
      start_left = SmallestIteratorWithValueGreaterThan_KnownToExist(
                                      start_left_plus1, end_left, *start_right);
    }
    if (*end_left <= *end_right) {
      auto end_right_minus1 = end_right;
      (void)--end_right_minus1;
      end_right = LargestIteratorWithValueLessThan_KnownToExist(
                                      start_right, end_right_minus1, *end_left);
    }
    if (*end_right <= *start_left || end_left <= start_left
        || end_right <= start_right) {
      is_trivial = true;
      break;
    }
    //Note that at this point,
    // 1) both length_left and length_right are >= 2, and
    // 2) *end_left > *end_right > *start_left > *start_right.

    auto length_left  = std::distance(start_left, end_left) + 1;
    auto length_right = std::distance(start_right, end_right) + 1;
    if (length_left <= length_right) {
      auto symmetric_point_right = start_right;
      std::advance(symmetric_point_right, length_left - 1);
      if (*symmetric_point_right <= *start_left) {
        //auto end_left_plus1 = end_left;
        //(void)++end_left_plus1;
        //std::swap_ranges(start_left, end_left_plus1, start_right);
        SwapRangesStable(start_left, end_left, start_right, end_right);
        start_left   = start_right;
        end_left = symmetric_point_right;
        start_right = symmetric_point_right;
        (void)++start_right;
        continue ;
      }
    }
    if (length_left >= length_right) {
      auto symmetric_point_left = end_left;
      std::advance(symmetric_point_left, - (length_right - 1));
      if (*end_right <= *symmetric_point_left) {
        //auto end_right_plus1 = end_right;
        //(void)++end_right_plus1;
        //std::swap_ranges(start_right, end_right_plus1, symmetric_point_left);
        SwapRangesStable(symmetric_point_left, end_left, start_right, end_right);
        end_right = end_left;
        end_left = symmetric_point_left;
        start_right = symmetric_point_left;
        (void)--end_left;
        continue ;
      }
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
template<typename RandomAccessIterator1, typename RandomAccessIterator2>
void MergeWithOutBufferTrim1_recursive_RAI(RandomAccessIterator1 start_left,
                                           RandomAccessIterator1 end_left,
                                           RandomAccessIterator2 start_right,
                                           RandomAccessIterator2 end_right,
                                           std::size_t length_left,
                                           std::size_t length_right) {
  if (TrimEnds1_2iters_RAI(start_left, end_left, start_right, end_right)) {
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
    MergeWithOutBufferTrim1_recursive_RAI<RandomAccessIterator1,
                                          RandomAccessIterator1>(start_left,
          start_2nd_quarter - 1, start_2nd_quarter, end_left,
          length_first_quarter, d);
  }

  auto start_4th_quarter = start_right;
  std::advance(start_4th_quarter, d);
  auto end_3rd_quarter = start_4th_quarter;
  --end_3rd_quarter;
  std::size_t length_4th_quarter = length_right - d;
//assert(static_cast<long>(d) == std::distance(start_right, start_4th_quarter));
//assert(static_cast<long>(length_4th_quarter) ==
//                             std::distance(start_4th_quarter, (end_right + 1)));
  MergeWithOutBufferTrim1_recursive_RAI<RandomAccessIterator2,
                                        RandomAccessIterator2>(start_right,
          end_3rd_quarter, start_4th_quarter, end_right, d, length_4th_quarter);
  return ;
}

//Assumes that start_left <= start_right
template<typename BiDirectionalIterator1, typename BiDirectionalIterator2>
void MergeWithOutBufferTrim1_recursive_bi(BiDirectionalIterator1 start_left,
                                          BiDirectionalIterator1 end_left,
                                          BiDirectionalIterator2 start_right,
                                          BiDirectionalIterator2 end_right,
                                          std::size_t length_left,
                                          std::size_t length_right) {
  if (TrimEnds1_2iters_bi(start_left, end_left, start_right, end_right)) {
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
    MergeWithOutBufferTrim1_recursive_bi<BiDirectionalIterator1,
                                         BiDirectionalIterator1>(start_left,
          start_2nd_quarter - 1, start_2nd_quarter, end_left,
          length_first_quarter, d);
  }

  auto start_4th_quarter = start_right;
  std::advance(start_4th_quarter, d);
  auto end_3rd_quarter = start_4th_quarter;
  --end_3rd_quarter;
  std::size_t length_4th_quarter = length_right - d;
//assert(static_cast<long>(d) == std::distance(start_right, start_4th_quarter));
//assert(static_cast<long>(length_4th_quarter) ==
//                             std::distance(start_4th_quarter, (end_right + 1)));
  MergeWithOutBufferTrim1_recursive_bi<BiDirectionalIterator2,
                                       BiDirectionalIterator2>(start_right,
          end_3rd_quarter, start_4th_quarter, end_right, d, length_4th_quarter);
  return ;
}


template<typename RandomAccessIterator1, typename RandomAccessIterator2>
inline void MergeWithOutBufferTrim1(RandomAccessIterator1 start_left,
                                    RandomAccessIterator1 end_left,
                                    RandomAccessIterator2 start_right,
                                    RandomAccessIterator2 end_right) {
  auto length_left  = std::distance(start_left, end_left + 1);
  auto length_right = std::distance(start_right, end_right + 1);
  MergeWithOutBufferTrim1_recursive_RAI<RandomAccessIterator1,
                                        RandomAccessIterator2>(start_left,
                  end_left, start_right, end_right, length_left, length_right);
  return ;
}

//Assumes that:
// (1) length_left > 0 and length_right > 0, where
//      length_left  == std::distance(start_left,  start_right) and
//      length_right == std::distance(start_right, one_past_end)
// (2) start_right == end_left + 1
// (3) comp(*start_right, *end_left)
template<typename RandomAccessIterator1, typename RandomAccessIterator2, typename Distance,
         typename Compare, typename CompareLessOrEqual, typename ValueType>
inline void MergeWithOutBufferTrim1_RAI(RandomAccessIterator1 start_left,
                                        RandomAccessIterator1 end_left,
                                        RandomAccessIterator2 start_right,
                                        RandomAccessIterator2 one_past_end,
                                        Distance length_left,
                                        Distance length_right,
                                        Compare comp,
                                        CompareLessOrEqual comp_le) {
  MergeWithOutBufferTrim1_recursive_RAI<RandomAccessIterator1, RandomAccessIterator2, Distance, Compare,
                                        CompareLessOrEqual, ValueType>(
                               start_left, start_right, one_past_end,
                               length_left, length_right, comp, comp_le);
  return ;
}



template<typename BidirectionalIterator1, typename BidirectionalIterator2,
         typename Distance, typename Compare, typename CompareLessOrEqual,
         typename ValueType>
inline void MergeWithOutBufferTrim1_bi(BidirectionalIterator1 start_left,
                                       BidirectionalIterator1 end_left,
                                       BidirectionalIterator2 start_right,
                                       BidirectionalIterator2 one_past_end,
                                       Distance length_left,
                                       Distance length_right,
                                       Compare comp,
                                       CompareLessOrEqual comp_le) {
  MergeWithOutBufferTrim1_recursive_bi<BidirectionalIterator1, BidirectionalIterator2, Distance, Compare,
                                    CompareLessOrEqual, ValueType>(
                               start_left, start_right, one_past_end,
                               length_left, length_right, comp, comp_le);
  return ;
}


template<typename RandomAccessIterator1, typename RandomAccessIterator2,
         typename Distance, typename Compare, typename CompareLessOrEqual,
         typename ValueType>
inline void MergeWithOutBufferTrim1(RandomAccessIterator1 start_left,
                                    RandomAccessIterator1 end_left,
                                    RandomAccessIterator2 start_right,
                                    RandomAccessIterator2 one_past_end,
                                    Distance length_left,
                                    Distance length_right,
                                    Compare comp,
                                    CompareLessOrEqual comp_le,
                                    std::random_access_iterator_tag) {
  MergeWithOutBufferTrim1_RAI<RandomAccessIterator1, RandomAccessIterator2, Distance, Compare,
    CompareLessOrEqual, ValueType>(start_left, end_left, start_right,
        one_past_end, length_left, length_right, comp, comp_le);
  return ;
}



template<typename BidirectionalIterator1, typename BidirectionalIterator2,
         typename Distance, typename Compare, typename CompareLessOrEqual,
         typename ValueType>
inline void MergeWithOutBufferTrim1(BidirectionalIterator1 start_left,
                                    BidirectionalIterator1 end_left,
                                    BidirectionalIterator2 start_right,
                                    BidirectionalIterator2 one_past_end,
                                    Distance length_left,
                                    Distance length_right,
                                    Compare comp,
                                    CompareLessOrEqual comp_le,
                                    std::bidirectional_iterator_tag) {
  MergeWithOutBufferTrim1_bi<BidirectionalIterator1, BidirectionalIterator2, Distance, Compare,
     CompareLessOrEqual, ValueType>(start_left, end_left, start_right,
         one_past_end, length_left, length_right, comp, comp_le);
  return ;
}

//Dispatch function
template<typename Iterator1, typename Iterator2, typename Compare,
         typename Distance = typename Iterator1::difference_type>
inline void MergeWithOutBufferTrim1(Iterator1 start_left,
                                    Iterator1 end_left,
                                    Iterator2 start_right,
                                    Iterator2 one_past_end_right,
                                    Distance length_left,
                                    Distance length_right,
                                    Compare comp) {
  typedef typename Iterator1::value_type ValueType;
  if (length_left == 0 || length_right == 0)
    return ;
  auto comp_le = [comp](const ValueType &lhs, const ValueType &rhs) -> bool {
    return !comp(rhs, lhs);
  };
  typedef decltype(comp_le) CompareLessOrEqual;
  if (comp_le(*end_left, *start_right)) //i.e. if *end_left <= *start_right
    return ;
  MergeWithOutBufferTrim1<Iterator1, Iterator2, Distance, Compare, CompareLessOrEqual,
                ValueType>(start_left, end_left, start_right,
                one_past_end_right, length_left, length_right, comp, comp_le,
                typename std::iterator_traits<Iterator1>::iterator_category());
  return ;
}

template<typename Iterator1, typename Iterator2, typename Compare>
inline void MergeWithOutBufferTrim1(Iterator1 start_left,
                                    Iterator1 end_left,
                                    Iterator2 start_right,
                                    Iterator2 one_past_end_right,
                                    Compare comp) {
  if (start_left > end_left || start_right == one_past_end_right)
    return ;
  typedef typename Iterator1::difference_type Distance;
  Distance length_left  = std::distance(start_left, start_right);
  Distance length_right = std::distance(start_right, one_past_end_right);
  MergeWithOutBufferTrim1<Iterator1, Iterator2, Compare, Distance>(
                start_left, end_left, start_right,
                one_past_end_right, length_left, length_right, comp);
  return ;
}


template<typename Iterator, typename Compare,
         typename Distance = typename Iterator::difference_type>
inline void MergeWithOutBufferTrim1(Iterator start_left,
                                    Iterator start_right,
                                    Iterator one_past_end,
                                    Distance length_left,
                                    Distance length_right,
                                    Compare comp) {
  if (start_left == start_right || start_right == one_past_end)
    return ;
  Iterator end_left = start_right;
  (void)--end_left;
  MergeWithOutBufferTrim1<Iterator, Iterator, Compare, Distance>(
                        start_left, end_left, start_right, one_past_end, comp);
  return ;
}

template<typename Iterator, typename Compare>
inline void MergeWithOutBufferTrim1(Iterator start_left,
                                    Iterator start_right,
                                    Iterator one_past_end,
                                    Compare comp) {
  if (start_left == start_right)
    return ;
  Iterator end_left = start_right;
  (void)--end_left;
  MergeWithOutBufferTrim1<Iterator, Iterator, Compare>(start_left, end_left,
                                             start_right, one_past_end, comp);
  return ;
}

template<typename Iterator>
inline void MergeWithOutBufferTrim1(Iterator start_left,
                                    Iterator start_right,
                                    Iterator one_past_end) {
  typedef typename Iterator::value_type ValueType;
  auto comp = std::less<ValueType>();
  typedef decltype(comp) Compare;
  MergeWithOutBufferTrim1<Iterator, Compare>(start_left, start_right,
                                             one_past_end, comp);
  return ;
}

template<typename Iterator,
std::enable_if_t<std::is_fundamental<typename Iterator::value_type>::value, int> = 0>
inline void MergeWithOutBufferTrim1(Iterator start_left,
                                    Iterator start_right,
                                    Iterator one_past_end) {
  typedef typename Iterator::value_type ValueType;
  auto comp = std::less<ValueType>();
  typedef decltype(comp) Compare;
  MergeWithOutBufferTrim1<Iterator, Compare>(start_left, start_right,
                                             one_past_end, comp);
  return ;
}

template<typename Iterator, typename Compare, typename Distance>
struct MergeWOBuffTrim1 {
  inline void operator()(Iterator start_left,
      Iterator start_right,
      Iterator one_past_end_right,
      Distance length_left,
      Distance length_right,
      Compare comp) {
    MergeWithOutBufferTrim1<Iterator>(start_left, start_right,
                                      one_past_end_right, comp);
    return ;
  }
};

#endif /* SRC_MERGE_WITHOUT_BUFFER_TRIM1_H_ */
