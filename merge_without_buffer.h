/*
 * merge_without_buffer.h
 *
 *  Created on: Jun 29, 2017
 *      Author: Matthew Gregory Krupa
 *
 *  The most important function defined in this file is MergeWithOutBuffer().
 *  MergeWithOutBufferTrim1() contains the near minimum code needed to make the
 *   merge algorithm work.
 *   MergeWithOutBuffer() is the fastest of MergeWithOutBuffer(),
 *   MergeWithOutBufferTrim4(), MergeWithOutBufferTrim3(),
 *   MergeWithOutBufferTrim5(), and MergeWithOutBufferTrim1().
 */

/*
 * Note that comp(x, y) is assumed to be a strict weak ordering.
 * (See https://en.wikipedia.org/wiki/Weak_ordering#Strict_weak_orderings )
 * This means that for all x, y, z:
 * (1) comp(x, x) is false,
 * (2) if comp(x, y) then !comp(y, x),
 * (3) if comp(x, y) and comp(y, z) then comp(x, z), and
 * (4) in addition, any of the following equivalent conditions hold:
 *   (4a) (transitivity of incomparability) if x is incomparable with y
 *        (i.e. neither x < y nor y < x is true), and y is incomparable with z,
 *        then x is incomparable with z.
 *   (4b) If x < y, then for all z, either x < z or z < y or both.
 *   (4c) If x is incomparable with y, then for all z != x, z != y,
 *         either (x < z and y < z) or (z < x and z < y) or
 *         (z is incomparable with x and z is incomparable with y).
 *
 * Theorem: Define the complement of comp() as C(x, y) := !comp(y, x).
 * Then C(x, y) is a total pre-order. In particular, C(x, y) is transitive.
 *
 * Note that for all x, y, z,
 * (5) if comp(x, y) and !comp(z, y) (i.e. y <= z) then comp(x, z) (i.e. x < z).
 *     (Proof: Since comp(x, y), using (4b) gives us either comp(x, z), or
 *      comp(z, y), or both. Since !comp(z, y) we must have comp(x, z),
 *      as desired Q.E.D.).
 * (6) if !comp(y, x) (i.e. x <= y) and comp(y, z) then comp(x, z) (i.e. x < z).
 *     (Proof: Since comp(y, z), using (4b) gives us either comp(y, x), or
 *      comp(x, z), or both. Since !comp(y, x) we must have comp(x, z),
 *      as desired Q.E.D.).
 *
 * Note that a range [start, end] is sorted with respect to comp() if and only
 *  if for all iterators it in [start, end] such that it + 1 is also an iterator
 *  defined in [start, end], comp(*(it+1), *it) is FALSE (i.e.
 *  C(*it, *(it+1)) is true).
 * See https://stackoverflow.com/a/59348418/7366867
 *  for the reason why this is true.
 *
 * Suppose that [start, end] is sorted. *
 * (7) if v is such that comp(*end, v) then for all iterators it in
 *     [start, end], comp(*it, v).
 *
 *     (Proof: Clearly this is true if it == end, so assume that it != end.
 *      Using (4b) with x = *end, y = v, and z = *it, gives us that either
 *       comp(*end, *it), comp(*it, v), or both. Since [start, end] is
 *       sorted, we have !comp(*end, *it) so we must have comp(*it, v),
 *       as desired. Q.E.D.)
 *
 * (8) if v is such that comp(v, *start) then for all iterators it in
 *     [start, end], comp(v, *it).
 *
 *     (Proof: Clearly this is true if it == start, so assume that it != start.
 *      Using (4b) with x = v, y = *start, and z = *it gives us that either
 *       comp(v, *it), comp(*it, *start), or both. Since [start, end] is
 *       sorted, we have !comp(*it, *start) so we must have comp(v, *it),
 *       as desired. Q.E.D.)
 *
 * (9) if v is such that !comp(v, *end) (i.e. *end <= v) then for all
 *     iterators it in [start, end], !comp(v, *it) (i.e. *it <= v).
 *
 *     (Proof: If it == end then the conclusion is immediate.
 *      So assume WLOG that it != end. Note that if v == *end then the
 *      desired conclusion is immediate, so assume that v != *end.
 *      If comp(*end, v) then by (2) above we have comp(*it, v) so that
 *      !comp(v, *it), as desired. So assume that !comp(*end, v).
 *      Thus v and *end are incomparable.
 *      Suppose from the sake of contradiction that comp(v, *it).
 *      Using (4b) with x = v, y = *it, and z = *end, we must have either
 *      comp(v, *end), or comp(*end, *it), or both. Since !comp(*end, v),
 *      it follows that comp(*end, *it). But this contradicts the fact that
 *      [start, end] is sorted. Thus we must have !comp(v, *it). Q.E.D.)
 *
 * (10) if v is such that !comp(*start, v) (i.e. v <= *start) then for all
 *     iterators it in [start, end], !comp(*it, v) (i.e. v <= *it).
 *
 *     (Proof: If it == start then the conclusion is immediate.
 *      So assume WLOG that it != start. Note that if v == *start then the
 *      desired conclusion is immediate, so assume that v != *start.
 *      If comp(v, *start) then by (3) above we have comp(v, *it) so that
 *      !comp(*it, v), as desired. So assume that !comp(v, *start).
 *      Thus v and *start are incomparable.
 *      Suppose from the sake of contradiction that comp(*it, v).
 *      Using (4b) with x = *it, y = v, and z = *start, we must have either
 *      comp(*it, *start), or comp(*start, v), or both. Since !comp(*start, v),
 *      it follows that comp(*it, *start). But this contradicts the fact that
 *      [start, end] is sorted. Thus we must have !comp(*it, v). Q.E.D.)
 *
 *
 * Suppose that [start, end] is sorted and that
 * !comp(*start, *end) (i.e. if *end <= *start). Then:
 * (11) for all iterators it in [start, end],
 *     !comp(*start, *it) and !comp(*it, *end)
 *     (i.e. *end <= *it and *it <= *start).
 *
 *     (Proof: Note that if it == start or it == end then this is immediate, so
 *      suppose that it != start and it != end. Suppose that comp(*start, *it).
 *      Then by letting z := end in (4b) we have either comp(*start, *end),
 *       or comp(*end, *it), or both. Since [start, end] is sorted,
 *       comp(*end, *it) is impossible so that we must have comp(*start, *end),
 *       but then this contradicts the assumption that !comp(*start, *end).
 *       Thus we must have !comp(*start, *it).
 *      Now suppose for the sake of contradiction that comp(*it, *end).
 *      Then by letting z := start in (4b) we have either comp(*it, *start), or
 *       comp(*start, *end), or both. By assumption, !comp(*start, *end) so
 *       that we must have comp(*it, *start), but then this contradicts
 *       the fast that [start, end] is sorted. Thus we must have
 *       !comp(*it, *end). Q.E.D.)
 *
 * (12) for any x, comp(*start, x) if and only if comp(*end, x).
 *      (Proof: (<=) follows from comp(*end, x) and (6) above.
 *       (=>) follows from comp(*start, x) and (6) above. Q.E.D.)
 *
 * (13) for any x, comp(x, *start) if and only if comp(x, *end).
 *      (Proof: (<=) follows from comp(x, *end) and (6) above.
 *       (=>) follows from comp(x, *start) and (6) above. Q.E.D.)
 *
 * (14) for any x, !comp(x, *start) (i.e. *start <= x) if and only if
 *       !comp(x, *end) (i.e. *end <= x).
 *      (Proof: This follows from the transitivity of the complement of comp().)
 *
 * (15) for any x, !comp(*start, x) (i.e. x <= *start) if and only if
 *       !comp(*end, x) (i.e. x <= *end).
 *      (Proof: This follows from the transitivity of the complement of comp().)
 *
 * Suppose that start < start_right <= end, let end_left := start_right - 1,
 *  and suppose that all of the following hold:
 *  (I)   [start, start_right) is sorted,
 *  (II)  [start_right, end]   is sorted,
 *  (III) comp(*start_right, *end_left),
 *  (IV)  comp(*start_right, *start_left), and
 *  (V)   comp(*end, *end_left)
 * Let it_right be the largest iterator in [start_right, end] such that
 *  comp(*start, *it_right).
 * Let it_left be the smallest iterator in [start, start_right) such that
 *  comp(*it_left, *end).
 * Then,
 *  (1) if !comp(*start_right, *end) (i.e. if *end <= *start_right)) then
 */

