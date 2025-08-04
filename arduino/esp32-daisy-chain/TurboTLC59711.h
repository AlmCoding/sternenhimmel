#ifndef TURBOTLC59711_H_
#define TURBOTLC59711_H_

#include <Arduino.h>
#include <cstring>

template <size_t N>
class TurboTLC59711 {
 public:
  TurboTLC59711() = default;
  ~TurboTLC59711() = default;

  void init();
  void setBrightness(uint8_t bcr, uint8_t bcg, uint8_t bcb);
  bool setLed(uint8_t chip_idx, uint8_t led_idx, uint16_t r, uint16_t g, uint16_t b);

  const uint8_t* getChainBuffer() const;
  size_t getChainBufferSize() const;

 private:
  constexpr static size_t BytesPerDevice = 224 / 8;  // 224 bits / 8 bits per byte
  constexpr static size_t LedsPerDevice = 4;         // 4 RGB LEDs per device
  constexpr static size_t BcMaxValue = 127;          // Maximum brightness value for BC (7 bits)
  constexpr static size_t HeaderSize = 4;            // Size of the header in bytes

  constexpr static size_t OffsetGsr0 = 26;  // Offset for GR0
  constexpr static size_t OffsetGsg0 = 24;  // Offset for GG0
  constexpr static size_t OffsetGsb0 = 22;  // Offset for GB0

  constexpr static size_t OffsetGsr1 = 20;  // Offset for GR1
  constexpr static size_t OffsetGsg1 = 18;  // Offset for GG1
  constexpr static size_t OffsetGsb1 = 16;  // Offset for GB1

  constexpr static size_t OffsetGsr2 = 14;  // Offset for GR2
  constexpr static size_t OffsetGsg2 = 12;  // Offset for GG2
  constexpr static size_t OffsetGsb2 = 10;  // Offset for GB2

  constexpr static size_t OffsetGsr3 = 8;  // Offset for GR3
  constexpr static size_t OffsetGsg3 = 6;  // Offset for GG3
  constexpr static size_t OffsetGsb3 = 4;  // Offset for GB3

  constexpr static size_t GsrBytesPerLed = 6;  // 6 bytes per LED (2 bytes for each color)
  constexpr static size_t GsrBytesOffset = HeaderSize;

  struct Header {
    uint8_t cmd;  // 6 bits
    bool outtmg;  // 1 bit
    bool extgck;  // 1 bit
    bool tmgrst;  // 1 bit
    bool dsprpt;  // 1 bit
    bool blank;   // 1 bit
    uint8_t bcb;  // 7 bits
    uint8_t bcg;  // 7 bits
    uint8_t bcr;  // 7 bits

    uint32_t serialize() const {
      uint32_t data = 0;
      data |= cmd & 0x3F;  // 6 bits for command
      data <<= 1;
      data |= outtmg & 0x01;
      data <<= 1;
      data |= extgck & 0x01;
      data <<= 1;
      data |= tmgrst & 0x01;
      data <<= 1;
      data |= dsprpt & 0x01;
      data <<= 1;
      data |= blank & 0x01;
      data <<= 7;
      data |= bcb & 0x7F;  // 7 bits for BCB
      data <<= 7;
      data |= bcg & 0x7F;
      data <<= 7;
      data |= bcr & 0x7F;
      return data;
    }

    static Header deserialize(uint32_t data) {
      Header h;
      h.bcr = data & 0x7F;
      data >>= 7;
      h.bcg = data & 0x7F;
      data >>= 7;
      h.bcb = data & 0x7F;
      data >>= 7;
      h.blank = (data & 0x01) != 0;
      data >>= 1;
      h.dsprpt = (data & 0x01) != 0;
      data >>= 1;
      h.tmgrst = (data & 0x01) != 0;
      data >>= 1;
      h.extgck = (data & 0x01) != 0;
      data >>= 1;
      h.outtmg = (data & 0x01) != 0;
      data >>= 1;
      h.cmd = data & 0x3F;  // 6 bits for command
      return h;
    }
  };

  uint8_t chain_buffer_[N * BytesPerDevice];
};

