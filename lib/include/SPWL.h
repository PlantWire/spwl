#ifndef LIB_INCLUDE_SPWL_H_
#define LIB_INCLUDE_SPWL_H_

#include <utility>
#include <array>
#include <vector>

typedef struct {
  uint16_t senderAddress;
  uint16_t dataSize;
  char channel;
  bool last;
} SPWLHeader;

class SPWLPacket{
 public:
  static constexpr int PACKETSIZE = 512;
  static constexpr int HEADERSIZE = 6;
  static constexpr int PREAMBLESIZE = 7;
  static constexpr int CHECKSUMSIZE = 2;
  static constexpr char TRAILERSIZE = 1;
  static constexpr int MAXDATASIZE = PACKETSIZE - PREAMBLESIZE - HEADERSIZE
                                      - CHECKSUMSIZE - TRAILERSIZE;
  static constexpr char PREAMBLE[] = "UUUUUUU";
  static constexpr unsigned char TRAILER = 4;

  using PacketContainer = std::array<unsigned char, PACKETSIZE>;
  using HeaderContainer = std::array<unsigned char, HEADERSIZE>;
  using PreambleContainer = std::array<unsigned char, PREAMBLESIZE>;
  using DataContainer = std::vector<unsigned char>;
  using ChecksumContainer = std::array<unsigned char, CHECKSUMSIZE>;

 private:
  static constexpr int CHANNEL = 23;
  static constexpr uint16_t ADDRESS = 8;

  DataContainer data;
  SPWLHeader header{};
  ChecksumContainer checksum;
  SPWLPacket(uint16_t senderAddress, char channel,
      DataContainer data, bool last);
  static bool checkChecksum(const ChecksumContainer& checksum,
      const HeaderContainer& header, const DataContainer& data);
  static ChecksumContainer generateChecksum(const HeaderContainer& header,
      const DataContainer& data);
  static HeaderContainer getRawFromHeader(const SPWLHeader& header);

 public:
  std::pair<SPWLPacket, bool> static encapsulateData(const DataContainer& data);

  std::pair<SPWLPacket, bool> static encapsulatePacket(
      const PacketContainer& rawData);

  bool static checkPreamble(PreambleContainer preamble);

  SPWLHeader static getHeaderFromRaw(const HeaderContainer& header);

  DataContainer getData() const;

  PacketContainer rawData() const;

  int rawDataSize() const;
};

#endif  // LIB_INCLUDE_SPWL_H_
