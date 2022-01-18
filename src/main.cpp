#include <iostream>
#include <thread>

#include "engine/game_controller.hpp"
#include "engine/map_generator.hpp"
#include "network/acceptor.hpp"

void StartServer() {
  using namespace boost::asio;
  using namespace network;
  std::cout << "Server start" << std::endl;
  auto waiting_lobby = std::make_shared<WaitingLobby>();

  io_service io_service;
  ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 7777);
  Acceptor acceptor(io_service, ep, waiting_lobby);

  ip::tcp::endpoint wep(ip::address::from_string("127.0.0.1"), 8888);
  WebAcceptor web_acceptor(io_service, wep, waiting_lobby);

  io_service.run();
}

int main() {
  using namespace utils;
  using namespace engine;
  using namespace events;
  using namespace network;

  try {
    std::thread server(StartServer);

    size_t seed = time(0);
    std::cout << "current seed: " << seed << std::endl;
    GameController gc(MapGenerator(1, 5, 5, seed)());

    auto print = [](const auto& lst) {
      for (auto& i : lst) {
        std::cout << i->GetMe() << ", ";
      }
      std::cout << std::endl;
    };

    bool end_game = false;

    while (!end_game) {
      char type;
      if (!(std::cin >> type)) {
        end_game = true;
        break;
      }

      engine::events::ListEvents list_events;
      if (type == 'm') {
        char direction;
        if (!(std::cin >> direction)) {
          end_game = true;
          break;
        }

        Direction dir;
        if (direction == 'l')
          dir = Direction::LEFT;
        else if (direction == 'u')
          dir = Direction::UP;
        else if (direction == 'r')
          dir = Direction::RIGHT;
        else
          dir = Direction::DOWN;
        list_events = gc.Step(std::make_unique<MoveCurrentPlayer>(dir));
        print(list_events);
      } else if (type == 'b') {
        char direction;
        std::cin >> direction;

        Direction dir;
        if (direction == 'l')
          dir = Direction::LEFT;
        else if (direction == 'u')
          dir = Direction::UP;
        else if (direction == 'r')
          dir = Direction::RIGHT;
        else
          dir = Direction::DOWN;
        list_events = gc.Step(std::make_unique<PlayerBuildWall>(dir));
        print(list_events);
      } else if (type == 'd') {
        end_game = false;
        break;
      }

      for (auto& i : list_events) {
        if (i->event_type ==
            engine::events::EventType::PlayerTriesRaiseExcessTreasure) {
          auto& event =
              dynamic_cast<engine::events::PlayerTriesRaiseExcessTreasure&>(
                  *i.get());
          std::cout << "You must to change treasure(print number) or 0 for to "
                       "opt out";
        }
      }

      std::cout << std::to_string(gc.GetMap()) << std::endl;
    }
    server.detach();
  } catch (...) {
  }
}
