/*
 * time_merge_algorithms_class.h
 *
 *  Created on: Jan 3, 2020
 *      Author: diagoras
 */

#ifndef SRC_TIME_MERGE_ALGORITHMS_CLASS_H_
#define SRC_TIME_MERGE_ALGORITHMS_CLASS_H_


#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

//These should correspond to the indices of the function in
// Timings::shared_merge_function_names_ in the obvious way.
enum TimingsIndex {
  StdMergeIndex = 0,
  StdInplaceMergeIndex,
  GnuMergeWithoutBufferIndex,
  MergeWithOutBuffer1Index,
  MergeWithOutBufferIndex
};

class Timings {
public:
  typedef std::chrono::nanoseconds time_units;
  typedef std::size_t SizeType;
  typedef long double RatioType;

  SizeType num_algorithms_;
  //Constraint: times_.size() == num_algorithms_
  //Note that each of the following vectors will have size num_algorithms_.
  std::vector<time_units> times_;
  std::vector<SizeType> number_of_times_merge_function_was_called_;
  //if is_merge_function_enabled_[i] == false then merge function i
  // will be completely ignored.
  const static std::vector<bool> shared_is_merge_function_enabled_;

  //default_num_algorithms_ should equal shared_merge_function_names_.size()
  static constexpr SizeType default_num_algorithms_ = 5;
  static std::vector<std::string> shared_merge_function_names_;

  static SizeType ratio_type_precision;
  //In all tables, rows represent denominators while
  // columns represent numerators.
  //Thus ratio_of_times_[i][j] = (times_[j])/(times_[i])
  std::vector<std::vector<RatioType>> ratio_of_times_;
  std::vector<std::vector<RatioType>> smallest_ratio_of_times_;
  std::vector<std::vector<RatioType>> largest_ratio_of_times_;
  std::vector<std::vector<SizeType>> size_when_smallest_ratio_was_encountered_;
  std::vector<std::vector<SizeType>> size_when_largest_ratio_was_encountered_;

  enum MergeFunctionBufferUseType {
    DoesNotUseBuffer = 0,
    UsesABufferIfAvailable,
    UsesABuffer
  };

  static std::vector<MergeFunctionBufferUseType>
                                        shared_merge_function_buffer_use_type_;
  static std::vector<std::string> shared_merge_function_buffer_use_type_string_;

  //default_time_units is the preferred time unit to see in the output.
  enum TimeUnit {
    UseDefaultTimeUnits = -1,
    Nanoseconds = 0,
    Microseconds,
    Milliseconds,
    Seconds
  };
  TimeUnit default_time_units_ = TimeUnit::Nanoseconds;


  static constexpr SizeType num_time_units_strings_ = 4;
  const static SizeType
          divsor_to_go_from_nano_to_other_time_units_[num_time_units_strings_];
  const static std::string time_units_strings_[num_time_units_strings_];
  bool max_string_length_of_given_time_units_[num_time_units_strings_]
       = { 1, 1, 1, 1 };
  bool times_units_to_print_[num_time_units_strings_] = {
      true, //nanoseconds
      true, //microseconds
      true, //milliseconds
      true  //seconds
  };


  //table_min_column_string_widths_[0] is the minimum with of every
  // string in the left-most column.
  std::vector<SizeType> table_min_column_string_widths_;
  static std::vector<SizeType> shared_table_min_column_string_widths_;

  static std::vector<std::string> shared_left_column_;
  static std::vector<std::string> shared_top_row_;

  template<class T>
  static inline auto CreateSquareVector(int size, T default_value) {
    return std::move(std::vector<std::vector<T>>(size, std::vector<T>(size, default_value)));
  }

  Timings(SizeType num_algorithms = default_num_algorithms_,
          bool default_value_of_is_merge_function_enabled = false) :
        num_algorithms_(num_algorithms),
        times_(std::vector<time_units>(num_algorithms, time_units(0))),
        number_of_times_merge_function_was_called_(std::vector<SizeType>(num_algorithms, 0)),
        ratio_of_times_(CreateSquareVector<RatioType>(num_algorithms, 0)),
        smallest_ratio_of_times_(CreateSquareVector<RatioType>(num_algorithms, 0)),
        largest_ratio_of_times_(CreateSquareVector<RatioType>(num_algorithms, 0)),
        size_when_smallest_ratio_was_encountered_(CreateSquareVector<SizeType>(num_algorithms, 0)),
        size_when_largest_ratio_was_encountered_(CreateSquareVector<SizeType>(num_algorithms, 0)),
        table_min_column_string_widths_(std::vector<SizeType>(1 + num_algorithms, 1))
        {
    assert(num_algorithms_ > 0);
    if (GetNumDefinedTopRowString() < num_algorithms_) {
      GetDefaultTopRowOfLabels();
    }
    if (GetNumDefinedLeftColumnString() < num_algorithms_) {
      GenerateDefaultLeftColumnOfAlgorithmNames();
    }
    InitializeRatioOfTimes();
    InitializeSmallestRatioOfTimes();
    InitializeLargestRatioOfTimes();
    return ;
  }


  std::string GetInfoString(SizeType divisor1 = 1,
                            bool should_print_times = true,
                            bool should_print_average_times = true,
                            bool should_print_ratios_of_times = true,
                            bool should_print_largest_ratios_of_times = true,
                            bool should_print_smallest_ratios_of_times = true,
                            bool should_include_percent_faster = true,
                            bool should_include_size = true,
                            bool should_print_merge_function_categories = true,
                            bool should_print_diagonal_elements = false,
                            SizeType num_lines_to_print_at_start = 1,
                            std::string start_line_to_print = std::string("*"),
                            SizeType num_new_lines_to_print_at_start = 2) {
    std::ostringstream strm;
    for (SizeType i = 0; i < num_new_lines_to_print_at_start; ++i)
      strm << '\n';
    for (SizeType num = 0; num < num_lines_to_print_at_start; ++num) {
      for (SizeType c = 0; c < 100; ++ c) {
        strm << start_line_to_print;
      }
      strm << '\n';
    }
    if (AreAllNumberOfTimesMergeFunctionWasCalledEqual(true)) {
      strm << "Total number of times each merge function was called = ";
      strm << GetMaxOfTotalNumberOfTimesEachMergeFunctionWasCalled()
           << '\n';
    } else {
      strm << "Each time was divided by the total number of times that"
             " merge function was called. This value was not the same for all"
             " merge functions. " << '\n';
    }

    if (should_print_times) {
      strm << "\n\nTimes:\n";
      strm << PrintTimesToString(divisor1, should_print_merge_function_categories);
    }
    if (should_print_average_times) {
      strm << "\nTimes Averages:\n";
      strm << PrintTimesToString(0, should_print_merge_function_categories);
      }
    if (should_print_ratios_of_times) {
      strm << "\nRatios of Times:\n";
      strm << ConstructTableStringFromRatiosUsingDefaultsWithPercentFaster(
          should_include_percent_faster, true, true,
          should_print_diagonal_elements);
    }
    if (should_print_largest_ratios_of_times) {
      strm << "\nSmallest Ratios of Times:\n";
      strm << ConstructTableStringFromMinRatiosUsingDefaultsWithPercentFaster(
          should_include_percent_faster, should_include_size,
          false, false, should_print_diagonal_elements);
    }
    if (should_print_largest_ratios_of_times) {
      strm << "\nLargest Ratios of Times:\n";
      strm << ConstructTableStringFromMaxRatiosUsingDefaultsWithPercentFaster(
          should_include_percent_faster, should_include_size,
          false, false, should_print_diagonal_elements);
    }
    strm << '\n';
    return strm.str();
  }

