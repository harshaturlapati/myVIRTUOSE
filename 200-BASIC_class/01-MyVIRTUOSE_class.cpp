#include <myINCLUDES.h>         // baked in
#include <myEXP_settings.h>     // baked in
#include "VirtuoseAPI.h"        
#include <myLOGGING.h>

#define DURATION 1000
// 2024-02-09 : Figure out a way to add these to myVIRTUOSE_LOG and avoid the stack overflow error
double              time_log[DURATION * 1000] = {};
int64_t             unix_epoch[DURATION * 1000] = {};
float               UDP_f_log[DURATION * 1000][6] = {};
float               force_log[DURATION * 1000][6] = {};
float               Virtuose_POS_log[DURATION * 1000][7] = {};

class myVIRTUOSE_LOG
{
public:
    int                 data_count = 0;
    static const int    dim_CART = 7;
    static const int    dim_f = 6;
    double              justnow;
    int64_t             timestamp;

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

    void write2LOG(int data_count_in, float position[7], float force[6], float UDP_f[6])
    {
        data_count = data_count_in;

        // Data logging starts

        time_log[data_count] = GetTickUs();

        unix_epoch[data_count] = timestamp; // Make sure GetTickUs() is called before timestamp is recorded into unix_epoch.
        
        for (int i = 0; i < dim_CART; i++)
        {
        	Virtuose_POS_log[data_count][i] = position[i];
        }
        
        for (int i = 0; i < dim_f; i++)
        {
        	force_log[data_count][i] = force[i];
            UDP_f_log[data_count][i] = UDP_f[i];
        }
        
    }

    myVIRTUOSE_LOG(int count) {
        data_count = count;
        justnow = GetTickUs();
    }
};

class myVIRTUOSE_UDP
{
private:
    
    

public:

    // UDP Receive global variables
    std::string UDPsendbuf;
    const char* sendbuf3;
    float num_TOSEND[7];
    int num_TOSEND_idx;
    std::string delimiter = "||";
    float num_float;
    size_t STRINGpos;
    std::string token;

    u_short SEND_PORT;
    u_short RECV_PORT;
    const char* SEND_IP_ADDRESS;
    const char* RECV_IP_ADDRESS;

    // Declare and initialize variables - SEND
    WSADATA wsaData;
    int iResult;

    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in clientService;

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






    float UDP_f[6] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
    void setup_UDP()
    {
        // define Winsock2 object
        WSADATA wsaData;

        // Initialise SEND UDP variables
        struct sockaddr_in clientService;

        // Initialise RECV UDP variabls
        SOCKET RecvSocket;
        struct sockaddr_in RecvAddr;

        struct sockaddr_in SenderAddr;
        int SenderAddrSize = sizeof(SenderAddr);

        // initialise winsock
        initialize_Winsock();
    }

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

        // Make port non-blocking - VERY IMPORTANT
        unsigned long ul = 1;
        int           nRet;
        nRet = ioctlsocket(RecvSocket, FIONBIO, (unsigned long*)&ul);

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
        clientService.sin_addr.s_addr = inet_addr(SEND_IP_ADDRESS);
        clientService.sin_port = htons(SEND_PORT);

