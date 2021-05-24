#include <iostream>
#include <random>
#include <cstdint>
#include <cstring>
#include <iomanip>

#include "nc/source/time/NcTime.h"

using namespace nc;

constexpr auto ComponentSize = 64u;
constexpr auto ComponentCount = 150000u;


void Fill(uint8_t* data, size_t size)
{
    uint8_t v = 0u;

    for(size_t i = 0u; i < size; ++i)
    {
        data[i] = ++v;
    }
}

void Print(uint8_t* data, size_t size)
{
    for(size_t i = 0; i < size; ++i)
        std::cout << data[i];
    
    std::cout << "\n\n";
}

int main()
{
    size_t size = ComponentSize * ComponentCount;
    uint8_t* p1 = new uint8_t[size];
    Fill(p1, size);
    uint8_t* p2 = new uint8_t[size];

    time::Timer timer;
    timer.Start();

    std::memcpy(p2, p1, size);

    timer.Stop();



    std::cout << "Time: " << std::fixed << timer.Value() / 1000000.0f << '\n';


    return 0;
}