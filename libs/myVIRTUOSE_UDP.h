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
