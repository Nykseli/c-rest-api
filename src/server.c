
/**
 * <sys/socket.h>
 *
 * defines:
 * PF_INET, SOCK_STREAM
 * AF_INET
 *
 * functions:
 * socket(), bind(), lisetn
 */
#include <sys/socket.h>

/**
 * <netinet/in.h>
 *
 * defines:
 * IPPROTO_TCP
 *
 * funcitons:
 * htons(), htonl()
 *
 * structs:
 * sockaddr_in
 */
#include <netinet/in.h>

/**
 * <stdint.h>
 *
 * structs:
 * uint16_t
 */
#include <stdint.h>

/**
 * <stdio.h>
 *
 * functions:
 * perror()
 */
#include <stdio.h>

/**
 * <stdlib.h>
 *
 * defines:
 * EXIT_FAILURE
 *
 * functions:
 * exit(), printf(), fgets(), feof(),
 * fclose()
 */
#include <stdlib.h>

/**
 * <string.h>
 *
 * functions:
 * memset(), strcpy()
 */
#include <string.h>
/**
 * <string.h>
 *
 * functions:
 * close(), read(), strlen()
 */
#include <unistd.h>

#include <pthread.h>

#include "http.h"
#include "options.h"
#include "requests/request.h"
#include "server.h"

RestServer __rs;
volatile int _server_option_verbose_output = 0;
volatile unsigned short _server_option_tcp_port = 8888;

void set_server_option_verbose_output()
{
    _server_option_verbose_output = 1;
}

void set_server_option_tcp_port_number(unsigned short port)
{
    _server_option_tcp_port = port;
}

typedef struct
{
    ApiUrl* urls;
    int urls_len;
    JSONObject* suburls;
} ApiTable;

static void init_apitable(ApiTable* at)
{
    at->urls = NULL;
    at->urls_len = 0;
    at->suburls = ALLOCATE(JSONObject, 1);
    init_table(at->suburls);
}

static String* parse_keywords(char* endpoint, int* length)
{
    String* kws = NULL;
    int e_len = (int)strlen(endpoint);
    int len = 0;
    int i, j;
    for (i = 0; i < e_len; i++) {
        if (endpoint[i] == ':') {
            // "Consume" the : character
            i++;
            len = 0;
            for (j = i; j < e_len; j++) {
                if (endpoint[j] == '/')
                    break;
                len++;
            }
            kws = GROW_ARRAY(kws, String, 0, *length + 1);
            kws[*length] = copy_chars(endpoint + i, len);
            *length = *length + 1;
            i += len;
        }
    }
    return kws;
}

static DataValue create_api_data(char* endpoint, RestCallback cb)
{
    ApiUrl au;
    au.callback = cb;
    au.kw_len = 0;
    au.keywords = parse_keywords(endpoint, &au.kw_len);

    DataValue val;
    val.type = TYPE_API_FUNCTION;
    AS_API_CALL(val) = au;
    return val;
}

static String endpoint_key(const char* uri, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        if (uri[i] == ':') {
            // Don't include the / or : chars
            i--;
            break;
        }
    }
    return copy_chars(uri, i);
}

/**
 * @brief Order endpoint Strings in ascending order.
 *
 * @param rs
 */
static void order_endpoints(RestServer* rs)
{
    String* tmp = NULL;
    //TODO: figure out a faster way
    for (int i = 0; i < rs->endpoint_len; i++) {
        for (int j = i + 1; j < rs->endpoint_len; j++) {
            if (rs->endpoints[i]->len > rs->endpoints[j]->len) {
                tmp = rs->endpoints[i];
                rs->endpoints[i] = rs->endpoints[j];
                rs->endpoints[j] = tmp;
            }
        }
    }
}

