/*
 * merge_without_buffer2.h
 *
 *  Created on: Jan 2, 2020
 *      Author: Matthew Gregory Krupa
 *   Copyright: Metthew Gregory Krupa
 */

#ifndef SRC_MERGE_WITHOUT_BUFFER2_H_
#define SRC_MERGE_WITHOUT_BUFFER2_H_

#include <algorithm>
#include <functional>
//functional is included to allow for the use of std::less<>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <type_traits>

#include "merge_without_buffer_common.h"

namespace merge_without_buffer_2_namespace {

//Assumes that:
// (1) length_left > 0 and length_right > 0, where
//      length_left  == std::distance(start_left,  start_right) and
//      length_right == std::distance(start_right, one_past_end)
// (2) comp(*start_right, *end_left), where end_left == start_right - 1
// (3) If is_left_trimmed == true  then comp(*start_right, *start_left)
// (4) If is_left_trimmed == false then comp(*end_right,   *end_left)
//
// Returns true if and only if the two non-decreasing sequences have been
//  merged, in which case the values of start_left_out, start_right_out, and
//  one_past_end_out would not have been changed.

//If this function returns false, then after execution finishes:
// (1) start_left_out (resp. start_right_out, one_past_end_out) would have
//     been replaced by the local variable start_left (resp. start_right,
//     one_past_end).
// (2) length_left_out > 1 and length_right_out > 1, and
//      length_left_out  == std::distance(start_left,  start_right) and
//      length_right_out == std::distance(start_right, one_past_end)
// (3) comp(*start_right, *end_left), where end_left == start_right - 1
// (4) comp(*start_right, *start_left)
// (5) comp(*end_left, *end_right)
// (6) comp(*start_left, *end_right)
// (7) comp(*(start_right + 1), *start_left), and
// (8) comp(*end_right, *(end_left - 1))

enum ThreeValue {
  Unknown = -1,
  False,
  True
};

//In what follows, all assert()s would evaluate to true if they were to be
// called.
template<typename RandomAccessIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline bool Trim2_switch_RAI(RandomAccessIterator &start_left_out,
                             RandomAccessIterator &start_right_out,
                             RandomAccessIterator &one_past_end_out,
                             Distance &length_left_out,
                             Distance &length_right_out,
                             Compare comp,
                             CompareLessOrEqual comp_le,
                             ThreeValue is_startleft_less_or_equal_to_startright,
                             ThreeValue is_startleft_less_or_equal_to_startright_plus1,
                             ThreeValue is_endleft_less_or_equal_to_endright,
                             ThreeValue is_endleft_minus1_less_or_equal_to_endright) {
  auto start_left   = start_left_out;
  auto start_right  = start_right_out;
  auto one_past_end = one_past_end_out;
  auto end_left     = start_right - 1;
  auto end_right    = one_past_end - 1;
  auto length_left  = length_left_out;
  auto length_right = length_right_out;
  //Meaning of the following variables (using
  // is_endleft_less_or_equal_to_endright as an example):
  // (< 0)  if (is_endleft_less_or_equal_to_endright < 0) then it is unknown
  //        whether or not comp_le(*end_left, *end_right) is true.
  // (== 0) if (is_endleft_less_or_equal_to_endright == 0) then
  //        comp_le(*end_left, *end_right) is false.
  // (> 0)  if (is_endleft_less_or_equal_to_endright > 0) then
  //        comp_le(*end_left, *end_right) is true.
  //The following are true:
  //if (is_startleft_less_or_equal_to_startright >= 0) then
  //    is_startleft_less_or_equal_to_startright ==
  //                                          comp_le(*start_left, *start_right)
  //if (is_startleft_less_or_equal_to_startright_plus1 >= 0) then
  //    length_right > 1 && is_startleft_less_or_equal_to_startright_plus1 ==
  //                                    comp_le(*start_left, *(start_right + 1))
  //if (is_endleft_less_or_equal_to_endright >= 0) then
  //    is_endleft_less_or_equal_to_endright == comp_le(*end_left, *end_right)
  //if (is_endleft_minus1_less_or_equal_to_endright >= 0) then
  //    length_left > 1 && is_endleft_minus1_less_or_equal_to_endright ==
  //                                       comp_le(*(end_left - 1), *end_right))
  if (is_startleft_less_or_equal_to_startright < ThreeValue::False)
    is_startleft_less_or_equal_to_startright =
                    static_cast<ThreeValue>(comp_le(*start_left, *start_right));
  if (is_startleft_less_or_equal_to_startright < ThreeValue::False)
    is_endleft_less_or_equal_to_endright =
                        static_cast<ThreeValue>(comp_le(*end_left, *end_right));
  enum SwitchLabels {
    startleft_le_startright = 0, //"le" stands for "less than or equal to"
    endleft_le_endright,
    startleft_le_startright_plus1,
    endleft_minus1_le_endright,
    length_dispatch,
    length_left_less_than_length_right,
    length_right_less_than_length_left,
    length_left_equal_to_length_right,
    trivial_case_length_left_equals_1,
    trivial_case_length_right_equals_1,
    trivial_case_endright_le_startleft
  };
  SwitchLabels jump_to = static_cast<SwitchLabels>(0);
  while (true) {
    //The following assert()s are true at this point:
    //assert(length_left > 0 && length_right > 0);
    //assert(comp(*start_right, *end_left));
    switch (jump_to) {
      case SwitchLabels::startleft_le_startright: {
        if (is_startleft_less_or_equal_to_startright != 0) {
          //The following are true at this point:
          // assert(is_endleft_less_or_equal_to_endright == 0);
          // if (is_startleft_less_or_equal_to_startright > 0)
          //     assert(comp_le(*start_left, *start_right));
          if (is_startleft_less_or_equal_to_startright > 0 ||
              comp_le(*start_left, *start_right)) {
            // assert(length_left > 1);
            mwob_namespace::TrimLeft1_RAI<RandomAccessIterator, Compare,
                                      Distance, CompareLessOrEqual, ValueType>(
                start_left, end_left, *start_right, length_left, comp, comp_le);
            if (length_left <= 1) { //assert(false);
              jump_to = SwitchLabels::trivial_case_length_left_equals_1;
              continue ;
            }
          }
          is_startleft_less_or_equal_to_startright = ThreeValue::False;
        }
        if (length_right <= 1) {
          jump_to = SwitchLabels::trivial_case_length_right_equals_1;
          continue ;
        }
        [[fallthrough]];
      }

      case SwitchLabels::endleft_le_endright: {
        if (is_endleft_less_or_equal_to_endright != ThreeValue::False) {
          //if (is_endleft_less_or_equal_to_endright > 0)
          //    assert(comp_le(*end_left, *end_right));
          //assert(is_startleft_less_or_equal_to_startright == ThreeValue::False);
          if (is_endleft_less_or_equal_to_endright > 0 ||
              comp_le(*end_left, *end_right)) {
            // assert(length_right > 1);
            mwob_namespace::TrimRight1_RAI<RandomAccessIterator, Compare,
                                      Distance, CompareLessOrEqual, ValueType>(
                 *end_left, start_right, end_right, one_past_end, length_right,
                 comp, comp_le);
            if (length_right <= 1) {
              jump_to = SwitchLabels::trivial_case_length_right_equals_1;
              continue ;
            }
          }
          is_endleft_less_or_equal_to_endright = ThreeValue::False;
        } else {  }
        if (length_left <= 1) {
          jump_to = SwitchLabels::trivial_case_length_left_equals_1;
          continue ;
        }
        [[fallthrough]];
      }

      case SwitchLabels::startleft_le_startright_plus1: {
        //assert(length_left > 1 && length_right > 1);
        //assert(is_startleft_less_or_equal_to_startright == ThreeValue::False);
        //assert(is_endleft_less_or_equal_to_endright == 0);
        if (is_startleft_less_or_equal_to_startright_plus1 != 0) {
          // if (is_startleft_less_or_equal_to_startright_plus1 > 0)
          //     assert(comp_le(*start_left, *start_right_plus1));
          auto start_right_plus1 = start_right + 1;
          if (is_startleft_less_or_equal_to_startright_plus1 > 0 ||
              comp_le(*start_left, *start_right_plus1)) {
            do {
              std::iter_swap(start_left++, start_right);
            } while(comp_le(*start_left, *start_right_plus1)) ;
            length_left = std::distance(start_left, start_right);
            if (length_left <= 1) {
              jump_to = SwitchLabels::trivial_case_length_left_equals_1;
              continue ;
            }
            // assert(comp(*start_right_plus1, *start_left));
          }
          is_startleft_less_or_equal_to_startright_plus1 = ThreeValue::False;
        }
        [[fallthrough]];
      }

      case SwitchLabels::endleft_minus1_le_endright: {
        // assert(length_left > 1 && length_right > 1);
        if (is_endleft_minus1_less_or_equal_to_endright != 0) {
          auto end_left_minus1 = end_left - 1;
          // if (is_endleft_minus1_less_or_equal_to_endright > 0)
          //     assert(comp_le(*end_left_minus1, *end_right));
          if (is_endleft_minus1_less_or_equal_to_endright > 0 ||
              comp_le(*end_left_minus1, *end_right)) {
            do {
              std::iter_swap(end_left, end_right--);
            } while(comp_le(*end_left_minus1, *end_right)) ;
            length_right = std::distance(start_right, end_right) + 1;
            one_past_end = end_right + 1;
            if (length_right <= 1) { //assert(false);
              jump_to = SwitchLabels::trivial_case_length_right_equals_1;
              continue ;
            }
          }
          is_endleft_minus1_less_or_equal_to_endright = ThreeValue::False;
        }
        [[fallthrough]];
      }

      case SwitchLabels::length_dispatch: {
        //assert(length_left > 1 && length_right > 1);
        //assert(length_left  == std::distance(start_left,  end_left)  + 1);
        //assert(length_right == std::distance(start_right, end_right) + 1);
        //assert(one_past_end == end_right + 1 && start_right == end_left + 1);
        //assert(comp(*start_right, *start_left));
        //assert(comp(*(start_right + 1), *start_left));
        //assert(comp(*end_right, *(end_left - 1)));
        //assert(std::is_sorted(start_left, start_right, comp));
        //assert(std::is_sorted(start_right, one_past_end, comp));
        //assert(is_startleft_less_or_equal_to_startright_plus1 ==
        //                            comp_le(*start_left, *(start_right + 1)));
        //assert(is_endleft_minus1_less_or_equal_to_endright ==
        //                                comp_le(*(end_left - 1), *end_right));
        if (length_left >= length_right) {
          jump_to = static_cast<SwitchLabels>(
                    SwitchLabels::length_right_less_than_length_left
                     + (length_left == length_right));
          continue ;
        }
        [[fallthrough]];
      }

      case SwitchLabels::length_left_less_than_length_right: {
        //assert(length_left < length_right);
        RandomAccessIterator symmetric_point_right;
        if (comp(*(symmetric_point_right = end_left + length_left), *start_left)) {
          //assert(!comp_le(*(end_left - 1), *end_right));
          bool is_left_length_less_than_right_length;
          do {
            //assert(length_left > 1 && length_right > 1);
            std::swap_ranges(start_left, start_right, start_right);
            start_left    = start_right;
            end_left      = symmetric_point_right;
            start_right   = symmetric_point_right + 1;
            length_right -= length_left;
            //assert(length_left > 1 && end_left != start_left);
            //assert(comp(*end_right, *end_left));
            //assert(!comp_le(*(end_left - 1), *end_right));
            //Note if length_right <= 1 then is_left_length_less_than_right_length
            // below is false.
          } while ((is_left_length_less_than_right_length = (length_left < length_right))
              && comp(*(symmetric_point_right = end_left + length_left), *start_left));
          is_startleft_less_or_equal_to_startright_plus1 = ThreeValue::Unknown;
          is_startleft_less_or_equal_to_startright
                  = static_cast<ThreeValue>(comp_le(*start_left, *start_right));
          //assert(length_left > 1 && end_left != start_left);
          //assert(!comp_le(*end_left, *end_right));
          //assert(!comp_le(*(end_left - 1), *end_right));
          //assert(comp(*start_right, *end_left));
          if (is_startleft_less_or_equal_to_startright) {
            jump_to = SwitchLabels::startleft_le_startright;
            continue ;
          } else if (length_right <= 1) {
            jump_to = SwitchLabels::trivial_case_length_right_equals_1;
            continue ;
          } else if (comp_le(*end_right, *start_left)) {
            jump_to = SwitchLabels::trivial_case_endright_le_startleft;
            continue ;
          } else if ((is_startleft_less_or_equal_to_startright_plus1 =
           static_cast<ThreeValue>(comp_le(*start_left, *(start_right + 1))))) {
            jump_to = SwitchLabels::startleft_le_startright_plus1;
            continue ;
          } else if (!is_left_length_less_than_right_length) {
            //assert(length_left >= length_right);
            jump_to = static_cast<SwitchLabels>(
                               SwitchLabels::length_right_less_than_length_left
                                + (length_left == length_right));
            continue ;
          }
          //assert(comp(*end_right, *(end_left - 1)));
        }
assert(start_right != end_right && start_right != one_past_end);
        break ;
      }

      case SwitchLabels::length_right_less_than_length_left: {
        //assert(length_left > length_right && length_right > 0);
        //assert(start_right == end_left + 1);
        RandomAccessIterator symmetric_point_left;
        if (comp(*end_right, *(symmetric_point_left = start_right - length_right))) {
          bool is_right_length_less_than_left;
          do {
            //assert(length_left > 1 && length_right > 1);
            //assert(one_past_end == end_right + 1);
            std::swap_ranges(start_right, one_past_end, symmetric_point_left);
            one_past_end = start_right;
            end_right    = one_past_end - 1;
            start_right  = symmetric_point_left; //= end_left + 1;
            //assert((end_left - length_right) == symmetric_point_left - 1);
            end_left     = symmetric_point_left - 1;
            length_left -= length_right;
            //assert(length_left > 0 && length_right > 1);
            //assert(comp(*start_right, *start_left));
            //assert(start_right != end_right && start_right != one_past_end);
            //assert(!comp_le(*start_left, *(start_right + 1)));
            //Note: if length_left <= 1 then is_right_less_than_left below
            // is false.
          } while ((is_right_length_less_than_left = (length_left > length_right)) &&
             comp(*end_right, *(symmetric_point_left = start_right - length_right)));
          is_endleft_minus1_less_or_equal_to_endright = ThreeValue::Unknown;
          is_endleft_less_or_equal_to_endright
                      = static_cast<ThreeValue>(comp_le(*end_left, *end_right));
          //assert(length_right > 1);
          //assert(start_right != end_right && start_right != one_past_end);
          //assert(!comp_le(*start_left, *start_right));
          //assert(!comp_le(*start_left, *(start_right + 1)));
          //assert(comp(*start_right, *end_left));
          if (is_endleft_less_or_equal_to_endright) {
            jump_to = SwitchLabels::endleft_le_endright;
            continue ;
          } else if (length_left <= 1) {
            jump_to = SwitchLabels::trivial_case_length_left_equals_1;
            continue ;
          } else if (comp_le(*end_right, *start_left)) {
            jump_to = SwitchLabels::trivial_case_endright_le_startleft;
            continue ;
          } else if ((is_endleft_minus1_less_or_equal_to_endright =
               static_cast<ThreeValue>(comp_le(*(end_left - 1), *end_right)))) {
            jump_to = SwitchLabels::endleft_minus1_le_endright;
            continue ;
          } else if (!is_right_length_less_than_left) {
            //assert(length_left <= length_right);
            //assert(!comp_le(*end_right, *start_left));
            jump_to = static_cast<SwitchLabels>(
                 SwitchLabels::length_left_less_than_length_right
               + (2 * static_cast<unsigned char>(length_left == length_right)));
            continue ;
          }
          //assert(comp(*(start_right + 1), *start_left));
        }
        break ;
      }

      case SwitchLabels::length_left_equal_to_length_right: {
        //assert(length_left == length_right);
        if (comp_le(*end_right, *start_left)) {
          jump_to = SwitchLabels::trivial_case_endright_le_startleft;
          continue ;
        }
        break ;
      }

      case SwitchLabels::trivial_case_length_left_equals_1: {
        //assert(length_left == 1 && start_left == end_left);
        //assert(start_left != start_right && start_right != one_past_end);
        if (length_right == 1) {
          std::iter_swap(start_left, start_right);
        } else {
          mwob_namespace::RotateLeftBy1<RandomAccessIterator, Distance>(
                                                      start_left, one_past_end);
        }
        //assert(std::is_sorted(start_left, one_past_end, comp));
        return true;
      }

      case SwitchLabels::trivial_case_length_right_equals_1: {
        //assert(length_right == 1 && start_right == end_right);
        mwob_namespace::RotateRightBy1<RandomAccessIterator, Distance>(
                                            start_left, end_right, length_left);
        //assert(std::is_sorted(start_left, one_past_end, comp));
        return true;
      }

      case SwitchLabels::trivial_case_endright_le_startleft: {
        if (comp(*end_right, *start_left)) {
          std::rotate(start_left, start_right, one_past_end);
          //assert(std::is_sorted(start_left, one_past_end, comp));
          return true;
        }
        //At this point:
        // (1) comp_le(*start_left, *end_right)
        // (2) comp_le(*end_right, *start_left)
        //     so in particular, *start_left and *end_right are equivalent
        //     under comp().
        // (3) comp(*start_right, *start_left)
        // (4) comp(*end_right, *end_left)
        // (5) start_left != end_left (or equivalently, length_left > 1)
        // (6) start_right != end_right (or equivalently, length_right > 1)
        // These imply that:
        // (A) comp(*start_right, *end_left)
        // (B) comp(*start_left, *end_right)
        //assert(length_right > 1 && length_left > 1);
        //assert(start_right != end_right && start_left != end_left);
        //assert(comp_le(*end_right, *start_left));
        //assert(comp_le(*start_left, *end_right));
        //assert(comp(*start_left, *end_left));
        //assert(comp(*start_right, *end_right));
       auto first_greater_than = mwob_namespace::SmallestIteratorGreaterThan_KnownToExist_RAI<
           RandomAccessIterator, Compare, Distance, CompareLessOrEqual, ValueType>(
           start_left + 1, end_left, start_right, *start_left, comp, comp_le,
           false //check_start_iterator_each_iteration
           );
       auto num_const_left  = std::distance(start_left, first_greater_than);
       auto last_less_than = mwob_namespace::LargestIteratorLessThan_KnownToExist_RAI<
           RandomAccessIterator, Compare, Distance, CompareLessOrEqual, ValueType>(
           start_right, end_right - 1, end_right, *start_left, comp, comp_le,
           false //check_end_iterator_each_iteration
           );

       auto num_const_right = std::distance(last_less_than, end_right);
       //In this comment, let end_right_value = *end_right and let
       // start_left_value = *start_left. Note that
       // comp_le(end_right_value, start_left_value) and
       // comp_le(start_left_value, end_right_value) (i.e. start_left_value
       // and end_right_value are equivalent under comp).
       //
       //After the following std::rotate() is executed, the value end_right_value
       // will be to the LEFT of the value start_left_value, which means that
       // the merge would not be stable. We correct this by performing another
       // std::rotate of all values that are equivalent to end_right_value
       auto location_of_old_start_left
                            = std::rotate(start_left, start_right, one_past_end);
       //assert(std::is_sorted(start_left, one_past_end, comp));
       std::rotate(location_of_old_start_left - num_const_right,
                              //leftmost value is equivalent to end_right_value
                   location_of_old_start_left,
                   location_of_old_start_left + num_const_left); //one past
                                //rightmost value equivalent to end_right_value

       //assert(std::is_sorted(start_left, one_past_end, comp));
        return true;
      }
    } //End switch()
    break ;
  } //END while (true)
  //assert(start_right == end_left + 1);
  //assert(length_left  == std::distance(start_left, end_left) + 1);
  //assert(length_right == std::distance(start_right, end_right) + 1);
  //assert(length_left > 1 && length_right > 1);
  //assert(comp(*end_right, *(end_left - 1)));
  //assert(comp(*(start_right + 1), *start_left));
  start_left_out   = start_left;
  start_right_out  = start_right;
  one_past_end_out = end_right + 1;
  length_left_out  = length_left;
  length_right_out = length_right;
  return false; //The two non-decreasing sequences are not yet merged.
}

//This is Trim2 designed for Bidirectional Iterators.
//Since computing std::distance() is potentially expensive, we keep track
// of length_left and length_right.
//
//In what follows, all assert()s would evaluate to true if they were to be
// called.
//Due to the similarity between Trim2_switch_bi() and Trim2_switch_RAI(),
// assert()s and comments that appear in Trim2_switch_RAI() and
// would be repeated in Trim2_switch_bi() are instead omitted
// from Trim2_switch_bi().
template<typename BidirectionalIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline bool Trim2_switch_bi(BidirectionalIterator &start_left_out,
                      BidirectionalIterator &start_right_out,
                      BidirectionalIterator &one_past_end_out,
                      Distance &length_left_out,
                      Distance &length_right_out,
                      Distance &d_out,
                      BidirectionalIterator &start_2nd_quarter_out,
                      BidirectionalIterator &start_4th_quarter_out,
                      Compare comp,
                      CompareLessOrEqual comp_le,
                      ThreeValue is_startleft_less_or_equal_to_startright,
                      ThreeValue is_startleft_less_or_equal_to_startright_plus1,
                      ThreeValue is_endleft_less_or_equal_to_endright,
                      ThreeValue is_endleft_minus1_less_or_equal_to_endright) {
  auto start_left   = start_left_out;
  auto start_right  = start_right_out;
  auto one_past_end = one_past_end_out;
  auto end_left     = start_right;
  (void)--end_left;
  auto end_right    = one_past_end;
  (void)--end_right;
  auto length_left  = length_left_out;
  auto length_right = length_right_out;
  auto start_right_plus1 = start_right;
  (void)++start_right_plus1;
  auto end_left_minus1 = end_left;
  (void)--end_left_minus1;
  if (is_startleft_less_or_equal_to_startright < ThreeValue::False)
    is_startleft_less_or_equal_to_startright
                  = static_cast<ThreeValue>(comp_le(*start_left, *start_right));
  if (is_endleft_less_or_equal_to_endright < ThreeValue::False)
    is_endleft_less_or_equal_to_endright
                      = static_cast<ThreeValue>(comp_le(*end_left, *end_right));
  enum SwitchLabels {
    startleft_le_startright = 0,
    endleft_le_endright,
    startleft_le_startright_plus1,
    endleft_minus1_le_endright,
    length_dispatch,
    length_left_less_than_length_right,
    length_right_less_than_length_left,
    length_left_equal_to_length_right,
    trivial_case_length_left_equals_1,
    trivial_case_length_right_equals_1,
    trivial_case_endright_le_startleft
  };
  BidirectionalIterator symmetric_point_left, symmetric_point_right;
  Distance distance_to_symmetric_point;
  SwitchLabels jump_to = static_cast<SwitchLabels>(0);
  while (true) {
    switch (jump_to) {
      case SwitchLabels::startleft_le_startright: {
        if (is_startleft_less_or_equal_to_startright != 0) {
          if (is_startleft_less_or_equal_to_startright > 0 ||
              comp_le(*start_left, *start_right)) {
            mwob_namespace::TrimLeft1_bi<BidirectionalIterator, Compare,
                 Distance, CompareLessOrEqual, ValueType>(start_left, end_left,
                                 start_right, length_left, comp, comp_le, true);
            if (length_left <= 1) {
              jump_to = SwitchLabels::trivial_case_length_left_equals_1;
              continue ;
            }
          }
        }
        is_startleft_less_or_equal_to_startright = ThreeValue::False;
        if (length_right <= 1) {
          jump_to = SwitchLabels::trivial_case_length_right_equals_1;
          continue ;
        }
      }
      [[fallthrough]];

      case SwitchLabels::endleft_le_endright: {
        if (is_endleft_less_or_equal_to_endright != ThreeValue::False) {
          if (is_endleft_less_or_equal_to_endright > 0 ||
              comp_le(*end_left, *end_right)) {
            mwob_namespace::TrimRight1_bi<BidirectionalIterator, Compare,
                Distance, CompareLessOrEqual, ValueType>(*end_left, start_right,
                    end_right, one_past_end, length_right, comp, comp_le, true);
            start_right_plus1 = start_right;
            (void)++start_right_plus1;
            if (length_right <= 1) {
              jump_to = SwitchLabels::trivial_case_length_right_equals_1;
              continue ;
            }
          }
        }
        is_endleft_less_or_equal_to_endright = ThreeValue::False;
        if (length_left <= 1) {
          jump_to = SwitchLabels::trivial_case_length_left_equals_1;
          continue ;
        }
      }
      [[fallthrough]];

      case SwitchLabels::startleft_le_startright_plus1: {
        if (is_startleft_less_or_equal_to_startright_plus1 != 0) {
          start_right_plus1 = start_right;
          (void)++start_right_plus1;
          if (is_startleft_less_or_equal_to_startright_plus1 > 0 ||
              comp_le(*start_left, *start_right_plus1)) {
            do {
              std::iter_swap(start_left++, start_right);
              --length_left;
            } while(comp_le(*start_left, *start_right_plus1)) ;
            if (length_left <= 1) {
              jump_to = SwitchLabels::trivial_case_length_left_equals_1;
              continue ;
            }
            is_startleft_less_or_equal_to_startright_plus1 = ThreeValue::False;
          }
        }
      }
      [[fallthrough]];

      case SwitchLabels::endleft_minus1_le_endright: {
        if (is_endleft_minus1_less_or_equal_to_endright != 0) {
          end_left_minus1 = end_left;
          (void)--end_left_minus1;
          if (is_endleft_minus1_less_or_equal_to_endright > 0 ||
              comp_le(*end_left_minus1, *end_right)) {
            do {
              std::iter_swap(end_left, end_right--);
              --length_right;
            } while(comp_le(*end_left_minus1, *end_right)) ;
            one_past_end = end_right;
            (void)++one_past_end;
            if (length_right <= 1) {
              jump_to = SwitchLabels::trivial_case_length_right_equals_1;
              continue ;
            }
          }
          is_endleft_minus1_less_or_equal_to_endright = ThreeValue::False;
        }
      }
      [[fallthrough]];

      case SwitchLabels::length_dispatch: {
        //assert(end_left_minus1 == end_left - 1);
        if (length_left >= length_right) {
          jump_to = static_cast<SwitchLabels>(
              SwitchLabels::length_right_less_than_length_left
              + (length_left == length_right));
          continue ;
        }
        [[fallthrough]];
      }

      case SwitchLabels::length_left_less_than_length_right: {
        symmetric_point_right = end_left;
        std::advance(symmetric_point_right, length_left);
        if (comp(*symmetric_point_right, *start_left)) {
          bool is_left_length_less_than_right_length;
          do {
            std::swap_ranges(start_left, start_right, start_right);
            start_left    = start_right;
            end_left      = symmetric_point_right;
            start_right   = symmetric_point_right;
            (void)++start_right;
            start_right_plus1 = start_right;
            (void)++start_right_plus1;
            length_right -= length_left;
            if (!(is_left_length_less_than_right_length
                  = (length_left < length_right)))
              break ;
            symmetric_point_right = end_left;
            std::advance(symmetric_point_right, length_left);
          } while (comp(*symmetric_point_right, *start_left));
          is_startleft_less_or_equal_to_startright_plus1 = ThreeValue::Unknown;
          is_startleft_less_or_equal_to_startright
                  = static_cast<ThreeValue>(comp_le(*start_left, *start_right));
          end_left_minus1 = end_left;
          (void)--end_left_minus1;
          bool is_length_right_less_or_equal_to_1 = (length_right <= 1);
          if (!is_length_right_less_or_equal_to_1) {
            start_right_plus1 = start_right;
            (void)++start_right_plus1;
          }
          if (is_startleft_less_or_equal_to_startright) {
            jump_to = SwitchLabels::startleft_le_startright;
            continue ;
          } else if (length_right <= 1) {
            jump_to = SwitchLabels::trivial_case_length_right_equals_1;
            continue ;
          } else if (comp_le(*end_right, *start_left)) {
            jump_to = SwitchLabels::trivial_case_endright_le_startleft;
            continue ;
          } else if ((is_startleft_less_or_equal_to_startright_plus1 =
           static_cast<ThreeValue>(comp_le(*start_left, *start_right_plus1)))) {
            jump_to = SwitchLabels::startleft_le_startright_plus1;
            continue ;
          } else if (!is_left_length_less_than_right_length) {
            jump_to = static_cast<SwitchLabels>(
                              SwitchLabels::length_right_less_than_length_left
                              + (length_left == length_right));
            continue ;
          }
        }
        symmetric_point_left = start_left;
        distance_to_symmetric_point = length_left;
        break ;
      }

      case SwitchLabels::length_right_less_than_length_left: {
        symmetric_point_left = start_right;
        std::advance(symmetric_point_left, - length_right);
        if (comp(*end_right, *symmetric_point_left)) {
          bool is_right_length_less_than_left;
          do {
            std::swap_ranges(start_right, one_past_end, symmetric_point_left);
            one_past_end = start_right;
            end_right    = one_past_end;
            (void)--end_right;
            start_right  = symmetric_point_left; //= end_left + 1;
            end_left     = symmetric_point_left;
            (void)--end_left;
            length_left -= length_right;
            start_right_plus1 = start_right;
            (void)++start_right_plus1;
            if (!(is_right_length_less_than_left = (length_left > length_right)))
              break ;
            symmetric_point_left = start_right;
            std::advance(symmetric_point_left, - length_right);
            distance_to_symmetric_point = length_right;
            //assert(std::distance(symmetric_point_left, end_left) ==
            //                           std::distance(start_right, end_right));
          } while (comp(*end_right, *symmetric_point_left));
          is_endleft_minus1_less_or_equal_to_endright = ThreeValue::Unknown;
          is_endleft_less_or_equal_to_endright
                      = static_cast<ThreeValue>(comp_le(*end_left, *end_right));
          bool is_length_left_less_or_equal_to_1 = (length_left <= 1);
          if (!is_length_left_less_or_equal_to_1) {
            end_left_minus1 = end_left;
            (void)--end_left_minus1;
          }
          start_right_plus1 = start_right;
          (void)++start_right_plus1;
          if (is_endleft_less_or_equal_to_endright) {
            jump_to = SwitchLabels::endleft_le_endright;
            continue ;
          } else if (length_left <= 1) {
            jump_to = SwitchLabels::trivial_case_length_left_equals_1;
            continue ;
          } else if (comp_le(*end_right, *start_left)) {
            jump_to = SwitchLabels::trivial_case_endright_le_startleft;
            continue ;
          } else if ((is_endleft_minus1_less_or_equal_to_endright =
              static_cast<ThreeValue>(comp_le(*end_left_minus1, *end_right)))) {
            jump_to = SwitchLabels::endleft_minus1_le_endright;
            continue ;
          } else if (!is_right_length_less_than_left) {
            jump_to = static_cast<SwitchLabels>(
               SwitchLabels::length_left_less_than_length_right
               + (2 * static_cast<unsigned char>(length_left == length_right)));
            continue ;
          }
        }
        symmetric_point_right = end_right;
        distance_to_symmetric_point = length_right;
        break ;
      }

      case SwitchLabels::length_left_equal_to_length_right: {
        if (comp_le(*end_right, *start_left)) {
          jump_to = SwitchLabels::trivial_case_endright_le_startleft;
          continue ;
        }
        symmetric_point_left = start_left;
        symmetric_point_right = end_right;
        distance_to_symmetric_point = length_left;
        break ;
      }

      case SwitchLabels::trivial_case_length_left_equals_1: {
        if (length_right == 1) {
          std::iter_swap(start_left, start_right);
        } else {
          mwob_namespace::RotateLeftBy1<BidirectionalIterator, Distance>(
                                                      start_left, one_past_end);
        }
        return true;
      }

      case SwitchLabels::trivial_case_length_right_equals_1: {
        mwob_namespace::RotateRightBy1<BidirectionalIterator, Distance>(
                                            start_left, end_right, length_left);
        return true;
      }

      case SwitchLabels::trivial_case_endright_le_startleft: {
        if (comp(*end_right, *start_left)) {
          std::rotate(start_left, start_right, one_past_end);
          return true;
        }
        Distance distance_from_start_it_to_first_greater_than;
  //    auto first_greater_than =
        mwob_namespace::SmallestIteratorGreaterThan_KnownToExist_bi<
          BidirectionalIterator, Compare, Distance, CompareLessOrEqual, ValueType>(
          start_left, end_left, start_right, length_left, *start_left,
          distance_from_start_it_to_first_greater_than,
          comp, comp_le
          );
        //assert(distance_from_start_it_to_first_greater_than ==
        //                       std::distance(start_left, first_greater_than));
        auto num_const_left = distance_from_start_it_to_first_greater_than;
        Distance distance_from_end_right_to_last_less_than;
//    auto last_less_than =
        mwob_namespace::LargestIteratorLessThan_KnownToExist_bi<
        BidirectionalIterator, Compare, Distance, CompareLessOrEqual, ValueType>(
        start_right, end_right, one_past_end, length_right, *start_left,
        distance_from_end_right_to_last_less_than,
        comp, comp_le
        );
        //auto distance_from_start_it_to_last_less_than = length_right
        //                          - distance_from_end_right_to_last_less_than;
        //assert(distance_from_end_right_to_last_less_than ==
        //                            std::distance(last_less_than, end_right));
        auto num_const_right = distance_from_end_right_to_last_less_than;
        auto location_of_old_start_left
                           = std::rotate(start_left, start_right, one_past_end);
        auto left_most_value = location_of_old_start_left;
        std::advance(left_most_value, - num_const_right);
        auto one_past_right_most_value = location_of_old_start_left;
        std::advance(one_past_right_most_value, num_const_left);
        std::rotate(left_most_value,
                    location_of_old_start_left,
                    one_past_right_most_value);
        return true;
      } //End case
    } //End switch()
    break ;
  } //END while (true)
  //one_past_end = end_right;
  //(void)++one_past_end;
  //assert(one_past_end == end_right + 1);
  //assert(comp(*start_right, *end_left));
  //assert(std::distance(symmetric_point_left, end_left) ==
  //                         std::distance(start_right, symmetric_point_right));
  //assert(distance_to_symmetric_point ==
  //                          std::distance(symmetric_point_left, start_right));
  //assert(distance_to_symmetric_point >= 2);
  //assert(comp_le(*symmetric_point_left, *symmetric_point_right));

  //We will now find the SMALLEST integer
  // 1 <= d < distance_to_symmetric_point such that
  // *(end_left - d) <= *(start_right + d)
  Distance d;
  //We will then define start_2nd_quarter and start_4th_quarter by:
  // start_2nd_quarter = start_right - d;
  // start_4th_quarter = start_right + d;
  BidirectionalIterator start_2nd_quarter, start_4th_quarter;
  {
    Distance d_min = 1, d_max = distance_to_symmetric_point - 1;
    //assert(start_left != end_left && start_right != end_right);
    BidirectionalIterator left_it_min = end_left, right_it_min = start_right;
    (void)--left_it_min;
    (void)++right_it_min;
    start_2nd_quarter = symmetric_point_left;
    start_4th_quarter = symmetric_point_right;

    while (d_min < d_max) {
      //assert(d_min == std::distance(start_right, right_it_min));
      //assert(d_min == std::distance(left_it_min, end_left));
      //assert(d_max == std::distance(start_2nd_quarter, end_left));
      //assert(d_max == std::distance(start_right, start_4th_quarter));
      auto half_distance = (d_max - d_min) / 2;
      auto next_left_it  = left_it_min;
      auto next_right_it = right_it_min;
      std::advance(next_left_it,  - half_distance);
      std::advance(next_right_it,   half_distance);
      if (comp_le(*next_left_it, *next_right_it)) {
        d_max = d_min + half_distance;
        start_2nd_quarter = next_left_it;
        start_4th_quarter = next_right_it;
      } else { // else *next_right_it < *next_left_it
        d_min += half_distance + 1;
        //assert(d_min <= d_max);
        //assert(next_left_it != start_left && next_right_it != end_right);
        left_it_min  = next_left_it;
        right_it_min = next_right_it;
        (void)--left_it_min;
        (void)++right_it_min;
      }
    }
    //assert(d_min == d_max && d_min > 0);
    d = d_min;
    //assert(comp_le(*start_2nd_quarter, *start_4th_quarter));
    //assert(start_2nd_quarter != end_left && start_4th_quarter != start_right);
    //assert(!comp_le(*(start_2nd_quarter + 1), *start_4th_quarter_minus1));
    //assert(comp(*(start_4th_quarter - 1), *start_2nd_quarter_plus1));
  }
  //assert(one_past_end == end_right + 1 && start_right == end_left + 1);
  //assert(length_left  == std::distance(start_left, end_left) + 1);
  //assert(length_right == std::distance(start_right, end_right) + 1);
  //assert(comp(*end_right, *(end_left - 1)));
  //assert(comp(*(start_right + 1), *start_left));
  start_left_out   = start_left;
  start_right_out  = start_right;
  one_past_end_out = one_past_end;
  d_out            = d;
  start_2nd_quarter_out = ++start_2nd_quarter;;
  start_4th_quarter_out = start_4th_quarter;
  length_left_out  = length_left;
  length_right_out = length_right;
  return false; //The two non-decreasing sequences are not yet merged.
}

//Assumes that:
// (1) length_left > 0 and length_right > 0, where
//      length_left  == std::distance(start_left,  start_right) and
//      length_right == std::distance(start_right, one_past_end)
// (2) comp(*start_right, *end_left) where end_left == start_right - 1
// (3) If is_left_trimmed == true  then comp(*start_right, *start_left)
// (4) If is_left_trimmed == false then comp(*end_right,   *end_left)
template<typename RandomAccessIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
void MergeWithOutBuffer2_recursive_RAI(RandomAccessIterator start_left,
                     RandomAccessIterator start_right,
                     RandomAccessIterator one_past_end,
                     Distance length_left,
                     Distance length_right,
                     Compare comp,
                     CompareLessOrEqual comp_le,
                     ThreeValue is_startleft_less_or_equal_to_startright,
                     ThreeValue is_startleft_less_or_equal_to_startright_plus1,
                     ThreeValue is_endleft_less_or_equal_to_endright,
                     ThreeValue is_endleft_minus1_less_or_equal_to_endright) {
  //assert(length_left > 0 && length_right > 0);
  //assert(comp(*start_right, *(start_right - 1)));
  //if (is_startleft_less_or_equal_to_startright >= 0)
  //    assert(is_startleft_less_or_equal_to_startright ==
  //           comp_le(*start_left, *start_right));
  //if (is_startleft_less_or_equal_to_startright_plus1 >= 0)
  //    assert(length_right > 1 && is_startleft_less_or_equal_to_startright_plus1
  //                                 == comp_le(*start_left, *start_right + 1));
  //if (is_endleft_less_or_equal_to_endright >= 0)
  //    assert(is_endleft_less_or_equal_to_endright ==
  //           comp_le(*end_left, *end_right));
  //if (is_endleft_minus1_less_or_equal_to_endright >= 0)
  //    assert(length_left > 1 && is_endleft_minus1_less_or_equal_to_endright ==
  //                              comp_le(*(end_left - 1), *end_right));
  if (Trim2_switch_RAI<RandomAccessIterator, Compare, Distance,
                       CompareLessOrEqual, ValueType>(start_left, start_right,
                           one_past_end, length_left, length_right, comp,
                           comp_le, is_startleft_less_or_equal_to_startright,
                           is_startleft_less_or_equal_to_startright_plus1,
                           is_endleft_less_or_equal_to_endright,
                           is_endleft_minus1_less_or_equal_to_endright)) {
    //assert(std::is_sorted(start_left, one_past_end, comp));
    return ; //The two non-decreasing sequences have been merged.
  }
  //assert(length_left  == std::distance(start_left,  start_right));
  //assert(length_right == std::distance(start_right, one_past_end));
  auto length_smaller = length_left < length_right ? length_left : length_right;
  Distance d = mwob_namespace::DisplacementToPotentialMedians_KnownToExist_RAI<
      RandomAccessIterator, Compare, Distance, CompareLessOrEqual, ValueType>(
          start_right - 1, start_right, length_smaller, comp, comp_le);
  //assert(d > 0 && d < length_smaller);
  {
    auto start_2nd_quarter = start_right - d;
    std::swap_ranges(start_2nd_quarter, start_right, start_right);
    //auto one_past_end_2nd_quarter = start_right;
    //Distance length_first_quarter = length_left - d;
    MergeWithOutBuffer2_recursive_RAI<RandomAccessIterator, Compare, Distance,
                                      CompareLessOrEqual, ValueType>(
      start_left, start_2nd_quarter, start_right, length_left - d, d,
      comp, comp_le,
      ThreeValue::False, ThreeValue::False, ThreeValue::Unknown,
      ThreeValue::Unknown);
  }
  //auto start_4th_quarter = start_right + d;
  //Distance length_4th_quarter = length_right - d;
  MergeWithOutBuffer2_recursive_RAI<RandomAccessIterator, Compare, Distance,
                                    CompareLessOrEqual, ValueType>(
    start_right, start_right + d, one_past_end, d, length_right - d,
    comp, comp_le,
    ThreeValue::Unknown, ThreeValue::Unknown, ThreeValue::False,
    ThreeValue::False);
  return ;
}

//This is the BidirectionalIterator version of
// MergeWithOutBuffer2_recursive_RAI().
//See MergeWithOutBuffer2_recursive_RAI() for a description of this function.
template<typename BidirectionalIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
void MergeWithOutBuffer2_recursive_bi(BidirectionalIterator start_left,
                      BidirectionalIterator start_right,
                      BidirectionalIterator one_past_end,
                      Distance length_left,
                      Distance length_right,
                      Compare comp,
                      CompareLessOrEqual comp_le,
                      ThreeValue is_startleft_less_or_equal_to_startright,
                      ThreeValue is_startleft_less_or_equal_to_startright_plus1,
                      ThreeValue is_endleft_less_or_equal_to_endright,
                      ThreeValue is_endleft_minus1_less_or_equal_to_endright) {
  BidirectionalIterator start_2nd_quarter, start_4th_quarter;
  Distance d;
  //assert(comp(*start_right, *(start_right - 1)));
  //assert(length_left == std::distance(start_left, start_right));
  //assert(length_right == std::distance(start_right, one_past_end));
  if (Trim2_switch_bi<BidirectionalIterator, Compare, Distance,
                      CompareLessOrEqual, ValueType>(start_left, start_right,
                          one_past_end, length_left, length_right, d,
                          start_2nd_quarter, start_4th_quarter,
                          comp, comp_le,
                          is_startleft_less_or_equal_to_startright,
                          is_startleft_less_or_equal_to_startright_plus1,
                          is_endleft_less_or_equal_to_endright,
                          is_endleft_minus1_less_or_equal_to_endright)) {
    //assert(std::is_sorted(start_left, one_past_end, comp));
    return ; //The two non-decreasing sequences have been merged.
  }
  //assert(length_left == std::distance(start_left, start_right));
  //assert(length_right == std::distance(start_right, one_past_end));
  {
    //auto start_2nd_quarter = start_right - d;
    auto one_past_end_2nd_quarter = start_right;
    std::swap_ranges(start_2nd_quarter, one_past_end_2nd_quarter, start_right);
    Distance length_first_quarter = length_left - d;

    MergeWithOutBuffer2_recursive_bi<BidirectionalIterator, Compare, Distance,
                                     CompareLessOrEqual, ValueType>(
      start_left, start_2nd_quarter, start_right, length_first_quarter, d,
      comp, comp_le,
      ThreeValue::False, ThreeValue::False, ThreeValue::Unknown,
      ThreeValue::Unknown);
  }
  //auto start_4th_quarter = start_right; std::advance(start_4th_quarter, d);
  Distance length_4th_quarter = length_right - d;
  MergeWithOutBuffer2_recursive_bi<BidirectionalIterator, Compare, Distance,
                                   CompareLessOrEqual, ValueType>(
    start_right, start_4th_quarter, one_past_end, d, length_4th_quarter,
    comp, comp_le,
    ThreeValue::Unknown, ThreeValue::Unknown, ThreeValue::False,
    ThreeValue::False);
  //assert(std::is_sorted(start_left, one_past_end, comp));
  return ;
}

//Assumes that:
// (1) length_left > 0 and length_right > 0, where
//      length_left  == std::distance(start_left,  start_right) and
//      length_right == std::distance(start_right, one_past_end)
// (2) start_right == end_left + 1
// (3) comp(*start_right, *end_left)
template<typename RandomAccessIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline void MergeWithOutBuffer2_RAI(RandomAccessIterator start_left,
                                    RandomAccessIterator end_left,
                                    RandomAccessIterator start_right,
                                    RandomAccessIterator one_past_end,
                                    Distance length_left,
                                    Distance length_right,
                                    Compare comp,
                                    CompareLessOrEqual comp_le) {
  //assert(length_left > 0 && length_right > 0);
  //assert(end_left + 1 == start_right);
  //assert(comp(*start_right, *end_left));
  if (comp_le(*start_left, *start_right)) {
    mwob_namespace::TrimLeft1_RAI<RandomAccessIterator,
              Compare, Distance, CompareLessOrEqual, ValueType>(
                start_left, end_left, *start_right, length_left, comp, comp_le);
  }
  //assert(length_left > 0 && length_right > 0);
  MergeWithOutBuffer2_recursive_RAI<RandomAccessIterator, Compare, Distance,
                                    CompareLessOrEqual, ValueType>(
                               start_left, start_right, one_past_end,
                               length_left, length_right, comp, comp_le,
                               ThreeValue::False, ThreeValue::Unknown,
                               ThreeValue::Unknown, ThreeValue::Unknown);
  return ;
}

template<typename BidirectionalIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline void MergeWithOutBuffer2_bi(BidirectionalIterator start_left,
                                   BidirectionalIterator end_left,
                                   BidirectionalIterator start_right,
                                   BidirectionalIterator one_past_end,
                                   Distance length_left,
                                   Distance length_right,
                                   Compare comp,
                                   CompareLessOrEqual comp_le) {
  //assert(length_left != 0 && length_right != 0);
  //assert(end_left + 1 == start_right);
  if (comp_le(*start_left, *start_right)) {
    mwob_namespace::TrimLeft1_bi<BidirectionalIterator,
    Compare, Distance, CompareLessOrEqual, ValueType>(start_left, end_left,
                                      start_right, length_left, comp, comp_le);
  }
  MergeWithOutBuffer2_recursive_bi<BidirectionalIterator, Compare, Distance,
                                    CompareLessOrEqual, ValueType>(
                               start_left, start_right, one_past_end,
                               length_left, length_right, comp, comp_le,
                               ThreeValue::False, ThreeValue::Unknown,
                               ThreeValue::Unknown, ThreeValue::Unknown);
  return ;
}

template<typename RandomAccessIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline void MergeWithOutBuffer2(RandomAccessIterator start_left,
                                RandomAccessIterator end_left,
                                RandomAccessIterator start_right,
                                RandomAccessIterator one_past_end,
                                Distance length_left,
                                Distance length_right,
                                Compare comp,
                                CompareLessOrEqual comp_le,
                                std::random_access_iterator_tag) {
  MergeWithOutBuffer2_RAI<RandomAccessIterator, Compare, Distance,
    CompareLessOrEqual, ValueType>(start_left, end_left, start_right,
        one_past_end, length_left, length_right, comp, comp_le);
  return ;
}

template<typename BidirectionalIterator, typename Compare,
         typename Distance, typename CompareLessOrEqual, typename ValueType>
inline void MergeWithOutBuffer2(BidirectionalIterator start_left,
                                BidirectionalIterator end_left,
                                BidirectionalIterator start_right,
                                BidirectionalIterator one_past_end,
                                Distance length_left,
                                Distance length_right,
                                Compare comp,
                                CompareLessOrEqual comp_le,
                                std::bidirectional_iterator_tag) {
  //assert(end_left + 1 == start_right);
  MergeWithOutBuffer2_bi<BidirectionalIterator, Compare, Distance,
     CompareLessOrEqual, ValueType>(start_left, end_left, start_right,
         one_past_end, length_left, length_right, comp, comp_le);
  return ;
}

} //END namespace: merge_without_buffer_2_namespace



