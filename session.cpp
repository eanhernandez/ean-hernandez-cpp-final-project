#include "session.hpp"
boost::mutex m;

session::session(boost::asio::io_service& io_service, int maxclients, int server_type, configuration_data config) 
	: socket_(io_service), maxclients_(maxclients) , server_type_(server_type), 
	config_(config), max_read_data_(0), raf_(ReadAggregatorFunctor())
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
void session::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
	// accumulating data from the reads
	raf_(data_, bytes_transferred);  

	// finds the "<end>" tag that indicates we are at the end of a transmission
	if (raf_.checkForEndOfTransMission())
	{
		std::cout << "completed read" << std::endl;
		handle_completed_read(max_read_data_);
	}
	else
	{
		std::cout << "still reading" << std::endl;
		socket_.async_read_some(boost::asio::buffer(raf_.GetFinalData(), max_length),
			boost::bind(&session::handle_read, this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred)
			);
	}
}

void session::handle_completed_read(size_t bytes_transferred)
{
	std::cout << "bytes_transferred: " << bytes_transferred << std::endl;
	
	// chops up data from original request, stores as a list of queries
	argsParser a(raf_.GetFinalData(),server_type_,config_);
	// this will hold the results of all clients
	resultsAggregator ra;
	// threads to do our work in
	boost::thread_group threads;
	// how many queries have we been passed to run
	int total_args_to_run = a.getArgsCount();
	// this allows us to give each thread a number
	int thread_counter=0;
	// removes the possibility of empty threads
	if(maxclients_ > total_args_to_run){maxclients_ = total_args_to_run;}
	// create each thread with the aggregator, the queries it will run, and the counter
	while(a.getArgsCount()>0)
	{
		threads.create_thread(
			boost::bind(&session::spawnClients,
				this,
				boost::ref(ra),
				a.get_n(total_args_to_run/maxclients_),
				thread_counter++));
	}
	
	// wait until all threads are done
	threads.join_all();	
	// get all the different threads' responses from the aggregator (comes in a vector)
	std::vector<std::string> v_all_responses = ra.getResponse();
	// add a count of queries run to the response vector
	v_all_responses.push_back("count=" + boost::lexical_cast<std::string>(v_all_responses.size()));
	// pull the response vector into a string
	aggregate_responses_to_this_session = std::accumulate(v_all_responses.begin(),v_all_responses.end(),std::string(""));	
	// dump to screen for reporting
	aggregate_responses_to_this_session.insert(0,"\r\n");
	std::cout << " aggregate response : " << aggregate_responses_to_this_session << std::endl;
	// write back to the original calling client
    boost::asio::async_write( socket_, boost::asio::buffer(aggregate_responses_to_this_session, aggregate_responses_to_this_session.length()),
        boost::bind(&session::handle_write, this, boost::asio::placeholders::error)
		);
}
void session::spawnClients(resultsAggregator& ra, std::vector<std::vector<std::string> >v_args, int thread_counter)
{
	client c(v_args,thread_counter);	
	boost::lock_guard<boost::mutex> lock(m);
	ra.setResponse(c.getResponseBody());
}
void session::handle_write(const boost::system::error_code& error)
{
	if(error)
	{
		std::cout << " error on async_write!!! " << std::endl;
	}
	// each session runs only once, now we close the 

	boost::system::error_code ignored_ec;
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	socket_.close();
	std::cout << " session is done." << std::endl;
	delete this;	
}
tcp::socket& session::socket() 
{ 
	return session::socket_; 
}