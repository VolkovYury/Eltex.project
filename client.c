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

void printProduct(Product* elem) {
	printf("id: %u, name: %s, description: %s, price: %f, quantity %u\n",
	elem->id, elem->name, elem->description, elem->price, elem->quantity);
}

void printProductList(ProductList *list) {
	for(size_t i = 0; i < list->n_data; ++i) {
		printProduct(list->data[i]);
	}
}

int main() {
	struct sockaddr_in server;
	int talkingSocket;

	ProductList *list;
	Product *elem;

	server.sin_family = AF_INET;
	if(0 >= inet_pton(AF_INET, "127.0.0.1", &(server.sin_addr))) {
		printf("Wrong ip adress\n");
	}
	server.sin_port = htons(3001);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

	talkingSocket = socket(AF_INET, SOCK_STREAM, 0);

	connect(talkingSocket, (const struct sockaddr * )&server, sizeof(server));

	printf("Received card:\n");
	receiveProduct(talkingSocket, &elem);

	if(NULL == elem) {
		printf("Can't unpack\n");
	}
	else {
		printProduct(elem);
	}

	printf("Received table:\n");
	receiveProductList(talkingSocket, &list);

	if(NULL == list) {
		printf("Can't unpack\n");
	}
	else {
		printProductList(list);
	}
	product_list__free_unpacked(list, NULL);
	shutdown(talkingSocket, SHUT_RDWR);
	close(talkingSocket);
	return 0;
}