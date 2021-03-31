/*
 * misc_helpers.h
 *
 *  Created on: Jul 1, 2017
 *      Author: diagoras
 */

#ifndef SRC_MISC_HELPERS_H_
#define SRC_MISC_HELPERS_H_

#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <type_traits>
#include <typeinfo>

template<typename T, typename U>
std::ostream &operator<<(std::ostream &os, const std::pair<T, U> &p) {
  os << '(' << p.first << ", " << p.second << ')';
  return os;
}

//This general case is designed to work with any type ValueType for which
// std::is_integral<A>::value == true.
template<typename ValueType, typename Iterator,
std::enable_if_t<std::is_integral<ValueType>::value, int> = 0>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, ValueType a, ValueType b) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_int_distribution<ValueType> dist(a, b);
  for (auto it = start; it != one_past_end; it++)
    *it = dist(generator);
  return ;
}

template<typename ValueType, typename Iterator,
std::enable_if_t<std::is_floating_point<ValueType>::value, int> = 0>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, ValueType a, ValueType b) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<ValueType> dist(a, b);
  for (auto it = start; it != one_past_end; it++)
    *it = dist(generator);
  return ;
}

template<typename ValueType, typename Iterator, typename T,
std::enable_if_t<std::is_same<ValueType, typename std::pair<T, T>>::value, int> = 0,
std::enable_if_t<std::is_integral<T>::value, int> = 0>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, T a, T b) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_int_distribution<T> dist(a, b);
  for (auto it = start; it != one_past_end; it++) {
    (*it).first  = dist(generator);
    (*it).second = dist(generator);
  }
  return ;
}

template<typename ValueType, typename Iterator, typename T,
std::enable_if_t<std::is_same<ValueType, typename std::pair<T, T>>::value, int> = 0,
std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, T a, T b) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<T> dist(a, b);
  for (auto it = start; it != one_past_end; it++) {
    (*it).first  = dist(generator);
    (*it).second = dist(generator);
  }
  return ;
}

template<typename ValueType, typename Iterator,
std::enable_if_t<std::is_same<ValueType, typename std::string>::value, int> = 0>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end,
            std::size_t min_string_size = 0, std::size_t max_string_size = 20,
            char min_char = 32, char max_char = 126) {
  assert(max_string_size < 100000); //This value likely shouldn't be too large.
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_int_distribution<std::size_t> size_dist(min_string_size,
                                                       max_string_size);
  std::uniform_int_distribution<char> char_dist(min_char, max_char);

  for (auto it = start; it != one_past_end; it++) {
    std::stringstream strm;
    std::size_t count = 0;
    while (count < min_string_size) {
      char c = char_dist(generator);
      strm << c;
      ++count;
    }
    std::size_t string_length = size_dist(generator);
    while (count < string_length) {
      char c = char_dist(generator);
      strm << c;
      ++count;
    }
    *it = strm.str();
    (*it).reserve(max_string_size);
  }
  return ;
}

/*
template<typename Iterator, class ValueType,
std::enable_if_t<std::is_same<ValueType, double>::value, int> = 0>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, double a, double b) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<double> dist(a, b);
  for (auto it = start; it != one_past_end; it++)
    *it = dist(generator);
  return ;
}

template<typename Iterator, class ValueType,
std::enable_if_t<std::is_same<ValueType, long double>::value, int> = 0>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, long double a, long double b) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<long double>  dist(a, b);
  for (auto it = start; it != one_past_end; it++)
    *it = dist(generator);
  return ;
}
*/

template<typename ValueType, typename T = ValueType,
std::enable_if_t<std::is_integral<ValueType>::value, int> = 0>
ValueType PickRandom(T lower_bound = std::numeric_limits<T>::min(),
                     T upper_bound = std::numeric_limits<T>::max()) {
  std::random_device rnd_device;
  std::mt19937 generator(rnd_device());
  std::uniform_int_distribution<ValueType> dist(lower_bound, upper_bound);
  return dist(generator);
}


