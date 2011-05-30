#include "ReadAggregatorFunctor.hpp"
ReadAggregatorFunctor::ReadAggregatorFunctor() : max_read_data_(0){}
void ReadAggregatorFunctor::operator ()(char* data, size_t bytes_transferred)
{
	data[bytes_transferred] = '\0';
	int i=0;
	while (data[i] != '\0')
	{
		ReadAggregatorFunctor::final_data_[ReadAggregatorFunctor::max_read_data_+i] = data[i];
		i++;
	}
	ReadAggregatorFunctor::max_read_data_ = i;
}
char* ReadAggregatorFunctor::GetFinalData()
{
	return ReadAggregatorFunctor::final_data_;
}
bool ReadAggregatorFunctor::checkForEndOfTransMission()
{
	std::string s = final_data_;
	if (std::string::npos != s.find("%3Cend%3E")) {return true;}
	else {return false;}
}