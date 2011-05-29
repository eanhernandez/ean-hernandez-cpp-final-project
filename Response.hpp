#ifndef Response_h__
#define Response_h__
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <boost/lexical_cast.hpp>
class Response
{
public:
	Response();
	void Set_Status_Line(std::string s);
	void Add_Header(std::string s);
	void Set_Body(std::string s);
	std::string Get_HTTP_Message();
	int Get_HTTP_Message_Length();
	void setHeaders();
private:
	std::string status_line_;
	std::vector<std::string> v_headers_;	// make these tuples
	std::string body_;
};
#endif
