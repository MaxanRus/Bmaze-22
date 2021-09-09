#include <iostream>
#include <cstring>
#include <vector>
#include <ranges>
#include <boost/bind/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <string_view>

#include "network/session.hpp"
#include "network/events/event.hpp"
#include "utils/cell.hpp"

namespace network {
Session::Session(boost::asio::io_service& io_service) : socket_(io_service) {
  memset(header, 0, kMaxHeaderSize);
  memset(data, 0, kMaxDataSize);
}

boost::asio::ip::tcp::socket& Session::socket() { return socket_; }

std::shared_ptr<Lobby>& Session::lobby() { return lobby_; }

void Session::Start() {
  using namespace std::string_literals;
  lobby_->Join(shared_from_this());
  lobby_->WriteEveryone("I'm connect "s + std::to_string(
    reinterpret_cast<int64_t>(shared_from_this().get())));

  boost::asio::async_read(
    socket_, boost::asio::buffer(header, kMaxHeaderSize),
    boost::bind(&Session::WaitMessageHeader, shared_from_this(),
                boost::asio::placeholders::error));
}

void Session::WaitMessageHeader(const boost::system::error_code& error) {
  if (!error) {
    boost::asio::async_read(
      socket_, boost::asio::buffer(data, atoi(header)),
      boost::bind(&Session::WaitMessageData, shared_from_this(),
                  boost::asio::placeholders::error));
  } else {
    lobby_->Leave(shared_from_this());
  }
}

void Session::WaitMessageData(const boost::system::error_code& error) {
  if (!error) {
    using namespace std::string_literals;
    data[atoi(header)] = '\0';
    std::string_view msg = data;

    std::vector<std::string> tokens;
    for (auto i: msg | std::ranges::views::split(';')) {
      tokens.push_back("");
      tokens.back().reserve(7);
      std::ranges::for_each(i.begin(), i.end(),
                            std::bind(&std::string::push_back, std::ref(tokens.back()),
                                      std::placeholders::_1));
      if (tokens.back() == "") tokens.pop_back();
    }

    if (tokens[0] == "create") {
      if (tokens.size() >= 1) {
        events::CreateGameLobby(tokens[1]).Apply(shared_from_this());
      }   
    } else if (tokens[0] == "join") {
      if (tokens.size() >= 1) {
        events::JoinGameLobby(std::move(tokens[1])).Apply(shared_from_this());
        lobby_->WriteEveryone("I'm joined in lobby "s + std::to_string(
         reinterpret_cast<int64_t>(shared_from_this().get())));
      }
    } else if (tokens[0] == "start") {
      if (tokens.size() >= 3) {
        events::StartGame(lobby_->GetNumbersPlayers(),
                          boost::lexical_cast<size_t>(tokens[1]),
                          boost::lexical_cast<size_t>(tokens[2])).Apply(shared_from_this());
      } 
      std::cout << std::to_string(std::dynamic_pointer_cast<GameLobby>(lobby_)->
            GetGameController().GetMap()) << std::endl;
    } else if (tokens[0] == "gamestep") {
      using engine::events::MoveCurrentPlayer;
      using engine::events::PlayerBuildWall;
      using namespace utils;

      engine::events::ListEvents events;
      if (tokens.size() >= 1) {
        char type = tokens[1][0];
        if (type == 'm' || type == 'b') {
          char direction = tokens[1][1];
          utils::Direction dir;
          if (direction == 'l') dir = Direction::LEFT;
          else if (direction == 'u') dir = Direction::UP;
          else if (direction == 'r') dir = Direction::RIGHT;
          else dir = Direction::DOWN;
          if (type == 'm')
            events::GameStep(std::make_unique<MoveCurrentPlayer>(dir),
                             events).Apply(shared_from_this());
          else if (type == 'b')
            events::GameStep(std::make_unique<PlayerBuildWall>(dir),
                             events).Apply(shared_from_this());
        }
      }
      std::string result;
      for (auto& i: events) {
        result += i->GetMe() + ", ";
      }
      std::cout << result << std::endl;
      lobby_->WriteEveryone(result);
      std::cout << std::to_string(std::dynamic_pointer_cast<GameLobby>(lobby_)->
            GetGameController().GetMap()) << std::endl;

    } else if (tokens[0] == "list") {

    }

    boost::asio::async_read(
      socket_, boost::asio::buffer(header, kMaxHeaderSize),
      boost::bind(&Session::WaitMessageHeader, shared_from_this(),
                  boost::asio::placeholders::error));
  } else {
    lobby_->Leave(shared_from_this());
  }
}

void Session::WriteMessage(const std::string& msg) {
  queue_messages_.push(msg);

  if (queue_messages_.size() == 1) {
    sprintf(send_message, "%4d",
            queue_messages_.front().size());
    memcpy(send_message + kMaxHeaderSize, queue_messages_.front().c_str(),
           queue_messages_.front().size());

    boost::asio::async_write(
        socket_, boost::asio::buffer(send_message, queue_messages_.front().size() + kMaxHeaderSize),
        boost::bind(&Session::LoopSendMessge, shared_from_this(),
                    boost::asio::placeholders::error));
    queue_messages_.pop();
  }
}

void Session::LoopSendMessge(const boost::system::error_code& error) {
  using namespace std;
  if (!queue_messages_.empty()) {
    sprintf(send_message, "%4d",
            queue_messages_.front().size());
    memcpy(send_message + kMaxHeaderSize, queue_messages_.front().c_str(),
           queue_messages_.front().size());

    boost::asio::async_write(
      socket_, boost::asio::buffer(send_message, queue_messages_.front().size() + kMaxHeaderSize),
      boost::bind(&Session::LoopSendMessge, shared_from_this(),
                  boost::asio::placeholders::error));
    queue_messages_.pop();
  } 
}
}

