#include "client/lime_chat.hpp"
#include "client/lime_gui.hpp"

int main() {
    std::string serverIp = "192.168.1.169";
    int serverPort = 54000;

    LimeGUI limeGUI(serverIp, serverPort);
    limeGUI.run();

    return 0;
}
