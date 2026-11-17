#include "contacts.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Это файл интерфейса: всё общение с пользователем находится здесь. */

/* Безопасно считывает строку, не допуская переполнения буфера. */
static int read_line(const char *prompt, char *buffer, size_t capacity)
{
    for (;;) {
        printf("%s", prompt);
        fflush(stdout);

        if (fgets(buffer, (int)capacity, stdin) == NULL) {
            return 0;
        }

        size_t length = strlen(buffer);
        if (length > 0 && buffer[length - 1] == '\n') {
            buffer[--length] = '\0';
            if (length > 0 && buffer[length - 1] == '\r') {
                buffer[length - 1] = '\0';
            }
            return 1;
        }
        if (feof(stdin)) {
            return 1;
        }

        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* Пропускаем слишком длинный ввод до конца строки. */
        }
        puts("Слишком длинная строка. Повторите ввод.");
    }
}

static int is_blank(const char *text)
{
    while (*text != '\0') {
        if (!isspace((unsigned char)*text)) {
            return 0;
        }
        ++text;
    }
    return 1;
}

static int read_required(const char *prompt, char *buffer, size_t capacity)
{
    for (;;) {
        if (!read_line(prompt, buffer, capacity)) {
            return 0;
        }
        if (!is_blank(buffer)) {
            return 1;
        }
        puts("Это обязательное поле. Введите непустое значение.");
    }
}

static int read_number(const char *prompt, int min, int max, int *result)
{
    char input[64];

    for (;;) {
        if (!read_line(prompt, input, sizeof(input))) {
            return 0;
        }

        char *end;
        errno = 0;
        long value = strtol(input, &end, 10);
        while (isspace((unsigned char)*end)) {
            ++end;
        }

        if (errno == 0 && end != input && *end == '\0'
            && value >= min && value <= max) {
            *result = (int)value;
            return 1;
        }
        printf("Введите целое число от %d до %d.\n", min, max);
    }
}

/* Читаем до MAX_VALUES значений. Пустая строка завершает список. */
static int read_values(const char *label,
                       char values[MAX_VALUES][MAX_VALUE_LENGTH],
                       size_t *count)
{
    *count = 0;
    for (size_t i = 0; i < MAX_VALUES; ++i) {
        char prompt[128];
        snprintf(prompt, sizeof(prompt), "%s %zu (Enter — закончить): ", label, i + 1);
        if (!read_line(prompt, values[i], MAX_VALUE_LENGTH)) {
            return 0;
        }
        if (is_blank(values[i])) {
            values[i][0] = '\0';
            break;
        }
        ++*count;
    }
    return 1;
}

static int read_new_contact(Contact *contact)
{
    memset(contact, 0, sizeof(*contact));

    if (!read_required("Фамилия: ", contact->last_name, sizeof(contact->last_name))
        || !read_required("Имя: ", contact->first_name, sizeof(contact->first_name))
        || !read_line("Отчество (необязательно): ", contact->middle_name, sizeof(contact->middle_name))
        || !read_line("Место работы (необязательно): ", contact->workplace, sizeof(contact->workplace))
        || !read_line("Должность (необязательно): ", contact->position, sizeof(contact->position))
        || !read_values("Телефон", contact->phones, &contact->phone_count)
        || !read_values("Электронная почта", contact->emails, &contact->email_count)
        || !read_values("Ссылка на соцсеть", contact->social_links, &contact->social_count)
        || !read_values("Профиль в мессенджере", contact->messenger_profiles,
                        &contact->messenger_count)) {
        return 0;
    }
    return 1;
}

static void print_values(const char *label,
                         const char values[MAX_VALUES][MAX_VALUE_LENGTH],
                         size_t count)
{
    printf("  %s: ", label);
    if (count == 0) {
        puts("не указаны");
        return;
    }
    printf("\n");
    for (size_t i = 0; i < count; ++i) {
        printf("    %zu) %s\n", i + 1, values[i]);
    }
}

static void print_contact(const Contact *contact, size_t index)
{
    printf("\n--- Контакт №%zu ---\n", index + 1);
    printf("  Фамилия: %s\n", contact->last_name);
    printf("  Имя: %s\n", contact->first_name);
    printf("  Отчество: %s\n", contact->middle_name[0] ? contact->middle_name : "не указано");
    printf("  Место работы: %s\n", contact->workplace[0] ? contact->workplace : "не указано");
    printf("  Должность: %s\n", contact->position[0] ? contact->position : "не указано");
    print_values("Телефоны", contact->phones, contact->phone_count);
    print_values("Электронная почта", contact->emails, contact->email_count);
    print_values("Соцсети", contact->social_links, contact->social_count);
    print_values("Мессенджеры", contact->messenger_profiles, contact->messenger_count);
}

static void show_contacts(const PhoneBook *book)
{
    if (phonebook_count(book) == 0) {
        puts("Телефонная книга пуста.");
        return;
    }
    printf("\nВсего контактов: %zu\n", phonebook_count(book));
    for (size_t i = 0; i < phonebook_count(book); ++i) {
        print_contact(phonebook_get(book, i), i);
    }
}