template<typename ValueType, typename T = ValueType,
std::enable_if_t<std::is_floating_point<ValueType>::value, int> = 0>
ValueType PickRandom(T lower_bound = std::numeric_limits<T>::min(),
                     T upper_bound = std::numeric_limits<T>::max()) {
  std::random_device rnd_device;
  std::mt19937 generator(rnd_device());
  std::uniform_real_distribution<ValueType> dist(lower_bound, upper_bound);
  return dist(generator);
}

template<typename ValueType, typename T = std::size_t,
std::enable_if_t<std::is_same<ValueType, typename std::string>::value, int> = 0>
std::string PickRandom(
            std::size_t min_string_size = 0, std::size_t max_string_size = 20,
            char min_char = 32, char max_char = 126) {
  std::random_device rnd_device;
  std::mt19937 generator(rnd_device());
  std::uniform_int_distribution<std::size_t> size_dist(min_string_size,
                                                       max_string_size);
  std::uniform_int_distribution<char> char_dist(min_char, max_char);

  std::stringstream strm;
  std::size_t count = 0;
  while (count < min_string_size) {
    char c = char_dist(generator);
    strm << c;
    ++count;
  }
  std::size_t string_length = size_dist(generator);
  while (count < string_length) {
    char c = char_dist(generator);
    strm << c;
    ++count;
  }
  return strm.str();
}

template<typename ValueType, typename T,
std::enable_if_t<std::is_same<ValueType, typename std::pair<T, T>>::value, int> = 0,
std::enable_if_t<std::is_integral<T>::value, int> = 0>
ValueType PickRandom(T lower_bound = std::numeric_limits<T>::min(),
                     T upper_bound = std::numeric_limits<T>::max()) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::pair<T, T> value;
  std::uniform_int_distribution<T> dist(lower_bound, upper_bound);
  value.first  = dist(generator);
  value.second = dist(generator);
  return value;
}

template<typename ValueType, typename T,
std::enable_if_t<std::is_same<ValueType, typename std::pair<T, T>>::value, int> = 0,
std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
ValueType PickRandom(T lower_bound = std::numeric_limits<T>::min(),
                     T upper_bound = std::numeric_limits<T>::max()) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::pair<T, T> value;
  std::uniform_real_distribution<T> dist(lower_bound, upper_bound);
  value.first  = dist(generator);
  value.second = dist(generator);
  return value;
}

/*
template<typename ValueType = double>
ValueType PickRandom(double lower_bound = std::numeric_limits<double>::min(),
                     double upper_bound = std::numeric_limits<double>::max()) {
  std::random_device rnd_device;
  std::mt19937 generator(rnd_device());
  std::uniform_real_distribution<double> dist(lower_bound, upper_bound);
  return dist(generator);
}

template<typename ValueType = long double>
ValueType PickRandom(
            long double lower_bound = std::numeric_limits<long double>::min(),
            long double upper_bound = std::numeric_limits<long double>::max()) {
  std::random_device rnd_device;
  std::mt19937 generator(rnd_device());
  std::uniform_real_distribution<long double> dist(lower_bound, upper_bound);
  return dist(generator);
}
*/

template<typename ForwardIterator>
bool IsNonDecreasing(ForwardIterator start_it, ForwardIterator one_past_last) {
  auto one_ahead = start_it;
  if(start_it == one_past_last || ++one_ahead == one_past_last)
    return true; //Vacuously non-decreasing.
  while (one_ahead != one_past_last) {
    if (*start_it > *one_ahead)
      return false;
    start_it++;
    one_ahead++;
  }
  return true;
}


std::string GetCurrentTimeString() {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream osstrm;
  osstrm << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
  return osstrm.str();
}

template<typename ValueType>
inline std::string GetTypeNameString() {
  return typeid(ValueType).name();
}

