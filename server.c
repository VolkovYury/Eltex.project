#include <stdio.h>
#include <unistd.h>
#include <string.h>//for memset
#include <stdlib.h>//for malloc
#include <stdint.h>//for uint8_t
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>//for pool
#include <netinet/ip.h> 
#include <arpa/inet.h>//for pton
#include <errno.h>
#include <pthread.h>
#include "products.pb-c.h"
#include "network.h"
#include "server_functions.h"
#include "jsonEditor.h"

#define MAX_CLIENTS_IN_QUEUE (size_t)10

int main()
{
	int connectionSocket;
	int talkingSocket;
	pthread_t commandThread;
	pthread_t *pthreadList;
	size_t pthreadsMax;
	size_t pthreadsSize;
	interprocessdata sharedData = {0, PTHREAD_MUTEX_INITIALIZER, NULL};
	struct pollfd poolData;

	//Fill database
	Product one = PRODUCT__INIT;
	one.id = 1;
	one.name = "Something";
	one.description = "This is Something";
	one.price = 1.0f;
	one.quantity = 5;
	Product two = PRODUCT__INIT;
	two.id = 2;
	two.name = "foo";
	two.description = "Lorem ipsum";
	two.price = 1.5f;
	two.quantity = 52;
	Product three = PRODUCT__INIT;
	three.id = 3;
	three.name = "The thing";
	three.description = "Super cool thing";
	three.price = 25.f;
	three.quantity = 1;
	ProductList list = PRODUCT_LIST__INIT;
	list.n_data = 3;
	list.data = malloc(sizeof(Product *) * 3);
	list.data[0] = &one;
	list.data[1] = &two;
	list.data[2] = &three;

	sharedData.database = &list;

	if (0 != pthread_create(&commandThread, NULL, listenInput, (void *)(&sharedData))) {
		printf("Can't create command thread\n");
		return -1;
	}

	connectionSocket = makeListenSocket("127.0.0.1", 3006);
	if (-1 == connectionSocket) {
		return -1;
	}
	
	poolData.fd = connectionSocket;
	poolData.events = POLLIN;
	while(!(sharedData.exit_signal)) {
		pthread_t locId = 0;
		threadData tmp = {&sharedData, talkingSocket};
		//Listen for incoming connections
		int ret = poll(&poolData, 1, SERVER_DISCOVER_TIME);
		// if where is error
		if ( ret == -1 ) {
			markExit(&sharedData);
			break;
		}
		//if nothing happend
		else if ( ret == 0 ) {
			continue;
		}

		//if receive somthing
		talkingSocket = accept(connectionSocket, 0, 0);
		if (-1 == talkingSocket) {
			printf("Can't accept connection\n");
			return -1;
		}
		if (0 != pthread_create(&locId, NULL, handleClient, (void *)(&tmp))) {
			printf("Can't create thread\n");
			return -1;
		}
		addPthreadToList(locId, &pthreadList, &pthreadsMax, &pthreadsSize);
	}

	printf("Server finaly can stop\n");
	printProductList(sharedData.database);

	waitForThreads(pthreadList, pthreadsSize);
	free(sharedData.database->data);
	pthread_mutex_destroy(&sharedData.data_mutex);
	shutdown(connectionSocket, SHUT_RDWR);
	close(connectionSocket);
	return 0;
}