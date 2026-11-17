#include "contacts.h"

#include <ctype.h>
#include <string.h>

/* Строка из одних пробелов не считается заполненной. */
static int has_text(const char *text)
{
    while (*text != '\0') {
        if (!isspace((unsigned char)*text)) {
            return 1;
        }
        ++text;
    }
    return 0;
}

static int contact_is_valid(const Contact *contact)
{
    return contact != NULL
        && has_text(contact->last_name)
        && has_text(contact->first_name)
        && contact->phone_count <= MAX_VALUES
        && contact->email_count <= MAX_VALUES
        && contact->social_count <= MAX_VALUES
        && contact->messenger_count <= MAX_VALUES;
}

void phonebook_init(PhoneBook *book)
{
    if (book != NULL) {
        book->count = 0;
    }
}

int phonebook_add(PhoneBook *book, const Contact *contact)
{
    if (book == NULL || !contact_is_valid(contact) || book->count >= MAX_CONTACTS) {
        return 0;
    }

    book->contacts[book->count] = *contact;
    ++book->count;
    return 1;
}

int phonebook_update(PhoneBook *book, size_t index, const Contact *contact)
{
    if (book == NULL || index >= book->count || !contact_is_valid(contact)) {
        return 0;
    }

    book->contacts[index] = *contact;
    return 1;
}

int phonebook_delete(PhoneBook *book, size_t index)
{
    if (book == NULL || index >= book->count) {
        return 0;
    }

    /* После удаления сдвигаем оставшиеся элементы массива влево. */
    for (size_t i = index; i + 1 < book->count; ++i) {
        book->contacts[i] = book->contacts[i + 1];
    }
    --book->count;
    return 1;
}

const Contact *phonebook_get(const PhoneBook *book, size_t index)
{
    if (book == NULL || index >= book->count) {
        return NULL;
    }
    return &book->contacts[index];
}

/* Алгоритмы сортировки написаны вручную, без qsort и других готовых решений. */
static int compare_contacts(const Contact *a, const Contact *b,
                            SortField field, SortOrder order)
{
    int result;
    if (field == SORT_FIRST_NAME) {
        result = strcmp(a->first_name, b->first_name);
        if (result == 0) result = strcmp(a->last_name, b->last_name);
    } else if (field == SORT_WORKPLACE) {
        result = strcmp(a->workplace, b->workplace);
        if (result == 0) result = strcmp(a->last_name, b->last_name);
        if (result == 0) result = strcmp(a->first_name, b->first_name);
    } else {
        result = strcmp(a->last_name, b->last_name);
        if (result == 0) result = strcmp(a->first_name, b->first_name);
    }
    if (result == 0) result = strcmp(a->middle_name, b->middle_name);
    if (order == SORT_DESCENDING) {
        return result > 0 ? -1 : (result < 0 ? 1 : 0);
    }
    return result;
}

static void swap_contacts(Contact *a, Contact *b)
{
    Contact temp = *a;
    *a = *b;
    *b = temp;
}

int phonebook_sort(PhoneBook *book, SortField field, SortOrder order,
                   SortAlgorithm algorithm)
{
    if (book == NULL || field < SORT_LAST_NAME || field > SORT_WORKPLACE
        || order < SORT_ASCENDING || order > SORT_DESCENDING
        || algorithm < SORT_BUBBLE || algorithm > SORT_INSERTION) {
        return 0;
    }
    size_t count = book->count;
    if (algorithm == SORT_BUBBLE) {
        for (size_t end = count; end > 1; --end) {
            int swapped = 0;
            for (size_t i = 1; i < end; ++i) {
                if (compare_contacts(&book->contacts[i - 1], &book->contacts[i],
                                     field, order) > 0) {
                    swap_contacts(&book->contacts[i - 1], &book->contacts[i]);
                    swapped = 1;
                }
            }
            if (!swapped) break;
        }
    } else if (algorithm == SORT_SELECTION) {
        for (size_t i = 0; i < count; ++i) {
            size_t best = i;
            for (size_t j = i + 1; j < count; ++j) {
                if (compare_contacts(&book->contacts[j], &book->contacts[best],
                                     field, order) < 0) {
                    best = j;
                }
            }
            if (best != i) swap_contacts(&book->contacts[i], &book->contacts[best]);
        }
    } else { /* Сортировка вставками. */
        for (size_t i = 1; i < count; ++i) {
            Contact current = book->contacts[i];
            size_t j = i;
            while (j > 0 && compare_contacts(&book->contacts[j - 1], &current,
                                             field, order) > 0) {
                book->contacts[j] = book->contacts[j - 1];
                --j;
            }
            book->contacts[j] = current;
        }
    }
    return 1;
}
