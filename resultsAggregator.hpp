#include <iostream>
#include <boost\thread\mutex.hpp>
#include "client.hpp"
class resultsAggregator
{
public:
	resultsAggregator();
	void setResponse(std::vector<std::string> v_s);
	std::vector<std::string> getResponse();
private:
	std::string responses_;
	std::vector<std::string> v_responses_;
};