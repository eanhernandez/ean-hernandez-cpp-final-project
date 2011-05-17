#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

using boost::asio::ip::tcp;

class argsParser
{
public:
      argsParser(char* s, int server_type);
      void narrowToArgLine();
      void tokenizeArgLine();
	  std::vector<std::vector<std::string> > get_n(int n);
      std::vector<std::vector<std::string> > getArgsVector();
	  int getArgsCount();
private:
      std::string s_;
      std::vector<std::vector<std::string> > v_;
	  int server_type_;
};