  std::string GetInfoString(SizeType divisor1,
                            const TestingOptions &to,
                            SizeType num_lines_to_print_at_start = 1,
                            std::string start_line_to_print = std::string("*"),
                            SizeType num_new_lines_to_print_at_start = 2) {
    return GetInfoString(divisor1,
                         to.should_print_times,
                         to.should_print_average_times,
                         to.should_print_ratios_of_times,
                         to.should_print_largest_ratios_of_times,
                         to.should_print_smallest_ratios_of_times,
                         to.should_include_percent_faster,
                         to.should_include_size,
                         to.should_print_merge_function_categories,
                         to.should_print_diagonal_elements,
                         num_lines_to_print_at_start,
                         start_line_to_print,
                         num_new_lines_to_print_at_start);
  }


  template<typename ValueType>
  std::string ConstructTableStringUsingDefaultsWithPercentFaster(
          std::vector<std::vector<ValueType>> &table_elements,
          bool should_include_percent_faster = true,
          bool should_update_table_min_column_string_widths = true,
          bool should_update_saved_table_min_col_widths_with_new_values = true,
          bool should_print_diagonal_elements = false,
          std::string left_column_separator = std::string(" | "),
          std::string column_separator = std::string(" ")) {
    auto string_table_elements
                  = StringifyTableWithPercentFaster<ValueType>(table_elements,
                                                should_include_percent_faster);
    std::string intro_string = ConstructTableGetIntroString(
                                          should_include_percent_faster, false);
    return intro_string + ConstructTableString(string_table_elements,
                      should_update_table_min_column_string_widths,
                      should_update_saved_table_min_col_widths_with_new_values,
                      should_print_diagonal_elements,
                      left_column_separator, column_separator);
  }

  template<typename ValueType>
  std::string ConstructTableStringUsingDefaultsWithSizesPercentFaster(
          const std::vector<std::vector<ValueType>> &table_elements,
          const std::vector<std::vector<SizeType>> &sizes,
          bool should_include_percent_faster = true,
          bool should_include_size = true,
          bool should_update_table_min_column_string_widths = true,
          bool should_update_saved_table_min_col_widths_with_new_values = true,
          bool should_print_diagonal_elements = false,
          std::string left_column_separator = std::string(" | "),
          std::string column_separator = std::string(" ")) {
    auto string_table_elements
                  = StringifyTableWithSizes<ValueType>(table_elements,
                      sizes, should_include_percent_faster, should_include_size);
    std::string intro_string = ConstructTableGetIntroString(
                           should_include_percent_faster, should_include_size);
    return intro_string + ConstructTableString(string_table_elements,
                      should_update_table_min_column_string_widths,
                      should_update_saved_table_min_col_widths_with_new_values,
                      should_print_diagonal_elements,
                      left_column_separator, column_separator);
  }


  std::string ConstructTableStringFromRatiosUsingDefaultsWithPercentFaster(
          bool should_include_percent_faster = true,
          bool should_update_table_min_column_string_widths = true,
          bool should_update_saved_table_min_col_widths_with_new_values = true,
          bool should_print_diagonal_elements = false,
          std::string left_column_separator = std::string(" | "),
          std::string column_separator = std::string(" ")) {
    std::string str;
    //str += std::string("Ratios of times:\n");
    str += ConstructTableStringUsingDefaultsWithPercentFaster<RatioType>(
                      ratio_of_times_,
                      should_include_percent_faster,
                      should_update_table_min_column_string_widths,
                      should_update_saved_table_min_col_widths_with_new_values,
                      should_print_diagonal_elements,
                      left_column_separator, column_separator);
    return str;
  }

  std::string ConstructTableStringFromMinRatiosUsingDefaultsWithPercentFaster(
          bool should_include_percent_faster = true,
          bool should_include_size = true,
          bool should_update_table_min_column_string_widths = true,
          bool should_update_saved_table_min_col_widths_with_new_values = true,
          bool should_print_diagonal_elements = false,
          std::string left_column_separator = std::string(" | "),
          std::string column_separator = std::string(" ")) {
    std::string str;
    //str += std::string("Smallest ratios of times encountered:\n");
    str += ConstructTableStringUsingDefaultsWithSizesPercentFaster<RatioType>(
                      smallest_ratio_of_times_,
                      size_when_smallest_ratio_was_encountered_,
                      should_include_percent_faster,
                      should_include_size,
                      should_update_table_min_column_string_widths,
                      should_update_saved_table_min_col_widths_with_new_values,
                      should_print_diagonal_elements,
                      left_column_separator, column_separator);
    return str;
  }

  std::string ConstructTableStringFromMaxRatiosUsingDefaultsWithPercentFaster(
          bool should_include_percent_faster = true,
          bool should_include_size = true,
          bool should_update_table_min_column_string_widths = true,
          bool should_update_saved_table_min_col_widths_with_new_values = true,
          bool should_print_diagonal_elements = false,
          std::string left_column_separator = std::string(" | "),
          std::string column_separator = std::string(" ")) {
    std::string str;
    //str += std::string("Largest ratios of times encountered:\n");
    str += ConstructTableStringUsingDefaultsWithSizesPercentFaster<RatioType>(
                      largest_ratio_of_times_,
                      size_when_largest_ratio_was_encountered_,
                      should_include_percent_faster,
                      should_include_size,
                      should_update_table_min_column_string_widths,
                      should_update_saved_table_min_col_widths_with_new_values,
                      should_print_diagonal_elements,
                      left_column_separator, column_separator);
    return str;
  }

  //Assumes that:
  // (1) table_elements.size() == num_algorithms_
  // (2) for all i, table_elements[i].size() == num_algorithms_
  //Every algorithm should have a ROW AND a COLUMN associated with it
  // in table_elements, regardless of whether or not it is enabled.
  //If GetIsMergeFunctionEnabled(i) == false then
  // row i will be ignored. Ditto for column i.
  std::vector<std::string> ConstructSingleTableColumnString(
          const std::vector<std::vector<std::string>> &table_elements,
          const SizeType column_number,
          std::string top_row_string = std::string(),
          bool should_print_diagonal_elements = false,
          std::string column_separator = std::string(" ")) {
    auto column_width = top_row_string.length();
    for (SizeType i = 0; i < table_elements.size(); ++i) {
      auto str_length = table_elements[i][column_number].length();
      if (str_length > column_width)
        column_width = str_length;
    }
    std::vector<std::string> vec;
    vec.reserve(table_elements.size() + 1);
    {
      std::ostringstream strm;
      strm << std::right << std::setw(column_width) << top_row_string
           << column_separator;
      vec.push_back(strm.str());
    }

    //Print the rest of the table, row by row
    for (SizeType row = 0; row < table_elements.size(); ++row) {
      if (GetIsMergeFunctionEnabled(row) == false)
        continue ;
      std::ostringstream strm;
      assert(table_elements[row][column_number].length() <= column_width);
      if (row == column_number && !should_print_diagonal_elements)
        strm << std::setw(column_width) << std::string();
      else
        strm << std::right << std::setw(column_width)
             << table_elements[row][column_number];
      strm << column_separator;
      vec.push_back(strm.str());
    }
    return vec;
  }

