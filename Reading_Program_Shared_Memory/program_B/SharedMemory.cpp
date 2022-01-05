#include"SharedMemory.h"
#include"shared_memory.h"
#include<iostream>
#include<conio.h>
#include <Windows.h>//for sleep

using namespace std;


SharedMemory::SharedMemory(char* Name, int Size, int q, int N)
{
	//char* Name;//Name of Shared Memory
	//int Size;//Size of Shared Memory
	//char* P;//Pointer to Shared Memory
	//int q;//Unique Identifier
	//int N;//total number of programs


	//Initializing native variables to 0
	this->Size = 0;
	this->q = 0;
	this->N = 0;


	double* pdouble;
	int* pint;

	//Making the native variables = to inputs
	this->Size = Size;
	this->q = q;
	this->N = N;



	P = shared_memory(Name, Size);


	if (P == NULL)
	{
		cout << "error" << endl;
		exit(1);
	}

	pdouble = (double*)P;
	pint = (int*)P;




	if (q == 0)//first program in queue
	{
		for (int i = 0; i < Size / (sizeof(char)); i++)
		{
			P[i] = '0';
		}

		pint[0] = 0;
	}



	cout << "Talking stick at: " << pint[0] << endl;



}



int SharedMemory::write_data(double* A, int* B, int* C )
{

	//first thing need to do is check if it is
	//my turn to write into the shared data

	int* pint = (int*)P;
	double* pdob = (double*)P;
	char* pchar = (char*)P;
	//int SizeOfString = strlen(str);
	int SizeOfDoubleA = 1;
	int SizeOfIntA = 1;
	int counter;
	int qvalue;
	
	int marker = 0;

	int wait_time = 0;
	int delay_time = 2;
	int timeout = 10;


	qvalue = pint[0];
	if (qvalue == q)
	{

		pdob[1] = A[0];
		pint[4] = B[0];
		pint[5] = C[0];

		//increment the q counter
		pint[0] = ++qvalue;

	}

	if (qvalue == N)
	{
		qvalue = 0;
	}


	return 0;//1 if everything is good
}



int SharedMemory::read_data(double* A, int* B, int* C)
{

	//first thing need to do is check if it is
	//my turn to write into the shared data

	int* pint = (int*)P;
	double* pdob = (double*)P;
	char* pchar = (char*)P;
	//int SizeOfString = strlen(str);
	int SizeOfDoubleA = 1;
	int SizeOfIntA = 1;
	int counter;
	int qvalue;

	int marker = 0;

	int wait_time = 0;
	int delay_time = 2;
	int timeout = 10;

	while (true)
	{
		qvalue = pint[0];
		if (qvalue == q)
		{

			//reading array of doubles
			counter = 0;
			for (int i = 1; i < (1 + SizeOfDoubleA); i++)
			{
				//cout << dint[i] << endl;
				A[counter] = pdob[i];
				counter++;
			}

			marker = SizeOfDoubleA * 2 + 2;
			////reading string
			//counter = 0;
			//for (int i = SizeOfDoubleA * 8 + 8; i < Size; i++)
			//{
			//
			//	//cout << P[i];
			//	str[counter] = P[i];
			//	if (P[i] = '\n') break;
			//	counter++;
			//}
			//cout << endl;

			//reading array of integers
			counter = 0;
			for (int i = marker; i < (marker + SizeOfIntA); i++)
			{
				//cout << dint[i] << endl;
				B[counter] = pint[i];
				counter++;
			}
			marker += SizeOfIntA;
			//reading array of integers
			counter = 0;
			for (int i = marker; i < (marker + SizeOfIntA); i++)
			{
				//cout << dint[i] << endl;
				C[counter] = pint[i];
				counter++;
			}

			//increment the q counter
			qvalue++;
			break;

		}
		else
		{
			break;
			//if (wait_time > timeout) break;
			//
			//wait_time += delay_time;
			//Sleep(delay_time);
		}

	}


	if (qvalue == N)
	{
		qvalue = 0;
	}

	pint[0] = qvalue;
	return 1;//1 if everything is good
}