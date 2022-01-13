#include <list>
#include <memory>
#include <string>
#include <vector>

#include "engine/events/event.hpp"
#include "utils/cell.hpp"
#include "engine/player.hpp"

using namespace utils;

namespace engine {
namespace events {
std::vector<Player>& Event::GetPlayers(GameMap& map) {
  return map.players_;
}

void ApplyAndPushBack(GameController& game_controller, size_t& number_player,
                      std::unique_ptr<Event> event, ListEvents& list) {
  list.push_back(std::move(event));
  list.splice(list.end(), list.back()->Apply(game_controller, number_player));
}

Event::Event(EventType event) : event_type(event) {}

MoveCurrentPlayer::MoveCurrentPlayer(Direction direction) : Event(EventType::MoveCurrentPlayer),
    direction_(direction) {}

ListEvents MoveCurrentPlayer::Apply(GameController& game_controller, size_t& number_player) {
  auto& map = game_controller.GetMap();
  std::list<std::unique_ptr<Event>> result; 
  Cell& position = GetPlayers(map)[number_player].position;
  if (map.GetWall(position, direction_)) {
    result.push_back(std::make_unique<FailedAttempt>());
  } else {
    position += direction_;
    result.push_back(std::make_unique<SuccessfulAttempt>());
  }
  ApplyAndPushBack(game_controller, number_player, std::make_unique<PlayerTriesRaiseItems>(), result);
  ++number_player;
  number_player %= GetPlayers(map).size();
  return result;
}

std::string MoveCurrentPlayer::GetMe() {
  return std::string("move player ") + std::to_string(direction_);
}

SuccessfulAttempt::SuccessfulAttempt() : Event(EventType::SuccessfulAttempt) {}

ListEvents SuccessfulAttempt::Apply(GameController& game_controller, size_t& number_player) {
  return {};
}

std::string SuccessfulAttempt::GetMe() {
  return std::string("successful attempt");
}

FailedAttempt::FailedAttempt() : Event(EventType::FailedAttempt) {}

ListEvents FailedAttempt::Apply(GameController& game_controller, size_t& number_player) {
  return {};
}

std::string FailedAttempt::GetMe() {
  return std::string("failed attempt");
}

PlayerBuildWall::PlayerBuildWall(Direction direction) : Event(EventType::PlayerBuildWall), direction_(direction) {}

ListEvents PlayerBuildWall::Apply(GameController& game_controller, size_t& number_player) {
  auto& map = game_controller.GetMap();
  ListEvents result; 
  Cell& position = GetPlayers(map)[number_player].position;
  map.GetWall(position, direction_) = true;
  ApplyAndPushBack(game_controller, number_player, std::make_unique<SomethingResult>(), result);
  ++number_player;
  number_player %= GetPlayers(map).size();
  return result;
}

std::string PlayerBuildWall::GetMe() {
  return std::string("build wall ") + std::to_string(direction_);
}

SomethingResult::SomethingResult() : Event(EventType::SomethingResult) {}

ListEvents SomethingResult::Apply(GameController& game_controller, size_t& number_player) {
  return {};
}

std::string SomethingResult::GetMe() {
  return std::string("something result");
}

PlayerTriesRaiseItems::PlayerTriesRaiseItems() : Event(EventType::PlayerTriesRaiseItems) {}

ListEvents PlayerTriesRaiseItems::Apply(GameController& game_controller, size_t& number_player) {
  auto& map = game_controller.GetMap();
  ListEvents result; 
  Player& player = GetPlayers(map)[number_player];
  Cell& position = player.position;
  if (auto treasures = map.RaiseTreasure(position); !treasures.empty()) {
    if (player.treasure) {      
      game_controller.SetGameControllerState(GameControllerState::WaitingDecisionsRaiseTreasure);
      // TODO
    } else {
      if (treasures.size() == 1) {
        ApplyAndPushBack(game_controller, number_player, std::make_unique<PlayerRaiseTreasure>(treasures[0]), result);
      } else {
        // TODO
      }
    }
  }
  return result;
}

std::string PlayerTriesRaiseItems::GetMe() {
  return std::string("player tries raise item");
}

PlayerRaiseTreasure::PlayerRaiseTreasure(size_t treasure) : Event(EventType::PlayerRaiseTreasure), treasure(treasure) {}

ListEvents PlayerRaiseTreasure::Apply(GameController& game_controller, size_t& number_player) {
  auto& map = game_controller.GetMap();
  Player& player = GetPlayers(map)[number_player];
  player.treasure = treasure;
  return {};
}

std::string PlayerRaiseTreasure::GetMe() {
  return "player raise treasure " + std::to_string(treasure);
}

PlayerTriesRaiseExcessTreasure::PlayerTriesRaiseExcessTreasure(size_t treasure) :
    Event(EventType::PlayerTriesRaiseExcessTreasure), treasure(treasure) {}

ListEvents PlayerTriesRaiseExcessTreasure::Apply(GameController& game_controller, size_t& number_player) {
  return {};
}

std::string PlayerTriesRaiseExcessTreasure::GetMe() {
  std::string result = "player raise treasure ";
  for (auto i: treasure) {
    result += std::to_string(i) + " ";
  }
  return result;
}
}
}

