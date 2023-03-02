#ifndef __SLIM__CONSOLE__TEMP
#define __SLIM__CONSOLE__TEMP
namespace slim::console::colors {
    std::vector<std::string> colors {
        "default", "black", "red", "green", "yellow", "blue", "magenta", "cyan", "white", "bright black",
        "bright red", "bright green", "bright yellow", "bright blue", "bright magenta", "bright cyan", "bright white"
    };
    std::unordered_map<std::string, int> text {
        {"default", 39}, {"black", 30}, {"red", 31}, {"green", 32}, {"yellow", 33}, {"blue", 34},
        {"magenta", 35}, {"cyan", 36}, {"white", 37}, {"bright black", 90}, {"bright red", 91}, {"bright green", 92},
        {"bright yellow", 93}, {"bright blue", 94}, {"bright magenta", 95}, {"bright cyan", 96}, {"bright white", 97},
    };
    std::unordered_map<std::string, int> background {
        {"default", 49}, {"black", 40}, {"red", 41}, {"green", 42}, {"yellow", 43}, {"blue", 44},
        {"magenta", 45}, {"cyan", 46}, {"white", 47}, {"bright black", 100}, {"bright red", 101}, {"bright green", 102},
        {"bright yellow", 103}, {"bright blue", 104}, {"bright magenta", 105}, {"bright cyan", 106}, {"bright white", 107}
    };
}
#endif