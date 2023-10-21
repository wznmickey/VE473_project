#ifndef CONFIG_H
#define CONFIG_H
#include <string>

namespace cfg
{
    struct Config
    {
        const unsigned int width = 2560;
        const unsigned int height = 720;
        const unsigned int fps = 10;
        const std::string savepath = "/home/zjche/Desktop/473/img/";
    };
};

#endif