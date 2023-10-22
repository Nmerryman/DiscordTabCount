// CountTestApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Make sure Windows.h doesn't include WinSock1.h
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <thread>
#include <WinSock2.h>
#include "discord_game_sdk/cpp/discord.h"  // This needs to point to discord.h. This is just how it worked on my setup.

#pragma comment(lib, "WS2_32")

#define APP_ID 1165228247173308467


// ----------------
// Discord callback stufff


void SetTabCount(discord::Core* core, int count) {
    // Build the activity
    discord::Activity activity{};
    char buffer[30];
    snprintf(buffer, 30, "I have %d tabs open.", count);
    activity.SetState(buffer);

    // Send the activity info to discord
    core->ActivityManager().UpdateActivity(activity, [](discord::Result res) {
        if (res == discord::Result::Ok) {
            printf("Activity Updated!\n");
        }
        else {
            printf("Activity failed with %d\n", res);
        }
        });
}



// ----------------------
// Threading stuff here


void NumServer(int* count) {
    // https://beej.us/guide/bgnet/html/split-wide/intro.html#cb6-3
    // Make sure that the sockets are ready for use.
    WSADATA wsaData;
    
    // Make sure the sockets boot with the right version
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(1);
    }

    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2)
    {
        fprintf(stderr, "Version 2.2 of Winsock is not available.\n");
        shuttingDown:
        WSACleanup();  // <=== REMEMBER TO CALL THIS TO CLOSE
        exit(2);
    }
    printf("No socket start issues\n");

    // Get UDP socket instance
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1) {
        printf("Failed to create socket\n");
        goto shuttingDown;
    }

    struct sockaddr_in sadd;
    sadd.sin_family = AF_INET;
    sadd.sin_addr.s_addr = INADDR_ANY;
    sadd.sin_port = htons(5566);

    // Bind to port 5566
    if (bind(sockfd, (sockaddr*)&sadd, sizeof(sadd)) < 0) {
        printf("Failed to bind\n");
        goto shuttingDown;
    }

    printf("Binded\n");
    char recvBuff[100];

    SOCKADDR senderAddr;
    int senderSize = sizeof(senderAddr);

    // Keep waiting for the next tab count update
    while (true) {
        int res = recvfrom(sockfd, recvBuff, 100, 0, &senderAddr, &senderSize);

        if (res == SOCKET_ERROR) {
            printf("Failed to recieve\n");
            goto shuttingDown;
        }
        // The python server simply sends an int as bytes. This works because the python server is hardcoded to use 4 bytes,
        // which matches the size of an int in c++
        printf("Recieved: %d\n", *(int*)recvBuff);
        *count = *(int*)recvBuff;  // Update the current count value for use on next discord update

    }

    // I don't expect this stuff to ever get run.
    closesocket(sockfd);


    printf("NO issues at end\n");

    goto shuttingDown;
}


// ---------------
// Mainloop stuff here


void doGameStuff(discord::Core* &core, int* tabCount) {
    std::cout << "Hello World!\n";

    // Run through the mainloop to try to trigger callbacks.
    discord::Result res;
    while (true) {
        SetTabCount(core, *tabCount);  // Set callback to send status to discord
        Sleep(5000);  // Discord wants at least a 4s delay between each update
        printf("Ask\n");
        res = core->RunCallbacks();  // Run all callbacks if ready
        if (res != discord::Result::Ok) {
            printf("Error with %d", res);
        }
    }

}

int main()
{
    // This holds the tab count
    int tabCount = 0;
    int* countPtr = &tabCount;

    // Start the socket listener
    std::thread t(NumServer, countPtr);

    // Create the Manager manager and enter the mainloop
    discord::Core* core{};
    auto result = discord::Core::Create(APP_ID, DiscordCreateFlags_Default, &core);
    if (core) {
        doGameStuff(core, countPtr);
    }
    else {
        std::cout << "We failed to auth as game :(\n";
        std::cout << "Maybe I should display result info lol";
    }

    // Not sure if this even matters
    t.join();

}
