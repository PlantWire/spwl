//
// Created by carlo on 06.04.20.
//

#ifndef TEST_INCLUDE_SPWL_TEST_H_
#define TEST_INCLUDE_SPWL_TEST_H_

#include <iostream>
#include <algorithm>
#include <array>

template <class T, std::size_t N>
std::ostream& operator<<(std::ostream& o, const std::array<T, N>& arr) {
  std::copy(arr.cbegin(), arr.cend(), std::ostream_iterator<T>(o, " "));
  return o;
}
#endif  // TEST_INCLUDE_SPWL_TEST_H_
