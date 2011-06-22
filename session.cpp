/* 
session is where most of the action occurs.  In this module, data is received from the 
original querying server, chopped up into individual queries, split up amongst workers
(if there are any) and sent of to workers in individual threads.  The results are waited
for, received, counted, and placed into a response message, along with headers, for the 
final reply to the querying server.
*/
#include "session.hpp"
boost::mutex m;

session::session(boost::asio::io_service& io_service, int maxclients, int server_type, configuration_data config) 
	: socket_(io_service), maxclients_(maxclients) , server_type_(server_type), 
	config_(config),  raf_(ReadAggregatorFunctor())
{	
	std::cout << " new session " << std::endl; 
}
void session::start()
{
	boost::asio::streambuf this_request;
	// reading until the end of the status line
	boost::asio::read_until(socket_, this_request, "\r\n");
	std::string status_line;
	std::istream response_stream(&this_request);
	std::getline(response_stream, status_line);

	// storing status line in the read aggregator, notice it is a functor 
	raf_(status_line);
	// reading until the end of the headers
	boost::asio::read_until(socket_, this_request, "\r\n\r\n");

	std::string header;
	// storing headers in the read aggregator, notice it is a functor
	while (std::getline(response_stream, header) && header != "\r")
	{
		raf_(header);
		header.clear();
	}

	// done reading... we don't care about the body
	std::cout << "completed read" << std::endl;

	handle_completed_read();
}
// after all queries have been read in
void session::handle_completed_read()
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
	
	// this is what we'll send back to the querying system, using an abstract 
	// factory and autopointers do demonstrate usage.
	//std::auto_ptr<ResponseAbstractFactory> raf (new ResponseGoingFactory());
	//std::auto_ptr<Response> response_for_reply(raf->CreateResponse());

	// template based abstract factory
	std::auto_ptr<ResponseAbstractFactory> raf (new ResponseGoingFactory());
	std::auto_ptr<Response> response_for_reply(raf->create<Response>());


	// pull the response vector into a string and store in the response object
	response_for_reply->Set_Body(std::accumulate(v_all_responses.begin(),v_all_responses.end(),std::string("")));	
	
	// add some headers
	response_for_reply->Add_Header("Result-Count", boost::lexical_cast<std::string>(ra.getResponseCount()));
	double how_long_thess_queries_took = query_timer.elapsed();
	response_for_reply->Add_Header("Query-Time",boost::lexical_cast<std::string>(how_long_thess_queries_took));

	// calculate and set standard headers
	response_for_reply->setHeaders();

	// get the entire response from the request
	full_http_response = response_for_reply->GetResponseMessage();	// TODO: can I use move here?
	//std::cout << " aggregate response : " << full_http_response << std::endl;
	std::cout << std::endl << "processed " << ra.getResponseCount() << " queries in " << how_long_thess_queries_took << " seconds." << std::endl;
	// write back to the original calling client
    boost::asio::async_write( socket_, boost::asio::buffer(full_http_response, full_http_response.length()),
        boost::bind(&session::handle_write, this, boost::asio::placeholders::error)
		);
}
// this is in the thread
void session::spawnClients(resultsAggregator& ra, std::vector<std::vector<std::string> >v_args, int thread_counter)
{
	// clients talk to the remote server
	client c(v_args,thread_counter);	
	// the client doesn't really return a value, and I noticed race conditions.  So, I added a 
	// member (c.finished) to client that indicates whether it is 100% done receiving and 
	// processing it's data.  We check this periodically to see if this thread is "done".
	while (c.finished==false)
	{
	//	Sleep(200);
		boost::this_thread::sleep(boost::posix_time::millisec(200));
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

	// each session runs only once, now we try to shut everything down gracefully
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
