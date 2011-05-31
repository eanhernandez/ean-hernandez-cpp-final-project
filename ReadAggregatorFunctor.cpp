// uses functor capabilities to accumulate data passed in by a querying server
#include "ReadAggregatorFunctor.hpp"
ReadAggregatorFunctor::ReadAggregatorFunctor() {};
void ReadAggregatorFunctor::operator ()(std::string data)
{
	final_data_.append(data);
}
// getter
const char* ReadAggregatorFunctor::GetFinalData()
{
	return final_data_.c_str();
}
// external procedures can use this to know if we've reached the end of this transmission
bool ReadAggregatorFunctor::checkForEndOfTransMission()
{
	std::string s = final_data_;
	if (std::string::npos != s.find("\r\n\r\n")) {return true;}
	else {return false;}
}