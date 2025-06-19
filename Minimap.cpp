#include "Minimap.h"

Minimap::Minimap(float worldWidth, float worldHeight, const sf::Font& font)
: show(false)
{
    // Dimensions for the small icon
    float mapW    = 200.f;
    float mapH    = 150.f;
    float padding = 10.f;

    // 1) Icon button (always drawn in screen-space later)
    button.setSize(sf::Vector2f(mapW, mapH));
    button.setFillColor(sf::Color(50,50,50,200));
    button.setOutlineColor(sf::Color::White);
    button.setOutlineThickness(2.f);

    // initial position (will be repositioned each frame in draw())
    button.setPosition(worldWidth - mapW - padding, padding);

    // 2) Popup background (will be centered in draw())
    float popupW = mapW * 2.5f;
    float popupH = mapH * 2.5f;
    popup.setSize(sf::Vector2f(popupW, popupH));
    popup.setFillColor(sf::Color(0,0,0,230));

    // 3) Close button inside popup
    float closeSize = 24.f;
    closeBtn.setSize(sf::Vector2f(closeSize, closeSize));
    closeBtn.setFillColor(sf::Color(200,50,50));

    closeX.setFont(font);
    closeX.setString("×");
    closeX.setCharacterSize(20);
    closeX.setFillColor(sf::Color::White);

    // 4) Compute scaling from world → minimap icon
    scaleX = mapW / worldWidth;
    scaleY = mapH / worldHeight;
}

void Minimap::handleEvent(const sf::Event& e, sf::RenderWindow& window) {
    if (e.type == sf::Event::MouseButtonPressed &&
        e.mouseButton.button == sf::Mouse::Left)
    {
        // pixel coords
        sf::Vector2f click{float(e.mouseButton.x), float(e.mouseButton.y)};
        if (!show && button.getGlobalBounds().contains(click))
            show = true;
        else if (show && closeBtn.getGlobalBounds().contains(click))
            show = false;
    }
}

void Minimap::draw(sf::RenderWindow& window,
                   const std::vector<sf::RectangleShape>& walls,
                   const Player& player,
                   Enemy& enemy)
{
    // 1) Draw in screen-space
    window.setView(window.getDefaultView());

    // 2) Reposition icon in top‐right each frame
    float padding = 10.f;
    auto winSz = window.getSize();
    button.setPosition(
        float(winSz.x) - button.getSize().x - padding,
        padding
    );
    window.draw(button);

    if (!show)
        return;

    // 3) Center and draw popup
    popup.setPosition(
        (winSz.x - popup.getSize().x) / 2.f,
        (winSz.y - popup.getSize().y) / 2.f
    );
    window.draw(popup);

    // 4) Draw walls scaled
    for (auto& w : walls) {
        sf::RectangleShape copy = w;
        auto sz  = w.getSize();
        auto pos = w.getPosition();
        copy.setSize(sf::Vector2f(sz.x * scaleX,
                                 sz.y * scaleY));
        copy.setPosition(popup.getPosition().x + pos.x * scaleX,
                         popup.getPosition().y + pos.y * scaleY);
        copy.setFillColor(sf::Color::White);
        window.draw(copy);
    }

    // 5) Player dot
    {
        sf::CircleShape dot(5.f);
        auto p = player.getPosition();
        dot.setPosition(popup.getPosition().x + p.x * scaleX - 5.f,
                        popup.getPosition().y + p.y * scaleY - 5.f);
        dot.setFillColor(sf::Color::Green);
        window.draw(dot);
    }

    // 6) Enemy dot
    {
        sf::CircleShape dot(5.f);
        auto epos = enemy.shape().getPosition();
        dot.setPosition(popup.getPosition().x + epos.x * scaleX - 5.f,
                        popup.getPosition().y + epos.y * scaleY - 5.f);
        dot.setFillColor(sf::Color::Red);
        window.draw(dot);
    }

    // 7) Reposition & draw close button and '×'
    closeBtn.setPosition(
        popup.getPosition().x + popup.getSize().x - closeBtn.getSize().x - 5.f,
        popup.getPosition().y + 5.f
    );
    closeX.setPosition(closeBtn.getPosition() + sf::Vector2f(2.f, -2.f));
    window.draw(closeBtn);
    window.draw(closeX);
}

