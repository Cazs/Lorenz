#include "../lib/easywsclient.hpp"
//#include "easywsclient.cpp" // <-- include only if you don't want compile separately
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <memory>
#include <mutex>
#include <deque>
#include <thread>
#include <chrono>
#include <atomic>

// #include <string>
// #include <winsock2.h>
// #include <ws2tcpip.h>
// #include <windows.h>

#define PORT "8008"
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

using easywsclient::WebSocket;
static WebSocket::pointer ws = NULL;

void handle_message(const std::string & message)
{
    printf("Handle message: %s\n", message.c_str());
    if (message == "world") { ws->close(); }
}

int main()
{
    // #ifdef _WIN32
    //     INT rc;
    //     WSADATA wsaData;

    //     rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    //     if (rc) {
    //         printf("WSAStartup Failed.\n");
    //         return 1;
    //     }
    // #endif

    WSADATA wsa;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsa);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return std::int32_t(INVALID_SOCKET);
    }

    printf("Winsock initialized\n");

    struct addrinfo hints, *addrs;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // 2
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Convert IPv4 and IPv6 addresses from text to binary form 
    // iResult = getaddrinfo("127.0.0.1", PORT, &hints, &addrs);
    iResult = getaddrinfo("192.168.1.101", PORT, &hints, &addrs);
    if (iResult != 0)
    {
        printf("Get Address Info failed: (%d) %s\n", iResult, gai_strerror(iResult));
        WSACleanup();
        return -1;
    }

    printf("Get Address Info Success: %d\n", iResult);

    SOCKET sock = INVALID_SOCKET;

    for(struct addrinfo *addr = addrs; addr != NULL; addr = addr->ai_next)
    {
        sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        printf("Is Valid Socket? %d\n", sock != INVALID_SOCKET);
        if (sock != INVALID_SOCKET)
        {
            printf("Valid Socket...\n");
            if (connect(sock, addr->ai_addr, addr->ai_addrlen) != SOCKET_ERROR)
                break;

            // closesocket(sock);
            // sock = INVALID_SOCKET;
        }
    }

    if (sock == INVALID_SOCKET)
    {
        printf("Socket creation failed/Connection failed\n");
        // printf(WSAGetLastError());
        WSACleanup();
        return -1;
    }

    printf("Connection successful\n");

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    int byteCount;
    std::string s;
    // while (run) {
    while (true) {
    // while (byteCount > 0) {
        // printf("Accepting connections...\n");

        // m_socketAddress.sin_family = AF_INET;
        // m_socketAddress.sin_port = htons(m_port);
        // m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str());

        sock = accept(sock, (sockaddr *)&addrs->ai_addr, int32_t(&addrs->ai_addrlen));
        if (sock < 0)
        {
            // std::ostringstream ss;
            // std::cout << "Server failed to accept incoming connection from ADDRESS: " << inet_ntoa(sock.sin_addr) << "; PORT: " << ntohs(sock.sin_port) << std::endl;
            std::cout << "Server failed to accept incoming connection..." << std::endl;
            // exitWithError(ss.str());
            // printf(ss.str());
            return -1;
        }
        // if (sock.recv(s) != SOCKET_ERROR) {
        // if (sock.recv(s)) {
        byteCount = recv(sock, recvbuf, recvbuflen, 2);
        // printf(">>> %s\n", sock.recv(s));
        // printf("Received a connection bytes: %d\n", byteCount);
        /*if () {
            printf("Received a connection...\n");
            std::cout << s << '\n';
        }*/
        if ( byteCount >= 0 )
            printf("Bytes received: %d\n", iResult);
        // else if ( iResult == 0 )
        //     printf("Connection closed\n");
        // else
        //     printf("recv failed: %d\n", WSAGetLastError());

        // printf("Thread sleeping...\n");
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // printf("Thread done sleeping...\n");
        // sock.close();
    }
    /*ws = WebSocket::from_url("ws://localhost:8126/foo");
    assert(ws);
    ws->send("goodbye");
    ws->send("hello");
    while (ws->getReadyState() != WebSocket::CLOSED) {
      ws->poll();
      ws->dispatch(handle_message);
    }
    delete ws;*/

    // using easywsclient::WebSocket;

    // WebSocket::pointer ws = WebSocket::from_url("ws://localhost:8126/foo");
    // assert(ws);

    // Ws socket("ws://192.168.1.101:4444");

    /*int bytesReceived;
    std::string s;
    while (true) {
        printf("Accepting connections...\n");

        bytesReceived = sock.recv(s);
        if (bytesReceived < 0) {
            std::cout << "Error: " << WSAGetLastError() << std::endl;
            exit(1);
        }

        printf("Received a connection...\n");
        std::cout << s << '\n';
        // printf("Thread sleeping...\n");
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // printf("Thread done sleeping...\n");
        // socket.close();

        /*printf("Accepting connections...\n");
        ws->poll();

        printf("Send hello...\n");
        ws->send("hello");

        printf("Dispatch message handler...\n");
        ws->dispatch(handle_message);*
        // ...do more stuff...
    }
    delete ws; // alternatively, use unique_ptr<> if you have C++11
    */
    return 0;
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#endif
    return 0;
}
