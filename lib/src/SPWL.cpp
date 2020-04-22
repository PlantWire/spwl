#include <algorithm>
#include <stdexcept>
#include <string>

#include "../include/SPWL.h"
#include "../include/crc16.h"

#include <iostream>

SPWLPacket::SPWLPacket(uint16_t senderAddress, char channel,
    DataContainer data, bool last = false) {
  this->senderAddress = senderAddress;
  this->channel = channel;
  this->data = data;
  this->last = last;
  this->checksum = generateChecksum(data);
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

  output.at(7) = static_cast<unsigned char>(this->senderAddress >> 8);
  output.at(8) = static_cast<unsigned char>(this->senderAddress);

  output.at(9) = this->channel;

  output.at(10) = static_cast<unsigned char>(this->data.size() >> 8);
  output.at(11) = static_cast<unsigned char>(this->data.size());

  unsigned char last = 0;
  if (this->last) {
    last = 255;
  }
  output.at(12) = last;

  auto outputIter = output.begin();
  std::advance(outputIter, 13);
  outputIter = std::copy(this->data.cbegin(), this->data.cend(), outputIter);
  std::copy(this->checksum.cbegin(), this->checksum.cend(), outputIter);

  output.at(PREAMBLESIZE + HEADERSIZE + this->data.size() + CHECKSUMSIZE)
      = TRAILER;

  return output;
}

std::pair<SPWLPacket, bool> SPWLPacket::encapsulateData(DataContainer data) {
  if (data.size() > MAXDATASIZE) {
    SPWLPacket packet{0, 0, DataContainer{}, false};
    std::pair<SPWLPacket, bool> result{packet, false};
    return result;
  }
  SPWLPacket packet{8, 24, data, true};
  std::pair<SPWLPacket, bool> result{packet, true};
  return result;
}

std::pair<SPWLPacket, bool> SPWLPacket::
    encapsulatePacket(PacketContainer rawData) {
  PreambleContainer preamble{};
  std::copy(rawData.begin(), rawData.begin() + PREAMBLESIZE, preamble.begin());

  if (checkPreamble(preamble)) {
    uint16_t senderAddress = rawData.at(0) << 8;
    senderAddress += rawData.at(8);

    char channel = rawData.at(9);

    uint16_t dataLenght = rawData.at(10) << 8;
    dataLenght += rawData.at(11);

    bool last = false;
    if (rawData.at(12) == 255) {
      last = true;
    }
    if (dataLenght <= MAXDATASIZE) {
      DataContainer data{};
      std::copy(rawData.cbegin() + PREAMBLESIZE + HEADERSIZE,
                rawData.cbegin() + PREAMBLESIZE + HEADERSIZE + dataLenght,
                std::back_inserter(data));

      ChecksumContainer checksum{};
      std::copy(rawData.cbegin() + PREAMBLESIZE + HEADERSIZE + dataLenght,
                rawData.cbegin() + PREAMBLESIZE + HEADERSIZE + dataLenght
                + CHECKSUMSIZE, checksum.begin());

      if (checkChecksum(checksum, data)) {
        SPWLPacket packet{senderAddress, channel, data, last};
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

uint16_t SPWLPacket::getLengthFromHeader(HeaderContainer header) {
  uint16_t length = header.at(3) << 8;
  length += header.at(4);
  return length;
}

bool SPWLPacket::checkChecksum(ChecksumContainer checksum, DataContainer data) {
  return (checksum == generateChecksum(data));
}

SPWLPacket::ChecksumContainer SPWLPacket::generateChecksum(DataContainer data) {
  CRC16 crc{};
  PacketContainer input;
  std::copy(data.cbegin(), data.cend(), input.begin());
  crc.update(input, data.size());
  ChecksumContainer result;
  result.at(0) = static_cast<uint8_t>(crc.get() >> 8);
  result.at(1) = static_cast<uint8_t>(crc.get());
  return result;
}
