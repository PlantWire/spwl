#include <algorithm>
#include <stdexcept>
#include <string>

#include "../include/SPWL.h"
#include "../include/crc16.h"

SPWLPacket::SPWLPacket(uint16_t senderAddress, char channel,
    DataContainer data, bool last = false) {
  this->header.senderAddress = senderAddress;
  this->header.channel = channel;
  this->header.last = last;
  this->header.dataSize = data.size();
  this->data = data;
  this->checksum = generateChecksum(getRawFromHeader(header), data);
}

std::vector<unsigned char> SPWLPacket::getData() const {
  return this->data;
}

int SPWLPacket::rawDataSize() const {
  return PREAMBLESIZE + HEADERSIZE + this->data.size()
      + CHECKSUMSIZE + TRAILERSIZE;
}

SPWLPacket::PacketContainer SPWLPacket::rawData() const {
  std::array<unsigned char, PACKETSIZE> output{};

  for (size_t i = 0; i < PREAMBLESIZE; i++) {
    output.at(i) = PREAMBLE[i];
  }

  HeaderContainer header = getRawFromHeader(this->header);

  auto outputIter = output.begin();
  std::advance(outputIter, PREAMBLESIZE);
  outputIter = std::copy(header.cbegin(), header.cend(), outputIter);
  outputIter = std::copy(this->data.cbegin(), this->data.cend(), outputIter);
  outputIter = std::copy(this->checksum.cbegin(), this->checksum.cend(),
      outputIter);
  *outputIter = TRAILER;

  return output;
}

std::pair<SPWLPacket, bool> SPWLPacket::encapsulateData(
    const DataContainer& data) {
  if (data.size() > MAXDATASIZE) {
    SPWLPacket packet{0, 0, DataContainer{}, false};
    std::pair<SPWLPacket, bool> result{packet, false};
    return result;
  }
  SPWLPacket packet{ADDRESS, CHANNEL, data, true};
  std::pair<SPWLPacket, bool> result{packet, true};
  return result;
}

std::pair<SPWLPacket, bool> SPWLPacket::
    encapsulatePacket(const PacketContainer& rawData) {
  PreambleContainer preamble{};
  std::copy(rawData.begin(), rawData.begin() + PREAMBLESIZE, preamble.begin());

  if (checkPreamble(preamble)) {
    auto inputIterator = rawData.cbegin();
    std::advance(inputIterator, PREAMBLESIZE);

    HeaderContainer rawHeader{};
    std::copy(inputIterator, inputIterator + HEADERSIZE, rawHeader.begin());
    std::advance(inputIterator, HEADERSIZE);
    SPWLHeader header = getHeaderFromRaw(rawHeader);

    if (header.dataSize <= MAXDATASIZE) {
      DataContainer data{};
      std::copy(inputIterator, inputIterator + header.dataSize,
          std::back_inserter(data));
      std::advance(inputIterator, header.dataSize);

      ChecksumContainer checksum{};
      std::copy(inputIterator, inputIterator + CHECKSUMSIZE, checksum.begin());

      if (checkChecksum(checksum, rawHeader, data)) {
        SPWLPacket packet{header.senderAddress, header.channel, data,
            header.last};
        std::pair<SPWLPacket, bool> result{packet, true};
        return result;
      }
    }
  }
  SPWLPacket packet{0, 0, DataContainer{}, false};
  std::pair<SPWLPacket, bool> result{packet, false};
  return result;
}

bool SPWLPacket::
    checkPreamble(PreambleContainer preamble) {
  for (size_t i = 0; i < PREAMBLESIZE; i++) {
    if (preamble[i] != PREAMBLE[i]) {
      return false;
    }
  }
  return true;
}

uint16_t SPWLPacket::getLengthFromHeader(const HeaderContainer& header) {
  SPWLHeader result = getHeaderFromRaw(header);
  return result.dataSize;
}

SPWLPacket::HeaderContainer SPWLPacket::getRawFromHeader(
    const SPWLHeader& header) {
  HeaderContainer result{};

  result.at(0) = static_cast<unsigned char>(header.senderAddress >> 8);
  result.at(1) = static_cast<unsigned char>(header.senderAddress);

  result.at(2) = header.channel;

  result.at(3) = static_cast<unsigned char>(header.dataSize >> 8);
  result.at(4) = static_cast<unsigned char>(header.dataSize);

  unsigned char last = 0;
  if (header.last) {
    last = 255;
  }
  result.at(5) = last;

  return result;
}

SPWLHeader SPWLPacket::getHeaderFromRaw(const HeaderContainer& header) {
  SPWLHeader result{};

  result.senderAddress = header.at(0) << 8;
  result.senderAddress += header.at(1);

  result.channel = header.at(2);

  result.dataSize = header.at(3) << 8;
  result.dataSize += header.at(4);

  result.last = false;
  if (header.at(5) == 255) {
    result.last = true;
  }

  return result;
}

bool SPWLPacket::checkChecksum(const ChecksumContainer& checksum,
    const HeaderContainer& header, const DataContainer& data) {
  return (checksum == generateChecksum(header, data));
}

SPWLPacket::ChecksumContainer SPWLPacket::generateChecksum(
    const HeaderContainer& header, const DataContainer& data) {
  CRC16 crc{};
  PacketContainer input;
  auto outputIter = std::copy(header.cbegin(), header.cend(), input.begin());
  std::copy(data.cbegin(), data.cend(), outputIter);
  crc.update(input, HEADERSIZE + data.size());
  ChecksumContainer result;
  result.at(0) = static_cast<uint8_t>(crc.get() >> 8);
  result.at(1) = static_cast<uint8_t>(crc.get());
  return result;
}
