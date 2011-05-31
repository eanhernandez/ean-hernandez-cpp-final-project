#include <string>

class ReadAggregatorFunctor
{
public:
	ReadAggregatorFunctor();
	bool checkForEndOfTransMission();
	void operator()(std::string data);
	const char* GetFinalData();
private:
	std::string final_data_;
};