#define tests
#include "../predictor.h"
#include <iostream>
#include <iomanip>
#include <bitset>
#include <cmath>

#define ADDR_WIDTH 4

Gshare* g_share;

void print_states() {
    for (int i = 0; i < pow(2,4); i++) {
        std::cout << std::hex << i << ": " << g_share->get_State(i) << std::endl;
    }
    std::cout << std::endl;
}

int main(){

    g_share = new Gshare(SaturatingCounter::weakly_not_taken, ADDR_WIDTH, 0);

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

    print_states();
    
    g_share->taken(false, 0x0);
    
    print_states();


    return 0;
}