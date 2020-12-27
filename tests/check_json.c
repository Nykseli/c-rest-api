#include "../src/utils/json.h"
#include <check.h>

START_TEST(simple_json_parse_t)
{
    char json[] = "{\"name\": \"sample\"}";
    JSONString jstring = copy_chars(json, 19);
    bool succss = false;
    JSONObject obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONString sample;
    json_get_string_c(&obj, "name", &sample);
    ck_assert_str_eq(sample.chars, "sample");
    free_json(&obj);
    STRING_FREE(&sample);
    STRING_FREE(&jstring);
}
END_TEST

START_TEST(multiple_pair_json_parse_t)
{
    char json[] = "{\"name\": \"sample\", \"number\": \"12345\", \"address\": \"Test Addr 1234\"}";
    JSONString jstring = copy_chars(json, 67);
    bool succss = false;
    JSONObject obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONString sample;
    json_get_string_c(&obj, "name", &sample);
    ck_assert_str_eq(sample.chars, "sample");
    JSONString number;
    json_get_string_c(&obj, "number", &number);
    ck_assert_str_eq(number.chars, "12345");
    JSONString address;
    json_get_string_c(&obj, "address", &address);
    ck_assert_str_eq(address.chars, "Test Addr 1234");
    free_json(&obj);
    STRING_FREE(&sample);
    STRING_FREE(&number);
    STRING_FREE(&address);
    STRING_FREE(&jstring);
}
END_TEST

START_TEST(object_json_parse_t)
{
    char json[] = "{\"name\": \"sample\", \"number\": {\"real\": \"1234567\"}}";
    JSONString jstring = copy_chars(json, 67);
    bool succss = false;
    JSONObject obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONString sample;
    json_get_string_c(&obj, "name", &sample);
    ck_assert_str_eq(sample.chars, "sample");
    JSONObject number;
    json_get_object_c(&obj, "number", &number);
    JSONString real;
    json_get_string_c(&number, "real", &real);
    ck_assert_str_eq(real.chars, "1234567");
    free_json(&obj);
    free_json(&number);
    STRING_FREE(&sample);
    STRING_FREE(&real);
    STRING_FREE(&jstring);
}
END_TEST

START_TEST(multiple_object_json_parse_t)
{
    char json[] = "{\"name\": \"sample\", \"number\": {\"real\": \"1234567\"}, \"addr\": {\"actual\": \"Test Addr 1234\"}}";
    JSONString jstring = copy_chars(json, 88);
    bool succss = false;
    JSONObject obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONString sample;
    json_get_string_c(&obj, "name", &sample);
    ck_assert_str_eq(sample.chars, "sample");
    JSONObject number;
    json_get_object_c(&obj, "number", &number);
    JSONString real;
    json_get_string_c(&number, "real", &real);
    ck_assert_str_eq(real.chars, "1234567");
    JSONObject addr;
    json_get_object_c(&obj, "addr", &addr);
    JSONString actual;
    json_get_string_c(&addr, "actual", &actual);
    ck_assert_str_eq(actual.chars, "Test Addr 1234");
    free_json(&obj);
    free_json(&number);
    free_json(&addr);
    STRING_FREE(&sample);
    STRING_FREE(&real);
    STRING_FREE(&actual);
    STRING_FREE(&jstring);
}
END_TEST

START_TEST(nested_object_json_parse_t)
{
    char json[] = "{\"name\": \"sample\", \"number\": {\"real\": \"1234567\", \"addr\": {\"actual\": \"Test Addr 1234\"}}}";
    JSONString jstring = copy_chars(json, 88);
    bool succss = false;
    JSONObject obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONString sample;
    json_get_string_c(&obj, "name", &sample);
    ck_assert_str_eq(sample.chars, "sample");
    JSONObject number;
    json_get_object_c(&obj, "number", &number);
    JSONString real;
    json_get_string_c(&number, "real", &real);
    ck_assert_str_eq(real.chars, "1234567");
    JSONObject addr;
    json_get_object_c(&number, "addr", &addr);
    JSONString actual;
    json_get_string_c(&addr, "actual", &actual);
    ck_assert_str_eq(actual.chars, "Test Addr 1234");
    free_json(&obj);
    free_json(&number);
    free_json(&addr);
    STRING_FREE(&sample);
    STRING_FREE(&real);
    STRING_FREE(&actual);
    STRING_FREE(&jstring);
}
END_TEST

