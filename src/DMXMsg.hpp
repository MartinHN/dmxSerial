#pragma once
#include "../libs/parse.hpp"
#include <cmath>
#include <numbers>
struct DMXMsg {

  enum MSGType : uint8_t { channelRamp = 0, clearAll, TOTAL_NUM };
  MSGType msgType = channelRamp;
  std::vector<uint16_t> channels;
  std::vector<uint8_t> values;
  std::vector<uint8_t> destValues;
  uint16_t timeToDest = 0;
  enum CurveType : uint8_t { linear = 0, sine, square };
  CurveType curveType = linear;
  uint16_t curveOffset = 0;

  struct Flags {
    bool looping : 1;
    bool symetric : 1;
    bool addValue : 1;
    bool neverStop : 1;
  };
  Flags flags = {0};

  // used by runner
  unsigned long long startTime = 0;

  DMXMsg() {}

  uint8_t *parse(uint8_t *data, int size) {
    auto *ori = data;
    data = parse::parse_enum(data, msgType);
    if (msgType == MSGType::channelRamp)
      data = parseChannelRamp(data);
    else if (msgType == MSGType::clearAll) { // noop;
    } else {
      std::cerr << "unknown message type" << std::endl;
      return nullptr;
    }
    auto len = ptrdiff_t(data - ori);
    return data;
  }

  uint8_t *parseChannelRamp(uint8_t *data) {
    data = parse::sizedVec(data, channels);
    data = parse::sizedVec(data, values);
    data = parse::sizedVec(data, destValues);
    data = parse::parse(data, timeToDest);
    data = parse::parse_enum(data, curveType);
    data = parse::parse(data, curveOffset);
    data = parse::parse_union(data, flags);
    return data;
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

    if (curveOffset > 0 && flags.looping) {
      pct = fmodf(pct + curveOffset / 1000.f, 1);
      //   std::cout << std::to_string(pct) << std::endl;
    }
    if (flags.symetric) {
      pct *= 2.f;
      if (pct > 1.f)
        pct = 2.f - pct;
    }

    if (curveType == sine)
      pct = sinf(pct * std::numbers::pi / 2);
    else if (curveType == square)
      pct = pct > .5 ? 1.f : 0.f;

    for (int ch = 0; ch < channels.size(); ch++) {
      uint8_t startV = 0;
      uint8_t endV = 0;
      getLastElemOrErr(values, ch,startV);
      if (!getLastElemOrErr(destValues, ch, endV))
        endV = startV;

      uint8_t cur = uint8_t(startV + (float(endV) - float(startV))*pct);
      if (flags.addValue)
        buf[channels[ch] - 1] =
            std::max(0, std::min(255, buf[channels[ch] - 1] + cur));
      else
        buf[channels[ch] - 1] = cur;
    }
  }
};
