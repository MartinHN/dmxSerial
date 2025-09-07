#include "SimpleDMXSender.hpp"
#include "TimeInterval.hpp"

struct DMXInterface {
  SimpleDMXSender dmxSender = {"ABADF5SD", 0x0403, 0x6001}; // ftdi vid,pid
  uint8_t frame[512 + 1] = {0};
  TimeInterval reopenInterval = {2000};
  DMXInterface(int _frameSize) : frameSize(_frameSize) {}
  bool update() {
    if (!dmxSender.isConnected()) {
      if (!reopenInterval.hasEllapsed())
        return false;
      if (!dmxSender.open())
        return false;
    }
    return sendCurrentFrame();
  }



  void close() { dmxSender.close(); }

  bool sendCurrentFrame() {
    dmxSender.setBreak(true);
    usleep(110);
    dmxSender.setBreak(false);
    usleep(16);
    if (dmxSender.sendDMXFrame(frame, frameSize)) {
      // std::cout << "DMX frame sent successfully." << std::endl;
    } else {
      std::cout << "DMX frame failed" << std::endl;
      return false;
    }
    usleep(100 + frameSize * 2);
    return true;
  }

  int frameSize;
};
