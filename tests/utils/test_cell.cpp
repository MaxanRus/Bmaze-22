#include <gtest/gtest.h>
#include "utils/cell.hpp" 

TEST(Cell, TestComparisonCell) {
  const int N = 10;
  Cell cells[N][N];

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      cells[i][j] = Cell(i, j);
    }
  }

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      ASSERT_EQ(cells[i][j], cells[i][j]);
    }
  }
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      for (int k = j; k < N; ++k) {
        ASSERT_LE(cells[i][j], cells[i][k]);
      } 
    }
  }
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      for (int k = i + 1; k < N; ++k) {
        ASSERT_NE(cells[i][j], cells[k][j]);
        ASSERT_LT(cells[i][j], cells[k][j]);
        ASSERT_GT(cells[k][j], cells[i][j]);
      } 
    }
  }
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      for (int k = i + 1; k < N; ++k) {
        for (int t = j + 1; t < N; ++t) {
          ASSERT_NE(cells[i][j], cells[k][t]);
          ASSERT_LT(cells[i][j], cells[k][t]);
          ASSERT_GT(cells[k][t], cells[i][j]);
        } 
      } 
    }
  }
}

TEST(Cell, TestArithmeticOperations) {
  ASSERT_EQ(Cell(1, 0) + Cell(0, 1), Cell(1, 1));
  ASSERT_EQ(Cell(2, 2) + Cell(-1, 1), Cell(1, 3));
  ASSERT_EQ(Cell(1, 1) + Direction::RIGHT + Direction::LEFT, Cell(1, 1));
  ASSERT_EQ(Cell(1, 2) + Direction::RIGHT + Direction::DOWN, Cell(2, 3));
}
