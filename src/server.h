#ifndef REST_SERVER_H_
#define REST_SERVER_H_

#include "requests/request.h"
#include "utils/hashtable.h"

extern RestServer __rs;

void parse_paramas(Request* r, ApiUrl* au);
ApiUrl* get_call_back(RestServer* rs, String endpoint);
void add_url(RestServer* rs, char* endpoint, RestCallback cb);
void init_server(RestServer* rs);
int run_server(RestServer* rs);
void free_server(RestServer* rs);

void set_server_option_verbose_output();
void set_server_option_tcp_port_number(unsigned short port);

#endif
