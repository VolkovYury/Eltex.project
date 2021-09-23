all: server.o client.o products.pb-c.o network.o server_functions.o client_functions.o wrapperfunc.o jsonEditor.o
	gcc client.o products.pb-c.o network.o client_functions.o wrapperfunc.o -lm -o client -lpthread -L./protobuf-c -lprotobuf-c -Wl,-rpath,./protobuf-c -Wall -Wextra -Werror
	gcc server.o products.pb-c.o network.o jsonEditor.o server_functions.o wrapperfunc.o -lm -o server -lpthread -L./protobuf-c -lprotobuf-c -Wl,-rpath,./protobuf-c -L./json-cH -ljson-c -Wl,-rpath,./json-cH -Wall -Wextra -Werror
server.o: server.c
	gcc server.c -c
client.o: client.c
	gcc client.c -c
products.pb-c.o: products.pb-c.c
	gcc products.pb-c.c -c
network.o: network.c
	gcc network.c -c
jsonEditor.o: jsonEditor.c
	gcc jsonEditor.c -c
server_functions.o: server_functions.c
	gcc server_functions.c -c
client_functions.o: client_functions.c
	gcc client_functions.c -c
wrapperfunc.o: wrapperfunc.c
	gcc wrapperfunc.c -c
clean:
	rm -rf network.o server.o client.o products.pb-c.o server_functions.o client_functions.o wrapperfunc.o jsonEditor.o client server
debug:
	#gcc -g client.c products.pb-c.c network.c client_functions.c wrapperfunc.c -o client -lpthread -L./protobuf-c -lprotobuf-c -Wl,-rpath,./protobuf-c -Wall -Wextra -Werror
	gcc -g server.c products.pb-c.c network.c server_functions.c wrapperfunc.c jsonEditor.c -o server -lpthread -L./protobuf-c -lprotobuf-c -Wl,-rpath,./protobuf-c -L./json-cH -ljson-c -Wl,-rpath,./json-cH -Wall -Wextra -Werror
