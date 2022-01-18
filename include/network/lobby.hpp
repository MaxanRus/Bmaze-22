#pragma once

namespace network {
class Lobby;
class WaitingLobby;
class GameLobby;
}  // namespace network

#include <list>
#include <memory>
#include <string>

#include "engine/game_controller.hpp"
#include "engine/map_generator.hpp"
#include "network/session.hpp"

namespace network {
class Lobby {
 public:
  void Join(std::shared_ptr<Session>);
  void Leave(std::shared_ptr<Session>);
  void WriteEveryone(const std::string& message);
  virtual ~Lobby() = default;
  size_t GetNumbersPlayers() const;

 protected:
  std::list<std::weak_ptr<Session>> session_list_;
};

class WaitingLobby : public Lobby {};

class GameLobby : public Lobby {
 public:
  auto Step(std::unique_ptr<engine::events::Event>&&)
      -> std::list<std::unique_ptr<engine::events::Event>>;
  void Generate(std::unique_ptr<engine::MapGenerator>);
  const engine::GameController& GetGameController() const;

 private:
  engine::GameController game_controller_;
};
}  // namespace network
