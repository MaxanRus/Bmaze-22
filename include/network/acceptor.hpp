#pragma once

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <memory>

#include "network/lobby.hpp"
#include "network/session.hpp"

namespace network {
class Acceptor {
 public:
  Acceptor(boost::asio::io_service& io_service,
           const boost::asio::ip::tcp::endpoint& endpoint,
           std::shared_ptr<WaitingLobby>);

  void HandleAccept(const boost::system::error_code&,
                    boost::asio::ip::tcp::socket socket);

 private:
  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::shared_ptr<WaitingLobby> waiting_lobby_;
};

class WebAcceptor {
 public:
  WebAcceptor(boost::asio::io_service& io_service,
              const boost::asio::ip::tcp::endpoint& endpoint,
              std::shared_ptr<WaitingLobby>);

  void HandleAccept(const boost::system::error_code&,
                    boost::asio::ip::tcp::socket socket);

 private:
  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::shared_ptr<WaitingLobby> waiting_lobby_;
};

}  // namespace network
