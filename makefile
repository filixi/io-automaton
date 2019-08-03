INCLUDE = -I include/
CCFLAG = -std=c++17 -fconcepts -Wall -Werror -Wfatal-errors -Wno-parentheses -pedantic
CC = g++

debug : CCFLAG += -g3

OBJS =

%.o : src/%.cc include/%.h
	$(CC) -c $< $(CCFLAG) $(INCLUDE)

main : $(OBJS)
	$(CC) src/main.cc $(OBJS) -o a.out $(CCFLAG) $(INCLUDE)

debug : main

clear :
	rm *.o *.out