  std::vector<std::string> GetLeftColumnVector(std::string top_row = std::string()) {
    std::vector<std::string> vec;
    SizeType width = 1;
    for (SizeType i = 0; i < num_algorithms_; ++i) {
      auto str_length = GetLeftColumnString(i).length();
      if (str_length > width)
        width = str_length;
    }
    vec.reserve(num_algorithms_ + 1);
    {
      std::stringstream strm;
      strm << std::left << std::setw(width) << top_row;
      vec.emplace_back(strm.str());
    }
    for (SizeType i = 0; i < num_algorithms_; ++i) {
      std::stringstream strm;
      strm << std::left << std::setw(width) << GetLeftColumnString(i);
      vec.emplace_back(strm.str());
    }
    return vec;
  }

  //Assumes that:
  // (1) table_elements.size() == num_algorithms_
  // (2) for all i, table_elements[i].size() == num_algorithms_
  //Every algorithm should have a ROW AND a COLUMN associated with it
  // in table_elements, regardless of whether or not it is enabled.
  //If GetIsMergeFunctionEnabled(i) == false then
  // row i will be ignored. Ditto for column i.
  std::string ConstructTableString(
          const std::vector<std::vector<std::string>> &table_elements,
          bool should_update_table_min_column_string_widths = false,
          bool should_update_saved_table_min_col_widths_with_new_values = true,
          bool should_print_diagonal_elements = false,
          std::string left_column_separator = std::string(" | "),
          std::string column_separator = std::string(" ")) {
    assert(table_elements.size() > 0);
    SizeType num_data_columns = table_elements[0].size();
    auto min_col_width = table_min_column_string_widths_;
    while (min_col_width.size() < num_algorithms_ + 1) {
      min_col_width.push_back(GetMinColumnStringLength(min_col_width.size()));
    }
    for (SizeType i = 0; i < num_algorithms_ + 1; ++i) {
      min_col_width[i] = GetMinColumnStringLength(i);
    }
    for (SizeType i = 0; i < num_algorithms_ + 1; ++i) {
      auto str_length = GetTopRowString(i).length();
      if (str_length > min_col_width[i])
        min_col_width[i] = str_length;
    }
    {
      SizeType left_size = min_col_width[0];
      for (SizeType i = 0; i < num_algorithms_; ++i) {
        auto str_length = GetLeftColumnString(i).length();
        if (str_length > left_size)
          left_size = str_length;
      }
      min_col_width[0] = left_size;
    }
    for (SizeType j = 0; j < num_data_columns; ++j) {
      SizeType max_column_string_size = min_col_width[j + 1];
      for (SizeType i = 0; i < table_elements.size(); ++i) {
        auto str_length = table_elements[i][j].length();
        if (str_length > max_column_string_size)
          max_column_string_size = str_length;
      }
      min_col_width[j + 1] = max_column_string_size;
    }
    if (should_update_table_min_column_string_widths) {
      table_min_column_string_widths_ = min_col_width;
      if (should_update_saved_table_min_col_widths_with_new_values)
        shared_table_min_column_string_widths_ = min_col_width;
    }
    std::ostringstream strm;
    //Print the top row, column by column
    for (SizeType i = 0; i < num_algorithms_ + 1; ++i) {
      if (i != 0 && GetIsMergeFunctionEnabled(i - 1) == false)
        continue ;
      if (i == 0)
        strm << std::left;
      else
        strm << std::right;
      SizeType str_width = min_col_width[i];
      assert(GetTopRowString(i).length() <= str_width);
      strm << std::setw(str_width) << GetTopRowString(i);
      if (i == 0) {
        strm << left_column_separator;
      } else {
        if (i != num_algorithms_)
          strm << column_separator;
      }
    }
    SizeType string_length_of_each_row = strm.str().size(); //Length of the top row
    strm << '\n';
    for (SizeType i = 0; i < string_length_of_each_row; ++i) {
      strm << '-';
    }
    strm << '\n';

    //Print the rest of the table, row by row
    for (SizeType row = 0; row < table_elements.size(); ++row) {
      if (GetIsMergeFunctionEnabled(row) == false)
        continue ;
      //Print out the left-most element first
      SizeType str_width_left = min_col_width[0];
      strm << std::left << std::setw(str_width_left) << GetLeftColumnString(row);
      strm << left_column_separator;
      for (SizeType col = 0; col < num_data_columns; ++col) {
        if (GetIsMergeFunctionEnabled(col) == false)
          continue ;
        assert(col + 1 < table_min_column_string_widths_.size());
        SizeType str_width = min_col_width[col + 1];
        assert(table_elements[row][col].length() <= str_width);
        if (row == col && !should_print_diagonal_elements)
          strm << std::setw(str_width) << std::string();
        else
          strm << std::right << std::setw(str_width) << table_elements[row][col];
        if (col + 1 != num_data_columns)
          strm << column_separator;
      }
      strm << '\n';
    }
    return strm.str();
  }

  static std::string ConstructTableGetIntroString(
                                   bool should_include_percent_faster = true,
                                   bool should_include_size = true,
                                   bool should_include_newline_at_end = true) {
    std::ostringstream strm;
    strm << "Each entry is of the form: column-time/row-time";
    if (should_include_percent_faster)
      strm << " (percent faster of column-time than row-time%)";
    if (should_include_size)
      strm << " (size when it occurred)";
    strm << '.';
    if (should_include_newline_at_end)
      strm << '\n';
    return strm.str();
  }

  template<class ValueType>
  static std::vector<std::vector<std::string>> StringifyTable(
                            const std::vector<std::vector<ValueType>> &table) {
    SizeType precision = ratio_type_precision;
    SizeType num_rows = table.size();
    assert(num_rows > 0);
    SizeType num_cols = table[0].size();
    std::vector<std::vector<std::string>> str_table =
      std::vector<std::vector<std::string>>(num_rows,
                                            std::vector<std::string>(num_cols));
    for (SizeType i = 0; i < num_rows; ++i) {
      assert(num_cols <= table[i].size());
      for (SizeType j = 0; j < num_cols; ++j) {
        std::ostringstream strm;
        strm << std::setprecision(precision) << table[i][j];
        str_table[i][j] = strm.str();
      }
    }
    return str_table;
  }

