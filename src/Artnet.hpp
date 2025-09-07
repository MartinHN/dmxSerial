#include <cstring>

#include "../libs/Artnet/ArtnetUnix.h"

std::function<void(const uint8_t *data, const uint16_t size)> artnetCallback;
void callback(const uint8_t *data, const uint16_t size) {
  //   std::cout << " got art net on u1 + size = " << std::to_string(size) <<
  //   std::endl;
  artnetCallback(data, size);
}

void startArtnetThreadEP(void *) {
  std::cerr << "starting artnet thread " << std::endl;
  ArtnetReceiver artnet;
  artnet.begin();

  if (artnetCallback)
    artnet.subscribe(0, callback);
  else
    std::cerr << "no artnet callback defined " << std::endl;

  while (true) {
    artnet.parse();
  }

  std::cerr << "exiting artnet thread " << std::endl;
}

#include <thread>
std::thread *artnetThread = nullptr;
void startArtnetThread(
    std::function<void(const uint8_t *data, const uint16_t size)> cb) {
  artnetCallback = cb;
  artnetThread = new std::thread(startArtnetThreadEP, nullptr);
}
