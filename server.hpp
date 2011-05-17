#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include "session.hpp"
using boost::asio::ip::tcp;

class server
{
	public:
	  server(boost::asio::io_service& io_service, short port, int maxclients, int server_type) ;
	  void handle_accept(session* new_session, const boost::system::error_code& error);

	private:
	  boost::asio::io_service& io_service_;
	  tcp::acceptor acceptor_;
	  int maxclients_;
	  int server_type_;
};