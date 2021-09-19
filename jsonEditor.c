#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
#include "json_visit.h"

#define JSON_FILE_NAME "product.json"
#define JSON_OBJECT_STR(obj, key) json_object_get_string(json_object_object_get(obj, key))

char filter[64] = "id";

enum action 
{
   VIEW = 1,
   CREATE = 2,
   SEARCH = 3,
   LOAD = 4,
   ADD_DATA = 5,
   EXIT = 6,
   ADD = 7
};

typedef struct PRODUCT
{
   int id; 
   int quantity;
   double price;
   char name[50];
   char description[100];
} PRODUCT;

int add_info(json_object * const, struct PRODUCT * const);
int print_json_object_arr(json_object *obj);
int print_json_object(json_object *obj);
int scan_json_object(json_object *obj);
static int doit(json_object *obj, int flags, json_object *parent, const char *key, size_t *index, void *data);
int write_json_object_in_struct();
int view_json_file();
int change_json_file(int);
int find_info_json_file();

int main(void) // Меню
{
   int exit = 0;
   
   while(EXIT != exit)
   {
      printf(" 1.View the json file\
             \n 2.Create the json file and add date \
             \n 3.Search in the json file\
             \n 4.Load the json file in struct\
             \n 5.Add date in the json file\
             \n 6.Exit\
             \nEnter: ");
      
      scanf("%d", &exit);
      
      switch(exit)
      {
         case VIEW:
            view_json_file();
            break;
         case CREATE:
            change_json_file(CREATE);
            break;
         case SEARCH:
            find_info_json_file();
            break;
         case LOAD:
            write_json_object_in_struct();
            break;
         case ADD_DATA:
            change_json_file(ADD_DATA);
            break;             
         case EXIT:
            break;                       
         default:
            printf("\nERROR\n");
            break;
      }
      printf("\n");
   }

   return 0;
}

int change_json_file(int CHOICE) // Функция позволяет добавлять информацию в непустой файл json и очищать данный файл.
{
   FILE *fp;

   char file_name[] = JSON_FILE_NAME;
   int exit = 0;
   int choice = CHOICE;

   switch(choice)
   {
      case CREATE:
         if(NULL == (fp = fopen(file_name, "w")))
         {
            printf("Не удалось открыть файл");
            getchar();
            return 0;
         }
         fputs("[]",fp);
         fclose(fp);
         break;
      case ADD_DATA:
         break;
      default:
         printf("\nERROR\n");
         return 0;
         break;
   }

   json_object *arr = json_object_from_file(JSON_FILE_NAME); 

   printf("\nChoise\n");

   while(EXIT != exit)
   {
      printf("\n6.Exit\n7.Add\n");
      scanf("%d", &exit);
      
      if(ADD == exit)
      {
         json_object *node = json_object_new_object();
         struct PRODUCT product = {0, 0, 0, "a", "a"};
         
         printf("id, quantity, price, name, description: ");
         scanf("%d %d %lf %s %s", 
               &product.id,
               &product.quantity, 
               &product.price, 
               product.name, 
               product.description);

         add_info(node, &product);
         json_object_array_add(arr, node);
      }
   }
   
   json_object_to_file(JSON_FILE_NAME, arr);
   json_object_put(arr);

   return 0;
}

int add_info( ///Функция помещает данные из структуры продукта в json формат.
            json_object *node, 
            struct PRODUCT *product)
{
   json_object_object_add(node, "id", json_object_new_int((int32_t)product->id));
   json_object_object_add(node, "quantity", json_object_new_int((int32_t)product->quantity));
   json_object_object_add(node, "price", json_object_new_double((double)product->price));
   json_object_object_add(node, "name", json_object_new_string((char*)product->name));
   json_object_object_add(node, "description", json_object_new_string((char*)product->description));
   
   return 0;
}

int print_json_object(json_object *obj) //Функция правильного отображения информации из json файла в консоли.
{
   printf("\n-------------------------------------------\n");
   
   json_object_object_foreach(obj, key, val)
   {
      if(strcmp(key, "id"))
      {
         switch (json_object_get_type(val))
            {
            case json_type_array:
               printf("\n%s  \n\n", key);
               print_json_object_arr(val);
               break;

            case json_type_object:
               printf("\n%s  \n\n", key);
               print_json_object(val);
               break;

            default:
               printf("%s: %s\n", key, json_object_get_string(val));
            }

      }
   }
   return 0;
}

int scan_json_object(json_object *obj)//Функция записывает значения полей из json файла в структру.
{
   json_object *node;
   int leght_arr = json_object_array_length(obj);
   
      for (int i = 0; i < leght_arr; i++)
      {
         struct PRODUCT products = {0, 0, 0, "a", "a"};
         node = json_object_array_get_idx(obj, i);

         json_object *id = json_object_object_get(node, "id");
         json_object *quantity = json_object_object_get(node, "quantity");
         json_object *price = json_object_object_get(node, "price");
         json_object *name = json_object_object_get(node, "name");
         json_object *description = json_object_object_get(node, "description");

         products.id = json_object_get_int64(id);
         products.quantity = json_object_get_int64(quantity);
         products.price = json_object_get_double(price);
         strcpy(products.name,json_object_get_string(name));
         strcpy(products.description, json_object_get_string(description));
         
         printf("\n%d, %d, %lf, %s, %s\n",products.id, products.quantity, products.price, products.name, products.description);
      } 
   
   return 0;
}


int print_json_object_arr(json_object *obj)//Функция вывода информации из json файла с массивом.
{
   int temp_n = json_object_array_length(obj);
   
   for (int i = 0; i < temp_n; i++)
      print_json_object(json_object_array_get_idx(obj, i));
   
   return 0;
}

int view_json_file()//Функция вывода информации из json файла.
{
   json_object *arr = json_object_from_file(JSON_FILE_NAME);
   int leght =  json_object_array_length(arr);
   
   for(int i = 0; i < leght; i++)
      print_json_object(json_object_array_get_idx(arr, i));
   json_object_put(arr);

   return 0;
}

static int doit(//Функция сравнивает строку из файла json с фильтрои и выводит совпадения в консоль.
                  json_object *obj, 
                  int flags, 
                  json_object *parent, 
                  const char *key, 
                  size_t *index, 
                  void *data)
{
   if (!parent || flags == JSON_C_VISIT_SECOND ||
       json_object_get_type(obj) == json_type_object ||
       json_object_get_type(obj) == json_type_array)
      
      return JSON_C_VISIT_RETURN_CONTINUE;

   if (strcmp(json_object_get_string(obj), filter) == 0)
   {
      printf("Found: %s %s %s\n", JSON_OBJECT_STR(parent, "name"),
             JSON_OBJECT_STR(parent, "description"), json_object_to_json_string(obj));
      
      return JSON_C_VISIT_RETURN_STOP;
   }
   
   return JSON_C_VISIT_RETURN_CONTINUE;
}

int find_info_json_file()//Функция поиска информации по фильтру из файла json.
{
   json_object *root = json_object_from_file(JSON_FILE_NAME);
   
   printf("Filter: ");
   scanf("%s", filter);
   printf("\nSearch for %s\n\n", filter);
   
   json_c_visit(root, 0, doit, NULL);

   json_object_put(root);

   return 0;
}

int write_json_object_in_struct()//Функция записи данных из файла json в структуру PRODUCT.
{
   json_object *arr = json_object_from_file(JSON_FILE_NAME); 
   
   scan_json_object(arr);
   json_object_put(arr);

   return 0;
}
