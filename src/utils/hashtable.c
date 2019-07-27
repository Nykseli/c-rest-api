#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
//#include "object.h"
#include "hashtable.h"
//#include "value.h"

void init_table(Table* table)
{
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void free_table(Table* table)
{
    FREE_ARRAY(Entry, table->entries, table->capacity);
    init_table(table);
}

uint32_t hash_string(const char* key, int length)
{
    uint32_t hash = 2166136261u;

    for (int i = 0; i < length; i++) {
        hash ^= key[i];
        hash *= 16777619;
    }

    return hash;
}

static Entry* find_entry(Entry* entries, int capacity, String* key)
{
    uint32_t index = key->hash % capacity;
    //TODO: fix the tombstone
    //Entry* tombstone = NULL;
    for (;;) {
        Entry* entry = &entries[index];
        return entry;
        // if (entry->key == key || entry->key == NULL) {
        //     return entry;
        // }

        // if (entry->key == NULL) {
        //     if (IS_NULL(entry->value)) {
        //         // Empty entry.
        //         return tombstone != NULL ? tombstone : entry;
        //     } else {
        //         // We found a tombstone.
        //         if (tombstone == NULL)
        //             tombstone = entry;
        //     }
        // } else if (entry->key == key) {
        //     // We found the key.
        //     return entry;
        // }

        // index = (index + 1) & capacity;
    }
}

static void adjust_capacity(Table* table, int capacity)
{
    Entry* entries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; i++) {
        entries[i].key = NULL;
        entries[i].value = NULL_VAL;
    }

    table->count = 0;
    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];
        if (entry->key == NULL)
            continue;

        Entry* dest = find_entry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }

    FREE_ARRAY(Entry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

bool table_get(Table* table, String* key, DataValue* value)
{
    // Create hash for the key if there is none
    if (key->hash == 0) {
        key->hash = hash_string(key->chars, key->len);
    }

    if (table->entries == NULL)
        return false;

    Entry* entry = find_entry(table->entries, table->capacity, key);
    if (entry == NULL)
        return false;

    *value = entry->value;
    return true;
}

bool table_set(Table* table, String* key, DataValue value)
{
    // Create hash for the key if there is none
    if (key->hash == 0) {
        key->hash = hash_string(key->chars, key->len);
    }

    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        adjust_capacity(table, capacity);
    }
    Entry* entry = find_entry(table->entries, table->capacity, key);

    bool is_new_key = entry->key == NULL;
    if (is_new_key)
        table->count++;
    if (is_new_key && IS_NULL(entry->value))
        table->count++;

    entry->key = key;
    entry->value = value;
    return is_new_key;
}

bool table_delete(Table* table, String* key)
{
    if (table->count == 0)
        return false;

    // Find the entry
    Entry* entry = find_entry(table->entries, table->capacity, key);
    if (entry->key == NULL)
        return false;

    // Place a tombstone in the entry.
    entry->key = NULL;
    entry->value = BOOL_VAL(true);

    return true;
}

void table_add_all(Table* from, Table* to)
{
    for (int i = 0; i < from->capacity; i++) {
        Entry* entry = &from->entries[i];
        if (entry->key != NULL) {
            table_set(to, entry->key, entry->value);
        }
    }
}

String* table_find_string(Table* table, const char* chars, int length, uint32_t hash)
{
    // If the table is empty, we definitely won't find it.
    if (table->entries == NULL)
        return NULL;

    uint32_t index = hash % table->capacity;

    for (;;) {
        Entry* entry = &table->entries[index];

        if (entry->key == NULL) {
            // Stop if we find an empty non-tombstone entry.
            if (IS_NULL(entry->value))
                return NULL;
        } else if (entry->key->len == length && entry->key->hash == hash && memcmp(entry->key->chars, chars, length) == 0) {
            // We found it.
            return entry->key;
        }

        // Try the next slot.
        index = (index + 1) % table->capacity;
    }
}

//TODO: also tables inside of the entries
Table* copy_table(const Table* table){
    Entry* tmp_entries = ALLOCATE(Entry, table->capacity);
    memcpy(tmp_entries, table->entries, (sizeof(Entry)) * table->capacity);

    Table* new_table = ALLOCATE(Table, 1);
    new_table->capacity = table->capacity;
    new_table->count = table->count;
    new_table->entries = tmp_entries;
    return new_table;
}

