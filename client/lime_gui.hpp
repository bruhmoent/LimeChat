#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <thread>
#include <sstream>
#include <ctime>
#include <unordered_set>
#include <iomanip>
#include "gui/ui-components/menu.hpp"
#include "gui/ui-components/input_field.hpp"
#include "gui/ui-util/menu_util.hpp"
#include "gui/ui-assets/text_object.hpp"
#include "gui/ui-components/scrollable_text_area.hpp"
#include "lime_chat.hpp"

#ifndef LIME_GUI_HPP
#define LIME_GUI_HPP

class LimeGUI {
public:
    LimeGUI(const std::string& serverIp, int serverPort) : chatClient(serverIp, serverPort),
        window(sf::VideoMode(640, 480), "Lime Chat"),
        textObject("Default Text", 40.0f, 40.0f, 16, 0, 0, 0),
        newMessagesReceived(false) {

        window.setFramerateLimit(60);
        menuUtil = std::make_unique<MenuUtil>();

        messageDisplayMenu = std::make_unique<ScrollableTextArea>(sf::Vector2f(0, 20), 600, 400);
        //messageDisplayMenu->set_background_color(sf::Color(0, 0, 0, 0));
        menuUtil->add_menu(messageDisplayMenu.get());

        if (!backgroundTexture.loadFromFile("background.png")) {
            std::cerr << "Failed to load background texture!" << std::endl;
            return;
        }

        backgroundSprite = sf::Sprite(backgroundTexture);

        inputMenu = std::make_unique<Menu>(sf::Vector2f(20, 0), false, true);
        inputMenu->set_background_color(sf::Color(200, 200, 200));
        inputField = std::make_unique<InputField>("Enter your message", 400, 40);
        inputField->set_position(20, 0);
        inputField->set_background_color(sf::Color::White);
        inputField->set_text_color(sf::Color::Black);
        inputMenu->add_input_field("Enter your message", 400, 40);
        inputField->set_enter_callback([this](const std::string& message) {
            if (!message.empty()) {
                auto now = std::chrono::system_clock::now();
                auto now_c = std::chrono::system_clock::to_time_t(now);
                std::tm tm_time;
                localtime_s(&tm_time, &now_c); // Convert time to local time
                std::stringstream ss;
                ss << "[" << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S") << "]";
                std::string current_time = ss.str();

                if(message != "")
                { 
                    chatClient.SendMessage(message, chatClient.getUsername(), chatClient.getPassword());
                    chatMessages.push_back(current_time + " <" + chatClient.getUsername() + ">: " + message);
                    newMessagesReceived = true;
                }
            }
            });
        menuUtil->add_menu(inputMenu.get());
    }

    void run() {
        std::thread clientThread(&LimeChat::Run, &chatClient, std::ref(newMessagesReceived));

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                inputField->handle_event(event);
                messageDisplayMenu->handle_event(event, window);
            }

            window.clear(sf::Color(1, 52, 32));

            window.draw(backgroundSprite);
            menuUtil->draw_menus(window);
            inputField->draw(window);

            // Only update chat messages if new messages were received
            if (newMessagesReceived) {
                displayChatMessages();
                newMessagesReceived = false;
            }

            window.display();
        }

        if (clientThread.joinable()) {
            clientThread.join();
        }
    }

private:
    LimeChat chatClient;
    std::unique_ptr<MenuUtil> menuUtil;
    std::unique_ptr<InputField> inputField;
    std::unique_ptr<ScrollableTextArea> messageDisplayMenu;
    std::unique_ptr<Menu> inputMenu;
    sf::RenderWindow window;
    TextObject textObject;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    std::vector<std::string> chatMessages;
    std::unordered_set<std::string> addedMessages;
    bool newMessagesReceived;

    void displayChatMessages() {
        std::vector<std::string> latestMessages = chatClient.getChatMessages();

        chatMessages.insert(chatMessages.end(), latestMessages.begin(), latestMessages.end());

        for (const auto& message : chatMessages) {
            std::istringstream iss(message);
            std::string messageContent;
            std::getline(iss, messageContent, '|'); // Read message content only

            if (addedMessages.find(messageContent) == addedMessages.end()) {
                messageDisplayMenu->add_string(messageContent);
                addedMessages.insert(messageContent);
            }
        }
    }
};

#endif // LIME_GUI_HPP