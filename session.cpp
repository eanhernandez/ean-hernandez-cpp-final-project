#include "session.hpp"

boost::mutex m;
session::session(boost::asio::io_service& io_service) : socket_(io_service)  
{	
	std::cout << " new session " << std::endl; 
	maxclients_ = 3;
}
void session::start()
{
	// reads data from this session's socket, calls handler

socket_.async_read_some(boost::asio::buffer(data_, max_length),
    boost::bind(&session::handle_read, this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
	);
}
//void spawnClients(resultsAggregator& ra,std::string path)
//{
//	client c("184.73.236.29", path);

//	boost::lock_guard<boost::mutex> lock(m);
//	std::cout << " writing to ra... " << std::endl;
//	ra.setResponse(c.getResponseBody());
//}
void session::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
	// chops up data from original request, stores as a list of queries
	argsParser a(data_);
	
	client c(a.getArgsVector());
	std::vector<std::vector<std::string> > v_spw;
	
	//std::cout << " from ra post thread: " << ra.getResponse() << std::endl;

	//aggregate_responses_to_this_session = ra.getResponse();	
	
	// write data back to original querying system
	// todo: this should integrate the sets of data returned by the clients created above
    boost::asio::async_write( socket_, boost::asio::buffer(aggregate_responses_to_this_session, aggregate_responses_to_this_session.length()),
        boost::bind(&session::handle_write, this, boost::asio::placeholders::error)
		);
}
void session::handle_write(const boost::system::error_code& error)
{
	// since this is only going to run once, we will kill the session at this point
	
    socket_.close();
    delete this;
}
tcp::socket& session::socket() 
{ 
	return session::socket_; 
}