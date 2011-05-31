// uses functor capabilities to accumulate data passed in by a querying server
#include "ReadAggregatorFunctor.hpp"
ReadAggregatorFunctor::ReadAggregatorFunctor() : max_read_data_(0){}
void ReadAggregatorFunctor::operator ()(char* data, size_t bytes_transferred)
{
	// sets a null terminator at the size of the bytes transferred
	data[bytes_transferred] = '\0';
	int i=0;
	// move through data up to null terminator, write this to internal storage char*
	// starting from the last place we wrote to (based on max_read_data_)
	while (data[i] != '\0')
	{
		ReadAggregatorFunctor::final_data_[ReadAggregatorFunctor::max_read_data_+i] = data[i];
		i++;
	}
	// keep track of how much we've read
	ReadAggregatorFunctor::max_read_data_ = i;
}
// getter
char* ReadAggregatorFunctor::GetFinalData()
{
	return ReadAggregatorFunctor::final_data_;
}
// external procedures can use this to know if we've reached the end of this transmission
bool ReadAggregatorFunctor::checkForEndOfTransMission()
{
	std::string s = final_data_;
	if (std::string::npos != s.find("%3Cend%3E")) {return true;}
	else {return false;}
}