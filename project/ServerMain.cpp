#include "config/Version.h"

#include "source/network/server/Server.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <csignal>

using namespace project;

namespace
{
    auto GetTime()
    {
        auto timePoint = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
        return std::ctime(&time);
    }

    void OnQuit(int signal)
    {
        (void)signal;
        std::cout << "\nServer stopped: " << GetTime() << '\n';
        exit(0);
    }
}

int main()
{
    std::signal(SIGINT, OnQuit);

    std::cout << "\n----------------\n"
              << " Caverna Server\n"
              << "     v" << NC_PROJECT_VERSION << '\n'
              << "----------------\n\n"
              << "Server started: " << GetTime() << '\n';

    network::Server server;

    while(true)
    {
        server.Poll(500u);
    }

    return 0;
}