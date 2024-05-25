#include "lime_chat.hpp"
#include <limits>
#include <sstream>
#include <unordered_set>
#pragma comment(lib, "ws2_32.lib")

LimeChat::LimeChat(const std::string& serverIp, int serverPort)
    : serverIp(serverIp), serverPort(serverPort), running(true), authenticated(false) {
    InitializeNetworking();
}

LimeChat::~LimeChat() {
    running = false;
    closesocket(clientSocket);
    WSACleanup();
}

void LimeChat::InitializeNetworking() {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &wsData);
    if (wsResult != 0) {
        std::cerr << "Can't start Winsock, Err #" << wsResult << std::endl;
        exit(1);
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }

    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr);

    if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed, Err #" << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(1);
    }

    std::cout << "Connected to server!" << std::endl;

    SendCredentials();
}

void LimeChat::SendCredentials() {
    std::cout << "Enter username: ";
    std::getline(std::cin, username);
    std::cout << "Enter password: ";
    std::getline(std::cin, password);

    SendMessage("", username, password);
}

void LimeChat::RequestPastMessages(int channelId) {
    // Send a request to the server to retrieve past messages for the channel
    std::string request = "GET_PAST_MESSAGES|" + std::to_string(channelId) + "\n";
    send(clientSocket, request.c_str(), request.size(), 0);
}

void LimeChat::HandleIncomingMessages(bool& newMessagesReceivedFlag) {
    char buffer[4096];
    int bytesReceived;

    while (running) {
        ZeroMemory(buffer, 4096);
        bytesReceived = recv(clientSocket, buffer, 4096, 0);

        if (bytesReceived > 0) {
            std::string message(buffer, bytesReceived);
            std::cout << "Received message from server: " << message << std::endl;

            ProcessMessage(message);
            newMessagesReceivedFlag = true;
        }
        else if (bytesReceived == 0) {
            std::cout << "Server disconnected" << std::endl;
            break;
        }
        else {
            std::cerr << "Error in recv(). Quitting" << std::endl;
            break;
        }
    }

    newMessagesReceivedFlag = false;
}


void LimeChat::ProcessMessage(const std::string& message) {
    std::istringstream iss(message);
    std::string line;

    while (std::getline(iss, line)) {
        if (line == "Authentication successful") {
            authenticated = true;
            RequestPastMessages(1);
        }
        else if (line == "Welcome to the chat server!") {
            std::cout << line << std::endl;
        }
        else if (line.find("GET_PAST_MESSAGES|") != std::string::npos) {
            ProcessPastMessages(line);
        }
        else {
            ProcessRegularMessage(line);
        }
    }
}

void LimeChat::ProcessPastMessages(const std::string& message) {
    // Extract channel ID from the message
    size_t pos = message.find('|');
    if (pos != std::string::npos) {
        int channelId = std::stoi(message.substr(pos + 1));
        RequestPastMessages(channelId);
    }
}

void LimeChat::ProcessRegularMessage(const std::string& message) {
    if (!message.empty()) {
        chatMessages.push_back(message);
    }
}

void LimeChat::SendMessage(const std::string& messageContent, const std::string& username, const std::string& password) {
    std::string message = messageContent + "|" + username + "|" + password + "\n";
    send(clientSocket, message.c_str(), message.size(), 0);
}

void LimeChat::Run(bool& newMessagesReceivedFlag) {
    std::thread receiveThread(&LimeChat::HandleIncomingMessages, this, std::ref(newMessagesReceivedFlag));

    std::string userInput;

    while (running) {
        std::cout << "Enter your message: ";
        std::getline(std::cin, userInput);

        if (userInput.empty() || userInput == "\n") {
            continue;
        }

        if (userInput == "/quit") {
            running = false;
        }
        else {
            // If authenticated, send the user input to the server as a message
            if (authenticated) {
                SendMessage(userInput, username, password);
            }
            else {
                // If not authenticated, re-send the credentials
                SendMessage("Please authenticate again", username, password);
            }
        }
    }

    receiveThread.join();
}