#include <cpr/cpr.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <iostream>

//                      cpr::Authentication{"user", "pass"},
char cUrl[1024];
std::string myindex;
int temp1 = 17;
int temp2 = temp1;
int temp3 = temp1;

//Functions headers
int server();
int serverThreads();
int serverThreads2();


std::string convertToString(char* a, int size)
{
    int i;
    std::string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

std::string getDateTime() {
    char datetime[1024];
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    sprintf(datetime,"%04d%02d%02d%02d%02d%02d\n",now->tm_year + 1900,(now->tm_mon + 1),now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    //std::cout << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday << "\n";
    std::cout << datetime;
    //20210503111305
    std::string str;
    str = convertToString(datetime,14);
    return str;
}

void put()
{    
    char myjson[1024];
    sprintf(myjson,"{\"sensor1\":\"20.%02d\",\"sensor2\":\"21.%02d\",\"sensor3\":\"22.%02d\"}",temp1,temp2,temp3);   
    myindex =   getDateTime();
    sprintf(cUrl,"http://192.168.15.6:9200/temperatura/leitura/%s",myindex.c_str());
    cpr::Response r = cpr::Post(cpr::Url{cUrl}, cpr::Header{{"Content-Type", "application/json"}},cpr::Body{myjson},cpr::ConnectTimeout{3000}, cpr::Timeout{3000} );
    r.status_code;                  // 200
    r.header["content-type"];       // application/json; charset=utf-8
    r.text;                         // JSON text string
    
    if( r.status_code == 200 || r.status_code == 201 ){
        printf("Success!!! [%ld]\n",r.status_code);
        printf("r.text[%s]\n",r.text.c_str());
    }else{
        printf("Error [%ld]\n",r.status_code);
    }
}

int main(int argc, char** argv) 
{
    getDateTime();
    put();
    usleep(490000);
    sprintf(cUrl,"http://192.168.15.6:9200/temperatura/_search?q=_id:%s",myindex.c_str());
    cpr::Response r = cpr::Get(cpr::Url{cUrl});
    r.status_code;                  // 200
    r.header["content-type"];       // application/json; charset=utf-8
    r.text;                         // JSON text string
    
    if( r.status_code == 200 ){
        printf("Success!!! [%ld]\n",r.status_code);
        printf("r.text[%s]\n",r.text.c_str());
    }else{
        printf("Error [%ld]\n",r.status_code);
    }
    serverThreads2();
} 


//**********************************************************************************************************************************************
/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc server.c -lpthread -o server
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

#define CT_NOTYPE        0      //No type 
#define CT_ECHO        1000     //Client Type echo 
#define CT_TEMPERATURA 1001     //Client Type TEMPERATURA JSON
#define CT_TIME        1002     //Client type text echo time

typedef struct {
    int client;
    int ihandler;
    int type;
    char buffer[1024];    
} CLIENT_TYPE;

//the thread function
void *connection_handler(void *);
void *connection_handler2(void *);
int fim=0;
CLIENT_TYPE stClients[100];
int handlerPointer=0;

int getHandlerSocket(int client )
{
    return stClients[client].ihandler;
}

void setHandlerSocket(int client, int data)
{
    printf("Recording sock #%04d\n",data);
    stClients[client].ihandler=data;
}

void closeSocketBysocket(int sock)
{
    int i;
    for(i=0;i<handlerPointer;i++){
        if( stClients[i].ihandler == sock){
            printf("closeSocketBysocket:Closing sock #%04d\n",sock);
            close(sock);
            stClients[i].ihandler=0;
        }
    }
    
}

void closeSocket(int client)
{
    if ( stClients[client].ihandler != 0 ){
        printf("closeSocket:Closing sock #%04d\n",stClients[client].ihandler);
        close(stClients[client].ihandler);
        stClients[client].ihandler=0;
    }
}

void closeAllHandlerSocket()
{
    int i;
    for(i=0;i<handlerPointer;i++){
        closeSocket(i);
    }
}

void listAllHandlerSocket()
{
    int i;
    printf("Tabela de sockets ******************************\n");
    for(i=0;i<handlerPointer;i++){
        printf("[%04d]",stClients[i].ihandler);;
    }
    printf("\nTabela de sockets ******************************\n");
}

void put(std::string sindex, std::string stype, std::string sid, std::string sjson)
{    
    //char myjson[1024];
    //sprintf(myjson,"{\"sensor1\":\"20.%02d\",\"sensor2\":\"21.%02d\",\"sensor3\":\"22.%02d\"}",temp1,temp2,temp3);   
    //myindex =   getDateTime();
    sprintf(cUrl,"http://192.168.15.6:9200/%s/%s/%s",sindex.c_str(),stype.c_str(),sid.c_str());
    cpr::Response r = cpr::Post(cpr::Url{cUrl}, cpr::Header{{"Content-Type", "application/json"}},cpr::Body{sjson},cpr::ConnectTimeout{3000}, cpr::Timeout{3000} );
    r.status_code;                  // 200
    r.header["content-type"];       // application/json; charset=utf-8
    r.text;                         // JSON text string
    
    if( r.status_code == 200 || r.status_code == 201 ){
        printf("Success!!! [%ld]\n",r.status_code);
        printf("r.text[%s]\n",r.text.c_str());
    }else{
        printf("Error [%ld]\n",r.status_code);
    }
}

int serverThreads2()
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     int reuse = 1;
     int client_number=0;
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 5038 );
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEADDR) failed");
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
         
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) && fim == 0)
    {
        stClients[client_number].ihandler = client_sock;
        puts("Connection accepted");

        if( pthread_create( &thread_id , NULL ,  connection_handler2 , (void*) &stClients[client_number]) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        client_number++; 
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");
    }
    printf("serverThreads: Closing sock #%04d\n",client_sock);
    close(client_sock);
    close(socket_desc); 
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    printf("serverThreads: Server is finishing...\n"); 
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler2(void *voidclient)
{
    //Get the socket descriptor
    CLIENT_TYPE *client = (CLIENT_TYPE *)voidclient;
    int sock = client->ihandler;
    int read_size;
    char *message , client_message[2000];
    std::string str; 
    //Send some messages to the client
    
    message = "?TYPE\n";
    write(sock , message , strlen(message));

    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Defining client type...
		client_message[read_size] = '\0';
		str = convertToString(client_message,strlen(client_message));
        std::cout << "String contem... "<<str;// << "\n";
        if ( (str.compare(0,4,"1000") == 0) || (str.compare(0,4,"echo") == 0) ||
                    (str.compare(0,4,"ECHO") == 0)   ){   
            client->type = CT_ECHO;
            std::cout << "Type JSON temperatura\n";
            message = "Now type something and i shall repeat what you type \n";
            write(sock , message , strlen(message));
        }else if ( (str.compare(0,4,"1001") == 0) || (str.compare(0,11,"temperatura") == 0) ||
                    (str.compare(0,11,"TEMPERATURA") == 0)   ){   
            client->type = CT_TEMPERATURA;
            std::cout << "Type text echo\n";
            message = "From now you must send json/temperatura messages\n";
            write(sock , message , strlen(message));
        }else if ( (str.compare(0,4,"1002") == 0) || (str.compare(0,4,"time") == 0) ||
                    (str.compare(0,4,"TIME") == 0)   ){   
            client->type = CT_TIME;
            std::cout << "Type text time\n";
            message = (char *)getDateTime().c_str();
            write(sock , message , strlen(message));
        }else{
             client->type = CT_NOTYPE;
             std::cout << "No type\n";
        }
        break;
    }

     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //end of string marker
		client_message[read_size] = '\0';
		
		str = convertToString(client_message,strlen(client_message));
        std::cout << "String contem... "<<str << "\n";
        if (str.compare(0,3,"end") == 0){   
             std::cout << "Terminando o programa..." << "\n";
             close(sock);
             fim=1;
             break;
        }else  if (str.compare(0,3,"fim") == 0){   
            //Send the message back to client
             std::cout << "Terminando o handler..." << "\n";
             close(sock);
            break; 
        }
        switch(client->type){
            case CT_ECHO:
                write(sock , client_message , strlen(client_message));             
                break;
            case CT_TEMPERATURA:{
                write(sock , "json/TEMPERATURA\n" , strlen("json/TEMPERATURA\n"));   
                std::stringstream ss(str);
                std::string token;
                char delim = '-';
                std::getline(ss, token, delim);
                std::string sindex = token;
                std::cout << token << "\n";          
                std::getline(ss, token, delim);
                std::string stype = token;
                std::cout << token << "\n";          
                std::getline(ss, token, delim);
                std::string sid = token;
                std::cout << token << "\n";          
                std::getline(ss, token, delim);
                std::string sjson = token;
                std::cout << token << "\n";          
                put(sindex,stype,sid,sjson);
                }
                break;
            case CT_TIME:
                message = (char *)getDateTime().c_str();
                write(sock , message , strlen(message));
                break;
        }
        
		//clear the message buffer
		memset(client_message, 0, 2000);
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
} 
/*
int serverThreads()
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     int reuse = 1;
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 5038 );
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEADDR) failed");
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) && fim == 0)
    {
        puts("Connection accepted");
        setHandlerSocket(client_sock);
        listAllHandlerSocket(); 
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");
    }
    printf("serverThreads: Closing sock #%04d\n",client_sock);
    close(client_sock);
    closeAllHandlerSocket();
    close(socket_desc); 
    usleep(999999);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    printf("serverThreads: Server is finishing...\n"); 
    return 0;
}
*/

