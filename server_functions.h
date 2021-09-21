#ifndef SERVER_FUNCTIONS
#define SERVER_FUNCTIONS
#include <stdint.h>
#include "products.pb-c.h"

typedef struct{
	uint8_t exit_signal;
	pthread_mutex_t data_mutex;
	ProductList *database;
} interprocessdata;

typedef struct{
	interprocessdata *shared;
	int fd;
} threadData;

uint8_t handleOrder(int fd, interprocessdata *sharedData);
void * handleClient(void *args);
size_t findCard(const Product *card, const ProductList *database);
void waitForThreads(pthread_t *list, size_t size);
int addPthreadToList(pthread_t thread, pthread_t **list, size_t *maxSize, size_t *size);
void *listenInput(void *data);
int makeListenSocket(const char *address, uint16_t port);
void markExit(interprocessdata *shared);
void printProductList(ProductList *list);
void printProduct(Product* elem);
int makeNewProduct(Product **elem, uint32_t id, const char *name, const char *description, float price, uint32_t quant);
int makeNewProductList(ProductList **list, size_t n_data);
void freeProduct(Product *elem);
void freeProductList(ProductList *list);

#endif