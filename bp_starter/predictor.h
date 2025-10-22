#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <iostream>
#include <cstdint>
#include <cmath>
#include <unordered_map>

class SaturatingCounter{
    public: 
    enum State {
        strongly_not_taken,
        weakly_not_taken,
        weakly_taken,
        strongly_taken
    };
    SaturatingCounter(){}

    SaturatingCounter(State starting_state){
        m_state = starting_state;
    }

    bool should_take(){
        //std::cout << ((m_state == weakly_taken || m_state == strongly_taken) ? "~ Should take" : "~ Should not take") << std::endl;
        return m_state == weakly_taken || m_state == strongly_taken;
    }
    void taken(bool was_taken){
        //std::cout << (was_taken ? "* Take" : "* Not Taken") << std::endl;

        switch (m_state){
        case strongly_not_taken:
            m_state = was_taken ? weakly_not_taken : strongly_not_taken;
            break;
        case weakly_not_taken:
            m_state = was_taken ? weakly_taken : strongly_not_taken;
            break;
        case weakly_taken:
            m_state = was_taken ? strongly_taken : weakly_not_taken;
            break;
        case strongly_taken:
            m_state = was_taken ? strongly_taken : weakly_taken;
            break;
        }
    }
    
    State get_state(){
        return m_state;
    }
    
    private:
    State m_state;
};

class Gbasic{
    public:
    Gbasic(SaturatingCounter::State default_state, int addr_width){
        m_addr_width = addr_width >= 32 ? 31 : addr_width;

        int table_size = 1 << m_addr_width;

        m_counter_table = new SaturatingCounter[table_size];

        for (int i = 0; i < table_size; i++){
            m_counter_table[i] = SaturatingCounter(default_state);
        }
    }
    
    ~Gbasic(){
        delete[] m_counter_table;
    }

    uint32_t get_table_address(uint32_t address){
        uint32_t table_address = address;
        table_address &= (1U << m_addr_width) - 1;    // Mask to only keep lower n bits of table_idx
        
        return table_address;
    }

    bool should_take(uint32_t address){
        uint32_t table_address = get_table_address(address);

        return m_counter_table[table_address].should_take();
    }

    void taken(bool was_taken, uint32_t address){
        uint32_t table_address = get_table_address(address);

        m_counter_table[table_address].taken(was_taken);
    }
    
    SaturatingCounter::State get_State(uint32_t address){
        uint32_t table_address = get_table_address(address);

        return m_counter_table[table_address].get_state();
    }

    private:
    SaturatingCounter* m_counter_table;
    int m_addr_width;
};

class Gselect{
    public:
    Gselect(SaturatingCounter::State default_state, int addr_width, int history_width, uint32_t default_history){
        int table_size = pow(2, addr_width);
        
        m_addr_width = addr_width >= 32 ? 31 : addr_width;
        m_history = default_history;
        m_history_width = history_width;
        m_history &= (1U << m_history_width) - 1;

        m_counter_table = new SaturatingCounter[table_size];

        for (int i = 0; i < table_size; i++){
            m_counter_table[i] = SaturatingCounter(default_state);
        }
    }

    ~Gselect(){
        delete[] m_counter_table;
    }

    bool should_take(uint32_t address, bool was_taken){
        uint32_t table_address = get_table_address(address, m_brh_fetch);

        m_brh_fetch = (m_brh_fetch << 1) | was_taken;

        return m_counter_table[table_address].should_take();
    }

    void taken(bool was_taken, uint32_t address){
        uint32_t table_address = get_table_address(address, m_brh_retire);

        m_counter_table[table_address].taken(was_taken);

        m_brh_retire = (m_brh_retire << 1) | was_taken;
    }

    // For tests:
    uint32_t get_table_address(uint32_t address, uint32_t history){
        
        uint32_t table_address = (address << m_history_width) | history;
        table_address &= (1U << m_addr_width) - 1;    // Mask to only keep lower n bits of table_idx
        
        return table_address;
    }

    SaturatingCounter::State get_State(uint32_t table_idx){
        table_idx &= (1U << m_addr_width) - 1;

        return m_counter_table[table_idx].get_state();
    }

    uint64_t get_history(){
        return m_history & ((1U << m_addr_width) - 1);
    }
    
    void set_history(uint64_t hist){
        m_history = hist;
    }

    private:
    SaturatingCounter* m_counter_table;
    int m_addr_width;
    int m_history_width;
    uint64_t m_history;
    uint32_t m_brh_fetch;
    uint32_t m_brh_retire;
};

class Gshare{
    public:
    Gshare(SaturatingCounter::State default_state, int addr_width, int history_width, uint32_t default_history){
        int table_size = pow(2, addr_width);
        
        m_addr_width = addr_width >= 32 ? 31 : addr_width;
        m_history = default_history;
        m_history_width = history_width;
        m_history &= (1U << m_history_width) - 1;

        m_counter_table = new SaturatingCounter[table_size];

        for (int i = 0; i < table_size; i++){
            m_counter_table[i] = SaturatingCounter(default_state);
        }
    }

    ~Gshare(){
        delete[] m_counter_table;
    }

    bool should_take(uint32_t address, bool was_taken){
        uint32_t table_address = get_table_address(address, m_brh_fetch);

        m_brh_fetch = (m_brh_fetch << 1) | was_taken;

        return m_counter_table[table_address].should_take();
    }

    void taken(bool was_taken, uint32_t address){
        uint32_t table_address = get_table_address(address, m_brh_retire);

        m_counter_table[table_address].taken(was_taken);

        m_brh_retire = (m_brh_retire << 1) | was_taken;
    }

    // For tests:
    uint32_t get_table_address(uint32_t address, uint32_t history){
        uint32_t table_address = address ^ history;
        table_address &= (1U << m_addr_width) - 1;    // Mask to only keep lower n bits of table_idx
        
        return table_address;
    }

    SaturatingCounter::State get_State(uint32_t table_idx){
        table_idx &= (1U << m_addr_width) - 1;

        return m_counter_table[table_idx].get_state();
    }

    uint64_t get_history(){
        return m_history & ((1U << m_addr_width) - 1);
    }
    
    void set_history(uint64_t hist){
        m_history = hist;
    }

    private:
    SaturatingCounter* m_counter_table;
    int m_addr_width;
    int m_history_width;
    uint64_t m_history;
    uint32_t m_brh_fetch;
    uint32_t m_brh_retire;
};

inline std::ostream& operator<<(std::ostream& os, SaturatingCounter::State state) {
    switch(state) {
        case SaturatingCounter::strongly_not_taken: os << "strongly_not_taken";   break;
        case SaturatingCounter::strongly_taken:     os << "strongly_taken"; break;
        case SaturatingCounter::weakly_taken:       os << "weakly_taken"; break;
        case SaturatingCounter::weakly_not_taken:   os << "weakly_not_taken"; break;
        default: os.setstate(std::ios_base::failbit);
    }
    return os;
}   

#endif