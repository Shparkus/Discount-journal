#include <stdio.h>
#include <stdlib.h>

#include "../lib/record_array_list.h"

static void trim_newline(char* s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[--n] = '\0';
    }
}

static int read_line(char* buf, size_t cap) {
    if (!fgets(buf, (int)cap, stdin)) return -1;
    trim_newline(buf);
    return 0;
}

static int is_blank(const char* s) {
    if (!s) return 1;
    for (; *s; ++s) {
        if (!isspace((unsigned char)*s)) {
            return 0;
        }
    }
    return 1;
}

// case insensitive
static int ci_strcmp(const char* a, const char* b) { 
    for (; *a && *b; ++a, ++b) {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

static int is_leap(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static int days_in_month(int year, int month) {
    static const int m_days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (month == 2 && is_leap(year)) {
        return m_days[1] + 1;
    }
    if (month == 2 && !is_leap(year)) {
        return m_days[1];
    }
    if (month >= 1 && month <= 12) {
        return m_days[month - 1];
    }
    return 0;
}

static int validate_date(const char* s) {
    if (!s) return 0;
    if (strlen(s) != 10) return 0;
    if (s[4] != '-' || s[7] != '-') return 0;
    int y = 0, m = 0, d = 0;
    if (sscanf(s, "%4d-%2d-%2d", &y, &m, &d) != 3) return 0;
    if (y < 2025) return 0;
    if (m < 1 || m > 12) return 0;
    int days_current = days_in_month(y, m);
    if (d < 1 || d > days_current) return 0;
    return 1;
}

static void print_record(const Record* r, size_t idx) {
    printf("%zu. Shop: %s\n", idx + 1, r->shop ? r->shop : "(null)");
    printf("   Discount: %d%%\n", r->discount);
    printf("   Expiry:   %s\n", r->expiry ? r->expiry : "(null)");
}

static void print_menu(void) {
    printf("Menu:\n");
    printf("  1. Add discount\n");
    printf("  2. Delete discount(s) by shop\n");
    printf("  3. Show discounts (sorted by shop)\n");
    printf("  4. Exit\n");
    printf("> ");
}

// Simple insertion sort 
static void sort_by_shop(RecordArrayList* list) {
    for (size_t i = 1; i < list->size; ++i) {
        Record key = list->data[i];
        size_t j = i;
        while (j > 0 && ci_strcmp(list->data[j - 1].shop, key.shop) > 0) {
        list->data[j] = list->data[j - 1];
        --j;
        }
        list->data[j] = key;
    }
}

static int prompt_shop(char* out, size_t cap) {
    for (;;) {
        printf("Shop (up to 100 chars): ");
        if (read_line(out, cap) != 0) return -1;
        if (is_blank(out)) {
            printf("Error: shop name cannot be empty.\n");
            continue;
        }
        if (strlen(out) > 100) {
            printf("Error: too long.\n");
            continue;
        }
        return 0;
    }
}

static int prompt_discount(int* out) {
    char buf[64];
    for (;;) {
        printf("Discount percent [0..100]: ");
        if (read_line(buf, sizeof(buf)) != 0) return -1;
        char* end = NULL;
        long v = strtol(buf, &end, 10);
        if (end == buf || *end != '\0') {
        printf("Error: enter integer 0..100.\n");
        continue;
        }
        if (v < 0 || v > 100) {
        printf("Error: must be between 0 and 100.\n");
        continue;
        }
        *out = (int)v;
        return 0;
    }
}

static int prompt_expiry(char* out, size_t cap) {
    for (;;) {
        printf("Expiry date (YYYY-MM-DD, year>=2025): ");
        if (read_line(out, cap) != 0) return -1;
        if (!validate_date(out)) {
        printf("Error: invalid date.\n");
        continue;
        }
        return 0;
    }
}

static void action_add(RecordArrayList* list) {
    char shop[256];
    char expiry[64];
    int discount = 0;

    if (prompt_shop(shop, sizeof(shop)) != 0) return;
    if (prompt_discount(&discount) != 0) return;
    if (prompt_expiry(expiry, sizeof(expiry)) != 0) return;

    Record r = record_make(shop, discount, expiry);
    if (record_array_list_push_back(list, &r) != 0) {
        printf("Error: out of memory while adding.\n");
    } else {
        printf("Added.\n");
    }
    record_destroy(&r);
}

static void action_delete_by_shop(RecordArrayList* list) {
    if (list->size == 0) {
        printf("Journal is empty.\n");
        return;
    }
    char shop[256];
    if (prompt_shop(shop, sizeof(shop)) != 0) return;

    size_t removed = 0;
    for (size_t i = 0; i < list->size;) {
        if (ci_strcmp(list->data[i].shop, shop) == 0) {
            if(record_array_list_remove_at(list, i) != 0){
                printf("Unexpected error during delete.\n");
            }
            ++removed;
            continue;
        }
        ++i;
    }
    if (removed == 0) {
        printf("No entries with shop \"%s\".\n", shop);
    } else if (removed == 1){
        printf("Removed %zu entry.\n", removed);
    } else {
        printf("Removed %zu entries.\n", removed);
    }
}

static void action_show_sorted(RecordArrayList* list) {
    if (list->size == 0) {
        printf("No discounts yet.\n");
        return;
    }
    sort_by_shop(list);
    printf("Your discounts (sorted by shop):\n");
    for (size_t i = 0; i < list->size; ++i) {
        print_record(&list->data[i], i);
    }
}

int main(void) {
    RecordArrayList list;
    record_array_list_init(&list);

    printf("Discount Journal. Enter the menu number and press [Enter].\n");

    for (;;) {
        print_menu();
        char buf[32];
        if (read_line(buf, sizeof(buf)) != 0) break;
        if (is_blank(buf)) continue;

        if (strcmp(buf, "1") == 0) {
            action_add(&list);
        } else if (strcmp(buf, "2") == 0) {
            action_delete_by_shop(&list);
        } else if (strcmp(buf, "3") == 0) {
            action_show_sorted(&list);
        } else if (strcmp(buf, "4") == 0) {
            break;
        } else {
            printf("Unknown command: %s\n", buf);
        }
        printf("\n");
    }

    record_array_list_destroy(&list);
    printf("Bye.\n");
    return 0;
}
