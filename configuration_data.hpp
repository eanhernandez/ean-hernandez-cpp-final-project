#ifndef configuration_data_h__
#define configuration_data_h__

#include <iostream>
#include <vector>

class configuration_data
{
public:
	configuration_data(int server_type, std::string listening_on_port);
	void addTarget(std::string server, std::string port);
	int server_type_;
	std::string listening_on_port_;
	std::vector<std::vector<std::string > > v_targets_;
};

#endif // configuration_data_h__
