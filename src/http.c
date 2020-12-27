
/**
 * <netinet/in.h>
 *
 * defines:
 * IPPROTO_TCP
 *
 * funcitons:
 * htons(), htonl(), send()
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
 * memset(), strcpy(), strlen()
 */
#include <string.h>
/**
 * <string.h>
 *
 * functions:
 * close(), read()
 */
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "http.h"
#include "options.h"
#include "server.h"
#include "socketcon.h"

#define SERVER_STR "Server: webasmhttpd/0.0.1\r\n"

static Filetype parse_filetype(const char* filepath)
{
    if (strncmp(filepath + (strlen(filepath)) - 3, ".js", 3) == 0)
        return FILE_JS;
    else if (strncmp(filepath + (strlen(filepath)) - 4, ".css", 4) == 0)
        return FILE_CSS;
    else if (strncmp(filepath + (strlen(filepath)) - 5, ".html", 5) == 0)
        return FILE_HTML;

    return FILE_DEFAULT;
}

static void send_filetype(Response* r, Filetype type)
{
    char buf[128];

    switch (type) {
    case FILE_HTML:
        strcpy(buf, "Content-Type: text/html\r\n");
        break;
    case FILE_CSS:
        strcpy(buf, "Content-Type: text/css\r\n");
        break;
    case FILE_JS:
        strcpy(buf, "Content-Type: text/javascript\r\n");
        break;
    default:
        // TODO: figure out better default content type
        strcpy(buf, "Content-Type: text/html\r\n");
        break;
    }

    send(r->conn.conn_fd, buf, strlen(buf), 0);
}

/**
 * @brief send 404 header to client
 *
 * @param conn Connection struct
 */
void http_404(Response* r)
{
    char buf[1024];

    //TODO: use filename to determine the Content-Type

    strcpy(buf, "HTTP/1.0 404 Not Found\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STR);
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
}

void http_200(Response* r, Filetype type)
{
    char buf[256];

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STR);
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    send_filetype(r, type);
    strcpy(buf, "\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
}

void send_json(Response* r, JSONObject obj)
{
    char buf[256];

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STR);
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: application/json\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    String str = json_to_string(obj);
    send(r->conn.conn_fd, str.chars, str.len, 0);
    STRING_FREE(&str);
}

void send_file(Response* r, const char* filepath)
{
    int fd;
    // open the files relative to server
    if (filepath[0] == '/')
        fd = open(filepath + 1, O_RDONLY);
    else
        fd = open(filepath, O_RDONLY);
    // File not found
    if (fd == -1) {
        http_404(r);
        return;
    }

    struct stat st;
    fstat(fd, &st);
    char* file_content = ALLOCATE(char, st.st_size);
    read(fd, file_content, st.st_size);

    Filetype ftype = parse_filetype(filepath);
    http_200(r, ftype);

    send(r->conn.conn_fd, file_content, st.st_size, 0);
    free(file_content);
    close(fd);
}

void* accept_client(void* clientptr)
{
    Connection conn;
    conn.conn_fd = *((int*)clientptr);
    Response resp;
    resp.conn = conn;
    Request r;
    init_request(&r);
    parse_request(&r, &conn);
    ApiUrl* au = get_call_back(&__rs, r.uri);
    if (au != NULL) {
        parse_paramas(&r, au);
        (au->callback)(&resp, &r);
    } else {
        send_file(&resp, r.uri.chars);
    }
    if (_server_option_verbose_output)
        printf("request handled\n");
    close(conn.conn_fd);
    free_request(&r);

    return NULL;
}
