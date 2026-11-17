#ifndef CONTACTS_H
#define CONTACTS_H

#include <stddef.h>

/* Ограничения телефонной книги. Данные хранятся только в массивах. */
#define MAX_CONTACTS 100
#define MAX_NAME_LENGTH 64
#define MAX_WORK_LENGTH 128
#define MAX_VALUES 3
#define MAX_VALUE_LENGTH 256

typedef struct {
    char last_name[MAX_NAME_LENGTH];
    char first_name[MAX_NAME_LENGTH];
    char middle_name[MAX_NAME_LENGTH];
    char workplace[MAX_WORK_LENGTH];
    char position[MAX_WORK_LENGTH];

    char phones[MAX_VALUES][MAX_VALUE_LENGTH];
    size_t phone_count;

    char emails[MAX_VALUES][MAX_VALUE_LENGTH];
    size_t email_count;

    char social_links[MAX_VALUES][MAX_VALUE_LENGTH];
    size_t social_count;

    char messenger_profiles[MAX_VALUES][MAX_VALUE_LENGTH];
    size_t messenger_count;
} Contact;

typedef struct {
    Contact contacts[MAX_CONTACTS];
    size_t count;
} PhoneBook;

/* Настройки сортировки массива контактов. */
typedef enum { SORT_LAST_NAME = 1, SORT_FIRST_NAME = 2, SORT_WORKPLACE = 3 } SortField;
typedef enum { SORT_ASCENDING = 1, SORT_DESCENDING = 2 } SortOrder;
typedef enum { SORT_BUBBLE = 1, SORT_SELECTION = 2, SORT_INSERTION = 3 } SortAlgorithm;

/* Функции логики не выполняют ввод и вывод: только работают с данными. */
void phonebook_init(PhoneBook *book);
int phonebook_add(PhoneBook *book, const Contact *contact);
int phonebook_update(PhoneBook *book, size_t index, const Contact *contact);
int phonebook_delete(PhoneBook *book, size_t index);
const Contact *phonebook_get(const PhoneBook *book, size_t index);
int phonebook_sort(PhoneBook *book, SortField field, SortOrder order, SortAlgorithm algorithm);

#endif
