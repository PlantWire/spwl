#include <sstream>

#include "../cute/cute.h"
#include "../cute/ide_listener.h"
#include "../cute/xml_listener.h"
#include "../cute/cute_runner.h"

#include "../../lib/include/SPWL.h"
#include "../../lib/include/crc16.h"
#include "../include/SPWL-test.h"

// Begin CRC-Tests
void crc1Test() {
  CRC16 crc{};
  crc.update(std::array<unsigned char, 9>{'1', '2', '3', '4', '5',
                                          '6', '7', '8', '9'}, 9);
  ASSERT_EQUAL(0xbb3d, crc.get());
}

void crc2Test() {
  CRC16 crc{};
  crc.update(std::array<unsigned char, 10>{'H', 'e', 'l', 'l', 'o',
                                          'W', 'o', 'r', 'l', 'd'}, 10);
  ASSERT_EQUAL(0x6053, crc.get());
}

void crc3Test() {
  CRC16 crc{};
  crc.update(std::array<unsigned char, 3>{'C', 'P', 'P'}, 3);
  ASSERT_EQUAL(0xe8cd, crc.get());
}
// End CRC-Tests

// Begin SPWL-Tests
void preambleCheckerTest() {
  std::array<unsigned char, SPWLPacket::PREAMBLESIZE> input{};
  std::string testPreamble = "UUUUUUU";
  for (size_t i = 0; i < testPreamble.size(); i++) {
    input[i] = testPreamble[i];
  }
  ASSERT_EQUAL(SPWLPacket::checkPreamble(input), true);
}

void packetTest() {
  std::vector<unsigned char> data{'H', 'e', 'l', 'l', 'o'};
  std::pair<SPWLPacket, bool> res = SPWLPacket::encapsulateData(data);
  if (res.second) {
    SPWLPacket packet{res.first};
    std::pair<SPWLPacket, bool> res =
        SPWLPacket::encapsulatePacket(packet.rawData());

    if (res.second) {
      ASSERT_EQUAL(data, res.first.getData());
    } else {
      ASSERT_EQUAL("Packet not valid", "");
    }
  } else {
    ASSERT_EQUAL("encapsulateData packet not valid", "");
  }
}

void corruptPacketTest() {
  std::vector<unsigned char> data{'H', 'e', 'l', 'l', 'o'};
  std::pair<SPWLPacket, bool> res = SPWLPacket::encapsulateData(data);
  if (res.second) {
    auto raw = res.first.rawData();
    raw[0] = 5;
    res = SPWLPacket::encapsulatePacket(raw);

    if (!res.second) {
      ASSERT_EQUAL("Packet not valid", "Packet not valid");
    } else {
      ASSERT_EQUAL("Packet should not be valid", "valid");
    }
  } else {
    ASSERT_EQUAL("encapsulateData Packet not valid", "");
  }
}

void corruptDataTest() {
  std::vector<unsigned char> data{'H', 'e', 'l', 'l', 'o'};
  std::pair<SPWLPacket, bool> res = SPWLPacket::encapsulateData(data);
  if (res.second) {
    auto raw = res.first.rawData();
    raw[SPWLPacket::PREAMBLESIZE + SPWLPacket::HEADERSIZE + 1] = 5;
    res = SPWLPacket::encapsulatePacket(raw);
    if (!res.second) {
      ASSERT_EQUAL("Packet not valid", "Packet not valid");
    } else {
      ASSERT_EQUAL("Packet should not be valid", "valid");
    }
  } else {
    ASSERT_EQUAL("encapsulateData Packet not valid", "");
  }
}

void packetMinTest() {
  std::vector<unsigned char> data{'o'};

  std::pair<SPWLPacket, bool> res = SPWLPacket::encapsulateData(data);
  if (res.second) {
    SPWLPacket packet{res.first};
    res = SPWLPacket::encapsulatePacket(packet.rawData());

    if (res.second) {
      ASSERT_EQUAL(data, res.first.getData());
    } else {
      ASSERT_EQUAL("Packet not valid", "");
    }
  } else {
    ASSERT_EQUAL("encapsulateData packet not valid", "");
  }
}

void packetMaxTest() {
  std::vector<unsigned char> data(SPWLPacket::MAXDATASIZE, 'h');

  std::pair<SPWLPacket, bool> res = SPWLPacket::encapsulateData(data);
  if (res.second) {
    SPWLPacket packet{res.first};
    res = SPWLPacket::encapsulatePacket(packet.rawData());
    if (res.second) {
      ASSERT_EQUAL(data, res.first.getData());
    } else {
      ASSERT_EQUAL("Packet not valid", "");
    }
  } else {
    ASSERT_EQUAL("encapsulateData packet not valid", "");
  }
}

void packetOverflowTest() {
  std::vector<unsigned char> data(SPWLPacket::MAXDATASIZE + 1, 'h');
  std::pair<SPWLPacket, bool> res = SPWLPacket::encapsulateData(data);
  ASSERT_EQUAL(res.second, false);
}

void lengthExtractorTest() {
  std::vector<unsigned char> data{'M', 'y', 'L', 'i', 't', 't', 'l', 'e',
                                  'C', 'p', 'p'};
  std::pair<SPWLPacket, bool> result = SPWLPacket::encapsulateData(data);
  if (result.second) {
    std::array<unsigned char, SPWLPacket::PACKETSIZE>
        rawData{result.first.rawData()};

    std::array<unsigned char, SPWLPacket::HEADERSIZE> header{};
    std::copy(rawData.cbegin() + SPWLPacket::PREAMBLESIZE,
        rawData.cbegin() + SPWLPacket::PREAMBLESIZE + SPWLPacket::HEADERSIZE,
        header.begin());
    uint16_t length = SPWLPacket::getLengthFromHeader(header);
    ASSERT_EQUAL(11, length);
  } else {
    ASSERT_EQUAL("encapsulateData packet not valid", "");
  }
}

void rawDataSizeTest() {
  std::vector<unsigned char> data{'M', 'y', 'L', 'i', 't', 't', 'l', 'e',
                                  'C', 'p', 'p'};
  std::pair<SPWLPacket, bool> result = SPWLPacket::encapsulateData(data);
  if (result.second) {
    ASSERT_EQUAL(7 + 6 + 11 + 2 + 1, result.first.rawDataSize());
  } else {
    ASSERT_EQUAL("encapsulateData packet not valid", "");
  }
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
