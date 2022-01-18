namespace utils {
enum class Direction { UP, RIGHT, DOWN, LEFT };
struct Cell;
}  // namespace utils

#pragma once
#include <iostream>

namespace utils {
struct Cell {
  int x, y;
  Cell();
  Cell(const Cell&);
  Cell(Cell&&);
  Cell(int x, int y);

  Cell& operator=(const Cell&) = default;

  bool operator<(const Cell& cell) const;
  bool operator>(const Cell& cell) const;
  bool operator<=(const Cell& cell) const;
  bool operator>=(const Cell& cell) const;
  bool operator==(const Cell& cell) const;
  bool operator!=(const Cell& cell) const;

  Cell& operator+=(const Cell& cell);
  Cell& operator+=(Direction direction);
  Cell& operator-=(const Cell& cell);
  Cell& operator-=(Direction direction);

  Cell operator+(const Cell& cell) const;
  Cell operator+(Direction direction) const;
  Cell operator-(const Cell& cell) const;
  Cell operator-(Direction direction) const;
};
}  // namespace utils

namespace std {
std::string to_string(utils::Direction);
}
