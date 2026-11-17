#include "contacts.h"

#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    Contact contact;
    uint64_t id;
} DisplayEntry;

typedef enum {
    SORT_BUBBLE = 1,
    SORT_SELECTION,
    SORT_INSERTION,
    SORT_QUICK
} SortAlgorithm;

typedef enum {
    FIELD_LAST_NAME = 1,
    FIELD_FIRST_NAME,
    FIELD_WORKPLACE,
    FIELD_POSITION
} SortField;

static SortAlgorithm current_sort = SORT_BUBBLE;
static SortField current_field = FIELD_LAST_NAME;

static int read_line(const char *prompt, char *buffer, size_t size)
{
    if (prompt != NULL) {
        fputs(prompt, stdout);
        fflush(stdout);
    }

    if (fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }

    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[--length] = '\0';
    } else if (!feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* Удаляем остаток слишком длинной строки. */
        }
        puts("Введённая строка слишком длинная. Повторите ввод.");
        buffer[0] = '\0';
        return -1;
    }

    if (length > 0 && buffer[length - 1] == '\r') {
        buffer[length - 1] = '\0';
    }

    return 1;
}

static int read_required_text(const char *prompt, char *buffer, size_t size)
{
    for (;;) {
        int result = read_line(prompt, buffer, size);
        if (result == 0) return 0;
        if (result < 0) continue;

        if (buffer[0] != '\0') {
            return 1;
        }
        puts("Поле обязательно для заполнения.");
    }
}

static int read_integer(const char *prompt, long minimum, long maximum,
                        long *value, int empty_is_zero)
{
    char buffer[128];

    for (;;) {
        int result = read_line(prompt, buffer, sizeof(buffer));
        if (result == 0) return 0;
        if (result < 0) continue;

        if (empty_is_zero && buffer[0] == '\0') {
            *value = 0;
            return 1;
        }

        char *end = NULL;
        long parsed = strtol(buffer, &end, 10);

        while (end != NULL && isspace((unsigned char)*end)) {
            end++;
        }

        if (end == buffer || end == NULL || *end != '\0' ||
            parsed < minimum || parsed > maximum) {
            printf("Введите целое число от %ld до %ld.\n", minimum, maximum);
            continue;
        }

        *value = parsed;
        return 1;
    }
}

static int read_optional_text(const char *prompt, char *buffer, size_t size)
{
    int result = read_line(prompt, buffer, size);
    return result == 1;
}

static int read_contact(Contact *contact)
{
    memset(contact, 0, sizeof(*contact));

    puts("\n--- Данные контакта ---");

    if (!read_required_text("Фамилия: ", contact->last_name,
                            sizeof(contact->last_name))) return 0;
    if (!read_required_text("Имя: ", contact->first_name,
                            sizeof(contact->first_name))) return 0;
    if (!read_optional_text("Отчество (можно пропустить): ",
                            contact->middle_name,
                            sizeof(contact->middle_name))) return 0;
    if (!read_optional_text("Место работы (можно пропустить): ",
                            contact->workplace,
                            sizeof(contact->workplace))) return 0;
    if (!read_optional_text("Должность (можно пропустить): ",
                            contact->position,
                            sizeof(contact->position))) return 0;

    long count = 0;

    if (!read_integer("Количество телефонов [0..3, Enter = 0]: ",
                      0, MAX_VALUES, &count, 1)) return 0;
    contact->phone_count = (size_t)count;
    for (size_t i = 0; i < contact->phone_count; i++) {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "Телефон %zu: ", i + 1);
        if (!read_required_text(prompt, contact->phones[i],
                                sizeof(contact->phones[i]))) return 0;
    }

    if (!read_integer("Количество e-mail [0..3, Enter = 0]: ",
                      0, MAX_VALUES, &count, 1)) return 0;
    contact->email_count = (size_t)count;
    for (size_t i = 0; i < contact->email_count; i++) {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "E-mail %zu: ", i + 1);
        if (!read_required_text(prompt, contact->emails[i],
                                sizeof(contact->emails[i]))) return 0;
    }

    if (!read_integer("Количество ссылок на соцсети [0..3, Enter = 0]: ",
                      0, MAX_VALUES, &count, 1)) return 0;
    contact->social_count = (size_t)count;
    for (size_t i = 0; i < contact->social_count; i++) {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "Ссылка %zu: ", i + 1);
        if (!read_required_text(prompt, contact->social_links[i],
                                sizeof(contact->social_links[i]))) return 0;
    }

    if (!read_integer("Количество профилей мессенджеров [0..3, Enter = 0]: ",
                      0, MAX_VALUES, &count, 1)) return 0;
    contact->messenger_count = (size_t)count;
    for (size_t i = 0; i < contact->messenger_count; i++) {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "Профиль %zu: ", i + 1);
        if (!read_required_text(prompt, contact->messenger_profiles[i],
                                sizeof(contact->messenger_profiles[i]))) return 0;
    }

    return 1;
}

