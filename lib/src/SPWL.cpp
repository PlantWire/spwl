#include "../include/SPWL.h"
#include <algorithm>
#include <stdexcept>
#include <string>

SPWLPackage::SPWLPackage(uint16_t senderAddress, char channel,
    std::string data, bool last = false) {
  this->senderAddress = senderAddress;
  this->channel = channel;
  this->data = data;
  this->last = last;
  this->length = data.size();
}

std::string SPWLPackage::getData() const {
  return this->data;
}

int SPWLPackage::rawDataSize() const {
  return HEADERSIZE + this->data.size() + TRAILERSIZE;
}

std::array<unsigned char, SPWLPackage::PACKETSIZE> SPWLPackage::
    rawData() const {
  std::array<unsigned char, PACKETSIZE> output{};

  for (size_t i = 0; i < PREAMBLESIZE; i++) {
    output.at(i) = PREAMBLE[i];
  }

  output.at(7) = static_cast<unsigned char>(this->senderAddress >> 8);
  output.at(8) = static_cast<unsigned char>(this->senderAddress);

  output.at(9) = this->channel;

  output.at(10) = static_cast<unsigned char>(this->length >> 8);
  output.at(11) = static_cast<unsigned char>(this->length);

  unsigned char last = 0;
  if (this->last) {
    last = 255;
  }
  output.at(12) = last;

  auto outputIter = output.begin();
  std::advance(outputIter, 13);
  std::string checksum = generateChecksum(this->data);
  outputIter = std::copy(checksum.cbegin(), checksum.cend(), outputIter);

  std::copy(this->data.cbegin(), this->data.cend(), outputIter);

  output.at(HEADERSIZE + this->data.size()) = TRAILER;

  return output;
}

std::pair<SPWLPackage, bool> SPWLPackage::encapsulateData(std::string data) {
  if (data.size() > MAXDATASIZE) {
    SPWLPackage package{0, 0, "", 0};
    std::pair<SPWLPackage, bool> result{package, false};
    return result;
  }
  SPWLPackage package{8, 24, data, true};
  std::pair<SPWLPackage, bool> result{package, true};
  return result;
}

std::pair<SPWLPackage, bool> SPWLPackage::
  encapsulatePackage(std::array<unsigned char, PACKETSIZE> rawData) {
  std::array<unsigned char, PREAMBLESIZE> preamble;
  std::copy(rawData.begin(), rawData.begin() + PREAMBLESIZE, preamble.begin());

  if (checkPreamble(preamble)) {
    uint16_t senderAddress = rawData.at(7) << 8;
    senderAddress += rawData.at(8);

    char channel = rawData.at(9);

    uint16_t dataLenght = rawData.at(10) << 8;
    dataLenght += rawData.at(11);

    bool last = false;
    if (rawData.at(12) == 255) {
      last = true;
    }

    if (dataLenght <= MAXDATASIZE) {
      std::string checksum{rawData.begin() + 13,
                           rawData.begin() + 13 + CHECKSUMSIZE};
      std::string data{rawData.begin() + HEADERSIZE,
                       rawData.begin() + HEADERSIZE + dataLenght};

      if (checkChecksum(checksum, data)) {
        SPWLPackage package{senderAddress, channel, data, last};
        std::pair<SPWLPackage, bool> result{package, true};
        return result;
      }
    }
  }
  SPWLPackage package{0, 0, 0, 0};
  std::pair<SPWLPackage, bool> result{package, false};
  return result;
}

bool SPWLPackage::
    checkPreamble(std::array<unsigned char, PREAMBLESIZE> preamble) {
  for (size_t i = 0; i < PREAMBLESIZE; i++) {
    if (preamble[i] != PREAMBLE[i]) {
      return false;
    }
  }
  return true;
}

bool SPWLPackage::checkChecksum(std::string checksum, std::string data) {
  return (checksum == generateChecksum(data));
}

std::string SPWLPackage::generateChecksum(std::string data) {
  return "HelloWorld!!!!!!";
}
