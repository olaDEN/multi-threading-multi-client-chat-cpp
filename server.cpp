//Disable inet_addr warnings/error
#pragma warning(disable:4996)


#include <WinSock2.h>
#include <string>
#include <iostream>
#include <exception>
#include <synchapi.h>

using namespace std;

SOCKET clientSock[2];
string clientsInfo[256];
string CONN = "CONN";
string MESG = "MESG";
string MERR = "MERR";
string GONE = "GONE";
string Quit = "quit";

//Handelr Thread
DWORD WINAPI HandelrThread(int index, SOCKET sListen, SOCKET newConnection){ //index = the index in the SOCKET clientSock array
    char buff[256]; //buff to get messages from client and send to clients
    //recv(clientSock[index], buff, sizeof(buff), NULL); //receive message from client    while ()
    string content = buff;
    while(content!=Quit) {
            recv(clientSock[index], buff, sizeof(buff), NULL); //receive message from client
            content = buff;
            string clientReceived = "\nReceived message from client ";
            string clientIndex = " [" + to_string(index) + "]: ";
            string message = clientReceived + clientIndex + content;

//            char serverReply[256] = "Server received the message."; //Create buff with message of the day
//            send(clientSock[index], serverReply, sizeof(serverReply), NULL); //Send serverReply buff
            cout << message.c_str() << endl;//Print what the client said

    }
    //cout << "Enter a command(CONN, MESG, MERR, GONE):\n " << endl;
    cout << "Client connection terminated" << endl;//Print what the client said
    return 0;
}


int main() {
    int Connections = 0;
    char commandBuf[256];
    //Winsock Startup
    WSAData wsaData;
    WORD DllVersion = MAKEWORD(2, 2);
    if (WSAStartup(DllVersion, &wsaData) !=
        0) //If WSAStartup returns anything other than 0, then that means an error has occured in the WinSock Startup.
    {
        MessageBoxA(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    SOCKADDR_IN addr; //Address that we will bind our listening socket to
    int addrlen = sizeof(addr); //length of the address (required for accept call)
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Broadcast locally
    addr.sin_port = htons(1111); //Port
    addr.sin_family = AF_INET; //IPv4 Socket

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket to listen for new clientSock
    bind(sListen, (SOCKADDR *) &addr, sizeof(addr)); //Bind the address to the socket
    listen(sListen,
           SOMAXCONN); //Places sListen socket in a state in which it is listening for an incoming connection. Note:SOMAXCONN = Socket Oustanding Max clientSock
    SOCKET newConnection; //Socket to hold the client's connection

    cout << "Server is waiting for connections...\nEnter quit to close the server." << endl;
    //cin.getline(commandBuf, sizeof(commandBuf));

    string command;
    string input = commandBuf;

    newConnection = accept(sListen, (SOCKADDR *) &addr, &addrlen);
    if (newConnection == 0) //If accepting the client connection failed
    {
        cout << "Failed to accept the client's connection." << endl;
        return 0;
    } else //If client connection properly accepted
    {
        char serverMesg[256] = "\nServer is connected"; //Create buff with message of the day
        send(newConnection, serverMesg, sizeof(serverMesg), NULL); //Send serverMesg buff
        clientSock[0] = newConnection; //Set socket in array to be the newest connection before creating the thread to handle this client's socket.
        Connections += 1; //Incremenent total # of clients that have connected
        cout << "New connection, client [" << clientSock[0] << "]," << "IP: " << inet_ntoa(addr.sin_addr)
             << "," << "Port: " << ntohs(addr.sin_port) << "," << endl;
        string c_sock = ") client: Socket: " + to_string(clientSock[0]) + ", Ip: ";
        string c_ip = inet_ntoa(addr.sin_addr);
        string c_port = " , Port: " + to_string(ntohs(addr.sin_port));
        string info = to_string(0) + c_sock + c_ip + c_port;
        clientsInfo[0] = info;

        char commandMSG[256];
        recv(newConnection, commandMSG, sizeof(commandMSG), NULL); //receive message from client
        command = commandMSG;
        cout<<"received command "<<command<<endl;
        while (input != Quit) {
            if (command == CONN) {
                cout<<"waiting for new clients.."<<endl;
                for (int i = 0; i < 2; i++) {
                    //int Connections = 0; //# of client clientSock
                    newConnection = accept(sListen, (SOCKADDR *) &addr, &addrlen); //Accept a new connection
                    if (newConnection == 0) //If accepting the client connection failed
                    {
                        cout << "Failed to accept the client's connection." << endl;
                        command = "BLAH";
                        break;
                    } else //If client connection properly accepted
                    {
                        char serverMesg[256] = "\nServer is connected"; //Create buff with message of the day
                        send(newConnection, serverMesg, sizeof(serverMesg), NULL); //Send serverMesg buff
                        int index = i+1;
                        clientSock[index] = newConnection; //Set socket in array to be the newest connection before creating the thread to handle this client's socket.
                        Connections += 1; //Incremenent total # of clients that have connected
                        cout << "New connection, client [" << clientSock[i] << "]," << "IP: " << inet_ntoa(addr.sin_addr)
                             << "," << "Port: " << ntohs(addr.sin_port) << "," << endl;
                        for (int j = 0; j < 3; j++) //For each client connection
                        {
                            char notifyMSG[256] = "****A new client joined the chat !****";
                            if (j == i) //Don't send the chat message to the same user who sent it
                                continue; //Skip user
                            send(clientSock[j], notifyMSG, sizeof(notifyMSG), NULL);//send the chat message to this client
                        }
                        //}

                        string c_sock = ") client: Socket: " + to_string(clientSock[index]) + ", Ip: ";
                        string c_ip = inet_ntoa(addr.sin_addr);
                        string c_port = " , Port: " + to_string(ntohs(addr.sin_port));
                        int j = index;
                        string info = to_string(j) + c_sock + c_ip + c_port;
                        clientsInfo[index] = info;
                        //Thread for handling client:
                        Sleep(5000);
                        int Data_Of_Thread = 1;
                        //HANDLE threadStatus =
                        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) HandelrThread, (LPVOID) (i), NULL,
                                     NULL); //Create Thread to handle this client. The index in the socket array for this thread is the value (i).
                        //                if (threadStatus == NULL)
                        //                    ExitProcess(Data_Of_Thread);
                        //                WaitForSingleObject(threadStatus, INFINITE);

                    }
                    cout << "Checking if any other clients..\n" << endl;
                    Sleep(5000);
                }
                cout << "\nList of current clients:" << endl;
                if (Connections > 1) {
                    for (int j = 0; j < 3; j++) {
                        cout << clientsInfo[j] << endl;
                        //cout << j++ <<": [" << clientSock[j] << "]," << "IP: " << inet_ntoa(addr.sin_addr)<< ","<< "Port: " << ntohs(addr.sin_port) << "," << endl;
                    }
                } else {
                    cout << clientsInfo[0] << endl;
                }

                Sleep(10000);
                //char commandBuf2[256];
                // cout << "\nEnter quit" << endl;
                //cin.getline(commandBuf2, sizeof(commandBuf2));
                //input = commandBuf2;
            }
            else if(command == "MESG")
            cout<<"NO CONN COMMAND"<<endl;
            input = Quit;
        }
    }

    system("pause");
    return 0;
}


