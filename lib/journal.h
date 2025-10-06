#pragma once
#include <stddef.h>
#include "record_array_list.h"

typedef struct {
    RecordArrayList list;
} Journal;

int journal_init(Journal* j);
void journal_destroy(Journal* j);
size_t journal_count(const Journal* j);

int journal_add(Journal* j, const char* shop, int discount, const char* expiry);
size_t journal_delete_by_shop(Journal* j, const char* shop);

void journal_for_each_sorted_by_shop(const Journal* j);