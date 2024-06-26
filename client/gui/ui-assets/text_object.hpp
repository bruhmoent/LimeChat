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

#include "../ui-util/directives.hpp"

#ifndef TEXT_OBJECT_HPP
#define TEXT_OBJECT_HPP

class TextObject
{
private:
    sf::Text m_text;
    sf::Font m_font;
    sf::Vector2f m_position;

public:
    TextObject(std::string text, float x, float y, unsigned int char_size, unsigned int r, unsigned int g, unsigned int b)
    {
        if (!m_font.loadFromFile("font.ttf"))
        {
            // Failed to load the font
            std::cout << "Failed to load the font" << std::endl;
        }

        m_text.setFont(m_font);
        m_text.setCharacterSize(char_size);
        m_text.setStyle(sf::Text::Regular);
        set_position(x, y);
        m_text.setFillColor(sf::Color(r, g, b));
        m_text.setOutlineThickness(2.0f);
        m_text.setOutlineColor(sf::Color(0, 0, 0, 125));
        m_text.setString(text);
    }

    unsigned int get_character_size() const {
        return m_text.getCharacterSize();
    }

    void set_text(const std::string& text)
    {
        m_text.setString(text);
    }

    void set_position(float x, float y)
    {
        m_position.x = x;
        m_position.y = y;
        m_text.setPosition(m_position);
    }

    sf::Vector2f get_position() const
    {
		return m_position;
	}

    void set_color(int r, int g, int b);

    void set_size(int size);

    sf::FloatRect get_local_bounds() const;

    const sf::Text& get_text() const;

    const sf::Font& get_font() const;
};


#endif // TEXT_OBJECT_HPP