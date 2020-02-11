#pragma once

#include <runtime.h>

typedef void* type_t;

typedef signed char (*lessthan_predicate_t)(type_t, type_t);

typedef struct ordered_array
{
	type_t* array;
	unsigned int size;
	unsigned int max_size;
	lessthan_predicate_t less_than;
} ordered_array_t;

signed char standard_lessthan_predicate(type_t a, type_t b);

ordered_array_t create_ordered_array(unsigned int max_size, lessthan_predicate_t less_than);
ordered_array_t place_ordered_array(void* address, unsigned int max_size, lessthan_predicate_t less_than);

void insert_ordered_array(type_t item, struct ordered_array* array);
void remove_ordered_array(unsigned int i, struct ordered_array* array);

type_t lookup_ordered_array(unsigned int i, struct ordered_array* array);