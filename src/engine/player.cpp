#include "engine/player.hpp"

using namespace utils;

namespace engine {
Player& Player::operator=(const Cell& cell) {
  position = cell;
  return *this;
}
}  // namespace engine
