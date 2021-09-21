#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/poll.h>
#include <pthread.h>
#include <arpa/inet.h>//for pton
#include "network.h"
#include "server_functions.h"

int makeNewProductList(ProductList **list, size_t n_data)
{
	product_list__init(*list);
	(*list)->n_data = n_data;
	(*list)->data = malloc(sizeof(Product *) * n_data);
	if (NULL == (*list)->data) {
		return -1;
	}
	return 0;
}

void makeNewProduct(Product *elem, int id, char *name, char *description, float price, uint32_t quantity)
{
	product__init(elem);
	elem->id = id;
	elem->name = name;
	elem->description = description;
	elem->price = price;
	elem->quantity = quantity;
}

//Print one product to terminal
void printProduct(Product* elem)
{
	printf("id: %u, name: %s, description: %s, price: %f, quantity %u\n",
	elem->id, elem->name, elem->description, elem->price, elem->quantity);
}

//Print database
void printProductList(ProductList *list)
{
	for(size_t i = 0; i < list->n_data; ++i) {
		printProduct(list->data[i]);
	}
}

//set exit_signal to 1
void markExit(interprocessdata *shared)
{
	pthread_mutex_lock(&(shared->data_mutex));
	shared->exit_signal = 1;
	pthread_mutex_unlock(&(shared->data_mutex));
}

//Creates socket for server, return -1 in case of error.
int makeListenSocket(const char *address, uint16_t port)
{
	struct sockaddr_in server;
	int connectionSocket;
	server.sin_family = AF_INET;
	if (0 >= inet_pton(AF_INET, address, &(server.sin_addr))) {
		printf("Wrong ip address\n");
		return -1;
	}
	server.sin_port = htons(port);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

	connectionSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == connectionSocket) {
		printf("Can't create socket\n");
		return -1;
	}
	if (-1 == bind(connectionSocket, (const struct sockaddr * )&server, sizeof(server))) {
		printf("Bind error\n");
		return -1;
	}
	if (-1 == listen(connectionSocket, 1)) {
		printf("Listen error\n");
		return -1;
	}
	return connectionSocket;
}

//Function what read commands from terminal
void *listenInput(void *data)
{
	interprocessdata *sharedData = (interprocessdata *)(data);
	char cmd[20];
	while (1) {
		scanf("%s", cmd);
		if (!strcmp("exit", cmd)) {
			markExit(sharedData);
			break;
		}
		else if (!strcmp("show", cmd)) {
			printf("---------------\n");
			printProductList(sharedData->database);
			printf("---------------\n");
		}
	}
	return NULL;
}

//Function what add new pthreadId and can resize pthread array if needed.
int addPthreadToList(pthread_t thread, pthread_t **list, size_t *maxSize, size_t *size)
{
	size_t startSize = 10;
	size_t delta = 5;
	if (NULL == *list) {
		*list = malloc(sizeof(pthread_t) * startSize);
		if (NULL == *list) {
			return -1;
		}
		*size = 1;
		*maxSize = startSize;
	}
	else {
		++(*size);
		if (*maxSize < *size) {
			*maxSize += delta;
			*list = realloc(*list, *maxSize * sizeof(pthread_t));
			if (NULL == *list) {
				return -1;
			}
		}
	}
	(*list)[*size - 1] = thread;
	return 0;
}

//Wait for all threads
void waitForThreads(pthread_t *list, size_t size)
{
	for(size_t i = 0; i < size; ++i) {
		pthread_join(list[i], NULL);
	}
}

//Search for card in database, if nothing found return database size
size_t findCard(const Product *card, const ProductList *database)
{
	for (size_t i = 0; i < database->n_data; ++i) {
		if (card->id == database->data[i]->id) {
			return i;
		}
	}
	return database->n_data;
}

//Handle one order request from client
uint8_t handleOrder(int fd, interprocessdata *sharedData)
{
	Product *card = NULL;
	size_t i;

	//Receive order
	if (-1 == receiveProduct(fd, &card)) {
		printf("Can't receive product\n");
		return FAILURE;
	}

	//Search for card in database
	i = findCard(card, sharedData->database);
	//If nothing found
	if (i == sharedData->database->n_data) {
		product__free_unpacked(card, NULL);
		return FAILURE;
	}
	pthread_mutex_lock(&(sharedData->data_mutex));
	//if where is enough products
	if (sharedData->database->data[i]->quantity >= card->quantity) {
		sharedData->database->data[i]->quantity -= card->quantity;
		//updateJSON();
		pthread_mutex_unlock(&(sharedData->data_mutex));
		product__free_unpacked(card, NULL);
		return ACCEPT;
	}
	else {
		pthread_mutex_unlock(&(sharedData->data_mutex));
		product__free_unpacked(card, NULL);
		return FAILURE;
	}
}

//Handle one client in a loop, until exit_signal is 0 and client online
void *handleClient(void *args)
{
	int fd = ((threadData *)args)->fd;
	interprocessdata *sharedData = ((threadData *)args)->shared;
        uint8_t signal;
	uint8_t respond;
	struct pollfd poolData;
	poolData.fd = fd;
	poolData.events = POLLIN;
	int ret;
	//Listen one client while server is up
	while (!(sharedData->exit_signal)) {
		ret = poll( &poolData, 1, CLIENT_POOL_TIME );
		// if where is error
		if ( ret == -1 ) {
			printf("Connection %d closed\n", fd);
			break;
		}
		//if nothing happened
		else if ( ret == 0 ) {
			continue;
		}
		//if can receive something happened
		poolData.revents = 0;
		if (0 == recv(fd, &signal, 1, 0)) {
			printf("Connection %d closed\n", fd);
			return NULL;
		}
		switch (signal) {
		case DATABASE_REQUEST:
			sendProductList(fd, sharedData->database);
			break;
		case ORDER_REQUEST:
			respond = handleOrder(fd, sharedData);
			if (-1 == send(fd, &respond, 1, 0)) {
				printf("Can't send respond\n");
				break;
			}
			break;

		default:
                        printf("Received strange signal%u\n", signal);
                        break;
		}
	}
	shutdown(fd, SHUT_RDWR);
	close(fd);
	return NULL;
}