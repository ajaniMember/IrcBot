#include <iostream>
#include <array>
#include <sstream>
#include "ircProto.hpp"
#include "connection.hpp"


ircProto::ircProto(char * channel, char * port, char * ni) : connection(channel, port), commandPrefix('#'), waiting(0), nick(ni){

	std::cout << "initialized successfully!" << std::endl;

}

void ircProto::run(){

	try{

		connect();

		size_t len;
		int end;
			
		std::array<char, 512> buf;
	 	boost::system::error_code error;
	 	std::string msg;

		for(int i = 1; end != 1 ;i++)
		{
			
			//std::cout << i << " - loop" << std::endl;
			if(i == 3){

				authenticate();//send info to irc server to register connection
			}
			
			len = receive(buf, error);

			if (error == boost::asio::error::eof) // Connection closed cleanly by host.
		        break; 
	        else if (error)
	        	throw boost::system::system_error(error);

	        //std::cout.write(buf.data(), len);
	        
        	//print out received data :D
        	//std::cout.write(buf.data(), len);

        	handle_raw_msg(buf);

	   
	        
		}

	}
	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

}

/*void splitLines(std::vector<std::string> &lines, std::string msg)
{

	std::size_t pos;
	//previous position
	std::size_t previousPos;

	//find the first delimeter
	pos = msg.find("\r\n") + 1;

	//if were waiting on the second half of a msg
	if(waiting)
	{
		lines.push_back( halfMsg + " " + msg.substr(0, pos) );

		waiting = 0;

	}else
	{
		//push the first message into the vector
		lines.push_back( msg.substr(0, pos) );
	}

	while(pos <= msg.length())
	{
		previousPos = pos;

		//find the next whitespace by searching from the previous positon+1 so the same whitespace isnt found again
		pos = msg.find("\r\n", previousPos + 1 );

		//generate a sub-string from the positon of the last delimiter +1 untill the next delimiter
		
		lines.push_back( msg.substr( previousPos+1, pos - previousPos) );
		

		if(pos == msg.length()) break;
	}

	for( auto e = lines.begin(); e != lines.end(); e++ )
	{

		std::cout << *e << std::endl;

	}

}*/

void ircProto::parseLine(std::string &msg)
{
	std::vector<std::string> prefix;
	std::string msgPrefix;
	std::string msgBody;

	if(msg[0] == ':')
	{
		size_t endPrefix = msg.find(" :");

		msgPrefix = msg.substr(1, endPrefix);
		
		split(msgPrefix, ' ', prefix);

		std::cout << prefix[1] << std::endl;

		//GET MAIN MESSAGE BODY
		msgBody = msg.substr(msg.find(" :") + 2);

		//check if a PRIVMSG or a server msg
		if(prefix[1] == "PRIVMSG"){

			if(msgBody[0] == commandPrefix)
			{
				handle_command(msgBody);
			}

		} else if(prefix[1] == "376"){

			send("JOIN ##gr33nBot");

		}

	} else if( msg[0] == 'P')
	{
		pong(msg);
	}

}

void ircProto::handle_raw_msg(std::array<char, 512> &buf)
{
	//turns the array of char's into a string using the string constructor
	//to be able to use the string functions to parse the message easier and more efficiently.
	std::string msg(buf.begin(), buf.end());
	std::vector<std::string> lines;

	std::size_t msgLength = msg.length();
	std::size_t msgLastOf = msg.find_last_of("\n");
	//std::cout << "msg.length = " << msgLength << " : " << "msgLastOf = " << msgLastOf << std::endl;
	//there is no half message at the end continue as normal
	if( msgLength == msgLastOf + 1){

		//std::cout << "where fine! pass it on to the line splitter" << std::endl;
		split(msg, '\n', lines);

	//there is a half message at the end of msg, save that half and continue as normal
	} else
	{

		halfMsg = msg.substr(msgLastOf + 1, msg.length());
		waiting = 1;
		split(msg, 'n', lines);
		//std::cout << "first half of msg = " << halfMsg << std::endl;

	}

	for(auto iterator = lines.begin(); iterator != lines.end(); iterator++)
	{

		//parse the raw line sent from the server
		std::cout << *iterator;
		parseLine( *iterator );
	}

	
}

int ircProto::handle_command(std::string &msg)
{
	/*
		wanted commands:

			reverse polish notation calculator;
	*/

}

void ircProto::pong(std::string msg)
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

void ircProto::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    
    std::stringstream ss;
    ss.str(s);
    std::string item;

    while (std::getline(ss, item, delim))
    {

        elems.push_back(item);
    }
}