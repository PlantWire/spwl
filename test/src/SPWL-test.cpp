#include <sstream>

#include "../cute/cute.h"
#include "../cute/ide_listener.h"
#include "../cute/xml_listener.h"
#include "../cute/cute_runner.h"

#include "../../lib/include/SPWL.h"
#include "../../lib/include/crc16.h"
#include "../include/SPWL-test.h"

const std::vector<unsigned char> hello { 'H', 'e', 'l', 'l', 'o' };
const std::vector<unsigned char> data1 { 'M', 'y', 'L', 'i', 't', 't', 'l', 'e',
    'C', 'p', 'p' };
const std::vector<unsigned char> data2 { 'I', '\'', 'm', ' ', 'a', ' ', 'l',
    'i', 't', 't', 'l', 'e', ' ', 'b', 'i', 't', ' ', 'l', 'o', 'n', 'g', 'e',
    'r', ',', ' ', 'b', 'e', 'a', 'r', ' ', 'w', 'i', 't', 'h', ' ', 'm', 'e',
    '.', ' ', 'T', 'h', 'e', ' ', 'T', 'e', 's', 't', ' ', 's', 'h', 'o', 'u',
    'l', 'd', ' ', 's', 't', 'i', 'l', 'l', ' ', 's', 'u', 'c', 'c', 'e', 'e',
    'd', '.', '\n', 'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'i', 'm', 'p', 'o',
    'r', 't', 'a', 'n', 't', ' ', 't', 'o', ' ', 'g', 'u', 'a', 'r', 'a', 'n',
    't', 'e', 'e', ' ', 'i', 't', ' ', 'd', 'o', 'e', 's', 'n', '\'', 't', ' ',
    'j', 'u', 's', 't', ' ', 't', 'e', 's', 't', 's', ' ', 'a', ' ', 'f', 'i',
    'x', 'a', 't', 'e', 'd', ' ', 'r', 'e', 's', 'u', 'l', 't', '.' };

// Begin CRC-Tests
void crc1Test() {
  CRC16 crc { };
  crc.update(std::array<unsigned char, 9> { '1', '2', '3', '4', '5', '6', '7',
      '8', '9' }, 9);
  ASSERT_EQUALM("Hash does not match with CRC-16 equivalent.", 0xbb3d,
      crc.get());
}

void crc2Test() {
  CRC16 crc { };
  crc.update(std::array<unsigned char, 10> { 'H', 'e', 'l', 'l', 'o', 'W', 'o',
      'r', 'l', 'd' }, 10);
  ASSERT_EQUALM("Hash does not match with CRC-16 equivalent.", 0x6053,
      crc.get());
}

void crc3Test() {
  CRC16 crc { };
  crc.update(std::array<unsigned char, 3> { 'C', 'P', 'P' }, 3);
  ASSERT_EQUALM("Hash does not match with CRC-16 equivalent.", 0xe8cd,
      crc.get());
}
// End CRC-Tests

// Begin SPWL-Tests
void preambleCheckerTest() {
  std::array<unsigned char, SPWLPacket::PREAMBLESIZE> input { };
  std::string testPreamble = "UUUUUUU";
  ASSERTM("Empty preample was correct.", !SPWLPacket::checkPreamble(input));

  for (size_t i = 0; i < testPreamble.size(); i++) {
    input[i] = testPreamble[i];
  }
  ASSERTM("Preample is wrong.", SPWLPacket::checkPreamble(input));
}

void packetTest() {
  std::pair<SPWLPacket, bool> res = SPWLPacket::encapsulateData(hello);
  ASSERTM("Result is missing.", res.second);
  SPWLPacket packet { res.first };
  res = SPWLPacket::encapsulatePacket(packet.rawData());
  ASSERTM("Result is missing.", res.second);
  ASSERT_EQUALM("Data was not in encapsulation.", hello, res.first.getData());
}

void corruptPacketTest() {
  std::pair<SPWLPacket, bool> res = SPWLPacket::encapsulateData(hello);

  ASSERTM("Result is missing.", res.second);
  auto raw = res.first.rawData();
  raw[0] = 5;
  res = SPWLPacket::encapsulatePacket(raw);
  ASSERTM("Corrupt data was encapsulated", !res.second);
}

