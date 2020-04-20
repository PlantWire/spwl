#ifndef LIB_INCLUDE_SPWL_H_
#define LIB_INCLUDE_SPWL_H_

#include <utility>
#include <string>
#include <array>

class SPWLPackage{
 public:
  static constexpr int PACKETSIZE = 512;
  static constexpr int HEADERSIZE = 38;
  static constexpr int PREAMBLESIZE = 7;
  static constexpr int CHECKSUMSIZE = 32;
  static constexpr char TRAILERSIZE = 1;
  static constexpr int MAXDATASIZE = PACKETSIZE -
                                     PREAMBLESIZE - HEADERSIZE - TRAILERSIZE;
  static constexpr char PREAMBLE[] = "UUUUUUU";
  static constexpr unsigned char TRAILER = 4;

 private:
  std::string data;
  uint16_t senderAddress;
  uint16_t length;
  char channel;
  bool last;
  std::array<unsigned char, CHECKSUMSIZE> checksum;
  SPWLPackage(uint16_t senderAddress, char channel,
              std::string data, bool last);
  static bool checkChecksum(std::array<unsigned char, CHECKSUMSIZE>
      checksum, std::string data);
  static std::array<unsigned char, CHECKSUMSIZE>
      generateChecksum(std::string data);

 public:
  std::pair<SPWLPackage, bool> static encapsulateData(std::string data);

  std::pair<SPWLPackage, bool> static
      encapsulatePackage(std::array<unsigned char, PACKETSIZE> rawData);

  bool static checkPreamble(std::array<unsigned char, PREAMBLESIZE> preamble);

  uint16_t static getLengthFromHeader(std::array<unsigned char, HEADERSIZE>
      header);

  std::string getData() const;

  std::array<unsigned char, PACKETSIZE> rawData() const;

  int rawDataSize() const;
};

#endif  // LIB_INCLUDE_SPWL_H_
