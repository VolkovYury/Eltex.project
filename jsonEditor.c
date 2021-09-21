#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "jsonEditor.h"
#include "json-cH/json.h"
#include "json-cH/json_visit.h"

#define JSON_FILE_NAME "product.json"
#define JSON_OBJECT_STR(obj, key) json_object_get_string(json_object_object_get(obj, key))

char filter[64] = "";

enum action {
        VIEW = 1,
        CREATE = 2,
        SEARCH = 3,
        LOAD = 4,
        ADD_DATA = 5,
        EXIT = 6,
        ADD = 7
};

int change_json_file(int CHOICE) // Функция позволяет добавлять информацию в непустой файл json и очищать данный файл.
{
        FILE *fp;

        char file_name[] = JSON_FILE_NAME;
        int choice = CHOICE;
        int exit = 0;
        int index = 1;
        int length_arr = 0;
        int i = 0;
        json_object *arr;
        switch (choice) {
        case CREATE:
                if (NULL == (fp = fopen(file_name, "w"))) {
                        printf("Could not open file");
                        getchar();
                        return 0;
                }
                fputs("[]", fp);
                fclose(fp);
                arr = json_object_from_file(JSON_FILE_NAME);
                break;
        case ADD_DATA:
                arr = json_object_from_file(JSON_FILE_NAME);
                length_arr = json_object_array_length(arr);
                i = length_arr;
                break;
        default:
                printf("\nERROR\n");
                return 0;
                break;
        }


        printf("\nChoice\n");

        while (EXIT != exit) {
                printf("\n6.Exit\n7.Add\n");
                scanf("%d", &exit);

                if (ADD == exit) {
                        i++;
                        json_object *node = json_object_new_object();
                        struct PRODUCT product = {0, 0, 0, "a", "a"};

                        printf("quantity, price, name, description: ");

                        product.id = i;
                        scanf(" %d %lf %s %s",
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
        json_object_object_add(node, "id", json_object_new_int((int32_t) product->id));
        json_object_object_add(node, "quantity", json_object_new_int((int32_t) product->quantity));
        json_object_object_add(node, "price", json_object_new_double((double) product->price));
        json_object_object_add(node, "name", json_object_new_string((char *) product->name));
        json_object_object_add(node, "description", json_object_new_string((char *) product->description));

        return 0;
}

int print_json_object(json_object *obj) //Функция правильного отображения информации из json файла в консоли.
{
        printf("\n-------------------------------------------\n");

        json_object_object_foreach(obj, key, val) {
                if (strcmp(key, "id")) {
                        switch (json_object_get_type(val)) {
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

int scan_json_object(json_object *obj)//Функция записывает значения полей из json файла в структуру.
{
        json_object *node;
        int length_arr = json_object_array_length(obj);

        for (int i = 0; i < length_arr; i++) {
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
                strcpy(products.name, json_object_get_string(name));
                strcpy(products.description, json_object_get_string(description));

                printf("\n%d, %d, %lf, %s, %s\n", products.id, products.quantity, products.price, products.name,
                       products.description);
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
        int length = json_object_array_length(arr);

        for (int i = 0; i < length; i++)
                print_json_object(json_object_array_get_idx(arr, i));
        json_object_put(arr);

        return 0;
}

static int doit(//Функция сравнивает строку из файла json с фильтром и выводит совпадения в консоль.
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

        if (strcmp(json_object_get_string(obj), filter) == 0) {
                printf("Found:\n");
                printf(" ID: %s\n", JSON_OBJECT_STR(parent, "id"));
                printf(" Quantity: %s\n", JSON_OBJECT_STR(parent, "quantity"));
                printf(" Price: %s\n", JSON_OBJECT_STR(parent, "price"));
                printf(" Name: %s\n", JSON_OBJECT_STR(parent, "name"));
                printf(" Description: %s\n", JSON_OBJECT_STR(parent, "description"));
                printf("----------------------------------\n");
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

int load_to_ProductList(interprocessdata *shared, const char *filename)
{
	json_object *node;
	json_object *arr = json_object_from_file(filename);
	size_t length = json_object_array_length(arr);
	int ret;

	pthread_mutex_lock(&(shared->data_mutex));
	if (-1 == makeNewProductList(&(shared->database), length)) {
		printf("Can't make new product list\n");
		return -1;
	}

	for (size_t i = 0; i < length; ++i) {
		node = json_object_array_get_idx(arr, i);

		//make one product
		ret = makeNewProduct(&(shared->database->data[i]),
		(uint32_t)json_object_get_uint64(json_object_object_get(node, "id")),
		json_object_get_string(json_object_object_get(node, "name")),
		json_object_get_string(json_object_object_get(node, "description")),
		json_object_get_double(json_object_object_get(node, "price")),
		(uint32_t)json_object_get_uint64(json_object_object_get(node, "quantity")));
		
		if (-1 == ret) {
			printf("Can't make new product\n");
			freeProductList(shared->database);
			pthread_mutex_unlock(&(shared->data_mutex));
			return -1;
		}
	}
	pthread_mutex_unlock(&(shared->data_mutex));
	return 0;
}

int save_to_file(interprocessdata *shared, const char *filename)
{
	ProductList *list = shared->database;
	json_object *arr = json_object_new_array();
	for (size_t i = 0; i < list->n_data; ++i) {
		pthread_mutex_lock(&(shared->data_mutex));
	   json_object *node = json_object_new_object();
		if (json_object_object_add(node, "id",
      json_object_new_uint64((uint64_t)(list->data[i]->id))) < 0) {
			printf("Can't add field to node\n");
			pthread_mutex_unlock(&(shared->data_mutex));
			return -1;
		}

		if (json_object_object_add(node, "quantity",
		json_object_new_uint64((uint64_t)(list->data[i]->quantity))) < 0) {
			printf("Can't add field to node\n");
			pthread_mutex_unlock(&(shared->data_mutex));
			return -1;
		}

		if (json_object_object_add(node, "price",
		json_object_new_double((double)(list->data[i]->price))) < 0) {
			printf("Can't add field to node\n");
			pthread_mutex_unlock(&(shared->data_mutex));
			return -1;
		}

		if (json_object_object_add(node, "name",
		json_object_new_string(((char*)list->data[i]->name))) < 0) {
			printf("Can't add field to node\n");
			pthread_mutex_unlock(&(shared->data_mutex));
			return -1;
		}

		if (json_object_object_add(node, "description",
		json_object_new_string(((char*)list->data[i]->description))) < 0) {
			printf("Can't add field to node\n");
			pthread_mutex_unlock(&(shared->data_mutex));
			return -1;
		}
		pthread_mutex_unlock(&(shared->data_mutex));

		json_object_array_add(arr, node);
	}
	if (-1 == json_object_to_file(filename, arr)) {
      printf("Can't save to file\n");
   }
	json_object_put(arr);
	return 0;
}