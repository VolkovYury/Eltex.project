#include <stdio.h>
#include <unistd.h>
#include <string.h>                     //for memset
#include <stdlib.h>                     //for malloc
#include <sys/socket.h>
#include <netinet/ip.h>

#include "products.pb-c.h"
#include "client_functions.h"
#include "wrapperfunc.h"

static struct sockaddr_in server;

int main()
{
        ProductList *list = NULL;

        int talkingSocket = connection(server);
        sleep(1);
        printMenu();

        while (1) {
                printf("Enter \"5\" to display the menu again\n");
                printf("Enter the section number: ");
                int menu = enterNumber(1, 5);
                int code = 0;

                switch (menu) {
                case PRINT_DATABASE:
                        requestDatabase(talkingSocket, &list);
                        printDatabase(list);
                        break;
                case PRINT_ONE_CARD:
                        printf("[Product selection for information]\n");
                        printf("Enter the product code: ");
                        code = enterNumber(1, 4294967295);      // max value - max uint32_t number
                        requestDatabase(talkingSocket, &list);

                        Product *oneCard = malloc(sizeof(Product));
                        product__init(oneCard);
                        oneCard->id = code;

                        size_t i = findCard(oneCard, list);

                        if (i == list->n_data)
                                printf("No card found in the database\n");
                        else
                                printOneInfo(list->data[i]);

                        free(oneCard);
                        break;
                case ORDER_REQST:
                        printf("[Product and quantity selection for order request]\n");
                        printf("Enter the product code: ");
                        code = enterNumber(1, 4294967295);
                        printf("Enter the quantity: ");
                        int pcs = enterNumber(1, 4294967295);

                        Product *card = malloc(sizeof(Product));
                        product__init(card);
                        card->id = code;
                        card->quantity = pcs;
                        orderCard(talkingSocket, card, &list);
                        free(card);

                        break;
                case CLOSE:
                        printf("Closing a connection...\n");
                        sleep(2);
                        product_list__free_unpacked(list, NULL);
                        Shutdown(talkingSocket, SHUT_RDWR);
                        Close(talkingSocket);
                        return EXIT_SUCCESS;
                case PRINT_MENU:
                        printMenu();
                        break;
                default:
                        printf("Invalid value entered!\n");
                }
        }

        return 0;
}
