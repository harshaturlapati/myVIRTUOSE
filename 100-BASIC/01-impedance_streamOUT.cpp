
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
#define SEND_PORT 27017
#define RECV_PORT 27018
float delta_t = 0.0001f; // sampling rate - VERY IMPORTANT

// Include for RECV
#ifndef UNICODE
#define UNICODE
#endif

#include <sys/types.h>
// Includes for UDP - end


#include	<stdio.h>
#include	"VirtuoseAPI.h"
//#include	<unistd.h>  


// LOGGING statements start
#include <time.h>
#include <ctime>

#include <chrono>
using namespace std::chrono;
int64_t timestamp = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();

#define dim_CART 7
#define dim_f 6

#define DURATION 400// Network timeout (seconds)
constexpr auto TIMEOUT_DURATION = std::chrono::seconds(20);
float time_duration = DURATION; // Duration of the example (seconds)

float UDP_f[7];
float position[7];

double time_log[DURATION * 1000]{};
int64_t unix_epoch[DURATION * 1000]{};
float UDP_f_log[DURATION * 1000][dim_f]{};
float force_log[DURATION * 1000][dim_f]{};
float Virtuose_POS_log[DURATION * 1000][dim_CART]{};


std::string GetTimestamp(time_t now) {
	tm* ltm = localtime(&now);
	std::string year = std::to_string(1900 + ltm->tm_year);
	std::string month = std::to_string(1 + ltm->tm_mon);
	std::string day = std::to_string(ltm->tm_mday);
	std::string hour = std::to_string(ltm->tm_hour);
	std::string min = std::to_string(ltm->tm_min);
	std::string sec = std::to_string(ltm->tm_sec);
	std::string timestamp = year + "_" + month + "_" + day + "_" + hour + min + sec + ".csv";
	return timestamp;
}

int64_t GetTickUs()
{
#if defined(_MSC_VER)
	LARGE_INTEGER start, frequency;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	timestamp = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();

	return (start.QuadPart * 1000000) / frequency.QuadPart;
#else
	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC, &start);

	return (start.tv_sec * 1000000LLU) + (start.tv_nsec / 1000);
#endif
}



// STATEMENTS to include so you get exp_folder from exp_settings.txt
std::string exp_folder;
char EXEPATH[MAX_PATH];

// Windows functions to get executable paths
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


TCHAR* GetEXEpath()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::cout << "Executable path: " << buffer << std::endl;
	return buffer;
}

// Custom string functions
void replace_all(
	std::string& s,
	std::string const& toReplace,
	std::string const& replaceWith
) {
	std::string buf;
	std::size_t pos = 0;
	std::size_t prevPos;

	// Reserves rough estimate of final size of string.
	buf.reserve(s.size());

	while (true) {
		prevPos = pos;
		pos = s.find(toReplace, pos);
		if (pos == std::string::npos)
			break;
		buf.append(s, prevPos, pos - prevPos);
		buf += replaceWith;
		pos += toReplace.size();
	}

	buf.append(s, prevPos, s.size() - prevPos);
	s.swap(buf);
}


void get_exp_folder_v2()
{
	GetModuleFileName(NULL, EXEPATH, MAX_PATH);
	//std::cout << EXEPATH << std::endl;
	DWORD length = GetModuleFileName(NULL, EXEPATH, MAX_PATH);

	wchar_t wtext[MAX_PATH];
	mbstowcs(wtext, EXEPATH, strlen(EXEPATH) + 1);//Plus null
	LPWSTR ptr = wtext;

	std::string s1(EXEPATH);
	//cout << s1.substr(0, s1.find_last_of("\\/")) << endl;

	std::string s2 = s1.substr(0, s1.find_last_of("\\/"));

	//std::cout << s2 << std::endl;

	exp_folder = s2;

	std::cout << "Executable path: " << exp_folder << std::endl;

	replace_all(exp_folder, "\\", "/");
	std::cout << "Executable path: " << exp_folder << std::endl;

	std::string exp_settings = exp_folder;
	exp_settings.append("/exp_settings.txt");
	std::cout << "exp settings file is at: " << exp_settings << std::endl;

	std::string read_exp_line;
	std::ifstream MyReadFile(exp_settings);
	std::getline(MyReadFile, read_exp_line);
	replace_all(read_exp_line, "\\", "/");
	read_exp_line.erase(0, 1);
	exp_folder.append("/");
	exp_folder.append(read_exp_line);
	exp_folder.append("/");
	std::cout << exp_folder << std::endl;

}
// STATEMENTS to include so you get exp_folder from exp_settings.txt