  template<class ValueType>
  static std::vector<std::vector<std::string>> StringifyTableWithPercentFaster(
                            const std::vector<std::vector<ValueType>> &table,
                            bool should_include_percent_faster = true) {
    SizeType precision = ratio_type_precision;
    SizeType num_rows = table.size();
    assert(num_rows > 0);
    SizeType num_cols = table[0].size();
    std::vector<std::vector<std::string>> str_table =
      std::vector<std::vector<std::string>>(num_rows,
                                            std::vector<std::string>(num_cols));
    for (SizeType i = 0; i < num_rows; ++i) {
      assert(num_cols <= table[i].size());
      for (SizeType j = 0; j < num_cols; ++j) {
        std::ostringstream strm;
        strm << std::setprecision(precision) << std::setw(5) << table[i][j];
        if (should_include_percent_faster) {
          RatioType percent_faster
          = ((1.0l / static_cast<long double>(table[i][j])) - 1.0l) * 100.0l;
          strm << " (" << std::setprecision(precision) << std::setw(4)
               << percent_faster << "%)";
        }
        str_table[i][j] = strm.str();
      }
    }
    return str_table;
  }

  template<class ValueType>
  std::vector<std::vector<std::string>> StringifyTableWithSizes(
      const std::vector<std::vector<ValueType>> &table,
      const std::vector<std::vector<SizeType>> &num_times,
      bool should_include_percent_faster = true,
      bool should_include_size = true,
      bool only_include_num_times_if_not_all_values_are_equal = true) const {
    SizeType precision = ratio_type_precision;
    SizeType num_rows = table.size();
    assert(num_rows > 0);
    SizeType num_cols = table[0].size();
    std::vector<std::vector<std::string>> str_table =
      std::vector<std::vector<std::string>>(num_rows,
                                            std::vector<std::string>(num_cols));
    bool should_include_num_times_called = should_include_size;
    if (only_include_num_times_if_not_all_values_are_equal
        && should_include_size) {
      if (AreAllNumberOfTimesMergeFunctionWasCalledEqual2D(num_times))
        should_include_num_times_called = false;
    }
    //num_times_width will be used in std::setw() for num_times[i][j].
    std::vector<SizeType> num_times_width
                                    = std::vector<SizeType>(num_algorithms_, 1);
    if (should_include_num_times_called) {
      for (SizeType i = 0; i < num_rows; ++i) {
        assert(num_cols <= table[i].size());
        for (SizeType j = 0; j < num_cols; ++j) {
          SizeType num_str_length = std::to_string(num_times[i][j]).length();
          if (num_times_width[j] < num_str_length)
            num_times_width[j] = num_str_length;
        }
      }
    }
    for (SizeType i = 0; i < num_rows; ++i) {
      for (SizeType j = 0; j < num_cols; ++j) {
        std::ostringstream strm;
        strm << std::setprecision(precision) << std::setw(5) << table[i][j];
        if (should_include_percent_faster || should_include_num_times_called)
          strm << " ";
        if (should_include_percent_faster) {
          RatioType percent_faster =
              ((1.0l / static_cast<long double>(table[i][j])) - 1.0l) * 100.0l;
          strm << "(" << std::setprecision(precision) << std::setw(4)
               << percent_faster << "%)";
        }
        if (should_include_num_times_called) {
          auto num = num_times[i][j];
          strm << "(" << std::setw(num_times_width[j]) << num << ")";
        }
        str_table[i][j] = strm.str();
      }
    }
    return str_table;
  }

  std::vector<std::string> GenerateDefaultLeftColumnOfAlgorithmNames(
        bool should_include_counter = true,
        bool should_include_merge_function_names = true,
        bool should_replace_entry_0_in_table_min_column_string_widths_ = true) {
    assert(should_include_counter || should_include_merge_function_names); //Else there's nothing to print.
    std::vector<std::string> vec;
    int counter = 0;
    SizeType str_width = 0;
    if (should_replace_entry_0_in_table_min_column_string_widths_
                                          && GetNumDefinedTopRowString() > 0) {
      if (str_width < GetTopRowString(0).size())
        str_width = GetTopRowString(0).size();
    }
    for (SizeType i = 0; i < num_algorithms_; ++i) {
      if (!GetIsMergeFunctionEnabled(i)) {
        vec.push_back(std::string());
        continue ;
      }
      std::ostringstream strm;
      if (should_include_counter) {
        strm << '(' << counter << ')';
        if (should_include_merge_function_names)
          strm << " ";
      }
      if (should_include_merge_function_names)
        strm << GetMergeFunctionName(i);
      ++counter;
      vec.push_back(strm.str());
      if (should_replace_entry_0_in_table_min_column_string_widths_) {
        SizeType this_string_width = vec.back().length();
        if (str_width < this_string_width)
          str_width = this_string_width;
      }
    } //End for() loop.
    if (should_replace_entry_0_in_table_min_column_string_widths_) {
      if (shared_table_min_column_string_widths_.size() == 0)
        shared_table_min_column_string_widths_.push_back(str_width);
      else
        shared_table_min_column_string_widths_[0] = str_width;
      if (table_min_column_string_widths_.size() == 0)
        table_min_column_string_widths_ = std::vector<SizeType>({str_width});
      else
        table_min_column_string_widths_[0] = str_width;
    }
    assert(vec.size() == num_algorithms_);
    shared_left_column_ = vec;
    return vec;
  }

  std::vector<std::string> GetDefaultTopRowOfLabels(
      bool should_include_counter = true,
      bool should_include_merge_function_names = true,
      bool should_update_table_min_column_string_widths = false,
      std::string left_most_string_of_top_row = std::string("(Denom. below, numer. right)")) {
    std::vector<std::string> vec;
    vec.push_back(left_most_string_of_top_row); //Element above the left-most column
    int counter = 0;
    for (SizeType i = 0; i < num_algorithms_; ++i) {
      if (!GetIsMergeFunctionEnabled(i)) {
        vec.push_back(std::string());
        continue ;
      }
      std::ostringstream strm;
      if (should_include_counter) {
        strm << '(' << counter << ')';
        if (should_include_merge_function_names)
          strm << " ";
      }
      if (should_include_merge_function_names)
        strm << GetMergeFunctionName(i);
      ++counter;
      vec.push_back(strm.str());
      if (should_update_table_min_column_string_widths) {
        SizeType str_width = vec.back().length();
        if (shared_table_min_column_string_widths_.size() < i)
          shared_table_min_column_string_widths_.push_back(str_width);
        else {
          if (str_width > table_min_column_string_widths_[i])
            shared_table_min_column_string_widths_[i] = str_width;
        }
        if (table_min_column_string_widths_.size() < i)
          table_min_column_string_widths_.push_back(str_width);
        else {
          if (str_width > table_min_column_string_widths_[i])
            table_min_column_string_widths_[i] = str_width;
        }
      }
    }
    shared_top_row_ = vec;
    return vec;
  }

  void UpdateSmallestAndLargestRatiosOfTimesFromOtherTimings(Timings &o,
          SizeType size_for_these_ratios,
          bool ignore_0_ratios = true, //If true, skips all ratios that are <= 0
          bool ignore_non_finite_ratios = true,
          bool ignore_NaN_ratios = true,
          bool ignore_not_enabled = true,
          bool ignore_diagonal_elements = true) {
    UpdateSmallestRatioOfTimes(o.smallest_ratio_of_times_,
                               size_for_these_ratios,
                               ignore_0_ratios, ignore_non_finite_ratios,
                               ignore_NaN_ratios, ignore_not_enabled,
                               ignore_diagonal_elements);
    UpdateLargestRatioOfTimes(o.largest_ratio_of_times_, size_for_these_ratios,
                              ignore_0_ratios, ignore_non_finite_ratios,
                              ignore_NaN_ratios, ignore_not_enabled,
                              ignore_diagonal_elements);
    return ;
  }