START_TEST(simple_json_parse_fail_t)
{
    char json[] = "{\"name\", \"sample\"}";
    JSONString jstring = copy_chars(json, 19);
    bool succss = false;
    JSONObject obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, false);
    free_json(&obj);
}
END_TEST

START_TEST(simple_json_bool_and_number_t)
{
    char json[] = "{\"false\": false, \"true\": true, \"number\": 1234}";
    JSONString jstring = copy_chars(json, 47);
    bool succss = true;
    JSONObject obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONBool fal;
    json_get_bool_c(&obj, "false", &fal);
    ck_assert_int_eq(fal, false);
    JSONBool tr;
    json_get_bool_c(&obj, "true", &tr);
    ck_assert_int_eq(tr, true);
    JSONNumber nr;
    json_get_number_c(&obj, "number", &nr);
    ck_assert_int_eq((int)(nr), 1234);
    free_json(&obj);
}
END_TEST

START_TEST(json_kw_array_len0_t)
{
    char json[] = "{\"array\": []}";
    JSONString jstring = copy_chars(json, 14);
    bool succss = true;
    JSONObject obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONArray arr;
    json_get_array_c(&obj, "array", &arr);
    ck_assert_int_eq(arr.length, 0);
    free_json(&obj);
}
END_TEST

START_TEST(json_kw_array_len1_t)
{
    char json[] = "{\"array\": [\"single\"]}";
    JSONString jstring = copy_chars(json, 22);
    bool succss = true;
    JSONObject obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONArray arr;
    json_get_array_c(&obj, "array", &arr);
    ck_assert_int_eq(arr.length, 1);
    ck_assert_int_eq(IS_STRING(arr.values[0]), 1);
    ck_assert_str_eq(AS_CSTRING(arr.values[0]), "single");
    free_json(&obj);
}
END_TEST

START_TEST(json_kw_array_len5_t)
{
    char json[] = "{\"array\": [\"single\", 2, false, true, {\"obj\": 3}]}";
    JSONString jstring = copy_chars(json, 53);
    bool succss = true;
    JSONObject obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONArray arr;
    json_get_array_c(&obj, "array", &arr);
    ck_assert_int_eq(arr.length, 5);
    ck_assert_int_eq(IS_STRING(arr.values[0]), 1);
    ck_assert_int_eq(IS_NUMBER(arr.values[1]), 1);
    ck_assert_int_eq(IS_BOOL(arr.values[2]), 1);
    ck_assert_int_eq(IS_BOOL(arr.values[3]), 1);
    ck_assert_int_eq(IS_OBJ(arr.values[4]), 1);
    ck_assert_str_eq(AS_CSTRING(arr.values[0]), "single");
    ck_assert_int_eq((int)AS_NUMBER(arr.values[1]), 2);
    ck_assert_int_eq(AS_BOOL(arr.values[2]), 0);
    ck_assert_int_eq(AS_BOOL(arr.values[3]), 1);
    JSONNumber nr;
    json_get_number_c(&AS_OBJ(arr.values[4]), "obj", &nr);
    ck_assert_int_eq((int)(nr), 3);

    free_json(&obj);
}
END_TEST

START_TEST(json_to_string_t)
{
    char json[] = "{\"array\":[\"single\",2,false,true,{\"obj\":3}]}";
    JSONString jstring = copy_chars(json, 52);
    bool succss = true;
    JSONObject obj = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONString str = json_to_string(obj);
    // Length should be 43 since the null doesn't count as length
    ck_assert_int_eq(str.len, 43);
    ck_assert_int_eq(strncmp(str.chars, json, 44), 0);
    free_json(&obj);
    STRING_FREE(&str);
}
END_TEST

