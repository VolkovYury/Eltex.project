#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include "network.h"

//Pack Product and send it
int sendProduct(int fd, const Product *element)
{
	void *resultBuffer;
	size_t resultBufferSize;

	resultBufferSize = product__get_packed_size(element);
	resultBuffer = malloc(resultBufferSize);
	if (NULL == resultBuffer) {
		printf("Malloc error\n");
		return -1;
	}
	product__pack(element, resultBuffer);
	resultBufferSize = htonl(resultBufferSize);
	if (-1 == send(fd, &resultBufferSize, 4, 0)) {
		printf("Send size error\n");
		free(resultBuffer);
		return -1;
	}
	resultBufferSize = ntohl(resultBufferSize);
	if (-1 == send(fd, resultBuffer, resultBufferSize, 0)) {
		printf("Send product error\n");
		free(resultBuffer);
		return -1;
	}
	free(resultBuffer);
	return 0;
}

//Receive data and unpack it to product, all previous data freed.
//Product MUST be freed later.
int receiveProduct(int fd, Product **element)
{
	void *resultBuffer;
	size_t resultBufferSize;

	if (-1 == recv(fd, &resultBufferSize, 4, 0)) {
		printf("Receive size error\n");
		return -1;
	}
	resultBufferSize = ntohl(resultBufferSize);
	resultBuffer = malloc(resultBufferSize);
	if (NULL == resultBuffer) {
		printf("Malloc error\n");
		return -1;
	}
	if (-1 == recv(fd, resultBuffer, resultBufferSize, 0)) {
		printf("Receive product error\n");
		return -1;
	}

	product__free_unpacked(*element, NULL);
	*element = product__unpack(NULL, resultBufferSize, resultBuffer);
	free(resultBuffer);
	return 0;
}

//Pack ProductList and send it
int sendProductList(int fd, const ProductList *element)
{
	uint8_t *resultBuffer;
	size_t resultBufferSize;
	size_t realSize;

	resultBufferSize = product_list__get_packed_size(element);
	realSize = resultBufferSize;
	if (1400 < resultBufferSize) {
		realSize = 1400;
	}
	resultBuffer = malloc(resultBufferSize);
	if (NULL == resultBuffer) {
		printf("Malloc error\n");
		return -1;
	}
	product_list__pack(element, resultBuffer);
	resultBufferSize = htonl(resultBufferSize);
	if (-1 == send(fd, &resultBufferSize, 4, 0)) {
		printf("Send size error\n");
		free(resultBuffer);
		return -1;
	}
	resultBufferSize = ntohl(resultBufferSize);

	for (size_t size = 0; size < resultBufferSize; size += realSize) {
		if (-1 == send(fd, resultBuffer + size, realSize, 0)) {
			printf("Send product error\n");
			free(resultBuffer);
			return -1;
		}
	}
	free(resultBuffer);
	return 0;
}

//Receive data and unpack it to productlist, all previous data freed.
//Product MUST be freed later.
int receiveProductList(int fd, ProductList **element)
{
	void *resultBuffer;
	size_t resultBufferSize;
	size_t realSize;

	if (-1 == recv(fd, &resultBufferSize, 4, 0)) {
		printf("Receive size error\n");
		return -1;
	}
	resultBufferSize = ntohl(resultBufferSize);

	realSize = resultBufferSize;
	if (1400 < resultBufferSize) {
		realSize = 1400;
	}
	resultBuffer = malloc(resultBufferSize);
	if (NULL == resultBuffer) {
		printf("Malloc error\n");
		return -1;
	}

	for (size_t size = 0; size < resultBufferSize; size += realSize) {
		if (-1 == recv(fd, resultBuffer + size, realSize, 0)) {
			printf("Receive product error\n");
			free(resultBuffer);
			return -1;
		}
	}

	*element = product_list__unpack(NULL, resultBufferSize, resultBuffer);
	free(resultBuffer);
	return 0;
}