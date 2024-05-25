//  AcornUI
//  Copyright (C) 2023 bruhmoent
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

#include "../ui-util/directives.hpp"

#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class MenuItem;
class InputField;

class Menu
{
public:
    std::vector<MenuItem*> m_items;
    std::vector<std::unique_ptr<InputField>> m_input_fields;

    bool m_is_active = false;

    Menu(const sf::Vector2f& pos, bool resize_on_item, bool is_active);
    virtual ~Menu();

    MenuItem& get_item(int index) { return *(m_items[index]); }

    void add_item_ptr(MenuItem* menu_item);

    virtual void add_string(const std::string& text);

    void correct_size(MenuItem* menu_item);

    virtual void draw(sf::RenderWindow& window);

    void set_menu_width(float width);

    void set_menu_height(float height);

    virtual void handle_event(const sf::Event& event);

    void add_input_field(const std::string& placeholder, float width, float height);

    float get_width();

    float get_height();

    void set_background_color(const sf::Color& color);

    void set_background_rect(const sf::RectangleShape& rect);

    void set_background_sprite(sf::Sprite* sprite);

    sf::Color get_background_color();

    sf::RectangleShape get_background_rect();

    sf::Sprite* get_background_sprite();
    
    sf::Vector2f get_position();

    void set_input_field_color(const sf::Color& text_color, const sf::Color& background_color, const sf::Color& placeholder_color);

protected:
    float m_menu_width = 0.f;
    float m_menu_height = 0.f;

    sf::Color m_background_color = sf::Color::White;
    sf::RectangleShape m_background_shape;
    sf::Sprite* m_background_sprite = nullptr;

    sf::Vector2f m_pos = { 0.f, 0.f };

    bool m_resize_on_item = true;

protected:
    int m_active_item = -1;
};

#endif // MENU_HPP