static String* get_endpoint_url(RestServer* rs, String* url)
{
    // First check if match without keyword parsing
    for (int i = 0; i < rs->endpoint_len; i++) {
        // Make sure endpoint and url are the same lenght first
        // because it's alot faster than comparing every string
        if (rs->endpoints[i]->len == url->len)
            if (strcmp(rs->endpoints[i]->chars, url->chars))
                return url;
    }

    // Check if there endpoint that is in url
    // Urls are ordered with longest last and we want to parse the longer ones first
    for (int i = rs->endpoint_len - 1; i >= 0; i--) {
        if (strstr(url->chars, rs->endpoints[i]->chars) != NULL)
            return rs->endpoints[i];
    }

    return NULL;
}

void init_server(RestServer* rss)
{
    rss->clients = NULL;
    rss->endpoints = NULL;
    rss->endpoint_len = 0;
    init_table(&rss->urls);
}

void free_server(RestServer* rss)
{
    if (rss->clients != NULL) {
        free(rss->clients);
    }
}

void parse_paramas(Request* r, ApiUrl* au)
{
    if (au->kw_len == 0)
        return;
    int pos = r->uri.len - 1;
    int i, j, len;
    r->params = ALLOCATE(JSONObject, 1);
    init_table(r->params);
    for (i = au->kw_len - 1; i >= 0; i--) {
        len = 0;
        for (j = pos; j >= 0; j--) {
            if (r->uri.chars[j] == '/')
                break;
            len++;
        }
        //TODO: use json_set when it gets implemented
        DataValue val;
        val.type = TYPE_STRING;
        AS_STRING(val) = copy_chars(r->uri.chars + j + 1, len);
        String keystr = copy_string(au->keywords[i]);
        table_set(r->params, &keystr, val);
        pos = j - 1;
    }
}

static String* split_url(char* endpoint, int* amount)
{
    String* val = NULL;
    int pos = 0;
    int len = 1;
    int endpoint_len = (int)strlen(endpoint);
    for (int i = 1; i < endpoint_len; i++) {
        if (endpoint[i] == '/') {
            String tmp = copy_chars(endpoint + pos, len);
            pos = len + pos;
            len = 0;
            val = GROW_ARRAY(val, String, 0, *amount + 1);
            val[*amount] = tmp;
            *amount += 1;
            if (endpoint[i + 1] == ':')
                return val;
        }
        len++;
    }
    String tmp = copy_chars(endpoint + pos, len);
    val = GROW_ARRAY(val, String, 0, *amount + 1);
    val[*amount] = tmp;
    *amount += 1;

    return val;
}

ApiUrl* get_call_back(RestServer* rs, String url)
{
    int len = 0;
    String* splits = split_url(url.chars, &len);
    Table* tmp_table = &rs->urls;
    ApiUrl* return_url = NULL;
    DataValue val;
    bool found = table_get(tmp_table, &splits[0], &val);
    if (!found || AS_VOID(val) == NULL)
        goto end;
    ApiTable* at = (ApiTable*)AS_VOID(val);
    for (int i = 0; i < len; i++) {
        if (i == len - 1) {
            if (at->urls[0].kw_len == 0)
                return_url = &at->urls[0];
        } else {
            if (at->suburls != NULL && at->suburls->count != 0) {
                // Test if there is a table for following keyword
                bool found = table_get(at->suburls, &splits[i + 1], &val);
                if (found && AS_VOID(val) != NULL) {
                    at = (ApiTable*)AS_VOID(val);
                    tmp_table = at->suburls;
                    continue;
                }
                // else try to find the callback apiurl
                for (int j = 0; j < at->urls_len; j++) {
                    if (at->urls[j].kw_len == (len - (i + 1))) {
                        return_url = &at->urls[j];
                    }
                }
                goto end;
            } else {
                for (int j = 0; j < at->urls_len; j++) {
                    if (at->urls[j].kw_len == (len - (i + 1))) {
                        return_url = &at->urls[j];
                    }
                }
                goto end;
            }
        }
    }

end:
    for (int i = 0; i < len; i++) {
        STRING_FREE(&splits[i]);
    }
    free(splits);
    return return_url;
}

