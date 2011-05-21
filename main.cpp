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
	if (argc < 4)
	{
		std::cout << " usage: port max_threads [0|1]{controller=0, worker=1} target1 port1 target2 port2... target10 port10" << std::endl;
		return 1;
	}	
	configuration_data c(std::atoi(argv[3]), std::string(argv[1]));
	for (int i=4;i<argc;i++)
	{
		c.addTarget(std::string(argv[i]), std::string(argv[i+1]));
		i++;
	}
	server s(io_service, std::atoi(argv[1]), std::atoi(argv[2]), std::atoi(argv[3]),c);	
    io_service.run();					
	return 0;
}