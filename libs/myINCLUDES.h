
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

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512

// Include for RECV
#ifndef UNICODE
#define UNICODE
#endif

#include <sys/types.h>
// Includes for UDP - end

#include	<stdio.h>
// LOGGING statements start
#include <time.h>
#include <ctime>
#include <chrono>
using namespace std::chrono;