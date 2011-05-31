#include "argsparser.hpp"

argsParser::argsParser(const char* s, int server_type,configuration_data config) 
	: s_(s), server_type_(server_type)
{
	// chopping up received query
	narrowToArgLine();
	tokenizeArgLine();
	// now we know how many there are
	total_args = v_.size();

	// if this is a control server, we repackage the data as one big query for a particular
	// type of server... another one of this server!
	if (server_type_ == 0)	
	{		
		refactorArgsForWorkers(config.v_targets_.size(), config.v_targets_);					
	}	
}
// total number of queries, regardless of server type
int argsParser::GetTotalArgs()
{
	return total_args;
}
// number of queries to send, if this server is a worker, there will be one for each
// final query.  if it's a controller, there will be one bunch of queries stored together
// as a single query for the next worker in the chain.
int argsParser::getArgsCount()
{
	return v_.size();
}

//gets rid of everything but the argument line
void argsParser::narrowToArgLine()
{
    s_ = s_.substr(0,s_.find("HTTP/1.1"));	// get rid of "HTTP/1.1 at the end of the line 
    s_ = s_.substr(s_.find("/")+1);		// get rid of GET and leading "/" at the beginning of the line
	s_ = s_.substr(0,s_.length()-1);	// get rid of trailing space
}

//splits up argument line and places each subquery in a vector element
void argsParser::tokenizeArgLine()
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
// uses T to determine the type of the vector's size property.  this lets us do a for loop
// later without worrying about comparing against the wrong type.  eg unsigned int vs signed
template <class T>
void argsParser::refactorArgsForWorkers(T, std::vector<std::vector<std::string > > v_targets)
{	
	// this divides up the queries between workers
	int queries_per_worker = (this->getArgsCount()/v_targets.size());
	
	std::vector<std::vector<std::string> > v_new_queries;
	
	std::string s_new_queries;
	std::string new_server;
	std::string new_port;
	// for every worker server target
	for (T i=0; i<v_targets.size(); i++)	
	{
		new_server = v_targets.at(i).at(0);	// get new server 
		new_port = v_targets.at(i).at(1);	// get new port

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
		// trimming any trailing spaces
		size_t endpos = s_new_queries.find_last_of(" ");
		if( std::string::npos != endpos )
		{
			s_new_queries = s_new_queries.substr( 0, endpos );
		}

		// adding the "end of transmission delimiter.  
		s_new_queries.append("%3Cend%3E");
		
		std::vector<std::string> v_new_inner;
		v_new_inner.push_back(new_server);
		v_new_inner.push_back(new_port);
		v_new_inner.push_back(s_new_queries);
		v_new_queries.push_back(v_new_inner);
		s_new_queries.clear();
	}
	// get rid of anything that's in the results vector now, and then
	// fill it with the new args
	v_.clear();
	v_.insert(v_.end(),v_new_queries.begin(),v_new_queries.end());
}
// again using templates to keep from having to know what the type of the vector's 
// size() property is, which is of course not entirely neccesary, but does 
// demonstrate the usage of templates.  Otherwise, this method gets a requested
// number of queries from the argsparser, and then removes those queries 
// from internal storage, essentially a "pop" type procedure.
template <class T>
std::vector<std::vector<std::string> > argsParser::get_n(T n)
{
	if (boost::lexical_cast<T>(v_.size())<n)
	{
		n = boost::lexical_cast<T>(v_.size());
	}
	std::vector<std::vector<std::string> > temp;
	temp.insert(temp.end(), v_.end()-n, v_.end());
	v_.erase(v_.end()-n,v_.end());
	return temp;
}
// getter
std::vector<std::vector<std::string> > argsParser::getArgsVector()
{
	return v_;
}
// setter
void argsParser::addArg(std::vector<std::string> v)
{
	v_.push_back(v);
}