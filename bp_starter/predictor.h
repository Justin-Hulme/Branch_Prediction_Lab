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
    uint32_t get_table_address(uint32_t address, uint32_t history_register);
    bool should_take(uint32_t address, uint64_t uop_id);
    void taken(bool was_taken, uint32_t address, uint64_t uop_id);
    void update_brh_fetch(bool mispredicted);

    private:
    SaturatingCounter* m_counter_table;
    int m_addr_width;
    uint32_t m_brh_fetch;
    uint32_t m_brh_retire;

    struct BranchMetaData{
        uint32_t brh_fetch;
        bool prediction;
    };

    std::unordered_map<uint64_t, BranchMetaData> m_branch_prediction_map;
    long long m_misprediction_count;
};

inline Gshare::Gshare(SaturatingCounter::State default_state, int addr_width, uint32_t default_history){
    int table_size = pow(2, addr_width);
    
    m_addr_width = addr_width >= 32 ? 31 : addr_width;
    // m_history = default_history;

    m_counter_table = new SaturatingCounter[table_size];

    for (int i = 0; i < table_size; i++){
        m_counter_table[i] = SaturatingCounter(default_state);
    }

    m_misprediction_count = 0;
}

inline Gshare::~Gshare(){
    std::cout << "misprediction count: " << m_misprediction_count << std::endl;
    delete[] m_counter_table;
}

inline uint32_t Gshare::get_table_address(uint32_t address, uint32_t history_register){
    uint32_t table_address = address ^ history_register;
    table_address &= (1U << m_addr_width) - 1;   // Mask to only keep lower n bits of table_idx
    
    return table_address;
}

inline bool Gshare::should_take(uint32_t address, uint64_t uop_id){
    uint32_t fetch_snapshot = m_brh_fetch;

    uint32_t table_address = get_table_address(address, fetch_snapshot);
    bool prediction = m_counter_table[table_address].should_take();

    
    m_branch_prediction_map[uop_id] = {m_brh_fetch, prediction};
    
    m_brh_fetch = (m_brh_fetch << 1) | prediction;

    return prediction;
}

inline void Gshare::taken(bool was_taken, uint32_t address, uint64_t uop_id){
    auto it = m_branch_prediction_map.find(uop_id);
    if (it != m_branch_prediction_map.end()){
        BranchMetaData meta_data = it->second;

        uint32_t table_address = get_table_address(address, meta_data.brh_fetch);
        m_counter_table[table_address].taken(was_taken);

        m_brh_retire = (m_brh_retire << 1) | was_taken;

        if (meta_data.prediction != was_taken){
            m_misprediction_count ++;
        }

        m_branch_prediction_map.erase(uop_id);
    }
}

inline void Gshare::update_brh_fetch(bool mispredicted){
    if (mispredicted){
        m_brh_fetch = m_brh_retire;
    }
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