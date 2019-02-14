#include "cihttp.h"

void *GetInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void AddHeaderToRequest(struct httprequest *req, char *name, char *value)
{
    NVL *header = malloc(sizeof(NVL));
    header->name = name;
    header->value = value;
    if (req->headers) 
        header->next = req->headers;
    else
        header->next = NULL;
    req->headers = header;
}

void AddHeaderToResponse(struct httpresponse *res, char *name, char *value)
{
    NVL *header = malloc(sizeof(NVL));
    header->name = name;
    header->value = value;
    if (res->headers)
        header->next = res->headers;
    else
        header->next = NULL;
    res->headers = header;
}

struct httprequest *ParseHTTPRrequest(char *data)
{
    struct httprequest *req;
    req = malloc(sizeof(struct httprequest));

    // TODO: incomplete
    //printf("%s", data);
    char* token = strtok(data, " ");

    //READS THROUGH ENTIRE STRING
    /*while(token != NULL)
    {
        printf("%s", token);
        token = strtok(NULL, " ");
    }*/
    return req;
}

void PrintHTTPRequest(struct httprequest *req)
{
    // TODO: incomplete
}

struct httpresponse *GenerateHTTPResponse(struct httprequest *req)
{
    struct httpresponse *res = malloc(sizeof(struct httpresponse));

    // TODO: incomplete

    return res;
}

void SendHTTPResponse(struct httpresponse *res, int cfd)
{
    // TODO: incomplete
}

int main(int argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    struct sockaddr_storage their_addr;
    int sfd, optval, res, cfd;
    socklen_t sin_size;
    char ip[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    res = getaddrinfo(NULL, PORT, &hints, &result);
    if (res != 0)
    {
        fprintf(stderr, "getaddrinfo() error: %s\n", strerror(errno));
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
        {
            fprintf(stderr, "socket() error: %s\n", strerror(errno));
            continue;
        }

        optval = 1;
        res = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        if (res != 0)
        {
            close(sfd);
            fprintf(stderr, "setsockopt() error: %s\n", strerror(errno));
            exit(1);
        }

        res = bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if (res != 0)
        {
            close(sfd);
            fprintf(stderr, "bind() error: %s\n", strerror(errno));
            continue;
        }

        break;
    }

    freeaddrinfo(result);

    if (rp == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    res = listen(sfd, BACKLOG);
    if (res != 0)
    {
        fprintf(stderr, "listen() error: %s\n", strerror(errno));
        exit(1);
    }

    for (;;)
    {
        sin_size = sizeof their_addr;
        cfd = accept(sfd, (struct sockaddr *)&their_addr, &sin_size);
        if (cfd == -1)
        {
            fprintf(stderr, "accept() error: %s\n", strerror(errno));
            continue;
        }

        switch (fork())
        {
        case -1: // error
            fprintf(stderr, "fork() can't create child: %s\n", strerror(errno));
            close(cfd);
            break;
        case 0: // child
        {
            char *data = malloc(MAX_LEN * sizeof(char));
            int res;

            // child doesn't need listening socket
            close(sfd);

            inet_ntop(their_addr.ss_family, GetInAddr((struct sockaddr *)&their_addr),
                      ip, sizeof(ip));
            printf("server: got a connection from %s\n", ip);

            res = recv(cfd, data, MAX_LEN, 0);
            if (res == 0 || res == -1)
            {
                fprintf(stderr, "server: recv'd no bytes (%s)\n", strerror(errno));
                return 0;
            }

            struct httprequest *req = ParseHTTPRrequest(data);
            PrintHTTPRequest(req);
            struct httpresponse *response = GenerateHTTPResponse(req);
            SendHTTPResponse(response, cfd);

            free(req);
            free(response);
            free(data);

            exit(0);
            break;
        }
        default: // parent
            // parent doesn't need client socket
            close(cfd);
            break;
        }
    }
}