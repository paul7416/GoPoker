#ifndef LIST_H
#define LIST_H
#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#define LIST_BLOCK_SIZE 0x100

//This code is auto generated using template 'templates/list.h.jinja' using script list.py
//Types handled
// uint64_list  (C type: uint64_t)
// uint16_list  (C type: uint16_t)
// uint8_list   (C type: uint8_t)
// int64_list   (C type: int64_t)
// int32_list   (C type: int32_t)
// int16_list   (C type: int16_t)
// int8_list    (C type: int8_t)
// double_list  (C type: double)
// bool_list    (C type: bool)



typedef struct{
    size_t capacity;
    int length;
    uint64_t *data;
}uint64_list;

uint64_list *uint64_list_create(int length);
uint64_t uint64_list_get(uint64_list *l, int index);
void uint64_list_free(uint64_list *l);
void uint64_list_set(uint64_list *l, int index, uint64_t value);
int uint64_list_append(uint64_list *l, uint64_t value);
int uint64_list_insert(uint64_list *l, int index, uint64_t value);
uint64_t uint64_list_pop(uint64_list *l, int index);
int uint64_list_concat(uint64_list *dst, const uint64_list *src);
int uint64_list_index(const uint64_list *l, const uint64_t value);


typedef struct{
    size_t capacity;
    int length;
    uint16_t *data;
}uint16_list;

uint16_list *uint16_list_create(int length);
uint16_t uint16_list_get(uint16_list *l, int index);
void uint16_list_free(uint16_list *l);
void uint16_list_set(uint16_list *l, int index, uint16_t value);
int uint16_list_append(uint16_list *l, uint16_t value);
int uint16_list_insert(uint16_list *l, int index, uint16_t value);
uint16_t uint16_list_pop(uint16_list *l, int index);
int uint16_list_concat(uint16_list *dst, const uint16_list *src);
int uint16_list_index(const uint16_list *l, const uint16_t value);


typedef struct{
    size_t capacity;
    int length;
    uint8_t *data;
}uint8_list;

uint8_list *uint8_list_create(int length);
uint8_t uint8_list_get(uint8_list *l, int index);
void uint8_list_free(uint8_list *l);
void uint8_list_set(uint8_list *l, int index, uint8_t value);
int uint8_list_append(uint8_list *l, uint8_t value);
int uint8_list_insert(uint8_list *l, int index, uint8_t value);
uint8_t uint8_list_pop(uint8_list *l, int index);
int uint8_list_concat(uint8_list *dst, const uint8_list *src);
int uint8_list_index(const uint8_list *l, const uint8_t value);


typedef struct{
    size_t capacity;
    int length;
    int64_t *data;
}int64_list;

int64_list *int64_list_create(int length);
int64_t int64_list_get(int64_list *l, int index);
void int64_list_free(int64_list *l);
void int64_list_set(int64_list *l, int index, int64_t value);
int int64_list_append(int64_list *l, int64_t value);
int int64_list_insert(int64_list *l, int index, int64_t value);
int64_t int64_list_pop(int64_list *l, int index);
int int64_list_concat(int64_list *dst, const int64_list *src);
int int64_list_index(const int64_list *l, const int64_t value);


typedef struct{
    size_t capacity;
    int length;
    int32_t *data;
}int32_list;

int32_list *int32_list_create(int length);
int32_t int32_list_get(int32_list *l, int index);
void int32_list_free(int32_list *l);
void int32_list_set(int32_list *l, int index, int32_t value);
int int32_list_append(int32_list *l, int32_t value);
int int32_list_insert(int32_list *l, int index, int32_t value);
int32_t int32_list_pop(int32_list *l, int index);
int int32_list_concat(int32_list *dst, const int32_list *src);
int int32_list_index(const int32_list *l, const int32_t value);


typedef struct{
    size_t capacity;
    int length;
    int16_t *data;
}int16_list;

int16_list *int16_list_create(int length);
int16_t int16_list_get(int16_list *l, int index);
void int16_list_free(int16_list *l);
void int16_list_set(int16_list *l, int index, int16_t value);
int int16_list_append(int16_list *l, int16_t value);
int int16_list_insert(int16_list *l, int index, int16_t value);
int16_t int16_list_pop(int16_list *l, int index);
int int16_list_concat(int16_list *dst, const int16_list *src);
int int16_list_index(const int16_list *l, const int16_t value);


typedef struct{
    size_t capacity;
    int length;
    int8_t *data;
}int8_list;

int8_list *int8_list_create(int length);
int8_t int8_list_get(int8_list *l, int index);
void int8_list_free(int8_list *l);
void int8_list_set(int8_list *l, int index, int8_t value);
int int8_list_append(int8_list *l, int8_t value);
int int8_list_insert(int8_list *l, int index, int8_t value);
int8_t int8_list_pop(int8_list *l, int index);
int int8_list_concat(int8_list *dst, const int8_list *src);
int int8_list_index(const int8_list *l, const int8_t value);


typedef struct{
    size_t capacity;
    int length;
    double *data;
}double_list;

double_list *double_list_create(int length);
double double_list_get(double_list *l, int index);
void double_list_free(double_list *l);
void double_list_set(double_list *l, int index, double value);
int double_list_append(double_list *l, double value);
int double_list_insert(double_list *l, int index, double value);
double double_list_pop(double_list *l, int index);
int double_list_concat(double_list *dst, const double_list *src);
int double_list_index(const double_list *l, const double value);


typedef struct{
    size_t capacity;
    int length;
    bool *data;
}bool_list;

bool_list *bool_list_create(int length);
bool bool_list_get(bool_list *l, int index);
void bool_list_free(bool_list *l);
void bool_list_set(bool_list *l, int index, bool value);
int bool_list_append(bool_list *l, bool value);
int bool_list_insert(bool_list *l, int index, bool value);
bool bool_list_pop(bool_list *l, int index);
int bool_list_concat(bool_list *dst, const bool_list *src);
int bool_list_index(const bool_list *l, const bool value);


#endif
/* vim: set filetype=c : */