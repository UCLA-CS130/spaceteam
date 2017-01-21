# spaceteam Makefile

CC = g++
CFLAGS = -std=c++0x -g -Wall

TARGET = config_parser

PARSER_OBJECTS = config_parser.o config_parser_main.o

all: parser

parser:	$(PARSER_OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(PARSER_OBJECTS)

config_parser.o: config_parser.cc config_parser.h
	$(CC) $(CFLAGS) -c config_parser.cc

config_parser_main.o: config_parser_main.cc
	$(CC) $(CFLAGS) -c config_parser_main.cc

clean:
	$(RM) *.o *~ $(TARGET)