  void UpdateSmallestAndLargestRatiosOfTimesFromOtherTimingsMinMax(Timings &o,
          bool ignore_0_ratios = true, //If true, skips all ratios that are <= 0
          bool ignore_non_finite_ratios = true,
          bool ignore_NaN_ratios = true,
          bool ignore_not_enabled = true,
          bool ignore_diagonal_elements = true) {
    UpdateSmallestRatioOfTimesFromOtherMins(o, ignore_0_ratios,
                               ignore_non_finite_ratios,
                               ignore_NaN_ratios, ignore_not_enabled,
                               ignore_diagonal_elements);
    UpdateLargestRatioOfTimesFromOtherMaxs(o, ignore_0_ratios,
                              ignore_non_finite_ratios,
                              ignore_NaN_ratios, ignore_not_enabled,
                              ignore_diagonal_elements);
    return ;
  }


  void AddTimesAndCountersFromOtherTimingsToThisObject(Timings &o,
          bool ignore_if_num_times_called_is_0 = true,
          bool ignore_not_enabled = true) {
    for (SizeType i = 0; i < num_algorithms_; ++i) {
      if (GetIsMergeFunctionEnabled(i) == false && ignore_not_enabled)
        continue ;
      assert(i < number_of_times_merge_function_was_called_.size());
      auto num_times_called = o.GetNumTimesEachMergeFunctionWasCalled(i);
      if (num_times_called == 0 && ignore_if_num_times_called_is_0)
        continue ;
      times_[i] += o.times_[i];
      number_of_times_merge_function_was_called_[i] += num_times_called;
    }
    return ;
  }

  void UpdateLargestAndSmallestRatioOfTimesUsingCurrentRatios(
                              SizeType size_for_these_ratios,
                              bool ignore_0_ratios = true,
                              bool ignore_non_finite_ratios = true,
                              bool ignore_NaN_ratios = true,
                              bool ignore_not_enabled = true,
                              bool ignore_diagonal_elements = true) {
    UpdateLargestRatioOfTimes(ratio_of_times_, size_for_these_ratios,
                              ignore_0_ratios, ignore_non_finite_ratios,
                              ignore_NaN_ratios, ignore_not_enabled,
                              ignore_diagonal_elements);
    UpdateSmallestRatioOfTimes(ratio_of_times_, size_for_these_ratios,
                               ignore_0_ratios, ignore_non_finite_ratios,
                               ignore_NaN_ratios, ignore_not_enabled,
                               ignore_diagonal_elements);
  }


  //Updates largest_ratio_of_times_ (and possibly also
  // size_when_largest_ratio_was_encountered_) backed on the given ratios.
  //Doesn't update size_when_largest_ratio_was_encountered_ if
  // size_for_these_ratios == 0.
  //If ignore_0_ratios == true then ratios that are <= 0 are skipped.
  void UpdateLargestRatioOfTimes(
                              const std::vector<std::vector<RatioType>> &ratios,
                              SizeType size_for_these_ratios,
                              bool ignore_0_ratios = true,
                              bool ignore_non_finite_ratios = true,
                              bool ignore_NaN_ratios = true,
                              bool ignore_not_enabled = true,
                              bool ignore_diagonal_elements = true) {
    bool should_update_size_when_later_ratio_was_encountered =
                                                  (size_for_these_ratios > 0);
    auto &cur_ratios = largest_ratio_of_times_;
    SizeType num_rows = std::min(cur_ratios.size(), ratios.size());
    for (SizeType i = 0; i < num_rows; ++i) {
      if (!GetIsMergeFunctionEnabled(i) && ignore_not_enabled)
        continue ;
      auto &ratios_row_i = ratios[i];
      SizeType num_cols = std::min(ratios_row_i.size(), cur_ratios[i].size());
      for (SizeType j = 0; j < num_cols; ++j) {
        if (!GetIsMergeFunctionEnabled(j) && ignore_not_enabled)
          continue ;
        if (i == j && ignore_diagonal_elements)
          continue ;
        auto ratio_i_j = ratios_row_i[j];
        if (ratio_i_j <= static_cast<RatioType>(0) && ignore_0_ratios)
          continue ;
        if (std::isnan(ratio_i_j) && ignore_NaN_ratios)
          continue ;
        if (!std::isfinite(ratio_i_j) && ignore_non_finite_ratios)
          continue ;
        if (ratio_i_j > cur_ratios[i][j]) {
          cur_ratios[i][j] = ratio_i_j;
          if (!should_update_size_when_later_ratio_was_encountered)
            continue ;
          size_when_largest_ratio_was_encountered_[i][j]
                                                        = size_for_these_ratios;
        }
      } //End inner for() loop
    } //End outer for() loop
    return ;
  }

  void UpdateLargestRatioOfTimesFromOtherMaxs(Timings &o,
                                 bool ignore_0_ratios = true,
                                 bool ignore_non_finite_ratios = true,
                                 bool ignore_NaN_ratios = true,
                                 bool ignore_not_enabled = true,
                                 bool ignore_diagonal_elements = true) {
    const std::vector<std::vector<RatioType>> &ratios = o.largest_ratio_of_times_;
    auto &cur_ratios = largest_ratio_of_times_;
    SizeType num_rows = std::min(cur_ratios.size(), ratios.size());
    for (SizeType i = 0; i < num_rows; ++i) {
      if (!GetIsMergeFunctionEnabled(i) && ignore_not_enabled)
        continue ;
      auto &ratios_row_i = ratios[i];
      SizeType num_cols = std::min(ratios_row_i.size(), cur_ratios[i].size());
      for (SizeType j = 0; j < num_cols; ++j) {
        if (!GetIsMergeFunctionEnabled(j) && ignore_not_enabled)
          continue ;
        if (i == j && ignore_diagonal_elements)
          continue ;
        auto ratio_i_j = ratios_row_i[j];
        if (ratio_i_j <= static_cast<RatioType>(0) && ignore_0_ratios)
          continue ;
        if (std::isnan(ratio_i_j) && ignore_NaN_ratios)
          continue ;
        if (!std::isfinite(ratio_i_j) && ignore_non_finite_ratios)
          continue ;
        if (ratio_i_j > cur_ratios[i][j]) {
          cur_ratios[i][j] = ratio_i_j;
          size_when_largest_ratio_was_encountered_[i][j]
                            = o.size_when_largest_ratio_was_encountered_[i][j];
        }
      } //End inner for() loop
    } //End outer for() loop
    return ;
  }

