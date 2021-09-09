cmake_minimum_required(VERSION 3.10)

project("Game")

# set(CMAKE_CXX_FLAGS "-O3")

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost)

include_directories(
  "include"
  ${GTEST_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR}
  )

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
  src/utils/cell.cpp
  src/engine/player.cpp
  src/engine/game_map.cpp
  src/engine/game_controller.cpp
  src/engine/events/event.cpp
  src/engine/map_generator.cpp
  src/network/acceptor.cpp
  src/network/session.cpp
  src/network/lobby.cpp
  src/network/events/event.cpp
  )

set(TEST_FILES
  tests/test_main.cpp
  tests/utils/test_cell.cpp
  )

add_executable(game src/main.cpp ${SOURCE_FILES})

# add_executable(TestGame ${TEST_FILES} ${SOURCE_FILES})

# target_link_libraries(TestGame ${GTEST_LIBRARIES} Threads::Threads)
target_link_libraries(game Threads::Threads ${Boost_LIBRARIES})

# enable_testing()
# add_test(TestGame "./TestGame")

