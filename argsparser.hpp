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
      argsParser(char* s);
      void narrowToArgLine();
      void tokenizeArgLine();
      std::string dumpVector();
      std::vector<std::string> getArgsVector();
private:
      std::string s_;
      std::vector<std::string>v_;
};