  //Updates smallest_ratio_of_times_ (and possibly also
  // size_when_smallest_ratio_was_encountered_) backed on the given ratios.
  //Doesn't update size_when_smallest_ratio_was_encountered_ if
  // size_for_these_ratios == 0.
  //If ignore_0_ratios == true then ratios that are <= 0 are skipped.
  void UpdateSmallestRatioOfTimes(
                              const std::vector<std::vector<RatioType>> &ratios,
                              SizeType size_for_these_ratios,
                              bool ignore_0_ratios = true,
                              bool ignore_non_finite_ratios = true,
                              bool ignore_NaN_ratios = true,
                              bool ignore_not_enabled = true,
                              bool ignore_diagonal_elements = true) {
    bool should_update_size_when_smaller_ratio_was_encountered =
                                                  (size_for_these_ratios > 0);
    auto &cur_ratios = smallest_ratio_of_times_;
    SizeType num_rows = std::min(cur_ratios.size(), ratios.size());
    for (SizeType i = 0; i < num_rows; ++i) {
      if (!GetIsMergeFunctionEnabled(i) && ignore_not_enabled)
        continue ;
      auto &ratios_row_i = ratios[i];
      SizeType num_cols = std::min(ratios_row_i.size(), cur_ratios[i].size());
      for (SizeType j = 0; j < num_cols; ++j) {
        if (!GetIsMergeFunctionEnabled(j) && ignore_not_enabled)
          continue ;
        if (i == j && ignore_diagonal_elements)
          continue ;
        auto ratio_i_j = ratios_row_i[j];
        if (ratio_i_j <= static_cast<RatioType>(0) && ignore_0_ratios)
          continue ;
        if (std::isnan(ratio_i_j) && ignore_NaN_ratios)
          continue ;
        if (!std::isfinite(ratio_i_j) && ignore_non_finite_ratios)
          continue ;
        if (ratio_i_j < cur_ratios[i][j]) {
          cur_ratios[i][j] = ratio_i_j;
          if (!should_update_size_when_smaller_ratio_was_encountered)
            continue ;
          size_when_smallest_ratio_was_encountered_[i][j]
                                                        = size_for_these_ratios;
        }
      } //End inner for() loop
    } //End outer for() loop
    return ;
  }

  //Updates smallest_ratio_of_times_ (and possibly also
  // size_when_smallest_ratio_was_encountered_) backed on the given ratios.
  //Doesn't update size_when_smallest_ratio_was_encountered_ if
  // size_for_these_ratios == 0.
  //If ignore_0_ratios == true then ratios that are <= 0 are skipped.
  void UpdateSmallestRatioOfTimesFromOtherMins(Timings &o,
                                  bool ignore_0_ratios = true,
                                  bool ignore_non_finite_ratios = true,
                                  bool ignore_NaN_ratios = true,
                                  bool ignore_not_enabled = true,
                                  bool ignore_diagonal_elements = true) {
    const std::vector<std::vector<RatioType>> &ratios = o.smallest_ratio_of_times_;
    auto &cur_ratios = smallest_ratio_of_times_;
    SizeType num_rows = std::min(cur_ratios.size(), ratios.size());
    for (SizeType i = 0; i < num_rows; ++i) {
      if (!GetIsMergeFunctionEnabled(i) && ignore_not_enabled)
        continue ;
      auto &ratios_row_i = ratios[i];
      SizeType num_cols = std::min(ratios_row_i.size(), cur_ratios[i].size());
      for (SizeType j = 0; j < num_cols; ++j) {
        if (!GetIsMergeFunctionEnabled(j) && ignore_not_enabled)
          continue ;
        if (i == j && ignore_diagonal_elements)
          continue ;
        auto ratio_i_j = ratios_row_i[j];
        if (ratio_i_j <= static_cast<RatioType>(0) && ignore_0_ratios)
          continue ;
        if (std::isnan(ratio_i_j) && ignore_NaN_ratios)
          continue ;
        if (!std::isfinite(ratio_i_j) && ignore_non_finite_ratios)
          continue ;
        if (ratio_i_j < cur_ratios[i][j]) {
          cur_ratios[i][j] = ratio_i_j;
          size_when_smallest_ratio_was_encountered_[i][j]
                            = o.size_when_smallest_ratio_was_encountered_[i][j];
        }
      } //End inner for() loop
    } //End outer for() loop
    return ;
  }


  void FillInRatioOfTimesFromCurrentTimings(bool dont_divide_by_zero = false,
                                            bool ignore_not_enabled = true) {
    bool are_all_num_times_called_equal =
                          AreAllNumberOfTimesMergeFunctionWasCalledEqual(true);

    if (!are_all_num_times_called_equal) {
      std::cout << __LINE__ << " WARNING: are_all_num_times_called_equal"
                               " == false.\n";
    }
    for (SizeType i = 0; i < num_algorithms_; ++i) {
      if (!GetIsMergeFunctionEnabled(i) && ignore_not_enabled)
        continue ;
      auto time_i = times_[i].count(); //time_i is the denominator.
      if (time_i == 0 && dont_divide_by_zero) {
        continue ;
      }
      for (SizeType j = 0; j < num_algorithms_; ++j) {
        if (!GetIsMergeFunctionEnabled(j) && ignore_not_enabled)
          continue ;
        if (j == i) {
          ratio_of_times_[i][j] = static_cast<RatioType>(1);
          continue ;
        }
        auto time_j = times_[j].count(); //time_j is the numerator
        RatioType ratio_i_j = static_cast<RatioType>(time_j) /
                              static_cast<RatioType>(time_i);
        if (!are_all_num_times_called_equal) {
          auto num_times_called_i = GetNumTimesEachMergeFunctionWasCalled(i);
          auto num_times_called_j = GetNumTimesEachMergeFunctionWasCalled(j);
          if (num_times_called_i != num_times_called_j) {
            long double ratio_num_times_called =
                static_cast<long double>(num_times_called_j) /
                static_cast<long double>(num_times_called_i);
            ratio_i_j *= ratio_num_times_called;
            //i.e. ratio_i_j = (time_j / num_times_called_j) /
            //                 (time_i / num_times_called_i)
          }
        }
        ratio_of_times_[i][j] = ratio_i_j;
      } //End inner for() loop
    }
    return ;
  }

  //Checks if all values of number_of_times_merge_function_was_called_[i]
  // are equal.
  bool AreAllNumberOfTimesMergeFunctionWasCalledEqual2D(
                                const std::vector<std::vector<SizeType>> &vec,
                                bool ignore_not_enabled = true) const {
    SizeType first_enabled = 0;
    for ( ; first_enabled < num_algorithms_; ++first_enabled) {
      if (!GetIsMergeFunctionEnabled(first_enabled) && ignore_not_enabled)
        continue ;
      break ;
    }
    assert(first_enabled < num_algorithms_);
    auto i = first_enabled;
    SizeType common_number = 0;
    for (SizeType j = i + 1 ; j < num_algorithms_; ++j) {
      if (!GetIsMergeFunctionEnabled(j) && ignore_not_enabled)
        continue ;
      common_number = vec[i][j];
      break ;
    }
    for ( ; i < num_algorithms_; ++i) {
      if (!GetIsMergeFunctionEnabled(i) && ignore_not_enabled)
        continue ;
      for (SizeType j = first_enabled ; j < num_algorithms_; ++j) {
        if (!GetIsMergeFunctionEnabled(j) && ignore_not_enabled)
          continue ;
        if (i == j)
          continue ;
        if (common_number != vec[i][j])
          return false;
      }
    }
    return true;
  }

