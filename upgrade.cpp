#include <exception>
#include <iostream>
#include <vector>
#include <utility>

#include "config_file.h"

int main()
{
    try
    {
        config_file file("mf174834.dat");
        if (file.get_bandwidth() != 100) {
            file.set_bandwidth(100);
            std::cout << "Upgraded bandwidth to 100MHz" << std::endl;
        }

        if (file.get_memory_depth() != memory_depth::DEPTH_28M) {
            file.set_memory_depth(memory_depth::DEPTH_28M);
            std::cout << "Upgraded memory depth to 28M" << std::endl;
        }

        const std::vector<std::pair<config_flag,const char*>> options = {
            { config_flag::HAS_HDMI, "HDMI" },
            { config_flag::HAS_500_uV, "500uV" },
            { config_flag::HAS_WLAN, "WLAN" },
            { config_flag::HAS_FREQUENCY_COUNTER, "Frequency Counter" },
            { config_flag::HAS_AUTO_V, "AutoV" }
        };
        for each (auto& pair in options) {
            if (!file.has_flag(pair.first)) {
                file.set_flag(pair.first, true);
                std::cout << "Enabled " << pair.second << std::endl;
            }
        }

        file.write("mf174834.dat");
        std::cout << "Upgrade finished!" << std::endl;
    }
    catch (std::exception& ex)
    {
        std::cout << "Upgrade failed: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}