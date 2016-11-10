#ifndef IRC_PROTO_H
#define IRC_PROTO_H

#include <string>
#include "connection.hpp"
#include <vector>

class ircProto : protected connection{

	public:

		ircProto(char*, char*, char*);
		void run();

	private:

		void pong(std::string&);

		//parses the raw message from the server and sends the information to the correct function
		void handle_raw_msg(std::array<char, 512>, std::string::size_type);

		void handle_line(std::string);

		//when a command has been found this function figures out which one and runs the required code
		int handle_command(std::string&);

		void parseLine(std::string&);

		//put lines from msg delimitted by "\r\n" into the vector
		void split(const std::string&, char, std::vector<std::string>&);

		//join a channel
		void join(std::string && channel);

		//send authentication data to server to authenticate connection
		void authenticate();

		//leave a channel
		void part(std::string && channel);

		//disconnect the connection
		void quit();

		void changeNick(std::string & nick);

		std::string buffer;
		std::string nick;
		//character to look for at the begginning of a command
		char commandPrefix;

};

#endif