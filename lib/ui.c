#include "ui.h"
#include <stdio.h>
#include <string.h>

static void trim_newline(char* s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n && (s[n-1]=='\n' || s[n-1]=='\r')) {
        s[--n] = '\0';
    }
}

static int read_line(char* buf, size_t cap) {
    if (!fgets(buf, (int)cap, stdin)) return -1;
    trim_newline(buf);
    return 0;
}

void ui_print_menu() {
    printf("Menu:\n");
    printf("  1. Add discount\n");
    printf("  2. Delete discount(s) by shop\n");
    printf("  3. Show discounts (sorted by shop)\n");
    printf("  4. Exit\n");
    printf("> ");
}

void ui_report_error(int status) {
    if (status) {
        printf("Error: not enough memory.\n");
    } else {
        printf("OK.\n");
    }
}

void ui_report_deleted(size_t n) {
    if (n==0) {
        printf("No discounts have been deleted.\n");
    }
    else {
        printf("Amount of discounts deleted: %zu\n", n);
    }
}

int ui_read_command(int* out_cmd) {
    char line[64];
    if (read_line(line, sizeof line) != 0) return -1;
    int v;
    if (sscanf(line, "%d", &v) != 1) { 
        return 0; 
    }
    *out_cmd = v;
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

int ui_prompt_shop(char* out, size_t cap) {
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

int ui_prompt_record(char* shop, size_t shop_cap, int* discount, char* expiry, size_t expiry_cap) {
    return ui_prompt_shop(shop, shop_cap) || prompt_discount(discount) || prompt_expiry(expiry, expiry_cap);
}

static int print_record_cb(const Record* r, void* user) {
    (void)user;
    printf("Shop: %s | Discount: %d | Expiry: %s\n",
           r->shop ? r->shop : "",
           r->discount,
           r->expiry ? r->expiry : "");
    return 0;
}

void ui_show_sorted(const Journal* j) {
    if (!j || journal_count(j) == 0) { 
        printf("No discounts yet.\n"); 
        return; 
    }
    journal_for_each_sorted_by_shop(j);
}
