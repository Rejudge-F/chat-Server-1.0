all: server client
server: server.o
	g++ -std=c++11 -g -o server server.o
client: client.o
	g++ -std=c++11 -g -o client client.o
server.o: server.cpp
	g++ -std=c++11 -g -c server.cpp
client.o: client.cpp
	g++ -std=c++11 -g -c client.cpp
clean:all
	rm server client client.o server.o
