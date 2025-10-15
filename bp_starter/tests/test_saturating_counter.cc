#include <iostream>
#include "predictor.h"

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