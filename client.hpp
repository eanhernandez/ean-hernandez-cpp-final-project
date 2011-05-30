#ifndef client_h__
#define client_h__

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "configuration_data.hpp"
#include "Response.hpp"
#include "ResponseAbstractFactory.hpp"

using boost::asio::ip::tcp;

class client
{
public:
	client(std::vector<std::vector<std::string> >v_args,int thread_counter);
	std::vector<std::string> getResponseBody(){return client::v_responses_;}
	void start(std::vector<std::string> v_inner);
	
private:
	void getConnected(std::string server, std::string port, boost::asio::io_service& io_service, tcp::socket* socket_);
	void DoWriteRead(std::string server, std::string port, std::string path, tcp::socket* socket_, Response* this_response);
	void handle_resolve(const boost::system::error_code& err);
	void handle_connect(const boost::system::error_code& err);
	void handle_write_request(const boost::system::error_code& err, tcp::socket* socket_ );
	void handle_read_status_line(const boost::system::error_code& err, tcp::socket* socket_ );
	void handle_read_headers(const boost::system::error_code& err, tcp::socket* socket_ );
	void handle_read_content(const boost::system::error_code& err, tcp::socket* socket_);
  
	std::string response_body_;
	std::string header_;
	boost::asio::streambuf* request_;
	boost::asio::streambuf response_;
	boost::asio::mutable_buffer result_;
	std::string getHeader(){return header_;}	
	std::string server_;
	std::string path_;
	std::vector<std::vector<std::string> >v_args_;
	std::vector<std::string> v_responses_;
	int thread_counter_;
};
#endif // client_h__