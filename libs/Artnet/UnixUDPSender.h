#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <iostream>
#include <memory.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct UDPSender {

  void begin(uint16_t p) {
    defaultPort = p;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
      Serial.println("socket could not be created");
      exit(1);
    }
  }
  // void stop() {}

  void beginPacket(IPAddress ip, uint16_t port) {
    in_addr addr;
    addr.s_addr = ip;
    beginPacket(inet_ntoa(addr), port);
  }
  void beginPacket(const char *host, uint16_t port) {

    memset(&addrDest, 0, sizeof(addrDest));

    // Filling server information
    addrDest.sin_family = AF_INET;
    addrDest.sin_port = htons(port);
    if (inet_aton(host, &addrDest.sin_addr) != 1) {
      Serial.println("could not set ip addr");
    }
    // addrDest.sin_addr.s_addr = INADDR_ANY;
  }
  // Finish off this packet and send it
  // Returns 1 if the packet was sent successfully, 0 if there was an error
  int endPacket() {
    auto res = sendto(sockfd, &msg, msgLen, 0, (const sockaddr *)&addrDest,
                      sizeof(addrDest));
    msgLen = 0;
    return res;
  }

  // Write size bytes from buffer into the packet
  size_t write(const uint8_t *buffer, size_t size) {
    size_t res = 0;
    for (int i = 0; i < size; i++) {
      msg[msgLen] = *(buffer + i);
      msgLen++;
      res++;
    }
    return res;
  }
  uint16_t defaultPort;
  int sockfd;

  // tmp for packet
  sockaddr_in addrDest;
  uint8_t msg[65507];
  size_t msgLen = 0;
};
