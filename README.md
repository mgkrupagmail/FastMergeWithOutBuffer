# FastMergeWithOutBuffer
This project implements a new inplace and stable recursive merge function that merges two non-decreasing ranges into one without the use of a buffer. It is compared to the implementation of __merge_without_buffer found in GNU ISO C++ Library and timed to be faster. Three other variants of this function are defined but the fastest of them is denoted MergeWithOutBuffer() and is found in merge_without_buffer_standalone.h, which contains the primary algorithm of this project and which is a standalone C++ header file (i.e. no other files are needed).

In additional, unlike the GNU ISO C++ Library's std::__merge_without_buffer(), the two sorted sequences to be merged need NOT even be accessed by iterators of the same type.



******** Difference between the algorithms ********

There are two versions of this algorithm, called MergeWithOutBuffer1() and MergeWithOutBuffer2(). 
MergeWithOutBuffer2() often outperforms MergeWithOutBuffer1() if the sorted lists contain many repeated values. This happens, for example, if the lists contain 10,000 integers and they are all valued between 0 and 2000. Otherwise, usually either MergeWithOutBuffer1() outperforms MergeWithOutBuffer2() or there is little difference in their execution times. If you do not know which one to use then use MergeWithOutBuffer1(). 

Each of these two algorithms has implementations specialized according to whether the iterator is a Random Access Iterator (RAI), such as std::vector, or a Bidirectional Iterator (bi). Calls to MergeWithOutBuffer1() and MergeWithOutBuffer2() will automatically select the most appropriate implementation; specifically, if the iterator is a RAI then the RAI version will be selected and otherwise the Bidirectional Iterator version will be selected. 

MergeWithOutBuffer2() is an extension of the MergeWithOutBuffer1() algorithm. MergeWithOutBuffer1() contains the "minimum" needed in order to implement this new merge algorithm. If someone is trying to understand how these algorithms work, then they should start by reading the implementation of MergeWithOutBuffer1() (instead of MergeWithOutBuffer2()). The implementations have many commented out assert()s that can significantly help in understanding these algorithms. The RAI and Bidirectional Iterator implementations of these algorithms are nearly identical and it is recommended that the RAI version be studied first. The Bidirectional Iterator implementation is an altered version of the RAI implementation, changed so that Random Access operations were replaced with appropriate Bidirectional Iterator code. 



******** Implementations of the algorithms and overview of project files ********

merge_without_buffer1.h       contains the implementation of MergeWithOutBuffer1() and includes example calls.

merge_without_buffer2.h       contains the implementation of MergeWithOutBuffer2() and includes example calls.

merge_without_buffer_common.h contains helper functions that are used by both merge_without_buffer1.h and merge_without_buffer2.h

The above files are the only ones that are needed in order to use these two algorithms. 
All of the other files in this project are used to: 

(1) test the correctness of the algorithms (e.g. merge_test_correctness.h and merge_verify_stability.h), or to 

(2) time the algorithms and output relevant information (e.g. merge_time.h, time_merge_algorithms_class.h, gnu_merge_without_buffer.h, and mins_maxs_and_lambda.h). 



************ Overloads of the algorithms ************

There are serval overloads of MergeWithOutBuffer1(). There is always one overload that accepts (and another overload that does not accept) as its last argument a custom comparison operator (see https://en.cppreference.com/w/cpp/named_req/Compare for the requirements). If no custom comparison operator is passed then the default comparison operator is used. 
There is one version of MergeWithOutBuffer1() that accepts four iterators as arguments and another that accepts only three. The overload that accepts only three iterator arguments (start_left, start_right, one_past_end) assumes that the lists belong to the same container object and also that start_right is one past the last element of the left list (for example, use this version is you have a single std::vector<int> with values { 1, 2, 3, 4, 0, 1, 2 }). The overload that accepts four iterator arguments does not assume that the left list and the right list belong to the same container object; for example, the left (sorted) list and the right (sorted) list may belong to two different std::vector<int> objects. Everything that has just been said about MergeWithOutBuffer1()'s overloads can also be said about MergeWithOutBuffer2()'s overloads. 



******** How the algorithms are timed ********

The algorithms are applied to various data types (e.g. char, short, int, long, float, double, etc.) with values taken in various ranges (e.g. int values between x and y), and to various containers (e.g. std::vector, etc.), to various total lengths of data (e.g. merging lists with a combined total of 10^1 items, going up to 10^13 total items, with the total size increasing exponentially), to various lengths of individual sorted lists (e.g. it times merging sorted lists of equal sizes and also times merging sorted lists where one list many more items than the other). 

The same data is sorted by each of the algorithms. Moreover, each algorithm accesses the _same_ memory (e.g. If MergeWithOutBuffer1() and gnu__merge_without_buffer() are applied to an std::vector then the each algorithm will access the same memory); this is meant to help control for possible variations in data access times that depend on where the data is stored in memory. For each data being merged, each algorithm is also called at least once before it starts being timed so that both the algorithm and the data are loaded into cache (this can actually have a significant impact on timing). Each algorithm is called multiple times on each data to help reduce variance in its timing. 

Templates are used heavily in the timing algorithms in order to prevent the code from becoming too bloated (because of the many variations mentioned above) and also to guarantee that the exact same code is used to time each algorithm. 
