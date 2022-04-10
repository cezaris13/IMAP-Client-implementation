build: 
	gcc -g  main.c -o main.o -L/usr/local/ssl/lib -lssl -lcrypto
	./main.o
	
debug: 
	gcc -g -Wall -lefence main.c -o main.o -L/usr/local/ssl/lib -lssl -lcrypto
	./main.o
