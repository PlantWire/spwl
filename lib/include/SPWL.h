#ifndef LIB_INCLUDE_SPWL_H_
#define LIB_INCLUDE_SPWL_H_

#include <utility>
#include <array>
#include <vector>

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
  DataContainer data;
  uint16_t senderAddress;
  uint16_t length;
  char channel;
  bool last;
  ChecksumContainer checksum;
  SPWLPacket(uint16_t senderAddress, char channel,
      DataContainer data, bool last);
  static bool checkChecksum(ChecksumContainer checksum, DataContainer data);
  static ChecksumContainer generateChecksum(DataContainer data);

 public:
  std::pair<SPWLPacket, bool> static encapsulateData(DataContainer data);

  std::pair<SPWLPacket, bool> static encapsulatePacket(PacketContainer rawData);

  bool static checkPreamble(PreambleContainer preamble);

  uint16_t static getLengthFromHeader(HeaderContainer header);

  DataContainer getData() const;

  PacketContainer rawData() const;

  int rawDataSize() const;
};

#endif  // LIB_INCLUDE_SPWL_H_
