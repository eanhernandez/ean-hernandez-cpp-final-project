#include "client.hpp"
client::client(std::vector<std::vector<std::string> >v_args,int thread_counter) 
	: v_args_(v_args), thread_counter_(thread_counter)
{
	std::cout << " starting new client in thread " << thread_counter_ <<  std::endl;
	for (unsigned int i=0;i<v_args_.size();++i)
	{
		client::start(v_args_.at(i));
	}
}
void client::start(std::vector<std::string> v_inner)
{
	boost::asio::io_service client_io_service;
	
	std::cout << " processing new query in thread " << thread_counter_ << " : " << v_inner.at(0) << "/" << v_inner.at(1) <<  std::endl;
	tcp::socket* socket_ = new tcp::socket(client_io_service);
	getConnected(v_inner.at(0), v_inner.at(1), v_inner.at(2), client_io_service, socket_);
	client_io_service.run();
}
void client::operator() (){}
void client::getConnected(std::string server, std::string port, std::string path, boost::asio::io_service& io_service, tcp::socket* socket_ )
	
// wrapper to handle connecting to the remote machine
{
	request_ = new boost::asio::streambuf;
	std::ostream request_stream(request_);
     
	request_stream << "GET /" << path << " HTTP/1.0\r\n" ;
	request_stream << "Host: " << server << "\r\n";
	request_stream << "Accept: */*\r\n" << "Connection: close\r\n\r\n";
	 
	tcp::resolver resolver(io_service); 
	
	tcp::resolver::query query(server, port); 
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	tcp::resolver::iterator end; 

	boost::system::error_code error = boost::asio::error::host_not_found;

	while(error && endpoint_iterator != end) 
	{
		socket_->close();
		socket_->connect(*endpoint_iterator++, error);
	}
	if(error) 
	{	
		throw boost::system::system_error(error);
	}	
	// after setting up, write
	boost::asio::async_write(*socket_, *request_,
	boost::bind(&client::handle_write_request, this, boost::asio::placeholders::error, socket_ ));
  }
void client::handle_write_request(const boost::system::error_code& err, tcp::socket* socket_)
// handle whatever comes from the write
{
if (!err)
{
	//read the status line
	boost::asio::async_read_until(*socket_, response_, "\r\n",
		boost::bind(&client::handle_read_status_line, this, boost::asio::placeholders::error, socket_ ));
}
else
{
	std::cout << "err on handle_write_request" << std::endl;
}
}
void client::handle_read_status_line(const boost::system::error_code& err, tcp::socket* socket_ )
//handle the status line read
{
	if (!err)
	{
		//read the headers
		boost::asio::async_read_until(*socket_, response_, "\r\n\r\n",
			boost::bind(&client::handle_read_headers, this, boost::asio::placeholders::error, socket_ ));
	}
	else
	{
		std::cout << " blew up reading headers " << std::endl;
	}
}
void client::handle_read_headers(const boost::system::error_code& err, tcp::socket* socket_)
//handle the headers read
{
	std::istream response_stream(&response_);
	std::string header;
	// read to the end of the headers
	while (std::getline(response_stream, header) && header != "\r")
	{
		header_.append(header);
		header_.append(1,'\n');
	}
	
	// read the rest of the message, this is our data
	boost::asio::async_read(*socket_, response_, boost::asio::transfer_at_least(1),
		boost::bind(&client::handle_read_content, this, boost::asio::placeholders::error, socket_));
  }
void client::handle_read_content(const boost::system::error_code& err, tcp::socket* socket_)
{
	std::istream response_stream(&response_);
	std::string body;
	std::string temp_response_body;
	// stuffing the results in our result string
	
	while (std::getline(response_stream, body) && body != "\r")
	{
		temp_response_body.append(body);
	}
	if (std::string::npos == temp_response_body.find("404"))
	{
		std::cout << " response received to client in thread " << thread_counter_ << " : " << temp_response_body << std::endl;
		v_responses_.push_back(temp_response_body);
	}
	else
	{	
		temp_response_body.resize(20);
		std::cout << " response received to client in thread " << thread_counter_ << " contained 404 : " << temp_response_body << std::endl;
	}
	temp_response_body.clear();
	socket_->close();
}