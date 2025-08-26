#include "random_utils.h"
#include <random>
#include <algorithm>

std::array<int, 3> generate_rand()
{
    static std::random_device rnd_device;
    static std::mt19937 rnd_engine{rnd_device()};
    static std::uniform_int_distribution<int> dist{0, 255};

    auto gen = [&]()
    {
        return dist(rnd_engine);
    };

    std::array<int, 3> rgb;
    std::generate(rgb.begin(), rgb.end(), gen);

    return rgb;
}