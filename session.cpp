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
	if (raf_.checkForEndOfTransMission())  // TODO: Get rid of this, just read to EOF
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
	// chops up data from original request, stores as a list of queries
	argsParser a(raf_.GetFinalData(),server_type_,config_);
	// this will hold the results of all clients
	resultsAggregator ra;
	// threads to do our work in
	boost::thread_group threads;
	// how many queries have we been passed to run
	int total_args_to_run = a.getArgsCount();
	std::cout << "processing " << a.GetTotalArgs() << " queries in " << total_args_to_run << " sets." << std::endl;
	// this allows us to give each thread a number
	int thread_counter=0;
	// removes the possibility of empty threads
	if(maxclients_ > total_args_to_run){maxclients_ = total_args_to_run;}
	// timer 
	boost::timer query_timer;

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
	
	// this is what we'll send back to the querying system
	std::auto_ptr<ResponseAbstractFactory> raf (new ResponseGoingFactory());
	std::auto_ptr<Response> response_for_reply(raf->CreateResponse());

	// pull the response vector into a string and store in the response object
	response_for_reply->Set_Body(std::accumulate(v_all_responses.begin(),v_all_responses.end(),std::string("")));	
	
	// add some headers
	response_for_reply->Add_Header("Result-Count", boost::lexical_cast<std::string>(ra.getResponseCount()));
	double how_long_thess_queries_took = query_timer.elapsed();
	response_for_reply->Add_Header("Query-Time",boost::lexical_cast<std::string>(how_long_thess_queries_took));

	// calculate and set standard headers
	response_for_reply->setHeaders();

	full_http_response = response_for_reply->GetResponseMessage();	// TODO: can I use move here?
	//std::cout << " aggregate response : " << full_http_response << std::endl;
	std::cout << std::endl << "processed " << ra.getResponseCount() << " queries in " << how_long_thess_queries_took << " seconds." << std::endl;
	// write back to the original calling client
    boost::asio::async_write( socket_, boost::asio::buffer(full_http_response, full_http_response.length()),
        boost::bind(&session::handle_write, this, boost::asio::placeholders::error)
		);
}
void session::spawnClients(resultsAggregator& ra, std::vector<std::vector<std::string> >v_args, int thread_counter)
{
	client c(v_args,thread_counter);	
	while (c.finished==false)
	{
		Sleep(200);
	}
	boost::lock_guard<boost::mutex> lock(m);
	ra.setResponse(c.getResponseBody());
	ra.incrementResponseCount(c.getResponseCount());
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