#include "Main.h"


Main::Main(void)
{
}


Main::~Main(void)
{
}

DWORD WINAPI myThread(LPVOID lpParameter)
{
	tcp_server* server = (tcp_server*)lpParameter ;
	server->start_listening();
	return 0 ;
}


void Main::start(){

	touchRenderer = new TouchRenderer();
	touchlistener = new TouchListener(touchRenderer);

	server = new tcp_server(8000);
	HANDLE myHandle = CreateThread(0, 0, myThread, server, 0,NULL);

}