//NOTE: To increase the speed of code, remove ASSERT()s.

#ifndef MERGE_WITHOUT_BUFFER_H_
#define MERGE_WITHOUT_BUFFER_H_

#include "merge_without_buffer2.h"

//Dispatch function
template<typename Iterator, typename Compare,
         typename Distance = typename Iterator::difference_type>
inline void MergeWithOutBuffer(Iterator start_left,
                               Iterator start_right,
                               Iterator one_past_end_right,
                               Distance length_left,
                               Distance length_right,
                               Compare comp) {
  MergeWithOutBuffer2<Iterator, Compare, Distance>(
                  start_left, start_right, one_past_end_right,
                  length_left, length_right, comp);
 return ;
}

template<typename Iterator, typename Compare,
         typename Distance = typename Iterator::difference_type>
inline void MergeWithOutBuffer(Iterator start_left,
                                Iterator start_right,
                                Iterator one_past_end_right,
                                Compare comp) {
  Distance length_left  = std::distance(start_left, start_right);
  Distance length_right = std::distance(start_right, one_past_end_right);
  MergeWithOutBuffer<Iterator, Compare, Distance>(
                              start_left, start_right, one_past_end_right,
                              length_left, length_right, comp);
  return ;
}

template<typename Iterator, typename Compare, typename Distance>
struct MergeWOBuff {
  inline void operator()(Iterator start_left,
      Iterator start_right,
      Iterator one_past_end_right,
      Distance length_left,
      Distance length_right,
      Compare comp) {
    MergeWithOutBuffer<Iterator, Compare, Distance>(start_left, start_right,
        one_past_end_right, length_left, length_right, comp);
    return ;
  }
};


#endif /* SRC_MERGE_WITHOUT_BUFFER_H_ */
