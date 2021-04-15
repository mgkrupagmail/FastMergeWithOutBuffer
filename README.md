# FastMergeWithOutBuffer
This project implements a new inplace and stable recursive merge function that merges two non-decreasing ranges into one without the use of a buffer. It is compared to the implementation of __merge_without_buffer found in GNU ISO C++ Library and timed to be faster. Three other variants of this function are defined but the fastest of them is denoted MergeWithOutBuffer() and is found in merge_without_buffer_standalone.h, which contains the primary algorithm of this project and which is a standalone C++ header file (i.e. no other files are needed).

In additional, unlike the GNU ISO C++ Library's std::__merge_without_buffer(), the two sorted sequences to be merged need NOT even be accessed by iterators of the same type.

There are two versions of this algorithm, called MergeWithOutBuffer1() and MergeWithOutBuffer2().

merge_without_buffer1.h       contains the implementation of MergeWithOutBuffer1() and includes example calls.
merge_without_buffer2.h       contains the implementation of MergeWithOutBuffer2() and includes example calls.
merge_without_buffer_common.h contains helper functions that are used by both merge_without_buffer1.h and merge_without_buffer2.h

The above files are the only ones that are needed in order to use these two algorithms. 
All of the other files in this project are used to:
(1) test the correctness of the algorithms (e.g. merge_test_correctness.h and merge_verify_stability.h), or to 
(2) time the algorithms and output relevant information (e.g. merge_time.h, time_merge_algorithms_class.h, gnu_merge_without_buffer.h, and mins_maxs_and_lambda.h). 

MergeWithOutBuffer2() often outperforms MergeWithOutBuffer1() if the sorted lists contain many repeated values. This happens, for example, if the lists contain 10,000 integers and they are all valued between 0 and 2000. Otherwise, usually either MergeWithOutBuffer1() outperforms MergeWithOutBuffer2() or there is little difference in their execution time. 

Each of these two algorithms has implementations specialized according to whether the iterator is a Random Access Interator (RAI), such as std::vector, or a Bidirectional Iterator (bi). Calls to MergeWithOutBuffer1() and MergeWithOutBuffer2() will automatically select the most appropriate implementation; specifically, if the iterator is a RAI then the RAI version will be selected and otherwise the Bidirectional Iterator version will be selected.
