#include "tcp_server.h"
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <sstream>
#include <string>
#include "../SCProtocol.h"

using namespace std ;
tcp_server::tcp_server( int port, Drawing* d)
{
    tcp_server::port = port;
	tcp_server::drawing = d ;
	tcp_server::hasToClose = false ;
	tcp_server::nbOfClients = 0 ;
	initializeWinsock();
}

tcp_server::~tcp_server()
{
	closesocket(ClientSocket);
	WSACleanup();
}

void tcp_server::setNbOfClients(int n){
	nbOfClients = n ;
}

int tcp_server::getNbOfClients(){
	return nbOfClients;
};

 void tcp_server::initializeWinsock(){
	  /* WSAStartup() initializes
		 the program to call winsock2.dll */
	WORD wVersionRequested;
    WSADATA wsaData;
    int wsaerr;
    wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaerr != 0)
    {
        printf("Server: ERROR FINDING WINSOCK2 dll!\n");
        WSACleanup();
        return ;
    }

    else
    {
        printf("Server: The Winsock2 dll was found \n");
    }

 }

void tcp_server::ProcessMessage(float data[]){
	cout << data[0] << ";" << data[1] << endl ;
	if(data[0] != CLIENTID ){
		cerr << "Not a valid ID" << endl ;
	}
	else{
		switch((int)data[1]){
			case TOUCHROTATION:
				drawing->rotateCamera(data[2],data[3],data[4]);
				break;
			case TOUCHTRANSLATION:
				drawing->translate(data[2],data[3],data[4]);
				break;
			case TOUCHSCALE:
				drawing->scale(data[2]);
				break;
			case TANGIBLEROTATION:

				break;
			case TANGIBLETRANSLATION:

				break;
			case TANGIBLESCALE:

				break;
			case PARTICLESDATASET:
				tcp_server::drawing->setFileName(PARTICLES);
				tcp_server::drawing->read();
				break;
			case JETLINESDATASET:
				tcp_server::drawing->setFileName(JETLINES);
				tcp_server::drawing->read();
				break;
			case ENDPROGRAM:
				tcp_server::hasToClose = true ;
				break ;
			default:
				cerr << "Not a valid message type" << endl ;
		}
	}
}

int tcp_server::start_listening()
{


   
    /* SOCKET is simply a UINT, created because
 on Unix sockets are file descriptors(UINT) but not in windows
 so new type SOCKET was created */
    //ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	unsigned long mode = 1;
	ioctlsocket(ListenSocket, FIONBIO, &mode);
    if(ListenSocket == INVALID_SOCKET)
    {
        cerr << "Server: Error initializing socket\n" << endl;
        WSACleanup();
        return 0;
    }

	//Non-Blocking
	//unsigned long mode = 1;
	//ioctlsocket(ListenSocket, FIONBIO, &mode);
    /* The SOCKADDR_IN structure is used by
 Win Sockets to specify an endpoint address
 to which the socket connects */
    sockaddr_in service,client ;
    service.sin_family = AF_INET;
    service.sin_port = htons(port);
	service.sin_addr.s_addr = INADDR_ANY;

	SOCKADDR_IN srvAddress;
	int srvLen = sizeof(srvAddress);
	memset( &srvAddress, 0, srvLen);
	srvAddress.sin_family = AF_INET;
	srvAddress.sin_addr.s_addr = INADDR_ANY;
	srvAddress.sin_port = htons(8000);
	
	drawing->rotate(0,0,0);
	drawing->rotate(0,0,0);
	drawing->translate(0,0,-0.4);
	drawing->scale(0.5);

    /* bind just links the socket
 with the sockaddr_in struct we initialized */
	if(bind(ListenSocket,(LPSOCKADDR) &srvAddress,sizeof(srvAddress))==SOCKET_ERROR)
    {
        printf("Server: Error binding socket to port \n");
        WSACleanup();
        return 0;
    }

	return 1 ;

    /* wait for incoming connections */
    if(listen(ListenSocket,10)==SOCKET_ERROR)
        printf("listen(): Error listening on socket %d\n", WSAGetLastError());
    else
    {
        printf("Server: Waiting for connections...\n");
    }
    
    /* accept connections */
	
	printf("Server: Waiting for a client to connect...\n");
	ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    } else {
        wprintf(L"Client connected.\n");
		acceptConns();

	}

    // No longer need server socket
    closesocket(ListenSocket);
    return 0;
}

/* TODO: To change functionality
 of the server change the following function... */
int tcp_server::acceptConns()
{
	
    sockaddr_in from;
	bool infinite = true ;
    int fromlen=sizeof(from);
    /* Infinte loop to echo
 the IP address of the client */

    int readsize;
    char* message;
	char* clientmessage = (char*) malloc(MESSAGESIZE*sizeof(char));
    string smatrix ;
    int ind ;
    string tok;
    int i = 0 ;
    int bytesSent ;


    float matrix[7] ;
    while( (readsize = recv(ClientSocket, clientmessage, 2000, 0))> 0 ){
        message = "ack";
        bytesSent = send(ClientSocket, message, strlen(message),0);
        if(bytesSent == 0){
            std::cerr << "Error sending ACK" << endl ;
        }
		cout <<"BytesSent" << bytesSent << endl;
        smatrix = clientmessage ;
        std::stringstream ss(smatrix);

        
        while(getline(ss, tok, ',') && i < 16 ){
            matrix[i] = static_cast<float>(::atof(tok.c_str()));
            i++ ;
        }
		cout << matrix << endl ;
        i = 0 ;
		message ="ok";
		bytesSent = send(ClientSocket, message, strlen(message),0);
        if(bytesSent == 0){
            std::cerr << "Error sending ACK" << endl ;
        }
		ProcessMessage(matrix);
    }
    if(readsize == 0 ){
        cerr << "client disconnected" << endl ;
    }
    cout << "Socket closed" << endl ;
    closesocket(ListenSocket);
    WSACleanup();
	return 0 ;
}

void tcp_server::ProcessIncomingMessage(){
	int readsize;
    char* message;
	char* clientmessage = (char*) malloc(MESSAGESIZE*sizeof(char));
    string smatrix ;
    int ind ;
    string tok;
    int i = 0 ;
    int bytesSent ;
	SOCKADDR from;
	int addrLen = sizeof(SOCKADDR);
	int bytes = 0;
	float matrix[7] ;
	do{
		bytes = recvfrom(ListenSocket, clientmessage, MESSAGESIZE, 0, (SOCKADDR*) &from, &addrLen);
		if(bytes <= 0){
#ifdef DEBUG
			cout << " Nothing received : " << WSAGetLastError() << endl ;
#endif
			break; 
		}
		message = "ack";
        bytesSent = send(ClientSocket, message, strlen(message),0);
        if(bytesSent == 0){
            std::cerr << "Error sending ACK" << endl ;
        }
		cout <<"BytesSent" << bytesSent << endl;
        smatrix = clientmessage ;
        std::stringstream ss(smatrix);

        
        while(getline(ss, tok, ',') && i < 16 ){
            matrix[i] = static_cast<float>(::atof(tok.c_str()));
            i++ ;
        }
		cout << matrix << endl ;
        i = 0 ;
		message ="ok";
		bytesSent = send(ClientSocket, message, strlen(message),0);
        if(bytesSent == 0){
            std::cerr << "Error sending ACK" << endl ;
        }
		ProcessMessage(matrix);
	}while( bytes > 0 );
}
