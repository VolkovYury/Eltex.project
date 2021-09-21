#ifndef JSON_EDITOR
#define JSON_EDITOR
#include <stdint.h>
#include "products.pb-c.h"
#include "json-cH/json.h"
#include "json-cH/json_visit.h"
#include "server_functions.h"


typedef struct PRODUCT
{
   int id; 
   int quantity;
   double price;
   char name[50];
   char description[100];
} PRODUCT;

int change_json_file(int CHOICE); // 2(Очистка файла с заполнением), 5(Добавление информации в файл)
int scan_json_object(json_object *obj);
int view_json_file();
int find_info_json_file();
int load_to_ProductList(interprocessdata *shared, const char *filename);
int save_to_file(interprocessdata *shared, const char *filename);


int add_info(json_object * const, struct PRODUCT * const);
int print_json_object_arr(json_object *obj);
int print_json_object(json_object *obj);
static int doit(json_object *obj, int flags, json_object *parent, const char *key, size_t *index, void *data);
int write_json_object_in_struct();

#endif