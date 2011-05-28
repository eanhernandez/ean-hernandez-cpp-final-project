#include <string>

class ReadAggregatorFunctor
{
public:
	ReadAggregatorFunctor();
	bool checkForEndOfTransMission();
	void operator()(char* data, size_t bytes_transferred);
	char* GetFinalData();
private:
	enum { max_length = 102400 };	//max length query is 100kb
	char final_data_[max_length];
	int max_read_data_;
};