#include <iostream>
#include "ircProto.hpp"

int main(int argc, char **argv){

	if(argc != 4){

		std::cerr << "Usage: ircBot <network> <port> <nick>" << std::endl;
		//TODO: add max character for nick, allow a comma delimmited list of nicks in case one fails

		return -1;
	}

	ircProto ircBot(argv[1], argv[2], argv[3]);

	ircBot.run();

	return 0;

}