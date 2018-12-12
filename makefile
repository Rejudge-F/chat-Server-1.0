all: server client
server: server.o
	g++ -std=c++11 -g -o server server.o `mysql_config --cflags --libs`
client: client.o
	g++ -std=c++11 -g -o client client.o `mysql_config --cflags --libs`
server.o: server.cpp
	g++ -std=c++11 -g -c server.cpp `mysql_config --cflags --libs`
client.o: client.cpp
	g++ -std=c++11 -g -c client.cpp `mysql_config --cflags --libs`
clean:all
	rm server client client.o server.o
