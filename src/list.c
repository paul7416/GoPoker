#include "list.h"

//This code is auto generated using template 'templates/list.c.jinja' using script list.py

static inline int check_bounds(int length, int index)
{
    int original_index = index;
    if(index < 0) index = length + index;
    if(index < 0 || index >= length)
    {
        fprintf(stderr, "Out-of-bounds access in function '%s' index=%d (normalized=%d), size=%d\n",__func__, original_index, index, length);
        exit(EXIT_FAILURE);
    }
    return index;
}

uint64_list *uint64_list_create(int length)
{
    int blocks = length / LIST_BLOCK_SIZE + 1;
    uint64_list *l = calloc(1, sizeof(uint64_list));
    l->capacity = blocks * LIST_BLOCK_SIZE;
    l->length = length;
    l->data = calloc(l->capacity, sizeof(uint64_t));
    return l;
}
void uint64_list_free(uint64_list *l)
{
    free(l->data);
    free(l);
}

uint64_t uint64_list_get(uint64_list *l, int index)
{
    index = check_bounds(l->length, index);
    return l->data[index];
}

void uint64_list_set(uint64_list *l, int index, uint64_t value)
{
    index = check_bounds(l->length, index);
    l->data[index] = value;
}

int uint64_list_append(uint64_list *l, uint64_t value)
{
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(uint64_t));
    }
    l->data[l->length] = value;
    l->length++;
    return l->length;
}

int uint64_list_insert(uint64_list *l, int index, uint64_t value)
{
    index = check_bounds(l->length, index);
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(uint64_t));
    }
    for(int i = l->length; i > index; i--)
    {
        l->data[i] = l->data[i - 1];
    }
    l->data[index] = value;
    l->length++;
    return l->length;
}

uint64_t uint64_list_pop(uint64_list *l, int index)
{
    index = check_bounds(l->length, index);
    uint64_t output = l->data[index];
    l->length--;
    for(int i = index; i < l->length; i++)
    {
        l->data[i] = l->data[i + 1];
    }
    return output;
}

int uint64_list_concat(uint64_list *dst, const uint64_list *src)
{
    if (dst == src) {
        fprintf(stderr, "Cannot concatenate list with itself in %s\n", __func__);
        exit(EXIT_FAILURE);
    }

    size_t total = (size_t)dst->length + (size_t)src->length;

    if (dst->capacity < total) {
        size_t blocks = (total + LIST_BLOCK_SIZE - 1) / LIST_BLOCK_SIZE;
        dst->capacity = blocks * LIST_BLOCK_SIZE;

        uint64_t *new_data = realloc(dst->data, dst->capacity * sizeof(uint64_t));
        if (!new_data) {
            fprintf(stderr, "Allocation failed in %s\n", __func__);
            exit(EXIT_FAILURE);
        }
        dst->data = new_data;
    }

    memcpy(
        dst->data + dst->length,
        src->data,
        src->length * sizeof(uint64_t)
    );

    dst->length = (int)total;
    return dst->length;
}

int uint64_list_index(const uint64_list *l, const uint64_t value)
{
    for(int i = 0; i < l->length; i++)
    {
        if (l->data[i] == value) return i;
    }
    return -1;
}

uint16_list *uint16_list_create(int length)
{
    int blocks = length / LIST_BLOCK_SIZE + 1;
    uint16_list *l = calloc(1, sizeof(uint16_list));
    l->capacity = blocks * LIST_BLOCK_SIZE;
    l->length = length;
    l->data = calloc(l->capacity, sizeof(uint16_t));
    return l;
}
void uint16_list_free(uint16_list *l)
{
    free(l->data);
    free(l);
}

uint16_t uint16_list_get(uint16_list *l, int index)
{
    index = check_bounds(l->length, index);
    return l->data[index];
}

void uint16_list_set(uint16_list *l, int index, uint16_t value)
{
    index = check_bounds(l->length, index);
    l->data[index] = value;
}

int uint16_list_append(uint16_list *l, uint16_t value)
{
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(uint16_t));
    }
    l->data[l->length] = value;
    l->length++;
    return l->length;
}

