#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include <string.h>

int getIP(const char* hostname, char* ip);