#include <sys/stat.h>
#include <time.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    /*
    Use stat(), gmtime(), and strftime() to get the format you need. 
    This file is so you can test your time formatting outside of the server code 
    */
    char last_modified[64];
    printf("%s", last_modified);

    return 0;
}