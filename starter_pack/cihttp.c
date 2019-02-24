#include "cihttp.h"

typedef int bool;
enum { false, true };
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
    //printf("\nADDING %s %s", header->name, header->value);
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
     //READS THROUGH ENTIRE STRING
    
    bool methodAssigned = false;
    int i = 0;
    //printf("%s\n", methodRequestVersion);
    char *token;
    token = strtok(data, " \r\n");

    //HANDLE METHOD
    
    for(i = 0; !methodAssigned && i < 3; i++)
    {
        if(strcmp(token, implemented_methods[i]) == 0)
        {
            methodAssigned = true;
            req->method = i;
        }
    }
    if(!methodAssigned)
    {
        //THROW EXCEPTION HERE WHEN IMPLEMENTED
        printf("ERROR: Method not recognized\n");
        return NULL;
    }
    //HANDLE REQUEST
    token = strtok(NULL, " \n");
    //printf("%s", methodRequestVersion);
    req->request_uri = token;

    //HANDLE VERSION
    char Version[20];
    token = strtok(NULL," \n");
    strcpy(Version, token);
    //char *temp = Version[5];
    for(i = 0; Version[i+5] != '\0'; i++)
        Version[i] = Version[i+5];
    Version[i] = '\0';
    req->version = atof(Version);
    token = strtok(NULL, "\r\n");

    //HANDLING THE REST
    /*
    char temp1[256];
    char temp2[256];
    //if(strstr(token, "Host:"))
    //    printf("TESTING!\n");
    strcpy(temp1, token);
    //printf("%s == %s --- ", token, temp1);
    token = strtok(NULL, "\r\n");
    strcpy(temp2, token);
    //printf("%s == %s --- ", token, temp2);
    //printf("%s && %s\n", temp1,temp2);
    //printf("%s && %s\n", temp1,temp2);
    AddHeaderToRequest(req, temp1, temp2);
    //token = strtok(NULL, "\r\n");
    token = strtok(NULL, " \n");
    */
    while(token != NULL)
    {
        char *temp1a;
        char *temp2a;
        //printf("NAME: %s -- ", token);
        temp1a = (char*) malloc(strlen(token) * sizeof(char));
        strcpy(temp1a, token);
        token = strtok(NULL, "\r\n");
        if(token != NULL)
            {
                temp2a = (char*) malloc(strlen(token) * sizeof(char));
                //printf(" VALUE %s\n", token);
                strcpy(temp2a, token);
                AddHeaderToRequest(req, temp1a, temp2a);
                token = strtok(NULL, " \n");
            }
    }

    //DO NOT DELETE
    /*while(token != NULL)
    {
        printf("%s\n", token);
        token = strtok(NULL, "\r\n");
    }*/

    //printf("TESTING\n%s\n", token);

    /*
    //THIS STUFF WORKS FOR NOW
    token = strtok(data, " \n");
    //HANDLE METHOD

    //char temp[strlen(token)];
    //for(i = 0; i < strlen(token); i++)
        //temp[i] = token[i];

    //char *methodRequestVersion = strtok(temp, " ");
    
    for(i = 0; !methodAssigned && i < 3; i++)
    {
        if(strcmp(token, implemented_methods[i]) == 0)
        {
            methodAssigned = true;
            req->method = i;
        }
    }
    if(!methodAssigned)
    {
        //THROW EXCEPTION HERE WHEN IMPLEMENTED
        printf("ERROR: Method not recognized\n");
        return NULL;
    }
    //HANDLE REQUEST
    token = strtok(NULL, " \n");
    //printf("%s", methodRequestVersion);
    req->request_uri = token;


    //HANDLE VERSION
    char Version[20];
    token = strtok(NULL," \n");
    strcpy(Version, token);
    //char *temp = Version[5];
    for(i = 0; Version[i+5] != '\0'; i++)
        Version[i] = Version[i+5];
    Version[i] = '\0';
    req->version = atof(Version);
    token = strtok(NULL," \n");
   
    printf("REMAINING INFOMRATION\n");
    while(token != NULL)
    {   
        //printf("%c\n", token[strlen(token)-1]);
        if(token[strlen(token) -1] == ':')
            //printf("Start Header:\n");
        printf("%s\n", token);
        token = strtok(NULL, " \n");
    }*/
    return req;
}

void PrintHTTPRequest(struct httprequest *req)
{
    // TODO: incomplete
    //printf("\n--HTTP REQUEST DATA--\n");
    printf("HTTP %.2f %s Request: \n", req->version,  implemented_methods[req->method]);
    printf("Request-URI: %s\n", req->request_uri);
    NVL *headers = req->headers;
    while(headers != NULL)
    {
        printf("        %s %s\n", headers->name, headers->value);
        headers = headers->next;
    }
    /*
    //THE FOLLOWING IS FOR TESTING INDIVIDUAL HTTP REQUESTS
    char *temp;
    scanf("%s", temp);
    */
    //printf("\n")
    
}

struct httpresponse *GenerateHTTPResponse(struct httprequest *req)
{
    struct httpresponse *res = malloc(sizeof(struct httpresponse));
    // TODO: incomplete
    FILE *fptr;
    char *fileName;
    fileName = malloc(sizeof(req->request_uri) + sizeof("./www"));
    strcpy(fileName, "./www");
    strcat(fileName, req->request_uri);
    fptr = fopen(fileName, "r");
    if(fptr == NULL)
    {
        printf("ERROR OPENING %s\n", fileName);
    }
    fclose(fptr);

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