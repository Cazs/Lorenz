#include "../lib/easywsclient.hpp"
//#include "easywsclient.cpp" // <-- include only if you don't want compile separately
#include <iostream>
#include <string>
#include <memory>
#include <mutex>
#include <deque>
#include <thread>
#include <chrono>
#include <atomic>

#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif
#include <assert.h>
#include <stdio.h>
// #include <string>

// a simple, thread-safe queue with (mostly) non-blocking reads and writes
namespace non_blocking {
template <class T>
class Queue {
    mutable std::mutex m;
    std::deque<T> data;
public:
    void push(T const &input) { 
        std::lock_guard<std::mutex> L(m);
        data.push_back(input);
    }

    bool pop(T &output) {
        std::lock_guard<std::mutex> L(m);
        if (data.empty())
            return false;
        output = data.front();
        data.pop_front();
        return true;
    }
};
}

// eastwsclient isn't thread safe, so this is a really simple
// thread-safe wrapper for it.
class Ws {
    std::thread runner;
    non_blocking::Queue<std::string> outgoing;
    non_blocking::Queue<std::string> incoming;
    std::atomic<bool> running { true };

public:
    void send(std::string const &s) { outgoing.push(s); }
    bool recv(std::string &s) { return incoming.pop(s); }

    Ws(std::string url) {
        using easywsclient::WebSocket;

        runner = std::thread([=] {
            std::unique_ptr<WebSocket> ws(WebSocket::from_url(url));
            while (running) {
                if (ws->getReadyState() == WebSocket::CLOSED)
                    break;
                std::string data;
                if (outgoing.pop(data))
                    ws->send(data);
                ws->poll();
                ws->dispatch([&](const std::string & message) {
                    incoming.push(message);
                });
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            ws->close();
            ws->poll();
        });
    }
    void close() { running = false; }
    ~Ws() { if (runner.joinable()) runner.join(); }
};

int main() {
    #ifdef _WIN32
        INT rc;
        WSADATA wsaData;

        rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (rc) {
            printf("WSAStartup Failed.\n");
            return 1;
        }
        printf("WSAStartup success.\n");
        // printf(rc);
    #endif

    // Ws socket("ws://192.168.1.101:4444");
    // Ws socket("ws://10.10.0.5:4444");
    // Ws socket("ws://127.0.0.1:4444");
    Ws socket("ws://localhost:4444");
    // Ws socket("ws://137.184.53.43:4444");
    // Ws socket("ws://10.116.0.2:4444");
    // Ws socket("wss://10.116.0.2:4444");
    // Ws socket("tcp://10.116.0.2:4444");
    // Ws socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    std::atomic<bool> run{true};
    auto receiver = std::thread([&] {
        printf("Started thread...\n");

        int connectionNotificationCount = 0;

        std::string s;
        while (run) {
            if(connectionNotificationCount == 0) {
                printf("Accepting connections...\n");
                connectionNotificationCount++;
            }

            if (socket.recv(s)) {
                printf("Received a connection...\n");
                connectionNotificationCount++;
                std::cout << s << '\n';
            }
            // printf("Thread sleeping...\n");
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            // printf("Thread done sleeping...\n");
            socket.close();
        }

        printf("Stopped thread...\n");
    });

    /*std::string line;
    while (std::getline(std::cin, line))
        socket.send(line);
    run = false;*/
    receiver.join();
    // socket.close();
}