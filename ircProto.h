#ifndef IRC_PROTO_H
#define IRC_PROTO_H

#include <string>
#include <tuple>
#include <vector>
#include <ctime>

#include "connection.h"


class ircProto : protected connection{

	public:

		ircProto(char*, char*, char*);
		void run();

	private:
		//vector hosts that are authenticated
		std::vector< std::tuple<std::time_t, std::string, int> > authHosts;
		//password acceptable to authenticate a user
		std::string authPass;

		//channels to join on connection to server
		std::vector<std::string> jChannel = {"##gr33nbot"};
		//buffer that all text is copied to from connection
		std::string buffer;
		std::string nick;
		//character to look for at the begginning of a command
		char commandPrefix;
		

		//parses the raw message from the server and sends the information to the correct function
		void handle_raw_msg(std::array<char, 512>, std::string::size_type);

		//parses line and figures out what to do from there
		// i.e. is there a command? am i just loggin this line? 
		void handle_line(std::string);

		//when a command has been found this function figures out which one and runs the required code
		void handle_command(std::string&, std::string&, std::string&, std::string&, bool);

		void split(std::string&, std::string&&, std::vector<std::string>&); 

		void sendMsg(std::string&, std::string&);
		//overloaded to be able to send messages without it being held in a variable
		void sendMsg(std::string&&, std::string&);

		void pong(std::string&);

		//send authentication data to server to register the connection
		void registerCon();

		//checks if users host is in authHosts vector
		bool isAuthenticated(std::string&);

		/*###########################################################################################
											command functions
		###########################################################################################*/

		//join a channel
		void join(std::string &channel);

		//leave a channel
		void part(std::string &channel);

		//disconnect the connection
		void quit();

		void setNick(std::string &nick);
		
};

#endif