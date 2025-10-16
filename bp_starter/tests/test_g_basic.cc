#define tests
#include "../predictor.h"
#include <iostream>
#include <bitset>

#define addr_1 (0xFFFFFFFF)
#define addr_2 (0xF0F0F0FF)
#define addr_3 (0xFFFFFFFE)

Gbasic* g_basic;

void print_states() {
    std::cout << "Addr_1: " << g_basic->get_State(addr_1) << std::endl;
    std::cout << "Addr_2: " << g_basic->get_State(addr_2) << std::endl;
    std::cout << "Addr_3: " << g_basic->get_State(addr_3) << std::endl;
    std::cout << std::endl;
}

int main(){

    g_basic = new Gbasic(SaturatingCounter::weakly_taken, 4);

    /*
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
    */

    // Print tables addresses
    std::cout << "addr_1 table address: " <<g_basic->get_table_address(addr_1) << std::endl;
    std::cout << "addr_2 table address: " <<g_basic->get_table_address(addr_2) << std::endl;
    std::cout << "addr_3 table address: " <<g_basic->get_table_address(addr_3) << std::endl;
    std::cout << std::endl;

    // test all 3 addresses:

    print_states(); // addr_1: weakly_taken, addr_2: weakly_taken, addr_3: weakly_taken
    
    g_basic->taken(true, addr_1);
    
    print_states(); // addr_1: strongly_taken, addr_2: strongly_taken, addr_3: weakly_taken

    g_basic->taken(false, addr_2);
    
    print_states(); // addr_1: weakly_taken, addr_2: weakly_taken, addr_3: weakly_taken

    g_basic->taken(false, addr_2);
    
    print_states(); // addr_1: weakly_not_taken, addr_2: weakly_not_taken, addr_3: weakly_taken

    g_basic->taken(true, addr_3);
    
    print_states(); // addr_1: weakly_not_taken, addr_2: weakly_not_taken, addr_3: strongly_taken

    return 0;
}