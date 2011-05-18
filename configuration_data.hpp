#ifndef CONFIGURATION_DATA_HPP
#define CONFIGURATION_DATA_HPP

#include <iostream>
#include <vector>

class configuration_data
{
public:
	configuration_data(int server_type, std::string listening_on_port)	
		: server_type_(server_type), listening_on_port_(listening_on_port){}
	void addTarget(std::string server, std::string port)
	{
		std::vector<std::string> v_temp;
		v_temp.push_back(server);
		v_temp.push_back(port);
		v_targets_.push_back(v_temp);
	}
	int server_type_;
	std::string listening_on_port_;
	std::vector<std::vector<std::string > > v_targets_;
};

#endif