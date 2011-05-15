#include "resultsAggregator.hpp"
resultsAggregator::resultsAggregator(){}
void resultsAggregator::setResponse(std::string s)
{
	resultsAggregator::responses_.append(s);
}
std::string resultsAggregator::getResponse()
{
	return responses_;
}