START_TEST(json_add_to_obj_basic_t)
{
    JSONObject obj;
    init_json(&obj);
    bool result = json_add_string_c(&obj, "string", "1234");
    ck_assert_int_eq(result, true);
    JSONString tmpstr;
    json_get_string_c(&obj, "string", &tmpstr);
    ck_assert_str_eq(tmpstr.chars, "1234");
    result = json_add_bool_c(&obj, "boolean", true);
    ck_assert_int_eq(result, true);
    JSONBool tmpbool;
    json_get_bool_c(&obj, "boolean", &tmpbool);
    ck_assert_int_eq(tmpbool, true);
    JSONNumber _num = (JSONNumber)1234;
    result = json_add_number_c(&obj, "number", _num);
    ck_assert_int_eq((int)result, true);
    JSONNumber tmpnum;
    json_get_number_c(&obj, "number", &tmpnum);
    ck_assert_int_eq((int)tmpnum, 1234);
    free_json(&obj);
}
END_TEST

START_TEST(json_add_to_obj_array_t)
{
    char json[] = "{\"name\": \"sample\"}";
    JSONString jstring = copy_chars(json, 19);
    bool succss = false;
    JSONObject obj_t = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONObject obj;
    init_json(&obj);
    JSONArray arr;
    init_array(&arr);
    JSONValue boolean = json_value_bool(true);
    JSONValue string = json_value_string_c("array");
    JSONValue number = json_value_number(1234);
    JSONValue object = json_value_object(obj_t);
    json_array_append_value(&arr, boolean);
    json_array_append_value(&arr, string);
    json_array_append_value(&arr, number);
    json_array_append_value(&arr, object);
    json_add_array_c(&obj, "arrayname", arr);
    JSONArray t_arr;
    json_get_array_c(&obj, "arrayname", &t_arr);
    ck_assert_int_eq(IS_BOOL(t_arr.values[0]), 1);
    ck_assert_int_eq(IS_STRING(t_arr.values[1]), 1);
    ck_assert_int_eq(IS_NUMBER(t_arr.values[2]), 1);
    ck_assert_int_eq(IS_OBJ(t_arr.values[3]), 1);
    ck_assert_int_eq(AS_BOOL(t_arr.values[0]), 1);
    ck_assert_str_eq(AS_CSTRING(t_arr.values[1]), "array");
    ck_assert_int_eq((int)AS_NUMBER(t_arr.values[2]), 1234);
    JSONString nr;
    json_get_string_c(&AS_OBJ(arr.values[3]), "name", &nr);
    ck_assert_str_eq(nr.chars, "sample");
    free_json(&obj_t);
}
END_TEST

START_TEST(json_add_to_obj_obj_t)
{
    char json[] = "{\"name\": \"sample\"}";
    JSONString jstring = copy_chars(json, 19);
    bool succss = false;
    JSONObject obj_t = parse_json(jstring, &succss);
    ck_assert_int_eq(succss, true);
    JSONObject obj;
    init_json(&obj);
    bool result = json_add_object_c(&obj, "obj", obj_t);
    ck_assert_int_eq(result, true);
    JSONObject r_obj;
    json_get_object_c(&obj, "obj", &r_obj);
    JSONString nr;
    json_get_string_c(&r_obj, "name", &nr);
    ck_assert_str_eq(nr.chars, "sample");
    free_json(&obj);
    free_json(&obj_t);
}
END_TEST

Suite* json_suite()
{
    Suite* s;
    TCase* tc_core;

    s = suite_create("JSON");

    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, simple_json_parse_t);
    tcase_add_test(tc_core, multiple_pair_json_parse_t);
    tcase_add_test(tc_core, object_json_parse_t);
    tcase_add_test(tc_core, multiple_object_json_parse_t);
    tcase_add_test(tc_core, nested_object_json_parse_t);
    tcase_add_test(tc_core, simple_json_parse_fail_t);
    tcase_add_test(tc_core, simple_json_bool_and_number_t);
    tcase_add_test(tc_core, json_kw_array_len0_t);
    tcase_add_test(tc_core, json_kw_array_len1_t);
    tcase_add_test(tc_core, json_kw_array_len5_t);
    tcase_add_test(tc_core, json_to_string_t);
    tcase_add_test(tc_core, json_add_to_obj_basic_t);
    tcase_add_test(tc_core, json_add_to_obj_array_t);
    tcase_add_test(tc_core, json_add_to_obj_obj_t);
    suite_add_tcase(s, tc_core);

    return s;
}

int main()
{
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = json_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
