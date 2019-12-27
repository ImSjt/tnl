#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>

int main()
{

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout<<sockfd<<std::endl;

    return 0;
}