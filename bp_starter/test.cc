#include <iostream>

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

SaturatingCounter::SaturatingCounter(){
    SaturatingCounter(default_counter_state);
}

SaturatingCounter::SaturatingCounter(SaturatingCounter::State starting_state){
    m_state = starting_state;
}

bool SaturatingCounter::should_take(){
    return m_state == weakly_taken || m_state == strongly_taken;
}

void SaturatingCounter::taken(bool was_taken){
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

    std::cout << std::endl << (was_taken ? "Path Taken" : "Path not Taken") << std::endl;
}

SaturatingCounter::State SaturatingCounter::get_state(){
    return m_state;
}

std::ostream& operator<<(std::ostream& os, SaturatingCounter::State state) {
    switch(state) {
        case SaturatingCounter::strongly_not_taken: os << "strongly_not_taken";   break;
        case SaturatingCounter::strongly_taken:     os << "strongly_taken"; break;
        case SaturatingCounter::weakly_taken:       os << "weakly_taken"; break;
        case SaturatingCounter::weakly_not_taken:   os << "weakly_not_taken"; break;
        default: os.setstate(std::ios_base::failbit);
    }
    return os;
}   

int main(){
    SaturatingCounter sat_count(SaturatingCounter::weakly_taken);

    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;

    sat_count.taken(true);
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;

    sat_count.taken(true);
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;


    sat_count.taken(false);
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;


    sat_count.taken(false);
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;


    sat_count.taken(false);
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;


    sat_count.taken(false);
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;


    sat_count.taken(true);
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;

    sat_count.taken(true);
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;

    sat_count.taken(true);
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;

    sat_count.taken(true);
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
    std::cout << sat_count.should_take() << " " << sat_count.get_state() <<std::endl;
}