#pragma once
#include <list>
#include <memory>
#include <string>

#include "engine/game_controller.hpp"
#include "utils/cell.hpp"

namespace engine {
class GameController;
namespace events {

enum class EventType {
  MoveCurrentPlayer, SuccessfulAttempt, FailedAttempt, PlayerBuildWall, SomethingResult,
  PlayerTriesRaiseItems, PlayerRaiseTreasure, PlayerTriesRaiseExcessTreasure, None
};

class Event;

using ListEvents = std::list<std::unique_ptr<Event>>;

class Event {
 public:
  const EventType event_type = EventType::None;
  Event(EventType);
  virtual ListEvents Apply(GameController&, size_t& number_player) = 0;
  virtual std::string GetMe() = 0;
 protected:
  std::vector<Player>& GetPlayers(GameMap& map);
};

class MoveCurrentPlayer : public Event {
 public:
  MoveCurrentPlayer() = delete;
  MoveCurrentPlayer(utils::Direction);
  ListEvents Apply(GameController&, size_t& number_player) override;
  std::string GetMe() override;
 private:
  utils::Direction direction_;
};

class SuccessfulAttempt : public Event {
 public:
  SuccessfulAttempt();
  ListEvents Apply(GameController&, size_t& number_player) override;
  std::string GetMe() override;
};

class FailedAttempt : public Event {
 public:
  FailedAttempt();
  ListEvents Apply(GameController&, size_t& number_player) override;
  std::string GetMe() override;
};

class PlayerBuildWall : public Event {
 public:
  PlayerBuildWall(utils::Direction);
  ListEvents Apply(GameController&, size_t& number_player) override;
  std::string GetMe() override;
 private:
  utils::Direction direction_;
};

class SomethingResult : public Event {
 public:
  SomethingResult();
  ListEvents Apply(GameController&, size_t& number_player) override;
  std::string GetMe() override;
};

class PlayerTriesRaiseItems : public Event {
 public:
  PlayerTriesRaiseItems();
  ListEvents Apply(GameController&, size_t& number_player) override;
  std::string GetMe() override;
};

class PlayerRaiseTreasure : public Event {
 public:
  PlayerRaiseTreasure(size_t treasure);
  ListEvents Apply(GameController&, size_t& number_player) override;
  std::string GetMe() override;
  size_t treasure;
};

class PlayerTriesRaiseExcessTreasure : public Event {
 public:
  PlayerTriesRaiseExcessTreasure(size_t treasure);
  ListEvents Apply(GameController&, size_t& number_player) override;
  std::string GetMe() override;
  size_t treasure;
};
}
}

