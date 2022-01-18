#pragma once

namespace network {
class Session;
class TCPSession;
class WebSession;
}  // namespace network

#include <boost/asio.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <memory>
#include <queue>

#include "network/lobby.hpp"

class Lobby;

namespace network {
class Session : public std::enable_shared_from_this<Session> {
 public:
  Session();

  virtual std::shared_ptr<Lobby>& lobby();

  virtual void Start() = 0;
  virtual void WriteMessage(const std::string& string) = 0;

  virtual ~Session() = default;

 protected:
  std::shared_ptr<Lobby> lobby_ = nullptr;
};

class TCPSession : public Session {
 public:
  TCPSession(boost::asio::ip::tcp::socket&& socket);

  void Start() override;
  void WriteMessage(const std::string& string) override;

 private:
  void WaitMessageHeader(const boost::system::error_code& error);
  void WaitMessageData(const boost::system::error_code& error);
  void LoopSendMessage(const boost::system::error_code& error);

  static constexpr size_t kMaxHeaderSize = 4;
  static constexpr size_t kMaxDataSize = 1024;
  char header[kMaxHeaderSize];
  char data[kMaxDataSize + 1];
  char send_message[kMaxHeaderSize + kMaxDataSize + 1];

  boost::asio::ip::tcp::socket socket_;
  std::queue<std::string> queue_messages_;
};

class WebSession : public Session {
 public:
  WebSession(boost::asio::ip::tcp::socket&& socket);

  void Start() override;
  void WriteMessage(const std::string& string) override;

 private:
  void OnRun();
  void OnAccept(boost::beast::error_code error);
  void DoRead();
  void OnRead(boost::beast::error_code error, std::size_t bytes_transferred);
  void OnWrite(boost::beast::error_code error, std::size_t bytes_transferred);

  void WaitMessageHeader(const boost::system::error_code& error);
  void WaitMessageData(const boost::system::error_code& error);
  void LoopSendMessage(const boost::system::error_code& error);

  std::queue<std::string> queue_messages_;
  boost::beast::websocket::stream<boost::beast::tcp_stream> web_socket_;
  boost::beast::flat_buffer buffer_;
};
}  // namespace network
