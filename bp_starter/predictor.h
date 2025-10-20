#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <iostream>
#include <cstdint>
#include <cmath>

class SaturatingCounter{
    public: 
    enum State {
        strongly_not_taken,
        weakly_not_taken,
        weakly_taken,
        strongly_taken
    };
    SaturatingCounter();
    SaturatingCounter(State);
    bool should_take();
    void taken(bool was_taken);
    State get_state();
    
    private:
    State m_state;
};

inline SaturatingCounter::SaturatingCounter(){}

inline SaturatingCounter::SaturatingCounter(SaturatingCounter::State starting_state){
    m_state = starting_state;
}

inline bool SaturatingCounter::should_take(){
    //std::cout << ((m_state == weakly_taken || m_state == strongly_taken) ? "~ Should take" : "~ Should not take") << std::endl;
    return m_state == weakly_taken || m_state == strongly_taken;
}

inline void SaturatingCounter::taken(bool was_taken){
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

inline SaturatingCounter::State SaturatingCounter::get_state(){
    return m_state;
}


class Gbasic{
    public:
    Gbasic(SaturatingCounter::State default_state, int addr_width);
    ~Gbasic();
    uint32_t get_table_address(uint32_t address);
    bool should_take(uint32_t address);
    void taken(bool was_taken, uint32_t address);
    SaturatingCounter::State get_State(uint32_t address);

    private:
    SaturatingCounter* m_counter_table;
    int m_addr_width;
};

inline Gbasic::Gbasic(SaturatingCounter::State default_state, int addr_width){
    m_addr_width = addr_width >= 32 ? 31 : addr_width;

    int table_size = 1 << m_addr_width;

    m_counter_table = new SaturatingCounter[table_size];

    for (int i = 0; i < table_size; i++){
        m_counter_table[i] = SaturatingCounter(default_state);
    }
}

inline Gbasic::~Gbasic(){
    delete[] m_counter_table;
}

inline uint32_t Gbasic::get_table_address(uint32_t address){
    uint32_t table_address = address;
    table_address &= (1U << m_addr_width) - 1;    // Mask to only keep lower n bits of table_idx
    
    return table_address;
}

inline bool Gbasic::should_take(uint32_t address){
    uint32_t table_address = get_table_address(address);

    return m_counter_table[table_address].should_take();
}

inline void Gbasic::taken(bool was_taken, uint32_t address){
    uint32_t table_address = get_table_address(address);

    m_counter_table[table_address].taken(was_taken);
}

inline SaturatingCounter::State Gbasic::get_State(uint32_t address){
    uint32_t table_address = get_table_address(address);

    return m_counter_table[table_address].get_state();
}


class Gshare{
    public:
    Gshare(SaturatingCounter::State default_state, int addr_width, uint32_t default_history);
    ~Gshare();

    bool should_take(uint32_t address);
    void taken(bool was_taken, uint32_t address);

    // For tests:
    uint32_t get_table_address(uint32_t address);

    SaturatingCounter::State get_State(uint32_t table_idx);

    uint64_t get_history();
    void set_history(uint64_t hist);

    private:
    SaturatingCounter* m_counter_table;
    int m_addr_width;
    uint64_t m_history;
};

inline Gshare::Gshare(SaturatingCounter::State default_state, int addr_width, uint32_t default_history){
    int table_size = pow(2, addr_width);
    
    m_addr_width = addr_width >= 32 ? 31 : addr_width;
    m_history = default_history;

    m_counter_table = new SaturatingCounter[table_size];

    for (int i = 0; i < table_size; i++){
        m_counter_table[i] = SaturatingCounter(default_state);
    }
}

inline Gshare::~Gshare(){
    delete[] m_counter_table;
}


inline bool Gshare::should_take(uint32_t address){
    uint32_t table_address = get_table_address(address);

    return m_counter_table[table_address].should_take();
}

inline void Gshare::taken(bool was_taken, uint32_t address){
    uint32_t table_address = get_table_address(address);

    m_counter_table[table_address].taken(was_taken);

    m_history = (m_history << 1) | was_taken;
}

inline uint32_t Gshare::get_table_address(uint32_t address){
    uint32_t table_address = address ^ m_history;
    table_address &= (1U << m_addr_width) - 1;    // Mask to only keep lower n bits of table_idx
    
    return table_address;
}

inline SaturatingCounter::State Gshare::get_State(uint32_t table_idx){
    table_idx &= (1U << m_addr_width) - 1;

    return m_counter_table[table_idx].get_state();
}

inline void Gshare::set_history(uint64_t hist) {
    m_history = hist;
}

inline uint64_t Gshare::get_history() {
    return m_history & (1U << m_addr_width) - 1;
}


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