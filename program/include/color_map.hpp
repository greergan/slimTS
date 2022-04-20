#ifndef __SLIM__COLOR__MAP__HPP
#define __SLIM__COLOR__MAP__HPP
#include <unordered_map>
#include <string>
namespace slim::colors {
    const std::unordered_map<std::string, int> foreground_map{
        {"none", 0},
        {"black", 30},
        {"red", 31},
        {"green", 32},
        {"yellow", 33},
        {"blue", 34},
        {"magenta", 35},
        {"cyan", 36},
        {"white", 37},
        {"bright black", 90},
        {"bright red", 91},
        {"bright green", 92},
        {"bright yellow", 93},
        {"bright blue", 94},
        {"bright magenta", 95},
        {"bright cyan", 96},
        {"bright white", 97}
    };
    const std::unordered_map<std::string, int> background_map{
        {"none", 0},
        {"black", 40},
        {"red", 41},
        {"green", 42},
        {"yellow", 43},
        {"blue", 44},
        {"magenta", 45},
        {"cyan", 46},
        {"white", 47},
        {"bright black", 100},
        {"bright red", 101},
        {"bright green", 102},
        {"bright yellow", 103},
        {"bright blue", 104},
        {"bright magenta", 105},
        {"bright cyan", 106},
        {"bright white", 107}
    };
    const std::unordered_map<std::string, std::unordered_map<std::string, int>> color_map {
        {"foreground", foreground_map},
        {"background", background_map}
    };
    std::unordered_map<std::string, std::string> default_colors {
        {"foreground", "none"},
        {"background", "none"}
    };
    std::string_view color_oupt_on() {
        return std::string_view("\033[" + color_map["foreground"]["bright blue"] + ";" + color_map["background"]["black"] + "m");
    }
    std::string_view color_oupt_off() {
        return std::string_view("\033[0m");
    }
}
#endif