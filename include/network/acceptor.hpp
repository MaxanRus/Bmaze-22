#pragma once

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <memory>

#include "network/session.hpp"
#include "network/lobby.hpp"

namespace network {
class Acceptor {
 public:
  Acceptor(boost::asio::io_service& io_service,
           const boost::asio::ip::tcp::endpoint& endpoint, std::shared_ptr<WaitingLobby>);

  void HandleAccept(std::shared_ptr<Session>, const boost::system::error_code&);
 private:
  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::shared_ptr<WaitingLobby> waiting_lobby_;
};
}