void corruptDataTest() {
  std::pair<SPWLPacket, bool> res = SPWLPacket::encapsulateData(hello);

  ASSERTM("Result is missing.", res.second);
  auto raw = res.first.rawData();
  raw[SPWLPacket::PREAMBLESIZE + SPWLPacket::HEADERSIZE + 1] = 5;
  res = SPWLPacket::encapsulatePacket(raw);
  ASSERTM("Invalid packet was encapsulated", !res.second);
}

void packetMinTest() {
  std::vector<unsigned char> data { 'o' };
  std::pair<SPWLPacket, bool> res = SPWLPacket::encapsulateData(data);

  ASSERTM("Result is missing.", res.second);
  SPWLPacket packet { res.first };
  res = SPWLPacket::encapsulatePacket(packet.rawData());
  ASSERTM("Result is missing.", res.second);
  ASSERT_EQUALM("Minimum sized data couldn't be encapsulated.", data,
      res.first.getData());
}

void packetMaxTest() {
  std::vector<unsigned char> data(SPWLPacket::MAXDATASIZE, 'h');
  std::pair<SPWLPacket, bool> res = SPWLPacket::encapsulateData(data);

  ASSERTM("Result is missing.", res.second);
  SPWLPacket packet { res.first };
  res = SPWLPacket::encapsulatePacket(packet.rawData());
  ASSERTM("Result is missing.", res.second);
  ASSERT_EQUALM("Maximum sized data couldn't be encapsulated.", data,
      res.first.getData());
}

void packetOverflowTest() {
  std::vector<unsigned char> data(SPWLPacket::MAXDATASIZE + 1, 'h');
  std::pair<SPWLPacket, bool> res = SPWLPacket::encapsulateData(data);
  ASSERTM("Oversized data was encapsulated", !res.second);
}

void lengthExtractorTest() {
  std::pair<SPWLPacket, bool> result = SPWLPacket::encapsulateData(data1);

  ASSERTM("Result is missing.", result.second);
  std::array<unsigned char, SPWLPacket::PACKETSIZE> rawData {
      result.first.rawData() };
  std::array<unsigned char, SPWLPacket::HEADERSIZE> header { };
  std::copy(rawData.cbegin() + SPWLPacket::PREAMBLESIZE,
      rawData.cbegin() + SPWLPacket::PREAMBLESIZE + SPWLPacket::HEADERSIZE,
      header.begin());
  uint16_t length = SPWLPacket::getLengthFromHeader(header);
  ASSERT_EQUALM("Length was not correctly extracted.", 11, length);
}

void rawDataSizeTest() {
  std::pair<SPWLPacket, bool> result = SPWLPacket::encapsulateData(data1);
  ASSERTM("Result is missing.", result.second);
  ASSERT_EQUALM("Data size wasn't correctly derived.",
      7 + 6 + data1.size() + 2 + 1, result.first.rawDataSize());

  result = SPWLPacket::encapsulateData(data2);
  ASSERTM("Result is missing.", result.second);
  ASSERT_EQUALM("Data size wasn't correctly derived.",
      7 + 6 + data2.size() + 2 + 1, result.first.rawDataSize());
}
// End SPWL-Tests
bool runAllTests(int argc, char const *argv[]) {
  cute::suite s;
  s.push_back(CUTE(crc1Test));
  s.push_back(CUTE(crc2Test));
  s.push_back(CUTE(crc3Test));
  s.push_back(CUTE(preambleCheckerTest));
  s.push_back(CUTE(packetTest));
  s.push_back(CUTE(corruptPacketTest));
  s.push_back(CUTE(corruptDataTest));
  s.push_back(CUTE(packetMinTest));
  s.push_back(CUTE(packetMaxTest));
  s.push_back(CUTE(packetOverflowTest));
  s.push_back(CUTE(lengthExtractorTest));
  s.push_back(CUTE(rawDataSizeTest));
  cute::xml_file_opener xmlfile(argc, argv);
  cute::xml_listener<cute::ide_listener<> > lis(xmlfile.out);
  bool success = cute::makeRunner(lis, argc, argv)(s, "AllTests");
  return success;
}

int main(int argc, char const *argv[]) {
  return runAllTests(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