int uint16_list_insert(uint16_list *l, int index, uint16_t value)
{
    index = check_bounds(l->length, index);
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(uint16_t));
    }
    for(int i = l->length; i > index; i--)
    {
        l->data[i] = l->data[i - 1];
    }
    l->data[index] = value;
    l->length++;
    return l->length;
}

uint16_t uint16_list_pop(uint16_list *l, int index)
{
    index = check_bounds(l->length, index);
    uint16_t output = l->data[index];
    l->length--;
    for(int i = index; i < l->length; i++)
    {
        l->data[i] = l->data[i + 1];
    }
    return output;
}

int uint16_list_concat(uint16_list *dst, const uint16_list *src)
{
    if (dst == src) {
        fprintf(stderr, "Cannot concatenate list with itself in %s\n", __func__);
        exit(EXIT_FAILURE);
    }

    size_t total = (size_t)dst->length + (size_t)src->length;

    if (dst->capacity < total) {
        size_t blocks = (total + LIST_BLOCK_SIZE - 1) / LIST_BLOCK_SIZE;
        dst->capacity = blocks * LIST_BLOCK_SIZE;

        uint16_t *new_data = realloc(dst->data, dst->capacity * sizeof(uint16_t));
        if (!new_data) {
            fprintf(stderr, "Allocation failed in %s\n", __func__);
            exit(EXIT_FAILURE);
        }
        dst->data = new_data;
    }

    memcpy(
        dst->data + dst->length,
        src->data,
        src->length * sizeof(uint16_t)
    );

    dst->length = (int)total;
    return dst->length;
}

int uint16_list_index(const uint16_list *l, const uint16_t value)
{
    for(int i = 0; i < l->length; i++)
    {
        if (l->data[i] == value) return i;
    }
    return -1;
}

uint8_list *uint8_list_create(int length)
{
    int blocks = length / LIST_BLOCK_SIZE + 1;
    uint8_list *l = calloc(1, sizeof(uint8_list));
    l->capacity = blocks * LIST_BLOCK_SIZE;
    l->length = length;
    l->data = calloc(l->capacity, sizeof(uint8_t));
    return l;
}
void uint8_list_free(uint8_list *l)
{
    free(l->data);
    free(l);
}

uint8_t uint8_list_get(uint8_list *l, int index)
{
    index = check_bounds(l->length, index);
    return l->data[index];
}

void uint8_list_set(uint8_list *l, int index, uint8_t value)
{
    index = check_bounds(l->length, index);
    l->data[index] = value;
}

int uint8_list_append(uint8_list *l, uint8_t value)
{
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(uint8_t));
    }
    l->data[l->length] = value;
    l->length++;
    return l->length;
}

int uint8_list_insert(uint8_list *l, int index, uint8_t value)
{
    index = check_bounds(l->length, index);
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(uint8_t));
    }
    for(int i = l->length; i > index; i--)
    {
        l->data[i] = l->data[i - 1];
    }
    l->data[index] = value;
    l->length++;
    return l->length;
}

uint8_t uint8_list_pop(uint8_list *l, int index)
{
    index = check_bounds(l->length, index);
    uint8_t output = l->data[index];
    l->length--;
    for(int i = index; i < l->length; i++)
    {
        l->data[i] = l->data[i + 1];
    }
    return output;
}

int uint8_list_concat(uint8_list *dst, const uint8_list *src)
{
    if (dst == src) {
        fprintf(stderr, "Cannot concatenate list with itself in %s\n", __func__);
        exit(EXIT_FAILURE);
    }

    size_t total = (size_t)dst->length + (size_t)src->length;

    if (dst->capacity < total) {
        size_t blocks = (total + LIST_BLOCK_SIZE - 1) / LIST_BLOCK_SIZE;
        dst->capacity = blocks * LIST_BLOCK_SIZE;

        uint8_t *new_data = realloc(dst->data, dst->capacity * sizeof(uint8_t));
        if (!new_data) {
            fprintf(stderr, "Allocation failed in %s\n", __func__);
            exit(EXIT_FAILURE);
        }
        dst->data = new_data;
    }

    memcpy(
        dst->data + dst->length,
        src->data,
        src->length * sizeof(uint8_t)
    );

    dst->length = (int)total;
    return dst->length;
}

