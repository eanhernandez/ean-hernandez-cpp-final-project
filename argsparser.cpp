#include "argsparser.hpp"

argsParser::argsParser(char* s, int server_type) : s_(s), server_type_(server_type)
	// this just helps to chop things up
{
	narrowToArgLine();
	tokenizeArgLine();
	if (server_type_ == 0){refactorArgsForWorkers();}	// if this is a control server
}
int argsParser::getArgsCount()
{
	return v_.size();
}
void argsParser::narrowToArgLine()
//gets rid of everything but the argument line
{
    s_ = s_.substr(0,s_.find("\n"));	// get first line from request, dumping headers, etc
    s_ = s_.substr(0,s_.find("HTTP"));	// get rid of "HTTP/1.1 at the end of the line 
    s_ = s_.substr(s_.find("/")+1);		// get rid of GET and leading "/" at the beginning of the line
}
void argsParser::tokenizeArgLine()
//splits up argument line and places each subquery in a vector element
{
    boost::char_separator<char> sep("!");
    boost::tokenizer<boost::char_separator<char>> tok(s_,sep);
	int posColon = 0;
	int posSlash = 0;
    BOOST_FOREACH(std::string t, tok)
    {
		std::vector<std::string> inner;
		posColon = t.find(":");
		inner.push_back(t.substr(0,posColon));			// server address goes here
		posSlash = t.find("/");
		inner.push_back(t.substr(posColon+1,(posSlash-posColon-1)));	// port number goes here
		inner.push_back(t.substr(posSlash+1));			// path goes here
                
		v_.push_back(inner);
    }
}
void argsParser::refactorArgsForWorkers()
{	
	/* todo */ 
}
std::vector<std::vector<std::string> > argsParser::get_n(int n)
{
	if (v_.size()<n)
	{
		n = v_.size();
	}
	std::vector<std::vector<std::string> > temp;
	temp.insert(temp.end(), v_.end()-n, v_.end());
	v_.erase(v_.end()-n,v_.end());
	return temp;
}
std::vector<std::vector<std::string> > argsParser::getArgsVector(){return v_;}// getter
