#ifndef Response_h__
#define Response_h__
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time.hpp>
#include <tuple>

typedef	std::tuple<std::string,std::string> header_pair;

class Response
{
public:
	Response();
	void Set_Status_Line(std::string s);
	void Add_Header(std::string s, std::string t);
	void Add_Header(std::string s);
	std::string Get_Header(std::string s);
	void Set_Body(std::string s);
	bool CheckResponseIsValid();
	virtual std::string GetResponseMessage()=0;
	int Get_HTTP_Message_Length();
	void setHeaders();

protected:
	std::vector<header_pair> v_headers_;	// TODO: make these tuples
	std::string body_;
	std::string status_line_;
};

class ReceivedResponse : public Response
{
public:
	std::string GetResponseMessage();		// just gets the http body
};

class SendableResponse : public Response
{
public:
	std::string GetResponseMessage();		// gets a full http message, body, headers, status
};
#endif
