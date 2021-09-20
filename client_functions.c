#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include "client_functions.h"
#include "network.h"
#include "products.pb-c.h"

//Request and update database
int requestDatabase(int fd, ProductList **list)
{
	ProductList *new = NULL;
	uint8_t signal = DATABASE_REQUEST;
	send(fd, &signal, 1, 0);
	if (-1 == receiveProductList(fd, &new)) {
		printf("Can't receive database\n");
		return -1;
	}
	//free old database
	product_list__free_unpacked(*list, NULL);
	*list = new;
	return 0;
}

//Search for card in database, if nothing found return database size
size_t findCard(const Product *card, const ProductList *database)
{
	for(size_t i = 0; i < database->n_data; ++i) {
		if (card->id == database->data[i]->id) {
			return i;
		}
	}
	return database->n_data;
}

//Order one card and handle answer
void orderCard(int fd, const Product *card, ProductList **list)
{
	uint8_t signal = ORDER_REQUEST;
	size_t i;

	if (-1 == requestDatabase(fd, list)) {
		return;
	}

	//Search for card in database
	i = findCard(card, *list);
	//If nothing found
	if(i == (*list)->n_data) {
		printf("No card in database\n");//THIS SHOULD BE CHANGED
		return;
	}

	if (-1 == send(fd, &signal, 1, 0)) {
		printf("Can't send signal\n");
		return;
	}
	if (-1 == sendProduct(fd, card)) {
		printf("Can't send product\n");
		return;
	}
	if (-1 == recv(fd, &signal, 1, 0)) {
		printf("Can't receive signal\n");
		return;
	}

	if(signal == ACCEPT){
		(*list)->data[i]->quantity -= card->quantity;
	}
	else {
		printf("Order failed.\n");//THIS SHOULD BE CHANGED
		if (-1 == requestDatabase(fd, list)) {
			return;
		}
	}
}
