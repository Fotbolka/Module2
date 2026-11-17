#include "contacts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct TreeNode {
    Contact contact;
    uint64_t id;
    struct TreeNode *left;
    struct TreeNode *right;
};

static int compare_text(const char *a, const char *b)
{
    int result = strcmp(a, b);
    return (result > 0) - (result < 0);
}

/* Ключ дерева: фамилия, имя, отчество, затем уникальный ID. */
static int compare_key(const Contact *a, uint64_t id_a,
                       const Contact *b, uint64_t id_b)
{
    int result = compare_text(a->last_name, b->last_name);
    if (result != 0) return result;

    result = compare_text(a->first_name, b->first_name);
    if (result != 0) return result;

    result = compare_text(a->middle_name, b->middle_name);
    if (result != 0) return result;

    return (id_a > id_b) - (id_a < id_b);
}

static TreeNode *create_node(const Contact *contact, uint64_t id)
{
    TreeNode *node = malloc(sizeof(*node));
    if (node == NULL) {
        return NULL;
    }

    node->contact = *contact;
    node->id = id;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static void insert_existing_node(TreeNode **root, TreeNode *node)
{
    if (*root == NULL) {
        *root = node;
        return;
    }

    if (compare_key(&node->contact, node->id,
                    &(*root)->contact, (*root)->id) < 0) {
        insert_existing_node(&(*root)->left, node);
    } else {
        insert_existing_node(&(*root)->right, node);
    }
}

static void free_nodes(TreeNode *node)
{
    if (node == NULL) {
        return;
    }

    free_nodes(node->left);
    free_nodes(node->right);
    free(node);
}

static size_t height_node(const TreeNode *node)
{
    if (node == NULL) {
        return 0;
    }

    size_t left_height = height_node(node->left);
    size_t right_height = height_node(node->right);
    return 1 + (left_height > right_height ? left_height : right_height);
}

static TreeNode *find_id_node(TreeNode *node, uint64_t id)
{
    if (node == NULL) {
        return NULL;
    }

    if (node->id == id) {
        return node;
    }

    TreeNode *found = find_id_node(node->left, id);
    if (found != NULL) {
        return found;
    }

    return find_id_node(node->right, id);
}

static const TreeNode *find_id_node_const(const TreeNode *node, uint64_t id)
{
    if (node == NULL) {
        return NULL;
    }

    if (node->id == id) {
        return node;
    }

    const TreeNode *found = find_id_node_const(node->left, id);
    if (found != NULL) {
        return found;
    }

    return find_id_node_const(node->right, id);
}

static TreeNode *minimum_node(TreeNode *node)
{
    while (node != NULL && node->left != NULL) {
        node = node->left;
    }
    return node;
}

static TreeNode *delete_key(TreeNode *root, const Contact *contact, uint64_t id,
                            int *deleted)
{
    if (root == NULL) {
        return NULL;
    }

    int comparison = compare_key(contact, id, &root->contact, root->id);

    if (comparison < 0) {
        root->left = delete_key(root->left, contact, id, deleted);
    } else if (comparison > 0) {
        root->right = delete_key(root->right, contact, id, deleted);
    } else {
        *deleted = 1;

        if (root->left == NULL) {
            TreeNode *right = root->right;
            free(root);
            return right;
        }

        if (root->right == NULL) {
            TreeNode *left = root->left;
            free(root);
            return left;
        }

        /* У узла два потомка: переносим данные минимального узла справа. */
        TreeNode *successor = minimum_node(root->right);
        Contact successor_contact = successor->contact;
        uint64_t successor_id = successor->id;

        root->contact = successor_contact;
        root->id = successor_id;

        int successor_deleted = 0;
        root->right = delete_key(root->right, &successor_contact,
                                 successor_id, &successor_deleted);
    }

    return root;
}

static void collect_inorder(TreeNode *node, TreeNode **array, size_t *position)
{
    if (node == NULL) {
        return;
    }

    collect_inorder(node->left, array, position);
    array[(*position)++] = node;
    collect_inorder(node->right, array, position);
}

static TreeNode *build_balanced(TreeNode **array, long left, long right)
{
    if (left > right) {
        return NULL;
    }

    long middle = left + (right - left) / 2;
    TreeNode *root = array[middle];

    root->left = build_balanced(array, left, middle - 1);
    root->right = build_balanced(array, middle + 1, right);

    return root;
}

static void rebalance_without_statistics(PhoneBook *book)
{
    if (book->root == NULL || book->count == 0) {
        return;
    }

    TreeNode **array = malloc(book->count * sizeof(*array));
    if (array == NULL) {
        return;
    }

    size_t position = 0;
    collect_inorder(book->root, array, &position);

    if (position == book->count) {
        book->root = build_balanced(array, 0, (long)position - 1);
    }

    free(array);
}

static void register_modification(PhoneBook *book)
{
    book->modifications++;

    if (book->modifications >= PHONEBOOK_BALANCE_INTERVAL) {
        size_t before = height_node(book->root);

        rebalance_without_statistics(book);

        size_t after = height_node(book->root);
        book->last_balance_before = before;
        book->last_balance_after = after;
        book->balance_count++;
        book->modifications = 0;
    }
}

static size_t count_nodes(const TreeNode *node)
{
    if (node == NULL) {
        return 0;
    }
    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

static const TreeNode *node_at_inorder(const TreeNode *node, size_t index)
{
    if (node == NULL) {
        return NULL;
    }

    size_t left_count = count_nodes(node->left);

    if (index < left_count) {
        return node_at_inorder(node->left, index);
    }
    if (index == left_count) {
        return node;
    }
    return node_at_inorder(node->right, index - left_count - 1);
}

static int check_balance(const TreeNode *node, size_t *height)
{
    if (node == NULL) {
        *height = 0;
        return 1;
    }

    size_t left_height = 0;
    size_t right_height = 0;

    int left_ok = check_balance(node->left, &left_height);
    int right_ok = check_balance(node->right, &right_height);

    size_t difference = left_height > right_height
                      ? left_height - right_height
                      : right_height - left_height;

    *height = 1 + (left_height > right_height ? left_height : right_height);
    return left_ok && right_ok && difference <= 1;
}

static void print_tree_branch(const TreeNode *node, size_t depth,
                              const char *branch_name)
{
    for (size_t i = 0; i < depth; i++) {
        printf("    ");
    }

    if (node == NULL) {
        printf("%s: (пусто)\n", branch_name);
        return;
    }

    printf("%s: [ID=%llu] %s %s",
           branch_name,
           (unsigned long long)node->id,
           node->contact.last_name,
           node->contact.first_name);

    if (node->contact.middle_name[0] != '\0') {
        printf(" %s", node->contact.middle_name);
    }
    putchar('\n');

    print_tree_branch(node->left, depth + 1, "├── Левое");
    print_tree_branch(node->right, depth + 1, "└── Правое");
}

void phonebook_init(PhoneBook *book)
{
    if (book == NULL) {
        return;
    }

    book->root = NULL;
    book->count = 0;
    book->next_id = 1;
    book->modifications = 0;
    book->balance_count = 0;
    book->last_balance_before = 0;
    book->last_balance_after = 0;
}

void phonebook_free(PhoneBook *book)
{
    if (book == NULL) {
        return;
    }

    free_nodes(book->root);
    phonebook_init(book);
}

size_t phonebook_count(const PhoneBook *book)
{
    return book == NULL ? 0 : book->count;
}

int phonebook_add(PhoneBook *book, const Contact *contact)
{
    if (book == NULL || contact == NULL ||
        contact->last_name[0] == '\0' || contact->first_name[0] == '\0') {
        return 0;
    }

    TreeNode *node = create_node(contact, book->next_id);
    if (node == NULL) {
        return 0;
    }

    insert_existing_node(&book->root, node);
    book->next_id++;
    book->count++;
    register_modification(book);
    return 1;
}

const Contact *phonebook_find_id(const PhoneBook *book, uint64_t id)
{
    if (book == NULL) {
        return NULL;
    }

    const TreeNode *node = find_id_node_const(book->root, id);
    return node == NULL ? NULL : &node->contact;
}

int phonebook_update_id(PhoneBook *book, uint64_t id, const Contact *contact)
{
    if (book == NULL || contact == NULL ||
        contact->last_name[0] == '\0' || contact->first_name[0] == '\0') {
        return 0;
    }

    TreeNode *old_node = find_id_node(book->root, id);
    if (old_node == NULL) {
        return 0;
    }

    TreeNode *replacement = create_node(contact, id);
    if (replacement == NULL) {
        return 0;
    }

    Contact old_contact = old_node->contact;
    int deleted = 0;
    book->root = delete_key(book->root, &old_contact, id, &deleted);

    if (!deleted) {
        free(replacement);
        return 0;
    }

    insert_existing_node(&book->root, replacement);
    register_modification(book);
    return 1;
}

int phonebook_delete_id(PhoneBook *book, uint64_t id)
{
    if (book == NULL) {
        return 0;
    }

    TreeNode *node = find_id_node(book->root, id);
    if (node == NULL) {
        return 0;
    }

    Contact contact = node->contact;
    int deleted = 0;
    book->root = delete_key(book->root, &contact, id, &deleted);

    if (deleted) {
        book->count--;
        register_modification(book);
    }

    return deleted;
}

const Contact *phonebook_get(const PhoneBook *book, size_t index)
{
    if (book == NULL || index >= book->count) {
        return NULL;
    }

    const TreeNode *node = node_at_inorder(book->root, index);
    return node == NULL ? NULL : &node->contact;
}

uint64_t phonebook_get_id(const PhoneBook *book, size_t index)
{
    if (book == NULL || index >= book->count) {
        return 0;
    }

    const TreeNode *node = node_at_inorder(book->root, index);
    return node == NULL ? 0 : node->id;
}

size_t phonebook_height(const PhoneBook *book)
{
    return book == NULL ? 0 : height_node(book->root);
}

int phonebook_is_balanced(const PhoneBook *book)
{
    if (book == NULL) {
        return 1;
    }

    size_t height = 0;
    return check_balance(book->root, &height);
}

void phonebook_print_tree(const PhoneBook *book)
{
    if (book == NULL || book->root == NULL) {
        puts("Дерево пустое.");
        return;
    }

    const TreeNode *root = book->root;

    printf("ROOT [ID=%llu] %s %s",
           (unsigned long long)root->id,
           root->contact.last_name,
           root->contact.first_name);

    if (root->contact.middle_name[0] != '\0') {
        printf(" %s", root->contact.middle_name);
    }
    putchar('\n');

    print_tree_branch(root->left, 1, "├── Левое поддерево");
    print_tree_branch(root->right, 1, "└── Правое поддерево");
}

void phonebook_balance(PhoneBook *book)
{
    if (book == NULL) {
        return;
    }

    size_t before = height_node(book->root);
    rebalance_without_statistics(book);
    size_t after = height_node(book->root);

    book->last_balance_before = before;
    book->last_balance_after = after;
    book->balance_count++;
    book->modifications = 0;
}

unsigned phonebook_modifications(const PhoneBook *book)
{
    return book == NULL ? 0 : book->modifications;
}

unsigned phonebook_balance_count(const PhoneBook *book)
{
    return book == NULL ? 0 : book->balance_count;
}

size_t phonebook_last_balance_before(const PhoneBook *book)
{
    return book == NULL ? 0 : book->last_balance_before;
}

size_t phonebook_last_balance_after(const PhoneBook *book)
{
    return book == NULL ? 0 : book->last_balance_after;
}
