#include "network/acceptor.hpp"

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <memory>

#include "network/lobby.hpp"
#include "network/session.hpp"

using namespace boost::asio::ip;

namespace network {
Acceptor::Acceptor(boost::asio::io_service& io_service,
                   const tcp::endpoint& endpoint,
                   std::shared_ptr<WaitingLobby> waiting_lobby)
    : io_service_(io_service),
      acceptor_(io_service, endpoint),
      waiting_lobby_(waiting_lobby) {
  acceptor_.async_accept(
      io_service_, std::bind(&Acceptor::HandleAccept, this,
                             std::placeholders::_1, std::placeholders::_2));
}

void Acceptor::HandleAccept(const boost::system::error_code& error,
                            boost::asio::ip::tcp::socket socket) {
  if (!error) {
    auto session = std::make_shared<TCPSession>(std::move(socket));
    session->lobby() = waiting_lobby_;
    session->Start();

    acceptor_.async_accept(
        io_service_, std::bind(&Acceptor::HandleAccept, this,
                               std::placeholders::_1, std::placeholders::_2));
  } else {
    std::cerr << "Acceptor::HandleAccept error" << std::endl;
  }
}

WebAcceptor::WebAcceptor(boost::asio::io_service& io_service,
                         const tcp::endpoint& endpoint,
                         std::shared_ptr<WaitingLobby> waiting_lobby)
    : io_service_(io_service),
      acceptor_(io_service, endpoint),
      waiting_lobby_(waiting_lobby) {
  acceptor_.async_accept(
      io_service_, std::bind(&WebAcceptor::HandleAccept, this,
                             std::placeholders::_1, std::placeholders::_2));
}

void WebAcceptor::HandleAccept(const boost::system::error_code& error,
                               boost::asio::ip::tcp::socket socket) {
  if (!error) {
    auto session = std::make_shared<WebSession>(std::move(socket));
    session->lobby() = waiting_lobby_;
    session->Start();

    acceptor_.async_accept(
        io_service_, std::bind(&WebAcceptor::HandleAccept, this,
                               std::placeholders::_1, std::placeholders::_2));
  } else {
    std::cerr << "Acceptor::HandleAccept error" << std::endl;
  }
}
}  // namespace network