//Dispatch function
template<typename Iterator, typename Compare,
         typename Distance = typename Iterator::difference_type>
inline void MergeWithOutBuffer2(Iterator start_left,
                                Iterator start_right,
                                Iterator one_past_end_right,
                                Distance length_left,
                                Distance length_right,
                                Compare comp) {
  if (start_left == start_right || start_right == one_past_end_right)
    return ;
  typedef typename Iterator::value_type ValueType;
  auto comp_le = [comp](const ValueType &lhs, const ValueType &rhs) -> bool {
    return !comp(rhs, lhs);
  };
  typedef decltype(comp_le) CompareLessOrEqual;
  Iterator end_left = start_right;
  (void)--end_left;
  if (comp_le(*end_left, *start_right)) //i.e. if *end_left <= *start_right
    return ;
  //assert(end_left + 1 == start_right);
  merge_without_buffer_2_namespace::MergeWithOutBuffer2<Iterator, Compare,
                  Distance, CompareLessOrEqual, ValueType>(
                  start_left, end_left, start_right, one_past_end_right,
                  length_left, length_right, comp, comp_le,
                  typename std::iterator_traits<Iterator>::iterator_category());

 return ;
}

template<typename Iterator, typename Compare,
         typename Distance = typename Iterator::difference_type>
