#include "engine/map_generator.hpp"

using namespace utils;

namespace engine {
MapGenerator::MapGenerator(size_t count_players, size_t width, size_t height,
                           size_t seed) : count_players(count_players),
    width(width), height(height), random_generator_(seed) {}

GameMap MapGenerator::operator()() {
  GameMap result(count_players, width, height);

  for (size_t i = 0; i < width; ++i) {
    result.GetHorizontalWall(Cell(i, 0)) = true;
    result.GetHorizontalWall(Cell(i, height)) = true;
  }
  for (size_t i = 0; i < height; ++i) {
    result.GetVerticalWall(Cell(0, i)) = true;
    result.GetVerticalWall(Cell(width, i)) = true;
  }

  size_t desired_number_walls = 0.9 * (width - 1) * (height - 1);
  size_t current_number_walls = 0;

  while (current_number_walls < desired_number_walls) {
    Cell cell(random_generator_() % width, random_generator_() % height);
    Direction direction = static_cast<Direction>(random_generator_() % 4);

    if (!result.GetWall(cell, direction)) {
      result.GetWall(cell, direction) = true;
      if (!CheckConnectivity(result))
        result.GetWall(cell, direction) = false;
      else
        ++current_number_walls;
    } 
  }

  if (size_t number = random_generator_() % (width + height); number < width) {
    int y = random_generator_() % 2 == 0 ? 0 : height;
    result.GetHorizontalWall(Cell(number, y)) = false;
  } else {
    int x = random_generator_() % 2 == 0 ? 0 : height;
    result.GetHorizontalWall(Cell(x, number)) = false;
  }

  for (size_t i = 0; i <= count_players; ++i) {
    result.PutTreasure({Cell(random_generator_() % width, random_generator_() % height), i});
  }
  
  return result;
}

bool MapGenerator::CheckConnectivity(const GameMap& map) {
  std::vector<std::vector<bool>> used(width, std::vector<bool>(height, false));
  CheckConnectivityDFS(map, used, Cell(0, 0));
  bool result = true;
  for (size_t i = 0; i < width; ++i) {
    for (size_t j = 0; j < height; ++j) {
      result &= used[i][j];
    }
  }
  return result;
}

void MapGenerator::CheckConnectivityDFS(const GameMap& map,
                                        std::vector<std::vector<bool>>& used,
                                        const Cell& cell) {
  static Cell d[] = {Cell(-1, 0), Cell(0, 1), Cell(1, 0), Cell(0, -1)};
  static Direction directions[] = {Direction::LEFT, Direction::DOWN,
    Direction::RIGHT, Direction::UP};
  for (uint8_t i = 0; i < 4; ++i) {
    Cell next_cell = cell + d[i];
    if (next_cell.x < 0 || next_cell.x >= width || next_cell.y < 0 || next_cell.y >= height)
      continue;
    if (!used[next_cell.x][next_cell.y] && !map.GetWall(cell, directions[i])) {
      used[next_cell.x][next_cell.y] = true;
      CheckConnectivityDFS(map, used, next_cell);
    }
  }
}
}

