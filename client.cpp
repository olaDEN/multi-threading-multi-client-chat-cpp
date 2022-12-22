//Disable inet_addr warnings/error
#pragma warning(disable:4996)

#include <WinSock2.h> //For win sockets
#include <string>
#include <iostream>
using namespace std;
SOCKET Connection;//This client's connection to the server


void ReceiveChat()//clientThread
{
    char buffer[256]; //Create buffer to hold messages up to 256 characters
    recv(Connection, buffer, sizeof(buffer), NULL); //receive buffer
    cout << buffer << endl;

}

int main()
{
    //Winsock Startup
    WSAData wsaData;
    WORD DllVersion = MAKEWORD(2, 2);
    if (WSAStartup(DllVersion, &wsaData) != 0)
    {
        MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    SOCKADDR_IN addr; //Address to be binded to our Connection socket
    int sizeofaddr = sizeof(addr); //Need sizeofaddr for the connect function
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Address = localhost (this pc)
    addr.sin_port = htons(1111); //Port = 1111
    addr.sin_family = AF_INET; //IPv4 Socket

    Connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Set Connection socket
    if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) //If we are unable to connect...
    {
        MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
        exit(3); //Failed to Connect
    }

    //cout << "Connected to server: 127.0.0.1" << endl;
    cout << "\nWaiting for server response.." << endl;
    char CONN[256]="CONN"; //256 char buffer to send connect command
    char buffer[256]; //256 char buffer to send message
    string input = buffer;
    string quit = "quit";

    while (buffer!=quit){
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ReceiveChat, NULL, NULL, NULL); //Create the client thread that will receive any data that the server sends.

        char buffer2[256]; //Create buffer to hold messages up to 256 characters
        recv(Connection, buffer2, sizeof(buffer2), NULL); //New Client Notification
        cout << buffer2 << endl;

        cout << "\nPlease specify the client and the message to send:" << endl;
        cin.getline(CONN, sizeof(CONN)); //Get line if user presses enter and fill the buffer
        send(Connection, CONN, sizeof(CONN), NULL); //Send buffer
        Sleep(10);

        cout << "\nSend:" << endl;
        send(Connection, buffer, sizeof(buffer), NULL); //Send buffer
        Sleep(10);

        //Create buffer to hold messages up to 256 characters
        recv(Connection, buffer2, sizeof(buffer2), NULL); //receive buffer
        cout << buffer2 << endl;
    }

    cout << "Quitting the server" << endl;
    exit(3);
}


