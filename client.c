#include <stdio.h>
#include <unistd.h>
#include <string.h>                     //for memset
#include <stdlib.h>                     //for malloc
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>                  //for pton
#include <errno.h>

#include "products.pb-c.h"
#include "client_functions.h"
#include "network.h"
#include "wrapperfunc.h"

static struct sockaddr_in server;

int main()
{
        int menu = 0;
        int code = 0;
        int pcs = 0;

        ProductList *list = NULL;
        Product *elem;

        int talkingSocket = connection(server);

        sleep(2);

        printMenu();

        while (1) {
                printf("Enter \"0\" to display the menu again\n");
                printf("Enter the section number (UNSAFE): ");
                scanf("%d", &menu);

                switch (menu) {
                case 1:
                        requestDatabase(talkingSocket, &list);
                        printDatabase(list);
                        break;
                case 2:
                        printf("[Product selection for information]\n");
                        printf("Enter the product code (UNSAFE): ");
                        scanf("%d", &code);

                        requestDatabase(talkingSocket, &list);


                        //receiveProduct(talkingSocket, &elem);
                        //printOneInfo(elem);
                        break;
                case 3:
                        printf("[Product and quantity selection for order request]\n");
                        printf("Enter the product code (UNSAFE): ");
                        scanf("%d", &code);
                        printf("Enter the quantity (UNSAFE): ");
                        scanf("%d", &pcs);

                        // Тут нужна функция, которая отправит серверу структуру [код товара + количество]. А сервер обработает и пришлёт ответ.

                        // Код из клиента Максима на полный выкуп(?) первой карточки(?)
                        Product *card = malloc(sizeof(Product));
                        product__init(card);
                        card->id = list->data[0]->id;
                        card->quantity = list->data[0]->quantity;
                        orderCard(talkingSocket, card, &list);
                        free(card);

                        break;
                case 4:
                        printf("Closing a connection...\n");
                        sleep(2);
                        product_list__free_unpacked(list, NULL);
                        Shutdown(talkingSocket, SHUT_RDWR);
                        Close(talkingSocket);
                        return EXIT_SUCCESS;
                case 0:
                        printMenu();
                        break;
                default:
                        printf("Invalid value entered!\n");
                }
        }

        return 0;
}
