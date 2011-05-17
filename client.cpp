#include "client.hpp"
boost::mutex m;
client::client( std::vector<std::vector<std::string> > v_spw) : v_spw_(v_spw)
{
	std::cout << " starting new client " << std::endl;	
	resultsAggregator ra;
	boost::thread_group threads;
	std::for_each(v_spw.begin(),v_spw.end(),[&threads,&ra,this](std::vector<std::string> s)
	{
		client::startup(ra,s);
	});
	threads.join_all();	
}
void client::startup(resultsAggregator& ra, const std::vector<std::string> s)
{
	boost::asio::io_service client_io_service;	
	tcp::socket* socket_ = new tcp::socket(client_io_service);
	getConnected(ra, s.at(0),s.at(1), client_io_service, socket_);
	client_io_service.run();
} 
void client::operator() (){}
void client::getConnected(resultsAggregator& ra, std::string server, std::string path, boost::asio::io_service& io_service, tcp::socket* socket_ )
// wrapper to handle connecting to the remote machine
{
	request_ = new boost::asio::streambuf;
	std::ostream request_stream(request_);
     
	request_stream << "GET /" << path << " HTTP/1.0\r\n" ;
	request_stream << "Host: " << server << "\r\n";
	request_stream << "Accept: */*\r\n" << "Connection: close\r\n\r\n";
	 
	tcp::resolver resolver(io_service); 
	
	tcp::resolver::query query(server, "http"); 
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
	boost::bind(&client::handle_write_request, this, boost::asio::placeholders::error, socket_, ra ));
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
	// stuffing the results in our result string
	
	while (std::getline(response_stream, body) && body != "\r")
	{
		response_body_.append(body);
		response_body_.append(1,'\n');
	}
	boost::lock_guard<boost::mutex> lock(m);
	ra.setResponse(response_body_);


}