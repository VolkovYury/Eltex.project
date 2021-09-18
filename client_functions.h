#ifndef CLIENT_FUNCTIONS
#define CLIENT_FUNCTIONS
#include <stdint.h>
#include "products.pb-c.h"

int requestDatabase(int fd, ProductList **list);

#endif
