/*
 * random_helpers.h
 *
 *  Created on: May 5, 2017
 *      Author: Matthew Gregory Krupa
 */

#ifndef RANDOM_HELPERS_H_
#define RANDOM_HELPERS_H_

#include <iostream>
#include <random>
#include <vector>

namespace randomhelpers {
#define FILL_WITH_RANDOM_NUMBERS_DEFAULT_VERBOSE false
#define FILL_WITH_RANDOM_NUMBERS_DEFAULT_OSTRM std::cout
#define FILL_WITH_RANDOM_NUMBERS_FINISHED_MESSAGE "Finished filling vector with random goodness.\n"

template<typename T>
static inline void FillWithRandomNumbersPrintStartMessage(const std::size_t size,
                const T a, const T b, const bool verbose, std::ostream &ostrm) {
  if (verbose) {
    ostrm << "Started filling vector of size " << size
          << " with random numbers from the range [" << a << ", " << b << ")."
          << '\n';
    ostrm.flush();
  }
}

//This general case is designed to work with any type T for which
// std::is_integral<A>::value == true.
template<class Iterator, typename T>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, T a, T b,
                           bool verbose = FILL_WITH_RANDOM_NUMBERS_DEFAULT_VERBOSE,
                           std::ostream &ostrm = FILL_WITH_RANDOM_NUMBERS_DEFAULT_OSTRM) {
  std::random_device rnd_device;
  std::mt19937 generator(rnd_device());
  std::uniform_int_distribution<T> dist(a, b);
  FillWithRandomNumbersPrintStartMessage(std::distance(start, one_past_end), a, b, verbose, ostrm);
  for (auto it = start; it != one_past_end; it++)
    *it = dist(generator);
  if (verbose) {
    ostrm << FILL_WITH_RANDOM_NUMBERS_FINISHED_MESSAGE;
    ostrm.flush();
  }
  return ;
}

template<class Iterator>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, float a, float b,
                           bool verbose = FILL_WITH_RANDOM_NUMBERS_DEFAULT_VERBOSE,
                           std::ostream &ostrm = FILL_WITH_RANDOM_NUMBERS_DEFAULT_OSTRM) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<float> dist(a, b);
  FillWithRandomNumbersPrintStartMessage(std::distance(start, one_past_end), a, b, verbose, ostrm);
  for (auto it = start; it != one_past_end; it++)
    *it = dist(generator);
  if (verbose) {
    ostrm << FILL_WITH_RANDOM_NUMBERS_FINISHED_MESSAGE;
    ostrm.flush();
  }
  return ;
}

template<class Iterator>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, double a, double b,
                           bool verbose = FILL_WITH_RANDOM_NUMBERS_DEFAULT_VERBOSE,
                           std::ostream &ostrm = FILL_WITH_RANDOM_NUMBERS_DEFAULT_OSTRM) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<double> dist(a, b);
  FillWithRandomNumbersPrintStartMessage(std::distance(start, one_past_end), a, b, verbose, ostrm);
  for (auto it = start; it != one_past_end; it++)
    *it = dist(generator);
  if (verbose) {
    ostrm << FILL_WITH_RANDOM_NUMBERS_FINISHED_MESSAGE;
    ostrm.flush();
  }
  return ;
}

template<class Iterator>
void FillWithRandomNumbers(Iterator start, Iterator one_past_end, long double a, long double b,
                           bool verbose = FILL_WITH_RANDOM_NUMBERS_DEFAULT_VERBOSE,
                           std::ostream &ostrm = FILL_WITH_RANDOM_NUMBERS_DEFAULT_OSTRM) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<long double>  dist(a, b);
  FillWithRandomNumbersPrintStartMessage(std::distance(start, one_past_end), a, b, verbose, ostrm);
  for (auto it = start; it != one_past_end; it++)
    *it = dist(generator);
  if (verbose) {
    ostrm << FILL_WITH_RANDOM_NUMBERS_FINISHED_MESSAGE;
    ostrm.flush();
  }
  return ;
}


//This general case is designed to work with any type T for which
// std::is_integral<A>::value == true.
template<typename T>
void FillVectorWithRandomNumbers(std::vector<T> &vec, T a, T b,
                                 bool verbose = FILL_WITH_RANDOM_NUMBERS_DEFAULT_VERBOSE,
                                 std::ostream &ostrm = FILL_WITH_RANDOM_NUMBERS_DEFAULT_OSTRM) {
  std::random_device rnd_device;
  std::mt19937 generator(rnd_device());
  std::uniform_int_distribution<T> dist(a, b);
  FillWithRandomNumbersPrintStartMessage(vec.size(), a, b, verbose, ostrm);
  for (unsigned int i = 0; i < vec.size(); i++)
    vec[i] = dist(generator);
  if (verbose) {
    ostrm << FILL_WITH_RANDOM_NUMBERS_FINISHED_MESSAGE;
    ostrm.flush();
  }
  return ;
}

void FillVectorWithRandomNumbers(std::vector<float> &vec, float a, float b,
                                 bool verbose = FILL_WITH_RANDOM_NUMBERS_DEFAULT_VERBOSE,
                                 std::ostream &ostrm = FILL_WITH_RANDOM_NUMBERS_DEFAULT_OSTRM) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<float> dist(a, b);
  FillWithRandomNumbersPrintStartMessage(vec.size(), a, b, verbose, ostrm);
  for (unsigned int i = 0; i < vec.size(); i++)
    vec[i] = dist(generator);
  if (verbose) {
    ostrm << FILL_WITH_RANDOM_NUMBERS_FINISHED_MESSAGE;
    ostrm.flush();
  }
}

void FillVectorWithRandomNumbers(std::vector<double> &vec, double a, double b,
                                 bool verbose = FILL_WITH_RANDOM_NUMBERS_DEFAULT_VERBOSE,
                                 std::ostream &ostrm = FILL_WITH_RANDOM_NUMBERS_DEFAULT_OSTRM) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<double> dist(a, b);
  FillWithRandomNumbersPrintStartMessage(vec.size(), a, b, verbose, ostrm);
  for (unsigned int i = 0; i < vec.size(); i++)
    vec[i] = dist(generator);
  if (verbose) {
    ostrm << FILL_WITH_RANDOM_NUMBERS_FINISHED_MESSAGE;
    ostrm.flush();
  }
  return ;
}

void FillVectorWithRandomNumbers(std::vector<long double> &vec, long double a, long double b,
                                 bool verbose = FILL_WITH_RANDOM_NUMBERS_DEFAULT_VERBOSE,
                                 std::ostream &ostrm = FILL_WITH_RANDOM_NUMBERS_DEFAULT_OSTRM) {
  std::random_device rnd_device;
  std::mt19937_64 generator(rnd_device());
  std::uniform_real_distribution<long double>  dist(a, b);
  FillWithRandomNumbersPrintStartMessage(vec.size(), a, b, verbose, ostrm);
  for (unsigned int i = 0; i < vec.size(); i++)
    vec[i] = dist(generator);
  if (verbose) {
    ostrm << FILL_WITH_RANDOM_NUMBERS_FINISHED_MESSAGE;
    ostrm.flush();
  }
  return ;
}

#undef FILL_WITH_RANDOM_NUMBERS_DEFAULT_VERBOSE
#undef FILL_WITH_RANDOM_NUMBERS_DEFAULT_OSTRM
#undef FILL_WITH_RANDOM_NUMBERS_FINISHED_MESSAGE


} //END NAMESPACE randomhelpers

#endif /* RANDOM_HELPERS_H_ */
