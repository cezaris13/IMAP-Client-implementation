init:
	cp .env_copy .env
	
build: 
	g++ -g -std=c++23 main.cpp -o main.o -L/usr/local/ssl/lib -lssl -lcrypto
	./main.o
	
debug: 
	g++ -g -Wall -lefence main.cpp sockets.cpp appLayer.cpp -o main.o -L/usr/local/ssl/lib -lssl -lcrypto
	./main.o
