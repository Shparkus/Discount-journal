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

void record_array_list_init(RecordArrayList* list);
void record_array_list_destroy(RecordArrayList* list);

size_t record_array_list_size(const RecordArrayList* list);
size_t record_array_list_capacity(const RecordArrayList* list);
int record_array_list_reserve(RecordArrayList* list, size_t min_capacity);
void record_array_list_shrink_to_fit(RecordArrayList* list);

int record_array_list_push_back(RecordArrayList* list, const Record* r);
int record_array_list_insert(RecordArrayList* list, size_t index, const Record* r);
int record_array_list_remove_at(RecordArrayList* list, size_t index);
int record_array_list_set(RecordArrayList* list, size_t index, const Record* r);
void record_array_list_clear(RecordArrayList* list);

const Record* record_array_list_get_const(const RecordArrayList* list, size_t index);
Record* record_array_list_get(RecordArrayList* list, size_t index);