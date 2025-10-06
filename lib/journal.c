#include "journal.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int journal_init(Journal* j) {
    if (!j) return -1;
    return record_array_list_init(&j->list);
}

void journal_destroy(Journal* j) {
    if (!j) return;
    record_array_list_destroy(&j->list);
}

size_t journal_count(const Journal* j) { 
    if (!j) {
        return 0;
    }
    return j->list.size;
}

int journal_add(Journal* j, const char* shop, int discount, const char* expiry) {
    if (!j) return 1;

    Record tmp = record_make(shop, discount, expiry);    
    if (record_array_list_push_back(&j->list, &tmp) == 0) {
        record_destroy(&tmp);
        return 0;
    }
    record_destroy(&tmp);
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

size_t journal_delete_by_shop(Journal* j, const char* shop) {
    if (!j || j->list.size == 0) return 0;

    size_t removed = 0;
    for (size_t i = 0; i < j->list.size;) {
        if (ci_strcmp(j->list.data[i].shop, shop) == 0) {
            record_array_list_remove_at(&j->list, i);
            removed++;
        } else {
            i++;
        }
    }
    return removed;
}

static void journal_sort(const Journal* j){
    const RecordArrayList* list = &j->list;
    for (size_t i = 1; i < list->size; ++i) {
        Record key = list->data[i];
        size_t j = i;

        while (j > 0 && ci_strcmp(list->data[j-1].shop, key.shop) > 0) {
            list->data[j] = list->data[j - 1];
            --j;
        }
        
        list->data[j] = key;
    } 
}

static void print_record(const Record* r, size_t idx) {
    printf("%zu. Shop: %s\n", idx + 1, r->shop);
    printf("   Discount: %d%%\n", r->discount);
    printf("   Expiry:   %s\n", r->expiry);
}

void journal_for_each_sorted_by_shop(const Journal* j) {
    journal_sort(j);
    printf("Your discounts (sorted by shop):\n");
    for (size_t i = 0; i < journal_count(j); i++) {
        print_record(&(&j->list)->data[i], i);
    }
}
