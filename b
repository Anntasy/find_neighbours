#!/bin/bash
g++ -std=c++20 main.cpp Dots.cpp Grid.cpp -o res -I/opt/homebrew/opt/boost/include -L/opt/homebrew/opt/boost/lib -lboost_filesystem -lboost_thread -lboost_chrono -lboost_process
# g++ -std=c++20 main_student.cpp student.cpp thread.cpp -o res -I/opt/homebrew/opt/boost/include -L/opt/homebrew/opt/boost/lib -lboost_filesystem -lboost_thread -lboost_chrono -lboost_process
./res
