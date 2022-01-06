
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <thread>

#include "shared_memory.h"
#include "SharedMemory.h"

#include "../../modules/Named_Pipes_Multithread/named_pipes/named_pipes/pipes.h"		   // IPC commands to commmunicate with python

using namespace std;

Pipes_Server Ipcserver("\\\\.\\pipe\\Foo");

void thread1()
{

	//Launch broadcasting pipes
	//Ipcserver.open_log();
	std::cout << "Thread 1: Launching Server..." << std::endl;
	Ipcserver.launch_server();
}


void thread2()
{

	int n = 100; // size of shared memory block (bytes)
	char name[] = "shared_memory1"; // name of shared memory block

	double A[1];
	int B[1];
	int C[1];
	char str[100];

	SharedMemory SM(name, n, 1, 2);

	while (true)
	{
		SM.read_data(A, B, C);


		std::cout << "Thread 2: broadcast string: " << A[0] << " | " << B[0] << " | " << C[0] << std::endl;

		Ipcserver.pipe_broadcast_string = std::to_string(A[0]) + ";" + std::to_string(B[0]) + ";" + std::to_string(C[0]);
		//cout << A[0] << " Seconds";
		Sleep(250);
	}
}



int main(int argc, char* argv[])
{

	thread t1(thread1);
	thread t2(thread2);

	t1.join();	// Launch Ipcserver thread
	t2.join();

	//int i;



	cout << "\ndone.\n";
	system("pause");

	return 0;
}

