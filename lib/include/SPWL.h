#ifndef LIB_INCLUDE_SPWL_H_
#define LIB_INCLUDE_SPWL_H_

#include <optional>
#include <string>
#include <array>

class SPWLPackage{
 private:
  std::string data;
  uint16_t senderAddress;
  uint16_t length;
  char channel;
  bool last;

 public:
  static constexpr int PACKETSIZE = 512;
  static constexpr int HEADERSIZE = 29;
  static constexpr int PREAMBLESIZE = 7;
  static constexpr int CHECKSUMSIZE = 16;
  static constexpr char TRAILERSIZE = 1;
  static constexpr int MAXDATASIZE = PACKETSIZE -
      PREAMBLESIZE - HEADERSIZE - TRAILERSIZE;
  static constexpr char PREAMBLE[] = "UUUUUUU";
  static constexpr unsigned char TRAILER = 4;

  SPWLPackage(uint16_t senderAddress, char channel,
      std::string data, bool last);

  SPWLPackage static encapsulateData(std::string data);

  std::optional<SPWLPackage> static
      encapsulatePackage(std::array<unsigned char, PACKETSIZE> rawData);

  bool static checkPreamble(std::array<unsigned char, PREAMBLESIZE> preamble);

  bool static checkChecksum(std::string checksum, std::string data);

  std::string static generateChecksum(std::string data);

  std::string getData() const;

  std::array<unsigned char, PACKETSIZE> rawData() const;

  int rawDataSize() const;
};

#endif  // LIB_INCLUDE_SPWL_H_
