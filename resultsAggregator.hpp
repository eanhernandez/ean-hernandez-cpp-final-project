#include <iostream>
#include <boost\thread\mutex.hpp>
#include "client.hpp"
class resultsAggregator
{
public:
	resultsAggregator();
	void setResponse(std::string s);
	std::string getResponse();
private:
	std::string responses_;
};