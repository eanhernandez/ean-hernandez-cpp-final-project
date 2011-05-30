#include "resultsAggregator.hpp"
resultsAggregator::resultsAggregator() : response_count_(0)
{}
void resultsAggregator::setResponse(std::vector<std::string> v_s)
{
	v_responses_.insert(v_responses_.end(),v_s.begin(),v_s.end());
}
std::vector<std::string> resultsAggregator::getResponse()
{
	return v_responses_;
}
void resultsAggregator::incrementResponseCount(int i)
{
	response_count_+=i;
}
int resultsAggregator::getResponseCount()
{
	return response_count_;
}