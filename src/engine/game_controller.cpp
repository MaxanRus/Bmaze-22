#include "engine/game_controller.hpp"

using namespace utils;
using EventPtr = std::unique_ptr<engine::events::Event>;

namespace engine {
GameController::GameController(size_t count_players, size_t width,
                               size_t height)
    : map_(count_players, width, height), number_player_(0) {}

GameController::GameController(const GameMap& map /*, size_t number_player*/)
    : map_(map), number_player_(0) {}

GameController::GameController(GameMap&& map /*, size_t number_player*/)
    : map_(std::move(map)), number_player_(0) {}

std::list<EventPtr> GameController::Step(EventPtr&& event) {
  std::list<EventPtr> result = event->Apply(*this, number_player_);
  result.push_front(std::move(event));
  return result;
}

GameMap& GameController::GetMap() { return map_; }

const GameMap& GameController::GetMap() const { return map_; }

GameControllerState GameController::GetGameControllerState() const {
  return state_;
}

void GameController::SetGameControllerState(GameControllerState state) {
  state_ = state;
}
}  // namespace engine
