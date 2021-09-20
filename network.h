#ifndef NETWORK_H
#define NETWORK_H

#include "products.pb-c.h"
#include <pthread.h>
#include <stdint.h>
#define DATABASE_REQUEST (uint8_t)10
#define ORDER_REQUEST (uint8_t)11
#define FAILURE (uint8_t)20
#define ACCEPT (uint8_t)21
#define SERVER_DISCOVER_TIME (int)1000
#define CLIENT_POOL_TIME (int)2000

int sendProduct(int fd, const Product *element);
int receiveProduct(int fd, Product **element);
int sendProductList(int fd, const ProductList *element);
int receiveProductList(int fd, ProductList **element);


#endif