template<> inline std::string GetTypeNameString<bool> () {
  std::string str = std::string("bool");
  return str;
}
template<> inline std::string GetTypeNameString<char> () {
  std::string str = std::string("char");
  return str;
}
template<> inline std::string GetTypeNameString<signed char> () {
  std::string str = std::string("signed char");
  return str;
}
template<> inline std::string GetTypeNameString<unsigned char> () {
  std::string str = std::string("unsigned char");
  return str;
}
template<> inline std::string GetTypeNameString<short> () {
  std::string str = std::string("short");
  return str;
}
template<> inline std::string GetTypeNameString<unsigned short> () {
  std::string str = std::string("unsigned short");
  return str;
}
template<> inline std::string GetTypeNameString<int> () {
  std::string str = std::string("int");
  return str;
}
template<> inline std::string GetTypeNameString<unsigned int> () {
  std::string str = std::string("unsigned int");
  return str;
}
template<> inline std::string GetTypeNameString<long> () {
  std::string str = std::string("long");
  return str;
}
template<> inline std::string GetTypeNameString<unsigned long> () {
  std::string str = std::string("unsigned long");
  return str;
}
template<> inline std::string GetTypeNameString<long long> () {
  std::string str = std::string("long long");
  return str;
}
template<> inline std::string GetTypeNameString<unsigned long long> () {
  std::string str = std::string("unsigned long long");
  return str;
}
template<> inline std::string GetTypeNameString<float> () {
  std::string str = std::string("float");
  return str;
}
template<> inline std::string GetTypeNameString<double> () {
  std::string str = std::string("double");
  return str;
}
template<> inline std::string GetTypeNameString<long double> () {
  std::string str = std::string("long double");
  return str;
}
template<> inline std::string GetTypeNameString<std::string> () {
  std::string str = std::string("std::string");
  return str;
}

