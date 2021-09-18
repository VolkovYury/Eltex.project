#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/poll.h>
#include <pthread.h>
#include "network.h"
#include "server_functions.h"

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
		//HERE SHOULD BE updateJson() FUNCTION
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
void handleClient(int fd, interprocessdata *sharedData)
{
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
		//if nothing happend
		else if ( ret == 0 ) {
			continue;
		}
		//if can receive somthing happend
		poolData.revents = 0;
		if (0 == recv(fd, &signal, 1, 0)) {
			printf("Connection %d closed\n", fd);
			return ;
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
			printf("recived strange signal%u\n", signal);
			break;
		}
	}
}