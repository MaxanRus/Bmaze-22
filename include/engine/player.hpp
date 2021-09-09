#pragma once
#include "utils/cell.hpp"
#include <optional>

namespace engine {
struct Player {
  Player() = default;
  Player(const Player&) = default;
  Player(Player&&) = default;
  Player(const utils::Cell& position) : position(position) {}

  Player& operator=(const Player&) = default;
  Player& operator=(const utils::Cell& cell);

  ~Player() = default;

  utils::Cell position;
  std::optional<size_t> treasure = std::nullopt;
};
}

