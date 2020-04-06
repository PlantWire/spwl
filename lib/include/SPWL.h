#ifndef LIB_INCLUDE_SPWLPACKAGE_H_
#define LIB_INCLUDE_SPWLPACKAGE_H_

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
  static constexpr int HEADERSIZE = 29;
  static constexpr int CHECKSUMSIZE = 16;
  static constexpr int PACKETSIZE = 512;
  static constexpr int MAXDATASIZE = 483;
  static constexpr int PREAMBLESIZE = 7;
  static constexpr char PREAMBLE[] = "UUUUUUU";

  SPWLPackage(uint16_t senderAddress, char channel,
      std::string data, bool last);

  SPWLPackage static encapsulateData(std::string data);

  std::optional<SPWLPackage> static encapsulatePackage(std::array<unsigned, PACKETSIZE> rawData);

  bool static checkPreamble(std::array<unsigned, PREAMBLESIZE> preamble);

  bool static checkChecksum(std::string checksum, std::string data);

  std::string static generateChecksum(std::string data);

  std::string getData() const;

  std::array<unsigned, PACKETSIZE> rawData() const;

  int rawDataSize() const;
};

#endif  // LIB_INCLUDE_SPWLPACKAGE_H_
