#!/bin/bash

# Testing no arguments
./client 
if [ $? -ne 0 ]
then
  echo "Invalid no arguments test passed!"
fi

# Testing too many arguments
./client 2>player.log plank 58516 A B C
if [ $? -ne 0 ]
then
  echo "Invalid too many arguments test passed!"
fi

# Testing bad hostname
./client 2>player.log globalhost 58516
if [ $? -ne 0 ]
then
  echo "Bad hostname test passed!"
fi

# Testing bad port number
./client 2>player.log plank 10
if [ $? -ne 0 ]
then
  echo "Bad port number test passed!"
fi

# Testing player with main.txt
./client 2>player1.log plank  player1 << EOF
kjhlkkkjjjnq 
EOF

if [ $? -eq 0 ]
then
  echo "Normal client with main.txt test passed!"
fi

# Testing player with challenge.txt
./client 2>player2.log plank 48830 player2 << EOF
kjhlkkkjjjnq 
EOF

if [ $? -eq 0 ]
then
  echo "Normal client test with challenge.txt passed!"
fi

# Testing spectator with challenge.txt
./client 2>spectator1.log plank 48830 << EOF
q
EOF

# Valgrind test with main.txt
valgrind --leak-check=full --show-leak-kinds=all ./client 2>player3mem.log  plank 58288 player3 << EOF
q
EOF

# Valgrind test with challenge.txt
valgrind --leak-check=full --show-leak-kinds=all ./client 2>player4mem.log  plank 48830 player4 << EOF
q
EOF
