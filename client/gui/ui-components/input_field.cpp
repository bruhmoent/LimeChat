#include "input_field.hpp"
#include "../ui-assets/text_object.hpp"

InputField::InputField(const std::string& placeholder, float width, float height)
    : m_placeholder(placeholder), m_width(width), m_height(height), m_focused(false),
    m_text_color(sf::Color::Black), m_background_color(sf::Color::White), m_cursorPosition(0),
    m_scrollOffset(0), m_text_object(nullptr), m_initialTextHeight(0.0f), m_placeholder_color(sf::Color(200, 200, 200)){
    m_background_shape.setSize(sf::Vector2f(width, height));
    m_background_shape.setFillColor(m_background_color);

    m_text_object = std::make_unique<TextObject>(placeholder, 5.f, (height - 16.f) / 2.f, 16, 0, 0, 0);
    m_text_object->set_color(m_placeholder_color.r, m_placeholder_color.g, m_placeholder_color.b);

    m_cursor.setFillColor(sf::Color::Black);
    m_cursor.setSize(sf::Vector2f(2.f, height - 10));
    update_cursor_position();

    m_initialTextHeight = m_text_object->get_local_bounds().height;
}

InputField::~InputField() {}

void InputField::draw(sf::RenderWindow& window) {
    window.draw(m_background_shape);

    sf::View originalView = window.getView();

    sf::View textView(sf::FloatRect(m_scrollOffset, 0.f, m_width, m_height));
    textView.setViewport(sf::FloatRect(m_pos.x / window.getSize().x, m_pos.y / window.getSize().y, m_width / window.getSize().x, m_height / window.getSize().y));
    window.setView(textView);

    window.draw(m_text_object->get_text());

    if (m_focused) {
        if (m_cursor_timer.getElapsedTime().asSeconds() < 0.5f) {
            window.draw(m_cursor);
        }
        else if (m_cursor_timer.getElapsedTime().asSeconds() >= 1.f) {
            m_cursor_timer.restart();
        }
    }

    window.setView(originalView);
}

void InputField::handle_event(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
        if (m_background_shape.getGlobalBounds().contains(mousePos)) {
            m_focused = true;
            if (m_text.empty()) {
                m_text_object->set_text("");
                m_text_object->set_color(m_text_color.r, m_text_color.g, m_text_color.b);
            }
            float mouseX = mousePos.x - (m_pos.x + 5.f - m_scrollOffset);
            for (size_t i = 0; i <= m_text.size(); ++i) {
                if (mouseX < m_text_object->get_text().findCharacterPos(i).x) {
                    m_cursorPosition = i;
                    break;
                }
            }
            update_cursor_position();
        }
        else {
            m_focused = false;
            if (m_text.empty()) {
                m_text_object->set_text(m_placeholder);
                m_text_object->set_color(m_placeholder_color.r, m_placeholder_color.g, m_placeholder_color.b);
            }
        }
    }
    else if (event.type == sf::Event::TextEntered && m_focused) {
        process_input(event.text);
    }
    else if (event.type == sf::Event::KeyPressed && m_focused) {
        if (event.key.code == sf::Keyboard::Left) {
            if (m_cursorPosition > 0) {
                m_cursorPosition--;
                update_cursor_position();
            }
        }
        else if (event.key.code == sf::Keyboard::Right) {
            if (m_cursorPosition < m_text.length()) {
                m_cursorPosition++;
                update_cursor_position();
            }
        }
    }
}

void InputField::process_input(const sf::Event::TextEvent& textEvent) {
    if (textEvent.unicode == '\b') {
        handle_backspace();
    }
    else if (textEvent.unicode < 128 && textEvent.unicode != 13) { // ASCII characters
        if (m_text.empty()) {
            m_text_object->set_color(m_text_color.r, m_text_color.g, m_text_color.b);
        }
        m_text.insert(m_text.begin() + m_cursorPosition, static_cast<char>(textEvent.unicode));
        m_cursorPosition++;
        m_text_object->set_text(m_text);
        update_cursor_position();
    }
    else if (textEvent.unicode == 13) { // Enter key
        if (m_enterCallback) {
            m_enterCallback(m_text);
        }
        clear();
    }
}

void InputField::handle_backspace() {
    if (m_cursorPosition > 0 && m_cursorPosition <= m_text.length()) {
        m_text.erase(m_cursorPosition - 1, 1);
        m_cursorPosition--;
        m_text_object->set_text(m_text);
        update_cursor_position();
        if (m_text.empty()) {
            m_text_object->set_text(m_placeholder);
            m_text_object->set_color(m_placeholder_color.r, m_placeholder_color.g, m_placeholder_color.b);
        }
    }
}

void InputField::update_cursor_position() {
    float cursorX;
    
    if (m_cursorPosition == 0) {
        cursorX = m_pos.x;
    } else if (m_cursorPosition > 0 && m_cursorPosition <= m_text.length()) {
        cursorX = m_text_object->get_text().findCharacterPos(m_cursorPosition).x;
    } else {
        cursorX = m_text_object->get_text().findCharacterPos(m_text.length()).x;
    }

    float cursorVisibleX = cursorX - m_scrollOffset;

    if (cursorVisibleX < m_pos.x) {
        m_scrollOffset -= (m_pos.x - cursorVisibleX);
    } else if (cursorVisibleX > m_pos.x + m_width) {
        m_scrollOffset += (cursorVisibleX - (m_pos.x + m_width));
    }

    float textWidth = m_text_object->get_text().getGlobalBounds().width;
    float maxScrollOffset = std::max(0.f, textWidth - m_width);
    if (m_scrollOffset < 0) {
        m_scrollOffset = 0;
    } else if (m_scrollOffset > maxScrollOffset) {
        m_scrollOffset = maxScrollOffset;
    }

    m_text_object->set_position(m_pos.x - m_scrollOffset, m_pos.y + (m_height - m_initialTextHeight) / 2.f);

    float cursorY = m_pos.y + (m_height - m_cursor.getSize().y) / 2.f;
    m_cursor.setPosition(cursorX, cursorY);
}

void InputField::set_enter_callback(EnterCallback callback) {
    m_enterCallback = std::move(callback);
}

void InputField::set_position(float x, float y) {
    m_pos.x = x;
    m_pos.y = y;
    m_background_shape.setPosition(m_pos);
    m_text_object->set_position(x + 5.f - m_scrollOffset, y + (m_height - m_text_object->get_local_bounds().height) / 2.f);
    update_cursor_position();
}

void InputField::set_background_color(const sf::Color& color) {
    m_background_color = color;
    m_background_shape.setFillColor(m_background_color);
}

void InputField::set_text_color(const sf::Color& color) {
    m_text_color = color;
    if (!m_text.empty()) {
        m_text_object->set_color(m_text_color.r, m_text_color.g, m_text_color.b);
    }
}

void InputField::set_placeholder(const std::string& placeholder) {
    m_placeholder = placeholder;
    if (m_text.empty() && !m_focused) {
        m_text_object->set_text(m_placeholder);
        m_text_object->set_color(m_placeholder_color.r, m_placeholder_color.g, m_placeholder_color.b);
    }
}

std::string InputField::get_text() const {
    return m_text;
}

void InputField::clear() {
    m_text.clear();
    m_text_object->set_text(m_placeholder);
    m_text_object->set_color(m_placeholder_color.r, m_placeholder_color.g, m_placeholder_color.b);
    m_cursorPosition = 0;
    m_scrollOffset = 0;
    update_cursor_position();
}

void InputField::set_placeholder_color(const sf::Color& color) {
    m_placeholder_color = color;
}
