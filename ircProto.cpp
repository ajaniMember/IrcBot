#include <iostream>
#include <array>
#include <sstream>

#include <boost/array.hpp>

#include "ircProto.hpp"
#include "connection.hpp"


ircProto::ircProto(char * channel, char * port, char * ni) : connection(channel, port), commandPrefix('#'), nick(ni){

	std::cout << "initialized successfully!" << std::endl;

}

void ircProto::run(){

	try{

		connect();

		std::size_t len;
		int end;
			
		std::array<char, 512> buf;
	 	boost::system::error_code error;
	 	std::string msg;
	 	std::vector<std::string> lines;

		for(int i = 1; end != 1 ;i++)
		{
			
			//std::cout << i << " - loop" << std::endl;
			if(i == 3){

				//send info to irc server to register connection
				authenticate();
			}
			
			len = receive(buf, error);

			// Connection closed cleanly by host.
			if (error == boost::asio::error::eof)
		        break; 
	        else if (error)
	        	throw boost::system::system_error(error); 
    		
        	//std::cout.write(buf.data(), len);

        	handle_raw_msg(buf, len);

		}

	}
	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

}

void ircProto::handle_raw_msg(std::array<char, 512> buf, std::string::size_type length)
{
	//turns the array of char's into a string using the string constructor
	//to be able to use the string functions to parse the message easier and more efficiently.
	
	buffer.append(buf.data(), length);
	std::size_t pos;
	std::string line;

	//find the next occurence of crlf
	pos = buffer.find("\r\n");

	while(1){

		if( pos == buffer.npos )
		{

			break;
		}

		line = buffer.substr(0, pos);
		
		handle_line(line);
		
		//add 2 to the pos variable to include the \r\n as they are counted as 2 chars
		//so we can remove the \r\n and find the next occurence in the next iteration of the loop
		//std::cout << buffer.substr(0, pos) << std::endl;
		buffer.erase(0, pos + 2);

		pos = buffer.find("\r\n");
	}

	//std::cout << buffer << std::endl;
}

void ircProto::handle_line(std::string line)
{

	std::cout << line << std::endl;
	//if message is a ping send pong
	if(line[0] == 'P')
	{
		pong(line);

	}//if message is 
	else if(line[0] == ':')
	{
		//vector containing all data split on whitespaces before the main message body
		std::vector<std::string> prefixVector;
		std::size_t pos;
		std::size_t endprefix = line.find(" :");
		std::string prefixString;

		
		//if there is a prefix and a message body
		if(endprefix != line.npos){
			//copy the prefix from the string (everything before the ' :') into the prefix variable
			//the reason its endprefix+1 is so the whitespace is included so when i am searching for
			//whitespaces to get the prefix parameters i don't miss the last one :D

			prefixString = line.substr(0, endprefix + 2);
			line.erase(0, endprefix + 2);

			//print the message body
			//std::string msgBody = line.substr(endprefix + 2);
			//std::cout << msgBody << std::endl;

		}
		else
		{
			prefixString = line;
		}
		//get the first prefix parameter but only copy from 1 onward to avoid getting
		//the ':' character on the front
		pos = prefixString.find(" ");
		prefixVector.push_back( prefixString.substr(1 , pos - 1) );
		prefixString.erase(0, pos + 1);

		pos = prefixString.find(" ");
		while(1)
		{
			if(pos == line.npos) break;

			//push substring onto vector
			prefixVector.push_back( prefixString.substr(0, pos) );
			//remove the substring plus the whitespace
			prefixString.erase(0, pos + 1);

			pos = prefixString.find(" ");
		}

		if(prefixVector[1] == "PRIVMSG")
		{
			//get username and channel

		}
		else if(prefixVector[1] == "376")
		{
			join("##gr33nbot");
		}
	}
}

int ircProto::handle_command(std::string &msg)
{
	/*
		implement a command whitelist for requred commands like quit and join
		wanted commands:

			reverse polish notation calculator;

			change nickname

			quit

			join

			part

	*/

}

void ircProto::pong(std::string &msg)
{
	//change PING to PONG
	msg[1] = 'O';
	send(msg);

}

void ircProto::authenticate()
{
	std::string msg;

	msg = "NICK ";
	msg.append(nick);
	send(msg);

	msg = "USER ";
	msg.append(nick);
	msg.append(" 0 * :Ronnie Reagan");
	send(msg);
}

void ircProto::join(std::string && channel)
{

	std::string msg = "JOIN ";
	msg.append(channel);
	msg.append("\r\n");

	send(msg);

}
void ircProto::part(std::string && channel)
{


}

void ircProto::quit()
{

	send("QUIT :my bits are tingling something must be wrong!!!");

}

void ircProto::changeNick(std::string & nick)
{


}