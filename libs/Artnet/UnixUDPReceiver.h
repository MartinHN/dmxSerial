#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <iostream>
#include <memory.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct UDPReceiver {

  ~UDPReceiver() {}
  void begin(uint16_t p) {
    port = p;
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
      Serial.println("socket could not be created");
      exit(1);
    }

    int dummy = 1;
    // if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &dummy, sizeof(dummy)) < 0)
    //   Serial.println("could not use reuse option");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &dummy, sizeof(dummy)) < 0)
      Serial.println("could not use reuse option");

    // just use the same sock fd
    sender.sockfd = sockfd;

    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
        0) {
      perror("bind failed");
      exit(EXIT_FAILURE);
    }
  }
  void stop(uint16_t p) { port = p; }
  int parsePacket() {
    int bytes = 0;
    lastBytes = 0;
    if (ioctl(sockfd, FIONREAD, &bytes) == -1)
      return 0;

    socklen_t cliLen = sizeof(cliaddr); // len is value/result
    auto n = recvfrom(sockfd, &rcvBuffer, bytes, MSG_PEEK,
                      (struct sockaddr *)&cliaddr, &cliLen);
    lastBytes = n;
    if (n < 0) {
      Serial.println("nothing to read");
      n = 0;
    }
    return n;
  }
  int lastBytes = 0;
  static constexpr int MAXMSGSIZE = 10000;
  char rcvBuffer[MAXMSGSIZE];
  // Number of bytes remaining in the current packet
  //  int available() =0;
  // Read a single byte from the current packet
  // virtual int read() =0;

  // Read up to len characters from the current packet and place them into
  // buffer Returns the number of characters read, or 0 if none are
  // available
  int read(uint8_t *buffer, size_t len) {
    socklen_t cliLen = sizeof(cliaddr); // len is value/result
    auto n = recvfrom(sockfd, (char *)buffer, len, MSG_WAITALL,
                      (struct sockaddr *)&cliaddr, &cliLen);
    lastN = n;
    if (n != lastBytes) {
      Serial.println("bug");
    }
    return n;
  }
  int lastN = 0;

  uint16_t remotePort() const { return cliaddr.sin_port; }
  IPAddress remoteIP() const { return IPAddress(cliaddr.sin_addr.s_addr); }
  IPAddress localIp() const {
    in_addr addr;
    inet_pton(AF_INET, "127.0.0.1", &addr);
    return IPAddress(addr.s_addr);
  }

  void beginPacket(IPAddress addr, uint16_t port) {
    sender.beginPacket(addr, port);
  }
  void beginPacket(const char *host, uint16_t port) {
    sender.beginPacket(host, port);
  }
  // Finish off this packet and send it
  // Returns 1 if the packet was sent successfully, 0 if there was an error
  int endPacket() { return sender.endPacket(); }

  // Write size bytes from buffer into the packet
  size_t write(const uint8_t *buffer, size_t size) {
    return sender.write(buffer, size);
  }

  uint16_t port;
  int sockfd;

  // tmp for packet
  sockaddr_in serverAddr, cliaddr;
  uint8_t msg[65507];
  size_t msgLen = 0;

  UDPSender sender;
};
