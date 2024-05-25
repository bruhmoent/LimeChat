#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#ifndef SCROLLABLE_TEXT_AREA_HPP
#define SCROLLABLE_TEXT_AREA_HPP

class ScrollableTextArea : public Menu {
public:
    ScrollableTextArea(const sf::Vector2f& pos, float width, float height)
        : Menu(pos, false, true), m_menu_width(width), m_menu_height(height), m_pos(pos), m_isDragging(false) {
        m_view.setSize(width, height);
        m_view.setCenter(width / 2, height / 2);

        if (!m_font.loadFromFile("font.ttf")) {
            throw std::runtime_error("Failed to load font");
        }
    }

    ~ScrollableTextArea() {}

    void draw(sf::RenderWindow& window) override {
        sf::View originalView = window.getView();

        window.setView(m_view);

        sf::FloatRect visibleArea(
            m_view.getCenter().x - m_view.getSize().x / 2.f,
            m_view.getCenter().y - m_view.getSize().y / 2.f,
            m_view.getSize().x, m_view.getSize().y);

        // Draw only the text items that are within the visible area
        for (const auto& text : m_visibleTexts) {
            if (visibleArea.intersects(text.getGlobalBounds())) {
                window.draw(text);
            }
        }

        window.setView(originalView);
    }

    void handle_event(const sf::Event& event, sf::RenderWindow& window) {
        Menu::handle_event(event);

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                m_isDragging = true;
                m_lastMousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                m_isDragging = false;
            }
        }
        else if (event.type == sf::Event::MouseMoved) {
            if (m_isDragging) {
                sf::Vector2f currentMousePosition = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                sf::Vector2f delta = m_lastMousePosition - currentMousePosition;
                m_view.move(delta);
                m_lastMousePosition = currentMousePosition;
            }
        }
        else if (event.type == sf::Event::MouseWheelScrolled) {
            m_view.move(0, event.mouseWheelScroll.delta * -10);
        }
    }

    void add_string(const std::string& new_line) {
        float margin_x = 20.f;
        float margin_y = 5.f;

        sf::Text text;
        text.setFont(m_font);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        text.setString(new_line);

        m_visibleTexts.push_back(text);

        // Adjust the positions of all text items
        float currentY = m_pos.y + m_menu_height - margin_y;
        for (auto it = m_visibleTexts.rbegin(); it != m_visibleTexts.rend(); ++it) {
            it->setPosition(m_pos.x + margin_x, currentY - it->getLocalBounds().height);
            currentY -= it->getLocalBounds().height + margin_y;
        }

        // Adjust the view's center to ensure the latest message is visible
        float viewBottom = m_pos.y + m_menu_height;
        float contentBottom = m_visibleTexts.back().getPosition().y + m_visibleTexts.back().getLocalBounds().height + margin_y;
        if (contentBottom > viewBottom) {
            m_view.setCenter(m_menu_width / 2, contentBottom - m_menu_height / 2);
        }
    }

private:
    sf::View m_view;
    sf::Font m_font;
    float m_menu_width;
    float m_menu_height;
    sf::Vector2f m_accumulatedMouseDelta;
    sf::Vector2f m_pos;
    std::vector<sf::Text> m_visibleTexts;
    bool m_isDragging;
    sf::Vector2f m_lastMousePosition;
};

#endif // SCROLLABLE_TEXT_AREA_HPP