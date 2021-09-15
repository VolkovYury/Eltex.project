all: server.o client.o products.pb-c.o network.o
	gcc client.o products.pb-c.o network.o -o client -L./protobuf-c -lprotobuf-c -Wl,-rpath,./protobuf-c -Wall -Wextra -Werror
	gcc server.o products.pb-c.o network.o -o server -L./protobuf-c -lprotobuf-c -Wl,-rpath,./protobuf-c  -Wall -Wextra -Werror
server.o: server.c
	gcc server.c -c
client.o: client.c
	gcc client.c -c
products.pb-c.o: products.pb-c.c
	gcc products.pb-c.c -c
network.o: network.c
	gcc network.c -c
clean:
	rm -rf network.o server.o client.o products.pb-c.o client server