        // Bind the socket to any address and the specified port - RECV
        RecvAddr.sin_family = AF_INET;
        RecvAddr.sin_addr.s_addr = inet_addr(RECV_IP_ADDRESS);
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
    }

    void UDP_send_recv_v3(float* input_pos)
    {
        int delimiter_idx = 0;

        UDPsendbuf = "";
        UDPsendbuf.append("q_start"); // string to send

        for (num_TOSEND_idx = 0; num_TOSEND_idx < 7; num_TOSEND_idx++) {
            num_TOSEND[num_TOSEND_idx] = input_pos[num_TOSEND_idx];
            UDPsendbuf.append(std::to_string(num_TOSEND[num_TOSEND_idx]));
            UDPsendbuf.append("||");

        }
        UDPsendbuf.append("q_end");

        // UDP part - start
        //wprintf(L"Sending datagrams...\n");

        sendbuf3 = UDPsendbuf.c_str();

        iResult = send(ConnectSocket, sendbuf3, (int)strlen(sendbuf3), 0);
        wprintf(L"datagrams sent...\n");
        iResult2 = recvfrom(RecvSocket,
            RecvBuf, BufLen, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);

        //iResult2 = recv(RecvSocket, RecvBuf, BufLen, 0);

        if (iResult2 > 0) // print recvbuffer ONLY if something was received
        {
            wprintf(L"Received datagrams...\n");

            //std::cout << RecvBuf << std::endl;
            std::string myMATLAB_DATA(RecvBuf);

            STRINGpos = 0;
            token = "";
            delimiter_idx = 0;
            while ((STRINGpos = myMATLAB_DATA.find(delimiter)) != std::string::npos) {
                token = myMATLAB_DATA.substr(0, STRINGpos);
                num_float = std::stof(token);
                UDP_f[delimiter_idx] = num_float;

                myMATLAB_DATA.erase(0, STRINGpos + delimiter.length());
                delimiter_idx++;
                if (delimiter_idx > 6 - 1)
                {
                    break;
                }
            }
            std::cout << "UDPf1 =" << UDP_f[0] << " UDPf2 =" << UDP_f[1] << " UDPf3 =" << UDP_f[2] << " UDPf4 =" << UDP_f[3] << " UDPf5 =" << UDP_f[4] << " UDPf6 =" << UDP_f[5] << std::endl;
        }
        else
        {
            wprintf(L"Received no datagrams...\n");
            // if no UDP communication occurred, set rendering force to 0
            for (int f_idx = 0; f_idx < 6; f_idx++) {
                UDP_f[f_idx] = 0;
            }
        }

        // UDP part - end
    }

    //	// UDP close - start
    //	// cleanup - SEND
    void cleanup() {
        // SEND socket close
        closesocket(ConnectSocket);

        //RECV socket close
        iResult2 = closesocket(RecvSocket);
        WSACleanup();
    }

    myVIRTUOSE_UDP(u_short SEND_PORT_input, u_short RECV_PORT_input, const char* SEND_IP_input, const char* RECV_IP_input)
    {
        SEND_PORT = SEND_PORT_input;
        RECV_PORT = RECV_PORT_input;
        SEND_IP_ADDRESS = SEND_IP_input;
        RECV_IP_ADDRESS = RECV_IP_input;
    }

};

class CMD {

private:
    

public:
    float k, b;
    float K[7], B[7];

    float X_d[7];
    float X[7], Xdot[7];
    float F_e[6] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};

    float dX[7];

    float f[6] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };

    void set_F_d(float F_e_input[6]) {
        for (int i = 0; i < 6; i++) {
            F_e[i] = F_e_input[i];
        }
    }

    void set_X_d(float X_d_input[7]) {
        for (int i = 0; i < 7; i++) {
            X_d[i] = X_d_input[i];
        }
    }

    void set_X(float X_input[7]) {
        for (int i = 0; i < 7; i++) {
            X[i] = X_input[i];
        }
    }
    void set_Xdot(float Xdot_input[7]) {
        for (int i = 0; i < 7; i++) {
            Xdot[i] = Xdot_input[i];
        }
    }
    void set_ext_F(float ext_F_input[6]) {
        for (int i = 0; i < 6; i++) {
            F_e[i] = ext_F_input[i];
        }
    }

    float* P_trn(float X_d_input[7], float X_input[7])
    {
        set_X_d(X_d_input);
        set_X(X_input);
        for (int i = 0; i < 3; i++)
        {
            dX[i] = X_d[i] - X[i];
            f[i] = K[i] * dX[i] + F_e[i];
        }
        return f;
    }

    float* PD_trn(float X_d_input[7], float X_input[7])
    {
        set_X_d(X_d_input);
        set_X(X_input);
        for (int i = 0; i < 3; i++)
        {
            dX[i] = X_d[i] - X[i];
            f[i] = K[i] * dX[i] - B[i] * Xdot[i] + F_e[i]; // While pushing the human towards the X_d, also give an opposite force along the current velocity
        }
        return f;
    }

    CMD(float k_input, float b_input) {
        k = k_input;
        b = b_input;
        for (int i = 0; i < 7; i++)
        {
            K[i] = k;
            B[i] = b;
        }
    }

    CMD(float K_input[7], float B_input[7]) {
        for (int i = 0; i < 7; i++)
        {
            K[i] = K_input[i];
            B[i] = B_input[i];
        }
    }
};

class MyVIRTUOSE {                          // The class
    private:
        
