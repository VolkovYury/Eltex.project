#include <stdio.h>
#include <unistd.h>
#include <string.h>//for memset
#include <stdlib.h>//for malloc
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>//for pton
#include <errno.h>
#include <pthread.h>
#include "products.pb-c.h"
#include "network.h"
#include "server_functions.h"


void printProduct(Product* elem) {
	printf("id: %u, name: %s, description: %s, price: %f, quantity %u\n",
	elem->id, elem->name, elem->description, elem->price, elem->quantity);
}

void printProductList(ProductList *list) {
	for(size_t i = 0; i < list->n_data; ++i) {
		printProduct(list->data[i]);
	}
}


int main(){
	struct sockaddr_in server;
	int connectionSocket;
	int talkingSocket;
	interprocessdata sharedData = {0, PTHREAD_MUTEX_INITIALIZER, NULL};

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

	sharedData.database = &list;


	connectionSocket = socket(AF_INET, SOCK_STREAM, 0);
	bind(connectionSocket, (const struct sockaddr * )&server, sizeof(server));
	listen(connectionSocket, 1);
	talkingSocket = accept(connectionSocket, 0, 0);

	handleClient(talkingSocket, &sharedData);
	printf("Server finaly can stop\n");
	printProductList(sharedData.database);

	free(sharedData.database->data);
	pthread_mutex_destroy(&sharedData.data_mutex);
	shutdown(talkingSocket, SHUT_RDWR);
	close(talkingSocket);
	shutdown(connectionSocket, SHUT_RDWR);
	close(connectionSocket);
	return 0;
}