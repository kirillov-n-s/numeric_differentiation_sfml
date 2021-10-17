#pragma once

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "seq.h"

namespace num::plots
{
    template<std::floating_point T>
    using plot_t = std::tuple<seq<T>, sf::Color, sf::PrimitiveType>;

    template<std::floating_point T>
    plot_t<T> make_plot(const seq<T> &seq, const sf::Color &color);

    template<std::floating_point T>
    plot_t<T> make_scatter(const seq<T> &seq, const sf::Color &color);

    struct display_kwargs
    {
        uint32_t    width      = 800,
                    height     = 800,
                    resolution = 10;
        std::string title      = "";
        sf::Color   fg_color   = sf::Color::White,
                    bg_color   = sf::Color::Black;
    };

    template<std::floating_point T>
    void display(const std::vector<plot_t<T>> &plots,
                 T from_x, T to_x,
                 T from_y, T to_y,
                 const display_kwargs &kwargs = {});
}

namespace num::plots
{
    template<std::floating_point T>
    plot_t<T> make_plot(const seq <T> &seq, const sf::Color &color)
    {
        return { seq, color, sf::LineStrip };
    }

    template<std::floating_point T>
    plot_t<T> make_scatter(const seq <T> &seq, const sf::Color &color)
    {
        return { seq, color, sf::Points };
    }

    template<std::floating_point T>
    void display(const std::vector<plot_t<T>> &plots,
                 T from_x, T to_x,
                 T from_y, T to_y,
                 const display_kwargs &kwargs)
    {
        auto n = plots.size();

        const auto& [width, height, resolution, title, fg_color, bg_color] = kwargs;

        float feature_scale = (float)width / (float)(display_kwargs{}.width),
              width_eff     = width  * 0.975f,
              height_eff    = height * 0.975f,
              offset_x      = width  * 0.1f,
              offset_y      = height * 0.1f,
              scale_x       = (width  - offset_x) / (to_x - from_x),
              scale_y       = (height - offset_y) / (to_y - from_y);

        std::vector<sf::VertexArray> bufs(n);
        for (std::size_t i = 0; i < n; i++)
        {
            auto& [seq, color, type] = plots[i];

            auto k = seq.size();
            bufs[i] = sf::VertexArray(type, k);
            for (std::size_t j = 0; j < k; j++)
            {
                const auto& [x, y] = seq[j];
                bufs[i][j] = sf::Vertex(sf::Vector2f(offset_x + (x - from_x) * scale_x,
                                                     height - (offset_y + (y - from_y) * scale_y)),
                                        (x >= from_x && y >= from_y) ? color : sf::Color::Transparent);
            }
        }

        float x = offset_x, y = height - offset_y;
        auto start = sf::Vector2f(x, y);
        sf::VertexArray axis_x(sf::Lines, 2), axis_y(sf::Lines, 2);
        axis_x[0] = axis_y[0] = sf::Vertex(start, fg_color);
        axis_x[1] = sf::Vertex(sf::Vector2f(width_eff, y), fg_color);
        axis_y[1] = sf::Vertex(sf::Vector2f(x, height - height_eff), fg_color);

        sf::VertexArray measures_x(sf::Lines, resolution * 2), measures_y(sf::Lines, resolution * 2);
        float step_x = (width_eff  - offset_x) / resolution,
              step_y = (height_eff - offset_y) / resolution,
              seg    = 5 * feature_scale;
        for (std::size_t i = 0; i < resolution * 2; i += 2)
        {
            auto j = (i / 2 + 1);
            measures_x[i]     = sf::Vertex(sf::Vector2f(x + j * step_x, y - seg), fg_color);
            measures_x[i + 1] = sf::Vertex(sf::Vector2f(x + j * step_x, y + seg), fg_color);
            measures_y[i]     = sf::Vertex(sf::Vector2f(x - seg, y - j * step_y), fg_color);
            measures_y[i + 1] = sf::Vertex(sf::Vector2f(x + seg, y - j * step_y), fg_color);
        }

        sf::Font font;
        font.loadFromFile("..\\cambria.ttf");
        uint32_t fontsize = 12 * feature_scale;
        std::vector<sf::Text> labels_x(resolution + 1), labels_y(resolution + 1);
        float h_x = (to_x - from_x) / resolution,
              h_y = (to_y - from_y) / resolution;
        for (std::size_t i = 0; i <= resolution; i++)
        {
            labels_x[i] = sf::Text(std::to_string(from_x + i * h_x), font, fontsize);
            //labels_x[i].setPosition(x + i * step_x - labels_x[i].getLocalBounds().width / 2, y + 3 * seg);
            const auto& bounds_x = labels_x[i].getLocalBounds();
            labels_x[i].setPosition(x + i * step_x + bounds_x.height, y + 3 * seg);
            labels_x[i].setRotation(90);
            labels_x[i].setFillColor(fg_color);

            labels_y[i] = sf::Text(std::to_string(from_y + i * h_y), font, fontsize);
            const auto& bounds_y = labels_y[i].getLocalBounds();
            labels_y[i].setPosition(x - 3 * seg - bounds_y.width, y - i * step_y - bounds_y.height);
            labels_y[i].setFillColor(fg_color);
        }

        sf::RenderWindow window(sf::VideoMode(width, height), title);
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
                if (event.type == sf::Event::Closed)
                    window.close();

            window.clear(bg_color);

            for (const auto& plot : bufs)
                window.draw(plot);

            window.draw(axis_x);
            window.draw(axis_y);

            window.draw(measures_x);
            window.draw(measures_y);

            for (const auto& label : labels_x)
                window.draw(label);
            for (const auto& label : labels_y)
                window.draw(label);

            window.display();
        }
    }
}
