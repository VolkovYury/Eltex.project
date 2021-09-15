#include <stdio.h>
#include <unistd.h>
#include <string.h>//for memset
#include <stdlib.h>//for malloc
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>//for pton
#include <errno.h>
#include "products.pb-c.h"
#include "network.h"

int main(){
	struct sockaddr_in server;
	int connectionSocket;
	int talkingSocket;

	server.sin_family = AF_INET;
	if(0 >= inet_pton(AF_INET, "127.0.0.1", &(server.sin_addr))) {
		printf("Wrong ip adress\n");
	}
	server.sin_port = htons(3001);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));


	Product one = PRODUCT__INIT;
	one.id = 1;
	one.name = "Something";
	one.description = "This is Something";
	one.price = 1.0f;
	one.quantity = 5;
	Product two = PRODUCT__INIT;
	two.id = 2;
	two.name = "foo";
	two.description = "Lorem ipsum";
	two.price = 1.5f;
	two.quantity = 52;
	Product three = PRODUCT__INIT;
	three.id = 3;
	three.name = "The thing";
	three.description = "Super cool thing";
	three.price = 25.f;
	three.quantity = 1;
	ProductList list = PRODUCT_LIST__INIT;
	list.n_data = 3;
	list.data = malloc(sizeof(Product *) * 3);
	list.data[0] = &one;
	list.data[1] = &two;
	list.data[2] = &three;


	connectionSocket = socket(AF_INET, SOCK_STREAM, 0);
	bind(connectionSocket, (const struct sockaddr * )&server, sizeof(server));
	listen(connectionSocket, 1);
	talkingSocket = accept(connectionSocket, 0, 0);

	printf("Send one card\n");
	sendProduct(talkingSocket, &one);
	printf("Send all table\n");
	sendProductList(talkingSocket, &list);
	free(list.data);

	
	shutdown(talkingSocket, SHUT_RDWR);
	close(talkingSocket);
	shutdown(connectionSocket, SHUT_RDWR);
	close(connectionSocket);
	return 0;
}