/* this class deals with socket level connection stuff */
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "connection.hpp"

using boost::asio::ip::tcp;

connection::connection( char* network, char * port)
		: query(network, port), resolver(io), iterator(resolver.resolve(query)), socket(io)

{

}

int connection::connect()
{

	boost::asio::connect(socket, iterator);
	
}

int connection::disconnect()
{

}

std::size_t connection::send(std::string &msg)
{
	msg.append("\r\n");
	//std::cout << "sent - " << msg << std::endl;

	return boost::asio::write(socket, boost::asio::buffer(msg));

}

std::size_t connection::send(std::string &&msg)
{
	msg.append("\r\n");
	//std::cout << "sent - " << msg << std::endl;
	
	return boost::asio::write(socket, boost::asio::buffer(msg));

}
std::size_t connection::receive(std::array<char, 512> &buf, boost::system::error_code &error)
{

	return socket.read_some(boost::asio::buffer(buf, 512), error);

}