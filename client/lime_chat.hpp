#ifndef LIME_CHAT_HPP
#define LIME_CHAT_HPP

#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>

class LimeChat {
private:
    std::string serverIp;
    int serverPort;
    SOCKET clientSocket;
    bool running;
    bool authenticated;
    std::string username;
    std::string password;
    std::vector<std::string> chatMessages;
    void InitializeNetworking();
    void SendCredentials();
    void RequestPastMessages(int channelId);
    void ProcessMessage(const std::string& message);
    void ProcessPastMessages(const std::string& message);
    void ProcessRegularMessage(const std::string& message);

public:
    LimeChat(const std::string& serverIp, int serverPort);
    ~LimeChat();
    void Run(bool& newMessagesReceivedFlag);
    bool isRunning() const { return running; }
    bool isAuthenticated() const { return authenticated; }
    std::string getUsername() const { return username; }
    std::string getPassword() const { return password; }    
    std::vector<std::string> getChatMessages() const {
        return chatMessages;
    }
    void Stop() {
        running = false;
    }
    void HandleIncomingMessages(bool& newMessagesReceivedFlag);
    void SendMessage(const std::string& messageContent, const std::string& username, const std::string& password);
};

#endif // LIME_CHAT_HPP
