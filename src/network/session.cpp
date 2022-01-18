#include "network/session.hpp"

#include <boost/bind/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <cstring>
#include <iostream>
#include <ranges>
#include <string_view>
#include <vector>

#include "network/events/event.hpp"
#include "network/lobby.hpp"
#include "utils/cell.hpp"

namespace network {
Session::Session() {}

std::shared_ptr<Lobby>& Session::lobby() { return lobby_; }

TCPSession::TCPSession(boost::asio::ip::tcp::socket&& socket)
    : socket_(std::move(socket)) {
  memset(header, 0, kMaxHeaderSize);
  memset(data, 0, kMaxDataSize);
}

void TCPSession::Start() {
  using namespace std::string_literals;
  lobby_->Join(shared_from_this());
  lobby_->WriteEveryone(
      "I'm connect "s +
      std::to_string(reinterpret_cast<int64_t>(shared_from_this().get())));

  boost::asio::async_read(
      socket_, boost::asio::buffer(header, kMaxHeaderSize),
      boost::bind(&TCPSession::WaitMessageHeader,
                  static_pointer_cast<TCPSession>(shared_from_this()),
                  boost::asio::placeholders::error));
}

void TCPSession::WaitMessageHeader(const boost::system::error_code& error) {
  if (!error) {
    boost::asio::async_read(
        socket_, boost::asio::buffer(data, atoi(header)),
        boost::bind(&TCPSession::WaitMessageData,
                    static_pointer_cast<TCPSession>(shared_from_this()),
                    boost::asio::placeholders::error));
  } else {
    lobby_->Leave(shared_from_this());
  }
}

void TCPSession::WaitMessageData(const boost::system::error_code& error) {
  if (!error) {
    using namespace std::string_literals;
    data[atoi(header)] = '\0';
    std::string_view msg = data;

    std::vector<std::string> tokens;
    for (auto i : msg | std::ranges::views::split(';')) {
      tokens.push_back("");
      tokens.back().reserve(7);
      std::ranges::for_each(
          i.begin(), i.end(),
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
        lobby_->WriteEveryone("I'm joined in lobby "s +
                              std::to_string(reinterpret_cast<int64_t>(
                                  shared_from_this().get())));
      }
    } else if (tokens[0] == "start") {
      if (tokens.size() >= 3) {
        events::StartGame(lobby_->GetNumbersPlayers(),
                          boost::lexical_cast<size_t>(tokens[1]),
                          boost::lexical_cast<size_t>(tokens[2]))
            .Apply(shared_from_this());
      }
      std::cout << std::to_string(std::dynamic_pointer_cast<GameLobby>(lobby_)
                                      ->GetGameController()
                                      .GetMap())
                << std::endl;
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
          if (direction == 'l')
            dir = Direction::LEFT;
          else if (direction == 'u')
            dir = Direction::UP;
          else if (direction == 'r')
            dir = Direction::RIGHT;
          else
            dir = Direction::DOWN;
          if (type == 'm')
            events::GameStep(std::make_unique<MoveCurrentPlayer>(dir), events)
                .Apply(shared_from_this());
          else if (type == 'b')
            events::GameStep(std::make_unique<PlayerBuildWall>(dir), events)
                .Apply(shared_from_this());
        }
      }
      std::string result;
      for (auto& i : events) {
        result += i->GetMe() + ", ";
      }
      std::cout << result << std::endl;
      lobby_->WriteEveryone(result);
      std::cout << std::to_string(std::static_pointer_cast<GameLobby>(lobby_)
                                      ->GetGameController()
                                      .GetMap())
                << std::endl;

    } else if (tokens[0] == "list") {
    }

    boost::asio::async_read(
        socket_, boost::asio::buffer(header, kMaxHeaderSize),
        boost::bind(&TCPSession::WaitMessageHeader,
                    std::static_pointer_cast<TCPSession>(shared_from_this()),
                    boost::asio::placeholders::error));
  } else {
    lobby_->Leave(shared_from_this());
  }
}

