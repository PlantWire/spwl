//
// Created by carlo on 06.04.20.
//

#ifndef PWIRE_SERVER_PWIRE_SERVER_TEST_H
#define PWIRE_SERVER_PWIRE_SERVER_TEST_H

#include <iostream>
#include <algorithm>
#include <array>

template <class T, std::size_t N>
std::ostream& operator<<(std::ostream& o, const std::array<T, N>& arr)
{
  std::copy(arr.cbegin(), arr.cend(), std::ostream_iterator<T>(o, " "));
  return o;
}
#endif //PWIRE_SERVER_PWIRE_SERVER_TEST_H
