#ifndef CLIENT_FUNCTIONS
#define CLIENT_FUNCTIONS
#include <stdint.h>
#include "products.pb-c.h"

enum acton {
        PRINT_DATABASE = 1,
        PRINT_ONE_CARD = 2,
        ORDER_REQST = 3,
        CLOSE = 4,
        PRINT_MENU = 5
};

void orderCard(int fd, const Product *card, ProductList **list);
int requestDatabase(int fd, ProductList **list);
size_t findCard(const Product *card, const ProductList *database);
int connection(struct sockaddr_in servaddr);
void printMenu();
void printInfo(Product *elem);
void printDatabase(ProductList *list);
void printOneInfo(Product *elem);
int enterNumber(uint32_t min, uint32_t max);
void enterIP(char *res);
int maxValue(int a, int b, int c, int d, int e);
int maxOfTwo(int a, int b);

#endif