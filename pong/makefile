LIBS=$(shell sdl2-config --libs)
CFLAGS=$(shell sdl2-config --cflags) -Wall -Wextra -ggdb
SOURCES=main.cpp
OBJS=$(SOURCES:.cpp=.o)

TARGET=pong

all:$(TARGET)

$(TARGET): $(OBJS)
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.cpp %.h
	g++ -c %< $(CFLAGS)

clean:
	rm *.o %(TARGET)