static int choose_contact_index(const PhoneBook *book, size_t *index)
{
    if (phonebook_count(book) == 0) {
        puts("Телефонная книга пуста.");
        return 0;
    }
    puts("\nДоступные контакты:");
    for (size_t i = 0; i < phonebook_count(book); ++i) {
        const Contact *contact = phonebook_get(book, i);
        printf("  %zu) %s %s\n", i + 1, contact->last_name, contact->first_name);
    }
    int number;
    if (!read_number("Номер контакта: ", 1, (int)phonebook_count(book), &number)) {
        return 0;
    }
    *index = (size_t)(number - 1);
    return 1;
}

static void add_contact(PhoneBook *book)
{
    Contact contact;
    puts("\nДобавление контакта");
    if (read_new_contact(&contact) && phonebook_add(book, &contact)) {
        puts("Контакт добавлен.");
    }
}

static void edit_contact(PhoneBook *book)
{
    size_t index;
    if (!choose_contact_index(book, &index)) {
        return;
    }

    Contact modified = *phonebook_get(book, index);
    int choice;
    for (;;) {
        print_contact(&modified, index);
        puts("\nЧто изменить?");
        puts("  1 — Фамилию            2 — Имя");
        puts("  3 — Отчество           4 — Место работы");
        puts("  5 — Должность          6 — Телефоны");
        puts("  7 — Электронную почту  8 — Соцсети");
        puts("  9 — Мессенджеры");
        puts("  0 — Сохранить         10 — Отменить изменения");

        if (!read_number("Выберите пункт: ", 0, 10, &choice)) {
            return;
        }
        if (choice == 0) {
            if (phonebook_update(book, index, &modified)) {
                puts("Изменения сохранены.");
            }
            return;
        }
        if (choice == 10) {
            puts("Изменения отменены.");
            return;
        }

        int ok = 1;
        switch (choice) {
            case 1:
                ok = read_required("Новая фамилия: ", modified.last_name,
                                   sizeof(modified.last_name));
                break;
            case 2:
                ok = read_required("Новое имя: ", modified.first_name,
                                   sizeof(modified.first_name));
                break;
            case 3:
                ok = read_line("Новое отчество (можно пустое): ", modified.middle_name,
                               sizeof(modified.middle_name));
                break;
            case 4:
                ok = read_line("Новое место работы (можно пустое): ", modified.workplace,
                               sizeof(modified.workplace));
                break;
            case 5:
                ok = read_line("Новая должность (можно пустую): ", modified.position,
                               sizeof(modified.position));
                break;
            case 6:
                ok = read_values("Телефон", modified.phones, &modified.phone_count);
                break;
            case 7:
                ok = read_values("Электронная почта", modified.emails, &modified.email_count);
                break;
            case 8:
                ok = read_values("Ссылка на соцсеть", modified.social_links,
                                 &modified.social_count);
                break;
            case 9:
                ok = read_values("Профиль в мессенджере", modified.messenger_profiles,
                                 &modified.messenger_count);
                break;
            default:
                break;
        }
        if (!ok) {
            return;
        }
    }
}

static void delete_contact(PhoneBook *book)
{
    size_t index;
    if (!choose_contact_index(book, &index)) {
        return;
    }
    int confirmation;
    if (!read_number("Удалить этот контакт? (1 — да, 0 — нет): ",
                     0, 1, &confirmation)) {
        return;
    }
    if (confirmation == 1 && phonebook_delete(book, index)) {
        puts("Контакт удалён.");
    } else {
        puts("Удаление отменено.");
    }
}


static void sort_contacts(PhoneBook *book)
{
    if (phonebook_count(book) == 0) {
        puts("Телефонная книга пуста.");
        return;
    }
    int field, order;
    puts("\nПоле сортировки:");
    puts("1 — Фамилия    2 — Имя    3 — Место работы");
    if (!read_number("Выберите поле: ", 1, 3, &field)) return;
    puts("1 — По возрастанию (А-Я)    2 — По убыванию (Я-А)");
    if (!read_number("Выберите порядок: ", 1, 2, &order)) return;
    if (phonebook_sort(book, (SortField)field, (SortOrder)order)) {
        puts("Порядок списка изменён. Новые контакты будут вставляться в этом порядке.");
        show_contacts(book);
    }
}

int main(void)
{
    /* static: массив контактов не размещается целиком на стеке. */
    static PhoneBook book;
    phonebook_init(&book);

    for (;;) {
        puts("\n===== ТЕЛЕФОННАЯ КНИГА =====");
        puts("1 — Добавить контакт");
        puts("2 — Показать все контакты");
        puts("3 — Редактировать контакт");
        puts("4 — Удалить контакт");
        puts("5 — Изменить сортировку");
        puts("0 — Выход");

        int choice;
        if (!read_number("Выберите действие: ", 0, 5, &choice)) {
            break;
        }

        switch (choice) {
            case 1:
                add_contact(&book);
                break;
            case 2:
                show_contacts(&book);
                break;
            case 3:
                edit_contact(&book);
                break;
            case 4:
                delete_contact(&book);
                break;
            case 5:
                sort_contacts(&book);
                break;
            case 0:
                puts("Завершение программы.");
                phonebook_free(&book);
                return 0;
            default:
                break;
        }
    }
    phonebook_free(&book);
    return 0;
}
