#include <cstdint>

#define powerOfTwo(n) ((uint64_t)1 << (n))

constexpr uint16_t swapBits(uint16_t n)
{
    return ((n) << 8) | ((n) >> 8);
}

// returns broadcast address or 0 if input parameters are invalid
constexpr uint32_t getBroadcastAddress(uint32_t ip, uint32_t mask)
{
    // Address 0.0.0.0 is an address of invalid node
    return (ip == 0) ? 0 : ip | ~mask;
}