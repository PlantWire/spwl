#include <sstream>

#include "../cute/cute.h"
#include "../cute/ide_listener.h"
#include "../cute/xml_listener.h"
#include "../cute/cute_runner.h"

#include "../../lib/include/SPWL.h"
#include "../include/SPWL-test.h"

void preambleCheckerTest() {
  std::array<unsigned, SPWLPackage::PREAMBLESIZE> input{};
  std::string testPreamble = "UUUUUUU";
  for (size_t i = 0; i < testPreamble.size(); i++) {
    input[i] = testPreamble[i];
  }
  ASSERT_EQUAL(SPWLPackage::checkPreamble(input), true);
}

void checksumTest() {
  std::string data {"Hello"};
  ASSERT_EQUAL("HelloWorld!!!!!!", SPWLPackage::generateChecksum(data));
}

void checksumCheckerTest() {
  std::string data {"Hello"};
  std::string checksum {SPWLPackage::generateChecksum(data)};
  ASSERT_EQUAL(SPWLPackage::checkChecksum(checksum, data), true);
}

void packageTest() {
  std::string data = "Hello";
  SPWLPackage package = SPWLPackage::encapsulateData(data);
  std::optional<SPWLPackage> res =
      SPWLPackage::encapsulatePackage(package.rawData());

  if (res.has_value()) {
    ASSERT_EQUAL(data, res.value().getData());
  } else {
    ASSERT_EQUAL("Package not valid", "");
  }
}

void packageMinTest() {
  std::string data = "o";
  SPWLPackage package = SPWLPackage::encapsulateData(data);
  std::optional<SPWLPackage> res =
      SPWLPackage::encapsulatePackage(package.rawData());

  if (res.has_value()) {
    ASSERT_EQUAL(data, res.value().getData());
  } else {
    ASSERT_EQUAL("Package not valid", "");
  }
}

void packageMaxTest() {
  std::string data(SPWLPackage::MAXDATASIZE, 'h');
  SPWLPackage package = SPWLPackage::encapsulateData(data);
  std::optional<SPWLPackage> res =
      SPWLPackage::encapsulatePackage(package.rawData());

  if (res.has_value()) {
    ASSERT_EQUAL(data, res.value().getData());
  } else {
    ASSERT_EQUAL("Package not valid", "");
  }
}

void packageOverflowTest() {
  std::string data(SPWLPackage::MAXDATASIZE+1, 'h');
  ASSERT_THROWS(SPWLPackage::encapsulateData(data), std::invalid_argument);
}

bool runAllTests(int argc, char const *argv[]) {
  cute::suite s;
  // TODO(ckirchme) add your test here
  s.push_back(CUTE(preambleCheckerTest));
  s.push_back(CUTE(checksumTest));
  s.push_back(CUTE(checksumCheckerTest));
  s.push_back(CUTE(packageTest));
  s.push_back(CUTE(packageMinTest));
  s.push_back(CUTE(packageMaxTest));
  s.push_back(CUTE(packageOverflowTest));
  cute::xml_file_opener xmlfile(argc, argv);
  cute::xml_listener<cute::ide_listener<> > lis(xmlfile.out);
  bool success = cute::makeRunner(lis, argc, argv)(s, "AllTests");
  return success;
}

int main(int argc, char const *argv[]) {
  return runAllTests(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
