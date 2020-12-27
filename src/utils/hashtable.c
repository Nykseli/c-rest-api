#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "memory.h"

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
    Entry* tombstone = NULL;

    for (;;) {
        Entry* entry = &entries[index];

        if (IS_NULL_STR(entry->key)) {
            if (IS_NULL(entry->value)) {
                // If entry->key is null or a tombstone,
                // it can be used to add the new entry.
                return tombstone != NULL ? tombstone : entry;
            } else {
                // We found a tombstone.
                // Save the pointer so it can be used on the next loop so it can
                // be returned as a reusable entry for the value.
                // Tombstone has null key but non null value.
                if (tombstone == NULL)
                    tombstone = entry;
            }
        } else if (strcmp(entry->key.chars, key->chars) == 0) {
            return entry;
        }

        index = (index + 1) % capacity;
    }
}

static void adjust_capacity(Table* table, int capacity)
{
    Entry* entries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; i++) {
        entries[i].key = NULL_STR;
        entries[i].value = NULL_VAL;
    }

    table->count = 0;
    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];
        if (IS_NULL_STR(entry->key))
            continue;

        Entry* dest = find_entry(entries, capacity, &entry->key);
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

    bool is_new_key = IS_NULL_STR(entry->key);
    if (is_new_key)
        table->count++;

    entry->key = *key;
    entry->value = value;
    return is_new_key;
}

bool table_delete(Table* table, String* key)
{
    if (table->count == 0)
        return false;

    // Find the entry
    Entry* entry = find_entry(table->entries, table->capacity, key);
    if (IS_NULL_STR(entry->key))
        return false;

    // Place a tombstone in the entry.
    entry->key = NULL_STR;
    entry->value = BOOL_VAL(true);

    return true;
}

void table_add_all(Table* from, Table* to)
{
    for (int i = 0; i < from->capacity; i++) {
        Entry* entry = &from->entries[i];
        if (!IS_NULL_STR(entry->key)) {
            table_set(to, &entry->key, entry->value);
        }
    }
}

String table_find_string(Table* table, const char* chars, int length, uint32_t hash)
{
    // If the table is empty, we definitely won't find it.
    if (table->entries == NULL)
        return NULL_STR;

    uint32_t index = hash % table->capacity;

    for (;;) {
        Entry* entry = &table->entries[index];

        if (IS_NULL_STR(entry->key)) {
            // Stop if we find an empty non-tombstone entry.
            if (IS_NULL(entry->value))
                return NULL_STR;
        } else if (entry->key.len == length && entry->key.hash == hash && memcmp(entry->key.chars, chars, length) == 0) {
            // We found it.
            return entry->key;
        }

        // Try the next slot.
        index = (index + 1) % table->capacity;
    }
}

//TODO: also tables inside of the entries
Table copy_table(const Table table)
{
    Entry* tmp_entries = ALLOCATE(Entry, table.capacity);
    memcpy(tmp_entries, table.entries, (sizeof(Entry)) * table.capacity);

    // Create copy of the data values
    for (int i = 0; i < table.capacity; i++) {
        if (!IS_NULL_STR(tmp_entries[i].key)) {
            tmp_entries[i].key = copy_string(tmp_entries[i].key);
            copy_data_value(&tmp_entries[i].value);
        }
    }

    Table new_table;
    new_table.capacity = table.capacity;
    new_table.count = table.count;
    new_table.entries = tmp_entries;
    return new_table;
}