void TCPSession::WriteMessage(const std::string& msg) {
  queue_messages_.push(msg);

  if (queue_messages_.size() == 1) {
    sprintf(send_message, "%4d", queue_messages_.front().size());
    memcpy(send_message + kMaxHeaderSize, queue_messages_.front().c_str(),
           queue_messages_.front().size());

    boost::asio::async_write(
        socket_,
        boost::asio::buffer(send_message,
                            queue_messages_.front().size() + kMaxHeaderSize),
        boost::bind(&TCPSession::LoopSendMessage,
                    std::static_pointer_cast<TCPSession>(shared_from_this()),
                    boost::asio::placeholders::error));
    queue_messages_.pop();
  }
}

void TCPSession::LoopSendMessage(const boost::system::error_code& error) {
  using namespace std;
  if (!queue_messages_.empty()) {
    sprintf(send_message, "%4d", queue_messages_.front().size());
    memcpy(send_message + kMaxHeaderSize, queue_messages_.front().c_str(),
           queue_messages_.front().size());

    boost::asio::async_write(
        socket_,
        boost::asio::buffer(send_message,
                            queue_messages_.front().size() + kMaxHeaderSize),
        boost::bind(&TCPSession::LoopSendMessage,
                    std::static_pointer_cast<TCPSession>(shared_from_this()),
                    boost::asio::placeholders::error));
    queue_messages_.pop();
  }
}

WebSession::WebSession(boost::asio::ip::tcp::socket&& socket)
    : web_socket_(std::move(socket)) {}

void WebSession::Start() {
  boost::asio::dispatch(
      web_socket_.get_executor(),
      std::bind(&WebSession::OnRun,
                std::static_pointer_cast<WebSession>(shared_from_this())));
}

void WebSession::OnRun() {
  using namespace boost::beast;
  web_socket_.set_option(websocket::stream_base::timeout::suggested(
      boost::beast::role_type::server));

  web_socket_.set_option(
      websocket::stream_base::decorator([](websocket::response_type& res) {
        res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) +
                                         " websocket-server-async");
      }));
  web_socket_.async_accept(
      std::bind(&WebSession::OnAccept,
                std::static_pointer_cast<WebSession>(shared_from_this()),
                std::placeholders::_1));
}

void WebSession::OnAccept(boost::beast::error_code error) {
  if (error) {
    return;
  }

  lobby_->Join(shared_from_this());
  lobby_->WriteEveryone(
      "I'm connect " +
      std::to_string(reinterpret_cast<int64_t>(shared_from_this().get())));

  DoRead();
}

void WebSession::DoRead() {
  web_socket_.async_read(
      buffer_,
      std::bind(&WebSession::OnRead,
                std::static_pointer_cast<WebSession>(shared_from_this()),
                std::placeholders::_1, std::placeholders::_2));
}

void WebSession::OnRead(boost::beast::error_code error,
                        std::size_t bytes_transferred) {
  using namespace boost;
  using namespace boost::beast;
  boost::ignore_unused(bytes_transferred);

  if (error == websocket::error::closed) {
    return;
  }

  web_socket_.text(web_socket_.got_text());
  std::string s(boost::asio::buffer_cast<const char*>(buffer_.data()),
                bytes_transferred);
  // TODO processing messages
  DoRead();
}

void WebSession::OnWrite(boost::beast::error_code error,
                         std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (error) {
    std::cout << "Error write websocket" << std::endl;
    return;
  }

  buffer_.consume(buffer_.size());
}

void WebSession::WaitMessageHeader(const boost::system::error_code& error) {}

void WebSession::WaitMessageData(const boost::system::error_code& error) {}

void WebSession::WriteMessage(const std::string& msg) {
  web_socket_.async_write(
      boost::asio::buffer(msg),
      boost::beast::bind_front_handler(
          &WebSession::OnWrite,
          std::static_pointer_cast<WebSession>(shared_from_this())));
}

void WebSession::LoopSendMessage(const boost::system::error_code& error) {}
}  // namespace network
