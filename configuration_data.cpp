#include "configuration_data.hpp"

configuration_data::configuration_data(int server_type, std::string listening_on_port)	
	: server_type_(server_type), listening_on_port_(listening_on_port){}
void configuration_data::addTarget(std::string server, std::string port)
{
	std::vector<std::string> v_temp;
	v_temp.push_back(server);
	v_temp.push_back(port);
	v_targets_.push_back(v_temp);
}