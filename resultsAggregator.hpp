#ifndef resultsAggregator_h__
#define resultsAggregator_h__

#include <iostream>
#include <boost/thread/mutex.hpp>
#include "client.hpp"

class resultsAggregator
{
public:
	resultsAggregator();
	void setResponse(std::vector<std::string> v_s);
	std::vector<std::string> getResponse();
	void incrementResponseCount(int i);
	int getResponseCount();
private:
	std::string responses_;
	std::vector<std::string> v_responses_;
	int response_count_;
};
#endif // resultsAggregator_h__