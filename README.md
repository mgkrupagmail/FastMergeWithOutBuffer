# FastMergeWithOutBuffer
This project implements a new inplace and stable recursive merge algorithm that merges two non-decreasing ranges into a single non-decreasing range with_*out*_ the use of a buffer. 
It is compared to the implementation of `__merge_without_buffer` found in GNU ISO C++ Library and timed to be faster. 

The C++ implementations accept iterators as inputs and they can be used various C++ standard library containers such as `std::vector`, `std::deque`, `std::list`, and others. 
There are several overloads. 
Some overloads accept as input interators to two sorted lists that (potentially) belong to two different objects (for example, to two distinct `std::vector<int>` objects: `left_sorted_data` and `right_sorted_data`). 
There are also overloads that accept custom comparison operators. 

Example calls can be found in the comments at the top of `merge_without_buffer1.h` and `merge_without_buffer2.h`, which contain C++ implementations of two variations of this new algorithm. 



# Difference between the algorithms

There are two versions of this algorithm, called `MergeWithOutBuffer1()` and `MergeWithOutBuffer2()`. 
If you do not know which one to use then use `MergeWithOutBuffer1()`. 
`MergeWithOutBuffer2()` often outperforms `MergeWithOutBuffer1()` *if* the sorted lists contain many repeated values. This happens, for example, if the two lists contain a sum total of 10,000 `int`s and all values are between `0` and `2000`. 
If this is _not_ the case (i.e. if there are relatively few values that are repeated in the lists, which is often the case with floating-point data for instance) then there is usually little difference in their execution times, although `MergeWithOutBuffer1()` may sometimes outperform `MergeWithOutBuffer2()`. 
Because `MergeWithOutBuffer2()` performs more object comparisons, `MergeWithOutBuffer1()` is more likely to outperform it if the computational cost of comparing two objects is high enough and if the two lists have enough objects that the algorithms' initialization times do not dominate their total run times. 

Each of these two algorithms has implementations specialized according to whether the iterator is a Random Access Iterator (RAI), such as `std::vector`, or a Bidirectional Iterator (bi). 
Calls to `MergeWithOutBuffer1()` and `MergeWithOutBuffer2()` will automatically select the most appropriate implementation; specifically, if the iterator is a RAI then the RAI version will be selected and otherwise the Bidirectional Iterator version will be selected. 

`MergeWithOutBuffer2()` is an extension of the `MergeWithOutBuffer1()` algorithm. `MergeWithOutBuffer1()` contains the "minimum" needed in order to implement this new merge algorithm. 
If someone is trying to understand how these algorithms work, then they should start by reading the implementation of `MergeWithOutBuffer1()` (instead of `MergeWithOutBuffer2()`). 
The implementations have many commented out assert()s that can significantly help in understanding these algorithms. 
The RAI and Bidirectional Iterator implementations of these algorithms are nearly identical and it is recommended that the RAI version be studied first. 
The Bidirectional Iterator implementation is an altered version of the RAI implementation, changed by replacing all Random Access operations with equivalent Bidirectional Iterator code. 



# Implementations of the algorithms and overview of project files

Implementations of two variations of this new algorithm are found in the following files:

* `merge_without_buffer1.h`       contains the implementation of `MergeWithOutBuffer1()` and includes example calls.
* `merge_without_buffer2.h`       contains the implementation of `MergeWithOutBuffer2()` and includes example calls.
* `merge_without_buffer_common.h` contains helper functions that are used by both `merge_without_buffer1.h` and `merge_without_buffer2.h`.

The above files are the only ones that are needed in order to use these two algorithms. 
All of the other files in this project are used to: 

1. test the correctness of the algorithms (e.g. `merge_test_correctness.h` and `merge_verify_stability.h`), or to 
2. time the algorithms and output relevant information (e.g. `merge_time.h`, `time_merge_algorithms_class.h`, `gnu_merge_without_buffer.h`, and `mins_maxs_and_lambda.h`). The majority of code in most of these files is dedicated to recording timing data, computing statistics, and/or displaying correctly formatted text output.

