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

/* This is from the config library, it's unclear what the physical configuration
   is for each model. Safe options seem to be 14M and 28M. */
enum class memory_depth : uint32_t
{
    DEPTH_3DOT6M = 1,
    DEPTH_18M = 2,
    DEPTH_90M = 3,
    DEPTH_14M = 4,
    DEPTH_28M = 5
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

    memory_depth get_memory_depth() const;
    void set_memory_depth(const memory_depth depth);

    bool has_flag(const config_flag flag) const;
    void set_flag(const config_flag flag, bool state);

    void write(const char* path);
};


