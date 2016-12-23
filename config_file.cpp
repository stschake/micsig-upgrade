#include "config_file.h"
#include "aes256.h"

#include <fstream>
#include <exception>
#include <cstdlib>

static const uint8_t encryption_key[] =
 {0x19, 0x05, 0x14, 0x20, 0x11, 0x27, 0x27, 0x11,
  0x06, 0x05, 0x13, 0x20, 0x20, 0x03, 0x84, 0x19,
  0x09, 0x02, 0x89, 0x19, 0x27, 0x03, 0x12, 0x20,
  0x16, 0x05, 0x12, 0x20, 0x31, 0x09, 0x12, 0x20};

constexpr int config_length = 20888;
constexpr int blocks = config_length / 16;
constexpr int remainder_bytes = config_length % 16;

static uint16_t calculate_crc(const uint8_t* input, const int length)
{
    const uint8_t* end = input + length;
    uint32_t v2 = 0xFF;
    uint32_t v3 = 0xFF;
    do {
        uint32_t b = *input;
        input++;
        v3 = v3 ^ b;
        for (int i = 0; i < 8; i++)
        {
            uint32_t v7 = v2 >> 1;
            uint32_t v8 = (v3 >> 1) & 0xFF;
            if (v2 & 1)
                v8 |= 0x80;
            uint32_t v9 = (v3 & 1) == 0;
            v2 >>= 1;
            v3 = v8;
            if (!v9) {
                v2 = v7 ^ 0xA0;
                v3 = v8 ^ 1;
            }

        }
    } while (input != end);
    return (int16_t)((v2 << 8) | v3);
}

config_file::config_file(const char* path)
{
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    auto size = (int32_t) input.tellg();
    if (input.bad() || size != config_length)
        throw std::exception("Invalid config file");
    input.seekg(0, std::ios::beg);
    data = std::make_unique<uint8_t[]>(size);
    input.read((char*)data.get(), size);

    aes256_context aes;
    aes256_init(&aes, encryption_key);
    for (int i = 0; i < blocks; i++)
        aes256_decrypt_ecb(&aes, data.get() + (i * 16));
    memset(data.get() + (blocks * 16), 0, remainder_bytes);

    // at this point, data contains the decrypted config file
    auto our_crc = calculate_crc(data.get() + 2, config_length - 2);
    auto expected_crc = (uint32_t)data.get()[0] | ((uint32_t)data.get()[1] << 8);
    if (our_crc != expected_crc)
        throw std::exception("Current config file failed CRC check");
}

void config_file::write(const char* path)
{
    auto crc = calculate_crc(data.get() + 2, config_length - 2);
    data.get()[0] = (uint8_t)(crc & 0xFF);
    data.get()[1] = (uint8_t)(crc >> 8);

    auto encrypted_data = std::make_unique<uint8_t[]>(config_length);
    memcpy(encrypted_data.get(), data.get(), config_length);
    aes256_context aes;
    aes256_init(&aes, encryption_key);
    for (int i = 0; i < blocks; i++)
        aes256_encrypt_ecb(&aes, encrypted_data.get() + (i * 16));
    memset(encrypted_data.get() + (blocks * 16), 0, remainder_bytes);

    std::ofstream out(path, std::ios::binary | std::ios::out | std::ios::trunc);
    out.write((char*)encrypted_data.get(), config_length);
    out.flush();
}

constexpr int bandwidth_offset = 0x1194;
int8_t config_file::get_bandwidth() const
{
    return data.get()[bandwidth_offset];
}
void config_file::set_bandwidth(int8_t value)
{
    data.get()[bandwidth_offset] = value;
}

constexpr int flags_offset = 0x0114;
uint32_t config_file::get_flags() const
{
    uint32_t* p = (uint32_t*)(data.get() + flags_offset);
    return *p;
}
void config_file::set_flags(uint32_t value)
{
    uint32_t* p = (uint32_t*)(data.get() + flags_offset);
    *p = value;
}

bool config_file::has_flag(const config_flag flag) const
{
    return (get_flags() & ((uint32_t) flag)) > 0;
}
void config_file::set_flag(const config_flag flag, bool state)
{
    if (state)
        set_flags(get_flags() | ((uint32_t)flag));
    else
        set_flags(get_flags() & ~((uint32_t)flag));
}