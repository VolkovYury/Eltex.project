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
	int connectionSocket = -1;
	int talkingSocket = 0;
	pthread_t commandThread = 0;
	pthread_t *pthreadList = NULL;
	size_t pthreadsMax;
	size_t pthreadsSize;
	interprocessdata sharedData = {0, PTHREAD_MUTEX_INITIALIZER, NULL};
	struct pollfd poolData;

	if (-1 == load_to_ProductList(&sharedData)) {
		printf("Can't load product list\n");
		goto FREE_AND_EXIT;
	}
	
	if (0 != pthread_create(&commandThread, NULL, listenInput, (void *)(&sharedData))) {
		printf("Can't create command thread\n");
		goto FREE_AND_EXIT;
	}

	connectionSocket = makeListenSocket("127.0.0.1", 3001);
	if (-1 == connectionSocket) {
		goto FREE_AND_EXIT;
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
			goto FREE_AND_EXIT;
		}
		if (0 != pthread_create(&locId, NULL, handleClient, (void *)(&tmp))) {
			printf("Can't create thread\n");
			goto FREE_AND_EXIT;
		}
		addPthreadToList(locId, &pthreadList, &pthreadsMax, &pthreadsSize);
	}

	printf("Server finaly can stop\n");
	printProductList(sharedData.database);

FREE_AND_EXIT:
	if (NULL != pthreadList) {
		waitForThreads(pthreadList, pthreadsSize);
	}
	freeProductList(sharedData.database);
	pthread_mutex_destroy(&sharedData.data_mutex);
	if (-1 != connectionSocket) {
		shutdown(connectionSocket, SHUT_RDWR);
		close(connectionSocket);
	}
	return 0;
}