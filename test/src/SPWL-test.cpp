#include <sstream>

#include "../cute/cute.h"
#include "../cute/ide_listener.h"
#include "../cute/xml_listener.h"
#include "../cute/cute_runner.h"

#include "../../lib/include/SPWL.h"
#include "../include/SPWL-test.h"

void preambleCheckerTest() {
  std::array<unsigned char, SPWLPackage::PREAMBLESIZE> input{};
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
  std::pair<SPWLPackage, bool> res = SPWLPackage::encapsulateData(data);
  if (res.second) {
    SPWLPackage package{res.first};
    std::pair<SPWLPackage, bool> res =
        SPWLPackage::encapsulatePackage(package.rawData());

    if (res.second) {
      ASSERT_EQUAL(data, res.first.getData());
    } else {
      ASSERT_EQUAL("Package not valid", "");
    }
  }
}

void packageMinTest() {
  std::string data = "o";

  std::pair<SPWLPackage, bool> res = SPWLPackage::encapsulateData(data);
  if (res.second) {
    SPWLPackage package{res.first};
    std::pair<SPWLPackage, bool> res =
        SPWLPackage::encapsulatePackage(package.rawData());

    if (res.second) {
      ASSERT_EQUAL(data, res.first.getData());
    } else {
      ASSERT_EQUAL("Package not valid", "");
    }
  }
}

void packageMaxTest() {
  std::string data(SPWLPackage::MAXDATASIZE, 'h');

  std::pair<SPWLPackage, bool> res = SPWLPackage::encapsulateData(data);
  if (res.second) {
    SPWLPackage package{res.first};
    std::pair<SPWLPackage, bool> res =
        SPWLPackage::encapsulatePackage(package.rawData());
    if (res.second) {
      ASSERT_EQUAL(data, res.first.getData());
    } else {
      ASSERT_EQUAL("Package not valid", "");
    }
  }
}

void packageOverflowTest() {
  std::string data(SPWLPackage::MAXDATASIZE+1, 'h');
  std::pair<SPWLPackage, bool> res = SPWLPackage::encapsulateData(data);
  ASSERT_EQUAL(res.second, false);
}

void lengthExtractorTest() {
  std::string data {"MyLittleCpp"};
  std::pair<SPWLPackage, bool> result = SPWLPackage::encapsulateData(data);
  if (result.second) {
    std::array<unsigned char, SPWLPackage::PACKETSIZE>
        rawData{result.first.rawData()};
    std::array<unsigned char, SPWLPackage::HEADERSIZE> header{};
    std::copy(rawData.cbegin(), rawData.cbegin() + SPWLPackage::HEADERSIZE,
        header.begin());
    uint16_t length = SPWLPackage::getLengthFromHeader(header);
    ASSERT_EQUAL(11, length);
  } else {
    ASSERT_EQUAL("Package not valid", "");
  }
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
  s.push_back(CUTE(lengthExtractorTest));
  cute::xml_file_opener xmlfile(argc, argv);
  cute::xml_listener<cute::ide_listener<> > lis(xmlfile.out);
  bool success = cute::makeRunner(lis, argc, argv)(s, "AllTests");
  return success;
}

int main(int argc, char const *argv[]) {
  return runAllTests(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