static void print_contact(const Contact *contact, uint64_t id)
{
    printf("\nID: %llu\n", (unsigned long long)id);

    printf("Фамилия: %s\n", contact->last_name);
    printf("Имя: %s\n", contact->first_name);

    if (contact->middle_name[0] != '\0') {
        printf("Отчество: %s\n", contact->middle_name);
    }

    if (contact->workplace[0] != '\0') {
        printf("Место работы: %s\n", contact->workplace);
    }

    if (contact->position[0] != '\0') {
        printf("Должность: %s\n", contact->position);
    }

    if (contact->phone_count > 0) {
        puts("Телефоны:");

        for (size_t i = 0; i < contact->phone_count; i++) {
            printf("  %zu) %s\n", i + 1, contact->phones[i]);
        }
    }

    if (contact->email_count > 0) {
        puts("Электронная почта:");

        for (size_t i = 0; i < contact->email_count; i++) {
            printf("  %zu) %s\n", i + 1, contact->emails[i]);
        }
    }

    if (contact->social_count > 0) {
        puts("Социальные сети:");

        for (size_t i = 0; i < contact->social_count; i++) {
            printf("  %zu) %s\n", i + 1, contact->social_links[i]);
        }
    }

    if (contact->messenger_count > 0) {
        puts("Мессенджеры:");

        for (size_t i = 0; i < contact->messenger_count; i++) {
            printf("  %zu) %s\n", i + 1, contact->messenger_profiles[i]);
        }
    }
}

static const char *field_name(SortField field)
{
    switch (field) {
        case FIELD_LAST_NAME: return "фамилии";
        case FIELD_FIRST_NAME: return "имени";
        case FIELD_WORKPLACE: return "месту работы";
        case FIELD_POSITION: return "должности";
        default: return "фамилии";
    }
}

static const char *algorithm_name(SortAlgorithm algorithm)
{
    switch (algorithm) {
        case SORT_BUBBLE: return "пузырьковая сортировка";
        case SORT_SELECTION: return "сортировка выбором";
        case SORT_INSERTION: return "сортировка вставками";
        case SORT_QUICK: return "быстрая сортировка";
        default: return "пузырьковая сортировка";
    }
}

static const char *selected_field(const Contact *contact, SortField field)
{
    switch (field) {
        case FIELD_FIRST_NAME: return contact->first_name;
        case FIELD_WORKPLACE: return contact->workplace;
        case FIELD_POSITION: return contact->position;
        case FIELD_LAST_NAME:
        default: return contact->last_name;
    }
}

static int compare_entries(const DisplayEntry *a, const DisplayEntry *b,
                           SortField field)
{
    int result = strcmp(selected_field(&a->contact, field),
                        selected_field(&b->contact, field));
    if (result != 0) return result;

    result = strcmp(a->contact.last_name, b->contact.last_name);
    if (result != 0) return result;

    result = strcmp(a->contact.first_name, b->contact.first_name);
    if (result != 0) return result;

    result = strcmp(a->contact.middle_name, b->contact.middle_name);
    if (result != 0) return result;

    return (a->id > b->id) - (a->id < b->id);
}

static void swap_entries(DisplayEntry *a, DisplayEntry *b)
{
    DisplayEntry temporary = *a;
    *a = *b;
    *b = temporary;
}

static void bubble_sort(DisplayEntry *array, size_t count, SortField field)
{
    for (size_t end = count; end > 1; end--) {
        int changed = 0;
        for (size_t i = 1; i < end; i++) {
            if (compare_entries(&array[i - 1], &array[i], field) > 0) {
                swap_entries(&array[i - 1], &array[i]);
                changed = 1;
            }
        }
        if (!changed) break;
    }
}

static void selection_sort(DisplayEntry *array, size_t count, SortField field)
{
    for (size_t i = 0; i < count; i++) {
        size_t minimum = i;
        for (size_t j = i + 1; j < count; j++) {
            if (compare_entries(&array[j], &array[minimum], field) < 0) {
                minimum = j;
            }
        }
        if (minimum != i) {
            swap_entries(&array[i], &array[minimum]);
        }
    }
}

