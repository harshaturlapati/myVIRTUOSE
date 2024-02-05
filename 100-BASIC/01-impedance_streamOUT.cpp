#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <vector>
#include <conio.h>
#include <time.h>
#include <ctime>

// Includes for converting data type from float to string
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <conio.h>
#include <sys/stat.h>
#include <io.h>
#include <inttypes.h>
#include <windows.h>
#include <process.h>

// Includes for UDP - start - always keep before all other INCLUDE statements
#define WIN32_LEAN_AND_MEAN

//#include <winsock2.h> // Apparently not needed when using VirtuoseAPI.h
//#include <Ws2tcpip.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define SEND_PORT 27015
#define RECV_PORT 27016

// Include for RECV
#ifndef UNICODE
#define UNICODE
#endif

#include <sys/types.h>
// Includes for UDP - end


#include	<stdio.h>
#include	"VirtuoseAPI.h"
//#include	<unistd.h>  


// Set up UDP - starts
//----------------------
// Declare and initialize variables - SEND
WSADATA wsaData;
int iResult;

SOCKET ConnectSocket = INVALID_SOCKET;
struct sockaddr_in clientService;
//int SenderAddrSize = sizeof(clientService);

char* sendbuf = "this is a test";
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;

// Declare and initialize variables - RECV
int iResult2 = 0;

SOCKET RecvSocket;
struct sockaddr_in RecvAddr;

unsigned short Port2 = 27016;

char RecvBuf[1024];
int BufLen = 1024;

struct sockaddr_in SenderAddr;
int SenderAddrSize = sizeof(SenderAddr);

int ret, iVal = 1;
unsigned int  sz = sizeof(iVal);
// Set up UDP - ends



