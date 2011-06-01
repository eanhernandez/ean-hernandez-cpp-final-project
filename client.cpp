// handles all the work of talking to remote servers and getting their results
#include "client.hpp"


client::client(std::vector<std::vector<std::string> >v_args,int thread_counter) 
	: v_args_(v_args), thread_counter_(thread_counter), response_count_(0), finished(false)
{
	// let users know we've kicked off another thread
	std::cout << "t" ;
	// for each of the queries this client must execute, kick off the process
	for (unsigned int i=0;i<v_args_.size();++i)
	{
		client::start(v_args_.at(i));
	}
}
void client::start(std::vector<std::string> v_inner)
{
	// asio's helper service
	boost::asio::io_service client_io_service;
	// the socket we will use
	tcp::socket* socket_ = new tcp::socket(client_io_service);

	// a factory to create responses, again perhaps overkill, but demonstrates the use
	// of the abstract factory pattern.
	// 
	// initially, I used the standard abstract factory pattern, which can be found 
	// (unused) in the ResponseAbstractFactory.hpp header, but again to demonstrate
	// usage, I modified this to use the template based abstract factory from lecture 16
	//
	//ResponseAbstractFactory* raf = new ResponseComingFactory();
	//Response* this_response = raf->create<Response>();

	// template based abstract factory
	std::auto_ptr<ResponseAbstractFactory> raf (new ResponseComingFactory());
	std::auto_ptr<Response> this_response(raf->create<Response>());

	try
	{
		// handles the tasks of connecting to the remote server
		getConnected(v_inner.at(0), v_inner.at(1), client_io_service, socket_);
		client_io_service.run();
	}
	catch (std::exception e)
	{
		// if there is a problem, shut this client down gracefully
		//std::cout << " error getting connected. closing socket and recording error" << std::endl;
		std::cout << "e" << std::flush;
		socket_->close();
		response_body_ = "error";
		finished = true;
	}
	try
	{
		// handles the tasks of writing to the remote server, and reading it's response
		DoWriteRead(v_inner.at(0), v_inner.at(1), v_inner.at(2), socket_, this_response);
	}
	catch (std::exception e)
	{
		// again any problems are caught and we close down the client gracefully
		//std::cout << " error getting data from remote server. closing socket and recording error ( " << e.what() << " )" << std::endl;
		std::cout << "e";
		socket_->close();
		response_body_ = "error";
		finished = true;
	}
}
// wrapper to handle connecting to the remote machine
void client::getConnected(std::string server, std::string port, boost::asio::io_service& io_service, tcp::socket* socket_ )
{
	// standard asio stuff to connect to the port "port" at server "server"
	tcp::resolver resolver(io_service); 
	tcp::resolver::query query(server, port); 
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	tcp::resolver::iterator end; 

	boost::system::error_code error = boost::asio::error::host_not_found;
	// sometimes, an endpoint can have multiple IPs associated.  in this case we
	// try each one until we get a success.
	while(error && endpoint_iterator != end) 
	{
		socket_->close();
		socket_->connect(*endpoint_iterator++, error);
	}
	// if there were no successes and we are out of endpoints, throw an error
	if(error) 
	{			
		throw boost::system::system_error(error);
	}	
  }
// handles writing to and reading from the remote server
void client::DoWriteRead(std::string server, std::string port, std::string path, tcp::socket* socket_, std::auto_ptr<Response> this_response)
{
	// asio likes buffers
	boost::asio::streambuf request_ ;
	std::ostream request_stream(&request_);
	
	// this is what you have to say to HTTP servers to get them in the mood to talk to you
	request_stream << "GET /" << path << " HTTP/1.0\r\n" ;
	request_stream << "Host: " << server << "\r\n";
	request_stream << "Accept: */*\r\n" << "Connection: close\r\n\r\n";

	// writing
	boost::asio::write(*socket_, request_);
	boost::asio::streambuf response;

	// reading until the end of the status line
	boost::asio::read_until(*socket_, response, "\r\n");
	std::string status_line;
	std::istream response_stream(&response);
	std::getline(response_stream, status_line);
	std::cout << "." ;
	// storing status line in the response object
	this_response->Set_Status_Line(status_line);

	// reading until the end of the headers
	boost::asio::read_until(*socket_, response, "\r\n\r\n");
	 
	std::string header;
	// storing headers in the response object
	while (std::getline(response_stream, header) && header != "\r")
	{
		this_response->Add_Header(header);
	}

	std::string body;	 
	boost::system::error_code error = boost::asio::error::host_not_found;
	// reading to the end of the body
	while (boost::asio::read(*socket_, response, boost::asio::transfer_at_least(1), error))
	{}
	std::string temp;
	while (std::getline(response_stream, body))
	{
		temp.append(body);	// TODO: perhaps a good place for move?
		body.clear();
	}
	// storing the body
	this_response->Set_Body(temp);
	// if this response came from a worker server, get it's Result-Count header
	// and add it to the general count for this client
	std::string ResultCountHeaderValue = this_response->Get_Header("Result-Count");
	if (ResultCountHeaderValue!="")
	{
		response_count_+= std::atoi(ResultCountHeaderValue.c_str());
	}
	else
	{
		response_count_++;
	}
	// do some error checking on the response
	if (this_response->CheckResponseIsValid()==true)
	{
		v_responses_.push_back(this_response->GetResponseMessage());
	}
	else
	{
		std::cout << "invalid response, dumping it." << std::endl;
	}
	
	// tearing down, we are at the end now
	temp.clear();
	socket_->close();
	finished = true;
}
// getter
int client::getResponseCount()
{
	return response_count_;
}
// getter
std::vector<std::string> client::getResponseBody()
{
	return client::v_responses_;
}