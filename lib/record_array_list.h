#pragma once

#include <stddef.h>

typedef struct {
    char* shop;
    int discount;
    char* expiry; //"2025-11-10"
} Record;

Record record_make(const char* shop, int discount, const char* expiry);
void record_destroy(Record* r);
int record_assign(Record* dst, const Record* src);

typedef struct {
    Record* data;
    size_t size;
    size_t capacity;
} RecordArrayList;

int record_array_list_init(RecordArrayList* list);
void record_array_list_destroy(RecordArrayList* list);

int record_array_list_reserve(RecordArrayList* list, size_t min_capacity);

int record_array_list_push_back(RecordArrayList* list, const Record* r);
int record_array_list_remove_at(RecordArrayList* list, size_t index);