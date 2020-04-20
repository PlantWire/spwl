#include "../include/SPWL.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <openssl/md5.h>

SPWLPackage::SPWLPackage(uint16_t senderAddress, char channel,
    std::string data, bool last = false) {
  this->senderAddress = senderAddress;
  this->channel = channel;
  this->data = data;
  this->last = last;
  this->length = data.size();
  this->checksum = this->generateChecksum();
}

std::string SPWLPackage::getData() const {
  return this->data;
}

int SPWLPackage::rawDataSize() const {
  return PREAMBLESIZE + HEADERSIZE + this->data.size() + TRAILERSIZE;
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
  outputIter = std::copy(this->checksum.cbegin(), this->checksum.cend(), outputIter);

  std::copy(this->data.cbegin(), this->data.cend(), outputIter);

  output.at(PREAMBLESIZE + HEADERSIZE + this->data.size()) = TRAILER;

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
      this->checksum{rawData.begin() + 13,
                           rawData.begin() + 13 + CHECKSUMSIZE};
      std::string data{rawData.begin() + PREAMBLESIZE + HEADERSIZE,
                       rawData.begin() + PREAMBLESIZE + HEADERSIZE
                       + dataLenght};

      if (checkChecksum(checksum, data)) {
        SPWLPackage package{senderAddress, channel, data, last};
        std::pair<SPWLPackage, bool> result{package, true};
        return result;
      }
    }
  }
  SPWLPackage package{0, 0, "", 0};
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

uint16_t SPWLPackage::getLengthFromHeader(std::array<unsigned char, HEADERSIZE>
    header) {
  uint16_t length = header.at(3) << 8;
  length += header.at(4);
  return length;
}

bool SPWLPackage::checkChecksum() {
  return (this->checksum == generateChecksum(data));
}

std::array<unsigned char, CHECKSUMSIZE> SPWLPackage::generateChecksum() {
  // ToDo(ckirchme): generate md5 from infos. Also try to remove lcrypto dependency from exe.
  const unsigned char * input = (unsigned char *) data.c_str();
  unsigned char * result = static_cast<unsigned char *>(malloc(MD5_DIGEST_LENGTH));
  MD5(input, static_cast<unsigned long>(data.size()), result);
  std::string strResult((char *) result, MD5_DIGEST_LENGTH);
  free(result);
  return strResult;
}
