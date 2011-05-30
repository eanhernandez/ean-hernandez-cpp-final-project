#ifndef session_h__
#define session_h__

#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <numeric>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>

#include "argsparser.hpp"
#include "resultsAggregator.hpp"
#include "ReadAggregatorFunctor.hpp"
#include "Response.hpp"
#include "ResponseAbstractFactory.hpp"

using boost::asio::ip::tcp;

class session
{
public:
	session(boost::asio::io_service& io_service, int maxclients, int server_type, configuration_data config);
	tcp::socket& socket();
	void start();
	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_completed_read(size_t bytes_transferred);
	void spawnClients(resultsAggregator& ra, std::vector<std::vector<std::string> >v_args, int thread_counter);
	void handle_write(const boost::system::error_code& error);
	
private:
	ReadAggregatorFunctor raf_;
	tcp::socket socket_;
	enum { max_length = 102400 };	//max length query is 100kb
	char data_[max_length];
	//char final_data_[max_length];
	int max_read_data_;
	std::string full_http_response; 
	int maxclients_;
	int server_type_;
	configuration_data config_;
	
};
#endif // session_h__
