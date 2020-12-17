FLAGS=-std=c++2a -fconcepts -Wall -Wextra -Wpedantic -Wno-unused -Werror -Og -ggdb3

exec: main.cpp *.hpp Makefile
	g++-9 $(FLAGS) -o exec main.cpp

.PHONY: clean
clean: rm -fr exec
