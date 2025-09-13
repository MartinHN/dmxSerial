#pragma once

namespace parse {

template <typename T> uint8_t *parse(uint8_t *data, T &dest) {
  dest = 0;
  for (int i = 0; i < sizeof(T); i++)
    dest += ((data[i] & 0xFF) << 8 * (sizeof(T) - 1 - i));
  return data + sizeof(T);
}

template <typename T> uint8_t *parse_enum(uint8_t *data, T &dest) {
  static_assert(sizeof(T) == 1);
  dest = T(data[0]);
  return data + sizeof(T);
}

template <typename T> uint8_t *parse_union(uint8_t *data, T &dest) {
  union Union {
    uint8_t bytes;
    T val;
  } un;
  static_assert(sizeof(T) == 1);
  un.bytes = data[0];
  dest = un.val;
  return data + sizeof(T);
}

template <typename T>
static uint8_t *sizedVec(uint8_t *data, std::vector<T> &dest) {
  uint16_t size;
  data = parse<uint16_t>(data, size);
  dest.clear();
  T elem;
  for (int i = 0; i < size; i++) {
    data = parse<T>(data, elem);
    dest.push_back(elem);
  }

  return data;
}
}; // namespace parse
