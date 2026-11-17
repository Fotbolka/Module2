#ifndef CONTACTS_H
#define CONTACTS_H

#include <stddef.h>
#include <stdint.h>

#define MAX_NAME_LENGTH 64
#define MAX_WORK_LENGTH 128
#define MAX_VALUES 3
#define MAX_VALUE_LENGTH 256

/* После каждых 8 успешных изменений дерево автоматически балансируется. */
#define PHONEBOOK_BALANCE_INTERVAL 8u

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

typedef struct TreeNode TreeNode;

typedef struct {
    TreeNode *root;
    size_t count;
    uint64_t next_id;

    /* Счётчик успешных изменений после последней балансировки. */
    unsigned modifications;

    /* Информация о последней автоматической или ручной балансировке. */
    unsigned balance_count;
    size_t last_balance_before;
    size_t last_balance_after;
} PhoneBook;

void phonebook_init(PhoneBook *book);
void phonebook_free(PhoneBook *book);
size_t phonebook_count(const PhoneBook *book);

/* Добавление, поиск, редактирование и удаление по уникальному ID. */
int phonebook_add(PhoneBook *book, const Contact *contact);
const Contact *phonebook_find_id(const PhoneBook *book, uint64_t id);
int phonebook_update_id(PhoneBook *book, uint64_t id, const Contact *contact);
int phonebook_delete_id(PhoneBook *book, uint64_t id);

/* Доступ к контактам по индексу симметричного (отсортированного) обхода. */
const Contact *phonebook_get(const PhoneBook *book, size_t index);
uint64_t phonebook_get_id(const PhoneBook *book, size_t index);

/* Информация и демонстрация дерева. */
size_t phonebook_height(const PhoneBook *book);
int phonebook_is_balanced(const PhoneBook *book);
void phonebook_print_tree(const PhoneBook *book);
void phonebook_balance(PhoneBook *book);

unsigned phonebook_modifications(const PhoneBook *book);
unsigned phonebook_balance_count(const PhoneBook *book);
size_t phonebook_last_balance_before(const PhoneBook *book);
size_t phonebook_last_balance_after(const PhoneBook *book);

#endif