void write_to_file_v3(int64_t unix_epoch[], double time_log[], int duration, int data_count, time_t now, std::string file_name)
{

	std::cout << "Writing to file..." << std::endl;

	get_exp_folder_v2();
	std::string base_path = exp_folder;

	std::string timestamp = GetTimestamp(now);

	std::ofstream log_file(base_path + file_name + timestamp + ".csv");

	if (log_file.is_open())
	{
		log_file << "Time(ms),Time(s),Unix_epoch(ns),"
			<< "UDP_f1,UDP_f2,UDP_f3,UDP_f4,UDP_f5,UDP_f6" << ","
			<< "X,Y,Z,qx,qy,qz,qw" << ","
			<< "f1,f2,f3,f4,f5,f6" << ","
			<< "Index\n";

		for (int i = 0; i < duration * 1000 && i < data_count - 1; ++i)
		{
			log_file << (time_log[i] - time_log[0]) / 1000 << ",";
			log_file << (time_log[i] - time_log[0]) / 1000000 << ",";
			log_file << unix_epoch[i] << ",";

			// "UDP_f1,UDP_f2,UDP_f3,UDP_f4,UDP_f5,UDP_f6" << ","
			for (int j = 0; j < dim_f; ++j)
			{
				log_file << UDP_f_log[i][j] << ",";
			}

			for (int j = 0; j < dim_CART; ++j)
			{
				log_file << Virtuose_POS_log[i][j] << ",";
			}

			for (int j = 0; j < dim_f; ++j)
			{
				log_file << force_log[i][j] << ",";
			}

			

			log_file << i << "\n";
		}
	}
	std::cout << "Writing to file completed!" << std::endl;
}


// LOGGING statements end


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
	unsigned long ul = 1;
	int           nRet;
	nRet = ioctlsocket(RecvSocket, FIONBIO, (unsigned long*)&ul); // make it non-blocking

	//struct timeval read_timeout;
	//read_timeout.tv_sec = 0;
	//read_timeout.tv_usec = 10;
	//setsockopt(RecvSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&read_timeout, sizeof read_timeout);

	iVal = 1; // if you set this too low, the recvsocket will be impatient and might not receive at all, because its too fast to listen.
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

	return 0;
}


void UDP_send_recv(float* input_pos)
{
	int delimiter_idx = 0;


	// Defining the UDP send
	float num_TOSEND[7];
	num_TOSEND[0] = input_pos[0];
	num_TOSEND[1] = input_pos[1];
	num_TOSEND[2] = input_pos[2];
	num_TOSEND[3] = input_pos[3];
	num_TOSEND[4] = input_pos[4];
	num_TOSEND[5] = input_pos[5];
	num_TOSEND[6] = input_pos[6];

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

			UDP_f[delimiter_idx] = num_float;

			myMATLAB_DATA.erase(0, pos + delimiter.length());
			delimiter_idx++;
			if (delimiter_idx > 6)
			{
				break;
			}
		}

		std::cout << "q1 =" << UDP_f[0] << " q2 =" << UDP_f[1] << " q3 =" << UDP_f[2] << " q4 =" << UDP_f[3] << " q5 =" << UDP_f[4] << " q6 =" << UDP_f[5] << " q7 =" << UDP_f[6] << std::endl;

	}
	
	// UDP part - end

}



#include <future>
#include <thread>

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
	virtSetTimeStep(VC, delta_t);
	virtSetBaseFrame(VC, identity);
	virtSetObservationFrame(VC, identity);
	virtSetCommandType(VC, COMMAND_TYPE_IMPEDANCE);
	//virtSetPowerOn(VC, 1);

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

		Sleep(1); // Very important or the virtGetPosition does not seem to work.
		j = j + 1;
	}

	// Impedance control
	int data_count = 0;
	int i = 0;
	

	while (!(GetKeyState('Q') & 0x8000))
	{
		
		float speed[6], force[6];
		
		time_log[data_count] = GetTickUs();
		virtGetPosition(VC, position);
		UDP_send_recv(position);
		unix_epoch[data_count] = timestamp; // Make sure GetTickUs() is called before timestamp is recorded into unix_epoch.
		
		for (int idx = 0; idx <= 2; idx++) {
			force[idx] = K[idx] * (des_X[idx] - position[idx]);
		}

		force[3] = 0;
		force[4] = 0;
		force[5] = 0;

		std::cout << "goal1 = " << des_X[0] << "goal2 = " << des_X[1] << "goal3 = " << des_X[2] << std::endl;
		std::cout << "f1 = " << force[0] << "f2 = " << force[1] << "f3 = " << force[2] << "f4 = " << force[3] << "f5 = " << force[4] << "f6 = " << force[5] << std::endl;

		// Data logging starts

		for (i = 0; i < dim_CART; i++)
		{
			Virtuose_POS_log[data_count][i] = position[i];
		}

		for (i = 0; i < dim_f; i++)
		{
			force_log[data_count][i] = force[i];
		}

		for (i = 0; i < dim_f; i++)
		{
			UDP_f_log[data_count][i] = UDP_f[i];
		}
		
		// Data logging ends
		//virtSetForce(VC, force);
		//Sleep(0.0001);
		data_count = data_count + 1;

		
		
	}
	virtSetPowerOn(VC, 0);
	virtClose(VC);

	// UDP close - start
	// cleanup - SEND
	closesocket(ConnectSocket);

	// Close the socket when finished receiving datagrams - RECV
	wprintf(L"Finished receiving. Closing socket.\n");
	iResult2 = closesocket(RecvSocket);


	WSACleanup();
	// UDP close - end
	time_t now_t = time(0);
	std::string file_name = "virtuose_log_file_";

	write_to_file_v3(unix_epoch, time_log, time_duration, data_count, now_t, file_name);

	return 0;
}