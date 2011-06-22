/*
helper object to wrap up and manage responses.  there are two subtypes: (1) ReceivedResponse
and (2) SendableResponse.  Both are very similar, but the GetResponseMessage needs to return
different data in different cases.  In the first case, we are interested in just the "body"
portion of the response, because we are going to collate it with other bodies to form a
combined response.  In the second case, we need a properly formed HTTP response message with
status line, headers, and a body.
*/
#include "Response.hpp"

Response::Response() 
{
	// setting the default response message
	status_line_ = "HTTP/1.1 200 OK";
}
// setter
void Response::Set_Status_Line(std::string s)
{
	status_line_ = 	s;
}
// setter overload 1: two values
void Response::Add_Header(std::string s, std::string t)
{
	// storing these in a header_pair tuple
	v_headers_.push_back(header_pair(s,t));
}
// setter overload 2: one value, break it up first, then store per overload 1
void Response::Add_Header(std::string s)
{
	int pos = s.find(":");
	std::string s1 = s.substr(0,pos);
	std::string s2 = s.substr(pos+1);
	Add_Header(s1,s2);
}
// returns a specifically queried for header, or "" if none is found
std::string Response::Get_Header(std::string s)
{
	bool found = false;
	std::string value;
	// nice place for a lambda
/*	no love for lambdas in g++ 4.4
	std::for_each(v_headers_.begin(), v_headers_.end(), [&found, &s, &value](header_pair h)
	{
		if (std::get<0>(h) == s) 
		{
			// tuples tuples 
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
*/

	for (int i=0;i<v_headers_.size();i++)
	{
		if (std::get<0>(v_headers_[i]) == s) 
		{
			// tuples tuples 
			value = std::get<1>(v_headers_[i]);
			return value;
		}
		return "";
	}

	
}
// setter
void Response::Set_Body(std::string s)
{
	body_ = s;
}
// useful for setting message size header in SendableResponses
int Response::Get_HTTP_Message_Length()
{
	return GetResponseMessage().length();
}
// the standard headers
void Response::setHeaders()
{
	boost::posix_time::ptime t;
	t = boost::posix_time::second_clock::local_time();
	std::string the_time = to_simple_string(t);
	
	Add_Header("Date",the_time);
	Add_Header("Server", "multislammer");
	Add_Header("Content-Length", boost::lexical_cast<std::string>(body_.length()));
	Add_Header("Keep-Alive", "timeout=15, max=99");
	Add_Header("Connection","Keep-Alive");
	Add_Header("Content-Type", "text/html");
}
// some simple checks for errors in received responses
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

// in this version, we're not looking for anything more than the body
std::string ReceivedResponse::GetResponseMessage()
{
	return body_;
}

// in this version, we're making a complete HTPP response
std::string SendableResponse::GetResponseMessage()
{
	std::string s;

	s = status_line_;
	s.append("\r\n");
/* 	no lambdas allowed...
	std::for_each(v_headers_.begin(),v_headers_.end(),[&s](header_pair header_line)
	{
		s.append(std::get<0>(header_line));
		s.append(": ");
		s.append(std::get<1>(header_line));

		s.append("\n");
	});
*/
	for (int i=0;i<v_headers_.size();i++)
	{
		s.append(std::get<0>(v_headers_[i]));
		s.append(": ");
		s.append(std::get<1>(v_headers_[i]));

		s.append("\n");
	
	} 

	s.append("\r\n\r\n");
	s.append(body_);	
	s.append("\r");

	return s;
}
