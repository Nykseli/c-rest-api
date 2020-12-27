#ifndef REST_REQUEST_H_
#define REST_REQUEST_H_

#include "../datatypes.h"
#include "../socketcon.h"
#include "../utils/json.h"

void parse_request(Request* r, Connection* conn);
void init_request(Request* r);
void free_request(Request* r);
void print_request(Request* r);

#endif