/*
 * This will handle connection for each client
 * */
/*
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
    std::string str; 
    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
     
    message = "Now type something and i shall repeat what you type \n";
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //end of string marker
		client_message[read_size] = '\0';
		
		str = convertToString(client_message,strlen(client_message));
        cout << "String contem... "<<str << "\n";
         if (str.compare(0,3,"end") == 0){   
             cout << "Terminando o programa..." << "\n";
             closeSocketBysocket(sock);
             fim=1;
             break;
         }
         if (str.compare(0,3,"fim") != 0){   
            //Send the message back to client
            write(sock , client_message , strlen(client_message));             
         }else{
             cout << "Terminando o handler..." << "\n";
             closeSocketBysocket(sock);
             usleep(999999);
            break; 
         }
        
		//clear the message buffer
		memset(client_message, 0, 2000);
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
} 
*/
/*
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
 
using namespace std;
 
int server()
{
    // Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return -1;
    }
 
    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(5039);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
 
    bind(listening, (sockaddr*)&hint, sizeof(hint));
 
    // Tell Winsock the socket is for listening
    listen(listening, SOMAXCONN);
 
    // Wait for a connection
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
 
    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
 
    char host[NI_MAXHOST];      // Client's remote name
    char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on
 
    memset(host, 0, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);
 
    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        cout << host << " connected on port " << service << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on port " << ntohs(client.sin_port) << endl;
    }
 
    // Close listening socket
    close(listening);
 
    // While loop: accept and echo message back to client
    char buf[4096];
 
    while (true)
    {
        memset(buf, 0, 4096);
 
        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }
 
        if (bytesReceived == 0)
        {
            cout << "Client disconnected " << endl;
            break;
        }
 
        cout << string(buf, 0, bytesReceived) << endl;
 
        // Echo message back to client
        send(clientSocket, buf, bytesReceived + 1, 0);
    }
 
    // Close the socket
    close(clientSocket);
 
    return 0;
}
*/
