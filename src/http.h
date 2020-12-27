#ifndef REST_HTTP_H_
#define REST_HTTP_H_

#include "requests/request.h"
#include "utils/json.h"

typedef enum {
    FILE_DEFAULT,
    FILE_HTML,
    FILE_CSS,
    FILE_JS
} Filetype;

void http_404(Response* resp);
void http_200(Response* r, Filetype type);
void send_json(Response* resp, JSONObject obj);
/*
* Send a file content basend on the filetype (.html, .css, .js etc)
* Send 404 if file is not found
*/
void send_file(Response* r, const char* filepath);

void* accept_client(void* clientptr);

#endif