static void insertion_sort(DisplayEntry *array, size_t count, SortField field)
{
    for (size_t i = 1; i < count; i++) {
        DisplayEntry current = array[i];
        size_t j = i;

        while (j > 0 && compare_entries(&array[j - 1], &current, field) > 0) {
            array[j] = array[j - 1];
            j--;
        }
        array[j] = current;
    }
}

static void quick_sort_recursive(DisplayEntry *array, long left, long right,
                                 SortField field)
{
    long i = left;
    long j = right;
    DisplayEntry pivot = array[left + (right - left) / 2];

    while (i <= j) {
        while (compare_entries(&array[i], &pivot, field) < 0) i++;
        while (compare_entries(&array[j], &pivot, field) > 0) j--;

        if (i <= j) {
            swap_entries(&array[i], &array[j]);
            i++;
            j--;
        }
    }

    if (left < j) quick_sort_recursive(array, left, j, field);
    if (i < right) quick_sort_recursive(array, i, right, field);
}

static void sort_entries(DisplayEntry *array, size_t count,
                         SortAlgorithm algorithm, SortField field)
{
    switch (algorithm) {
        case SORT_SELECTION:
            selection_sort(array, count, field);
            break;
        case SORT_INSERTION:
            insertion_sort(array, count, field);
            break;
        case SORT_QUICK:
            if (count > 1) quick_sort_recursive(array, 0, (long)count - 1, field);
            break;
        case SORT_BUBBLE:
        default:
            bubble_sort(array, count, field);
            break;
    }
}

static void show_all_contacts(const PhoneBook *book)
{
    size_t count = phonebook_count(book);
    if (count == 0) {
        puts("Телефонная книга пуста.");
        return;
    }

    DisplayEntry *array = malloc(count * sizeof(*array));
    if (array == NULL) {
        puts("Ошибка: недостаточно памяти для сортировки.");
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const Contact *contact = phonebook_get(book, i);
        array[i].contact = *contact;
        array[i].id = phonebook_get_id(book, i);
    }

    sort_entries(array, count, current_sort, current_field);

    printf("\nКонтакты: %zu. Сортировка по %s, алгоритм: %s.\n",
           count, field_name(current_field), algorithm_name(current_sort));

    for (size_t i = 0; i < count; i++) {
        printf("\n========== КОНТАКТ %zu ==========\n", i + 1);
        print_contact(&array[i].contact, array[i].id);
    }

    free(array);
}

static void report_balance_if_happened(const PhoneBook *book,
                                      unsigned previous_balance_count)
{
    if (phonebook_balance_count(book) != previous_balance_count) {
        printf("\n*** Выполнена ПЕРИОДИЧЕСКАЯ БАЛАНСИРОВКА ***\n");
        printf("Высота до балансировки: %zu\n",
               phonebook_last_balance_before(book));
        printf("Высота после балансировки: %zu\n",
               phonebook_last_balance_after(book));
        printf("Балансировок всего: %u\n", phonebook_balance_count(book));
    }
}

static void add_contact(PhoneBook *book)
{
    Contact contact;
    if (!read_contact(&contact)) return;

    unsigned previous_balance_count = phonebook_balance_count(book);

    if (phonebook_add(book, &contact)) {
        printf("Контакт добавлен. Всего контактов: %zu.\n",
               phonebook_count(book));
        report_balance_if_happened(book, previous_balance_count);
    } else {
        puts("Не удалось добавить контакт.");
    }
}

static void edit_contact(PhoneBook *book)
{
    long raw_id;
    if (!read_integer("Введите ID контакта для редактирования: ",
                      1, 2147483647L, &raw_id, 0)) return;

    uint64_t id = (uint64_t)raw_id;
    if (phonebook_find_id(book, id) == NULL) {
        puts("Контакт с таким ID не найден.");
        return;
    }

    puts("Введите новые данные контакта. Старые данные будут заменены.");
    Contact contact;
    if (!read_contact(&contact)) return;

    unsigned previous_balance_count = phonebook_balance_count(book);

    if (phonebook_update_id(book, id, &contact)) {
        puts("Контакт успешно отредактирован.");
        report_balance_if_happened(book, previous_balance_count);
    } else {
        puts("Не удалось отредактировать контакт.");
    }
}

