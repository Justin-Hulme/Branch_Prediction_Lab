#define tests
#include "../predictor.h"
#include <iostream>
#include <bitset>

#define addr_1 0xFF'FF'FF'FF
#define addr_2 0xF0'F0'F0'FF
#define addr_3 0xFF'FF'FF'FF

Gbasic* g_basic;

int main(){
    // for (int i = 0; i < 32; i ++){
    //     uint32_t table_address = i;
    //     table_address &= (1U << 4) - 1;

    //     std::cout 
    //         << std::bitset<32>(i) 
    //         << " | "
    //         << std::bitset<32>(table_address) 
    //         << std::endl;

    //     std::cout << std::flush;
    // }
    // std::cout << std::endl;

    g_basic = new Gbasic(SaturatingCounter::weakly_taken, 4);
    std::cout << "test" << std::endl << std::flush;

    // test get_table_address
    for (uint32_t i = 0; i < 32; i ++){
        std::cout 
            << std::bitset<32>(i) 
            << " | "
            << std::bitset<32>(g_basic->get_table_address(i)) 
            << std::endl
            <<std::flush;
    }
    std::cout << std::endl;

    // test same address
    // std::cout << g_basic->get_State(addr_1) << std::endl;
    // g_basic->taken(true, addr_1)

    // test address with same lower 4

    // test address with different lower 4
}