In the future, the file `merge_without_buffer_standalone.h` will contain the primary algorithm of this project in a standalone C++ header file (i.e. no other files will be needed). 

# Overloads of the algorithms

There are serval overloads of `MergeWithOutBuffer1()`. There is always one overload that accepts (and another overload that does not accept) as its last argument a custom comparison operator (see https://en.cppreference.com/w/cpp/named_req/Compare for the requirements). 
If no custom comparison operator is passed then the default comparison operator is used. 

There is one version of `MergeWithOutBuffer1()` that accepts four iterators as arguments and another that accepts only three. 
The overload that accepts only three iterator arguments `(start_left, start_right, one_past_end)` assumes that the lists belong to the same container object and also that start_right is one past the last element of the left list (for example, use this version is you have a single `std::vector<int>` with values `{ 1, 2, 3, 4, 0, 1, 2 }`). 
The overload that accepts four iterator arguments does not assume that the left list and the right list belong to the same container object; for example, the left (sorted) list and the right (sorted) list may belong to two different `std::vector<int>` objects; currently, the two objects need to have the same iterator type (e.g. if one iterator has type `std::vector<int>::iterator` then so must the other). 
However, it is possible to implement these algorithms for two different iterator types because all that is required is that the iterators point to the same type of object; such an implementation was previously present in this project (before it was removed after a major update) and needs to be added back. 

Everything that has just been said about `MergeWithOutBuffer1()`'s overloads can also be said about `MergeWithOutBuffer2()`'s overloads. 



# How the algorithms are timed

The algorithms are applied to:

1. various containers (specifically, `std::vector`, `std::deque`, and `std::list`), and to 
2. various data types (e.g. `char`, `short`, `int`, `long`, `float`, `double`, `std::string`, etc.), and to 
3. various total lengths of data (e.g. merging lists with a combined total size of (`some_constant` multipled by) c^1, c^2, c^2, ..., c^20, ..., or more items, where `c` is a float constant (such as `1.7` or `2`); here the total size increases exponentially up to a maximum size of `TestingOptions::maximum_vec_size`. These and other testing parameters (including what information is outputted) are controlled by the member variables of `struct TestingOptions`, which is found in `main.cpp`.), and to 
4. various lengths of individual sorted lists (e.g. it times merging sorted lists of equal sizes and also times merging sorted lists where one list has an order of magnitude more items than the other list), 
5. with values taken in various ranges (e.g. `int` values between some `x` and `y`; this helps to control the number of repeated values in the lists: for example, if the number of possible values between `x` and `y` is small relative to the total number of items in the lists, then the randomly generated lists will necessarily have more repeated values). 

For each possible combination above: 
* Random data is generated multiple times and each aglorithm is timed on each instance of data (of course, the data is returned to its original initial state after each call so that the same data is sorted by each of the algorithms). 
* For each instance of data being merged, each algorithm is also called at least once before it starts being timed so that both the algorithm and the data are loaded into cache before timing starts (this can actually have a significant impact on timing). 
* Each algorithm is called multiple times on each instance of data to help reduce variance in its timing. 

Moreover, to the furthest extent possible (e.g. the following might not be guaranteed for non-primitive data types whose objects allocate memory on the heap), each algorithm accesses the _same memory_ (e.g. if `MergeWithOutBuffer1()` and `gnu__merge_without_buffer()` are applied to an `std::vector<int>` of size 10,000 whose data starts at location `x` in memory, then every algorithm will access the same memory that starts at location `x`). 
This is meant to help control for possible variations in data access times that depend on where the data is stored in memory. 

Templates are used heavily in the timing algorithms in order to prevent the code from becoming too bloated (because of the many variations mentioned above) and also to guarantee that the exact same code is used to time each algorithm. 
