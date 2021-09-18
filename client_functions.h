#ifndef CLIENT_FUNCTIONS
#define CLIENT_FUNCTIONS
#include <stdint.h>
#include "products.pb-c.h"

void orderCard(int fd, const Product *card, ProductList **list);
int requestDatabase(int fd, ProductList **list);
size_t findCard(const Product *card, const ProductList *database);

#endif