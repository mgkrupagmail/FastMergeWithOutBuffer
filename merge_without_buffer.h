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

#ifndef MERGE_WITHOUT_BUFFER_H_
#define MERGE_WITHOUT_BUFFER_H_

#include <algorithm>

#include "merge_without_buffer_trim5.h"

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
inline void MergeWithOutBuffer(RandomAccessIterator1 start_left,
                               RandomAccessIterator1 end_left,
                               RandomAccessIterator2 start_right,
                               RandomAccessIterator2 end_right) {
  auto length_left  = std::distance(start_left, end_left + 1);
  auto length_right = std::distance(start_right, end_right + 1);
  MergeWithOutBufferTrim5<RandomAccessIterator1, RandomAccessIterator2>(
                             start_left, end_left,
                             start_right, end_right, length_left, length_right);
  return ;
}

template<class RandomAccessIterator>
inline void MergeWithOutBuffer(RandomAccessIterator start_left,
                               RandomAccessIterator start_right,
                               RandomAccessIterator one_past_end) {
  auto length_left  = std::distance(start_left, start_right);
  auto length_right = std::distance(start_right, one_past_end);
  MergeWithOutBufferTrim5<RandomAccessIterator, RandomAccessIterator>(
                             start_left, start_right - 1,
                             start_right, one_past_end - 1,
                             length_left, length_right);
  return ;
}


#endif /* SRC_MERGE_WITHOUT_BUFFER_H_ */
