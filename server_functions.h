#ifndef SERVER_FUNCTIONS
#define SERVER_FUNCTIONS
#include <stdint.h>
#include "products.pb-c.h"

typedef struct{
	uint8_t exit_signal;
	pthread_mutex_t data_mutex;
	ProductList *database;
} interprocessdata;

uint8_t handleOrder(int fd, interprocessdata *sharedData);
void handleClient(int fd, interprocessdata *sharedData);
size_t findCard(const Product *card, const ProductList *database);

#endif