inline void MergeWithOutBuffer2(Iterator start_left,
                                Iterator start_right,
                                Iterator one_past_end_right,
                                Compare comp) {
  Distance length_left  = std::distance(start_left, start_right);
  Distance length_right = std::distance(start_right, one_past_end_right);
  MergeWithOutBuffer2<Iterator, Compare, Distance>(
                              start_left, start_right, one_past_end_right,
                              length_left, length_right, comp);
 return ;
}


//Dispatch function
template<typename Iterator,
         typename Distance = typename Iterator::difference_type>
inline void MergeWithOutBuffer2(Iterator start_left,
                                Iterator start_right,
                                Iterator one_past_end_right) {
  typedef typename Iterator::value_type ValueType;
  auto comp = std::less<ValueType>();
  typedef decltype(comp) Compare;
  MergeWithOutBuffer2<Iterator, Compare, Distance>(
              start_left, start_right, one_past_end_right, comp);
  return ;
}


template<typename Iterator, typename Compare, typename Distance>
struct MergeWOBuff2 {
  inline void operator()(Iterator start_left,
      Iterator start_right,
      Iterator one_past_end_right,
      Distance length_left,
      Distance length_right,
      Compare comp) {
    MergeWithOutBuffer2<Iterator, Compare, Distance>(start_left, start_right,
        one_past_end_right, length_left, length_right, comp);
    return ;
  }
};




#endif /* SRC_MERGE_WITHOUT_BUFFER2_H_ */
