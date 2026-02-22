CC = qcc
CFLAGS = -Wall -g -std=c++11
LIBS = -lpthread

TARGET = thread_test

SRCS = test_program.cpp thread_manager.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean