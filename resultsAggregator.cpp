#include "resultsAggregator.hpp"
resultsAggregator::resultsAggregator(){}
void resultsAggregator::setResponse(std::vector<std::string> v_s)
{
	v_responses_.insert(v_responses_.end(),v_s.begin(),v_s.end());
}
std::vector<std::string> resultsAggregator::getResponse()
{
	return v_responses_;
}
