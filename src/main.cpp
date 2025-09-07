#include "Artnet.hpp"
#include "DMXInterface.hpp"

void ramp(uint8_t &v, uint8_t inc = 1) { v = uint8_t(v + inc); }
struct COBPar {
  COBPar(uint8_t *uni, int addr) : uni(uni), addr(addr) {}

  uint8_t &a_dim() { return uni[addr]; };
  uint8_t &a_r() { return uni[addr + 1]; };
  uint8_t &a_g() { return uni[addr + 2]; };
  uint8_t &a_b() { return uni[addr + 3]; };
  void setDim(uint8_t d) { a_dim() = d; }
  void setRGB(uint8_t r, uint8_t g, uint8_t b) {
    a_r() = r;
    a_g() = g;
    a_b() = b;
  }

  uint8_t *uni;
  int addr;
};

// Example usage
int main() {

  DMXInterface dmx(14);

  // startThread();
  startArtnetThread([&dmx](const uint8_t *data, const uint16_t size) {
    if (size != 512) {
      std::cerr << "artnet size not defined" << std::endl;
      return;
    }
    std::cerr << "new frame from artnet : " << std::to_string(size)
              << std::endl;
    memcpy(dmx.frame + 1, data, size);
  });
  COBPar par(dmx.frame, 1);
  par.setDim(255);
  par.setRGB(255, 0, 0);
  // dmx.frame[1] = 255; // Channel 2
  // dmx.frame[2] = 255; // Channel 2
  // dmxFrame[3] = 255;           // Channel 1
  while (true) {

    if (!dmx.update())
      usleep(10 * 1000);
    else {
      // ramp(par.a_r());
      // ramp(par.a_dim(), -2);
    }
  }
  dmx.close();

  return 0;
}
