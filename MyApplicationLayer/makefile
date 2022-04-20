build: 
	gcc -g  main.c sockets.c appLayer.c -o main.o -L/usr/local/ssl/lib -lssl -lcrypto
	./main.o
	
debug: 
	gcc -g -Wall -lefence main.c sockets.c appLayer.c -o main.o -L/usr/local/ssl/lib -lssl -lcrypto
	./main.o
