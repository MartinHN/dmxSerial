#pragma once
#include <chrono>

struct TimeInterval {

  TimeInterval(int ms) : msBtwUpdate(ms) {}

  static unsigned long long getNow() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
        .count();
  }
  bool hasEllapsed() {
    auto now = getNow();
    if (now - lastValidTime > msBtwUpdate) {
      lastValidTime = now;
      return true;
    }
    return false;
  }

  int msBtwUpdate;
  unsigned long long lastValidTime = 0;
};
