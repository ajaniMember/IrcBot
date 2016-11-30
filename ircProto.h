#ifndef IRC_PROTO_H
#define IRC_PROTO_H

#include <string>
#include <tuple>
#include <vector>
#include <ctime>
#include <fstream>

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
		std::vector<std::string> jChannel = {"##gr33nbot", "#archlinux", "#archlinux-offtopic", "##linux", "#ubuntu", "##c++", "#c++-general", "#c++-basic", "##math", "#ruby"};
		//buffer that all text is copied to from connection
		std::string buffer;
		std::string nick;
		//character to look for at the begginning of a command
		char commandPrefix;
		
		//log file stream
		std::ofstream fs;

		//parses the raw message from the server and sends the information to the correct function
		void handle_raw_msg(std::array<char, 512>, std::string::size_type);

		//parses line and figures out what to do from there
		// i.e. is there a command? am i just loggin this line? 
		void handle_line(std::string);

		//when a command has been found this function figures out which one and runs the required code
		void handle_command(std::string&, const std::string&, const std::string&, const std::string&, const bool);

		void split(std::string&, const std::string&&, std::vector<std::string>&); 

		void sendMsg(const std::string&, const std::string&);
		//overloaded to be able to send messages without it being held in a variable
		void sendMsg(const std::string&&, const std::string&);

		void pong(std::string&);

		//send authentication data to server to register the connection
		void registerCon();

		//checks if users host is in authHosts vector
		bool isAuthed(const std::string&);

		void notAuthed(const std::string&);

		std::string getNickname(const std::string&);

		std::string getNickHostname(const std::string&);

		/*###########################################################################################
											command functions
		###########################################################################################*/

		//join a channel
		void join(const std::string &channel);

		//leave a channel
		void part(const std::string &channel);

		//disconnect the connection
		void quit();

		void setNick(const std::string &nick);
		
};

#endif