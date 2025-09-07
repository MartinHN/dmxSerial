#include "Artnet.hpp"
#include "DMXInterface.hpp"
#include "udpServer.hpp"
#include <mutex>

struct MsgRunner {

  MsgRunner(uint8_t *dmxData) : dmxData(dmxData) {}
  void addMsg(DMXMsg &&m) {
    std::scoped_lock lock(mutex);
    m.startTime = TimeInterval::getNow();
    msgs.push_back(m);
  }

  void doStep() {
    std::scoped_lock lock(mutex);
    auto now = TimeInterval::getNow();
    auto dt = now - lastNow;
    lastNow = now;

    for (auto &m : msgs) {
      float pct = 1;
      if (m.timeToDest > 0)
        pct = std::min(1.f, float(now - m.startTime) / m.timeToDest);
      m.applyToDMXBuf(pct, dmxData);

      if (pct == 1)
        m.timeToDest = 0;
    }

    for (int i = msgs.size() - 1; i >= 0; i--) {
      if (msgs[i].timeToDest == 0)
        msgs.erase(msgs.begin() + i);
    }
  }

  unsigned long long lastNow = 0;
  std::vector<DMXMsg> msgs;
  uint8_t *dmxData;
  std::mutex mutex;
};

int main(int argc, char **argv) {

  DMXInterface dmx(7 * 4);
  MsgRunner msgRunner(dmx.frame);

  // conf
  bool enableArtnet = false;
  for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == std::string("--artnet"))
      enableArtnet = true;
  }

  if (enableArtnet) {
    startArtnetThread([&dmx](const uint8_t *data, const uint16_t size) {
      if (size != 512) {
        std::cerr << "artnet size not defined" << std::endl;
        return;
      }
      // std::cout << "new frame from artnet : " << std::to_string(size)
      //           << std::endl;
      memcpy(dmx.frame + 1, data, size);
    });
  }

  startUDPThread([&msgRunner](DMXMsg &&m) { msgRunner.addMsg(std::move(m)); });
  // dmx.frame[1] = 255; // Channel 2
  // dmx.frame[2] = 255; // Channel 2
  // dmxFrame[3] = 255;           // Channel 1
  while (true) {

    if (!dmx.update())
      usleep(10 * 1000);
    else {
    }
    msgRunner.doStep();
  }
  dmx.close();

  return 0;
}
