#include "server.hpp"

server::server(boost::asio::io_service& io_service, short port, int maxclients, int server_type, configuration_data config) 
	: io_service_(io_service),
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		maxclients_(maxclients),
		server_type_(server_type)
{
	// listens on the socket for a new request, calls handler when one is received

	session* new_session = new session(io_service_, maxclients_,server_type_, config);
	acceptor_.async_accept(new_session->socket(),
		boost::bind(&server::handle_accept, this, new_session, boost::asio::placeholders::error, config)
		);
}
void server::handle_accept(session* new_session, const boost::system::error_code& error, configuration_data config)
{
	// creates a new session for recently received connection, calls itself to wait for another

	new_session->start();
	new_session = new session(io_service_, maxclients_,server_type_,config);
	acceptor_.async_accept(new_session->socket(),
	boost::bind(&server::handle_accept, this, new_session, boost::asio::placeholders::error, config));
}