void add_url(RestServer* rs, char* endpoint, RestCallback cb)
{
    //TODO: throw an error and close program if endpoint doesn't start with /
    //TODO: throw an error if the endpoint already exists;

    int len = 0;
    String* splits = split_url(endpoint, &len);
    Table* tmp_table = &rs->urls;
    for (int i = 0; i < len; i++) {
        if (i == len - 1) {
            DataValue val;
            bool found = table_get(tmp_table, &splits[i], &val);
            if (!found || AS_VOID(val) == NULL) {
                ApiTable* at = ALLOCATE(ApiTable, 1);
                init_apitable(at);
                at->urls = ALLOCATE(ApiUrl, 1);
                at->urls->kw_len = 0;
                at->urls->keywords = parse_keywords(endpoint, &at->urls->kw_len);
                at->urls->callback = cb;
                at->urls_len++;
                DataValue d_val;
                d_val.type = TYPE_API_FUNCTION;
                AS_VOID(d_val) = (void*)at;
                table_set(tmp_table, &splits[i], d_val);
            } else {
                ApiTable* at = (ApiTable*)AS_VOID(val);
                at->urls = GROW_ARRAY(at->urls, ApiUrl, 0, at->urls_len + 1);
                at->urls[at->urls_len].kw_len = 0;
                at->urls[at->urls_len].keywords = parse_keywords(endpoint, &at->urls[at->urls_len].kw_len);
                at->urls[at->urls_len].callback = cb;
                at->urls_len++;
            }
        } else {
            DataValue val;
            bool found = table_get(tmp_table, &splits[i], &val);
            if (!found || AS_VOID(val) == NULL) {
                ApiTable* at = ALLOCATE(ApiTable, 1);
                init_apitable(at);
                DataValue d_val;
                d_val.type = TYPE_API_FUNCTION;
                AS_VOID(d_val) = (void*)at;
                table_set(tmp_table, &splits[i], d_val);
                tmp_table = at->suburls;
            } else {
                tmp_table = ((ApiTable*)AS_VOID(val))->suburls;
            }
        }
    }
}

int run_server(RestServer* rs)
{
    __rs = *rs;
    order_endpoints(&__rs);
    if (_server_option_verbose_output) {
        for (int i = 0; i < __rs.endpoint_len; i++) {
            printf("Endpoint: %s\n", __rs.endpoints[i]->chars);
        }
    }
    // Ports range is 0-65535 (0x0000-0xffff)
    // which is exacly what uint16_t holds
    uint16_t port = _server_option_tcp_port;

    pthread_t thread;

    struct sockaddr_in sa;

    // Set ipv4 tcp socket with stream socket
    int socketfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketfd == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    // Try to reuse the port
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) == -1) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }

    // Initialize the sa struct with zeros (nulls) amount the struct size
    // TODO: do we really need this?
    memset(&sa, 0, sizeof(sa));

    // Set the socket addres to be ipv4
    sa.sin_family = AF_INET;
    // Set the socket port to port with correct byte order
    sa.sin_port = htons(port);
    // Set socket to accept any incoming messages
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socketfd file descriptor to local addres (lsof -i tcp)
    if (bind(socketfd, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
        perror("bind failed");
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    // Prepare to accept connections on socket FD.
    // This tries 10 times if connection is working and returns -1
    // if there is a problem
    if (listen(socketfd, 10) == -1) {
        perror("listen failed");
        close(socketfd);
        exit(EXIT_FAILURE);
    }

    // Tcp connection loop
    for (;;) {
        // Wait for new connection and set the file descriptor for it
        int connectfd = accept(socketfd, NULL, NULL);

        // if file descriptor is -1, the kernel couldn't create new port
        // for the connection
        if (connectfd == -1) {
            perror("accept failed");
            close(socketfd);
            exit(EXIT_FAILURE);
        }

        // Handle each connection in a thread
        if (pthread_create(&thread, NULL, accept_client, &connectfd) != 0) {
            perror("thread failed");
            close(connectfd);
        }
    }

    close(socketfd);
    return EXIT_SUCCESS;
}