    public:                                 // Access specifier
        VirtContext VC;
        float X[7];
        float f[6] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
        const char* PORT;                                       // myAttributes
        float forcefactor, speedfactor, dt;                     // force factor, speed factor, and sampling rate - VERY IMPORTANT
        float identity[7] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f };

        void set_dt(float dt_input) {
            dt = dt_input;
        }

        void set_forcefactor(float forcefactor_input) {
            forcefactor = forcefactor_input;
        }

        void set_speedfactor(float speedfactor_input) {
            speedfactor = speedfactor_input;
        }

        void queryPOS() {
            virtGetPosition(VC, X);
        }

        float* getPOS() {
            queryPOS();
            return X;
        }

        void set_f(float f_input[7]) {
            for (int i = 0; i < 6; i++) {
                f[i] = f_input[i];
            }
        }

        void sendCMD_f(float f_input[7]) {
            set_f(f_input);
            virtSetForce(VC, f);
            std::cout << f[0] << f[1] << f[2] << f[3] << f[4] << f[5] << std::endl;
            // reset force to 0 - after every issued command - VERY VERY IMPORTANT if UDP drops out... - discuss with dc.
            for (int i = 0; i < 6; i++) {
                f[i] = 0;
            }
        }

        void debug_getPOS() {
            printf("Virtuose API debugging initiated\n");
            for (int i = 0; i < 10; i++) {
                queryPOS();
                std::cout << "X1 = " << X[0] << "|X2 = " << X[1] << "|X3 = " << X[2] << "|X4 = " << X[3] << "|X5 = " << X[4] << "|X6 = " << X[5] << "|X7 = " << X[6] << std::endl;
            }
            printf("Debugging done, hopefully you're seeing valid data - don't proceed if you see 0's or 10e8 values.\n");
        }

        //virtSetForce(VC, force);

        void setDEFAULTPARAMS() {
            virtSetIndexingMode(VC, INDEXING_ALL);
        	    virtSetForceFactor(VC, forcefactor);
        	    virtSetSpeedFactor(VC, speedfactor);
        	    virtSetTimeStep(VC, dt);
        	    virtSetBaseFrame(VC, identity);
        	    virtSetObservationFrame(VC, identity);
        	    virtSetCommandType(VC, COMMAND_TYPE_IMPEDANCE);
        }

        void engage() {
            virtSetPowerOn(VC, 1);
        }

        void disengage() {
            virtSetPowerOn(VC, 0);
        }

        void connect() {  // Method/function defined inside the class
            std::cout << "Hello World!";
            VC = virtOpen(PORT);
        	    if (VC == NULL)
        	    {
        		    fprintf(stderr, "Erreur dans virtOpen: %s\n", virtGetErrorMessage(virtGetErrorCode(NULL)));
        	    }
            printf("virtOpen worked - nothing else on Virtuose setup has been done\n");
        }

        void disconnect(){
            virtClose(VC);
        }

        void testrun() {
            printf("Doing a test run automatically\n");
            connect();  // Connect to Virtuose
            setDEFAULTPARAMS(); // Impedance mode parameters - as prescribed by the Documentation.
            engage(); // Now the Virtuose will stream data if you hold the handle.
            disengage(); // Now the Virtuose will stop streaming data even if you hold the handle.
            disconnect();
        }

        void quick_start() {
            printf("quick start entails - (i) connect, (ii) setDEFAULTPARAMS and (iii) engage.\n");
            connect();  // Connect to Virtuose
            setDEFAULTPARAMS(); // Impedance mode parameters - as prescribed by the Documentation.
            engage(); // Now the Virtuose will stream data if you hold the handle.
        }
        void quick_stop() {
            printf("quick stop entails - (i) disengage and (ii) disconnect.\n");
            disengage(); // Now the Virtuose will stop streaming data even if you hold the handle.
            disconnect();
        }

        MyVIRTUOSE(const char* port_input, float forcefactor_input, float speedfactor_input, float dt_input) 
        {   // Constructor 1 declaration
            PORT = port_input;
            dt = dt_input;
            forcefactor = forcefactor_input;
            speedfactor = speedfactor_input;
        }

        MyVIRTUOSE(const char* port_input)
        {   // Constructor 2  declaration
            PORT = port_input;
        }
};

class ARM : public MyVIRTUOSE // single colon for inheritance
{
    using MyVIRTUOSE::MyVIRTUOSE; // "using" for inheriting the constructors written for MyVIRTUOSE
public:
    std::string name;

