#include "SimpleDMXSender.hpp"
#include "TimeInterval.hpp"

struct DMXInterface {
  SimpleDMXSender dmxSender = {"ABADF5SD", 0x0403, 0x6001}; // ftdi vid,pid
  uint8_t frame[512 + 1] = {0};
  // uint8_t internalCopy[512 + 1] = {0};
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
    // usleep(110);
    usleep(92 + 0);
    dmxSender.setBreak(false);
    // usleep(16);
    usleep(12 + 0);
    // memcpy(internalCopy, frame, frameSize + 1);
    // if (dmxSender.sendDMXFrame(internalCopy, frameSize + 1)) {
    if (dmxSender.sendDMXFrame(frame, frameSize + 1)) {
      // std::cout << "DMX frame sent successfully." << std::endl;
    } else {
      std::cout << "DMX frame failed" << std::endl;
      return false;
    }
#if 1
    usleep(3000);
#else
    usleep(100 + frameSize * 3); // heuristic allowing to sleep less if only few
                                 // dmx address needs to be sent
#endif
    return true;
  }

  int frameSize;
};
