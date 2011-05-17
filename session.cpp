#include "session.hpp"

boost::mutex m;
session::session(boost::asio::io_service& io_service, int maxclients, int server_type) 
	: socket_(io_service), maxclients_(maxclients) , server_type_(server_type)
{	
	std::cout << " new session " << std::endl; 
}
void session::start()
{
	// reads data from this session's socket, calls handler

	socket_.async_read_some(boost::asio::buffer(data_, max_length),
		boost::bind(&session::handle_read, this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
		);
}
void spawnClients(resultsAggregator& ra,std::vector<std::vector<std::string> >v_args, int thread_counter)
{
	client c(v_args,thread_counter);	
	boost::lock_guard<boost::mutex> lock(m);
	ra.setResponse(c.getResponseBody());
}
void session::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
	// chops up data from original request, stores as a list of queries
	argsParser a(data_,server_type_);
	resultsAggregator ra;
	boost::thread_group threads;
	int total_args_to_run = a.getArgsCount();
	int thread_counter=0;
	if(maxclients_ > total_args_to_run){maxclients_ = total_args_to_run;}
	while(a.getArgsCount()>0)
	{
		threads.create_thread(
			std::bind(&spawnClients,
				boost::ref(ra),
				a.get_n(total_args_to_run/maxclients_),
				thread_counter++));
	}
	threads.join_all();	
	std::vector<std::string> v_all_responses = ra.getResponse();
	v_all_responses.push_back("count=" + boost::lexical_cast<std::string>(v_all_responses.size()));
	aggregate_responses_to_this_session = std::accumulate(v_all_responses.begin(),v_all_responses.end(),std::string(""));	
	std::cout << " aggregate response : " << aggregate_responses_to_this_session << std::endl;
	
    boost::asio::async_write( socket_, boost::asio::buffer(aggregate_responses_to_this_session, aggregate_responses_to_this_session.length()),
        boost::bind(&session::handle_write, this, boost::asio::placeholders::error)
		);
}
void session::handle_write(const boost::system::error_code& error)
{
	if(error)
	{
		std::cout << " error on async_write!!! " << std::endl;
	}
	// since this is only going to run once, we will kill the session at this point
	std::cout << " session is done." << std::endl;
	boost::system::error_code ignored_ec;
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	socket_.close();

	delete this;	
}
tcp::socket& session::socket() 
{ 
	return session::socket_; 
}