    // can define new methods that allow to perform debugging operations on a single arm at a higher level - while having access to the lower level methods
};

int main() {

    /// <summary>
    /// MyVIRTUOSE methods          : connect, quick_start, set_dt, debug_getPOS, quick_stop, testrun
    /// MyVIRTUOSE constructor1     : MyVIRTUOSE(const char* port_input, float forcefactor_input, float speedfactor_input, float dt_input)
    /// MyVIRTUOSE constructor2     : MyVIRTUOSE(const char* port_input,)
    /// ARM methods                 : 
    /// CMD methods                 : 
    /// </summary>
    /// <returns></returns>

    // UDP class object
    myVIRTUOSE_UDP myUDP(27017, 27018, "127.0.0.1", "127.0.0.1");
    float input_pos[7] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f };
    myUDP.setup_UDP();
    
    // Virtuose object attributes
    const char* myPORT = "127.0.0.1#53210";
    float myFORCEFACTOR = 1.0f, mySPEEDFACTOR = 1.0f, myDT = 0.001f;

    // Virtuose object definition
    ARM RightARM("127.0.0.1#53210", myFORCEFACTOR, mySPEEDFACTOR, myDT);
    RightARM.name = "RightARM";
    RightARM.quick_start();
    RightARM.debug_getPOS();

    // Impedance control parameters
    float k = 0.1f, b = 0.1f;
    
    // Command structure object
    CMD cmd_R(k, b);

    for (int i = 0; i < 7; i++) {
        cmd_R.X_d[i] = RightARM.getPOS()[i];
    }

    myVIRTUOSE_LOG RightARM_LOG(0);

    int data_count = 0;
    printf("Press Q to exit loop\n");
    while (!(GetKeyState('Q') & 0x8000))
    {
        myUDP.UDP_send_recv_v3(RightARM.getPOS());

        cmd_R.P_trn(cmd_R.X_d, RightARM.X);
        
        //RightARM_LOG.write2LOG(data_count, cmd_R.X, cmd_R.f, myUDP.UDP_f);
        
        // RightARM.sendCMD_f(cmd_R.f); // issues the commanded force and resets the force variable also
        
        data_count = data_count + 1;
    }

    myUDP.cleanup();
    RightARM.quick_stop();
    


    //	time_t now_t = time(0);
    //	std::string file_name = "virtuose_log_file_";
    //
    //	write_to_file_v3(unix_epoch, time_log, time_duration, data_count, now_t, file_name);
    

	//// Impedance control
	//int data_count = 0;
	//int i = 0;
	//

	//while (!(GetKeyState('Q') & 0x8000))
	//{
	//	
	//	float speed[6], force[6];
	//	
	//	time_log[data_count] = GetTickUs();
	//	virtGetPosition(VC, position);
	//	UDP_send_recv_v3(position);
	//	unix_epoch[data_count] = timestamp; // Make sure GetTickUs() is called before timestamp is recorded into unix_epoch.
	//	
	//	for (int idx = 0; idx <= 2; idx++) {
	//		force[idx] = K[idx] * (des_X[idx] - position[idx]);
	//	}

	//	force[3] = 0;
	//	force[4] = 0;
	//	force[5] = 0;

	//	std::cout << "goal1 = " << des_X[0] << "goal2 = " << des_X[1] << "goal3 = " << des_X[2] << std::endl;
	//	std::cout << "f1 = " << force[0] << "f2 = " << force[1] << "f3 = " << force[2] << "f4 = " << force[3] << "f5 = " << force[4] << "f6 = " << force[5] << std::endl;

	//	// Data logging starts

	//	for (i = 0; i < dim_CART; i++)
	//	{
	//		Virtuose_POS_log[data_count][i] = position[i];
	//	}

	//	for (i = 0; i < dim_f; i++)
	//	{
	//		force_log[data_count][i] = force[i];
	//	}

	//	for (i = 0; i < dim_f; i++)
	//	{
	//		UDP_f_log[data_count][i] = UDP_f[i];
	//	}
	//	
	//	// Data logging ends
	//	//virtSetForce(VC, force);
	//	//Sleep(0.0001);
	//	data_count = data_count + 1;

	//	
	//	
	//}
	//virtSetPowerOn(VC, 0);
	//virtClose(VC);


    return 0;
}

//////////// debug statements
// myObj.debug_getPOS(); // you can observe that the first few iterations, the API does NOT pick up the right positions from the 
