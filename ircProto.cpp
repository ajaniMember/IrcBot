#include <iostream>
#include <array>
#include <tuple>
#include <sstream>
#include <ctime>

#include "ircProto.h"

ircProto::ircProto(char * channel, char * port, char * ni) : connection(channel, port), nick(ni), authPass("password1"), commandPrefix('#')
{
	std::cout << "initialized successfully!" << std::endl;
}

/*##################################################################################################################
											MAIN FUNCTIONS
##################################################################################################################*/

void ircProto::run(){

	try
	{

		connect();

		std::size_t len;
			
		std::array<char, 512> buf;
	 	boost::system::error_code error;
	 	std::string msg;
	 	std::vector<std::string> lines;

		for(int i = 1;;i++)
		{
			
			//std::cout << i << " - loop" << std::endl;
			if(i == 3){

				//send info to irc server to register connection
				registerCon();
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

	}//if message isn't a ping parse and route accordingly
	else if(line[0] == ':')
	{
		//vector containing all data split on whitespaces before the main message body
		std::vector<std::string> prefixVector;

		std::string prefixString;
		std::string msgBody;
		std::string fromNickname;
		std::string fromNicknameHost;
		std::string empty = "";

		std::size_t endprefix = line.find(" :");
		

		//if there is a prefix and a message body
		if(endprefix != line.npos){
			//copy the prefix from the string (everything before the ' :') into the prefix variable
			//the reason its endprefix+1 is so the whitespace is included so when i am searching for
			//whitespaces to get the prefix parameters i don't miss the last one :D

			prefixString = line.substr(0, endprefix + 2);
			line.erase(0, endprefix + 2);

			msgBody = line;

			//print the message body
			//std::cout << msgBody << std::endl;
		}
		else
		{
			prefixString = line;
			prefixString.append(" ");
		}
		//remove leading ':'
		prefixString.erase(0,1);
		split(prefixString, " ", prefixVector);

		if(prefixVector[1] == "PRIVMSG")
		{
		
			
			fromNickname = getNickname(prefixVector[0]);
			fromNicknameHost = getNickHostname(prefixVector[0]);
			
			//if prefixVector[2] is a channel
			//message to a channel
			if(prefixVector[2][0] == '#' || prefixVector[2][0] == '&')
			{
				handle_command(msgBody, fromNickname, fromNicknameHost, prefixVector[2], true);
			}
			//private message to the bot
			else if(prefixVector[2] == nick)
			{
				handle_command(msgBody, fromNickname, fromNicknameHost, empty, false);
			}

			
		}
		else if(prefixVector[1] == "JOIN")
		{

			//!!!add user to channel vector

		}
		else if(prefixVector[1] == "PART")
		{


		}
		else if(prefixVector[1] == "QUIT")
		{
			fromNicknameHost = getNickHostname(prefixVector[0]);
			//!!!if hostname is in authHosts vector remove it :D pretty easy

			for(auto e = authHosts.begin(); e != authHosts.end(); e++){

				if(std::get<1>(*e) == fromNicknameHost)
				{
					authHosts.erase(e);
					break;		
				}
			}
			
		}
		//end of MOTD
		else if(prefixVector[1] == "376")
		{
			for(auto i = jChannel.begin(); i != jChannel.end(); i++)
			{

				join(*i);
			}
		}
	}
}

void ircProto::handle_command(std::string &msg, const std::string &fromNickname, const std::string &fromNicknameHost, const std::string& channelName, const bool fromChannel)
{

	std::vector<std::string> parameters;
	std::string message;
	msg.append(" ");

	split(msg, " ", parameters);

	//add hostname to authHosts vector
	if(parameters[0] == "!auth")
	{
		//if command was sent from a private message from a user
		if(!fromChannel)
		{

			if(parameters.size() >= 2)
			{
				//if parameters[1] is equal to password then save the users host
				//in the authHosts vector 
				if(parameters[1] == authPass)
				{

					if( !isAuthed(fromNicknameHost) )
					{
						std::cout << "authenticated " << fromNickname << std::endl;
						//add an authenticated user with the timestamp the hostname and the level of authority
						authHosts.push_back( std::make_tuple( std::time(nullptr), fromNicknameHost, 1) );
					}

				}
				else
				{
					//send back error response
					sendMsg("error authenticating!", fromNickname);
				}
			}

		}
	}
	//show authorised nicknames's hostnames
	else if(parameters[0] == "!showauth")
	{
		if(isAuthed(fromNicknameHost))
		{

			for(auto e = authHosts.begin(); e != authHosts.end(); e++)
			{	
				message.append(std::get<1>(*e) + " ");
			}
			sendMsg(message, fromNickname);
		}
		else
		{
			notAuthed(fromNickname);
		}
	}
	else if(parameters[0] == "!unauth")
	{
		if(isAuthed(fromNicknameHost))
		{

			for(auto e = authHosts.begin(); e != authHosts.end(); e++)
			{
				if( std::get<1>(*e) == fromNicknameHost)
				{
					authHosts.erase(e);
					break;
				}
			}

		}
		else
		{
			sendMsg("you arne't auth'ed anyway you silly bitch.", fromNickname);
		}
	}
	//quit the connection
	else if(parameters[0] == "!quit")
	{
		if(isAuthed(fromNicknameHost))
		{
			quit();
		}
		else
		{
			notAuthed(fromNickname);
		}
	}
	//join a channel
	else if(parameters[0] == "!join")
	{

		if(isAuthed(fromNicknameHost))
		{
			if(parameters.size() >= 2)
			{

				join(parameters[1]);

			} 
			else{

				message = "No channel provided";
				sendMsg(message, fromNickname);
			}
		}
		else
		{
			notAuthed(fromNickname);
		}
	}
	//part from channel
	else if(parameters[0] == "!part")
	{

		if(isAuthed(fromNicknameHost))
		{
			if(fromChannel)
			{

				part(channelName);
			}
			//message is a private message from a nickname
			else
			{
				//if there is enough parameters part from channel
				if(parameters.size() >= 2)
				{
					//if there is a name of a channel provided part from it other wise send error to nick
					if(parameters[1][0] == '#' || parameters[1][0] == '&')
					{
						part(parameters[1]);
					}
					else{
						message = "No channel provided";
						sendMsg(message, fromNickname);
					}
				}
				else
				{
					message = "No channel provided";
					sendMsg(message, fromNickname);
				}
			}
		}
		else
		{
			notAuthed(fromNickname);
		}
	}
	else if(parameters[0] == "!nick")
	{

		if(isAuthed(fromNicknameHost))
		{
			if(parameters.size() >= 2)
			{
				//makesure username is less than 20 chars
				if(parameters[1].size() <= 20)
				{
					setNick(parameters[1]);
				}
				//send the first 20 chars as nick
				else
				{
					setNick(parameters[1].substr(0, 20));
				}
			}
			else
			{
				message = "No nickname Provided";
				sendMsg(message, fromNickname);
			}
		}
		else
		{
			notAuthed(fromNickname);
		}
	}
	


	//change nickname 

}

void ircProto::split(std::string &mainStr, const std::string &&delim, std::vector<std::string> &returnVec)
{
	
	int delimSize = delim.length();
	size_t pos;

	//make sure delim isnt larger than mainStr
	if(mainStr.length() > delimSize)
	{
		while(1)
		{
			pos = mainStr.find(delim);

			//no match found - then break
			if(pos == mainStr.npos) break;

			returnVec.push_back( mainStr.substr(0, pos) );

			//erase chars from 0 to pos + delimiter to get to the next 
			mainStr.erase(0, pos + delimSize);
		}
	}

}

void ircProto::registerCon()
{
	setNick(nick);

	std::string msg = "USER ";
	msg.append(nick);
	msg.append(" 8 * :Ronnie Reagan");

	send(msg);
}

void ircProto::pong(std::string &msg)
{
	//change PING to PONG
	msg[1] = 'O';
	send(msg);

}

//checks if users host is in authHosts vector
bool ircProto::isAuthed(const std::string &host)
{

	if(!host.empty())
	{
		for(auto e = authHosts.begin(); e != authHosts.end(); e++)
		{
			std::cout << "isAuthenticated - " << std::get<1>(*e) << std::endl;
			if(std::get<1>(*e) == host)
			{

				return true;
			}
		}
	}
	std::cout << "isnt authenticated !!!! - !!!!" << std::endl;
	return false;

}

void ircProto::notAuthed(const std::string &nickname)
{
	sendMsg("you are not authorised to perform this action! kindly fuck off ;)", nickname);
}

//get nickname from raw msg
std::string ircProto::getNickname(const std::string &msg)
{	
	//:ajaniMember!~ajaniMemb@45.55.137.32 PRIVMSG ##gr33nbot :hello
	//delimPos =  ^ the character being pointed to in the above text
	std::size_t delimPos = msg.find('!');
			
	return msg.substr(0, delimPos);
}

//get the hostname from raw msg
std::string ircProto::getNickHostname(const std::string &msg)
{
	//:ajaniMember!~ajaniMemb@45.55.137.32 PRIVMSG ##gr33nbot :hello
	//delimPos =  ^ the character being pointed to in the above text
	std::size_t delimPos = msg.find('!');

	return msg.substr(delimPos + 1, msg.find(" "));
}

//send msg to recipient NOTE: recipient can be a channel ;)
void ircProto::sendMsg(const std::string &message, const std::string &recipient)
{
	std::string msg = "PRIVMSG ";
	msg.append(recipient);
	msg.append(" :");
	msg.append(message);

	std::cout << "full msg = " << msg << std::endl;
	send(msg);
}

//send msg to recipient NOTE: recipient can be a channel ;)
void ircProto::sendMsg(const std::string &&message, const std::string &recipient)
{
	std::string msg = "PRIVMSG ";
	msg.append(recipient);
	msg.append(" :");
	msg.append(message);

	std::cout << "full msg = " << msg << std::endl;
	send(msg);
}

/*###################################################################################################################
											command functions
##################################################################################################################*/





void ircProto::join(const std::string &channel)
{

	std::string msg = "JOIN ";
	msg.append(channel);

	send(msg);

}

void ircProto::part(const std::string &channel)
{

	std::string msg = "PART ";
	msg.append(channel);

	send(msg);

}

void ircProto::quit()
{
	
	send("QUIT :my bits are tingling something must be wrong!!!");

}

void ircProto::setNick(const std::string & nick)
{
	std::string msg = "NICK ";
	msg.append(nick);
	send(msg);
}