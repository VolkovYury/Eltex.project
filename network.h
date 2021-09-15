#ifndef NETWORK_H
#define NETWORK_H

#include "products.pb-c.h"

int sendProduct(int fd, const Product *element);
int receiveProduct(int fd, Product **element);
int sendProductList(int fd, const ProductList *element);
int receiveProductList(int fd, ProductList **element);

#endif
