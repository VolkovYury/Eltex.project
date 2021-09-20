#ifndef CLIENT_FUNCTIONS
#define CLIENT_FUNCTIONS
#include <stdint.h>
#include "products.pb-c.h"

void orderCard(int fd, const Product *card, ProductList **list);
int requestDatabase(int fd, ProductList **list);
size_t findCard(const Product *card, const ProductList *database);

int connection(struct sockaddr_in servaddr);
void printMenu();
void printInfo(Product *elem);
void printDatabase(ProductList *list);
void printOneInfo(Product *elem);
#endif