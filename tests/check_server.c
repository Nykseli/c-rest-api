#include "../src/server.h"
#include <check.h>

int simple_global_int = -1;

void simple_rest_callback_fun(Response* r, Request* test)
{
    simple_global_int = 10;
}

START_TEST(simple_rest_callback_t)
{
    RestServer rs;
    init_server(&rs);
    add_url(&rs, "/api", simple_rest_callback_fun);
    String url = copy_chars("/api", strlen("/api"));
    ApiUrl* au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(simple_global_int, 10);
    STRING_FREE(&url);
}
END_TEST

int stress_global_int = -1;
void stress_api_fun(Response* r, Request* test)
{
    stress_global_int = 10;
}
void stress_api_test_name_fun(Response* r, Request* test)
{
    stress_global_int = 11;
}
void stress_api_test_name_endpoint_fun(Response* r, Request* test)
{
    stress_global_int = 12;
}
void stress_api_test_name_id_fun(Response* r, Request* test)
{
    stress_global_int = 13;
}
void stress_api_test_name_id_pos_fun(Response* r, Request* test)
{
    stress_global_int = 14;
}
void stress_api_backend_username_delete_id_fun(Response* r, Request* test)
{
    stress_global_int = 15;
}
void stress_api_backend_username_add_id_fun(Response* r, Request* test)
{
    stress_global_int = 16;
}
void stress_api_backend_username_get_id_fun(Response* r, Request* test)
{
    stress_global_int = 17;
}
void stress_frontend_get_main_fun(Response* r, Request* test)
{
    stress_global_int = 18;
}
void stress_frontend_get_home_fun(Response* r, Request* test)
{
    stress_global_int = 19;
}
void stress_frontend_get_blog_fun(Response* r, Request* test)
{
    stress_global_int = 20;
}
void stress_frontend_get_blog_id_fun(Response* r, Request* test)
{
    stress_global_int = 21;
}
void stress_fun(Response* r, Request* test)
{
    stress_global_int = 22;
}
void stress_website_endpoint_id_pos_error_fun(Response* r, Request* test)
{
    stress_global_int = 23;
}
void stress_website_report_errorid_fun(Response* r, Request* test)
{
    stress_global_int = 24;
}
void stress_website_error_fun(Response* r, Request* test)
{
    stress_global_int = 25;
}
void stress_website_status_fun(Response* r, Request* test)
{
    stress_global_int = 26;
}
void stress_backend_status_fun(Response* r, Request* test)
{
    stress_global_int = 27;
}
void stress_backend_error_id_fun(Response* r, Request* test)
{
    stress_global_int = 28;
}
void stress_backend_fun(Response* r, Request* test)
{
    stress_global_int = 29;
}
void stress_online_fun(Response* r, Request* test)
{
    stress_global_int = 30;
}

