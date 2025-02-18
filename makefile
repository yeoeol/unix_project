all: server client

server: server.o data.o
	gcc -o server server.o data.o
	
client: client.o
	gcc -o client client.o
	
server.o: server.c
	gcc -c server.c
	
data.o: data.c
	gcc -c data.c
	
client.o: client.c
	gcc -c client.c
	
clean:
	rm -f *.o