int uint8_list_index(const uint8_list *l, const uint8_t value)
{
    for(int i = 0; i < l->length; i++)
    {
        if (l->data[i] == value) return i;
    }
    return -1;
}

int64_list *int64_list_create(int length)
{
    int blocks = length / LIST_BLOCK_SIZE + 1;
    int64_list *l = calloc(1, sizeof(int64_list));
    l->capacity = blocks * LIST_BLOCK_SIZE;
    l->length = length;
    l->data = calloc(l->capacity, sizeof(int64_t));
    return l;
}
void int64_list_free(int64_list *l)
{
    free(l->data);
    free(l);
}

int64_t int64_list_get(int64_list *l, int index)
{
    index = check_bounds(l->length, index);
    return l->data[index];
}

void int64_list_set(int64_list *l, int index, int64_t value)
{
    index = check_bounds(l->length, index);
    l->data[index] = value;
}

int int64_list_append(int64_list *l, int64_t value)
{
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(int64_t));
    }
    l->data[l->length] = value;
    l->length++;
    return l->length;
}

int int64_list_insert(int64_list *l, int index, int64_t value)
{
    index = check_bounds(l->length, index);
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(int64_t));
    }
    for(int i = l->length; i > index; i--)
    {
        l->data[i] = l->data[i - 1];
    }
    l->data[index] = value;
    l->length++;
    return l->length;
}

int64_t int64_list_pop(int64_list *l, int index)
{
    index = check_bounds(l->length, index);
    int64_t output = l->data[index];
    l->length--;
    for(int i = index; i < l->length; i++)
    {
        l->data[i] = l->data[i + 1];
    }
    return output;
}

int int64_list_concat(int64_list *dst, const int64_list *src)
{
    if (dst == src) {
        fprintf(stderr, "Cannot concatenate list with itself in %s\n", __func__);
        exit(EXIT_FAILURE);
    }

    size_t total = (size_t)dst->length + (size_t)src->length;

    if (dst->capacity < total) {
        size_t blocks = (total + LIST_BLOCK_SIZE - 1) / LIST_BLOCK_SIZE;
        dst->capacity = blocks * LIST_BLOCK_SIZE;

        int64_t *new_data = realloc(dst->data, dst->capacity * sizeof(int64_t));
        if (!new_data) {
            fprintf(stderr, "Allocation failed in %s\n", __func__);
            exit(EXIT_FAILURE);
        }
        dst->data = new_data;
    }

    memcpy(
        dst->data + dst->length,
        src->data,
        src->length * sizeof(int64_t)
    );

    dst->length = (int)total;
    return dst->length;
}

int int64_list_index(const int64_list *l, const int64_t value)
{
    for(int i = 0; i < l->length; i++)
    {
        if (l->data[i] == value) return i;
    }
    return -1;
}

int32_list *int32_list_create(int length)
{
    int blocks = length / LIST_BLOCK_SIZE + 1;
    int32_list *l = calloc(1, sizeof(int32_list));
    l->capacity = blocks * LIST_BLOCK_SIZE;
    l->length = length;
    l->data = calloc(l->capacity, sizeof(int32_t));
    return l;
}
void int32_list_free(int32_list *l)
{
    free(l->data);
    free(l);
}

int32_t int32_list_get(int32_list *l, int index)
{
    index = check_bounds(l->length, index);
    return l->data[index];
}

void int32_list_set(int32_list *l, int index, int32_t value)
{
    index = check_bounds(l->length, index);
    l->data[index] = value;
}

int int32_list_append(int32_list *l, int32_t value)
{
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(int32_t));
    }
    l->data[l->length] = value;
    l->length++;
    return l->length;
}

