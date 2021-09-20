#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <string.h>
#include "network.h"
#include "products.pb-c.h"
#include "wrapperfunc.h"
#include "client_functions.h"
#include "network.h"
#include "products.pb-c.h"

// Search for card in database, if nothing found return database size
size_t findCard(const Product *card, const ProductList *database)
{
	for(size_t i = 0; i < database->n_data; ++i) {
		if (card->id == database->data[i]->id) {
			return i;
		}
	}
	return database->n_data;
}

//Request and update database
int requestDatabase(int fd, ProductList **list)
{
	ProductList *new = NULL;
	uint8_t signal = DATABASE_REQUEST;
	Send(fd, &signal, 1, 0);
	if (-1 == receiveProductList(fd, &new)) {
		printf("Can't receive database\n");
		return -1;
	}
	//free old database
	product_list__free_unpacked(*list, NULL);
	*list = new;
	return 0;
}

//Order one card and handle answer
void orderCard(int fd, const Product *card, ProductList **list)
{
	uint8_t signal = ORDER_REQUEST;
	size_t i;

	if (-1 == requestDatabase(fd, list)) {
		return;
	}

	//Search for card in database
	i = findCard(card, *list);
	//If nothing found
	if(i == (*list)->n_data) {
		printf("No card in database\n");//THIS SHOULD BE CHANGED
		return;
	}

	if (-1 == send(fd, &signal, 1, 0)) {
		printf("Can't send signal\n");
		return;
	}
	if (-1 == sendProduct(fd, card)) {
		printf("Can't send product\n");
		return;
	}
	if (-1 == recv(fd, &signal, 1, 0)) {
		printf("Can't receive signal\n");
		return;
	}

	if(signal == ACCEPT){
		(*list)->data[i]->quantity -= card->quantity;
	}
	else {
		printf("Order failed.\n");//THIS SHOULD BE CHANGED
		if (-1 == requestDatabase(fd, list)) {
			return;
		}
	}
}

//
int connection(struct sockaddr_in servaddr) {
        char addressServ[16];
        int port;

        int fd = Socket(AF_INET, SOCK_STREAM, 0);

        printf("================================================================================\n");
        printf("Enter the IP address of the server (UNSAFE): ");
        scanf("%s", addressServ);
        printf("Enter the server port number (UNSAFE): ");
        scanf("%d", &port);
        printf("================================================================================\n");

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        memset(servaddr.sin_zero, 0, sizeof(servaddr.sin_zero));
        Inet_pton(AF_INET, addressServ, &servaddr.sin_addr);

        Connect(fd, (struct sockaddr *) &servaddr, sizeof servaddr);

        return fd;
}

//
void printMenu() {
        printf("=====================================[MENU]=====================================\n");
        printf("1. Get all relevant information\n");
        printf("2. Get up-to-date information on the product\n");
        printf("3. Make a purchase request\n");
        printf("4. Exit\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("Use keyboard input to interact. To open a section of interest, enter its number and press \"Enter\".\n");
        printf("Enter \"0\" to display the menu again\n");
        printf("================================================================================\n");
}

// Функция построения строки таблицы
void printInfo(Product *elem) {
        // Количество строк в каждом поле (исходя из ширины поля и размера массива символов)
        int numOfLines = 1;
        int stringId = 1;
        int stringName = 1;
        int stringDescription = 1;
        int stringPrice = 1;
        int stringPieces = 1;

        char separator = '|';
        /*
         * magic number:
         * 9 - 4 char * 2 string + (\n)
         * 41 - 10 char * 4 string + (\n)
         * 181 - 45 char * 4 string + (\n)
         * 23 - 11 char * 2 string + (\n)
         * 9 - 4 char * 2 string + (\n)
         */
        char id[9];
        snprintf(id, 9, "%u", elem->id);
        char name[41];
        snprintf(name, 41, "%s", elem->name);
        char description[181];
        snprintf(description, 181, "%s", elem->description);
        char price[23];
        snprintf(price, 23, "%.2f", elem->price);
        char pieces[9];
        snprintf(pieces, 9, "%u", elem->quantity);

        for (int ptr = 1; ptr <= strlen(id); ptr = ptr + 4) {
                if ((float) ptr / 4 > (float) numOfLines) {
                        numOfLines++;
                        stringId++;
                }
        }

        for (int ptr = 1; ptr <= strlen(name); ptr = ptr + 10) {
                if ((float) ptr / 10 > (float) numOfLines) {
                        numOfLines++;
                        stringName++;
                }
        }

        for (int ptr = 1; ptr <= strlen(description); ptr = ptr + 45) {
                if ((float) ptr / 45 > (float) numOfLines) {
                        numOfLines++;
                        stringDescription++;
                }
        }

        for (int ptr = 1; ptr <= strlen(price); ptr = ptr + 11) {
                if ((float) ptr / 11 > (float) numOfLines) {
                        numOfLines++;
                        stringPrice++;
                }
        }

        for (int ptr = 1; ptr <= strlen(pieces); ptr = ptr + 4) {
                if ((float) ptr / 4 > (float) numOfLines) {
                        numOfLines++;
                        stringPieces++;
                }
        }

        // Построение структуры таблицы
        for (int i = 1; i <= numOfLines; i++) {

                if (i <= stringId) {
                        printf("%c", separator);
                        printf("%-4.4s|", id + 4 * (i - 1));
                } else {
                        printf("%c", separator);
                        printf("%5c", separator);
                }

                if (i <= stringName) {
                        printf("%-10.10s|", name + 10 * (i - 1));
                } else {
                        printf("%11c", separator);
                }

                if (i <= stringDescription) {
                        printf("%-45.45s|", description + 45 * (i - 1));
                } else {
                        printf("%46c", separator);
                }

                if (i <= stringPrice) {
                        printf("%-11.11s|", price + 11 * (i - 1));
                } else {
                        printf("%12c", separator);
                }

                if (i <= stringPieces) {
                        printf("%-4.4s|\n", pieces + 4 * (i - 1));
                } else {
                        printf("%5c\n", separator);
                }

        }

        printf("--------------------------------------------------------------------------------\n");
}

// Функция вывода в консоль содержимого всей таблицы
void printDatabase(ProductList *list) {
        if(NULL == list) {
                printf("Can't unpack\n");
        }
        else {
                printf("                           [All relevant information]                           \n");
                printf("--------------------------------------------------------------------------------\n");
                printf("|CODE|   NAME   |                 DESCRIPTION                 |   PRICE   |PCS |\n");
                printf("--------------------------------------------------------------------------------\n");

                for(size_t i = 0; i < list->n_data; ++i) {
                        printInfo(list->data[i]);
                }
        }
}

// Функция вывода в консоль содержимого одного товара
void printOneInfo(Product *elem) {
        if(NULL == elem) {
                printf("Can't unpack\n");
        }
        else {
                printf("                        [Information about one product!]                        \n");
                printf("--------------------------------------------------------------------------------\n");
                printf("|CODE|   NAME   |                 DESCRIPTION                 |   PRICE   |PCS |\n");
                printf("--------------------------------------------------------------------------------\n");

                printInfo(elem);
        }
}