#include "Artnet/UnixUDPReceiver.h"

namespace parse {

template <typename T> uint8_t *type(uint8_t *data, T &dest) {
  dest = 0;
  for (int i = 0; i < sizeof(T); i++)
    dest += ((data[i] & 0xFF) << 8 * (sizeof(T) - 1 - i));
  return data + sizeof(T);
}

template <typename T>
static uint8_t *sizedVec(uint8_t *data, std::vector<T> &dest) {
  uint16_t size;
  data = type<uint16_t>(data, size);
  dest.clear();
  T elem;
  for (int i = 0; i < size; i++) {
    data = type<T>(data, elem);
    dest.push_back(elem);
  }

  return data;
}
}; // namespace parse
struct DMXMsg {

  std::vector<uint16_t> channels;
  std::vector<uint8_t> values;
  std::vector<uint8_t> destValues;
  uint16_t timeToDest = 0;
  bool looping = false;

  // used by runner
  unsigned long long startTime = 0;
  DMXMsg(uint8_t *data, int size) { parse(data, size); }

  void parse(uint8_t *data, int size) {
    data = parse::sizedVec(data, channels);
    data = parse::sizedVec(data, values);
    data = parse::sizedVec(data, destValues);
    data = parse::type(data, timeToDest);
  }

  static bool getLastElemOrErr(const std::vector<uint8_t> &v, int i,
                               uint8_t &out) {
    if (v.size() == 0)
      return false;
    out = v[std::min(v.size(), size_t(i))];
    return true;
  }

  void applyToDMXBuf(float pct, uint8_t *buf) {
    if(values.size()==0)return;
    for (int ch = 0; ch < channels.size(); ch++) {
      uint8_t startV = 0;
      uint8_t endV = 0;
      getLastElemOrErr(values, ch,startV);
      if (!getLastElemOrErr(destValues, ch, endV))
        endV = startV;
      uint8_t cur = uint8_t(startV + (float(endV) - float(startV))*pct);
      buf[channels[ch]] = cur;
    }
  }
};

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
        DMXMsg msg(d.data(), size);
        udpCallback(std::move(msg));

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
