#include "network/acceptor.hpp"

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <memory>
#include <iostream>

#include "network/session.hpp"
#include "network/lobby.hpp"

using namespace boost::asio::ip;

namespace network {
Acceptor::Acceptor(boost::asio::io_service& io_service, const tcp::endpoint& endpoint,
                   std::shared_ptr<WaitingLobby> waiting_lobby) : io_service_(io_service),
    acceptor_(io_service, endpoint), waiting_lobby_(waiting_lobby) {
  auto new_session = std::make_shared<Session>(io_service_);
  acceptor_.async_accept(new_session->socket(),
    boost::bind(&Acceptor::HandleAccept, this, new_session,
                boost::asio::placeholders::error));
}

void Acceptor::HandleAccept(std::shared_ptr<Session> session,
                           const boost::system::error_code& error) {
  if (!error) {
    session->lobby() = waiting_lobby_;
    session->Start();

    auto new_session = std::make_shared<Session>(io_service_);
    acceptor_.async_accept(new_session->socket(),
      boost::bind(&Acceptor::HandleAccept, this, new_session,
                  boost::asio::placeholders::error));
  } else {
    std::cerr << "Acceptor::HandleAccept error" << std::endl;
  }
}
}