int initialize_Winsock()
{
	//----------------------
// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	//----------------------
	// Create a SOCKET for connecting to server - SEND
	ConnectSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Create a receiver socket to receive datagrams - RECV
	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//struct timeval read_timeout;
	//read_timeout.tv_sec = 0;
	//read_timeout.tv_usec = 10;
	//setsockopt(RecvSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&read_timeout, sizeof read_timeout);

	iVal = 1; // if you set this too low, the recvsocket will be impatient and might not 
	ret = setsockopt(RecvSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&iVal, sz);

	if (RecvSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error %d\n", WSAGetLastError());
		return 1;
	}

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to. - SEND
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
	clientService.sin_port = htons(SEND_PORT);

	// Bind the socket to any address and the specified port - RECV
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	RecvAddr.sin_port = htons(RECV_PORT);

	iResult2 = ::bind(RecvSocket, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
	if (iResult2 != 0) {
		wprintf(L"bind failed with error %d\n", WSAGetLastError());
		return 1;
	}

	printf("Connect instead of bind was run.\n");
	if (iResult2 == -1) {
		printf("Connect instead of bind failed.\n");
		exit(2);
	}

	//----------------------
	// Connect to server.
	iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		printf("Unable to connect to server: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);


}


void UDP_send_recv(float* pos)
{
	int delimiter_idx = 0;
	float UDP_q[7];

	// Defining the UDP send
	float num_TOSEND[7];
	num_TOSEND[0] = pos[0];
	num_TOSEND[1] = pos[1];
	num_TOSEND[2] = pos[2];
	num_TOSEND[3] = pos[3];
	num_TOSEND[4] = pos[4];
	num_TOSEND[5] = pos[5];
	num_TOSEND[6] = pos[6];

	//int num_TOSEND_idx = 0;
	std::string sendbuf1;
	std::string sendbuf2;
	sendbuf2.append("q_start");
	//= "STRING_TO_SEND:";
	for (int num_TOSEND_idx = 0; num_TOSEND_idx < 7; num_TOSEND_idx++) {
		sendbuf1 = std::to_string(num_TOSEND[num_TOSEND_idx]);
		sendbuf2.append(sendbuf1);
		sendbuf2.append("||");
	}
	sendbuf2.append("q_end");

	// UDP part - start
		const char* sendbuf3 = sendbuf2.c_str();
	wprintf(L"Sending datagrams...\n");

	iResult = send(ConnectSocket, sendbuf3, (int)strlen(sendbuf3), 0);
	wprintf(L"datagrams sent...\n");
	iResult2 = recvfrom(RecvSocket,
		RecvBuf, BufLen, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);


	if (iResult2 > 0) // print recvbuffer ONLY if something was received
	{
		wprintf(L"Received datagrams...\n");

		std::cout << RecvBuf << std::endl;
		std::string myMATLAB_DATA(RecvBuf);
		std::string delimiter = "||";
		size_t pos = 0;
		std::string token;
		delimiter_idx = 0;
		while ((pos = myMATLAB_DATA.find(delimiter)) != std::string::npos) {
			token = myMATLAB_DATA.substr(0, pos);

			float num_float = std::stof(token);

			UDP_q[delimiter_idx] = num_float;

			myMATLAB_DATA.erase(0, pos + delimiter.length());
			delimiter_idx++;
			if (delimiter_idx > 6)
			{
				break;
			}
		}

		std::cout << "q1 =" << UDP_q[0] << " q2 =" << UDP_q[1] << " q3 =" << UDP_q[2] << " q4 =" << UDP_q[3] << " q5 =" << UDP_q[4] << " q6 =" << UDP_q[5] << " q7 =" << UDP_q[6] << std::endl;

	}

	// UDP part - end

}

int main()
{

	initialize_Winsock();
	

	//printf("hello world\n");
	VirtContext VC;
	VC = virtOpen("127.0.0.1#53210");
	if (VC == NULL)
	{
		fprintf(stderr, "Erreur dans virtOpen: %s\n", virtGetErrorMessage(virtGetErrorCode(NULL)));
		return -1;
	}
	printf("virtOpen worked\n");

	float identity[7] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f };
	virtSetIndexingMode(VC, INDEXING_ALL);
	virtSetForceFactor(VC, 1.0f);
	virtSetSpeedFactor(VC, 1.0f);
	virtSetTimeStep(VC, 0.003f);
	virtSetBaseFrame(VC, identity);
	virtSetObservationFrame(VC, identity);
	virtSetCommandType(VC, COMMAND_TYPE_IMPEDANCE);
	virtSetPowerOn(VC, 1);

	int j = 0;

	float des_X[7];
	float K[3];
	K[0] = 100;
	K[1] = 100;
	K[2] = 100;

	// Define a goal
	while (j < 100)
	{
		
		float myPOS[7];
		virtGetPosition(VC, myPOS);
		for (int i = 0; i <= 6; i++) {
			des_X[i] = myPOS[i];
		}
		UDP_send_recv(myPOS);
		Sleep(1); // Very important or the virtGetPosition does not seem to work.
		j = j + 1;
	}

	// Impedance control
	int i = 0;
	while (i < 200000)
	{
		
		float position[7], speed[6], force[6];

		virtGetPosition(VC, position); //std::cout << "q1 = " << position[0] << "q2 = " << position[1] << "q3 = " << position[2] << "q4 = " << position[3] << "q5 = " << position[4] << "q6 = " << position[5] <<  "q7 = " << position[6] << std::endl;

		UDP_send_recv(position);

		for (int idx = 0; idx <= 2; idx++) {
			force[idx] = K[idx] * (des_X[idx] - position[idx]);
		}

		force[3] = 0;
		force[4] = 0;
		force[5] = 0;

		std::cout << "goal1 = " << des_X[0] << "goal2 = " << des_X[1] << "goal3 = " << des_X[2] << std::endl; 
		std::cout << "f1 = " << force[0] << "f2 = " << force[1] << "f3 = " << force[2] << "f4 = " << force[3] << "f5 = " << force[4] << "f6 = " << force[5] << std::endl;

		//virtSetForce(VC, force);
		Sleep(1);
		i = i + 1;
	}
	virtSetPowerOn(VC, 0);
	virtClose(VC);

	return 0;
}