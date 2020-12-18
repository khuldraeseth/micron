FLAGS=-std=c++2a -fconcepts -Wall -Wextra -Wpedantic -Wno-unused -Werror -Iinclude -Og -ggdb3

vpath %.hpp ./include

exec: main.cpp $(shell find include -type f) Makefile
	g++ $(FLAGS) -o exec main.cpp

.PHONY: clean
clean:
	rm -fr exec