  //Checks if all values of number_of_times_merge_function_was_called_[i]
  // are equal.
  bool AreAllNumberOfTimesMergeFunctionWasCalledEqual(
                                        bool ignore_not_enabled = true) const {
    SizeType i = 0;
    for ( ; i < num_algorithms_; ++i) {
      if (!GetIsMergeFunctionEnabled(i) && ignore_not_enabled)
        continue ;
      break ;
    }
    assert(i < num_algorithms_);
    SizeType common_number = GetNumTimesEachMergeFunctionWasCalled(i);
    for ( ; i < num_algorithms_; ++i) {
      if (!GetIsMergeFunctionEnabled(i) && ignore_not_enabled)
        continue ;
      if (common_number != GetNumTimesEachMergeFunctionWasCalled(i))
        return false;
    }
    return true;
  }

  template<typename T>
  static void Initialize2DVector(std::vector<std::vector<T>> &m,
                                 T default_value = static_cast<T>(0),
                                 bool use_different_value_for_diagonal = true,
                                 T default_diagonal_value = static_cast<T>(1)) {
    if (!use_different_value_for_diagonal)
      default_diagonal_value = default_value;
    for (SizeType i = 0; i < m.size(); ++i) {
      for (SizeType j = 0; j < m[i].size(); ++j) {
        if (i == j) {
          m[i][j] = default_diagonal_value;
        } else {
          m[i][j] = default_value;
        }
      }
    }
    return ;
  }

  void InitializeRatioOfTimes() {
    for (SizeType i = 0; i < ratio_of_times_.size(); ++i) {
      SizeType row_size = ratio_of_times_[i].size();
      for (SizeType j = 0; j < row_size; ++j) {
        if (i == j) {
          ratio_of_times_[i][j] = 1;
        } else {
          ratio_of_times_[i][j] = -1;
        }
      }
    }
    return ;
  }

  void InitializeSmallestRatioOfTimes() {
    for (SizeType i = 0; i < ratio_of_times_.size(); ++i) {
      SizeType row_size = ratio_of_times_[i].size();
      for (SizeType j = 0; j < row_size; ++j) {
        if (i == j) {
          smallest_ratio_of_times_[i][j] = 1;
        } else {
          smallest_ratio_of_times_[i][j] = std::numeric_limits<RatioType>::max();
        }
        size_when_smallest_ratio_was_encountered_[i][i] = 0;
      }
    }
    return ;
  }

  void InitializeLargestRatioOfTimes() {
    for (SizeType i = 0; i < ratio_of_times_.size(); ++i) {
      SizeType row_size = ratio_of_times_[i].size();
      for (SizeType j = 0; j < row_size; ++j) {
        if (i == j) {
          largest_ratio_of_times_[i][j] = 1;
        } else {
          largest_ratio_of_times_[i][j] = -1;
        }
        size_when_largest_ratio_was_encountered_[i][i] = 0;
      } //End inner for() loop
    }
    return ;
  }

  template<typename T> static std::size_t GetStringWidth(T value) {
    return std::to_string(value).length();
  }


  std::string PrintTimesToString(std::size_t divisor,
      bool should_print_merge_function_categories = true,
      bool should_print_divisor_used = true,
      bool should_also_print_0_times = true) {
    //First, find the maximum string length of the times (in nanoseconds)
    SizeType max_nano_str_length = 1;
    SizeType max_function_name_str_length = 1;
    auto divisor_to_use = divisor;
    SizeType first_enabled = 0;
    for ( ; first_enabled < num_algorithms_; ++first_enabled) {
      if (GetIsMergeFunctionEnabled(first_enabled) == false)
        continue ;
      break ;
    }
    assert(first_enabled < num_algorithms_);
    MergeFunctionBufferUseType last_buffer_use_type
                              = GetMergeFunctionBufferUsageType(first_enabled);
    for (SizeType i = first_enabled; i < num_algorithms_; ++i) {
      if (GetIsMergeFunctionEnabled(i) == false)
        continue ;
      auto nano_time = times_[i];
      if (nano_time.count() == 0 && !should_also_print_0_times)
        continue ;
      if (divisor == 0) {
        divisor_to_use = GetNumTimesEachMergeFunctionWasCalled(i);
        if (divisor_to_use == 0)
          divisor_to_use = 1;
      }
      std::size_t nano = static_cast<std::size_t>(nano_time.count() /
                                                                divisor_to_use);
      SizeType str_length_i = std::to_string(nano).length();
      if (max_nano_str_length < str_length_i)
        max_nano_str_length = str_length_i;
      if (should_print_merge_function_categories) {
        //auto merge_function_cat = GetMergeFunctionBufferUsageType(i);
        //if (i == first_enabled || merge_function_cat != last_buffer_use_type) {
        //  last_buffer_use_type = merge_function_cat;
        //  SizeType cat_string_size = GetMergeFunctionBufferUsageTypeString(
        //                                         merge_function_cat).length();
        //  if (cat_string_size > max_function_name_str_length)
        //    max_function_name_str_length = cat_string_size;
        //}
      }
      SizeType merge_function_str_length_i = GetMergeFunctionName(i).length();
      if (max_function_name_str_length < merge_function_str_length_i)
        max_function_name_str_length = merge_function_str_length_i;
    }
    //Find the largest integer last_time_unit_to_print such that
    // we should print time unit number last_time_unit_to_print - 1.
    SizeType last_time_unit_to_print = 4;
    while (last_time_unit_to_print > 0 &&
        !times_units_to_print_[last_time_unit_to_print - 1]) {
      --last_time_unit_to_print;
    }
    assert(last_time_unit_to_print > 0);//Else the caller wants to print nothing
    std::ostringstream strm;
    bool are_num_times_called_all_equal =
                          AreAllNumberOfTimesMergeFunctionWasCalledEqual(true);
    bool do_all_have_common_divisor =
                divisor > 0 || (divisor == 0 && are_num_times_called_all_equal);
    if (should_print_divisor_used) {
      if (do_all_have_common_divisor)
        strm << "Each time was divided by: divisor = "
             << divisor_to_use << '\n';
      else
        strm << "Each time was divided by the total number of times that"
               " merge function was called. This value was not the same for all"
               " merge functions. " << '\n';
    }
    last_buffer_use_type = GetMergeFunctionBufferUsageType(first_enabled);
    for (SizeType i = first_enabled; i < num_algorithms_; ++i) {
      if (GetIsMergeFunctionEnabled(i) == false)
        continue ;
      auto nano_time = times_[i];
      if (nano_time.count() == 0 && !should_also_print_0_times)
        continue ;
      if (should_print_merge_function_categories) {
        auto merge_function_cat = GetMergeFunctionBufferUsageType(i);
        if (i == first_enabled || merge_function_cat != last_buffer_use_type) {
          last_buffer_use_type = merge_function_cat;
          std::string cat_string = GetMergeFunctionBufferUsageTypeString(
                                     static_cast<SizeType>(merge_function_cat));
          strm << cat_string << '\n';
        }
      }
      strm << std::left << std::setw(max_function_name_str_length)
           << GetMergeFunctionName(i);
      strm << " \t";
      auto divisor_to_use = divisor;
      if (divisor_to_use == 0) {
        divisor_to_use = GetNumTimesEachMergeFunctionWasCalled(i);
        if (divisor_to_use == 0)
          divisor_to_use = 1;
      }
      std::size_t nano = static_cast<std::size_t>(nano_time.count() /
                                                  divisor_to_use);
      auto cur_time_count = nano;
      auto width = max_nano_str_length;
      for (SizeType j = 0; j < last_time_unit_to_print; ++j) {
        if (times_units_to_print_[j]) {
          strm << " = ";
          strm << std::right << std::setw(width)
               << cur_time_count << " " << GetTimeUnitString(j);
        }
        width = (width > 3) ? width - 3 : 1;
        cur_time_count /= 1000;
      }
      if (!do_all_have_common_divisor && should_print_divisor_used) {
        strm << " (divisor=" << divisor_to_use << ")";
      }
      strm << '\n';
    }
    return strm.str();
  }

