CC = g++
CFLAGS = -std=c++11 -g
CUR_PATH = $(shell pwd)
TOP_PATH = $(CUR_PATH)
INC_PATH = $(TOP_PATH)/header
object = server client server.o client.o Message.o
MYSQL = `mysql_config --cflags --libs`
proc: server client
server: server.o Message.o
	$(CC) $(CFLAGS) -o server server.o Message.o $(MYSQL)
client: client.o Message.o
	$(CC) $(CFLAGS) -o client client.o Message.o $(MYSQL)
client.o: client.cpp 
	$(CC) $(CFLAGS) -c client.cpp $(MYSQL)
server.o: server.cpp
	$(CC) $(CFLAGS) -c server.cpp $(MYSQL)
Message.o: $(INC_PATH)/Message/Message.h $(INC_PATH)/Message/Message.cpp 
	$(CC) $(CFLAGS) -c $(INC_PATH)/Message/Message.cpp
clean: 
	rm $(object)