static void delete_contact(PhoneBook *book)
{
    long raw_id;
    if (!read_integer("Введите ID контакта для удаления: ",
                      1, 2147483647L, &raw_id, 0)) return;

    uint64_t id = (uint64_t)raw_id;
    const Contact *contact = phonebook_find_id(book, id);

    if (contact == NULL) {
        puts("Контакт с таким ID не найден.");
        return;
    }

    printf("Удалить контакт %s %s (ID=%llu)?\n",
           contact->last_name, contact->first_name,
           (unsigned long long)id);

    char answer[16];
    if (!read_line("Подтвердить (д/н): ", answer, sizeof(answer))) return;
    if (strcmp(answer, "д") != 0 && strcmp(answer, "Д") != 0 &&
        strcmp(answer, "да") != 0 && strcmp(answer, "Да") != 0 &&
        strcmp(answer, "y") != 0 && strcmp(answer, "Y") != 0 &&
        strcmp(answer, "1") != 0) {
        puts("Удаление отменено.");
        return;
    }

    unsigned previous_balance_count = phonebook_balance_count(book);

    if (phonebook_delete_id(book, id)) {
        puts("Контакт удалён.");
        report_balance_if_happened(book, previous_balance_count);
    } else {
        puts("Не удалось удалить контакт.");
    }
}

static void configure_sorting(void)
{
    long choice;

    puts("\n--- Поле сортировки ---");
    puts("1 — Фамилия");
    puts("2 — Имя");
    puts("3 — Место работы");
    puts("4 — Должность");

    if (!read_integer("Поле: ", 1, 4, &choice, 0)) return;
    current_field = (SortField)choice;

    puts("\n--- Алгоритм сортировки ---");
    puts("1 — Пузырьковая сортировка");
    puts("2 — Сортировка выбором");
    puts("3 — Сортировка вставками");
    puts("4 — Быстрая сортировка");

    if (!read_integer("Алгоритм: ", 1, 4, &choice, 0)) return;
    current_sort = (SortAlgorithm)choice;

    printf("Настройки сохранены: сортировка по %s, алгоритм — %s.\n",
           field_name(current_field), algorithm_name(current_sort));
}

static void show_tree_statistics(const PhoneBook *book)
{
    printf("\nКоличество контактов: %zu\n", phonebook_count(book));
    printf("Высота дерева: %zu\n", phonebook_height(book));
    printf("Сбалансировано по высоте: %s\n",
           phonebook_is_balanced(book) ? "да" : "нет");
    printf("Изменений после последней балансировки: %u из %u\n",
           phonebook_modifications(book), PHONEBOOK_BALANCE_INTERVAL);
    printf("Выполнено балансировок: %u\n",
           phonebook_balance_count(book));

    if (phonebook_balance_count(book) > 0) {
        printf("Последняя балансировка: высота %zu -> %zu\n",
               phonebook_last_balance_before(book),
               phonebook_last_balance_after(book));
    }
}

static void show_menu(void)
{
    puts("\n========== ТЕЛЕФОННАЯ КНИГА: БИНАРНОЕ ДЕРЕВО ==========");
    puts("1 — Добавить контакт");
    puts("2 — Показать все контакты (с выбранной сортировкой)");
    puts("3 — Редактировать контакт");
    puts("4 — Удалить контакт");
    puts("5 — Показать дерево контактов");
    puts("6 — Настроить сортировку списка");
    puts("7 — Показать высоту и состояние балансировки");
    puts("8 — Выполнить балансировку вручную");
    puts("0 — Выход");
}

int main(void)
{
    (void)setlocale(LC_ALL, "");

    PhoneBook book;
    phonebook_init(&book);

    for (;;) {
        show_menu();

        long choice;
        if (!read_integer("Выберите действие: ", 0, 8, &choice, 0)) {
            putchar('\n');
            break;
        }

        switch (choice) {
            case 1:
                add_contact(&book);
                break;
            case 2:
                show_all_contacts(&book);
                break;
            case 3:
                edit_contact(&book);
                break;
            case 4:
                delete_contact(&book);
                break;
            case 5:
                puts("\n--- Структура бинарного дерева ---");
                phonebook_print_tree(&book);
                break;
            case 6:
                configure_sorting();
                break;
            case 7:
                show_tree_statistics(&book);
                break;
            case 8: {
                size_t before = phonebook_height(&book);
                phonebook_balance(&book);
                size_t after = phonebook_height(&book);
                printf("Балансировка выполнена: высота %zu -> %zu.\n",
                       before, after);
                show_tree_statistics(&book);
                break;
            }
            case 0:
                phonebook_free(&book);
                puts("Телефонная книга закрыта.");
                return 0;
            default:
                puts("Неизвестная команда.");
                break;
        }
    }

    phonebook_free(&book);
    return 0;
}
