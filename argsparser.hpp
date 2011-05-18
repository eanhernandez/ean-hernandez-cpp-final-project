#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

class configuration_data;
using boost::asio::ip::tcp;


class argsParser
{
public:
      argsParser(char* s, int server_type);
	  int getArgsCount();
	  void narrowToArgLine();
      void tokenizeArgLine();
	  void refactorArgsForWorkers();
	  void refactorArgsForWorkers(configuration_data config);
	  std::vector<std::vector<std::string> > get_n(int n);
      std::vector<std::vector<std::string> > getArgsVector();
	  
private:
      std::string s_;
      std::vector<std::vector<std::string> > v_;
	  int server_type_;
};
