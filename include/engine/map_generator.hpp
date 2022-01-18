#pragma once
#include <random>

#include "engine/game_map.hpp"
#include "utils/cell.hpp"

namespace engine {
class MapGenerator {
 public:
  MapGenerator(size_t count_players, size_t width, size_t height,
               size_t seed = 0);
  MapGenerator(const MapGenerator&) = default;
  MapGenerator(MapGenerator&&) = default;
  MapGenerator& operator=(const MapGenerator&) = default;
  ~MapGenerator() = default;
  GameMap operator()();

  size_t count_players;
  size_t width;
  size_t height;

 private:
  bool CheckConnectivity(const GameMap&);
  void CheckConnectivityDFS(const GameMap&, std::vector<std::vector<bool>>&,
                            const utils::Cell&);
  std::mt19937 random_generator_;
};
}  // namespace engine
