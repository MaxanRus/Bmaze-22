#include "utils/cell.hpp"

#include <tuple>

namespace utils {
Cell::Cell() : x(0), y(0) {}

Cell::Cell(const Cell& another) : x(another.x), y(another.y) {}

Cell::Cell(Cell&& another) : x(another.x), y(another.y) {}

Cell::Cell(int x, int y) : x(x), y(y) {}

bool Cell::operator<(const Cell& cell) const {
  return std::tie(x, y) < std::tie(cell.x, cell.y);
}

bool Cell::operator>(const Cell& cell) const { return cell < *this; }

bool Cell::operator<=(const Cell& cell) const { return !(cell < *this); }

bool Cell::operator>=(const Cell& cell) const { return !(*this < cell); }

bool Cell::operator==(const Cell& cell) const {
  return (*this <= cell) && (*this >= cell);
}

bool Cell::operator!=(const Cell& cell) const {
  return (*this < cell) || (cell < *this);
}

Cell& Cell::operator+=(const Cell& cell) {
  x += cell.x;
  y += cell.y;
  return *this;
}

Cell& Cell::operator+=(Direction direction) {
  switch (direction) {
    case Direction::UP:
      --y;
      break;
    case Direction::RIGHT:
      ++x;
      break;
    case Direction::DOWN:
      ++y;
      break;
    case Direction::LEFT:
      --x;
      break;
  }
  return *this;
}

Cell& Cell::operator-=(const Cell& cell) {
  x -= cell.x;
  y -= cell.y;
  return *this;
}

Cell& Cell::operator-=(Direction direction) {
  switch (direction) {
    case Direction::UP:
      ++y;
      break;
    case Direction::RIGHT:
      --x;
      break;
    case Direction::DOWN:
      --y;
      break;
    case Direction::LEFT:
      ++x;
      break;
  }
  return *this;
}

Cell Cell::operator+(const Cell& cell) const {
  Cell tmp(*this);
  tmp += cell;
  return tmp;
}

Cell Cell::operator+(Direction direction) const {
  Cell tmp(*this);
  tmp += direction;
  return tmp;
}

Cell Cell::operator-(const Cell& cell) const {
  Cell tmp(*this);
  tmp -= cell;
  return tmp;
}

Cell Cell::operator-(Direction direction) const {
  Cell tmp(*this);
  tmp -= direction;
  return tmp;
}
}  // namespace utils

namespace std {
std::string to_string(utils::Direction direction) {
  using namespace utils;
  switch (direction) {
    case Direction::UP:
      return "up";
    case Direction::RIGHT:
      return "right";
    case Direction::DOWN:
      return "down";
    case Direction::LEFT:
      return "left";
  }
  throw std::logic_error(
      "GetWall in GameMap dosn't was able to choose direction");
}
}  // namespace std
