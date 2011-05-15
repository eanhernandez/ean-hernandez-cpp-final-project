#include "argsparser.hpp"

argsParser::argsParser(char* s) : s_(s)
// this just helps to chop things up
{
    narrowToArgLine();
    tokenizeArgLine();
}
void argsParser::narrowToArgLine()
//gets rid of everything but the argument line
{
    int pos = s_.find("\n");
    s_ = s_.substr(0,pos);
    pos = s_.find("GET");
    s_ = s_.substr(pos+3);
    pos = s_.find("HTTP");
    s_ = s_.substr(0,pos);
    pos = s_.find("/");
    s_ = s_.substr(pos+1);
}
void argsParser::tokenizeArgLine()
//splits up argument line and places each subquery in a vector element
{
    boost::char_separator<char> sep("!");
    boost::tokenizer<boost::char_separator<char>> tok(s_,sep);
    BOOST_FOREACH(std::string t, tok)
    {
    v_.push_back(t);
    }
}
std::string argsParser::dumpVector()
// in case you need to get the vector for debugging
{
	std::string s;
    BOOST_FOREACH(std::string u,v_)
    {
		s.append(u);
		s.append(1,'\n');
    }
	return s;
}
std::vector<std::string> argsParser::getArgsVector(){return v_;}// getter