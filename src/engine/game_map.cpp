#include <exception>

#include "engine/game_map.hpp"

using namespace utils;

namespace engine {
GameMap::GameMap(size_t count_players, size_t width, size_t height) :
    width_(width), height_(height) {
  players_.resize(count_players);
}

bool& GameMap::GetVerticalWall(const Cell& cell) {
  return vertical_walls_[cell];
}

bool& GameMap::GetHorizontalWall(const Cell& cell) {
  return horizontal_walls_[cell];
}

bool& GameMap::GetWall(const Cell& cell, Direction direction) {
  switch (direction) {
    case Direction::LEFT:
      return GetVerticalWall(cell);
    case Direction::UP:
      return GetHorizontalWall(cell);
    case Direction::RIGHT:
      return GetVerticalWall(cell + Cell(1, 0));
    case Direction::DOWN:
      return GetHorizontalWall(cell + Cell(0, 1));
  }
  throw std::logic_error("GetWall in GameMap dosn't was able to choose direction");
}

bool GameMap::GetVerticalWall(const Cell& cell) const {
  return const_cast<GameMap&>(*this).GetVerticalWall(cell);
}

bool GameMap::GetHorizontalWall(const Cell& cell) const {
  return const_cast<GameMap&>(*this).GetHorizontalWall(cell);
}

bool GameMap::GetWall(const Cell& cell, Direction direction) const {
  return const_cast<GameMap&>(*this).GetWall(cell, direction);
}

std::vector<Player>& GameMap::GetPlayers() {
  return players_;
}

const std::vector<Player>& GameMap::GetPlayers() const {
  return players_;
}

size_t GameMap::GetCountPlayers() const {
  return players_.size();
}

void GameMap::PutTreasure(const Treasure& treasure) {
  treasures_.push_back(treasure);
}

std::vector<size_t> GameMap::RaiseTreasure(const Cell& cell) {
  std::vector<size_t> result;
  for (auto it = treasures_.begin(); it != treasures_.end();) {
    if (it->position == cell) {
      result.push_back(it->number);
      std::swap(*it, *treasures_.rbegin());
      treasures_.pop_back();
    } else {
      it++;
    }
  }
  return result;
}
}

namespace std {
std::string to_string(const engine::GameMap& map) {
  std::string result;
  std::string red_color = "\033[01;31m", clear_color = "\033[00m";
  int min_x = 0, max_x = map.width_ - 1, min_y = 0, max_y = map.height_ - 1;

  for (const auto& player: map.players_) {
    min_x = std::min(min_x, player.position.x);
    min_y = std::min(min_y, player.position.y);
    max_x = std::max(max_x, player.position.x);
    max_y = std::max(max_y, player.position.y);
  }
  std::cout << min_x << " " << min_y << std::endl;

  for (int i = 2 * min_y; i < 2 * max_y + 3; ++i) {
    for (int j = 2 * min_x; j < 2 * max_x + 3; ++j) {
      if (i == 0 && j == 0) {
        result += red_color + "╔" + clear_color;
      } else if (i == 0 && j == 2 * map.width_) {
        result += red_color + "╗" + clear_color;
      } else if (i == 2 * map.height_ && j == 0) {
        result += red_color + "╚" + clear_color;
      } else if (i == 2 * map.height_ && j == 2 * map.width_) {
        result += red_color + "╝" + clear_color;
      } else if ((i == 0 || i == 2 * map.height_) && std::abs(j % 2) == 1) {
        if (map.GetHorizontalWall(Cell((j - 1) / 2, i / 2)))
          result += "═";
        else
          result += " ";
      } else if (i == 0) {
        result += "╦";
      } else if (std::abs(i % 2) == 0 && j == 0) {
        result += "╠";
      } else if (std::abs(j % 2) == 0 && i == 2 * map.height_) {
        result += "╩";
      } else if (std::abs(i % 2) == 0 && j == 2 * map.width_) {
        result += "╣";
      } else if (std::abs(i % 2) == 0 && std::abs(j % 2) == 0) {
        result += "╬";
      } else if (j == 0 || j == 2 * map.width_) {
        if (map.GetVerticalWall(Cell(j / 2, (i - 1) / 2)))
          result += "║";
        else
          result += " ";
      } else if (std::abs(i % 2) == 0) {
        if (map.GetHorizontalWall(Cell((j - 1) / 2, i / 2))) 
          result += "═";
        else
          result += " ";
      } else if (std::abs(j % 2) == 0) {
        if (map.GetVerticalWall(Cell(j / 2, (i - 1) / 2))) 
          result += "║";
        else
          result += " ";
      } else {
        int player = 0;
        for (size_t t = 0; t < map.players_.size(); ++t) {
          if (map.players_[t].position == Cell((j - 1) / 2, (i - 1) / 2)) {
            player |= (1 << t);
          }
        }
        bool treasure = false;
        for (const auto& t: map.treasures_) {
          if (t.position == Cell((j - 1) / 2, (i - 1) / 2)) {
            treasure = true;
          }
        }
        if (treasure)
          result += "#";
        else if (player == 0)
          result += " ";
        else
          result += std::to_string(player).back();
      }
    }
    result += "\n";
  }
  return result;
}
}

