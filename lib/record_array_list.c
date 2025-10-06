#include "record_array_list.h"

#include <stdlib.h>
#include <string.h>

static const size_t kInitialCapacity = 4;

static char* str_duplicate(const char* src) {
    if (src == NULL) return NULL;
    size_t n = strlen(src) + 1;
    char* p = (char*)malloc(n);
    if (!p) return NULL;
    memcpy(p, src, n);
    return p;
}

Record record_make(const char* shop, int discount, const char* expiry) {
    Record r;
    r.shop = str_duplicate(shop);
    r.discount = discount;
    r.expiry = str_duplicate(expiry);
    return r;
}

void record_destroy(Record* r) {
    if (!r) return;
    free(r->shop);
    free(r->expiry);
    r->shop = NULL;
    r->expiry = NULL;
}

int record_assign(Record* dst, const Record* src) {
    if (!dst || !src) return -1;
    if (dst == src) return 0;
    char* new_shop = str_duplicate(src->shop);
    char* new_expiry = str_duplicate(src->expiry);
    if ((src->shop && !new_shop) || (src->expiry && !new_expiry)) {
        free(new_shop);
        free(new_expiry);
        return -1;
    }
    free(dst->shop);
    free(dst->expiry);
    dst->shop = new_shop;
    dst->discount = src->discount;
    dst->expiry = new_expiry;
    return 0;
}

int record_array_list_init(RecordArrayList* list) {
    if (!list) return 1;
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
    return 0;
}

void record_array_list_destroy(RecordArrayList* list) {
    for(size_t i = 0; i < list->size; ++i) {
        record_destroy(&list->data[i]);
    }
    free(list->data);
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
}

size_t record_array_list_size(const RecordArrayList* list) {
    if (!list) return 0;
    return list->size;
}

size_t record_array_list_capacity(const RecordArrayList* list) {
    if (!list) return 0;
    return list->capacity;
}

int record_array_list_reserve(RecordArrayList* list, size_t min_capacity) {
    if (!list) return -1;
    if (list->capacity >= min_capacity) return 0;
    size_t new_cap = kInitialCapacity;
    if (list->capacity){
        new_cap = list->capacity;
    }
    while (new_cap < min_capacity) {
        if (new_cap > (SIZE_MAX / 2)) {
            new_cap = min_capacity;
            break;
        }
        new_cap *= 2;
    }
    Record* new_data = (Record*)realloc(list->data, new_cap * sizeof(Record));
    if (!new_data) return -1;
    list->data = new_data;
    list->capacity = new_cap;
    return 0;
}

void record_array_list_shrink_to_fit(RecordArrayList* list) {
    if (!list) return;
    if (list->size == list->capacity) return;
    if (list->size == 0) {
        free(list->data);
        list->data = NULL;
        list->capacity = 0;
        return;
    }
    Record* new_data = (Record*)realloc(list->data, list->size * sizeof(Record));
    if (new_data) {
        list->data = new_data;
        list->capacity = list->size;
    }
}

int record_array_list_push_back(RecordArrayList* list, const Record* r) {
    if (!list || !r) return -1;
    if (record_array_list_reserve(list, list->size + 1) != 0) return -1;
    Record* dst = &list->data[list->size];
    dst->shop = NULL;
    dst->expiry = NULL;
    if (record_assign(dst, r) != 0) return -1;
    list->size += 1;
    return 0;
}

int record_array_list_insert(RecordArrayList* list, size_t index, const Record* r) {
    if (!list || !r) return -1;
    if (index > list->size) return -1;

    if (record_array_list_reserve(list, list->size + 1) != 0) return -1;
    for (size_t i = list->size; i > index; --i) {
        list->data[i] = list->data[i - 1];
    }
    list->data[index].shop = NULL;
    list->data[index].expiry = NULL;
    if (record_assign(&list->data[index], r) != 0) {
        for (size_t i = index; i < list->size; ++i) {
            list->data[i] = list->data[i + 1];
        }
        return -1;
    }
    list->size += 1;
    return 0;
}

int record_array_list_remove_at(RecordArrayList* list, size_t index) {
    if (!list) return -1;
    if (index >= list->size) return -1;
    record_destroy(&list->data[index]);
    for (size_t i = index + 1; i < list->size; ++i) {
        list->data[i - 1] = list->data[i];
    }
    list->size -= 1;
    return 0;
}

int record_array_list_set(RecordArrayList* list, size_t index, const Record* r) {
    if (!list || !r) return -1;
    if (index >= list->size) return -1;
    return record_assign(&list->data[index], r);
}

void record_array_list_clear(RecordArrayList* list) {
    if (!list) return;
    for (size_t i = 0; i < list->size; ++i) {
        record_destroy(&list->data[i]);
    }
    list->size = 0;
}

const Record* record_array_list_get_const(const RecordArrayList* list, size_t index) {
    if (!list || index >= list->size) return NULL;
    return &list->data[index];
}

Record* record_array_list_get(RecordArrayList* list, size_t index) {
    if (!list || index >= list->size) return NULL;
    return &list->data[index];
}
