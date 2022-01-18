#pragma once

#include <memory>
#include <network/lobby.hpp>
#include <string>
#include <vector>

namespace network {
namespace events {
struct Event {
  virtual void Apply(std::shared_ptr<Session>) = 0;
  virtual ~Event() = default;
};

struct CreateGameLobby : public Event {
  CreateGameLobby(const std::string& name);
  CreateGameLobby(std::string&& name);
  void Apply(std::shared_ptr<Session>) override;
  std::string name;
};

struct JoinGameLobby : public Event {
  JoinGameLobby(const std::string& name);
  JoinGameLobby(std::string&& name);
  void Apply(std::shared_ptr<Session>) override;
  std::string name;
};

struct StartGame : public Event {
  StartGame(size_t count_players, size_t width, size_t height);
  void Apply(std::shared_ptr<Session>) override;
  size_t count_players;
  size_t width;
  size_t height;
};

struct GameStep : public Event {
  GameStep(std::unique_ptr<engine::events::Event>&&,
           engine::events::ListEvents&);
  void Apply(std::shared_ptr<Session>) override;

 private:
  std::unique_ptr<engine::events::Event> event_;
  engine::events::ListEvents& list_events_;
};
}  // namespace events
}  // namespace network
