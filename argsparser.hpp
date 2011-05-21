#ifndef argsparser_h__
#define argsparser_h__

#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include "configuration_data.hpp"
using boost::asio::ip::tcp;


class argsParser
{
public:
      argsParser(char* s, int server_type, configuration_data config);
	  int getArgsCount();
	  void narrowToArgLine();
      void tokenizeArgLine();
	  void refactorArgsForWorkers();
	  void refactorArgsForWorkers(configuration_data config);
	  void addArg(std::vector<std::string>);
	  std::vector<std::vector<std::string> > get_n(int n);
      std::vector<std::vector<std::string> > getArgsVector();
	  
private:
      std::string s_;
      std::vector<std::vector<std::string> > v_;
	  int server_type_;
};
#endif // argsparser_h__
