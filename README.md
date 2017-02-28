# IrcBot
IrcBot written in c++.

# Objective
I am using this project as a usefull tool for me to:
    
    a) learn more about c++ 
    b) how to solve problems in the most efficient manner both in terms of time and complexity.

I plan to create other classes that can operate on their own but target a specific problem that I want to solve within the IrcBot.
I am doing this to offer a way to easily have alot of different problems that I can solve that will all converge and help create a more robust irc bot.
At the same time this repetition of solving different programing problems will make it easier for me to learn how to go about solving them in a more efficient way.


# To Do
Requirements that need to be complete before I would consider it complete

    - = not started
    # = started
    * = complete

    [-] - add error checking to all commands send to server to avoid bugs
    [-] - alphabeticaly sorting nickname vectors for channels
    [-] - reconnecting on failure and trying multiple nicknames and options
    [-] - refactoring and simplifying code
    [-] - adding appropriate comments
    [-] - find and post the title of a webpage posted in channels(only works on channels that you setup)
    [-] - More to come

# Requirements
This project depends on the boost asio library.

#Compilation
clang++ -std=c++11 ircbot.cpp ircProto.cpp connection.cpp -lboost_system -lboost_thread -lpthread
