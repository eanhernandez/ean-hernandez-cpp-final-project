#include "argsparser.hpp"

argsParser::argsParser(char* s, int server_type,configuration_data config) 
	: s_(s), server_type_(server_type)
	// this just helps to chop things up
{
	narrowToArgLine();
	tokenizeArgLine();
	std::cout << " received input in parser: " << s_ << std::endl;
	if (server_type_ == 0)	// if this is a control server
	{		
		refactorArgsForWorkers(config);					
	}	
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
void argsParser::refactorArgsForWorkers(configuration_data config)
{	
	int queries_per_worker = (this->getArgsCount()/config.v_targets_.size());
	
	std::vector<std::vector<std::string> > v_new_queries;
	
	std::string s_new_queries;
	std::string new_server;
	std::string new_port;
	// for every worker server target
	for (int i=0;i<config.v_targets_.size();i++)	
	{
		new_server = config.v_targets_.at(i).at(0);	// get new server 
		new_port = config.v_targets_.at(i).at(1);	// get new port

		// get one worker's share of the queries
		std::vector<std::vector<std::string> >temp = this->get_n(queries_per_worker);
		// loop through all of those queries
		BOOST_FOREACH(std::vector<std::string> & query_line, temp)
		{
			s_new_queries.append("!");
			s_new_queries.append(query_line.at(0));
			s_new_queries.append(":");
			s_new_queries.append(query_line.at(1));
			s_new_queries.append("/");
			s_new_queries.append(query_line.at(2));
		}
		size_t endpos = s_new_queries.find_last_of(" ");
		if( std::string::npos != endpos )
		s_new_queries = s_new_queries.substr( 0, endpos );
		std::cout << "within controller :" << new_server << " - " << new_port << " - " << s_new_queries << std::endl;
		std::vector<std::string> v_new_inner;
		v_new_inner.push_back(new_server);
		v_new_inner.push_back(new_port);
		v_new_inner.push_back(s_new_queries);
		v_new_queries.push_back(v_new_inner);
		s_new_queries.clear();
	}
	v_.clear();
	v_.insert(v_.end(),v_new_queries.begin(),v_new_queries.end());
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
void argsParser::addArg(std::vector<std::string> v)
{
	v_.push_back(v);
}