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
	std::cout << "." ;
	tcp::socket* socket_ = new tcp::socket(client_io_service);

	ResponseAbstractFactory* raf = new ResponseComingFactory;
	//Response* this_response = raf->CreateResponse();
	std::auto_ptr<Response> this_response(raf->CreateResponse());

	try
	{
		getConnected(v_inner.at(0), v_inner.at(1), client_io_service, socket_);
		client_io_service.run();
	}
	catch (std::exception e)
	{
		std::cout << " error getting connected. closing socket and recording error" << std::endl;
		socket_->close();
		response_body_ = "error";
	}
	try
	{
		DoWriteRead(v_inner.at(0), v_inner.at(1), v_inner.at(2), socket_, this_response);
	}
	catch (std::exception e)
	{
		std::cout << " error getting data from remote server. closing socket and recording error" << std::endl;
		socket_->close();
		response_body_ = "error";
	}
}
void client::getConnected(std::string server, std::string port, boost::asio::io_service& io_service, tcp::socket* socket_ )
	
// wrapper to handle connecting to the remote machine
{
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
  }
void client::DoWriteRead(std::string server, std::string port, std::string path, tcp::socket* socket_, std::auto_ptr<Response> this_response)
{
	boost::asio::streambuf request_ ;
	std::ostream request_stream(&request_);

	request_stream << "GET /" << path << " HTTP/1.0\r\n" ;
	request_stream << "Host: " << server << "\r\n";
	request_stream << "Accept: */*\r\n" << "Connection: close\r\n\r\n";

	 boost::asio::write(*socket_, request_);
	 boost::asio::streambuf response;
	 boost::asio::read_until(*socket_, response, "\r\n");
	 std::string status_line;
	 std::istream response_stream(&response);
	 std::getline(response_stream, status_line);
	 //std::cout << "status_line: " << status_line << std::endl;
	 this_response->Set_Status_Line(status_line);
	 boost::asio::read_until(*socket_, response, "\r\n\r\n");
	 
	 std::string header;
	 while (std::getline(response_stream, header) && header != "\r")
	 {
		 //std::cout << "header: " << header << std::endl;
		 this_response->Add_Header(header);
	 }

	 std::string body;	 
	 boost::system::error_code error = boost::asio::error::host_not_found;
	 int foo = 1;
	 while (boost::asio::read(*socket_, response, boost::asio::transfer_at_least(1), error))
	 {}
	 std::string temp;
	 while (std::getline(response_stream, body))
	 {
		 //std::cout << "body: " << body << std::endl;
		 temp.append(body);
		 body.clear();
	 }
	 this_response->Set_Body(temp);
	 v_responses_.push_back(this_response->GetResponseMessage());
	 temp.clear();
	 socket_->close();
}
