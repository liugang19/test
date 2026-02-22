CC = g++
CFLAGS = -Wall -g
LIBS = -lpthread

TARGET = test_program

SRCS = test_program.cpp thread_manager.cpp thread_manager_pthread.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean