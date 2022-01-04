
#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>

#define BUFSIZE 512

class Pipes_Server
{

	HANDLE hPipe;
	char buffer[1024];
	DWORD dwRead;

	BOOL fConnected;
	DWORD  dwThreadId = 0;


	

public:

	int launch_server();
	
	Pipes_Server();
	~Pipes_Server();


};


class Pipes_Client
{

	HANDLE hPipe;
	char buffer[1024];
	DWORD dwRead;

public:


	Pipes_Client();
	~Pipes_Client();


};