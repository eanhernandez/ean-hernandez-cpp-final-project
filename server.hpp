#ifndef server_h__
#define server_h__

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
	  server(boost::asio::io_service& io_service, short port, int maxclients, int server_type, configuration_data config) ;
	  void handle_accept(session* new_session, const boost::system::error_code& error, configuration_data config);

	private:
	  boost::asio::io_service& io_service_;
	  tcp::acceptor acceptor_;
	  int maxclients_;
	  int server_type_;
};

#endif // server_h__
