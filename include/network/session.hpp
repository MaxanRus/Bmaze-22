#pragma once

namespace network {
class Session;
}

#include <boost/asio.hpp>
#include <memory>
#include <queue>

#include "network/lobby.hpp"

namespace network {
class Session : public std::enable_shared_from_this<Session> {
 public:
  Session(boost::asio::io_service& io_service);

  boost::asio::ip::tcp::socket& socket();
  std::shared_ptr<Lobby>& lobby();

  void Start();
  void WaitMessageHeader(const boost::system::error_code& error);
  void WaitMessageData(const boost::system::error_code& error);
  void WriteMessage(const std::string& string);
  void LoopSendMessge(const boost::system::error_code& error);

 private:
  static constexpr size_t kMaxHeaderSize = 4;
  static constexpr size_t kMaxDataSize = 1024;
  char header[kMaxHeaderSize];
  char data[kMaxDataSize + 1];
  char send_message[kMaxHeaderSize + kMaxDataSize + 1];

  boost::asio::ip::tcp::socket socket_;
  std::shared_ptr<Lobby> lobby_ = nullptr;
  std::queue<std::string> queue_messages_;
};
}

