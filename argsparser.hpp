#ifndef argsparser_h__
#define argsparser_h__

#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/type_traits.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include "configuration_data.hpp"
using boost::asio::ip::tcp;


class argsParser
{
public:
      argsParser(char* s, int server_type, configuration_data config);
	  int getArgsCount();	// this is for after any worker refactoring
	  int GetTotalArgs();	// this is for total args passed into the parser
	  void narrowToArgLine();
      void tokenizeArgLine();
	  void refactorArgsForWorkers();
	  template <class T>
	  void refactorArgsForWorkers(T, std::vector<std::vector<std::string > > v_targets_);
	  void addArg(std::vector<std::string>);
	  template <class T>
	  std::vector<std::vector<std::string> > get_n(T n);
      std::vector<std::vector<std::string> > getArgsVector();
	  
	  
private:
      std::string s_;
      std::vector<std::vector<std::string> > v_;
	  int server_type_;
	  int total_args;
};
#endif // argsparser_h__
