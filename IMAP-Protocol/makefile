build: 
	g++ -g  main.cpp -o main.o -L/usr/local/ssl/lib -lssl -lcrypto
	./main.o
	
debug: 
	g++ -g -Wall -lefence main.cpp sockets.cpp appLayer.cpp -o main.o -L/usr/local/ssl/lib -lssl -lcrypto
	./main.o
