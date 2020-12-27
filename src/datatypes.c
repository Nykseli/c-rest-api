#include <string.h>

#include "datatypes.h"
#include "utils/hashtable.h"
#include "utils/memory.h"

static String allocate_string(char* chars, int length, uint32_t hash)
{
    String string;
    string.len = length;
    string.chars = chars;
    string.hash = hash;

    return string;
}

String copy_chars(const char* chars, int length)
{
    uint32_t hash = hash_string(chars, length);

    char* tmp_chars = ALLOCATE(char, length + 1);
    memcpy(tmp_chars, chars, length);
    tmp_chars[length] = '\0';

    return allocate_string(tmp_chars, length, hash);
}

String copy_string(const String str)
{
    return copy_chars(str.chars, str.len);
}

void copy_data_value(DataValue* value)
{
    if (value == NULL)
        return;
    switch (value->type) {
    case TYPE_STRING: {

        value->data.string = copy_string(value->data.string);
    } break;

    case TYPE_OBJECT:
        value->data.obj = copy_table(value->data.obj);
        break;

    default:
        break;
    }
}

void init_array(Array* arr)
{
    arr->values = NULL;
    arr->length = 0;
    arr->capacity = 0;
}