int int32_list_insert(int32_list *l, int index, int32_t value)
{
    index = check_bounds(l->length, index);
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(int32_t));
    }
    for(int i = l->length; i > index; i--)
    {
        l->data[i] = l->data[i - 1];
    }
    l->data[index] = value;
    l->length++;
    return l->length;
}

int32_t int32_list_pop(int32_list *l, int index)
{
    index = check_bounds(l->length, index);
    int32_t output = l->data[index];
    l->length--;
    for(int i = index; i < l->length; i++)
    {
        l->data[i] = l->data[i + 1];
    }
    return output;
}

int int32_list_concat(int32_list *dst, const int32_list *src)
{
    if (dst == src) {
        fprintf(stderr, "Cannot concatenate list with itself in %s\n", __func__);
        exit(EXIT_FAILURE);
    }

    size_t total = (size_t)dst->length + (size_t)src->length;

    if (dst->capacity < total) {
        size_t blocks = (total + LIST_BLOCK_SIZE - 1) / LIST_BLOCK_SIZE;
        dst->capacity = blocks * LIST_BLOCK_SIZE;

        int32_t *new_data = realloc(dst->data, dst->capacity * sizeof(int32_t));
        if (!new_data) {
            fprintf(stderr, "Allocation failed in %s\n", __func__);
            exit(EXIT_FAILURE);
        }
        dst->data = new_data;
    }

    memcpy(
        dst->data + dst->length,
        src->data,
        src->length * sizeof(int32_t)
    );

    dst->length = (int)total;
    return dst->length;
}

int int32_list_index(const int32_list *l, const int32_t value)
{
    for(int i = 0; i < l->length; i++)
    {
        if (l->data[i] == value) return i;
    }
    return -1;
}

int16_list *int16_list_create(int length)
{
    int blocks = length / LIST_BLOCK_SIZE + 1;
    int16_list *l = calloc(1, sizeof(int16_list));
    l->capacity = blocks * LIST_BLOCK_SIZE;
    l->length = length;
    l->data = calloc(l->capacity, sizeof(int16_t));
    return l;
}
void int16_list_free(int16_list *l)
{
    free(l->data);
    free(l);
}

int16_t int16_list_get(int16_list *l, int index)
{
    index = check_bounds(l->length, index);
    return l->data[index];
}

void int16_list_set(int16_list *l, int index, int16_t value)
{
    index = check_bounds(l->length, index);
    l->data[index] = value;
}

int int16_list_append(int16_list *l, int16_t value)
{
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(int16_t));
    }
    l->data[l->length] = value;
    l->length++;
    return l->length;
}

int int16_list_insert(int16_list *l, int index, int16_t value)
{
    index = check_bounds(l->length, index);
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(int16_t));
    }
    for(int i = l->length; i > index; i--)
    {
        l->data[i] = l->data[i - 1];
    }
    l->data[index] = value;
    l->length++;
    return l->length;
}

int16_t int16_list_pop(int16_list *l, int index)
{
    index = check_bounds(l->length, index);
    int16_t output = l->data[index];
    l->length--;
    for(int i = index; i < l->length; i++)
    {
        l->data[i] = l->data[i + 1];
    }
    return output;
}

int int16_list_concat(int16_list *dst, const int16_list *src)
{
    if (dst == src) {
        fprintf(stderr, "Cannot concatenate list with itself in %s\n", __func__);
        exit(EXIT_FAILURE);
    }

    size_t total = (size_t)dst->length + (size_t)src->length;

    if (dst->capacity < total) {
        size_t blocks = (total + LIST_BLOCK_SIZE - 1) / LIST_BLOCK_SIZE;
        dst->capacity = blocks * LIST_BLOCK_SIZE;

        int16_t *new_data = realloc(dst->data, dst->capacity * sizeof(int16_t));
        if (!new_data) {
            fprintf(stderr, "Allocation failed in %s\n", __func__);
            exit(EXIT_FAILURE);
        }
        dst->data = new_data;
    }

    memcpy(
        dst->data + dst->length,
        src->data,
        src->length * sizeof(int16_t)
    );

    dst->length = (int)total;
    return dst->length;
}

