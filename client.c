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
#include "client_functions.h"
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

	ProductList *list = NULL;

	server.sin_family = AF_INET;
	if(0 >= inet_pton(AF_INET, "127.0.0.1", &(server.sin_addr))) {
		printf("Wrong ip adress\n");
	}
	server.sin_port = htons(3001);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

	talkingSocket = socket(AF_INET, SOCK_STREAM, 0);

	connect(talkingSocket, (const struct sockaddr * )&server, sizeof(server));

	sleep(4);

	printf("Received table:\n");
	requestDatabase(talkingSocket, &list);

	printProductList(list);

	Product *card = malloc(sizeof(Product));
	product__init(card);
	card->id = list->data[0]->id;
	card->quantity = list->data[0]->quantity;
	orderCard(talkingSocket, card, &list);
	free(card);

	printProductList(list);

	product_list__free_unpacked(list, NULL);
	shutdown(talkingSocket, SHUT_RDWR);
	close(talkingSocket);
	return 0;
}