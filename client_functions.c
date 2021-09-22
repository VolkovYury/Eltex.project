#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <string.h>

#include "network.h"
#include "products.pb-c.h"
#include "wrapperfunc.h"
#include "client_functions.h"

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

// Order one card and handle answer
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

// The function is responsible for connecting the client to the server at a known ip-address and port
int connection(struct sockaddr_in servaddr)
{
        char addressServ[16];

        int fd = Socket(AF_INET, SOCK_STREAM, 0);

        printf("================================================================================\n");
        printf("Enter the IP address of the server: ");
        enterIP(addressServ);
        printf("Enter the server port number: ");
        int port = enterNumber(1, 65535);
        printf("================================================================================\n");

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        memset(servaddr.sin_zero, 0, sizeof(servaddr.sin_zero));
        Inet_pton(AF_INET, addressServ, &servaddr.sin_addr);

        Connect(fd, (struct sockaddr *) &servaddr, sizeof servaddr);

        return fd;
}

// The function prints menu to the console
void printMenu()
{
        printf("=====================================[MENU]=====================================\n");
        printf("1. Get all relevant information\n");
        printf("2. Get relevant information on the product\n");
        printf("3. Make a purchase request\n");
        printf("4. Exit\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("Use keyboard input to interact. To open a section of interest, enter its number and press \"Enter\".\n");
        printf("================================================================================\n");
}

// The function prints the contents of one card in the format of a table row to the console
void printInfo(Product *elem)
{
        // Number of lines in each field (based on field width and character array size)
        int numOfLines = 1;
        int stringId = 1;
        int stringName = 1;
        int stringDescription = 1;
        int stringPrice = 1;
        int stringPieces = 1;

        char separator = '|';
        /*
         * magic number:
         * 11 - uint32_t max 10-digit number + (\n)
         * 41 - 10 char * 4 string + (\n)
         * 181 - 45 char * 4 string + (\n)
         * 23 - 11 char * 2 string + (\n)
         */
        char id[11];
        snprintf(id, 11, "%u", elem->id);
        char name[41];
        snprintf(name, 41, "%s", elem->name);
        char description[181];
        snprintf(description, 181, "%s", elem->description);
        char price[23];
        snprintf(price, 23, "%.2f", elem->price);
        char pieces[11];
        snprintf(pieces, 11, "%u", elem->quantity);

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

        // Creating the structure of a table row
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

// The function prints the contents of the database to the console
void printDatabase(ProductList *list)
{
        if(NULL == list) {
                printf("Can't unpack\n");
        }
        else {
                printf("\n\n");
                printf("                           [All relevant information]                           \n");
                printf("--------------------------------------------------------------------------------\n");
                printf("|CODE|   NAME   |                 DESCRIPTION                 |   PRICE   |PCS |\n");
                printf("--------------------------------------------------------------------------------\n");

                for(size_t i = 0; i < list->n_data; ++i) {
                        printInfo(list->data[i]);
                }

                printf("\n\n");
        }
}

// The function prints the contents of one card to the console
void printOneInfo(Product *elem)
{
        if(NULL == elem) {
                printf("Can't unpack\n");
        }
        else {
                printf("\n\n");
                printf("                        [Information about one product!]                        \n");
                printf("--------------------------------------------------------------------------------\n");
                printf("|CODE|   NAME   |                 DESCRIPTION                 |   PRICE   |PCS |\n");
                printf("--------------------------------------------------------------------------------\n");

                printInfo(elem);
                printf("\n\n");
        }
}

// The function is responsible for user input of numbers
// min, max - the minimum and maximum value, respectively. These values are included in the range of values
// return - correct number. If the user does not enter a number, then strtol() will return 0.
int enterNumber(uint32_t min, uint32_t max)
{
        char enteredString[80];

        while (1) {
                scanf("%s", enteredString);
                int result = (int) strtol(enteredString, NULL, 10);

                if (result < min || result > max)
                        printf("WARNING: Invalid value. Try again:\n");
                else
                        return result;
        }
}

// The function is responsible for entering the IP-address by the user
// *res - pointer to an array of char with the received address
void enterIP(char *res) {
        char enteredString[80];
        int done = 1;

        while(done) {
                scanf("%s", enteredString);

                char ip[16] = "";               // Found IP-address
                char result[4];                 // Found number
                uint8_t countIndex = 0;         // Index counter of the found number
                uint8_t countNum = 0;           // Counter of the digits
                uint8_t countDot = 0;           // Counter of the dots
                uint8_t prevNumber = 0;         // Flag "Previous character is a number"

                for (int i = 0; enteredString[i] != '\0'; i++) {
                        // If digits are processed:
                        if (enteredString[i] >= 48 && enteredString[i] <= 57) {
                                countNum++;
                                if (countNum > 3) {
                                        printf("\"IP\" is wrong. The octet must be a number between 0 and 255, "
                                               "inclusive.\n Try again: ");
                                        break;
                                }

                                result[countIndex] = enteredString[i];
                                countIndex++;

                                if (enteredString[i+1] == '\0') {
                                        result[countIndex] = '\0';

                                        int octet = (int) strtol(result, NULL, 10);
                                        if (octet >=0 && octet <=255 && countDot == 3) {
                                                strcat(ip, ".");
                                                strncat(ip, result, 3);

                                                strcpy(res, ip);
                                                done = 0;
                                                break;
                                        } else {
                                                printf("\"IP\" is wrong. The octet must be a number between 0 and"
                                                       " 255, inclusive.\n Try again: ");
                                                break;
                                        }
                                }

                                prevNumber = 1;

                        // if dot is processed:
                        } else if (enteredString[i] == 46) {
                                countNum = 0;
                                countDot++;
                                if (countDot > 3) {
                                        printf("\"IP\" is wrong. \n Try again: ");
                                        break;
                                }

                                if (prevNumber) {
                                        result[countIndex] = '\0';

                                        int octet = (int) strtol(result, NULL, 10);
                                        if (octet >=0 && octet <=255) {

                                                if (countDot == 1) {
                                                        strncat(ip, result, 3);
                                                } else {
                                                        strcat(ip, ".");
                                                        strncat(ip, result, 3);
                                                }

                                                prevNumber = 0;
                                                countIndex = 0;
                                        } else {
                                                printf("\"IP\" is wrong. The octet must be a number between 0 and"
                                                       " 255, inclusive.\n Try again: ");
                                                break;
                                        }

                                } else {
                                        printf("\"IP\" is wrong. The IP can't start with a dot or contain two "
                                               "dots in a row.\n Try again: ");
                                        break;
                                }

                        } else {
                                printf("\"IP\" is wrong. The address contains invalid characters.\n Tyr again: ");
                                break;
                        }
                }
        }
}
