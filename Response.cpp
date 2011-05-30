#include "Response.hpp"

Response::Response() 
{
	status_line_ = "HTTP/1.1 200 OK";
}
void Response::Set_Status_Line(std::string s)
{
	status_line_ = 	s;
}
void Response::Add_Header(std::string s, std::string t)
{
	v_headers_.push_back(header_pair(s,t));
}
void Response::Add_Header(std::string s)
{
	int pos = s.find(":");
	std::string s1 = s.substr(0,pos);
	std::string s2 = s.substr(pos+1);
	Add_Header(s1,s2);
}
std::string Response::Get_Header(std::string s)
{
	bool found = false;
	std::string value;
	std::for_each(v_headers_.begin(), v_headers_.end(), [&found, &s, &value](header_pair h)
	{
		if (std::get<0>(h) == s) 
		{
			value = std::get<1>(h);
			found = true;
		}
	});
	if (found)
	{
		return value;
	}
	else
	{
		return "";
	}
	
}
void Response::Set_Body(std::string s)
{
	body_ = s;
}
int Response::Get_HTTP_Message_Length()
{
	return GetResponseMessage().length();
}
void Response::setHeaders()
{
	Add_Header("Date","today");
	Add_Header("Server", "multislammer");
	Add_Header("Content-Length", boost::lexical_cast<std::string>(body_.length()));
	Add_Header("Keep-Alive", "timeout=15, max=99");
	Add_Header("Connection","Keep-Alive");
	Add_Header("Content-Type", "text/html");
}
bool Response::CheckResponseIsValid()
{
	// looking for errors in the return vals
	std::string ok_status_line = "HTTP/1.1 200 OK\r";
	size_t endpos = body_.find("error");
	if( std::string::npos != endpos )
	{
		return false;
	}
	// looking if we did not receive a 200 OK
	
	else if (status_line_.compare("HTTP/1.1 200 OK\r")!=0)
	{
		return false;
	}

	// looking if we got a 404
	endpos = body_.find("404 Not Found");
	if (std::string::npos != endpos)
	{
		return false;
	}

	return true;
	
	

}
///////////////////////////////////////////////////////////////////////
// derived classes here

std::string ReceivedResponse::GetResponseMessage()
{
	return body_;
}
std::string SendableResponse::GetResponseMessage()
{
	std::string s;

	s = status_line_;
	s.append("\r\n");
	std::for_each(v_headers_.begin(),v_headers_.end(),[&s](header_pair header_line)
	{
		s.append(std::get<0>(header_line));
		s.append(": ");
		s.append(std::get<1>(header_line));

		s.append("\n");
	});
	s.append("\r\n\r\n");
	s.append(body_);	
	s.append("\r");

	return s;
}
