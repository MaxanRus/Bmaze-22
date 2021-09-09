#include <memory>
#include <map>

#include "network/events/event.hpp"

namespace network {
namespace events {
std::unordered_map<std::string, std::weak_ptr<Lobby>> lobbies;

CreateGameLobby::CreateGameLobby(const std::string& name) : name(name) {}
CreateGameLobby::CreateGameLobby(std::string&& name) : name(std::move(name)) {}

void CreateGameLobby::Apply(std::shared_ptr<Session> session) {
  if (lobbies.find(name) != lobbies.end()) {
    // TODO
  }
  session->lobby()->Leave(session);
  auto lobby = std::make_shared<GameLobby>();
  lobby->Join(session);
  session->lobby() = lobby;
  lobbies[name] = lobby;
}

JoinGameLobby::JoinGameLobby(const std::string& name) : name(name) {}
JoinGameLobby::JoinGameLobby(std::string&& name) : name(std::move(name)) {}


void JoinGameLobby::Apply(std::shared_ptr<Session> session) {
  if (lobbies.find(name) == lobbies.end()) {
    // TODO
  }
  auto lobby = lobbies[name].lock();
  session->lobby()->Leave(session);
  lobby->Join(session);
  session->lobby() = lobby;
}

StartGame::StartGame(size_t count_players, size_t width, size_t height) :
    count_players(count_players), width(width), height(height) {}

void StartGame::Apply(std::shared_ptr<Session> session) {
  if (auto lobby = std::dynamic_pointer_cast<GameLobby>(session->lobby())) {
    lobby->Generate(std::make_unique<engine::MapGenerator>(count_players, width, height));
  } else {
    // TODO
  }
}

GameStep::GameStep(std::unique_ptr<engine::events::Event>&& event,
                   engine::events::ListEvents& list_events) :
    event_(std::move(event)), list_events_(list_events) {}

void GameStep::Apply(std::shared_ptr<Session> session) {
  if (auto lobby = std::dynamic_pointer_cast<GameLobby>(session->lobby())) {
    list_events_ = lobby->Step(std::move(event_));
  } else {
    // TODO
  }
}
}
}