template<> inline std::string GetTypeNameString<std::pair<bool, bool>>() {
  std::string subtype_str = GetTypeNameString<bool>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<char, char>>() {
  std::string subtype_str = GetTypeNameString<char>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<unsigned char, unsigned char>>() {
  std::string subtype_str = GetTypeNameString<unsigned char>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<short, short>>() {
  std::string subtype_str = GetTypeNameString<short>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<unsigned short, unsigned short>>() {
  std::string subtype_str = GetTypeNameString<unsigned short>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<int, int>>() {
  std::string subtype_str = GetTypeNameString<int>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<unsigned int, unsigned int>>() {
  std::string subtype_str = GetTypeNameString<unsigned int>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<long, long>>() {
  std::string subtype_str = GetTypeNameString<long>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<unsigned long, unsigned long>>() {
  std::string subtype_str = GetTypeNameString<unsigned long>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<long long, long long>>() {
  std::string subtype_str = GetTypeNameString<long long>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<unsigned long long, unsigned long long>>() {
  std::string subtype_str = GetTypeNameString<unsigned long long>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<float, float>>() {
  std::string subtype_str = GetTypeNameString<float>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<double, double>>() {
  std::string subtype_str = GetTypeNameString<double>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}
template<> inline std::string GetTypeNameString<std::pair<long double, long double>>() {
  std::string subtype_str = GetTypeNameString<long double>();
  std::string str = std::string("std::pair<") + subtype_str + std::string(",")
                  + subtype_str + std::string(">");
  return str;
}


template<typename ValueType>
inline std::string GetTypeNameString(char sep) {
  std::string str = GetTypeNameString<ValueType>();
  auto spaces_to_hyphens = [&sep](char c) {
    return c == ' ' ? sep : c;
  };
  std::transform(str.begin(), str.end(), str.begin(), spaces_to_hyphens);
  return str;
}


template<typename ValueType,
std::enable_if_t<std::is_integral<ValueType>::value, int> = 0>
inline std::string GetDescriptionOfRangeOfValues(ValueType value_lower_bound,
                                                 ValueType value_upper_bound) {
  std::stringstream strm;
  strm << "ValueType will range in the interval: ";
  strm << "[" << value_lower_bound << ", " << value_upper_bound << "]\n";
  return strm.str();
}

template<typename ValueType,
std::enable_if_t<std::is_floating_point<ValueType>::value, int> = 0>
inline std::string GetDescriptionOfRangeOfValues(ValueType value_lower_bound,
                                                 ValueType value_upper_bound) {
  std::stringstream strm;
  strm << "ValueType will range in the interval: ";
  strm << "(" << value_lower_bound << ", " << value_upper_bound << ")\n";
  return strm.str();
}


template<typename ValueType,
std::enable_if_t<std::is_same<ValueType, typename std::string>::value, int> = 0>
inline std::string GetDescriptionOfRangeOfValues(ValueType value_lower_bound,
                                                 ValueType value_upper_bound) {
  std::stringstream strm;
  strm << "Valutype will range over strings of length between ";
  strm << value_lower_bound << " and " << value_upper_bound << "\n";
  return strm.str();
}

template<typename T, typename ValueType>
std::string GetNameOfOutputFile(std::string container_type_string,
    T value_lower_bound, T value_upper_bound,
    std::string file_name_prefix = std::string("time_merges_"),
    std::string file_name_postfix = std::string(".txt")) {
  std::stringstream strm;
  std::string value_type_string = GetTypeNameString<ValueType>();
  //Replace spaces with '_'
  auto spaces_to_underscore_lambda = [](char c) {
      return c == ' ' ? '_' : c;
  };
  std::transform(value_type_string.begin(), value_type_string.end(),
                 value_type_string.begin(), spaces_to_underscore_lambda);
  std::transform(container_type_string.begin(), container_type_string.end(),
                 container_type_string.begin(), spaces_to_underscore_lambda);
  strm << file_name_prefix;
  strm << "_" << value_type_string;
  strm << "_" << container_type_string;
  strm << file_name_postfix;
  return strm.str();
}

inline std::string GetLineString(const std::string str,
                                 const int num_repetitions = 80) {
  std::stringstream strm;
  for (auto i = 0; i < num_repetitions; i++)
    strm << str;
  strm << '\n';
  return strm.str();
}

inline void PrintLine(const std::string str, const int num_repetitions = 80) {
  std::cout << GetLineString(str, num_repetitions) << std::endl;
  return ;
}

template<typename BiDirectionalIterator, typename Compare>
std::string GetNondecreasingSubsequences(BiDirectionalIterator start,
                                         std::size_t length,
                                         Compare comp) {
  auto comp_le = [comp](const auto &lhs, const auto &rhs) -> bool {
    return !comp(rhs, lhs);
  };
  std::size_t count_width = 1;
  std::size_t num = 0;
  auto it = start;
  while (num + 1 < length) {
    std::size_t count = 1;
    auto it_plus1 = it;
    ++it_plus1;
    if (comp(*it_plus1, *it)) {
      it = it_plus1++;
      num++;
      continue;
    }
    it_plus1 = it;
    ++it_plus1;
    //Count how many elements are in the current monotone subsequence.
    while (num + 1 < length && comp_le(*it, *it_plus1)) {
      it = it_plus1++;
      num++;
      count++;
    }
    auto cur_count_length = std::to_string(count).length();
    if (count_width < cur_count_length)
      count_width = cur_count_length;
  }

  num = 0;
  it = start;
  std::stringstream strm;
  while (num < length) {
    auto cur_it       = it;
    auto cur_num      = num;
    std::size_t count = 1;
    //Count how many elements are in the current monotone subsequence.
    auto cur_it_plus1 = cur_it;
    ++cur_it_plus1;
    while (cur_num + 1 < length && comp_le(*cur_it, *cur_it_plus1)) {
      cur_it = cur_it_plus1++;
      cur_num++;
      count++;
    }
    strm << std::left << std::setw(count_width) << count << "|";
    strm << ' ' << *it;
    num++;
    auto it_plus1 = it;
    ++it_plus1;
    while (num < length && comp_le(*it, *it_plus1)) {
      it = it_plus1++;
      strm << ' ' << *it;
      num++;
    }
    ++it;
    strm << '\n';
  }
  return strm.str();
}

template<typename BiDirectionalIterator, typename Compare>
void PrintNondecreasingSubsequences(BiDirectionalIterator start,
                                    std::size_t length,
                                    Compare comp,
                                    bool print_new_line_at_end = true,
                                    std::ostream &ostrm = std::cout) {
  ostrm << GetNondecreasingSubsequences(start, length, comp);
  if (print_new_line_at_end)
    ostrm << '\n';
  ostrm.flush();
}

template<typename BiDirectionalIterator, typename Compare>
void PrintNondecreasingSubsequences(BiDirectionalIterator start,
                                    std::size_t length,
                                    Compare comp,
                                    bool print_new_line_at_end,
                                    std::stringstream &strm) {
  strm << GetNondecreasingSubsequences(start, length, comp);
  if (print_new_line_at_end)
    strm << '\n';
  return ;
}

/* Helper function for VerifyMergeIsInplace() andVerifyMergeIsInplace().
 */
template<typename T>
void PrintTwoSortedVectorsInfo(const std::vector<T> &vec, int start_left,
                               int end_left, int start_right, int end_right) {
    std::cout << "start_left = "   << start_left          << "\tend_left =\t"
              << end_left          << "\tstart_right =\t" << start_right
              << "\tend_right =\t" << end_right           << std::endl;
    auto length_left  = end_left  + 1 - start_left;
    auto length_right = end_right + 1 - start_right;
    PrintNondecreasingSubsequences(vec.begin() + start_left,  length_left,
                                   false);
    PrintNondecreasingSubsequences(vec.begin() + start_right, length_right,
                                   true);
    return ;
}


/*


template<typename T>
inline T ForceValueToBeWithinBounds(T value, T minimum_value, T maximum_value) {
  if (value <= minimum_value)
    return minimum_value;
  else if (value >= maximum_value)
    return maximum_value;
  else
    return value;
}

template<typename T>
inline void AssignRightVectorValuesToLeft(std::vector<T> &lhs,
                                          const std::vector<T> &rhs) {
  assert(lhs.size() >= rhs.size());
  for (auto it_lhs = lhs.begin(), it_rhs = rhs.begin();
            it_rhs != rhs.end(); ++it_lhs, ++it_rhs) {
    *it_lhs = *it_rhs;
  }
  return ;
}

template<typename ForwardIterator1, typename ForwardIterator2, typename RAI>
inline void AssignOriginalVectorValuesToLeftAndRightRanges(
    ForwardIterator1 lhs_start, ForwardIterator1 lhs_one_past_end,
    ForwardIterator2 rhs_start, ForwardIterator2 rhs_one_past_end,
    RAI vec_original_start, RAI vec_original_one_past_end) {
//assert(std::distance(lhs_start, lhs_one_past_end)
//       + std::distance(rhs_start, rhs_one_past_end) <= vec_original.size());
   auto lhs = lhs_start;
   auto vec_original_iter = vec_original_start;
   while (lhs != lhs_one_past_end) {
assert(vec_original_iter != vec_original_one_past_end);
     *lhs = *vec_original_iter;
     ++lhs;
     ++vec_original_iter;
   }
   auto rhs = rhs_start;
   while (rhs != rhs_one_past_end) {
assert(vec_original_iter != vec_original_one_past_end);
     *rhs = *vec_original_iter;
     ++rhs;
     ++vec_original_iter;
   }
   return ;
}

template<typename ForwardIterator1, typename ForwardIterator2>
inline void AssignRightRangeValuesToLeftRange(
    ForwardIterator1 lhs_start, ForwardIterator1 lhs_one_past_end,
    ForwardIterator2 rhs_start, ForwardIterator2 rhs_one_past_end) {
assert(std::distance(lhs_start, lhs_one_past_end)
                                 >= std::distance(rhs_start, rhs_one_past_end));
  auto lhs = lhs_start;
  auto rhs = rhs_start;
  while (lhs != lhs_one_past_end && rhs != rhs_one_past_end) {
    *lhs = *rhs;
    ++lhs;
    ++rhs;
  }
  return ;
}

template<typename ForwardIterator1, typename ForwardIterator2, typename RAI>
inline void MoveOriginalVectorValuesToLeftAndRightRanges(
    ForwardIterator1 lhs_start, ForwardIterator1 lhs_one_past_end,
    ForwardIterator2 rhs_start, ForwardIterator2 rhs_one_past_end,
    RAI vec_original_start, RAI vec_original_one_past_end) {
//assert(std::distance(lhs_start, lhs_one_past_end)
//       + std::distance(rhs_start, rhs_one_past_end) <= vec_original.size());
   auto lhs = lhs_start;
   auto vec_original_iter = vec_original_start;
   while (lhs != lhs_one_past_end) {
assert(vec_original_iter != vec_original_one_past_end);
     *lhs = std::move(*vec_original_iter);
     ++lhs;
     ++vec_original_iter;
   }
   auto rhs = rhs_start;
   while (rhs != rhs_one_past_end) {
assert(vec_original_iter != vec_original_one_past_end);
     *rhs = std::move(*vec_original_iter);
     ++rhs;
     ++vec_original_iter;
   }
   return ;
}
*/

/*
 * This function checks whether or not the sequence of elements:
 *  *iter_left_start, ..., *(iter_left_one_past_end - 1), *iter_right_start,
 *    ..., *(iter_right_one_past_end - 1)
 *  is sorted according to the C++ standard's defintion of "sorted".
 *
 * RECALL FROM THE C++ STANDARD THE DEFINITION OF "SORTED":
 *  A sequence is sorted with respect to a comparator comp if for any iterator
 *   it pointing to the sequence and any non-negative integer n such that it + n
 *   is a valid iterator pointing to an element of the sequence,
 *   comp(*(it + n), *it) evaluates to false.
 */
/*
template<typename Iterator1, typename Iterator2>
bool AreCombinedRangesSorted(Iterator1 iter_left_start,
                             Iterator1 iter_left_one_past_end,
                             Iterator2 iter_right_start,
                             Iterator2 iter_right_one_past_end
                             ) {
  if (!std::is_sorted(iter_left_start, iter_left_one_past_end)) {
    return false;
  }
  if (!std::is_sorted(iter_right_start, iter_right_one_past_end)) {
    return false;
  }
  if (iter_left_start  == iter_left_one_past_end ||
      iter_right_start == iter_right_one_past_end) {
    return true;
  }
  auto iter_left_end = iter_left_one_past_end;
  --iter_left_end;
  if (*iter_left_end < *iter_right_start) {
    return true;
  }
  if (*iter_right_start < *iter_left_end) {
    return false;
  }
  {
    auto iter_right = iter_right_start;
    ++iter_right;
    const auto &left_end_value = *iter_left_end;
    while (iter_right != iter_right_one_past_end) {
      if (*iter_right < left_end_value) {
        return false;
      }
      ++iter_right;
    }
  }
  {
    auto iter_right = iter_right_start;
    ++iter_right;
    const auto &left_end_value = *iter_left_end;
    while (iter_right != iter_right_one_past_end) {
      if (*iter_right < left_end_value) {
        return false;
      }
      ++iter_right;
    }
  }
}
*/

#endif /* SRC_MISC_HELPERS_H_ */
