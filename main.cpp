#include <random>

#include "plot.h"

int main()
{
    using real = float;

    real epsilon = std::numeric_limits<real>::epsilon();

    real min_x = -10, max_x = 10;
    std::mt19937 gen((std::random_device()()));
    std::uniform_real_distribution<real> distrib(min_x, max_x);
    auto rng = [&gen, &distrib]() -> real
    {
        return distrib(gen);
    };

    auto f   = [](real x) -> real { return  sin(x); };
    auto d3f = [](real x) -> real { return -cos(x); };

    auto delta_estimate = [epsilon](real h) -> real
    {
        return h / 4 + 9 * epsilon / (4 * h * h * h);
    };

    auto delta_calculate = [&rng, &f, &d3f](real h) -> real
    {
        std::size_t n = 32;
        real res = 0,
             x, df_exact, df_calc, df_delta;

        //for (std::size_t i = 0; i < n; i++)
        {
            x = rng();
            df_exact = d3f(x);
            df_calc  = (-f(x - 2 * h) + 6 * f(x) - 8 * f(x + h) + 3 * f(x + 2 * h)) / (4 * h * h * h);
            df_delta = std::abs(df_exact - df_calc);
            res = std::max(res, df_delta);
        }

        return res;
    };

    real h_min = 0, h_max = 0.125,
         e_min = 0, e_max = delta_estimate(h_max);
    auto delta_estimate_seq  = num::make_seq<real>(delta_estimate,  h_min, h_max, 1000),
         delta_calculate_seq = num::make_seq<real>(delta_calculate, h_min, h_max, 100000);

    num::plots::display<real>({
        num::plots::make_scatter(delta_calculate_seq, sf::Color::Blue),
        num::plots::make_plot(delta_estimate_seq, sf::Color::Yellow)
    }, h_min, h_max, e_min, e_max, { .width = 1600, .height = 1600, .resolution = 10, .title = "Numeric Differentiation" });

    return 0;
}
