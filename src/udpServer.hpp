#include "Artnet/UnixUDPReceiver.h"
#include "DMXMsg.hpp"

std::function<void( DMXMsg &&msg)> udpCallback;
void UDPthreadEP(void *) {
  std::cerr << "starting udp thread " << std::endl;

  UDPReceiver server;
  server.begin(5555);
  while (true) {
    try {
      int size = server.parsePacket();
      if (size > 0) {
        auto d = std::vector<uint8_t>(size);
        server.read(d.data(), size);
        uint8_t *data = d.data();
        uint16_t numMsgs = 0;
        data = parse::parse(data, numMsgs);
        for (int i = 0; i < numMsgs; i++) {
          DMXMsg msg;
          data = msg.parse(data, size);
          if (data == nullptr) {
            std::cerr << "got invalid dmx udp msg " << std::endl;
            break;
          } else
            udpCallback(std::move(msg));
        }
        auto remaining = ptrdiff_t(d.data() + size - data);
        if (remaining) {
          std::cerr << "weird remaining part in udp msg " << std::endl;
        }

      } else {
        usleep(100);
      }

      // handlePacket(OSCPP::Server::Packet(buffer.data(), kMaxPacketSize));
    } catch (std::exception &e) {
      std::cerr << "Exception: " << e.what() << std::endl;
    };
  }
}

#include <thread>
std::thread *udpThread = nullptr;
void startUDPThread(std::function<void( DMXMsg &&msg)> cb) {
  udpCallback = cb;
  udpThread = new std::thread(UDPthreadEP, nullptr);
}
