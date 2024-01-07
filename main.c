#include "main.h"

int checkResponse(const int socket, char* buf) {
    char byte;
    int i = 0;
    int code;
    char message[300];
    while(1) {
        i = 0;
        memset(buf, 0, 300); // clear buf
        
        do {
            read(socket, &byte, 1);
            if(byte != '\n') {
                buf[i++] = byte;
            }
        } while (byte != '\n'); // read the whole server response into buf
        
        printf("%s\n", buf);
        if((sscanf(buf, "%d-%[^\n]", &code, message) != 2)) {
            break;
        }
    }

    return code; // return the response code
}

int connectToServer(const int socket, const char* user, const char* password) {
    char user_input[5+strlen(user)+1];
    sprintf(user_input, "USER %s\r\n", user); // user_input = "USER <user>"

    char password_input[5+strlen(password)+1];
    sprintf(password_input, "PASS %s\r\n", password); // password_input = "PASS <password>"
    
    char response[300];
    
    printf("%s", user_input);
    write(socket, user_input, strlen(user_input)); // send "USER <user>" message to server
    
    /* after sending USER message, read server's response;
    if server responds with a code other than 331, end execution */
    if (checkResponse(socket, response) != 331) {
        printf("Unable to authenticate user '%s', server responded with code %s.\n", user, response);
        exit(-1);
    }
    printf("%s", password_input);
    write(socket, password_input, strlen(password_input)); // send "PASS <password>" message to server
    
    /* after sending PASS message, read (and return) server's response */
    return checkResponse(socket, response);
}

int enterPassiveMode(const int socket, char* ip_address, int* port) {
    char response[300];
    int ip_byte1, ip_byte2, ip_byte3, ip_byte4, port_byte1, port_byte2;

    write(socket, "pasv\r\n", 6); // send "pasv" message to server
    printf("pasv\n");

    /* after sending "pasv" message, read server's response;
    if server responds with a code other than 227, end execution */
    if (checkResponse(socket, response) != 227){
        return -1;
    }

    // obtain values for the IP and Port bytes, and use them to set the values of the ip_address and port arguments
    sscanf(response, "%*[^(](%d,%d,%d,%d,%d,%d)%*[^\n$)]", &ip_byte1, &ip_byte2, &ip_byte3, &ip_byte4, &port_byte1, &port_byte2);
    *port = (port_byte1 * 256) + port_byte2;
    sprintf(ip_address, "%d.%d.%d.%d", ip_byte1, ip_byte2, ip_byte3, ip_byte4);
    return 227;
}

int requestFile(const int socket, char* path_to_file) {
    char file_input[5+strlen(path_to_file)+1];
    char response[300];
    sprintf(file_input, "retr %s\r\n", path_to_file); // file_input = "retr <path>"
    printf("%s", file_input);
    write(socket, file_input, strlen(file_input)); // write "retr <path>" message to server

    /* after sending "retr" message, read and return server's response */
    return checkResponse(socket, response);
}

int main(int argc, char* argv[]) {
    if (argc > 2) { // wrong usage of command
        printf("ERROR: usage is %s ftp://[<user>:<password>@]<host>/<url-path>", argv[0]);
        exit(-1);
    }

    char user[300]; // username for authentication
    char password[300]; // password for authentication
    char host[300]; // connection host
    char path[300]; // path to file
    char file_to_download[300]; // name of the file to download
    char ip[300]; // IP address of the server

    regex_t regex;
    regcomp(&regex, "/", 0);
    
    if(regexec(&regex, argv[1], 0, NULL, 0)) {
        return -1;
    }

    regcomp(&regex, "@", 0);

    if (regexec(&regex, argv[1], 0, NULL, 0) != 0) { // there is no '@' in the provided argument; it is of type ftp://<host>/<url-path>
        sscanf(argv[1], "ftp://%300[^/]", host);
        strcpy(user, "anonymous");
        strcpy(password, "anonymous");

    }
    else { // there is a '@' in the provided argument; it is of type ftp://<user>:<password>@<host>/<url-path>
        sscanf(argv[1], "ftp://%300[^:]:%300[^@]@%300[^/]", user, password, host);
    }

    sscanf(argv[1], "ftp://%*[^/]/%300s", path); // obtain path to file
    strcpy(file_to_download, strrchr(argv[1], '/') + 1); // obtain file name

    (void) getIP(host, ip); // set ip's value to the IP address of the host

    printf("Hostname: %s\nPath: %s\nFile to Download: %s\nUser: %s\nPassword: %s\nIP: %s\n", host, path, file_to_download, user, password, ip);

    char response_buf[300];

    int socket_A = clientTCP(ip, 21); // set socket_A to be the file descriptor of port 21 on the IP address of the host
    int socket_A_val = socket_A;

    if(socket_A < 0 || checkResponse(socket_A, response_buf) != 220) { // if server does not respond with code 220, something went wrong
        printf("Error connecting to port 21.\n");
        exit(-1);
    }

    /* try to connect to the socket using the credentials obtained above;
    if the server responds with a code other than 230, end execution */
    if(connectToServer(socket_A, user, password) != 230) {
        printf("Error authenticating.\n");
        exit(-1);
    }

    uint16_t download_port;
    char download_ip[300];


    /* enter Passive Mode on socket_A, and store the IP address and Port for the connection to socket_B
    in download_ip and download_port, respectively;
    if the server responds with a code other than 227, end execution */
    if (enterPassiveMode(socket_A, download_ip, &download_port) != 227) { 
        printf("Couldn't enter passive mode.\n");
        exit(-1);
    }

    printf("downloading from port %d on address %s\n", download_port, download_ip);
    socket_A = socket_A_val;

    int socket_B = clientTCP(download_ip, download_port); // set socket_B as the file descriptor of port <download_port> on the IP address <download_ip>
    if (socket_B < 0) {
        printf("Error connecting to port %d\n", download_port);
        exit(-1);
    }
    
    /* send "retr" request to socket_A, requesting the file present in path;
    if the server responds with a code other than 150, end execution */
    int requestFileResponse = requestFile(socket_A, path);
    if (requestFileResponse!=150 && requestFileResponse!=125) {
        printf("Couldn't find requested file in the port.\n");
        exit(-1);
    }

    FILE* fd = fopen(file_to_download, "w"); // open the file <file_to_download> with "w" so we can write to it
    if (fd == NULL) {
        printf("Couldn't open the requested file.\n");
        exit(-1);
    }

    char data_buffer[300];
    int bytes;

    do {
        bytes = read(socket_B, data_buffer, 300); // read <=300 bytes each time from socket_B
        if (fwrite(data_buffer, bytes, 1, fd) < 0) { // write those bytes to the opened file
            return -1;
        }
        printf("wrote %d bytes to file %s\n", bytes, file_to_download);
    } while (bytes);

    fclose(fd); // after we finish writing to the file, close it

    /* after the file is fully downloaded, we must check the message from the server;
    if the server responds with a code other than 226, end execution */
    if (checkResponse(socket_A, response_buf) != 226) {
        printf("Error transfering file.\n");
        exit(-1);
    }

    write(socket_A, "quit\r\n", 7); // send "quit" message to socket_A in order to close the connection
    printf("quit\n");

    /* after sending "quit" message, check for the server's response;
    if the server responds with a code other than 221, end execution */
    if(checkResponse(socket_A, response_buf) != 221) {
        return -1;
    }
    
    /* close the sockets */
    if(close(socket_A)!=0 || close(socket_B)!=0) {
        printf("Error: Couldn't close sockets.\n");
        exit(-1);
    }

    printf("SUCCESS\n");
    return 0;
}
