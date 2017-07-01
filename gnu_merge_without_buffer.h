 // Copyright (C) 2001-2014 Free Software Foundation, Inc.
 //
 // This file is part of the GNU ISO C++ Library.  This library is free
 // software; you can redistribute it and/or modify it under the
 // terms of the GNU General Public License as published by the
 // Free Software Foundation; either version 3, or (at your option)
 // any later version.

 // This library is distributed in the hope that it will be useful,
 // but WITHOUT ANY WARRANTY; without even the implied warranty of
 // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 // GNU General Public License for more details.

 // Under Section 7 of GPL version 3, you are granted additional
 // permissions described in the GCC Runtime Library Exception, version
 // 3.1, as published by the Free Software Foundation.

 // You should have received a copy of the GNU General Public License and
 // a copy of the GCC Runtime Library Exception along with this program;
 // see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
 // <http://www.gnu.org/licenses/>.

 /*
  *
  * Copyright (c) 1994
  * Hewlett-Packard Company
  *
  * Permission to use, copy, modify, distribute and sell this software
  * and its documentation for any purpose is hereby granted without fee,
  * provided that the above copyright notice appear in all copies and
  * that both that copyright notice and this permission notice appear
  * in supporting documentation.  Hewlett-Packard Company makes no
  * representations about the suitability of this software for any
  * purpose.  It is provided "as is" without express or implied warranty.
  *
  *
  * Copyright (c) 1996
  * Silicon Graphics Computer Systems, Inc.
  *
  * Permission to use, copy, modify, distribute and sell this software
  * and its documentation for any purpose is hereby granted without fee,
  * provided that the above copyright notice appear in all copies and
  * that both that copyright notice and this permission notice appear
  * in supporting documentation.  Silicon Graphics makes no
  * representations about the suitability of this software for any
  * purpose.  It is provided "as is" without express or implied warranty.
  */
/* The above license is taken from
 * https://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen/a01499_source.html
 */

#ifndef SRC_GNU_MERGE_WITHOUT_BUFFER_H_
#define SRC_GNU_MERGE_WITHOUT_BUFFER_H_

#include <algorithm>

namespace gnu {
/* The following gnu__merge_without_buffer() function is taken directly from
 *  the GNU public library. Its definition (as of 29 June 2017) can be found here:
 *  https://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen/a01499_source.html#l02491
 * Due to the definition of the stl_algo.h header, it is unfortunately not
 *  possible to directly call the function __merge_without_buffer(),
 *  which necessitated the creation of the gnu__merge_without_buffer() function
 *  below.
 * Due to linking issues, the following changes have been made:
 * (1) the name of this function was changed from
 *  __merge_without_buffer to gnu__merge_without_buffer and two instances of
 *  std::__merge_without_buffer were changed to gnu__merge_without_buffer.
 * (2) if (__comp(__middle, __first)) was changed to if (__middle < __first)
 * (3) std::__lower_bound was changed to std::lower_bound
 * (4) std::__upper_bound was changed to std::upper_bound
 * (5) __gnu_cxx::__ops::__iter_comp_val(__comp) was changed to __comp
 * (6) __gnu_cxx::__ops::__val_comp_iter(__comp) was changed to __comp
 * These are the only modification that were made to the original function
 *  whose original definition can be found in the link to the public GNU
 *  library provided above. Since we will be sorting elements of type int,
 *  there should not be, at least in theory, any difference in performance  
 *  between gnu__merge_without_buffer() and __merge_without_buffer().
 */
template<typename _BidirectionalIterator, typename _Distance,
         typename _Compare>
      void
      gnu__merge_without_buffer(_BidirectionalIterator __first,
                             _BidirectionalIterator __middle,
                 _BidirectionalIterator __last,
                 _Distance __len1, _Distance __len2,
                 _Compare __comp)
      {
        if (__len1 == 0 || __len2 == 0)
      return;
        if (__len1 + __len2 == 2)
      {
        if (__middle < __first)//if (__comp(__middle, __first))
          std::iter_swap(__first, __middle);
        return;
      }
        _BidirectionalIterator __first_cut = __first;
        _BidirectionalIterator __second_cut = __middle;
        _Distance __len11 = 0;
        _Distance __len22 = 0;
        if (__len1 > __len2)
      {
        __len11 = __len1 / 2;
        std::advance(__first_cut, __len11);
        __second_cut
          = std::lower_bound(__middle, __last, *__first_cut,
                   __comp);
        __len22 = std::distance(__middle, __second_cut);
      }
        else
      {
        __len22 = __len2 / 2;
        std::advance(__second_cut, __len22);
        __first_cut
          = std::upper_bound(__first, __middle, *__second_cut,
                   __comp);
        __len11 = std::distance(__first, __first_cut);
      }
        std::rotate(__first_cut, __middle, __second_cut);
        _BidirectionalIterator __new_middle = __first_cut;
        std::advance(__new_middle, std::distance(__middle, __second_cut));
        gnu__merge_without_buffer(__first, __first_cut, __new_middle,
                    __len11, __len22, __comp);
        gnu__merge_without_buffer(__new_middle, __second_cut, __last,
                    __len1 - __len11, __len2 - __len22, __comp);
      }
}

#endif /* SRC_GNU_MERGE_WITHOUT_BUFFER_H_ */
