#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>  
#include <thread>
#include <mutex>
#include <semaphore.h>



#define PORT 60016

using namespace std;

string parseHTTPGETHeader(string input);
string parseCharArray(char* input, int size);
void buildResponse(string fileToSend, int socket_fd);

int main(void)
{
    cout << "Initializing server..." << endl;

    int listen_Socket; //server listen socket descriptor
    int new_socket; //socket descriptor for the new socket that will be created for each accepted connection request
    struct sockaddr_in serverAddr; //server address
    struct sockaddr_in clientAddr; //client address
    
    char buffer [2048] = {0}; //data buffer
    char * ptr = buffer; //pointer to buffer
    
    //buffer variables necessary for sending/recieving data sent in multiple chunks
    //int len = 0; //num bytes to send/recieve
    //int maxLen = sizeof(buffer); //max number of bytes to recieve
    //int n = 0; //num bytes in each recv call

    int waitSize = 16; //max num of waiting clients

    int clientAddrLen = sizeof(clientAddr);//length of client address

    
    //create local socket address for server
    cout << "Creating local socket address..." << endl;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //default IP
    serverAddr.sin_port = htons(PORT); //port


    //create listen socket
    cout << "Creating a listen socket..." << endl;
    if((listen_Socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR: Listen socket failed!");
        exit(1);
    }

    
    //bind and listen to socket
    cout << "Bind and listen to socket..." << endl;
    if(bind(listen_Socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("ERROR: Binding failed!");
        exit(1);
    }

    
    //listen for connection requests
    cout << "Listening for connection requests..." << endl;
    if(listen(listen_Socket, waitSize) < 0){
        perror("ERROR: Listening failed!");
        exit(1);
    }


    //handle connection
    cout << "Begin connection handling..." << endl;
    while(1){

        //accept connections from client
        cout << "Accepting a new connection on listening socket..." << endl;
        new_socket = accept(listen_Socket, (struct sockaddr *)&clientAddr, (socklen_t*)&clientAddrLen);
        if(new_socket < 0){
            perror("ERROR: accepting failed!");
            exit(0);
        }

        //transfer data between client and server
        cout << "Reading data from client into buffer..." << endl;
        int n = read(new_socket, buffer, sizeof(buffer));
        cout << n << " bytes read" << endl;
        //code for filling buffer when multiple calls to recv are necessary
        // while((n = recv(new_socket, ptr, maxLen, 0)) > 0){
        //     cout << n << " bytes read, total: " << numn << endl;
        //     ptr += n; //ptr moves to point to next bytes to recieve
        //     maxLen -= n; //decrease # of bytes we still need to recieve
        //     len += n; //increase # of bytes recieved
        // }

        cout << "\n-------------------------{Begin Data Recieved}-------------------------"<< endl;
        cout << buffer << endl; //print buffer contents
        cout << "-------------------------{End Data Recieved}-------------------------\n"<< endl;


        //parse the file name from the GET request
        cout << "Getting name of requested file..." << endl;
        string requestedFile = parseHTTPGETHeader(parseCharArray(buffer, sizeof(buffer)));//search in current directory
        cout << "File name requested: " << requestedFile << endl;


        //use the parsed filename to open and read the file
        cout << "Getting the contents of file: " << requestedFile << endl;
        cout << "Building response and sending to server..." << endl;


        cout << "\n-------------------------{Begin Server Response}-------------------------"<< endl;
        buildResponse(requestedFile, new_socket);   
        cout << "-------------------------{End Server Response}-------------------------\n"<< endl;


        //write(new_socket, buffer, strlen(buffer)); //debug
        cout << "Closing socket..." << endl;
        close(new_socket); //close the socket
    }
    return 0;
}

//helper functions
//char* -> string
//converts a char array to a string and then returns that string
//status: complete
string parseCharArray(char* input, int size)
{
    if(size <= 0){
        cout << "ERROR: Char array parse error" << endl;
        return "ERR";
    }

    string result = "";
    for(int i = 0; i < size; i++){
        result = result + input[i]; 
    }

    return result;
}

//parse the HTTP headers to get name of requested file
//returns a string with the filename 
//status: complete
string parseHTTPGETHeader(string input){
    istringstream inSS(input);
    string H1, H2, H3;

    inSS >> H1 >> H2 >> H3;

    if(H1 == "GET"){
        if(H2[2] == '\0' or H2[2] == ' '){
            cout << "ERROR: NO FILE SPECIFIED IN HEADER" << endl;
            return "ERR";
        }
        else {
            return H2;
        }
    } else {
        cout << "ERROR: Parse HTTP header error!" << endl;
        cout << "Client asked for: " << H1 << " instead of GET" << endl;
        return "ERR";
    }
}

//opens the requested file and assembles the HTTP response before sending it to the client
//parameters are the name of the file to send and the socket file descriptor
void buildResponse(string fileToSend, int socket_fd){
    static char buffer[2048] = {0};
    int FD; //file descriptor for requested file
    int Bytes; //int representing the number of bytes read from the requested file
    if((FD = open(("."+fileToSend).c_str() , O_RDONLY)) == -1){
        cout << "ERROR: " << fileToSend << " cannot be opened or may not exist!!!" << endl;
        cout << "Server will respond with 404" << endl;
        sprintf(buffer,"HTTP/1.1 404 NOT_FOUND\r\n\r\n404: File couldn't be opened or may not exist."); //Send a 404 because file couldn't be opened
    } else {
        sprintf(buffer,"HTTP/1.1 200 OK\r\n\r\n"); //write the HTTP response to the buffer before send
    }
    
    fputs(buffer, stdout);//(buffer); //print the buffer containing the response to the terminal
    write(socket_fd,buffer,strlen(buffer)); //send the HTTP response to the client(body sent separately)
    
    if(FD != -1){
        //send file in chunks until EOF or Error
        while ((Bytes = read(FD, buffer, 2048)) > 0 ) {
            write(socket_fd, buffer, Bytes);
            fputs(buffer, stdout);
        }
        fputs("\n", stdout);
    }
}
