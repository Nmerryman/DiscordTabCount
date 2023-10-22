// CountTestApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Make sure Windows.h doesn't include WinSock1.h
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <thread>
#include <WinSock2.h>
#include "discord_game_sdk/cpp/discord.h"

#pragma comment(lib, "WS2_32")

#define APP_ID 1165228247173308467
#define MY_ID 532751332445257729
#define OTHER_ID 155385446241665024



// ----------------
// Discord callback stufff


void PrintCurrent(discord::Core* core) {
    // On current user update try to print current username

    discord::User* user{};
    printf("Current Callback\n");
    discord::Result res = core->UserManager().GetCurrentUser(user);
    if (res == discord::Result::Ok) {
        printf("Current User is: %s\n", user->GetUsername());
    }
    else {
        printf("Current user errored with: %d. (4=internal)\n", res);
    }

}


void PrintJakeInfo(discord::Core* core) {
    // Register callback to simply print out the current user name
    discord::User user{};
    core->UserManager().GetUser(OTHER_ID, [&user](discord::Result result, discord::User u) {
        if (result == discord::Result::Ok) {
            printf("We got user (%s).\n", u.GetUsername());
        }
        else {
            printf("No users caught.\n");
        }
    });

}


void TryToUpdateActivity(discord::Core* core) {
    discord::Activity activity{};

    activity.SetState("WE PLAYING");
    activity.SetDetails("We try");

    core->ActivityManager().UpdateActivity(activity, [](discord::Result res) {
        if (res == discord::Result::Ok) {
            printf("Activity Updated!\n");
        }
        else {
            printf("Activity failed with %d\n", res);
        }
        });

}


void SetTabCount(discord::Core* core, int count) {
    discord::Activity activity{};
    char buffer[30];
    snprintf(buffer, 30, "I have %d tabs open.", count);
    activity.SetState(buffer);

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


void TestThread(const char* text) {
    Sleep(2000);
    printf("Testing %s", text);
}


void NumServer(int* count) {
    // https://beej.us/guide/bgnet/html/split-wide/intro.html#cb6-3
    // Make sure that the sockets are ready for use.
    WSADATA wsaData;
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(1);
    }

    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2)
    {
        fprintf(stderr, "Versiion 2.2 of Winsock is not available.\n");
        shuttingDown:
        WSACleanup();  // <=== REMEMBER TO CALL THIS TO CLOSE
        exit(2);
    }
    printf("No socket start issues\n");

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1) {
        printf("Failed to create socket\n");
        goto shuttingDown;
    }

    struct sockaddr_in sadd;
    sadd.sin_family = AF_INET;
    sadd.sin_addr.s_addr = INADDR_ANY;
    sadd.sin_port = htons(5566);


    if (bind(sockfd, (sockaddr*)&sadd, sizeof(sadd)) < 0) {
        printf("Failed to bind\n");
        goto shuttingDown;
    }

    printf("Binded\n");
    char recvBuff[100];

    SOCKADDR senderAddr;
    int senderSize = sizeof(senderAddr);

    while (true) {
        int res = recvfrom(sockfd, recvBuff, 100, 0, &senderAddr, &senderSize);

        if (res == SOCKET_ERROR) {
            printf("Failed to recieve\n");
            goto shuttingDown;
        }
        printf("Recieved: %d\n", *(int*)recvBuff);
        *count = *(int*)recvBuff;

    }

    closesocket(sockfd);


    printf("NO issues at end\n");

    goto shuttingDown;
}



void doGameShit(discord::Core* &core, int* tabCount) {
    std::cout << "Hello World!\n";

    // Try to search for Jake
    //PrintJakeInfo(core);

    // Register a update callback to get current User (Doesn't seem to work)
    //core->UserManager().OnCurrentUserUpdate.Connect([&core]() {PrintCurrent(core); });




    // Run through the mainloop to try to trigger callbacks.
    discord::Result res;
    while (true) {
        SetTabCount(core, *tabCount);
        Sleep(5000);
        printf("Ask\n");
        res = core->RunCallbacks();
        if (res != discord::Result::Ok) {
            printf("Error with %d", res);
        }
    }

}

int main()
{
    int tabCount = 0;
    int* countPtr = &tabCount;

    std::thread t(NumServer, countPtr);

    // Create the Manager manager lol
    discord::Core* core{};
    auto result = discord::Core::Create(APP_ID, DiscordCreateFlags_Default, &core);
    if (core) {
        doGameShit(core, countPtr);
    }
    else {
        std::cout << "We failed to auth as game :(\n";
        std::cout << "Maybe I should display result info lol";
    }


    t.join();

}
