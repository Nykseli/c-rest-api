#ifndef REST_DATATYPES_H_
#define REST_DATATYPES_H_

#include "socketcon.h"
#include "utils/memory.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_OBJECT,
    TYPE_BOOL,
    TYPE_NULL,

    // Rest api
    TYPE_API_FUNCTION
} DataType;

typedef struct Entry Entry;
typedef struct DataValue DataValue;

typedef struct {
    void* value;
} Null;

typedef struct {
    char* chars;
    int len;
    int capacity;
    uint32_t hash; // for table reference
} String;

typedef struct {
    DataValue* values;
    int length;
    int capacity;
} Array;

typedef struct {
    int count;
    int capacity;
    Entry* entries;
} Table;

typedef enum {
    GET = 1,
    POST,
    PUT,
    DELETE
} RequestType;

typedef struct {
    RequestType type;
    String uri;
    String content;
    Table* params;
} Request;

// Struct used for callback functions
typedef struct {
    Connection conn;
} Response;

typedef void (*RestCallback)(Response* resp, Request* test);

//TODO: own tables for each request type POST GET PUT etc.
typedef struct {
    int* clients;
    Table urls;
    String** endpoints;
    int endpoint_len;
} RestServer;

typedef struct {
    String* keywords; // Contains the /:id/:name etc keywords in order
    int kw_len;
    RestCallback callback;
} ApiUrl;

struct DataValue {
    DataType type;
    union datatypes {
        bool boolean;
        float number;
        String string;
        Array array;
        Table obj;
        ApiUrl api_url;
        void* voider;
    } data;
};

struct Entry {
    String key;
    DataValue value;
};

#define IS_BOOL(value) ((value).type == TYPE_BOOL)
#define IS_NULL(value) ((value).type == TYPE_NULL)
#define IS_NUMBER(value) ((value).type == TYPE_NUMBER)
#define IS_OBJ(value) ((value).type == TYPE_OBJECT)
#define IS_ARRAY(value) ((value).type == TYPE_ARRAY)
#define IS_STRING(value) ((value).type == TYPE_STRING)

#define IS_NULL_STR(str) ((str).len == -1)

#define AS_OBJ(value) (value.data.obj)
#define AS_NULL(value) (value.data.null)
#define AS_BOOL(value) (value.data.boolean)
#define AS_VOID(value) (value.data.voider)
#define AS_ARRAY(value) (value.data.array)
#define AS_NUMBER(value) (value.data.number)
#define AS_STRING(value) (value.data.string)
#define AS_CSTRING(value) (value.data.string.chars)
#define AS_API_CALL(value) (value.data.api_url)
#define AS_API_TABLE(value) (value.data.api_table)

#define BOOL_VAL(value) ((DataValue) { TYPE_BOOL, {value} })
#define NULL_VAL ((DataValue) { TYPE_NULL, { 0 } })
#define NULL_STR ((String) {NULL, -1, -1, -1})

#define STRING_APPEND(str, c)                                                              \
    do {                                                                                   \
        if ((str)->capacity < (str)->len + 1) {                                            \
            int _old_capacity = (str)->capacity;                                           \
            (str)->capacity = GROW_CAPACITY(_old_capacity);                                \
            (str)->chars = GROW_ARRAY((str)->chars, char, _old_capacity, (str)->capacity); \
            for (int _i = _old_capacity; _i < (str)->capacity; _i++) {                     \
                (str)->chars[_i] = '\0';                                                   \
            }                                                                              \
        }                                                                                  \
        (str)->chars[(str)->len] = c;                                                      \
        (str)->len++;                                                                      \
    } while (0);

#define STRING_APPEND_STRING(str1, str2)        \
    for (int _i = 0; _i < (str2)->len; _i++) {  \
        STRING_APPEND(str1, (str2)->chars[_i]); \
    };

#define STRING_APPEND_CSTRING(str1, str2, len) \
    for (int _i = 0; _i < len; _i++) {         \
        STRING_APPEND(str1, str2[_i])          \
    };

// Free the char pointer
#define STRING_FREE(str) FREE(char, (str)->chars)
// Free the char pointer and the String pointer
#define STRINGP_FREE(str)     \
    FREE(char, (str)->chars); \
    FREE(String, (str))

#define STRING_INIT(str) \
    (str)->chars = NULL; \
    (str)->len = 0;      \
    (str)->capacity = 0; \
    (str)->hash = 0;

String copy_chars(const char* chars, int length);
String copy_string(const String str);
void copy_data_value(DataValue* value);
void init_array(Array* arr);

#endif
