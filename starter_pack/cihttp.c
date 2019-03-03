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
    if (req->headers) 
        header->next = req->headers;
    else
        header->next = NULL;
    req->headers = header;
}
void AddPostDataToRequest(struct httprequest *req, char *name, char *value)
{
    NVL *header = malloc(sizeof(NVL));
    header->name = name;
    header->value = value;
    if (req->post_data) 
        header->next = req->post_data;
    else
        header->next = NULL;
    req->post_data = header;
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

    // TODO: Complete
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
    char *post;
    while(token != NULL)
    {
        char *temp1a;
        char *temp2a;
        //printf("NAME: %s -- ", token);
        temp1a = (char*) malloc(strlen(token) * sizeof(char));
        strcpy(temp1a, token);
        token = strtok(NULL, "\r\n");
        //printf("%s\n", token);
        if(token != NULL)
            {
                temp2a = (char*) malloc(strlen(token) * sizeof(char));
                //printf(" VALUE %s\n", token);
                strcpy(temp2a, token);
                if(req->method == POST && strcmp(temp1a, "\r") == 0)
                {
                    //HANDLE POST DATA HERE
                        //printf("HANDLE POST DATA HERE\n");
                        post = malloc(strlen(temp2a) * sizeof(char));
                        strcpy(post, temp2a);
                        char *dataToken;
                        dataToken = strtok(post, "=&\r\n");
                        while(dataToken != NULL)
                        {
                            char *temp1b;
                            char *temp2b;
                            temp1b = (char*) malloc(strlen(dataToken) * sizeof(char));
                            strcpy(temp1b, dataToken);
                            dataToken = strtok(NULL, "&\r\n");
                            temp2b = (char*) malloc(strlen(dataToken) * sizeof(char));
                            strcpy(temp2b, dataToken);
                            AddPostDataToRequest(req, temp1b, temp2b);
                            dataToken = strtok(NULL, "=");
                        }
                }
                else
                {
                    AddHeaderToRequest(req, temp1a, temp2a);
                    token = strtok(NULL, " \n");
                    post = temp2a;
                }
                //printf("%s\n", token);
            }
    }
    //printf("%s\n", )

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
    if(req->method == POST)
    {
        printf("\n");
        NVL *post = req->post_data;
        while(post != NULL)
        {
            if(post->next == NULL)
                printf("%s=%s", post->name, post->value);
            else
                printf("%s=%s&", post->name, post->value);
            post = post->next;
        }
    }
    /*
    //THE FOLLOWING IS FOR TESTING INDIVIDUAL HTTP REQUESTS
    char *temp;
    scanf("%s", temp);
    */
    printf("\n");
    
}

struct httpresponse *GenerateHTTPResponse(struct httprequest *req)
{
    struct httpresponse *res = malloc(sizeof(struct httpresponse));

    // TODO: incomplete

    //Grabs FILE's location (www/uri)
    FILE *fptr;
    char *fileName;
    //HANDLES POST response
    if(req->method == POST)
    {
            fileName = malloc(sizeof("./www/response.html"));
            strcpy(fileName, "./www/response.html");
    }
    //CHECKS FOR OTHER
    else{
    fileName = malloc(sizeof(req->request_uri) + sizeof("./www"));
    strcpy(fileName, "./www");
    strcat(fileName, req->request_uri);
    }
    //SUCCESFULLY FOUND FILE
    if((fptr = fopen(fileName, "rb")))
    {    
        //printf("SUCCESS OPENING %s", fileName);
        res->status = 200;
        res->version = req->version;
        res->reason = "OK\n";  
    }
    //OTHERWISE SEND 404
    else   
    {   
        printf("ERROR OPENING %s\n", fileName);
        res->status = 404;
        res->version = req->version;
        res->reason = "Not Found\n";
        //return res;
    }
    char *filebuffer;
    //IF NOT A 404
    if(res->status != 404)
    {
    fseek( fptr , 0L , SEEK_END);
    long filelength = ftell ( fptr );
    rewind( fptr );
    filebuffer = malloc(MAX_FILE_LEN);
    if(fread( filebuffer , filelength, 1 , fptr))
        //printf("%s", filebuffer);
    res->body = filebuffer;
    }
    //IF A 404
    else
    {
        if(fptr = fopen("./www/error.html", "rb"))
        {
            fseek( fptr , 0L , SEEK_END);
            long filelength = ftell ( fptr );
            rewind( fptr );
            filebuffer = malloc(MAX_FILE_LEN);
            if(fread( filebuffer , filelength, 1 , fptr))
                //printf("%s", filebuffer);
            res->body = filebuffer;
        }
        else
        {
            printf("ERROR OPENING 404\n");
        }
    }
    fclose(fptr);
    //CREATING DATA FOR RESPONSE
    char buf[40];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
        //printf("Time is: [%s]\n", buf);

    //CREATE DEFAULT EXPIRATION DATE
    char expire[40];
    strftime(expire, sizeof expire,"Mon, 1 Jan 2001 00:00:00 GMT", &tm);
    AddHeaderToResponse(res,"Expires", strcat(expire, "\n"));
    AddHeaderToResponse(res,"Date", strcat(buf, "\n"));

    //NO IDEA WHY BY YOU MUST READ THROUGH ALL HEADERS OR ELSE IT DOES NOT SEND PROPERLY
    while( res->headers->next != NULL )
    {
        //printf("\t%s: %s\n", res->headers->name, res->headers->value);
        res->headers = res->headers->next;
    }

    return res;
}

void SendHTTPResponse(struct httpresponse *res, int cfd)
{
    // TODO: Complete
    char *buffer;
    
    
    switch(res->status)
    {
        case 404:
            strcpy(buffer, "HTTP/1.1 404 Not Found\r\n");
            send(cfd, buffer, strlen(buffer), 0);

            break;
        default:
            strcpy(buffer, "HTTP/1.1 200 Ok\r\n");
            send(cfd, buffer, strlen(buffer), 0);
    }

    //HEADERS
    while(res->headers->next != NULL)
    {
        strcpy(buffer, strcat(res->headers->name, res->headers->value));
            //printf("%s\n", buffer);
    }

    //BODY
    strcpy(buffer, res->body);
    send(cfd, buffer, strlen(buffer), 0);
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
            //FREEING OF THE DATA RESULTS IN A STRANGE ERROR WHEN TESTING form.html
            //free(data);

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