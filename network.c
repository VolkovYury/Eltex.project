#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h> 
#include "network.h"

int sendProduct(int fd, const Product *element){
	void *resultBuffer;
	size_t resultBufferSize;

	resultBufferSize = product__get_packed_size(element);
	resultBuffer = malloc(resultBufferSize);
	product__pack(element, resultBuffer);
	resultBufferSize = htonl(resultBufferSize);
	send(fd, &resultBufferSize, 4, 0);
	resultBufferSize = ntohl(resultBufferSize);
	send(fd, resultBuffer, resultBufferSize, 0);
	free(resultBuffer);
	return 0;
}
int receiveProduct(int fd, Product **element){
	void *resultBuffer;
	size_t resultBufferSize;

	recv(fd, &resultBufferSize, 4, 0);
	resultBufferSize = ntohl(resultBufferSize);
	resultBuffer = malloc(resultBufferSize);
	recv(fd, resultBuffer, resultBufferSize, 0);

	*element = product__unpack(NULL, resultBufferSize, resultBuffer);
	free(resultBuffer);
	return 0;
}
int sendProductList(int fd, const ProductList *element){
	void *resultBuffer;
	size_t resultBufferSize;

	resultBufferSize = product_list__get_packed_size(element);
	resultBuffer = malloc(resultBufferSize);
	product_list__pack(element, resultBuffer);
	resultBufferSize = htonl(resultBufferSize);
	send(fd, &resultBufferSize, 4, 0);
	resultBufferSize = ntohl(resultBufferSize);
	send(fd, resultBuffer, resultBufferSize, 0);
	free(resultBuffer);
	return 0;
}
int receiveProductList(int fd, ProductList **element){
	void *resultBuffer;
	size_t resultBufferSize;

	recv(fd, &resultBufferSize, 4, 0);
	resultBufferSize = ntohl(resultBufferSize);
	resultBuffer = malloc(resultBufferSize);
	recv(fd, resultBuffer, resultBufferSize, 0);

	*element = product_list__unpack(NULL, resultBufferSize, resultBuffer);
	free(resultBuffer);
	return 0;
}