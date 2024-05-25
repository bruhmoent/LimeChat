#include "menu.hpp"
#include "menu_item.hpp"
#include "input_field.hpp"

Menu::Menu(const sf::Vector2f& pos, bool resize_on_item, bool is_active)
    : m_resize_on_item(resize_on_item), m_pos(pos), m_is_active(is_active)
{
    sf::Vector2f size(m_menu_width, m_menu_height);
    m_background_shape.setPosition(m_pos);
    m_background_shape.setSize(size);
}

Menu::~Menu()
{
    for (auto item : m_items)
    {
        delete item;
    }
    delete m_background_sprite;
    m_items.clear();
}

void Menu::correct_size(MenuItem* menu_item)
{
    float width = menu_item->get_width();
    float height = menu_item->get_height();

    if (width > m_menu_width)
        m_menu_width = width;

    if (height > m_menu_height)
        m_menu_height = height;
}

void Menu::set_menu_width(float width)
{
    m_menu_width = width;
}

void Menu::set_menu_height(float height)
{
    m_menu_height = height;
}

void Menu::add_item_ptr(MenuItem* menu_item)
{
    if (m_resize_on_item)
        correct_size(menu_item);

    float item_y = m_pos.y;
    for (const auto& item : m_items)
    {
        item_y += item->get_height();
    }

    menu_item->set_position(m_pos.x, item_y);

    m_items.push_back(menu_item);

    m_menu_height += m_items.back()->get_height();

    if (m_active_item == -1)
        m_active_item = static_cast<int>(m_items.size()) - 1;
}

void Menu::add_string(const std::string& text)
{
    float margin_x = 20.f;
    float margin_y = 10.f;

    float item_width = m_menu_width - 2 * margin_x;
    float item_height = (m_menu_height - (m_items.size() + 1) * margin_y) / (m_items.size() + 1);

    auto item = new MenuItem(text, item_width, item_height);

    if (m_resize_on_item)
        correct_size(item);

    float total_items_height = m_items.size() * (item_height + margin_y);
    float vertical_margin = (m_menu_height - total_items_height) / 2;

    float item_y = m_pos.y + vertical_margin;

    for (const auto& menuItem : m_items)
    {
        item_y += menuItem->get_height() + margin_y;
    }

    float center_x = m_pos.x + (m_menu_width - item->get_width()) / 2;

    item->set_position(center_x, item_y);
    m_items.push_back(item);

    m_menu_height += item_height + margin_y;
    m_background_shape.setSize(sf::Vector2f(m_menu_width, m_menu_height));
}

void Menu::add_input_field(const std::string& placeholder, float width, float height)
{
    auto input_field = std::make_unique<InputField>(placeholder, width, height);

    float item_y = m_pos.y;
    for (const auto& item : m_items)
    {
        item_y += item->get_height();
    }
    for (const auto& input : m_input_fields)
    {
        item_y += input->m_height;
    }

    input_field->set_position(m_pos.x, item_y);
    m_input_fields.push_back(std::move(input_field));

    m_menu_height += m_input_fields.back()->m_height;
    m_background_shape.setSize(sf::Vector2f(m_menu_width, m_menu_height));
}

void Menu::draw(sf::RenderWindow& window)
{
    if (m_background_shape.getGlobalBounds().width > 0 && m_background_shape.getGlobalBounds().height > 0) {
        window.draw(m_background_shape);
    }

    if (m_background_sprite && m_background_sprite->getGlobalBounds().width > 0 && m_background_sprite->getGlobalBounds().height > 0) {
        window.draw(*m_background_sprite);
    }

    for (auto& item : m_items)
    {
        item->draw(window);
    }

    for (auto& input : m_input_fields)
    {
        input->draw(window);
    }
}

void Menu::handle_event(const sf::Event& event)
{
    for (auto& input : m_input_fields)
    {
        input->handle_event(event);
    }
}

void Menu::set_background_color(const sf::Color& color)
{
    m_background_color = color;
}

void Menu::set_background_sprite(sf::Sprite* sprite)
{
    m_background_sprite = sprite;
}

void Menu::set_background_rect(const sf::RectangleShape& rect)
{
    m_background_shape = rect;
}

float Menu::get_width()
{
    return m_menu_width;
}

void Menu::set_input_field_color(const sf::Color& text_color, const sf::Color& background_color, const sf::Color& placeholder_color) {
    for (auto& input : m_input_fields) {
        input->set_text_color(text_color);
        input->set_background_color(background_color);
    }
}

float Menu::get_height()
{
    return m_menu_height;
}

sf::Color Menu::get_background_color()
{
    return m_background_color;
}

sf::RectangleShape Menu::get_background_rect()
{
    return m_background_shape;
}

sf::Sprite* Menu::get_background_sprite()
{
    return m_background_sprite;
}

sf::Vector2f Menu::get_position()
{
    return m_pos;
}