template <size_t N>
void TurboTLC59711<N>::init() {
  std::memset(chain_buffer_, 0, sizeof(chain_buffer_));

  Header header;
  header.cmd = 0x25;
  header.outtmg = true;
  header.extgck = false;
  header.tmgrst = true;
  header.dsprpt = true;
  header.blank = false;
  header.bcb = BcMaxValue;
  header.bcg = BcMaxValue;
  header.bcr = BcMaxValue;

  uint32_t serialized_header = header.serialize();
  for (size_t i = 0; i < N; ++i) {
    size_t chip_offset = i * BytesPerDevice;
    // Each device has a 4-byte header at the start
    chain_buffer_[chip_offset] = static_cast<uint8_t>((serialized_header >> 24) & 0xFF);
    chain_buffer_[chip_offset + 1] = static_cast<uint8_t>((serialized_header >> 16) & 0xFF);
    chain_buffer_[chip_offset + 2] = static_cast<uint8_t>((serialized_header >> 8) & 0xFF);
    chain_buffer_[chip_offset + 3] = static_cast<uint8_t>(serialized_header & 0xFF);
  }
}

template <size_t N>
void TurboTLC59711<N>::setBrightness(uint8_t bcr, uint8_t bcg, uint8_t bcb) {
  if (bcr > BcMaxValue) {
    bcr = BcMaxValue;
  }
  if (bcg > BcMaxValue) {
    bcg = BcMaxValue;
  }
  if (bcb > BcMaxValue) {
    bcb = BcMaxValue;
  }

  // Read the existing header, modify brightness values
  uint32_t serialized_header = (chain_buffer_[0] << 24) |  //
                               (chain_buffer_[1] << 16) |  //
                               (chain_buffer_[2] << 8) |   //
                               chain_buffer_[3];
  Header header = Header::deserialize(serialized_header);
  header.bcr = bcr;
  header.bcg = bcg;
  header.bcb = bcb;
  serialized_header = header.serialize();

  for (size_t i = 0; i < N; ++i) {
    size_t chip_offset = i * BytesPerDevice;
    chain_buffer_[chip_offset] = static_cast<uint8_t>((serialized_header >> 24) & 0xFF);
    chain_buffer_[chip_offset + 1] = static_cast<uint8_t>((serialized_header >> 16) & 0xFF);
    chain_buffer_[chip_offset + 2] = static_cast<uint8_t>((serialized_header >> 8) & 0xFF);
    chain_buffer_[chip_offset + 3] = static_cast<uint8_t>(serialized_header & 0xFF);
  }
}

template <size_t N>
bool TurboTLC59711<N>::setLed(uint8_t chip_idx, uint8_t led_idx, uint16_t r, uint16_t g, uint16_t b) {
  if (chip_idx >= N || led_idx >= LedsPerDevice) {
    return false;  // Invalid chip or LED index
  }

  size_t chip_offset = chip_idx * BytesPerDevice;
  size_t inv_idx = (LedsPerDevice - 1) - led_idx;  // Invert led index

  size_t offest_gsb = chip_offset + GsrBytesOffset + (inv_idx * GsrBytesPerLed);
  size_t offset_gsg = offest_gsb + sizeof(uint16_t);
  size_t offset_gsr = offset_gsg + sizeof(uint16_t);

  chain_buffer_[offest_gsb] = static_cast<uint8_t>((b >> 8) & 0xFF);
  chain_buffer_[offest_gsb + 1] = static_cast<uint8_t>(b & 0xFF);
  chain_buffer_[offset_gsg] = static_cast<uint8_t>((g >> 8) & 0xFF);
  chain_buffer_[offset_gsg + 1] = static_cast<uint8_t>(g & 0xFF);
  chain_buffer_[offset_gsr] = static_cast<uint8_t>((r >> 8) & 0xFF);
  chain_buffer_[offset_gsr + 1] = static_cast<uint8_t>(r & 0xFF);
  return true;
}

template <size_t N>
const uint8_t* TurboTLC59711<N>::getChainBuffer() const {
  return chain_buffer_;
}

template <size_t N>
size_t TurboTLC59711<N>::getChainBufferSize() const {
  return sizeof(chain_buffer_);
}

#endif  // TURBOTLC59711_H_
