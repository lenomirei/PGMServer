#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include<winsock2.h>
#include<WS2tcpip.h>	//ip_mreqͷ
#include <wsrm.h>
#include <stdio.h>
#include <chrono>

using namespace std;
#pragma comment(lib,"ws2_32.lib")

int main() {
    WSADATA WSAData;
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &WSAData) != 0)
        return 0;
    
    FILE *fp;
    fopen_s(&fp, "test.webm", "rb+");

    SOCKET        s;
    SOCKADDR_IN   salocal, sasession;
    int           dwSessionPort;

    s = socket(AF_INET, SOCK_RDM, IPPROTO_RM);

    salocal.sin_family = AF_INET;
    salocal.sin_port = htons(0);    // Port is ignored here
    salocal.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(s, (SOCKADDR *)&salocal, sizeof(salocal));

    //
    // Set all relevant sender socket options here
    //

    //
    // Now, connect <entity type="hellip"/>
    // Setting the connection port (dwSessionPort) has relevance, and
    // can be used to multiplex multiple sessions to the same
    // multicast group address over different ports
    //
    dwSessionPort = 1234;
    sasession.sin_family = AF_INET;
    sasession.sin_port = htons(dwSessionPort);
    sasession.sin_addr.s_addr = inet_addr("224.4.5.6");

    RM_SEND_WINDOW send_window;
    send_window.RateKbitsPerSec = 8000* 1000 * 8;
    send_window.WindowSizeInBytes = 8000 * 1000;
    send_window.WindowSizeInMSecs = 1;

    int rc = setsockopt(s, IPPROTO_RM, RM_RATE_WINDOW_SIZE, (char *)&send_window, sizeof(send_window));
    if (rc == SOCKET_ERROR)
    {

        cout << "setsockopt(): RM_RATE_WINDOW_SIZE failed with error code " << WSAGetLastError() << endl;

    }
    connect(s, (SOCKADDR *)&sasession, sizeof(sasession));

    //
    // We're now ready to send data!
    //
    char pSendBuffer[1400];

    sockaddr_in serverAddr;
    int iAddrlen = sizeof(serverAddr);




    while (1) {
        if (feof(fp))
            break;
        memset(pSendBuffer, 0, 1400);

        int data_size = fread(pSendBuffer, 1, 1400, fp);
        
        LONG        error;

        std::cout << "start" << std::endl;
        auto start = std::chrono::system_clock::now();

        error = sendto(s, pSendBuffer, data_size, 0, (sockaddr*)&serverAddr,iAddrlen);
        auto end = std::chrono::system_clock::now();
        std::cout << "end" << std::endl;

        std::cout << (end - start).count() << "s" << std::endl;
        if (error == SOCKET_ERROR)
        {
            fprintf(stderr, "send() failed: Error = %d\n",
                WSAGetLastError());
        }
    }

    WSACleanup();
    return 0;
}