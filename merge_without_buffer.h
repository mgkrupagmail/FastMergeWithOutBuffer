/*
 * merge_without_buffer.h
 *
 *  Created on: Jun 29, 2017
 *      Author: Matthew Gregory Krupa
 */

#ifndef SRC_MERGE_WITHOUT_BUFFER_H_
#define SRC_MERGE_WITHOUT_BUFFER_H_

#include <algorithm>

#include "merge_common.h"
#include "trim_ends.h"
#include "merge.h"

//Assumes that start_left <= start_right
template<class RAI>
void MergeWithOutBuffer(RAI start_left, RAI end_left, RAI start_right, RAI end_right) {
  int length_left, length_right, length_smaller, d;
  TrimEnds3(start_left, end_left, start_right, end_right);
  length_left  = std::distance(start_left, end_left + 1);
  length_right = std::distance(start_right, end_right + 1);
  length_smaller = length_left < length_right ? length_left : length_right;
  //Check for triviality.
  if (start_left > end_left || start_right > end_right || *end_left <= *start_right) {
    return ;
  }
  if (length_smaller <= 1) {
    if (length_smaller <= 0)
      return ;
    else if (length_left == 1 && length_right == 1) {
      if (*start_left > *start_right)
        std::iter_swap(start_left, start_right);
      return ;
    }
  }
  //At this point we're guaranteed to have start_left < start_right and
  // *start_left > *start_right.
  d = DisplacementFromMiddleIiteratorToPotentialMediansContiguous_KnownToExist(end_left, length_smaller);
  auto start_2nd_quarter = end_left - (d - 1);
  std::swap_ranges(start_2nd_quarter, end_left + 1, start_right);
  auto start_4th_quarter = start_right + d;
  MergeWithOutBuffer(start_left, start_2nd_quarter - 1, start_2nd_quarter, end_left);
  MergeWithOutBuffer(start_right, start_4th_quarter - 1, start_4th_quarter, end_right);
  return ;
}



//Assumes that start_left <= start_right
template<class RAI>
void MergeWithOutBufferTrim2(RAI start_left, RAI end_left, RAI start_right, RAI end_right) {
  int length_left, length_right, length_smaller, d;
  TrimEnds2(start_left, end_left, start_right, end_right);
  length_left  = std::distance(start_left, end_left + 1);
  length_right = std::distance(start_right, end_right + 1);
  length_smaller = length_left < length_right ? length_left : length_right;
  //Check for triviality.
  if (start_left > end_left || start_right > end_right || *end_left <= *start_right)
    return ;
  if (length_smaller <= 1) {
    if (length_smaller <= 0)
      return ;
    else if (length_left == 1 && length_right == 1) {
      if (*start_left > *start_right)
        std::iter_swap(start_left, start_right);
      return ;
    }
  }
  d = DisplacementFromMiddleIiteratorToPotentialMediansContiguous_KnownToExist(end_left, length_smaller);
  auto start_2nd_quarter = end_left - (d - 1);
  std::swap_ranges(start_2nd_quarter, end_left + 1, start_right);
  auto start_4th_quarter = start_right + d;
  MergeWithOutBufferTrim2(start_left, start_2nd_quarter - 1, start_2nd_quarter, end_left);
  MergeWithOutBufferTrim2(start_right, start_4th_quarter - 1, start_4th_quarter, end_right);
  return ;
}


//Assumes that start_left <= start_right
template<class RAI>
void MergeWithOutBufferTrim1(RAI start_left, RAI end_left, RAI start_right, RAI end_right) {
  int length_left, length_right, length_smaller, d;
  TrimEnds1(start_left, end_left, start_right, end_right);
  length_left  = std::distance(start_left, end_left + 1);
  length_right = std::distance(start_right, end_right + 1);
  length_smaller = length_left < length_right ? length_left : length_right;
  //Check for triviality.
  if (start_left > end_left || start_right > end_right || *end_left <= *start_right)
    return ;
  if (length_smaller <= 1) {
    if (length_smaller <= 0)
      return ;
    else if (length_left == 1 && length_right == 1) {
      if (*start_left > *start_right)
        std::iter_swap(start_left, start_right);
      return ;
    }
  }
  d = DisplacementFromMiddleIiteratorToPotentialMediansContiguous_KnownToExist(end_left, length_smaller);
  auto start_2nd_quarter = end_left - (d - 1);
  std::swap_ranges(start_2nd_quarter, end_left + 1, start_right);
  auto start_4th_quarter = start_right + d;
  MergeWithOutBufferTrim1(start_left, start_2nd_quarter - 1, start_2nd_quarter, end_left);
  MergeWithOutBufferTrim1(start_right, start_4th_quarter - 1, start_4th_quarter, end_right);
  return ;
}

#endif /* SRC_MERGE_WITHOUT_BUFFER_H_ */