  //Given a tume nano_time (in nanoseconds), it divides the time by divisor
  // and returns a string describing the resulting quotient
  // in nanoseconds, microseconds, milliseconds, and seconds.
  std::string GetTimeEqualityStr(time_units nano_time, std::size_t divisor) {
    std::ostringstream strm;
    if (divisor == 0)
      divisor = 1;
    std::size_t nano = static_cast<std::size_t>(nano_time.count() / divisor);
    auto nano_str_length = GetStringWidth(nano);
    static std::size_t max_nano_str_width = 1;
    if (nano_str_length > max_nano_str_width)
      max_nano_str_width = nano_str_length;
    for (SizeType i = 0; i < 4; ++i) {
      if (times_units_to_print_[i]) {
        strm << std::setw(max_nano_str_width)
             << (nano / divsor_to_go_from_nano_to_other_time_units_[i])
             << " " << GetTimeUnitString(i);
        if (i != 3)
          strm << " = ";
      }
    }
    return strm.str();
  }

  std::string GetLeftColumnString(SizeType index) const {
    if (index < shared_left_column_.size())
      return shared_left_column_[index];
    return std::string("\n\nERROR in GetLeftColumnString(): index = ")
     + std::to_string(index) + std::string(" is invalid.\n\n");
  }

  inline SizeType GetNumDefinedLeftColumnString() const {
    return shared_left_column_.size();
  }

  SizeType GetMaxOfTotalNumberOfTimesEachMergeFunctionWasCalled() {
    SizeType max = 0;
    for (SizeType i = 0; i < number_of_times_merge_function_was_called_.size();
                                                                          ++i) {
      if (GetIsMergeFunctionEnabled(i) == false)
        continue ;
      auto value = number_of_times_merge_function_was_called_[i];
      if (value > max)
        max = value;
    }
    return max;
  }

  bool GetIsMergeFunctionEnabled(SizeType index) const {
    if (index < shared_is_merge_function_enabled_.size())
      return shared_is_merge_function_enabled_[index];
    else
      return false;
  }

  MergeFunctionBufferUseType GetMergeFunctionBufferUsageType(SizeType index)
                                                                        const {
    if (index < shared_merge_function_buffer_use_type_.size())
      return shared_merge_function_buffer_use_type_[index];
    return MergeFunctionBufferUseType::DoesNotUseBuffer;
  }

  std::string GetMergeFunctionBufferUsageTypeString(SizeType index)
                                                                        const {
    if (index < shared_merge_function_buffer_use_type_string_.size())
      return shared_merge_function_buffer_use_type_string_[index];
    return std::string("Error in GetMergeFunctionBufferUsageTypeString(): "
                       " index = " + std::to_string(index)
           + std::string(" invalid.\n"));
  }

  std::string GetMergeFunctionName(SizeType index) const {
    if (index < shared_merge_function_names_.size())
      return shared_merge_function_names_[index];
    return std::string("\n\nERROR in GetMergeFunctionName(): index = ")
     + std::to_string(index) + std::string(" is invalid.\n\n");
  }

  SizeType GetMinColumnStringLength(SizeType index) const {
    if (index < table_min_column_string_widths_.size())
      return table_min_column_string_widths_[index];
    else if (index < shared_table_min_column_string_widths_.size())
      return shared_table_min_column_string_widths_[index];
    return 3;
  }

  SizeType GetNumTimesEachMergeFunctionWasCalled(SizeType index) const {
    assert(index < number_of_times_merge_function_was_called_.size());
    return number_of_times_merge_function_was_called_[index];
  }

  std::string GetTimeUnitString(SizeType index) const {
    if (index < num_time_units_strings_)
      return time_units_strings_[index];
    return std::string("\n\nERROR in GetTimeUnitString(): index = ")
     + std::to_string(index) + std::string(" is invalid.\n\n");
  }

  std::string GetTopRowString(SizeType index) const {
    if (index < shared_top_row_.size())
      return shared_top_row_[index];
    return std::string("\n\nERROR in GetTopRowString(): index = ")
     + std::to_string(index) + std::string(" is invalid.\n\n");
  }

  inline SizeType GetNumDefinedTopRowString() const {
    return shared_top_row_.size();
  }
};

Timings::SizeType Timings::ratio_type_precision = 4;

const std::vector<bool> Timings::shared_is_merge_function_enabled_ = std::vector<bool>({
      true,  //std::merge
      true,  //std::inplace_merge
      true,  //gnu_merge_without_buffer
      true,  //MergeWithOutBuffer1
      true   //MergeWithOutBuffer
  });

std::vector<std::string> Timings::shared_merge_function_names_ = {
      std::string("std::merge"),
      std::string("std::inplace_merge"),
      std::string("gnu_merge_without_buffer"),
      std::string("MergeWithOutBuffer1"),
      std::string("MergeWithOutBuffer")
  };

std::vector<std::string> Timings::shared_merge_function_buffer_use_type_string_
  = {
    std::string("Merge functions that do NOT use a buffer:"),
    std::string("Merge functions that use a buffer if available:"),
    std::string("Merge functions that use a buffer:")
};
std::vector<Timings::MergeFunctionBufferUseType>
                             Timings::shared_merge_function_buffer_use_type_ = {
      Timings::MergeFunctionBufferUseType::UsesABuffer,
      Timings::MergeFunctionBufferUseType::UsesABufferIfAvailable,
      Timings::MergeFunctionBufferUseType::DoesNotUseBuffer,
      Timings::MergeFunctionBufferUseType::DoesNotUseBuffer,
      Timings::MergeFunctionBufferUseType::DoesNotUseBuffer
  };
const Timings::SizeType
  Timings::divsor_to_go_from_nano_to_other_time_units_[]
           = { 1, 1000, 1000000, 1000000000 };
const std::string Timings::time_units_strings_[Timings::num_time_units_strings_] = {
      std::string("ns"),
      std::string("mus"),
      std::string("ms"),
      std::string("s")
  };
std::vector<std::string> Timings::shared_left_column_;
std::vector<std::string> Timings::shared_top_row_ = {
    std::string("(Denom. below, numer. right)")
};
std::vector<Timings::SizeType> Timings::shared_table_min_column_string_widths_;

#endif /* SRC_TIME_MERGE_ALGORITHMS_CLASS_H_ */
