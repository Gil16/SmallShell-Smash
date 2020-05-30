COMPILER := g++
COMPILER_FLAGS := --std=c++11 -Wall
SRCS := Commands.cpp signals.cpp smash.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
HDRS := Commands.h signals.h
PROG_BIN := prog

$(PROG_BIN): $(OBJS)
	$(COMPILER) $(COMPILER_FLAGS) $^ -o $@

$(OBJS): %.o: %.cpp
	$(COMPILER) $(COMPILER_FLAGS) -c $^