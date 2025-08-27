#include "random_utils.h"
#include <random>
#include <algorithm>

std::array<double, 2> generate_rand_arr()
{
    static std::random_device rnd_device;
    static std::mt19937 rnd_engine{rnd_device()};
    static std::uniform_real_distribution<double> dist{0.0, 1.0};

    auto gen = [&]()
    {
        return dist(rnd_engine);
    };

    std::array<double, 2> rgb;
    std::generate(rgb.begin(), rgb.end(), gen);

    return rgb;
}

double generate_rand_single(){
    static std::random_device rnd_device;
    static std::mt19937 rnd_engine{rnd_device()};
    static std::uniform_real_distribution<double> dist{0, 1.0};
    return dist(rnd_engine);
}