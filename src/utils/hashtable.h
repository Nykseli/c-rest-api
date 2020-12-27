#ifndef CLOX_TABLE_H
#define CLOX_TABLE_H

#include "../datatypes.h"
#include <stdbool.h>
#include <stdint.h>

#define TABLE_MAX_LOAD 0.75

uint32_t hash_string(const char* key, int length);

void init_table(Table* table);
void free_table(Table* table);
bool table_get(Table* table, String* key, DataValue* value);
bool table_set(Table* table, String* key, DataValue value);
bool table_delete(Table* table, String* key);
void table_add_all(Table* from, Table* to);
String table_find_string(Table* table, const char* chars, int length, uint32_t hash);
Table copy_table(const Table table);


#endif