START_TEST(stress_rest_callback_t)
{
    RestServer rs;
    init_server(&rs);
    add_url(&rs, "/api", stress_api_fun);
    add_url(&rs, "/api/test/name", stress_api_test_name_fun);
    add_url(&rs, "/api/test/name/endpoint", stress_api_test_name_endpoint_fun);
    add_url(&rs, "/api/name/:id", stress_api_test_name_id_fun);
    add_url(&rs, "/api/name/:id/:pos", stress_api_test_name_id_pos_fun);
    add_url(&rs, "/api/backend/username/delete/:id", stress_api_backend_username_delete_id_fun);
    add_url(&rs, "/api/backend/username/add/:id", stress_api_backend_username_add_id_fun);
    add_url(&rs, "/api/backend/username/get/:id", stress_api_backend_username_get_id_fun);
    add_url(&rs, "/api/frontend/get/main", stress_frontend_get_main_fun);
    add_url(&rs, "/api/frontend/get/home", stress_frontend_get_home_fun);
    add_url(&rs, "/api/frontend/get/blog", stress_frontend_get_blog_fun);
    add_url(&rs, "/api/frontend/get/blog/:id", stress_frontend_get_blog_id_fun);
    add_url(&rs, "/", stress_fun);
    add_url(&rs, "/website/endpoint/:id/:pos/:error", stress_website_endpoint_id_pos_error_fun);
    add_url(&rs, "/website/report/:errorid", stress_website_report_errorid_fun);
    add_url(&rs, "/website/error", stress_website_error_fun);
    add_url(&rs, "/website/status", stress_website_status_fun);
    add_url(&rs, "/backend/status", stress_backend_status_fun);
    add_url(&rs, "/backend/error/:id", stress_backend_error_id_fun);
    add_url(&rs, "/backend", stress_backend_fun);
    add_url(&rs, "/online", stress_online_fun);
    String url = copy_chars("/api", strlen("/api"));
    ApiUrl* au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 10);
    STRING_FREE(&url);
    url = copy_chars("/api/test/name", strlen("/api/test/name"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 11);
    STRING_FREE(&url);
    url = copy_chars("/api/test/name/endpoint", strlen("/api/test/name/endpoint"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 12);
    STRING_FREE(&url);
    url = copy_chars("/api/name/123", strlen("/api/name/123"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 13);
    STRING_FREE(&url);
    url = copy_chars("/api/name/1234/test", strlen("/api/name/1234/test"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 14);
    STRING_FREE(&url);
    url = copy_chars("/api/backend/username/delete/2", strlen("/api/backend/username/delete/2"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 15);
    STRING_FREE(&url);
    url = copy_chars("/api/backend/username/add/4", strlen("/api/backend/username/add/4"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 16);
    STRING_FREE(&url);
    url = copy_chars("/api/backend/username/get/33", strlen("/api/backend/username/get/33"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 17);
    STRING_FREE(&url);
    url = copy_chars("/api/frontend/get/main", strlen("/api/frontend/get/main"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 18);
    STRING_FREE(&url);
    url = copy_chars("/api/frontend/get/home", strlen("/api/frontend/get/home"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 19);
    STRING_FREE(&url);
    url = copy_chars("/api/frontend/get/blog", strlen("/api/frontend/get/blog"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 20);
    STRING_FREE(&url);
    url = copy_chars("/api/frontend/get/blog/2", strlen("/api/frontend/get/blog/2"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 21);
    STRING_FREE(&url);
    url = copy_chars("/", strlen("/"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 22);
    STRING_FREE(&url);
    url = copy_chars("/website/endpoint/1/2/3", strlen("/website/endpoint/1/2/3"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 23);
    STRING_FREE(&url);
    url = copy_chars("/website/report/4", strlen("/website/report/4"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 24);
    STRING_FREE(&url);
    url = copy_chars("/website/error", strlen("/website/error"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 25);
    STRING_FREE(&url);
    url = copy_chars("/website/status", strlen("/website/status"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 26);
    STRING_FREE(&url);
    url = copy_chars("/backend/status", strlen("/backend/status"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 27);
    STRING_FREE(&url);
    url = copy_chars("/backend/error/2", strlen("/backend/error/2"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 28);
    STRING_FREE(&url);
    url = copy_chars("/backend", strlen("/backend"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 29);
    STRING_FREE(&url);
    url = copy_chars("/online", strlen("/online"));
    au = get_call_back(&rs, url);
    (au->callback)(NULL, NULL);
    ck_assert_int_eq(stress_global_int, 30);
    STRING_FREE(&url);
}
END_TEST

String global_parameter_string1;
String global_parameter_string2;
String global_parameter_string3;

void parameter_api_id_fun(Response* r, Request* test)
{
    json_get_string_c(test->params, "id", &global_parameter_string1);
}
void parameter_api_id_pos_fun(Response* r, Request* test)
{
    json_get_string_c(test->params, "id", &global_parameter_string1);
    json_get_string_c(test->params, "pos", &global_parameter_string2);
}
void parameter_endpoint_id_fun(Response* r, Request* test)
{
    json_get_string_c(test->params, "id", &global_parameter_string1);
    json_get_string_c(test->params, "pos", &global_parameter_string2);
    json_get_string_c(test->params, "end", &global_parameter_string3);
}

START_TEST(parameter_rest_callback_t)
{
    RestServer rs;
    init_server(&rs);
    add_url(&rs, "/api/:id", parameter_api_id_fun);
    String url = copy_chars("/api/123", strlen("/api/123"));
    ApiUrl* au = get_call_back(&rs, url);
    Request r;
    init_request(&r);
    r.uri = url;
    parse_paramas(&r, au);
    (au->callback)(NULL, &r);
    ck_assert_str_eq(global_parameter_string1.chars, "123");
    STRING_FREE(&url);
    STRING_FREE(&global_parameter_string1);
    add_url(&rs, "/api/:id/:pos", parameter_api_id_pos_fun);
    url = copy_chars("/api/321/name", strlen("/api/321/name"));
    au = get_call_back(&rs, url);
    r.uri = url;
    parse_paramas(&r, au);
    (au->callback)(NULL, &r);
    ck_assert_str_eq(global_parameter_string1.chars, "321");
    ck_assert_str_eq(global_parameter_string2.chars, "name");
    STRING_FREE(&url);
    STRING_FREE(&global_parameter_string1);
    STRING_FREE(&global_parameter_string2);
    add_url(&rs, "/endpoint/:id/:pos/:end", parameter_endpoint_id_fun);
    url = copy_chars("/endpoint/name/end/len", strlen("/endpoint/name/end/len"));
    au = get_call_back(&rs, url);
    r.uri = url;
    parse_paramas(&r, au);
    (au->callback)(NULL, &r);
    ck_assert_str_eq(global_parameter_string1.chars, "name");
    ck_assert_str_eq(global_parameter_string2.chars, "end");
    ck_assert_str_eq(global_parameter_string3.chars, "len");
    STRING_FREE(&url);
    STRING_FREE(&global_parameter_string1);
    STRING_FREE(&global_parameter_string2);
    STRING_FREE(&global_parameter_string3);
}
END_TEST

Suite* server_suite()
{
    Suite* s;
    TCase* tc_core;

    s = suite_create("SERVER");

    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, simple_rest_callback_t);
    tcase_add_test(tc_core, stress_rest_callback_t);
    tcase_add_test(tc_core, parameter_rest_callback_t);

    suite_add_tcase(s, tc_core);

    return s;
}

int main()
{
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = server_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
