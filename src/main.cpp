#include "Artnet.hpp"
#include "DMXInterface.hpp"
#include "MsgRunner.hpp"
#include "udpServer.hpp"

void usage() {
  std::cout << "--artnet\n\t enables artnet\n"
            << "--channels\n\t specify last dmx address\n"
            << std::endl;
}

int main(int argc, char **argv) {

  // conf
  bool enableArtnet = false;
  int maxNumCh = 512;
  for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == std::string("--help")) {
      usage();
      exit(0);
    } else if (std::string(argv[i]) == std::string("--artnet"))
      enableArtnet = true;
    else if (std::string(argv[i]) == std::string("--channels")) {
      i++;
      if (i >= argc) {
        usage();
        exit(1);
      }
      maxNumCh = std::stoi(std::string(argv[i]));
    }
  }

  DMXInterface dmx(maxNumCh);
  MsgRunner msgRunner(dmx.frame + 1);

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
