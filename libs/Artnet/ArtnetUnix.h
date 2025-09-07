#pragma once
#ifndef ARTNET_UNIX_H
#define ARTNET_UNIX_H

#define ARTNET_ENABLE_UNIX
// preludes
#include <chrono>
#include <stdint.h>
//
// #include "WString.h"
#include <iostream>
#include <string>
typedef std::string String;
#define F(x) x
#define HEX 16
#define constrain(amt, low, high)                                              \
  ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
struct Serial_T {
  void println(const String &s) { println(s.c_str()); };
  void println(const char *x) {
    print(x);
    std::cout << std::endl;
  };

  void println(int x, int format) { println(std::to_string(x)); };
  void print(const char *x) { std::cout << x; };
  void print(const String &s) { print(s.c_str()); };
};
Serial_T Serial;
long long millis() {
  auto now = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             now.time_since_epoch())
      .count();
}

//
#include "Artnet/util/ArxContainer/ArxContainer.h"
#include "IPAddress.h"

//
#include "Artnet/util/ArxTypeTraits/ArxTypeTraits.h"
// now lets include artnet
#include "Artnet/ArtnetCommon.h"
// #include
// "Artnet/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#include "UnixUDPSender.h"
// depends on Sender
#include "UnixUDPReceiver.h"

// using Artnet = arx::artnet::Manager<EthernetUDP>;
using ArtnetSender = arx::artnet::Sender<UDPSender>;
using ArtnetReceiver = arx::artnet::Receiver<UDPReceiver>;

#if 0
#include "PCAPUDPReceiver.h"
using ArtnetPCAPReceiver = arx::artnet::Receiver<PCAPReceiver>;
#endif

#endif // ARTNET_UNIX_H
