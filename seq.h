#pragma once

#include <functional>
#include <concepts>
#include <vector>

namespace num
{
    template<std::floating_point T>
    using func = std::function<T(T)>;

    template<std::floating_point T>
    using seq = std::vector<std::pair<T, T>>;

    template<std::floating_point T>
    seq<T> make_seq(const func<T>& f, T min, T max, std::size_t n);
}

namespace num
{
    template<std::floating_point T>
    seq<T> make_seq(const func<T>& f, T min, T max, std::size_t n)
    {
        T h = (max - min) / (n - 1);
        seq<T> seq(n);
        for (std::size_t i = 0; i < n; i++)
        {
            auto x = min + i * h;
            seq[i] = { x, f(x) };
        }
        return seq;
    }
}
