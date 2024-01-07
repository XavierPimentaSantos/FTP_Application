#include "clientTCP.h"
#include "getIP.h"
#include <regex.h>

int checkResponse(const int socket, char* buf);

int connectToServer(const int socket, const char* user, const char* password);

int enterPassiveMode(const int socket, char* ip_address, int* port);

int main(int argc, char* argv[]);