int int16_list_index(const int16_list *l, const int16_t value)
{
    for(int i = 0; i < l->length; i++)
    {
        if (l->data[i] == value) return i;
    }
    return -1;
}

int8_list *int8_list_create(int length)
{
    int blocks = length / LIST_BLOCK_SIZE + 1;
    int8_list *l = calloc(1, sizeof(int8_list));
    l->capacity = blocks * LIST_BLOCK_SIZE;
    l->length = length;
    l->data = calloc(l->capacity, sizeof(int8_t));
    return l;
}
void int8_list_free(int8_list *l)
{
    free(l->data);
    free(l);
}

int8_t int8_list_get(int8_list *l, int index)
{
    index = check_bounds(l->length, index);
    return l->data[index];
}

void int8_list_set(int8_list *l, int index, int8_t value)
{
    index = check_bounds(l->length, index);
    l->data[index] = value;
}

int int8_list_append(int8_list *l, int8_t value)
{
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(int8_t));
    }
    l->data[l->length] = value;
    l->length++;
    return l->length;
}

int int8_list_insert(int8_list *l, int index, int8_t value)
{
    index = check_bounds(l->length, index);
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(int8_t));
    }
    for(int i = l->length; i > index; i--)
    {
        l->data[i] = l->data[i - 1];
    }
    l->data[index] = value;
    l->length++;
    return l->length;
}

int8_t int8_list_pop(int8_list *l, int index)
{
    index = check_bounds(l->length, index);
    int8_t output = l->data[index];
    l->length--;
    for(int i = index; i < l->length; i++)
    {
        l->data[i] = l->data[i + 1];
    }
    return output;
}

int int8_list_concat(int8_list *dst, const int8_list *src)
{
    if (dst == src) {
        fprintf(stderr, "Cannot concatenate list with itself in %s\n", __func__);
        exit(EXIT_FAILURE);
    }

    size_t total = (size_t)dst->length + (size_t)src->length;

    if (dst->capacity < total) {
        size_t blocks = (total + LIST_BLOCK_SIZE - 1) / LIST_BLOCK_SIZE;
        dst->capacity = blocks * LIST_BLOCK_SIZE;

        int8_t *new_data = realloc(dst->data, dst->capacity * sizeof(int8_t));
        if (!new_data) {
            fprintf(stderr, "Allocation failed in %s\n", __func__);
            exit(EXIT_FAILURE);
        }
        dst->data = new_data;
    }

    memcpy(
        dst->data + dst->length,
        src->data,
        src->length * sizeof(int8_t)
    );

    dst->length = (int)total;
    return dst->length;
}

int int8_list_index(const int8_list *l, const int8_t value)
{
    for(int i = 0; i < l->length; i++)
    {
        if (l->data[i] == value) return i;
    }
    return -1;
}

double_list *double_list_create(int length)
{
    int blocks = length / LIST_BLOCK_SIZE + 1;
    double_list *l = calloc(1, sizeof(double_list));
    l->capacity = blocks * LIST_BLOCK_SIZE;
    l->length = length;
    l->data = calloc(l->capacity, sizeof(double));
    return l;
}
void double_list_free(double_list *l)
{
    free(l->data);
    free(l);
}

double double_list_get(double_list *l, int index)
{
    index = check_bounds(l->length, index);
    return l->data[index];
}

void double_list_set(double_list *l, int index, double value)
{
    index = check_bounds(l->length, index);
    l->data[index] = value;
}

int double_list_append(double_list *l, double value)
{
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(double));
    }
    l->data[l->length] = value;
    l->length++;
    return l->length;
}

int double_list_insert(double_list *l, int index, double value)
{
    index = check_bounds(l->length, index);
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(double));
    }
    for(int i = l->length; i > index; i--)
    {
        l->data[i] = l->data[i - 1];
    }
    l->data[index] = value;
    l->length++;
    return l->length;
}

