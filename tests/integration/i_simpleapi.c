
#include "../../src/http.h"
#include "../../src/server.h"
#include <string.h>

void simple_callback(Response* res, Request* req)
{
    char json[] = "{\"test\": \"callback\"}";
    JSONString jstring = copy_chars(json, strlen(json));
    JSONObject obj = parse_json(jstring, NULL);
    send_json(res, obj);
    free_json(&obj);
    STRING_FREE(&jstring);
}

void data_callback(Response* res, Request* req)
{
    bool success;
    JSONObject json_obj = parse_json(req->content, &success);
    char* json = NULL;
    if (success == false) {
        json = "{\"error\": \"Parse failed!\"}";
    } else {
        String tmp;
        json_get_string_c(&json_obj, "tdata", &tmp);
        if (strcmp(tmp.chars, "test1") == 0) {
            json = "{\"result\": \"test1\"}";
        } else if (strcmp(tmp.chars, "test2") == 0) {
            json = "{\"result\": \"test2\"}";
        } else {
            json = "{\"result\": \"not found\"}";
        }
        STRING_FREE(&tmp);
    }
    JSONString jstring = copy_chars(json, strlen(json));
    JSONObject obj = parse_json(jstring, NULL);
    send_json(res, obj);
    free_json(&obj);
    STRING_FREE(&jstring);
}

void parameter_callback(Response* res, Request* req)
{
    JSONString json_str;
    bool succes = json_get_string_c(req->params, "param", &json_str);
    char* json = NULL;
    if (succes == false) {
        json = "{\"error\": \"No 'param' parameter found!\"}";
    } else {
        if (strcmp(json_str.chars, "1") == 0) {
            json = "{\"result\": \"1\"}";
        } else if (strcmp(json_str.chars, "2") == 0) {
            json = "{\"result\": \"2\"}";
        } else {
            json = "{\"result\": \"not found\"}";
        }
    }
    JSONString jstring = copy_chars(json, strlen(json));
    JSONObject obj = parse_json(jstring, NULL);
    send_json(res, obj);
    free_json(&obj);
    STRING_FREE(&jstring);
}

void return_request_params(Response* res, Request* req)
{
    send_json(res, *req->params);
}

int main(int argc, char const* argv[])
{

    RestServer rs;
    init_server(&rs);
    add_url(&rs, "/", simple_callback);
    add_url(&rs, "/api", data_callback);
    add_url(&rs, "/parameter/:param", parameter_callback);
    add_url(&rs, "/req/:num/:id", return_request_params);
    return run_server(&rs);
}
