#include "DMXMsg.hpp"
#include <mutex>

struct MsgRunner {
  MsgRunner(uint8_t *dmxData) : dmxData(dmxData) {}
  void addMsg(DMXMsg &&m) {
    std::scoped_lock lock(mutex);
    if (m.msgType == DMXMsg::MSGType::clearAll) {
      msgs.clear();
      memset(dmxData, 0, 512);
      return;
    }
    m.startTime = TimeInterval::getNow();
    if (!m.flags.looping)
      m.startTime += m.curveOffset;
    if (m.flags.looping || m.flags.neverStop) {
      for (int i = msgs.size() - 1; i >= 0; i--) {
        if ((msgs[i].flags.looping || msgs[i].flags.neverStop) &&
            (msgs[i].channels == m.channels))
          msgs.erase(msgs.begin() + i);
      }
      msgs.insert(msgs.begin(), m);
    } else
      msgs.push_back(m);

    std::cout << "total msg : " << std::to_string(msgs.size()) << std::endl;
  }

  void doStep() {
    std::scoped_lock lock(mutex);
    auto now = TimeInterval::getNow();
    auto dt = now - lastNow;
    lastNow = now;

    for (auto &m : msgs) {
      float pct = 1;
      if (m.startTime > now)
        continue;
      if (m.timeToDest > 0)
        pct = std::min(1.f, float(now - m.startTime) / m.timeToDest);
      m.applyToDMXBuf(pct, dmxData);

      if (pct == 1) {
        if (m.flags.looping)
          m.startTime = now;
        else
          m.timeToDest = 0;
      }
    }

    for (int i = msgs.size() - 1; i >= 0; i--) {
      if (msgs[i].timeToDest == 0 && !msgs[i].flags.neverStop)
        msgs.erase(msgs.begin() + i);
    }
  }

  unsigned long long lastNow = 0;
  std::vector<DMXMsg> msgs;
  uint8_t *dmxData;
  std::mutex mutex;
};
