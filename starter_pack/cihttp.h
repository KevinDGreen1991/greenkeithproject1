#ifndef _CI_HTTP_H_
#define _CI_HTTP_H_

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <time.h>

#define PORT "9001"
// it's over 9000!
#define BACKLOG 10
#define MAX_LEN 1024
#define MAX_FILE_LEN 10240

typedef enum method
{
    GET,
    POST,
    HEAD
} METHOD;
char *implemented_methods[] = {"GET", "POST", "HEAD"};

// linked list
typedef struct name_value_list
{
    char *name;
    char *value;
    struct name_value_list *next;
} NVL;

struct httprequest
{
    METHOD method;
    char *request_uri;
    float version;

    NVL *headers;
    NVL *post_data;

    char *body;
};

struct httpresponse
{
    int status;
    float version;
    char *reason;

    NVL *headers;

    char *body;
};

void SendHTTPResponse(struct httpresponse *res, int cfd);
struct httpresponse *GenerateHTTPResponse(struct httprequest *req);
void PrintHTTPRequest(struct httprequest *req);
struct httprequest *ParseHTTPRrequest(char *data);
void AddHeaderToResponse(struct httpresponse *res, char *name, char *value);
void AddPostDataToRequest(struct httprequest *res, char *name, char *value);
void AddHeaderToRequest(struct httprequest *req, char *name, char *value);
void *GetInAddr(struct sockaddr *sa);

#endif