CC = g++
CFLAGS = -Wall -g

TARGET = test_program.exe

SRCS = test_program.cpp thread_manager.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del $(OBJS) $(TARGET)

.PHONY: all clean