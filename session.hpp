#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lambda/lambda.hpp>
#include <Windows.h>
#include "argsparser.hpp"
#include "client.hpp"

using boost::asio::ip::tcp;

class session
{
	public:
		session(boost::asio::io_service& io_service);
		tcp::socket& socket();
		void start();
		void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
		void handle_write(const boost::system::error_code& error);
		
	private:
		tcp::socket socket_;
		enum { max_length = 1024 };
		char data_[max_length];
		std::string aggregate_responses_to_this_session; 
		int maxclients_;
};