double double_list_pop(double_list *l, int index)
{
    index = check_bounds(l->length, index);
    double output = l->data[index];
    l->length--;
    for(int i = index; i < l->length; i++)
    {
        l->data[i] = l->data[i + 1];
    }
    return output;
}

int double_list_concat(double_list *dst, const double_list *src)
{
    if (dst == src) {
        fprintf(stderr, "Cannot concatenate list with itself in %s\n", __func__);
        exit(EXIT_FAILURE);
    }

    size_t total = (size_t)dst->length + (size_t)src->length;

    if (dst->capacity < total) {
        size_t blocks = (total + LIST_BLOCK_SIZE - 1) / LIST_BLOCK_SIZE;
        dst->capacity = blocks * LIST_BLOCK_SIZE;

        double *new_data = realloc(dst->data, dst->capacity * sizeof(double));
        if (!new_data) {
            fprintf(stderr, "Allocation failed in %s\n", __func__);
            exit(EXIT_FAILURE);
        }
        dst->data = new_data;
    }

    memcpy(
        dst->data + dst->length,
        src->data,
        src->length * sizeof(double)
    );

    dst->length = (int)total;
    return dst->length;
}

int double_list_index(const double_list *l, const double value)
{
    for(int i = 0; i < l->length; i++)
    {
        if (l->data[i] == value) return i;
    }
    return -1;
}

bool_list *bool_list_create(int length)
{
    int blocks = length / LIST_BLOCK_SIZE + 1;
    bool_list *l = calloc(1, sizeof(bool_list));
    l->capacity = blocks * LIST_BLOCK_SIZE;
    l->length = length;
    l->data = calloc(l->capacity, sizeof(bool));
    return l;
}
void bool_list_free(bool_list *l)
{
    free(l->data);
    free(l);
}

bool bool_list_get(bool_list *l, int index)
{
    index = check_bounds(l->length, index);
    return l->data[index];
}

void bool_list_set(bool_list *l, int index, bool value)
{
    index = check_bounds(l->length, index);
    l->data[index] = value;
}

int bool_list_append(bool_list *l, bool value)
{
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(bool));
    }
    l->data[l->length] = value;
    l->length++;
    return l->length;
}

int bool_list_insert(bool_list *l, int index, bool value)
{
    index = check_bounds(l->length, index);
    if(l->length == (int)l->capacity)
    {
        l->capacity *= 2;
        l->data = realloc(l->data, (l->capacity) * sizeof(bool));
    }
    for(int i = l->length; i > index; i--)
    {
        l->data[i] = l->data[i - 1];
    }
    l->data[index] = value;
    l->length++;
    return l->length;
}

bool bool_list_pop(bool_list *l, int index)
{
    index = check_bounds(l->length, index);
    bool output = l->data[index];
    l->length--;
    for(int i = index; i < l->length; i++)
    {
        l->data[i] = l->data[i + 1];
    }
    return output;
}

int bool_list_concat(bool_list *dst, const bool_list *src)
{
    if (dst == src) {
        fprintf(stderr, "Cannot concatenate list with itself in %s\n", __func__);
        exit(EXIT_FAILURE);
    }

    size_t total = (size_t)dst->length + (size_t)src->length;

    if (dst->capacity < total) {
        size_t blocks = (total + LIST_BLOCK_SIZE - 1) / LIST_BLOCK_SIZE;
        dst->capacity = blocks * LIST_BLOCK_SIZE;

        bool *new_data = realloc(dst->data, dst->capacity * sizeof(bool));
        if (!new_data) {
            fprintf(stderr, "Allocation failed in %s\n", __func__);
            exit(EXIT_FAILURE);
        }
        dst->data = new_data;
    }

    memcpy(
        dst->data + dst->length,
        src->data,
        src->length * sizeof(bool)
    );

    dst->length = (int)total;
    return dst->length;
}

int bool_list_index(const bool_list *l, const bool value)
{
    for(int i = 0; i < l->length; i++)
    {
        if (l->data[i] == value) return i;
    }
    return -1;
}




/* vim: set filetype=c : */