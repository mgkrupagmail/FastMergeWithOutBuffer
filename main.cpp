/*
 * main.cpp
 *
 *  Created on: Jun 29, 2017
 *      Author: Matthew Gregory Krupa
 *   Copyright: Matthew Gregory Krupa
 */
/*============================================================================
 *  Name        : main.cpp
 *  Author      : Matthew Gregory Krupa
 *  Version     : 1.0
 *  Copyright   : Any thing that's neither public domain nor already
 *                 owned by someone (as of March 24, 2017), I hereby copywrite
 *                 to the maximum extent allowed by law. :)
 *  Description : The primary algorithm of interest here would be
 *                MergeWithOutBuffer()
 *  In general, the larger the size of the vector, the bigger the difference in
 *   speed between my algorithms and the corresponding std:: algorithms.
 *============================================================================
 */

#define MALLOC_CHECK_ 2

#include <algorithm>
#include <cassert>
#include <chrono>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <vector>

#include "misc_helpers.h"

#include "merge_without_buffer_common.h"
#include "merge_without_buffer.h"
#include "merge_without_buffer2.h"

#include "TimingAndTestingCorrectness/main_timing_verifying_with_settings.h"

/*
To customize the testing and timing of these new algorithms, see the file:
main_timing_verifying_with_settings.h
located in the folder:
TimingAndTestingCorrectness/
and change the variable in the TestingOptions class.
*/

int main() {
  std::ios::sync_with_stdio(false); //Makes output with std::cout faster.

  TestingOptions to;
  //out_file_path_base will be the prefix used for all output files' names
  std::string out_file_path_base = GetPathThatWillPrefixAllOutputFileNames();
  int return_value = 0;
  Timings supreme_grand_total;

  to.should_print_to_file = false;

  std::vector<Timings> timings_vector;
  {//Test and time the algorithms when containers are std::vector
  to.SetContainerType(TestingOptions::ContainerTypeEnum::vector_type);
  to.out_file_path_base = out_file_path_base + to.GetContainerTypeString();
  return_value = TimeAndTestMergeFunctionsOnGivenContainerType<std::vector>(
      to, timings_vector);
  if (return_value != 0)//If an algorithm didn't not merge correctly
    return return_value;// then exit in failure.
  supreme_grand_total.AddTimesAndCountersFromOtherTimingsToThisObject(
                                                        timings_vector.back());
  supreme_grand_total.UpdateSmallestAndLargestRatiosOfTimesFromOtherTimingsMinMax(
                                                        timings_vector.back());
  }

  std::vector<Timings> timings_deque;
  {//Test and time the algorithms when containers are std::deque
  to.SetContainerType(TestingOptions::ContainerTypeEnum::deque_type);
  to.out_file_path_base = out_file_path_base + to.GetContainerTypeString();
  return_value = TimeAndTestMergeFunctionsOnGivenContainerType<std::deque>(
      to, timings_deque);
  if (return_value != 0)
    return return_value;
  supreme_grand_total.AddTimesAndCountersFromOtherTimingsToThisObject(
                                                         timings_deque.back());
  supreme_grand_total.UpdateSmallestAndLargestRatiosOfTimesFromOtherTimingsMinMax(
                                                         timings_deque.back());
  }

  std::vector<Timings> timings_list;
  {//Test and time the algorithms when containers are std::list
  to.SetContainerType(TestingOptions::ContainerTypeEnum::list_type);
  to.out_file_path_base = out_file_path_base + to.GetContainerTypeString();
  return_value = TimeAndTestMergeFunctionsOnGivenContainerType<std::list>(
      to, timings_list);
  if (return_value != 0)
    return return_value;
  supreme_grand_total.AddTimesAndCountersFromOtherTimingsToThisObject(
                                                          timings_list.back());
  supreme_grand_total.UpdateSmallestAndLargestRatiosOfTimesFromOtherTimingsMinMax(
                                                          timings_list.back());
  supreme_grand_total.FillInRatioOfTimesFromCurrentTimings();
  }

  to.out_file_path_base = out_file_path_base;
  std::ostringstream strm;
  strm << "Grand Total Times for ALL timings: \n\n";
  strm << supreme_grand_total.GetInfoString(1,
                                true,  //should_print_times
                                true,  //should_print_average_times
                                true,  //should_print_ratios_of_times
                                true,  //should_print_largest_ratios_of_times
                                true,  //should_print_smallest_ratios_of_times
                                true,  //should_include_percent_faster
                                true,  //should_include_size
                                true   //should_print_merge_function_categories
                                );
  if (to.also_print_to_std_cout) {
    std::cout << strm.str();
  }
  std::cout.flush();
  if (to.should_print_to_file) {
    std::string file_path_out = out_file_path_base + to.sep
        + std::string("GRAND-TOTAL") + to.ext_string;
    std::ofstream f(file_path_out, std::ios::out | std::ios::ate);
    f << strm.str();
    f.flush();
  }
  return return_value;
}

