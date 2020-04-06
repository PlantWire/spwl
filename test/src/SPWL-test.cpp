#include <sstream>

#include "../cute/cute.h"
#include "../cute/ide_listener.h"
#include "../cute/xml_listener.h"
#include "../cute/cute_runner.h"

#include "../../lib/include/SPWL.h"
#include "../include/SPWL-test.h"

void SPWLpreambleCheckerTest(){
  std::array<unsigned, SPWLPackage::PREAMBLESIZE> input{};
  std::string testPreamble = "UUUUUUU";
  for(size_t i = 0; i < testPreamble.size(); i++) {
    input[i] = testPreamble[i];
  }
  ASSERT_EQUAL(SPWLPackage::checkPreamble(input), true);
}

void SPWLchecksumTest(){
  std::string data {"Hello"};
  ASSERT_EQUAL("HelloWorld!!!!!!", SPWLPackage::generateChecksum(data));
}

void SPWLchecksumCheckerTest(){
  std::string data {"Hello"};
  std::string checksum {SPWLPackage::generateChecksum(data)};
  ASSERT_EQUAL(SPWLPackage::checkChecksum(checksum,data), true);
}

void SPWLpackageTest(){
  std::string data = "Hello";
  SPWLPackage package = SPWLPackage::encapsulateData(data);
  std::optional<SPWLPackage> res = SPWLPackage::encapsulatePackage(package.rawData());

  if(res.has_value()){
    ASSERT_EQUAL(data, res.value().getData());
  } else{
    ASSERT_EQUAL("Package not valid", "");
  }
}

void SPWLpackageMinTest(){
  std::string data = "o";
  SPWLPackage package = SPWLPackage::encapsulateData(data);
  std::optional<SPWLPackage> res = SPWLPackage::encapsulatePackage(package.rawData());

  if(res.has_value()){
    ASSERT_EQUAL(data, res.value().getData());
  } else{
    ASSERT_EQUAL("Package not valid", "");
  }
}

void SPWLpackageMaxTest(){
  std::string data (SPWLPackage::MAXDATASIZE, 'h');
  SPWLPackage package = SPWLPackage::encapsulateData(data);
  std::optional<SPWLPackage> res = SPWLPackage::encapsulatePackage(package.rawData());

  if(res.has_value()){
    ASSERT_EQUAL(data, res.value().getData());
  } else{
    ASSERT_EQUAL("Package not valid", "");
  }
}

void SPWLpackageOverflowTest(){
  std::string data (SPWLPackage::MAXDATASIZE+1, 'h');
  ASSERT_THROWS(SPWLPackage::encapsulateData(data),std::invalid_argument);
}

bool runAllTests(int argc, char const *argv[]) {
  cute::suite s;
  // TODO(ckirchme) add your test here
  s.push_back(CUTE(SPWLpreambleCheckerTest));
  s.push_back(CUTE(SPWLchecksumTest));
  s.push_back(CUTE(SPWLchecksumCheckerTest));
  s.push_back(CUTE(SPWLpackageTest));
  s.push_back(CUTE(SPWLpackageMinTest));
  s.push_back(CUTE(SPWLpackageMaxTest));
  s.push_back(CUTE(SPWLpackageOverflowTest));
  cute::xml_file_opener xmlfile(argc, argv);
  cute::xml_listener<cute::ide_listener<> > lis(xmlfile.out);
  bool success = cute::makeRunner(lis, argc, argv)(s, "AllTests");
  return success;
}

int main(int argc, char const *argv[]) {
  return runAllTests(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
