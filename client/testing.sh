#!/bin/bash

SERVER_PORT="58085" # Set your server's port number here

# Testing no arguments
./client 2>invalidtest1.log
if [ $? -ne 0 ]
then
  echo "Invalid no arguments test passed!"
fi

# Testing too many arguments
./client 2>invalidtest2.log plank $SERVER_PORT A B C
if [ $? -ne 0 ]
then
  echo "Invalid too many arguments test passed!"
fi

# Testing bad hostname
./client 2>invalidtest3.log globalhost $SERVER_PORT
if [ $? -ne 0 ]
then
  echo "Bad hostname test passed!"
fi

# Testing bad port number
./client 2>invalidtest4.log plank 10
if [ $? -ne 0 ]
then
  echo "Bad port number test passed!"
fi

# Testing player with main.txt
./client 2>player1.log plank $SERVER_PORT player1 
if [ $? -eq 0 ]
then
  echo "Normal client with main.txt test passed!"
fi

# Testing spectator with challenge.txt
./client 2>spectator.log plank $SERVER_PORT

# Valgrind test with main.txt
valgrind --leak-check=full --show-leak-kinds=all ./client 2>player2mem.log  plank $SERVER_PORT player3 


