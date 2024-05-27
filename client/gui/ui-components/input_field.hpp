//  AcornUI
//  Copyright (C) 2024 bruhmoent
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INPUT_FIELD_HPP
#define INPUT_FIELD_HPP

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <memory>

class TextObject;

class InputField
{
public:
    using EnterCallback = std::function<void(const std::string&)>;

    InputField(const std::string& placeholder, float width, float height);
    virtual ~InputField();

    virtual void draw(sf::RenderWindow& window);
    void handle_event(const sf::Event& event);
    void clear();

    void set_position(float x, float y);
    void set_background_color(const sf::Color& color);
    void set_text_color(const sf::Color& color);
    void set_placeholder(const std::string& placeholder);
    void set_placeholder_color(const sf::Color& color);
    void set_enter_callback(EnterCallback callback);
    std::string get_text() const;

    float m_width;
    float m_height;
private:
    EnterCallback m_enterCallback;
    std::string m_text;
    std::string m_placeholder;
    sf::Color m_text_color;
    sf::Color m_placeholder_color;
    sf::Color m_background_color;
    sf::Clock m_cursor_timer;
    sf::RectangleShape m_background_shape;
    sf::RectangleShape m_cursor;
    std::unique_ptr<TextObject> m_text_object;
    sf::Vector2f m_pos;
    float m_cursorPosition;
    float m_scrollOffset;
    float m_initialTextHeight;
    float m_cursorOffset = 0.f;
    bool m_focused;

    void update_cursor_position();
    void process_input(const sf::Event::TextEvent& text_event);
    void handle_backspace();
};

#endif // INPUT_FIELD_HPP
