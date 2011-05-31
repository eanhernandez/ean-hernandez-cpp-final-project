/*
This program is designed to accept a set of ! delimited HTTP GET style queries 
and then execute them in parallel.  it can be used in concert with more instances
of itself to spread work across machines.  Once queries are executed, the 
results are combined, and returned to the originating server.
*/
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
	// standard usage guidelines
	if (argc < 4)
	{
		// example command lines for one controller and two workers...
		// ./main 80 50 0 localhost 81 localhost 82 
		// ./main 81 20 1 
		// ./main 82 20 1
		std::cout << " usage: port max_threads [0|1]{controller=0, worker=1} target1 port1 target2 port2... target10 port10" << std::endl;
		return 1;
	}	
	// 
	if (atoi(argv[3])==0)
	{
		std::cout << "This is a control server" << std::endl;
	}
	else
	{
		std::cout << "This is a worker server" << std::endl;
	}
	std::cout << "t = new thread, . = successful query" << std::endl;
	// configuration_data stores frequently used configuration data, for
	// example server type and listening ports
	configuration_data c(std::atoi(argv[3]), std::string(argv[1]));
	for (int i=4;i<argc;i++)
	{
		c.addTarget(std::string(argv[i]), std::string(argv[i+1]));
		i++;
	}
	// here we begin the server
	server s(io_service, std::atoi(argv[1]), std::atoi(argv[2]), std::atoi(argv[3]),c);	
    io_service.run();					
	return 0;
}