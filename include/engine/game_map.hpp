#pragma once

namespace engine {
class GameMap;
}

#include <map>
#include <vector>

#include "utils/cell.hpp"
#include "engine/player.hpp"

namespace std {
std::string to_string(const engine::GameMap& map);
}

namespace engine {
namespace events {
class Event;
}

struct Treasure {
  utils::Cell position;
  size_t number;
};

class GameMap {
  friend class events::Event;
  friend std::string std::to_string(const GameMap&);
 public:
  GameMap() = default;
  GameMap(size_t count_players, size_t width, size_t height);
  GameMap(const GameMap&) = default;
  GameMap(GameMap&&) = default;
  GameMap& operator=(const GameMap&) = default;
  ~GameMap() = default;  

  bool& GetVerticalWall(const utils::Cell&);
  bool& GetHorizontalWall(const utils::Cell&);
  bool& GetWall(const utils::Cell&, utils::Direction);
  bool GetVerticalWall(const utils::Cell&) const;
  bool GetHorizontalWall(const utils::Cell&) const;
  bool GetWall(const utils::Cell&, utils::Direction) const;
  size_t GetCountPlayers() const;
  std::vector<Player>& GetPlayers();
  const std::vector<Player>& GetPlayers() const;
  void PutTreasure(const Treasure&);
  std::vector<size_t> RaiseTreasure(const utils::Cell&);
 private:
  size_t width_, height_;
  std::map<utils::Cell, bool> vertical_walls_, horizontal_walls_;
  std::vector<Player> players_;
  std::vector<Treasure> treasures_;
};
}

