#ifndef __SLIM__CONSOLE__COLORS__H
#define __SLIM__CONSOLE__COLORS__H
#include <unordered_map>
#include <string>
#include <vector>
namespace slim::console::colors {
    extern std::vector<std::string> colors;
    extern std::unordered_map<std::string, int> text;
    extern std::unordered_map<std::string, int> background;
};
#endif