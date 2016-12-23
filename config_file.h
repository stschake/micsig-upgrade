#pragma once

#include <cstdint>
#include <cstdbool>
#include <memory>

enum class config_flag : uint32_t
{
    HAS_FREQUENCY_COUNTER = 1 << 20,
    HAS_HDMI = 1 << 21,
    HAS_500_uV = 1 << 22,
    HAS_AUTO_V = 1 << 23,    
    HAS_WLAN = 1 << 28,
};

class config_file
{
    private:
    std::unique_ptr<uint8_t[]> data;

    public:
    config_file(const char* path);

    int8_t get_bandwidth() const;
    void set_bandwidth(int8_t value);

    uint32_t get_flags() const;
    void set_flags(uint32_t value);

    bool has_flag(const config_flag flag) const;
    void set_flag(const config_flag flag, bool state);

    void write(const char* path);
};


