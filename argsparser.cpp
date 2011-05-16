#include "argsparser.hpp"

int argsParser::getArgsCount()
{
	return v_.size();
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
                std::vector<std::string> inner;
                inner.push_back("184.73.236.29");
                inner.push_back(t);
                
                v_.push_back(inner);
    }
}

std::vector<std::vector<std::string> > argsParser::getArgsVector(){return v_;}// getter
