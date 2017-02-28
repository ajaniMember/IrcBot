#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>
#include <boost/asio.hpp>
#include <boost/array.hpp>



class connection
{
	
	public:

		connection( char *network, char *port);
		
		void connect();
		void disconnect();

		std::size_t send(std::string &msg);
		std::size_t send(std::string &&msg);
		std::size_t receive(std::array<char, 512>&, boost::system::error_code &error);



	private:

		boost::asio::io_service io;
		boost::asio::ip::tcp::resolver resolver;
		boost::asio::ip::tcp::resolver::query query;
		boost::asio::ip::tcp::resolver::iterator iterator;
		boost::asio::ip::tcp::socket socket;
		
};

#endif