#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include "server.hpp"

int main(int argc, char* argv[])
{
    boost::asio::io_service io_service;
	
    server s(io_service, 81, 50, 0);	// creates the server listening on port 81
    io_service.run();					// starts the server
	return 0;
}