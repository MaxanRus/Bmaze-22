#pragma once
#include <list>
#include <memory>

#include "engine/events/event.hpp"
#include "engine/game_map.hpp"

namespace engine {
enum class GameControllerState { WaitingPlayer, WaitingDecisionsRaiseTreasure };

class GameController {
 public:
  GameController() = default;
  GameController(size_t count_players, size_t width, size_t height);
  GameController(const GameMap&);
  GameController(GameMap&&);
  std::list<std::unique_ptr<events::Event>> Step(
      std::unique_ptr<events::Event>&&);
  GameMap& GetMap();
  const GameMap& GetMap() const;
  GameControllerState GetGameControllerState() const;
  void SetGameControllerState(GameControllerState);

 private:
  GameMap map_;
  size_t number_player_ = 0;
  GameControllerState state_ = GameControllerState::WaitingPlayer;
};
}  // namespace engine
