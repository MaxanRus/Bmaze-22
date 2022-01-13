#include <iostream>

#include "engine/game_controller.hpp"
#include "engine/map_generator.hpp"

#include "network/acceptor.hpp"

int main() {
  using namespace utils;
  using namespace engine;
  using namespace events;

  size_t seed = time(0);
  std::cout << "current seed: " << seed << std::endl; 
  GameController gc(MapGenerator(1, 5, 5, seed)());

  auto print = [](const auto& lst) {
    for (auto& i: lst) {
      std::cout << i->GetMe() << ", ";
    }
    std::cout << std::endl;
  };

  while (true) {
    char type;
    std::cin >> type;

    engine::events::ListEvents list_events;
    if (type == 'm') {
      char direction;
      std::cin >> direction;

      Direction dir;
      if (direction == 'l') dir = Direction::LEFT;
      else if (direction == 'u') dir = Direction::UP;
      else if (direction == 'r') dir = Direction::RIGHT;
      else dir = Direction::DOWN;
      list_events = gc.Step(std::make_unique<MoveCurrentPlayer>(dir));
      print(list_events);
    } else if (type == 'b') {
      char direction;
      std::cin >> direction;

      Direction dir;
      if (direction == 'l') dir = Direction::LEFT;
      else if (direction == 'u') dir = Direction::UP;
      else if (direction == 'r') dir = Direction::RIGHT;
      else dir = Direction::DOWN;
      list_events = gc.Step(std::make_unique<PlayerBuildWall>(dir));
      print(list_events);
    }

    for (auto& i: list_events) {
      if (i->event_type == engine::events::EventType::PlayerTriesRaiseExcessTreasure) {
        auto& event = dynamic_cast<engine::events::PlayerTriesRaiseExcessTreasure&>(*i.get());
        std::cout << "You must to change treasure(print number) or 0 for to opt out";
      }
    }

    std::cout << std::to_string(gc.GetMap()) << std::endl;
  }
}

