# FastMergeWithOutBuffer
This project implements a new inplace and stable recursive merge function that merges two non-decreasing ranges into one without the use of a buffer. It is compared to the implementation of __merge_without_buffer found in GNU ISO C++ Library and timed to be faster. Three other variants of this function are defined but the fastest of them is denoted MergeWithOutBuffer() and is found in merge_without_buffer.h, which contains the primary algorithm of this project and which is a standalone C++ header file (i.e. no other files are needed).

In additional, unlike std::__merge_without_buffer(), the two sorted sequences to be merged need NOT even be accessed by iterators of the same type.
